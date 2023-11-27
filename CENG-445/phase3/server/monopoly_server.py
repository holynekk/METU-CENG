from threading import Thread, Lock, Condition
import socket
import json
import sys
import copy
import json

from constants import *
from models import Board, User, Player


SERVER = "127.0.0.1"
PORT = None
ADDR = None


class GameMonitor:
    def __init__(self):
        self.connected_users = {}
        self.boards = {}
        self.user_left_session = {}
        self.buf = []
        # General purpose lock variables
        self.lock = Lock()
        self.newmess = Condition(self.lock)

    def new_command(self, mess, peer):
        """
        This is the method where commands are processed by server.
        Works for both general and board specific commands.
        """
        try:
            self.lock.acquire()
            user_text = ""
            user_obj = self.connected_users.get(peer, None)
            if user_obj is None:
                user_text = "Unknown User"
            else:
                user_text = (
                    user_obj.username
                    if isinstance(user_obj, User)
                    else user_obj.user.username
                )
            new_command = mess.decode().strip("\n").strip(" ")
            mess = {}
            if new_command == "get user state":
                if isinstance(self.connected_users.get(peer, None), Player):
                    mess = self.connected_users.get(peer).get_user_state()
                elif isinstance(self.connected_users.get(peer, None), User):
                    mess = self.connected_users.get(peer).get()
                else:
                    mess = "Players that are not authorize cannot get their user state!"
            elif new_command == "get all boards":
                mess["status"] = 200
                mess["message"] = "Listed all boards!"
                mess["boards"] = [
                    self.boards[board_id].getboardinfo() for board_id in self.boards
                ]
            elif new_command.startswith("get board state"):
                username = new_command.split(" ")[3]
                mess.update(self.connected_users.get(username).board.getboardstate())
            elif new_command.startswith("login"):
                new_command = new_command.split(" ")

                if len(new_command) == 3:
                    user__name = new_command[1]
                    password = new_command[2]
                    user = any(
                        filter(
                            lambda x: True if x == new_command[1] else False,
                            self.connected_users,
                        )
                    )
                    if not user:
                        mess["status"] = 400
                        mess[
                            "message"
                        ] = f"User with username {user__name} could not found. Try again!"
                    else:
                        # TODO: DO field check for auth here!!!!
                        user = self.connected_users[user__name]
                        if user.passwd == password:
                            mess["status"] = 200
                            mess[
                                "message"
                            ] = f"You are logged in successfully as {user__name}!"
                            mess["boards"] = [
                                board.getboardinfo() for board in self.boards
                            ]
                        else:
                            mess["status"] = 400
                            mess["message"] = f"Your password is wrong!"
                else:
                    mess["status"] = 400
                    mess["message"] = f"Fill in all the blanks!"

            elif new_command.startswith("new user"):
                new_command = new_command.split(" ")
                if len(new_command) >= 5:
                    user = User(
                        new_command[2],
                        new_command[3],
                        " ".join(new_command[5:]),
                        new_command[4],
                    )
                    self.connected_users[user.username] = user
                    mess["status"] = 200
                    mess[
                        "message"
                    ] = f"User with username '{user.username}' has been created!"
                    mess["boards"] = [
                        self.boards[board_id].getboardinfo() for board_id in self.boards
                    ]
                else:
                    mess["status"] = 400
                    mess["message"] = f"Fill in all the blanks!"
            elif new_command.startswith("new board"):
                new_command = new_command.split(" ")
                if len(new_command) <= 2:
                    mess["status"] = 400
                    mess["message"] = f"Board name cannot be empty!"

                else:
                    input_location = "sample1.json"
                    with open(input_location) as f:
                        input_ = json.loads(f.read())

                    board_instance = Board(**input_, name=new_command[2:])
                    self.boards[board_instance.id] = board_instance
                    mess["status"] = 200
                    mess[
                        "message"
                    ] = f"New board created with the id: {board_instance.id} - name: {' '.join(board_instance.name)}!"

                mess["boards"] = [
                    self.boards[board_id].getboardinfo() for board_id in self.boards
                ]
            elif new_command == "list users":
                result = copy.deepcopy(self.connected_users)
                for user in self.connected_users:
                    result[user] = str(self.connected_users[user])
                mess = f"{user_text} listed users: " + str(result)
            elif new_command == "list boards":
                result = {}
                for board in self.boards:
                    result[board] = str(self.boards[board])
                mess = f"{user_text} listed boards: " + str(result)
            elif new_command.startswith("open"):
                # try:
                new_command = new_command.split(" ")
                board = self.boards.get(int(new_command[1]), None)
                player = board.attach(self.connected_users[new_command[2]], None, None)
                self.connected_users[new_command[2]] = player
                mess.update(
                    self.connected_users.get(player.user.username).board.getboardstate()
                )
                mess = json.dumps(mess)
                self.buf.append(mess)
                self.newmess.notify_all()
                self.lock.release()
                return None
                # except:
                #     mess = "Something went wrong!"
            elif new_command.startswith("close"):
                new_command = new_command.split(" ")
                user__name = new_command[1]
                user_obj = self.connected_users[user__name]
                board = user_obj.board
                mess["message"] = board.detach(user_obj)
                mess["boards"] = [
                    self.boards[board_id].getboardinfo() for board_id in self.boards
                ]
                mess = json.dumps(mess)
                self.connected_users[user__name] = user_obj.user
                self.user_left_session[peer] = board.id
                self.buf.append(mess)
                self.newmess.notify_all()
                self.lock.release()
                return None
            elif new_command.startswith("ready"):
                new_command = new_command.split(" ")
                player = self.connected_users[new_command[1]]
                player.board.ready(player)
                mess.update(
                    self.connected_users.get(player.user.username).board.getboardstate()
                )
                mess["message"] = f"User {player.user.username} is ready!" + (
                    "\nThe game is starting!" if not player.board.is_game_over else ""
                )
            elif new_command.startswith("command"):
                new_command = new_command.split(" ")
                player = self.connected_users[new_command[2]]
                mess.update(
                    self.connected_users.get(player.user.username).board.getboardstate()
                )
                if new_command[1] == "pass":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_PASS][1]
                    )
                elif new_command[1] == "bail":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_BAIL][1]
                    )
                elif new_command[1] == "roll":
                    mess["message"] = player.board.turn(
                        player,
                        COMMAND_CHOICES[COMMAND_ROLL][1],
                    )
                elif new_command[1] == "buy":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_BUY][1]
                    )
                elif new_command[1] == "upgrade":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_UPGRADE][1]
                    )
                elif new_command[1] == "downgrade":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_DOWNGRADE][1]
                    )
                elif new_command[1] == "pick":
                    mess["message"] = player.board.turn(
                        player,
                        COMMAND_CHOICES[COMMAND_PICK][1],
                        {"picked_location": " ".join(new_command[3:])},
                    )
                elif new_command[1] == "teleport":
                    mess["message"] = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_TELEPORT][1]
                    )
                else:
                    mess["message"] = "Unknown command!"

            else:
                mess["message"] = "Unknown command!"
        except Exception as e:
            mess["message"] = "Something happened. Please try again!"
            print(new_command)
            print(e)
        mess = json.dumps(mess)
        self.buf.append(mess)
        self.newmess.notify_all()
        self.lock.release()

    def getmessages(self):
        self.lock.acquire()
        if not self.buf:
            a = []
        else:
            a = self.buf[-1:]
        self.lock.release()
        return a


class RecieveAgent(Thread):
    def __init__(self, conn, addr, game):
        self.game = game
        self.conn = conn
        self.claddr = addr
        Thread.__init__(self)

    def run(self):
        inp = self.conn.recv(1024)
        while inp:
            self.game.new_command(inp, self.claddr[1])
            inp = self.conn.recv(1024)
        print(f"Client ({self.claddr[1]}) is terminating!")
        self.conn.close()


class SendAgent(Thread):
    def __init__(self, conn, addr, game):
        self.conn = conn
        self.claddr = addr
        self.game = game
        self.current = 0
        Thread.__init__(self)

    def run(self):
        oldmess = self.game.getmessages()
        self.current += len(oldmess)
        self.conn.send("\n".join([i for i in oldmess]).encode())
        notexit = True
        while notexit:
            self.game.lock.acquire()
            self.game.newmess.wait()
            self.game.lock.release()
            oldmess = self.game.getmessages()
            self.current += len(oldmess)
            try:
                user_text = ""
                user_obj = self.game.connected_users.get(self.claddr[1], None)
                if user_obj is None:
                    user_text = "Unknown User"
                else:
                    user_text = (
                        user_obj.username
                        if isinstance(user_obj, User)
                        else user_obj.user.username
                    )
                self.conn.send(
                    ("\n" + "\n".join([i for i in oldmess]) + user_text + "> ").encode()
                )
            except Exception as e:
                print(e)
                notexit = False
        self.conn.close()


def main():
    global PORT, ADDR
    PORT = int(sys.argv[2])
    ADDR = (SERVER, PORT)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(ADDR)
    s.listen(16)
    game_monitor = GameMonitor()

    while True:
        ns, peer = s.accept()
        print("Connection established by: ", peer)
        ra = RecieveAgent(ns, peer, game_monitor)
        sa = SendAgent(ns, peer, game_monitor)
        ra.start()
        sa.start()


if __name__ == "__main__":
    main()
