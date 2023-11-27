**CENG445 - Online Monopoly**
--

Concurrent TCP server with django application for online monopoly game. 

**Authors:**
- Mert Kaan YILMAZ - 2381093

**Description**
--

This is the classic monopoly game implemented with python. The mechanics of the game is pretty much same with the classic version with a few additions. Here we have game board and player classes with their method implementations for specific actions that players made.


**Content Overview**
--
- *server/* : This folder includes phase2 code (TCP server implementation with required class library and other relevant things).
	- *models.py* : This file is where we've written classes.
	- *constans.py* : This file includes constant values such as cell types, card types, user statuses, etc.
	- *utils.py* : Includes util functions to use in class methods.
	- *sample1.json* : Sample input file which we have created.
	- *monopoly_server.py* : This is the server script where we create TCP server socket and wait for connection. It works with one GameMonitor object and multiple threads per client.
- *online_monopoly_game/* : The django project which serves a connection between browser and our TCP server.
	- *api/* : This is the django application where we hande browser requests.
		- *static/api/style.css* : All the styling content for style sheets to load.
		- *templates/api/\** : All the template files required for pages to load. 
		- *\** : All other files are related with djang applciation.
	- *online_monopoly_game/* : Django project's base folder.
- *requirements.txt* : Required packages to run python-django project.

**How to Run**
--

- The first to e done is running the server at the background.
	- Get into server directory `cd server`
	- Run the server script `python3 monopoly_server.py --port 30000` (Must work on port 3000!)
- Run the django project.
	- Move into django project `cd online_monopoly_game`
	- Create a virtual environment with python3.10 `python3 -m venv venv`
	- Activate venv `source venv/bin/activate`
	- Install required packages `pip install -r requirements.txt`
	- Run the project `./manage.py runserver`
	- Now, you can go and visit `localhost:8000` on your browser and play the game

