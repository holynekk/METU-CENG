# Cansu Ozer       - 2380780
# Mert Kaan YILMAZ - 2381093

import json

from models import Board, User
from constants import *


def main():
    input_location = "sample1.json"
    with open(input_location) as f:
        input_ = json.loads(f.read())

    board_instance = Board(**input_)

    demo(board_instance, input_location)


def demo(board_instance, input_file):
    print(
        f"Monopoly Board initialized with '{input_file}' file.\n------------------------------------------------------------"
    )

    # Checking if circular linked list structure created for board instance
    starting_cell = loop_cell = board_instance.initial_cell
    board_cell_count = len(board_instance.cells)
    for _ in range(board_cell_count):
        loop_cell = loop_cell.next_cell
    assert loop_cell == starting_cell

    # Creating user instances
    user1 = User("holynekk", "holynekk@gmail.com", "Mert Kaan YILMAZ", "bumbum34")
    user2 = User("cnzr", "cnzr@gmail.com", "Cansu Ozer", "cnz(3g4)")
    user3 = User("oguzsasi", "oguzsasi@hotmail.com", "Oguz Sasi", "oguz435()")
    user4 = User("ohnePixel", "ohnePixel@gmail.com", "Mark", "mark&24g")

    # Logging in action for each user
    user1.login("bumbum34")
    user2.login("cnz(3g4)")
    user3.login("oguz435()")
    user4.login("mark&24g")

    # Attach users to the board
    player1 = board_instance.attach(user1, None, None)
    player2 = board_instance.attach(user2, None, None)
    player3 = board_instance.attach(user3, None, None)
    player4 = board_instance.attach(user4, None, None)

    # Player list to use it in tester
    player_list = [player1, player2, player3, player4]

    for board_user, user in tuple(zip(board_instance.player_list, player_list)):
        assert board_user == user

    # Update user status as 'Ready'
    board_instance.ready(player1)
    board_instance.ready(player2)
    board_instance.ready(player3)
    board_instance.ready(player4)

    # Game has been started! ----------------------------------------------

    assert board_instance.turn_ == 0

    # Player 2 -> tries to play when turn is not on it.
    result_text = board_instance.turn(
        player2, "Roll", extra_field={"first_dice": 1, "second_dice": 3}
    )
    assert result_text == "It's not your turn!"

    # Player 1 -> ROLL AND BUY
    result_text = board_instance.turn(
        player1, "Roll", {"first_dice": 1, "second_dice": 3}
    )
    assert "on the property" in result_text
    assert player1.board_position.name == "Anadolu Hisari"
    board_instance.turn(player1, "Buy")
    assert player1.board_position.owner.user.username == player1.user.username
    assert player1.balance == board_instance.startup - player1.board_position.price

    assert board_instance.turn_ == 1

    # Player 2 -> ROLL, GET ON TAX CELL, PAY
    board_instance.turn(player2, "Roll", {"first_dice": 1, "second_dice": 2})
    assert player2.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_TAX][1]
    assert player2.balance == board_instance.startup - board_instance.tax

    assert board_instance.turn_ == 2

    # Player 3 -> ROLL, GET ON PROPERTY, DOES NOTHING
    board_instance.turn(player3, "Roll", {"first_dice": 4, "second_dice": 2})
    assert player3.board_position.name == "BeylerBeyi"
    board_instance.turn(player3, "Pass")

    assert board_instance.turn_ == 3

    # Player 4 -> ROLL, GET ON PROPERTY OWNED BY PLAYER1, PAYS RENT
    board_instance.turn(player4, "Roll", {"first_dice": 3, "second_dice": 1})
    assert player4.board_position.name == "Anadolu Hisari"
    assert player4.balance == board_instance.startup - player4.board_position.get_rent()
    assert (
        player1.balance
        == board_instance.startup
        - player4.board_position.price
        + player4.board_position.get_rent()
    )
    board_instance.turn(player4, "Pass")

    assert board_instance.turn_ == 0

    # Player 1 -> ROLLS, GETS INTO JAIL
    result = board_instance.turn(player1, "Roll", {"first_dice": 1, "second_dice": 3})
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]
    assert player1.in_jail == True

    assert board_instance.turn_ == 1

    # Player 2 -> ROLLS, GETS ON CHANCE, PICK GOTO JAIL CARD, GETS INTO JAIL
    board_instance.turn(player2, "Roll", {"first_dice": 1, "second_dice": 1})
    assert player2.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]
    assert player2.in_jail == True

    assert board_instance.turn_ == 2

    # Player 3 -> ROLLS, GETS ON PROPERTY, BUYS
    board_instance.turn(player3, "Roll", {"first_dice": 4, "second_dice": 2})
    assert player3.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
    current_balance = player3.balance
    board_instance.turn(player3, "Buy")
    assert player3.balance == current_balance - player3.board_position.price
    assert player3.board_position.owner.user.username == player3.user.username

    assert board_instance.turn_ == 3

    # Player 4 -> ROLLS, DOES NOTHING
    board_instance.turn(player4, "Roll", {"first_dice": 5, "second_dice": 6})
    assert player4.board_position.name == "Emirgan"
    board_instance.turn(player4, "Pass")

    assert board_instance.turn_ == 0

    # Player 1 -> TRYING TO GET DOUBLE DICE TO GET OUT FROM JAIL, FAILS
    board_instance.turn(player1, "Roll", {"first_dice": 1, "second_dice": 6})
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]
    assert player1.in_jail == True

    assert board_instance.turn_ == 1

    # Player 2 -> USES JAIL BAIL, PAYS AND GETS OUT FROM JAIL
    current_balance = player2.balance
    board_instance.turn(player2, "Bail")
    assert player2.balance == current_balance - board_instance.jailbail
    assert player2.in_jail == False

    assert board_instance.turn_ == 2

    # Player 3 -> ROLLS, GETS ON CHANCE, PICK LOTTERY CHANCE CARD, INCREASES BALANCE
    current_balance = player3.balance
    board_instance.turn(player3, "Roll", {"first_dice": 1, "second_dice": 5})
    assert player3.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert player3.balance == current_balance + board_instance.lottery

    assert board_instance.turn_ == 3

    # Player 4 -> ROLLS, GETS ON CHANCE, PICK JAIL FREE CHANCE CARD AND KEEPS IT
    number_of_chance_cards = len(board_instance.chances_cards)
    board_instance.turn(player4, "Roll", {"first_dice": 1, "second_dice": 2})
    assert player4.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert number_of_chance_cards == len(board_instance.chances_cards) + 1
    assert len(player4.chance_cards_owned) == 1
    assert player4.chance_cards_owned[0] == CHANCE_CHOICES[CHANCE_JAIL_FREE][1]

    assert board_instance.turn_ == 0

    # Player 1 -> TRYING TO GET DOUBLE AGAIN DICE TO GET OUT FROM JAIL, SUCCESS
    assert player1.in_jail == True
    resut = board_instance.turn(player1, "Roll", {"first_dice": 2, "second_dice": 2})
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]
    assert player1.in_jail == False

    assert board_instance.turn_ == 1

    # Player 2 -> ROLLS, GETS ON CHANCE, PICK TAX CHANCE CARD, PAYS
    current_balance = player2.balance
    board_instance.turn(player2, "Roll", {"first_dice": 4, "second_dice": 6})
    assert player2.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert player2.balance == current_balance - board_instance.tax

    assert board_instance.turn_ == 2

    # Player 3 -> ROLLS, GOTS TURN-COMPLETE MONEY, GET ON PROPERTY, BUYS IT
    current_balance = player3.balance
    board_instance.turn(player3, "Roll", {"first_dice": 1, "second_dice": 3})
    assert player3.balance == current_balance + board_instance.startup
    assert player3.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
    assert player3.board_position.name == "Mecidiye Koy"
    board_instance.turn(player3, "Buy")
    assert (
        player3.board_position.get_propert_state()["owner"]["username"]
        == player3.user.username
    )

    assert board_instance.turn_ == 3

    # Player 4 -> ROLLS, GOTS INTO JAIL, BUT HAVE JAIL FREE CARD, INSTANTLY GETS OUT FROM JAIL
    number_of_chance_cards = len(board_instance.chances_cards)
    assert player4.in_jail == False
    board_instance.turn(player4, "Roll", {"first_dice": 4, "second_dice": 6})
    assert player4.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]
    assert player4.in_jail == False
    assert len(player4.chance_cards_owned) == 0
    assert len(board_instance.chances_cards) == number_of_chance_cards + 1

    assert board_instance.turn_ == 0

    # Player 1 -> ROLLS, GETS ON TELEPORT CELL, PICKS MORE THAN ONCE, TELEPORTS, AND UPGRADES HIS OWN PROPERTY
    board_instance.turn(player1, "Roll", {"first_dice": 2, "second_dice": 6})
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_TELEPORT][1]
    board_instance.turn(player1, "Pick", {"picked_location": "Yenikoy"})
    assert player1.picked_cell == "Yenikoy"
    board_instance.turn(player1, "Pick", {"picked_location": "Anadolu Hisari"})
    assert player1.picked_cell == "Anadolu Hisari"
    board_instance.turn(player1, "Teleport")
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
    assert player1.board_position.name == "Anadolu Hisari"
    r = board_instance.turn(player1, "Roll", {"first_dice": 4, "second_dice": 6})
    assert r == "You have already used Roll command!"
    current_balance = player1.balance
    board_instance.turn(player1, "Upgrade")
    assert player1.balance == current_balance - board_instance.upgrade
    assert player1.board_position.level == 1

    assert board_instance.turn_ == 1

    # Player 2 -> ROLLS, GETS ON PROPERTY OWNED BY PLAYER 1, PAYS RENT
    current_balance = player2.balance
    current_balance_2 = player1.balance
    board_instance.turn(player2, "Roll", {"first_dice": 2, "second_dice": 4})
    assert player2.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
    assert player2.board_position.name == "Anadolu Hisari"
    assert (
        player2.balance
        == current_balance
        + board_instance.startup
        - player2.board_position.rents[player2.board_position.level]
    )
    assert (
        player1.balance
        == current_balance_2
        + player2.board_position.rents[player2.board_position.level]
    )

    assert board_instance.turn_ == 2

    # Player 3 -> ROLLS, PICK TELEPORT CHANCE CARD, PICKS AND TELEPORTS, BUYS
    r = board_instance.turn(player3, "Roll", {"first_dice": 2, "second_dice": 1})
    assert player3.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert board_instance.turn_ == 2
    board_instance.turn(player3, "Pick", {"picked_location": "Uskudar"})
    board_instance.turn(player3, "Teleport")
    assert player3.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
    assert player3.board_position.name == "Uskudar"
    board_instance.turn(player3, "Buy")

    assert board_instance.turn_ == 3

    # Player 4 -> ROLLS, BUYS
    board_instance.turn(player4, "Roll", {"first_dice": 1, "second_dice": 2})
    board_instance.turn(player4, "Buy")

    assert board_instance.turn_ == 0

    # Player 1 -> ROLLS, PICK CHANCE, UPGRADE HIS OWN PROPERTY
    board_instance.turn(player1, "Roll", {"first_dice": 1, "second_dice": 0})
    # (0, 1) dice result is not possible in game but we are testing here, so why not :)
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert len(player1.chance_cards_owned) == 1
    assert player1.chance_cards_owned[0] == CHANCE_CHOICES[CHANCE_UPGRADE][1]
    board_instance.turn(player1, "Pick", {"picked_location": "Anadolu Hisari"})
    current_balance = player1.balance
    board_instance.turn(player1, "Upgrade")
    assert len(player1.chance_cards_owned) == 0
    assert player1.balance == current_balance
    assert player1.get_user_state()["properties_owned"][0]["current_rent"] == 500

    assert board_instance.turn_ == 1

    # Player 2 -> ROLLS, PICK CHANCE, DOWNGRADE PLAYER1's PROPERTY
    board_instance.turn(player2, "Roll", {"first_dice": 1, "second_dice": 0})
    # (0, 1) dice result is not possible in game but we are testing here, so why not :)
    assert player2.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert len(player2.chance_cards_owned) == 1
    assert player2.chance_cards_owned[0] == CHANCE_CHOICES[CHANCE_DOWNGRADE][1]
    board_instance.turn(player2, "Pick", {"picked_location": "Anadolu Hisari"})
    board_instance.turn(player2, "Downgrade")
    assert player1.get_user_state()["properties_owned"][0]["current_rent"] == 200

    assert board_instance.turn_ == 2

    # Player 3 -> ROLLS, BUYS
    board_instance.turn(player3, "Roll", {"first_dice": 1, "second_dice": 1})
    board_instance.turn(player3, "Buy")

    assert board_instance.turn_ == 3

    # Player 4 -> ROLLS, PICK COLOR_UPGRADE CHANCE CARD, PICKS AND UPGRADES BLUE COLOR
    board_instance.turn(player4, "Roll", {"first_dice": 4, "second_dice": 3})
    assert player4.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert len(player4.chance_cards_owned) == 1
    assert player4.chance_cards_owned[0] == CHANCE_CHOICES[CHANCE_COLOR_UPGRADE][1]
    board_instance.turn(player4, "Pick", {"picked_location": "Uskudar"})
    board_instance.turn(player4, "Upgrade")  # Upgrades blue properties
    assert (
        player1.get_user_state()["properties_owned"][0]["current_rent"] == 500
    )  # Anadolu Hisari
    assert (
        player3.get_user_state()["properties_owned"][2]["current_rent"] == 300
    )  # Uskudar

    assert board_instance.turn_ == 0

    # Player 1 -> ROLLS, PICK CHANCE COLOR_DOWNGRADE, DOWNGRADES BLUE COLOR
    board_instance.turn(player1, "Roll", {"first_dice": 0, "second_dice": 0})
    # (0, 0) dice result is not possible in game but we are testing here, so why not :)
    assert player1.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
    assert len(player1.chance_cards_owned) == 1
    assert player1.chance_cards_owned[0] == CHANCE_CHOICES[CHANCE_COLOR_DOWNGRADE][1]
    board_instance.turn(player1, "Pick", {"picked_location": "Anadolu Hisari"})
    board_instance.turn(player1, "Downgrade")
    assert len(player1.chance_cards_owned) == 0
    assert (
        player1.get_user_state()["properties_owned"][0]["current_rent"] == 200
    )  # Anadolu Hisari
    assert (
        player3.get_user_state()["properties_owned"][2]["current_rent"] == 80
    )  # Uskudar

    assert board_instance.turn_ == 1

    # Force players to have negative funds.

    # Player 2 -> ROLLS, GETS ON TAX CELL, CANNOT PAY, LOSES
    player2.balance = 1
    board_instance.turn(player2, "Roll", {"first_dice": 3, "second_dice": 5})
    assert player2.balance < 0
    assert len(board_instance.player_list) == 3
    assert (
        board_instance.player_list[board_instance.turn_].user.username
        == player3.user.username
    )

    # Player2 is out of game [p1,p2,p3,p4] -> [p1,p3,p4]
    # To continue game with player 3, turn_ should be still 1
    assert board_instance.turn_ == 1

    # Player 3 -> ROLLS, GETS ON TAX CELL, CANNOT PAY, LOSES
    player3.balance = 1
    board_instance.turn(player3, "Roll", {"first_dice": 3, "second_dice": 1})
    assert player3.balance < 0
    for p in player3.properties_owned:
        assert p.owner == None
    assert len(board_instance.player_list) == 2
    assert (
        board_instance.player_list[board_instance.turn_].user.username
        == player4.user.username
    )

    assert board_instance.turn_ == 1

    # Player 4 -> ROLLS, GETS ON TAX CELL, CANNOT PAY, LOSES
    player4.balance = -1 - board_instance.startup
    board_instance.turn(player4, "Roll", {"first_dice": 3, "second_dice": 2})
    assert player4.balance < 0
    for p in player4.properties_owned:
        assert p.owner == None
    assert len(board_instance.player_list) == 1
    assert (
        board_instance.player_list[board_instance.turn_].user.username
        == player1.user.username
    )

    assert board_instance.turn_ == 0

    result_text = board_instance.turn(
        player1, "Roll", {"first_dice": 3, "second_dice": 2}
    )
    assert result_text == "Game is over!"
    result_text = board_instance.turn(
        player1, "Roll", {"first_dice": 2, "second_dice": 2}
    )
    assert result_text == "Game is over!"

    # ---------------------------------------------------------------------

    # Game over! Detach users from the board
    board_instance.detach(player1)
    board_instance.detach(player2)
    board_instance.detach(player3)
    board_instance.detach(player4)

    assert board_instance.player_list == []

    print("All tests run. [OK]")
    print("------------------------------------------------------------\nGame is over!")


if __name__ == "__main__":
    main()
