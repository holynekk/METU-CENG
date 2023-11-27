import hashlib
import random


def hash_password(plain_password):
    password = plain_password.encode("utf-8")
    hash_instance = hashlib.sha3_256(password)
    hashed_password = hash_instance.hexdigest()
    return hashed_password


def roll_dice():
    random.seed()
    dice1 = random.randint(1, 6)
    dice2 = random.randint(1, 6)
    return dice1, dice2
