import itertools
import random
import json

from utils import hash_password, roll_dice
from constants import *


class Cell:
    """
    Base cell class to create different types of cells on the board.
    """

    def __init__(self, type_, next_cell):
        self.type = type_
        self.next_cell = next_cell

    def __str__(self):
        return f"Cell with type: {self.type}"


class Property(Cell):
    """
    Inherits from Base Cell class. It's created to hold extra information for
    property type cell.
    """

    def __init__(self, *args, **kwargs):
        super().__init__(kwargs["type"], None)
        self.name = kwargs["name"]
        self.cell = kwargs["cell"]
        self.color = kwargs["color"]
        self.price = kwargs["price"]
        self.rents = kwargs["rents"]
        self.owner = None
        self.level = 0

    def upgrade(self):
        self.level = self.level + 1 if self.level < 4 else self.level

    def downgrade(self):
        self.level = self.level - 1 if self.level > 0 else self.level

    def get_rent(self):
        return self.rents[self.level]

    def get_propert_state(self):
        data = dict()
        data["name"] = self.name
        data["color"] = self.color
        data["price"] = self.price
        data["rent"] = self.rents[self.level]
        data["owner"] = self.owner.get_user_state() if self.owner else None
        return data

    def __str__(self):
        return f"Property with color-name: {self.color} - {self.name}"


class User:
    """
    Base user class to handle user management
    """

    id_iter = itertools.count()

    def __init__(self, username, email, fullname, passwd):
        self.id = next(self.id_iter)
        self.username = username
        self.email = email
        self.fullname = fullname
        self.passwd = hash_password(passwd)

    def auth(self, plainpass):
        return hash_password(plainpass) == self.passwd

    def login(self, passwd):
        # TODO: Start a session for the user, return a random token to be used during the session
        if self.auth(passwd):
            pass
        return False

    def logout(self):
        # TODO: End the session invalidating the token
        pass

    def checksession(token):
        # TODO: Check if the token is valid, returned by the last login
        pass

    # TODO: CRUD methods in addition to constructor
    def get(self):
        return {
            "username": self.username,
            "fullname": self.fullname,
            "email": self.email,
        }

    def update(self, username, email, fullname):
        self.username = username
        self.email = email
        self.fullname = fullname

    def delete(self):
        pass

    def __str__(self):
        return f"{{id: {self.id}, username: {self.username}}}"

    def __del__(self):
        return f"User {self.username} with id: {self.id} has been deleted!"


class Player:
    """
    Created after attaching to the board.
    """

    id_iter = itertools.count()

    def __init__(self, user, board, board_position, initial_balance):
        self.id = next(self.id_iter)
        self.user = user
        self.board = board
        self.board_position = board_position
        self.status = USER_STATUS_NOT_READY
        self.balance = initial_balance
        self.picked_cell = None
        self.properties_owned = []
        self.chance_cards_owned = []
        self.in_jail = False
        self.enough_is_enough = USER_ACTION_STATUS_MUST_ROLL

    def move(self, cell_count=1, is_teleported=False):
        """
        Used to move player according to dice result (cell_count).
        If 'is_teleported' parameter is not provided, players earns money
        everytime it passes through start cell
        """
        for _ in range(cell_count):
            self.board_position = self.board_position.next_cell
            if (
                not is_teleported
                and self.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_START][1]
            ):
                self.balance += self.board.startup

    def put_into_jail(self):
        """
        Method used to put the player into jail. It checks if player has jail_free card or not.
        If it has one, card is automatically used and put back into the deck.
        """
        while self.board_position.type != CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]:
            self.move(is_teleported=True)
        if not CHANCE_CHOICES[CHANCE_JAIL_FREE][1] in self.chance_cards_owned:
            self.in_jail = True
        else:
            self.board.put_card_back(CHANCE_CHOICES[CHANCE_JAIL_FREE][1])
            self.chance_cards_owned.remove(CHANCE_CHOICES[CHANCE_JAIL_FREE][1])

    def pay(self, amount, other_player=None):
        """
        Wherever there is a payment (tax, rent, etc.), this method is called. At the end of the
        payment, player balance is checked and player loses the game if balance is negative.
        """
        if not other_player:
            self.balance -= amount
        else:
            self.balance -= amount
            other_player.balance += amount
        if self.balance < 0:
            for p in self.properties_owned:
                p.level = 0
                p.owner = None
            if CHANCE_CHOICES[CHANCE_JAIL_FREE][1] in self.chance_cards_owned:
                self.board.put_card_back(CHANCE_CHOICES[CHANCE_JAIL_FREE][1])
            self.board.turn_ -= 1
            self.board.player_list.remove(self)
            self.board.is_game_over = True if len(self.board.player_list) < 2 else False

    def get_user_state(self):
        data = dict()
        data["username"] = self.user.username
        data["status"] = USER_STATUS_CHOICES[self.status][1]
        data["balance"] = self.balance
        data["properties_owned"] = []
        for property in self.properties_owned:
            data["properties_owned"].append(
                {
                    "property_name": property.name,
                    "color": property.color,
                    "current_rent": property.get_rent(),
                }
            )
        data["chance_cards_owned"] = self.chance_cards_owned
        data["is_in_jail"] = self.in_jail
        return data

    def __str__(self):
        return f"{{{self.id} - {self.user.username} - ${self.balance}}}"


class Board:
    """
    Monopoly board class where the game information is held.
    """

    id_iter = itertools.count()

    def __init__(self, *args, **kwargs):
        self.id = next(self.id_iter)
        self.cells = kwargs["cells"]
        self.initial_cell = None
        self.upgrade = kwargs["upgrade"]
        self.teleport = kwargs["teleport"]
        self.jailbail = kwargs["jailbail"]
        self.tax = kwargs["tax"]
        self.lottery = kwargs["lottery"]
        self.startup = kwargs["startup"]
        self.chances_cards = []
        self.player_list = []
        self.turn_ = 0
        self.is_game_over = True

        # Initializing chance cards
        for card in kwargs["chances"]:
            self.chances_cards.append(card["type"])

        # Creating the board (cells) as a cyclic Linkedlist
        self.initial_cell = self.cells[0]
        self.initial_cell = (
            Property(**self.initial_cell)
            if self.initial_cell["type"] == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
            else Cell(self.initial_cell["type"], None)
        )
        previous_cell = self.initial_cell
        for cell_ in self.cells[1:]:
            new_cell = (
                Property(**cell_)
                if cell_["type"] == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                else Cell(cell_["type"], None)
            )
            previous_cell.next_cell = new_cell
            previous_cell = new_cell
        new_cell.next_cell = self.initial_cell

    def __str__(self):
        return f"{{id:{self.id}, is_game_started: {'Yes' if not self.is_game_over else 'No'}, number_of_players_in: {len(self.player_list)}}}"

    def attach(self, user, callback, turncb):
        """
        Users attach to a board and Player object instance created according to
        specified user object.
        """
        if len(self.player_list) < 4:
            # Do the attach action here
            new_player = Player(user, self, self.initial_cell, self.startup)
            self.player_list.append(new_player)
            return new_player
        else:
            return "Board is full, no room for a new player!"

    def detach(self, player):
        """
        Users deatches to a board with this method and if there is only one
        attached player left, game automatically ends.
        """
        if player in self.player_list:
            self.player_list.remove(player)
            for p in player.properties_owned:
                p.level = 0
                p.owner = None
            if CHANCE_CHOICES[CHANCE_JAIL_FREE][1] in player.chance_cards_owned:
                self.put_card_back(CHANCE_CHOICES[CHANCE_JAIL_FREE][1])
        if len(self.player_list) < 2:
            self.is_game_over = True
        return f"{player.user.username} has left the game!"

    def ready(self, player):
        """
        Setting player status ready. After each player sets itself
        as ready, game starts.
        """
        index_ = self.player_list.index(player)
        self.player_list[index_].status = USER_STATUS_READY

        is_all_ready = (
            len(
                list(
                    filter(
                        lambda player: player.status == USER_STATUS_READY,
                        self.player_list,
                    )
                )
            )
            == 4
        )
        if is_all_ready:
            self.is_game_over = False

    def turn(self, player, command, extra_field={}):
        """
        This method is where all the magic happens. Every player interaction (commands, chance cards)
        is evaluated here.
        First,
        """
        pname = player.user.username
        if self.player_list.index(player) != self.turn_:
            return f"{pname}, it's not your turn!"
        elif not self.is_game_over:
            result_text = None

            if player.enough_is_enough == USER_ACTION_STATUS_MUST_PICK_AND_TELEPORT:
                """
                If player gets on a teleport cell or picks a teleport card,
                it can only use PICK or TELEPORT commands
                """
                right_command = (
                    command == COMMAND_CHOICES[COMMAND_PICK][1]
                    or command == COMMAND_CHOICES[COMMAND_TELEPORT][1]
                )
                if not right_command:
                    return f"{pname}, you have to pick and teleport. other commands are not available!"
            elif player.enough_is_enough == USER_ACTION_STATUS_CAN_PASS:
                """
                Restricts player to use Roll command only once
                """
                if command == COMMAND_CHOICES[COMMAND_ROLL][1]:
                    return f"{pname}, you have already used Roll command!"

            # After this line, the command sent from player is processed.

            if command == COMMAND_CHOICES[COMMAND_PASS][1]:
                """
                If player does not want to do anything more after rolling,
                it can pass the turn to the other player.
                """
                if player.enough_is_enough == USER_ACTION_STATUS_MUST_ROLL:
                    return f"{pname}, you have to roll the dice first!"
                result_text = f"End of turn for {pname}!"

            elif player.in_jail and command == COMMAND_CHOICES[COMMAND_BAIL][1]:
                """
                If player is in jail, pays the price and gets out.
                """
                if player.balance >= self.jailbail:
                    player.in_jail = False
                    player.balance -= self.jailbail
                    result_text = f"{pname} paid ${self.jailbail} and got out of jail!"
                    player.enough_is_enough = USER_ACTION_STATUS_MUST_PASS
                elif player.balance < self.jailbail:
                    return "Insufficient funds to pay jailbail!"

            elif command == COMMAND_CHOICES[COMMAND_ROLL][1]:
                """
                With this command, player rolls the dice and moves onthe board.
                According to last position of the user (jail, chance, tax, etc.) some action
                is taken by the board.
                """
                first_dice, second_dice = roll_dice()
                # This section after rolling the dice is only for testing purposes. In test cases, -----
                # we forced user to move with predetermined dice results to build a strict scenerio to -
                # test all cases without randomness ----------------------------------------------------
                if "first_dice" in extra_field:
                    first_dice, second_dice = extra_field.get(
                        "first_dice"
                    ), extra_field.get("second_dice")
                # --------------------------------------------------------------------------------------
                if player.in_jail:
                    # Checks user gets double dice to get out from jail
                    player.in_jail = first_dice != second_dice
                    self.turn_ = (self.turn_ + 1) % len(self.player_list)
                    return (
                        f"{pname}, you are still in jail."
                        if player.in_jail
                        else f"{pname}, you're out of jail!"
                    )
                player.enough_is_enough = USER_ACTION_STATUS_CAN_PASS
                result_text = f"Player {player} threw {first_dice}-{second_dice}!"
                # Player moves according to the dice results.
                player.move(first_dice + second_dice)
                # After player moved, apply tax-jail-rent-chance actions
                if player.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_JAIL][1]:
                    player.put_into_jail()
                    player.enough_is_enough = USER_ACTION_STATUS_MUST_PASS
                elif player.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_TAX][1]:
                    player.pay(self.tax)
                    player.enough_is_enough = USER_ACTION_STATUS_MUST_PASS
                    result_text = f"{player.user.username} paid tax (${self.tax})."
                elif (
                    player.board_position.type == CELL_TYPE_CHOICES[CELL_TYPE_CHANCE][1]
                ):
                    """
                    Player picks a card and relevant action happend according to type of the card
                    """
                    pulled_card = self.draw_a_card()
                    if pulled_card == CHANCE_CHOICES[CHANCE_JAIL_FREE][1]:
                        player.chance_cards_owned.append(pulled_card)
                    elif pulled_card == CHANCE_CHOICES[CHANCE_GOTO_JAIL][1]:
                        player.put_into_jail()
                        self.put_card_back(pulled_card)
                    elif pulled_card == CHANCE_CHOICES[CHANCE_LOTTERY][1]:
                        player.balance += self.lottery
                        self.put_card_back(pulled_card)
                    elif pulled_card == CHANCE_CHOICES[CHANCE_TAX][1]:
                        player.pay(self.tax)
                        self.put_card_back(pulled_card)
                    elif pulled_card == CHANCE_CHOICES[CHANCE_TELEPORT][1]:
                        player.enough_is_enough = (
                            USER_ACTION_STATUS_MUST_PICK_AND_TELEPORT
                        )
                        self.put_card_back(pulled_card)
                        return f"{pname}, you picked teleport card. Pick a property and teleport!"
                    elif (
                        pulled_card == CHANCE_CHOICES[CHANCE_UPGRADE][1]
                        or pulled_card == CHANCE_CHOICES[CHANCE_DOWNGRADE][1]
                        or pulled_card == CHANCE_CHOICES[CHANCE_COLOR_UPGRADE][1]
                        or pulled_card == CHANCE_CHOICES[CHANCE_COLOR_DOWNGRADE][1]
                    ):
                        player.chance_cards_owned.append(pulled_card)
                        return f"{pname}, you picked a chance card ({pulled_card})!"
                elif (
                    player.board_position.type
                    == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                ):
                    """
                    If player's last location is owned by someone else, it pays rent.
                    Otherwise player has the option to buy it.
                    """
                    if (
                        player.board_position.owner
                        and player.board_position.owner.user.id != player.user.id
                    ):
                        player.pay(
                            player.board_position.get_rent(),
                            player.board_position.owner,
                        )
                        result_text = f"{pname} has paid rent to {player.board_position.owner.user.username}."
                    else:
                        return f"{pname} is on the property named {player.board_position.name}"
                elif (
                    player.board_position.type
                    == CELL_TYPE_CHOICES[CELL_TYPE_TELEPORT][1]
                ):
                    """
                    After getting on a teleport cell, players action status changes.
                    Player MUST pick and teleport onto a property.
                    """
                    player.enough_is_enough = USER_ACTION_STATUS_MUST_PICK_AND_TELEPORT
                    return f"{pname}, you are now on the teleport cell. You have to pick a property to teleport!"

            elif command == COMMAND_CHOICES[COMMAND_BUY][1]:
                """
                Simple buy command for property type of cells. If some condisitons are satisfied,
                player owns the property by paying the price.
                """
                cell = player.board_position
                if cell.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]:
                    property_price = cell.price
                    if player.balance >= property_price:
                        player.balance -= property_price
                        cell.owner = player
                        player.properties_owned.append(cell)
                        player.enough_is_enough = USER_ACTION_STATUS_MUST_PASS
                        result_text = f"{pname} bought property named '{cell.name}'"
                    else:
                        return "Insufficient funds to buy this property!"
                else:
                    return "There is nothing to buy here!"

            elif command == COMMAND_CHOICES[COMMAND_UPGRADE][1]:
                """
                Upgrade command to use on a property. There are three different cases for upgrade.
                1- Upgrading a cell after picking it with a chance card.
                2- Upgrading a group of (same color) properties with a chance card.
                3- Upgrading the property which players is currently on.
                """
                if (
                    player.picked_cell
                    and CHANCE_CHOICES[CHANCE_UPGRADE][1] in player.chance_cards_owned
                ):
                    self.put_card_back(CHANCE_CHOICES[CHANCE_UPGRADE][1], player)
                    upgraded_cell = self.initial_cell
                    while True:
                        if (
                            upgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and upgraded_cell.name == player.picked_cell
                        ):
                            upgraded_cell.upgrade()
                            break
                        upgraded_cell = upgraded_cell.next_cell
                    player.picked_cell = None
                    result_text = (
                        f"{pname} has upgraded property with name {player.picked_cell}"
                    )
                elif (
                    player.picked_cell
                    and CHANCE_CHOICES[CHANCE_COLOR_UPGRADE][1]
                    in player.chance_cards_owned
                ):
                    self.put_card_back(CHANCE_CHOICES[CHANCE_COLOR_UPGRADE][1], player)
                    upgraded_cell = self.initial_cell
                    color, amount = None, 0
                    while True:
                        if (
                            upgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and upgraded_cell.name == player.picked_cell
                        ):
                            color, amount = upgraded_cell.color, upgraded_cell.cell
                            break
                        upgraded_cell = upgraded_cell.next_cell
                    while amount > 0:
                        if (
                            upgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and upgraded_cell.color == color
                        ):
                            upgraded_cell.upgrade()
                            amount -= 1
                        upgraded_cell = upgraded_cell.next_cell
                    player.picked_cell = None
                    result_text = (
                        f"{pname} has upgraded all properties with color {color}"
                    )
                elif (
                    player.board_position.type
                    == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                ):
                    cell = player.board_position
                    if cell.level == 4:
                        return f"Property '{cell.name}' is already maxed out!"
                    elif "upgrade" in player.chance_cards_owned:
                        cell.upgrade()
                    elif player.balance >= self.upgrade:
                        cell.upgrade()
                        player.balance -= self.upgrade
                    else:
                        return f"{pname}, you don't have enough funds to upgrade this property!"
                    result_text = f"Property '{cell.name}' has been upgraded."
                    player.enough_is_enough = USER_ACTION_STATUS_MUST_PASS
                else:
                    return "There is nothing to upgrade here!"

            elif command == COMMAND_CHOICES[COMMAND_DOWNGRADE][1]:
                """
                Downgrade command to use on a property. There are two different cases for upgrade.
                1- Downgrading a cell after picking it with a chance card.
                2- Downgrading a group of (same color) properties with a chance card.
                """
                if (
                    player.picked_cell
                    and CHANCE_CHOICES[CHANCE_DOWNGRADE][1] in player.chance_cards_owned
                ):
                    self.put_card_back(CHANCE_CHOICES[CHANCE_DOWNGRADE][1], player)
                    downgraded_cell = self.initial_cell
                    while True:
                        if (
                            downgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and downgraded_cell.name == player.picked_cell
                        ):
                            downgraded_cell.downgrade()
                            break
                        downgraded_cell = downgraded_cell.next_cell
                    player.picked_cell = None
                    result_text = (
                        f"{pname} upgraded property with name {player.picked_cell}"
                    )
                elif (
                    player.picked_cell
                    and CHANCE_CHOICES[CHANCE_COLOR_DOWNGRADE][1]
                    in player.chance_cards_owned
                ):
                    self.put_card_back(
                        CHANCE_CHOICES[CHANCE_COLOR_DOWNGRADE][1], player
                    )
                    downgraded_cell = self.initial_cell
                    color, amount = None, 0
                    while True:
                        if (
                            downgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and downgraded_cell.name == player.picked_cell
                        ):
                            color, amount = downgraded_cell.color, downgraded_cell.cell
                            break
                        downgraded_cell = downgraded_cell.next_cell
                    while amount > 0:
                        if (
                            downgraded_cell.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and downgraded_cell.color == color
                        ):
                            downgraded_cell.downgrade()
                            amount -= 1
                        downgraded_cell = downgraded_cell.next_cell
                    player.picked_cell = None
                    result_text = (
                        f"{pname} has downgraded all the properties with color {color}!"
                    )

            elif command == COMMAND_CHOICES[COMMAND_PICK][1]:
                """
                Pick a 'property' with its name.
                """
                picked_location = extra_field.get("picked_location", None)
                if picked_location:
                    for cll in self.cells:
                        if (
                            cll["type"] == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and cll["name"] == extra_field["picked_location"]
                        ):
                            player.picked_cell = picked_location
                            return f"{pname} you have picked the property named '{extra_field}'!"
                    return f"Could not find property with name {extra_field}!"
                else:
                    return f"{pname}, you cannot pick cell types other than properties!"

            elif command == COMMAND_CHOICES[COMMAND_TELEPORT][1]:
                """
                Teleports the player onto a picked property.
                """
                if player.picked_cell is None:
                    return f"{pname}, you should pick a cell to teleport first!"
                elif (
                    player.enough_is_enough == USER_ACTION_STATUS_MUST_PICK_AND_TELEPORT
                ):
                    while True:
                        if (
                            player.board_position.type
                            == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]
                            and player.board_position.name == player.picked_cell
                        ):
                            break
                        else:
                            player.move(is_teleported=True)
                    player.enough_is_enough = USER_ACTION_STATUS_CAN_PASS
                    return f"{pname} has teleported onto a property named '{player.picked_cell}'"
                else:
                    return f"{pname}, you don't have a teleport card!"

            self.turn_ = (self.turn_ + 1) % len(self.player_list)
            player.enough_is_enough = USER_ACTION_STATUS_MUST_ROLL
            return result_text
        else:
            return "Game is over!"

    def getboardstate(self):
        """
        Returns the entire board state for that specific time with all
        properties and player informations.
        """
        dummy_cell = self.initial_cell

        players = []
        for player in self.player_list:
            players.append(player.get_user_state())

        properties = []
        for _ in range(len(self.cells)):
            if dummy_cell.type == CELL_TYPE_CHOICES[CELL_TYPE_PROPERTY][1]:
                properties.append(dummy_cell.get_propert_state())
            dummy_cell = dummy_cell.next_cell

        data = dict()
        data["players"] = players
        data["properties"] = properties

        return json.dumps(data)

    # EXTRA ----------------------------------------------------------------------------
    def draw_a_card(self):
        """
        Picks a chance card from the deck.
        """
        chance_card = self.chances_cards[0]
        self.chances_cards = self.chances_cards[1:]
        return chance_card

    def put_card_back(self, card, player=None):
        """
        Puts the chance card back into the deck after its effect is applied.
        """
        self.chances_cards.append(card)
        if player and card in player.chance_cards_owned:
            player.chance_cards_owned.remove(card)

    def shuffle_deck(self):
        deck_size = len(self.chances)
        indices = [index for index in range(deck_size)]
        for i in range(random.randint(deck_size // 2, deck_size)):
            i = random.choice(indices)
            j = random.choice(indices)
            self.chances_cards[i], self.chances_cards[j] = (
                self.chances_cards[j],
                self.chances_cards[i],
            )

    def game_restart(self):
        pass

    # ----------------------------------------------------------------------------------
