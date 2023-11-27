from threading import Thread, Lock, Condition
import socket
import json
import sys
import copy
import json

from constants import *
from models import Board, User, Player

import logging
import asyncio
import websockets

from notf_server import websockethandler, getaddrinfo, Notifications


SERVER = "127.0.0.1"
PORT = None
ADDR = None
NTF_MONITOR = None


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
                    if new_command[2] in self.connected_users.keys():
                        mess["status"] = 400
                        mess[
                            "message"
                        ] = f"There is already a user with username '{new_command[2]}'!"
                    else:
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
                            self.boards[board_id].getboardinfo()
                            for board_id in self.boards
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
                if board is None:
                    mess["message"] = f"Board with id {new_command[2]} does not exists!"
                    mess["status"] = 400
                player = board.attach(self.connected_users[new_command[2]], None, None)
                self.connected_users[new_command[2]] = player
                mess["message"] = f"You are in the board with id {new_command[2]}"
                mess["status"] = 200
                mess.update(
                    self.connected_users.get(player.user.username).board.getboardstate()
                )
                mess = json.dumps(mess)
                self.buf.append(mess)
                self.newmess.notify_all()
                self.lock.release()
                return mess
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
                mess["status"] = 200
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
        # Notifications().addNotification(
        #     "*",
        #     mess
        #     if type(mess) == str
        #     else mess.get("message", "This is a random message"),
        # )
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
            server_message = self.game.getmessages()
            self.conn.send("\n".join([i for i in server_message]).encode())
            inp = self.conn.recv(1024)
        print(f"Client ({self.claddr[1]}) is terminating!")
        self.conn.close()


class WSThread(Thread):
    def __init__(self):
        Thread.__init__(self)

    def run(self):
        ws_addr = "127.0.0.1:30002".split(":", 1)
        ws_addr = getaddrinfo(
            ws_addr[0], ws_addr[1], socket.AF_INET, socket.SOCK_STREAM
        )
        ws_addr = ws_addr[0][4]

        loop = asyncio.new_event_loop()
        asyncio.set_event_loop(loop)

        ws_server = websockets.serve(
            websockethandler, ws_addr[0], ws_addr[1], loop=loop
        )

        loop.run_until_complete(ws_server)
        loop.run_forever()


class TCPThread(Thread):
    def __init__(self, s, g_monitor):
        self.s = s
        self.g_monitor = g_monitor
        Thread.__init__(self)

    def run(self):
        print("INFO: TCP server is active!\n-------------------------")
        while True:
            ns, peer = self.s.accept()
            print("Connection established by: ", peer)
            ra = RecieveAgent(ns, peer, self.g_monitor)
            ra.start()


def main():
    global PORT, ADDR, NTF_MONITOR
    PORT = int(sys.argv[2])
    ADDR = (SERVER, PORT)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(ADDR)
    s.listen(16)

    game_monitor = GameMonitor()

    ra = WSThread()
    sa = TCPThread(s, game_monitor)
    ra.start()
    sa.start()


if __name__ == "__main__":
    main()
