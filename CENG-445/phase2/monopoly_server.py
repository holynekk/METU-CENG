from threading import Thread, Lock, Condition
import socket
import json
import sys
import copy

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
            if isinstance(self.connected_users.get(peer, None), Player):
                self.boards[self.connected_users[peer].board.id]["condition"].acquire()
            else:
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
            if new_command == "get user state":
                if isinstance(self.connected_users.get(peer, None), Player):
                    mess = self.connected_users.get(peer).get_user_state()
                elif isinstance(self.connected_users.get(peer, None), User):
                    mess = self.connected_users.get(peer).get()
                else:
                    mess = "Players that are not authorize cannot get their user state!"
            elif new_command == "get board state":
                if isinstance(self.connected_users.get(peer, None), Player):
                    mess = self.connected_users.get(peer).board.getboardstate()
                else:
                    mess = (
                        "Players who are not joined any boards cannot get board state!"
                    )
            elif new_command.startswith("new user"):
                new_command = new_command.split(" ")
                user = User(
                    new_command[2],
                    new_command[3],
                    " ".join(new_command[5:]),
                    new_command[4],
                )
                self.connected_users[peer] = user
                mess = f"User with username '{user.username}' has been created!"
            elif new_command == "new board":
                input_location = "sample1.json"
                with open(input_location) as f:
                    input_ = json.loads(f.read())

                board_instance = Board(**input_)
                lock = Lock()
                condition = Condition(lock)
                self.boards[board_instance.id] = {
                    "board": board_instance,
                    "lock": lock,
                    "condition": condition,
                }
                mess = f"New board created with the id - {board_instance.id}!"
            elif new_command == "list users":
                result = copy.deepcopy(self.connected_users)
                for user in self.connected_users:
                    result[user] = str(self.connected_users[user])
                mess = f"{user_text} listed users: " + str(result)
            elif new_command == "list boards":
                result = {}
                for board in self.boards:
                    result[board] = str(self.boards[board]["board"])
                mess = f"{user_text} listed boards: " + str(result)
            elif new_command.startswith("open"):
                try:
                    new_command = new_command.split(" ")
                    board = self.boards.get(int(new_command[1]), None)
                    if not board:
                        mess = f"There is no board with id {int(new_command[1])}"
                    elif not isinstance(self.connected_users.get(peer, None), User):
                        mess = f"To open a board, you have to create a user first!"
                    else:
                        player = board["board"].attach(
                            self.connected_users[peer], None, None
                        )
                        self.connected_users[peer] = player
                        mess = f"User {player.user.username} is connected to the board {player.board.id}"

                    mess = (str(mess) + "\n").encode()
                    self.buf.append(mess)
                    self.newmess.notify_all()
                    self.lock.release()
                    return None
                except:
                    mess = "Something went wrong!"
            elif new_command == "close":
                board = self.connected_users[peer].board
                board_lock = self.boards[board.id]["lock"]
                board_condition = self.boards[board.id]["condition"]
                mess = board.detach(user_obj)
                self.connected_users[peer] = user_obj.user
                self.user_left_session[peer] = board.id
                mess = (str(mess) + "\n").encode()
                self.buf.append(mess)
                board_condition.notify_all()
                board_lock.release()
                return None
            elif new_command == "ready":
                player = self.connected_users[peer]
                player.board.ready(player)
                mess = f"User {player.user.username} is ready!" + (
                    "\nThe game is starting!" if not player.board.is_game_over else ""
                )
            elif new_command.startswith("command"):
                player = self.connected_users[peer]
                new_command = new_command.split(" ")

                if new_command[1] == "pass":
                    mess = player.board.turn(player, COMMAND_CHOICES[COMMAND_PASS][1])
                elif new_command[1] == "bail":
                    mess = player.board.turn(player, COMMAND_CHOICES[COMMAND_BAIL][1])
                elif new_command[1] == "roll":
                    mess = player.board.turn(
                        player,
                        COMMAND_CHOICES[COMMAND_ROLL][1],
                    )
                elif new_command[1] == "buy":
                    mess = player.board.turn(player, COMMAND_CHOICES[COMMAND_BUY][1])
                elif new_command[1] == "upgrade":
                    mess = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_UPGRADE][1]
                    )
                elif new_command[1] == "downgrade":
                    mess = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_DOWNGRADE][1]
                    )
                elif new_command[1] == "pick":
                    mess = player.board.turn(
                        player,
                        COMMAND_CHOICES[COMMAND_PICK][1],
                        {"picked_location": " ".join(new_command[2:])},
                    )
                elif new_command[1] == "teleport":
                    mess = player.board.turn(
                        player, COMMAND_CHOICES[COMMAND_TELEPORT][1]
                    )

                elif new_command[1] == "get_board_state":
                    pass
                else:
                    mess = "Unknown command!"

            else:
                mess = "Unknown command!"
        except:
            mess = "Something happened. Please try again!"
        mess = (str(mess) + "\n").encode()
        self.buf.append(mess)
        if isinstance(self.connected_users.get(peer, None), Player):
            self.boards[self.connected_users[peer].board.id]["condition"].notify_all()
        else:
            self.newmess.notify_all()

        if isinstance(self.connected_users.get(peer, None), Player):
            self.boards[self.connected_users[peer].board.id]["condition"].release()
        else:
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
        self.conn.send("\n".join([i.decode() for i in oldmess]).encode())
        notexit = True
        while notexit:
            if isinstance(self.game.connected_users.get(self.claddr[1], None), Player):
                self.game.boards[self.game.connected_users[self.claddr[1]].board.id][
                    "lock"
                ].acquire()
            else:
                self.game.lock.acquire()
            old = isinstance(
                self.game.connected_users.get(self.claddr[1], None), Player
            )
            if isinstance(self.game.connected_users.get(self.claddr[1], None), Player):
                self.game.boards[self.game.connected_users[self.claddr[1]].board.id][
                    "condition"
                ].wait()
            else:
                self.game.newmess.wait()
            new = isinstance(
                self.game.connected_users.get(self.claddr[1], None), Player
            )
            if (old and new) or (old and not new):
                close_board = self.game.user_left_session.get(self.claddr[1], None)
                if type(close_board) == int:
                    self.game.boards[close_board]["lock"].release()
                else:
                    self.game.boards[
                        self.game.connected_users[self.claddr[1]].board.id
                    ]["lock"].release()
            else:
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
                    (
                        "\n"
                        + "\n".join([i.decode() for i in oldmess])
                        + user_text
                        + "> "
                    ).encode()
                )
            except Exception as e:
                print(e)
                notexit = False


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
