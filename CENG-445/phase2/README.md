**CENG445 - Online Monopoly**
--

Concurrent TCP server implementation for online monopoly game. 

**Authors:**
- Mert Kaan YILMAZ - 2381093

**Description**
--

This is the classic monopoly game implemented with python. The mechanics of the game is pretty much same with the classic version with a few additions. Here we have game board and player classes with their method implementations for specific actions that players made.


**Content Overview**
--

- *models.py*: This file is where we've written classes
- *constans.py*: This file includes constant values such as cell types, card types, user statuses, etc.
- *utils.py*: Includes util functions to use in class methods.
- *sample1.json*: Sample input file which we have created.
- *monopoly_server.py*: This is the server script where we create TCP server socket and wait for connection. It works with one GameMonitor object and multiple threads per client.

**How to Run**
--

- Server script should be running with `python3 monopoly_server.py --port 30000`
- Multiple different connection shoulld be established by other clients with `nc 127.0.0.1 30000`
- Every client should follow these steps to create a user and join/create a game:
  - User creation `new user <username> <email> <password> <fullname>`
  - Board Creation `new board`
  - Board attachment `open <board_id>`
- After these steps, all four users should give ready to start the game.
- If all users ready, *"The game is starting!"* message will come and all players will be able to send commands.
- To send commands use: `command <command_type>` such as `command roll`, `command pass`, `command buy`, etc.

