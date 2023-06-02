from mp2 import Mp2Client, tokenize_command
from validators import *

AUTH_SELLER = None
ANON_SELLER = "ANONYMOUS"
POSTGRESQL_CONFIG_FILE_NAME = "database.cfg"


def print_success_msg(message):
    print(message)


def print_error_msg(message):
    print("ERROR: %s" % message)


def print_seller_info(seller):
    global ANON_SELLER

    if seller:
        print(seller, end=" > ")
    else:
        print(ANON_SELLER, end=" > ")


def main():
    global AUTH_SELLER, POSTGRESQL_CONFIG_FILE_NAME

    client = Mp2Client(config_filename=POSTGRESQL_CONFIG_FILE_NAME)
    client.help()

    while True:
        # print customer information if signed in
        print_seller_info(seller=AUTH_SELLER)

        # get new command from user
        cmd_text = input()
        cmd_tokens = tokenize_command(cmd_text)
        cmd = cmd_tokens[0]

        if cmd == "help":
            client.help()

        elif cmd == "sign_up":
            # validate command
            validation_result, validation_message = sign_up_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                client.connect()
                _, arg_seller_id, arg_sub_key, arg_zip, arg_city, arg_state, arg_plan_id = cmd_tokens

                # sign up
                exec_status, exec_message = client.sign_up(seller_id=arg_seller_id, sub_key=arg_sub_key,
                                                            zip=arg_zip, city=arg_city,
                                                            state=arg_state,
                                                            plan_id=arg_plan_id)

                client.disconnect()

                # print message
                if exec_status:
                    print_success_msg(exec_message)
                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "sign_in":
            # validate command
            validation_result, validation_message = sign_in_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                _, arg_seller_id, arg_sub_key = cmd_tokens

                client.connect()
                seller, exec_message = client.sign_in(seller_id=arg_seller_id, sub_key=arg_sub_key)

                if seller:
                    AUTH_SELLER = seller
                    print_success_msg(exec_message)

                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "sign_out":

            # validate command
            validation_result, validation_message = basic_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.sign_out(seller=AUTH_SELLER)

                if exec_status:
                    AUTH_SELLER = None
                    client.disconnect()
                    print_success_msg(exec_message)

                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "quit":

            # validate command
            validation_result, validation_message = quit_validator(cmd_tokens)

            if validation_result:

                exec_status, exec_message = client.quit(seller=AUTH_SELLER)

                if exec_status:
                    break
                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "show_plans":
            # validate command
            validation_result, validation_message = basic_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.show_plans()

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)
        
        elif cmd == "show_subscription":
            # validate command
            validation_result, validation_message = basic_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.show_subscription(seller=AUTH_SELLER)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)
        
        elif cmd == "change_stock":
            # validate command
            validation_result, validation_message = change_stock_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                _, arg_product_id, arg_op, arg_change = cmd_tokens

                if arg_op == "add":
                    arg_change = int(arg_change)
                elif arg_op == "remove":
                    arg_change = -1 * int(arg_change)
                else:
                    print_error_msg(messages.CMD_UNDEFINED)

                exec_status, exec_message = client.change_stock(seller=AUTH_SELLER, product_id=arg_product_id, change_amount=arg_change)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "show_quota":
            # validate command
            validation_result, validation_message = basic_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.show_quota(seller=AUTH_SELLER)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "subscribe":
            # validate command
            validation_result, validation_message = subscribe_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                _, arg_plan_id = cmd_tokens

                seller, exec_message = client.subscribe(seller=AUTH_SELLER, plan_id=arg_plan_id)

                if seller:
                    AUTH_SELLER = seller
                    print_success_msg(exec_message)

                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "ship":

            # validate command
            validation_result, validation_message = ship_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.ship(seller=AUTH_SELLER, product_ids=cmd_tokens[1:])

                if exec_status:
                    print_success_msg(exec_message)
                else:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "calc_gross":
            # validate command
            validation_result, validation_message = basic_validator(AUTH_SELLER, cmd_tokens)

            if validation_result:
                exec_status, exec_message = client.calc_gross(seller=AUTH_SELLER)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)

        elif cmd == "show_cart":
            client.connect()
            # validate command
            validation_result, validation_message = show_cart_validator(cmd_tokens)

            if validation_result:
                _, arg_customer_id = cmd_tokens

                exec_status, exec_message = client.show_cart(customer_id=arg_customer_id)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)
            
            client.disconnect()
        
        elif cmd == "change_cart":
            client.connect()

            # validate command
            validation_result, validation_message = change_cart_validator( cmd_tokens)

            if validation_result:
                _, arg_customer_id, arg_product_id, arg_seller_id, arg_op, arg_change = cmd_tokens

                if arg_op == "add":
                    arg_change = int(arg_change)
                elif arg_op == "remove":
                    arg_change = -1 * int(arg_change)
                else:
                    print_error_msg(messages.CMD_UNDEFINED)

                exec_status, exec_message = client.change_cart(customer_id=arg_customer_id, product_id=arg_product_id, seller_id=arg_seller_id, change_amount=arg_change)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)
                
            client.disconnect()

        elif cmd == "purchase_cart":
            client.connect()
            # validate command
            validation_result, validation_message = purchase_cart_validator(cmd_tokens)

            if validation_result:
                _, arg_customer_id = cmd_tokens

                exec_status, exec_message = client.purchase_cart(customer_id=arg_customer_id)

                if not exec_status:
                    print_error_msg(exec_message)

            else:
                print_error_msg(validation_message)
            
            client.disconnect()

        elif cmd == "":
            pass

        else:
            print_error_msg(messages.CMD_UNDEFINED)


if __name__ == '__main__':
    main()
