import messages


def sign_up_validator(auth_seller, cmd_tokens):
    if len(cmd_tokens) != 7:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 6

    if auth_seller:
        return False, messages.USER_ALREADY_SIGNED_IN

    return True, None


def sign_in_validator(auth_seller, cmd_tokens):
    if len(cmd_tokens) != 3:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 2

    if auth_seller:
        if auth_seller.seller_id == cmd_tokens[1]:
            return None, messages.USER_ALREADY_SIGNED_IN
        else:
            return None, messages.USER_OTHER_SIGNED_IN
    
    return True, None


"""
    This validator is basic validator that returns (True, None) 
    when a seller is authenticated and the number of command tokens is 1.
    Returns (False, <message>) otherwise.
"""


def basic_validator(auth_sellers, cmd_tokens):
    if auth_sellers:
        return True, None
    elif not auth_sellers and len(cmd_tokens) == 1:
        return False, messages.USER_NOT_AUTHORIZED
    else:
        return False, messages.CMD_INVALID_ARGS


def quit_validator(cmd_tokens):
    if len(cmd_tokens) == 1:
        return True, None
    else:
        return False, messages.CMD_INVALID_ARGS


def ship_validator(auth_seller, cmd_tokens):
    if not auth_seller:
        return False, messages.USER_NOT_AUTHORIZED
    elif len(cmd_tokens) <= 1:
        return False, messages.CMD_NOT_ENOUGH_ARGS_AT_LEAST % 1
    else:
        return True, None


def change_stock_validator(auth_seller, cmd_tokens):
    if not auth_seller:
        return False, messages.USER_NOT_AUTHORIZED
    elif len(cmd_tokens) == 4:
        return True, None
    else:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 3

def subscribe_validator(auth_seller, cmd_tokens):
    if not auth_seller:
        return False, messages.USER_NOT_AUTHORIZED
    elif len(cmd_tokens) == 2:
        return True, None
    else:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 1

def show_cart_validator(cmd_tokens):
    if len(cmd_tokens) == 2:
        return True, None
    else:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 1

def change_cart_validator(cmd_tokens):
    if len(cmd_tokens) == 6:
        return True, None
    else:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 5

def purchase_cart_validator(cmd_tokens):
    if len(cmd_tokens) == 2:
        return True, None
    else:
        return False, messages.CMD_NOT_ENOUGH_ARGS % 1
