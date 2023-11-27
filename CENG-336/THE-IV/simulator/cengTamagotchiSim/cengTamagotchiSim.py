#!/usr/bin/env python
import collections
import queue
import string
import time
import os, pygame, sys, threading, serial, time, array
from pygame.locals import *
import math
import random
from collections import deque
import json
import utils

FPS = 30
PORT = '/dev/ttyUSB0'
BAUDRATE = 115200
SEED_VALUE = 3362022

WAITING = 0
GETTING = 1

timeout = 100

DISPLAY_WIDTH = 800
DISPLAY_HEIGHT = 350

with open("settings.json","r") as f:
    SETTINGS = json.loads(f.read())

class Miniterm:
    def __init__(self, port, baudrate, parity, rtscts, xonxoff, virtual_cat):
        self.serial = serial.Serial(port, baudrate, parity=parity,
                                rtscts=rtscts, xonxoff=xonxoff, timeout=None)
        self.state = WAITING
        self.data = b''
        self.time = 0
        self.avg_time = -1
        self.max_time = -1
        self.min_time = -1
        self.prev_time = -1
        self.startTime = -1
        self.endTime = -1
        self.remTime = timeout
        self.cmdCount = 0
        self.commands = queue.Queue()
        self.writer_lock = threading.Lock()
        self.virtual_cat = virtual_cat
        virtual_cat.miniterm = self

        self.alive = True
        self.receiver_thread = threading.Thread(target=self.reader)
        self.receiver_thread.setDaemon(1)
        self.receiver_thread.start()


    def start(self):
        self.cmdCount = 0
        self.state = WAITING
        self.startTime = time.time()



    def stop(self):
        self.alive = False

    def join(self):
        self.receiver_thread.join(0.1)

    def updateFPS(self):
        if self.virtual_cat.get_op_mode() != "ACTIVE":
            return
        self.time = time.time()
        diff1 = (self.time-self.startTime)*1000.0
        self.cmdCount = self.cmdCount + 1
        self.avg_time = (diff1)/self.cmdCount

        if(self.prev_time != -1):
            diff2 = (self.time-self.prev_time)*1000.0
            #print('Current time difference from previous command: '+str(diff2)+' ms')
            if diff2 < self.min_time or self.min_time == -1:
                self.min_time = diff2
            if diff2 > self.max_time or self.max_time == -1:
                self.max_time = diff2
        self.prev_time = self.time

    def reader(self):
        WAITING = 0
        GETTING = 1
        while self.alive:
            if self.state == WAITING:
                byte = self.serial.read()
                if byte == b'{':
                    self.state = GETTING
            elif self.state == GETTING:
                byte = self.serial.read()
                if byte == b'}':
                    if self.data == b'C' or \
                        self.data in COMMAND_STAT_DICT or \
                        (self.data.startswith(b'H') and len(self.data) == 17):
                        self.updateFPS()
                        self.commands.put(self.data)
                    else:
                        print(f"Warning: unknown command {self.data}")
                    self.data = b""
                    self.state = WAITING
                else:
                    self.data = b"".join([self.data, byte])

    def write(self, strr):
        self.writer_lock.acquire()
        self.serial.write(strr)
        self.writer_lock.release()

INCREMENT_PRICE_DICT = {"hunger": (SETTINGS["FOOD_INCREMENT"], SETTINGS["FOOD_PRICE"]),
                        "happy": ((SETTINGS["TOY_INCREMENT"], SETTINGS["TOY_PRICE"])),
                        "thirst": ((SETTINGS["WATER_INCREMENT"], SETTINGS["WATER_PRICE"]))}

COMMAND_STAT_DICT = {b"F": "hunger", b"P": "happy", b"W": "thirst"}

class VirtualCat:
    def __init__(self):
        self.lock = threading.Lock()
        self._stats = {"hunger": 100, "happy": 100, "thirst": 100}

        self.decrements = []
        self.money = 0
        time_passed = 0
        self._solved_keys = 0
        self._sent_keys = 0
        self.miniterm = None

        self.op_mode = "IDLE"

    def get_op_mode(self):
        self.lock.acquire()
        op_mode = self.op_mode
        self.lock.release()
        return op_mode

    def set_op_mode(self, op_mode):
        self.lock.acquire()
        self.op_mode = op_mode
        self.lock.release()


    def get_stats(self):
        self.lock.acquire()
        result = self._stats.copy()
        self.lock.release()
        return result

    def increment_stat(self, stat, amount):
        self.lock.acquire()
        self._stats[stat] += amount
        self._stats[stat] = min(100,max(0,self._stats[stat]))
        self.lock.release()

    def increment_money(self, amount):
        self.lock.acquire()
        self.money += amount
        self.lock.release()

    def replenish_stat(self, stat):
        self.lock.acquire()
        increment, price = INCREMENT_PRICE_DICT[stat]
        if self.money-price < 0:
            print(f"Error: Insufficient funds for increasing {stat} for the price {INCREMENT_PRICE_DICT[stat][1]}!")
            self.lock.release()
            return
        self._stats[stat] += increment
        self._stats[stat] = min(100, max(0, self._stats[stat]))
        self.money -= price
        self.lock.release()

    def get_money(self):
        self.lock.acquire()
        result = self.money
        self.lock.release()
        return result

    def get_sent_keys(self):
        self.lock.acquire()
        result = self._sent_keys
        self.lock.release()
        return result

    def increment_sent_keys(self):
        self.lock.acquire()
        self._sent_keys += 1
        self.lock.release()

    def get_solved_keys(self):
        self.lock.acquire()
        result = self._solved_keys
        self.lock.release()
        return result

    def key_solved(self,ind):
        self.lock.acquire()
        self._solved_keys += 1
        self.money += self.key_queue[ind][1]
        self.lock.release()

    def start(self, easy):
        self.easy = easy
        if easy:
            time_passed = 0.0
            while True:
                interval = random.uniform(SETTINGS["INTERVAL_MIN"], SETTINGS["INTERVAL_MAX"])
                if interval+time_passed > SETTINGS["TOTAL_TIME"]-5.0:
                    break
                time_passed += interval
                decrement = random.randrange(SETTINGS["DECREMENT_MIN"], SETTINGS["DECREMENT_MAX"])
                stat = random.choice(list(self._stats.keys()))
                self.decrements.append((interval, stat, decrement))

            dry_stats = self._stats.copy()
            required_money = 0
            for _, stat, decrement in self.decrements:
                dry_stats[stat] -= decrement
                for key,(increment, price) in INCREMENT_PRICE_DICT.items():
                    assert(dry_stats[key] > 0)
                    if dry_stats[key] <= 100-increment:
                        dry_stats[key] = max(100, dry_stats[key]+increment)
                        required_money += price

            self.money = required_money+SETTINGS["FRUGALNESS_EASY_TOLERANCE"]
        else:
            time_passed = 0.0
            self.key_queue = []
            for _ in range(SETTINGS["TOTAL_HASHES"]):
                interval = random.uniform(SETTINGS["HASH_INTERVAL_MIN"], SETTINGS["HASH_INTERVAL_MAX"])
                time_passed +=  interval
                key = "".join([random.choice(string.ascii_lowercase) for _ in range(SETTINGS["HASH_KEY_LENGTH"])])
                key = key.encode("ascii")
                answer = utils.hash_336(key.decode("ascii")).encode("ascii")
                assert(utils.bytes_sanity_check(answer))
                self.key_queue.append([key, 0, time_passed, answer])
            assert(time_passed <= SETTINGS["TOTAL_TIME"])
            assert(len(self.key_queue) == len(set([k[0] for k in self.key_queue])))
            assert(len(self.key_queue) == len(set([k[3] for k in self.key_queue])))

            time_passed = 0.0
            while True:
                interval = random.uniform(SETTINGS["INTERVAL_MIN"], SETTINGS["INTERVAL_MAX"])
                if interval+time_passed > SETTINGS["TOTAL_TIME"]-5.0:
                    break
                time_passed += interval
                decrement = random.randrange(SETTINGS["DECREMENT_MIN"], SETTINGS["DECREMENT_MAX"])
                stat = random.choice(list(self._stats.keys()))
                self.decrements.append((interval, stat, decrement))

            dry_stats = self._stats.copy()
            required_moneys = []
            time_passed = 0.0
            periods = [k[2] for k in self.key_queue]
            periods_iter = iter(periods)
            current_period = next(periods_iter)
            current_required_money = 0
            for interval, stat, decrement in self.decrements:
                if interval+time_passed > current_period:
                    required_moneys.append(current_required_money+SETTINGS["FRUGALNESS_HARD_TOLERANCE"])
                    try:
                        current_period = next(periods_iter)
                    except:
                        current_period = SETTINGS["TOTAL_TIME"]
                    current_required_money = 0

                time_passed += interval
                dry_stats[stat] -= decrement
                for key,(increment, price) in INCREMENT_PRICE_DICT.items():
                    assert(dry_stats[key] > 0)
                    if dry_stats[key] <= 100-increment:
                        dry_stats[key] = max(100, dry_stats[key]+increment)
                        current_required_money += price

            required_moneys.append(current_required_money)

            assert(len(required_moneys) == SETTINGS["TOTAL_HASHES"]+1)
            self.money = required_moneys[0]
            for i,k in enumerate(self.key_queue):
                k[1] = required_moneys[i+1]

            self.key_sending_thread = threading.Thread(target=self._key_sending_thread)
            self.key_sending_thread.setDaemon(1)
            self.key_sending_thread.start()

        self.decrementing_thread = threading.Thread(target=self._decrementing_thread)
        self.decrementing_thread.setDaemon(1)
        self.decrementing_thread.start()

    def _decrementing_thread(self):
        time.sleep(1)
        while(self.decrements):
            interval, stat, decrement = self.decrements[0]
            self.decrements = self.decrements[1:]
            time.sleep(interval)
            self.increment_stat(stat,-decrement)

    def _key_sending_thread(self):
        prev_time = 0
        for i,elem in enumerate(self.key_queue):
            key, money, time_passed, answer = elem
            #print(f"will send {key} at {time_passed}")
            time.sleep(time_passed-prev_time-2)
            while i > self.get_solved_keys():
                time.sleep(1) #terrible hack but just werks.
            time.sleep(2)
            if self.get_op_mode() == "ACTIVE":
                print(f"sending key {key} with hash {answer}")
                self.miniterm.write(b"".join([b"{A", key ,b"}"]))
                self.increment_sent_keys()

            prev_time = time_passed


ITEM_FRAMES = 30

def main():
    random.seed(SEED_VALUE)
    pygame.init()
    cute_cat_sprite = pygame.image.load("cute_cat.png")
    fail_cat_sprite = pygame.image.load("fail_cat.png")
    win_cat_sprite = pygame.image.load("win_cat.png")

    current_cat_sprite = cute_cat_sprite

    food_sprite = pygame.image.load("food.png")
    water_sprite = pygame.image.load("water.png")
    toy_sprite = pygame.image.load("toy.png")

    clock = pygame.time.Clock()
    virtual_cat = VirtualCat()
    miniterm = Miniterm(PORT, BAUDRATE, 'N', rtscts=False, xonxoff=False, virtual_cat=virtual_cat)
    start_time = -1
    final_time = -1

    stat_drawing_list = []

    font_h2 = pygame.font.SysFont("Arial", 16)

    screen = pygame.display.set_mode((DISPLAY_WIDTH,DISPLAY_HEIGHT))

    def get_sim_time():
        if start_time == -1:
            return 0
        if final_time != -1:
            return final_time-start_time

        return time.time() - start_time

    def draw_info():
        margin = 10
        current_top = margin
        max_textwidth = max([font_h2.size("f{stat}: {value}")[0] for stat,value in virtual_cat.get_stats().items()])

        for stat, value in virtual_cat.get_stats().items():
            render = font_h2.render(f"{stat}: {value}", True, (0, 0, 0))
            size = font_h2.size(f"{stat}: {value}")
            screen.blit(render, (10, current_top))
            pygame.draw.rect(screen, (int(255*(100-value)/100),int(255*(value)/100),0), (max_textwidth+margin, current_top, value, size[1]))
            current_top += size[1]+margin
            max_textwidth = max(max_textwidth, size[0])

        text5 = font_h2.render(f'Money: {virtual_cat.get_money():>5d} Sent Alerts: {virtual_cat.get_sent_keys()} '+
                               f'Received Hashes : {virtual_cat.get_solved_keys()}', True, (0, 100, 0))


        text6 = font_h2.render(f'Timing[ms] Avg: {miniterm.avg_time:>4.2f} '+
                               f'Max: {miniterm.avg_time:>4.2f} '+
                               f'Min: {miniterm.min_time:>4.2f} '+
                               f'Time Elapsed[s]: {int(get_sim_time()):>4d}', True, (0,100,0))
        screen.blit(text5,(20.,DISPLAY_HEIGHT-60))
        screen.blit(text6,(20.,DISPLAY_HEIGHT-30))


    def print_final_info(success):
        print(f'Simulation result: {"SUCCESS" if success else "FAIL"}\n'
        f'Timing[ms] Avg: {miniterm.avg_time:>4.2f}\n' +
        f'Max: {miniterm.avg_time:>4.2f}\n' +
        f'Min: {miniterm.min_time:>4.2f}\n' +
        f'Time Elapsed: {int(get_sim_time()):>4d}\n' +
        f'Received Hashes: {virtual_cat.get_solved_keys()}\n' +
        f'Sent Alerts: {virtual_cat.get_sent_keys()}\n' +
        f'Remaining Money: {virtual_cat.get_money()}')


    pygame.display.set_caption("CENG 336 THE4 2022")

    while True:
        screen.fill((255, 255, 255))
        screen.blit(current_cat_sprite, (DISPLAY_WIDTH-300,30))

        new_stat_drawing_list = []
        for item,duration in stat_drawing_list:
            if item == b'F':
                screen.blit(food_sprite, (DISPLAY_WIDTH - 380, 50))
            elif item == b'W':
                screen.blit(water_sprite, (DISPLAY_WIDTH - 372, 130))
            elif item == b'P':
                screen.blit(toy_sprite, (DISPLAY_WIDTH - 380, 210))
            if duration > 0:
                new_stat_drawing_list.append((item, duration-1))

        stat_drawing_list = new_stat_drawing_list

        draw_info()

        pygame.display.update()
        for event in pygame.event.get():
            if event.type == QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    pygame.quit()
                    sys.exit()
                if event.key == K_e and virtual_cat.get_op_mode() == "IDLE":
                    virtual_cat.set_op_mode("ACTIVE")
                    start_time = time.time()
                    virtual_cat.start(True)
                    money = virtual_cat.get_money()
                    assert(money < 65535)
                    money = int.to_bytes(money,2,'big')
                    miniterm.start()
                    msg = b''.join([b'{GO',money,b'}'])
                    assert(len(msg) == 6)
                    miniterm.write(msg)
                if event.key == K_h and virtual_cat.get_op_mode() == "IDLE":
                    virtual_cat.set_op_mode("ACTIVE")
                    start_time = time.time()
                    virtual_cat.start(False)
                    money = virtual_cat.get_money()
                    assert(money < 65535)
                    money = int.to_bytes(money,2,'big')
                    miniterm.start()
                    msg = b''.join([b'{GO',money,b'}'])
                    assert(len(msg) == 6)
                    miniterm.write(msg)

        if virtual_cat.get_op_mode() == "ACTIVE":
            stats = virtual_cat.get_stats()
            dead_flag = False
            for stat, value in stats.items():
                if value <= 0:
                    print(f"The digital pet has unfortunately died: {stat} meter is 0")
                    print_final_info(False)
                    miniterm.write(b'{END}')
                    virtual_cat.set_op_mode("END")
                    final_time = time.time()
                    dead_flag = True
                    current_cat_sprite = fail_cat_sprite
                    break
            if not dead_flag and time.time()-start_time > SETTINGS["TOTAL_TIME"]:
                virtual_cat.set_op_mode("END")
                final_time = time.time()
                miniterm.write(b'{END}')
                print(f"Congrats, your digital pet has made it for {SETTINGS['TOTAL_TIME']} seconds!!")
                print_final_info(True)
                current_cat_sprite = win_cat_sprite

        if virtual_cat.get_op_mode() != "ACTIVE":
            while not miniterm.commands.empty():
                command = miniterm.commands.get()
                print(f"Warning! Command sent in {virtual_cat.get_op_mode()} mode: {command}")
        else:
            while not miniterm.commands.empty():
                command = miniterm.commands.get()
                if command in COMMAND_STAT_DICT:
                    virtual_cat.replenish_stat(COMMAND_STAT_DICT[command])
                    stat_drawing_list.append((command, ITEM_FRAMES))
                elif command == b'C':
                    stats = virtual_cat.get_stats()
                    msg = b''.join([b'{S',bytes([stats['hunger'],stats['happy'],stats['thirst']]),b'}'])
                    assert(len(msg) == 6)
                    miniterm.write(msg)
                elif command.startswith(b'H') and len(command) == 17:
                    answers = [k[-1] for k in virtual_cat.key_queue]
                    if command[1:] in answers:
                        ind = answers.index(command[1:])
                        money = virtual_cat.key_queue[ind][1]
                        virtual_cat.key_solved(ind)
                        assert(utils.bytes_sanity_check(int.to_bytes(money,2,'big')))
                        miniterm.write(b''.join([b'{M',int.to_bytes(money,2,'big'),b'}']))
                else:
                    print(f"Unknown command sent in ACTIVE mode: {command}")


        clock.tick(FPS)

main()
