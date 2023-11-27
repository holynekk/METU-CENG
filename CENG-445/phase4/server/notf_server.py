from socket import *
import json
import http.cookies


def singleton(cls):
    """generic python decorator to make any class
    singleton."""
    _instances = {}  # keep classname vs. instance

    def getinstance():
        """if cls is not in _instances create it
        and store. return the stored instance"""
        if cls not in _instances:
            _instances[cls] = cls()
        return _instances[cls]

    return getinstance


@singleton
class Notifications:
    """An observer class, saving notifications and notifiying
    registered coroutines"""

    def __init__(self):
        self.observers = {}
        self.broadcast = set()
        self.messages = {}

    def newconnection(self, ws):
        self.broadcast.add(ws)

    def closeconnection(self, ws):
        self.broadcast.discard(ws)

    def register(self, ws, cid):
        """register a Lock and an id string"""
        if cid in self.observers:
            self.observers[cid].add(ws)
        else:
            self.observers[cid] = set([ws])
        print("Current observers", self.observers, self.broadcast)

    def unregister(self, ws, cid):
        """remove registration"""
        if cid not in self.observers:
            return
        self.observers[cid].discard(ws)
        if self.observers[cid] == set():
            del self.observers[cid]
        print("Current observers", self.observers, self.broadcast)

    async def addNotification(self, oid, message):
        """add a notification for websocket conns with id == oid
        the '*' oid is broadcast. Message is the dictionary
        to be sent to connected websockets.
        """
        print(oid, message)
        if oid == "*":  # broadcast message
            for c in self.broadcast:
                await c.send(json.dumps(message))
        elif oid in self.observers:
            for c in self.observers[oid]:
                await c.send(json.dumps(message))


async def websockethandler(websocket, path):
    # websocket.request_headers is a dictionary like object
    print(websocket.request_headers.items())
    # following parses the cookie object
    if "Cookie" in websocket.request_headers:
        print(http.cookies.SimpleCookie(websocket.request_headers["Cookie"]))

    # get the list of ids to follow from browser
    reqlist = await websocket.recv()
    idlist = json.loads(reqlist)

    print("connected", idlist)

    Notifications().newconnection(websocket)
    if type(idlist) != list:
        idlist = [idlist]
    for myid in idlist:
        Notifications().register(websocket, myid)
    idlist = set(idlist)

    try:
        while True:
            data = await websocket.recv()
            try:
                message = json.loads(data)
                if "command" in message and message["command"] == "add":
                    Notifications().register(websocket, message["id"])
                    idlist.add(message["id"])
                elif "command" in message and message["command"] == "delete":
                    Notifications().unregister(websocket, message["id"])
                    idlist.discard(message["id"])
                else:
                    await Notifications().addNotification(message["id"], message)
            except Exception as e:
                print("invalid message. {} : exception: {}".format(data, str(e)))
    except Exception as e:
        print(e)
    finally:
        print("closing", idlist)
        for myid in idlist:
            Notifications().unregister(websocket, myid)
        Notifications().closeconnection(websocket)
        websocket.close()
