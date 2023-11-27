import hashlib
import json
import socket

HOST, PORT = "127.0.0.1", 30000


def hash_password(plain_password):
    password = plain_password.encode("utf-8")
    hash_instance = hashlib.sha3_256(password)
    hashed_password = hash_instance.hexdigest()
    return hashed_password


def send_command(command):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.connect((HOST, PORT))
        sock.sendall(
            bytes(
                command + "\n",
                "utf-8",
            )
        )
        received = str(sock.recv(10000), "utf-8")
    return json.loads(received)
