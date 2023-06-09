from seller import Seller

import psycopg2
import uuid
from datetime import datetime, timezone

import configparser
from messages import *

"""
    Splits given command string by spaces and trims each token.
    Returns token list.
"""


def tokenize_command(command):
    tokens = command.split(" ")
    return [t.strip() for t in tokens]


class Mp2Client:
    def __init__(self, config_filename):
        cfg = configparser.ConfigParser()
        cfg.read(config_filename)
        self.db_conn_params = cfg["postgresql"]
        self.conn = None

    """
        Connects to PostgreSQL database and returns connection object.
    """

    def connect(self):
        self.conn = psycopg2.connect(**self.db_conn_params)
        self.conn.autocommit = False

    """
        Disconnects from PostgreSQL database.
    """

    def disconnect(self):
        self.conn.close()

    """
    Prints list of available commands of the software.
    """

    def help(self):
        # prints the choices for commands and parameters
        print("\n*** Please enter one of the following commands ***")
        print("> help")
        print(
            "> sign_up <seller_id> <subscriber_key> <zip_code> <city> <state> <plan_id>"
        )
        print("> sign_in <seller_id> <subscriber_key>")
        print("> sign_out")
        print("> show_plans")
        print("> show_subscription")
        print("> change_stock <product_id> <add or remove> <amount>")
        print("> show_quota")
        print("> subscribe <plan_id>")
        print("> ship <product_id_1> <product_id_2> <product_id_3> ... <product_id_n>")
        print("> calc_gross")
        print("> show_cart <customer_id>")
        print(
            "> change_cart <customer_id> <product_id> <seller_id> <add or remove> <amount>"
        )
        print("> purchase_cart <customer_id>")
        print("> quit")

    def sign_up(self, seller_id, sub_key, zip, city, state, plan_id):
        """
        Saves seller with given details.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        """
        try:
            cursor = self.conn.cursor()

            cursor.execute(
                "SELECT seller_id FROM sellers WHERE seller_id = %s;", (seller_id,)
            )
            if cursor.fetchone() is not None:
                return False, CMD_EXECUTION_FAILED
            cursor.execute(
                "SELECT plan_id FROM subscription_plans WHERE plan_id = %s;",
                (plan_id,),
            )
            if cursor.fetchone() is None:
                return False, CMD_EXECUTION_FAILED
            cursor.execute(
                "INSERT INTO sellers (seller_id, seller_zip_code_prefix, seller_city, seller_state) VALUES (%s, %s, %s, %s);",
                (seller_id, zip, city, state),
            )
            self.conn.commit()
            cursor.execute(
                "INSERT INTO seller_subscription (seller_id, subscriber_key, session_count, plan_id) VALUES (%s, %s, %s, %s);",
                (seller_id, sub_key, 0, plan_id),
            )
            self.conn.commit()

            cursor.close()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def sign_in(self, seller_id, sub_key):
        """
        Retrieves seller information if seller_id and subscriber_key is correct and seller's session_count < max_parallel_sessions.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If seller_id or subscriber_key is wrong, return tuple (None, USER_SIGNIN_FAILED).
        - If session_count < max_parallel_sessions, commit changes (increment session_count) and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If session_count >= max_parallel_sessions, return tuple (None, USER_ALL_SESSIONS_ARE_USED).
        - If any exception occurs; rollback, do nothing on the database and return tuple (None, USER_SIGNIN_FAILED).
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT seller_id, subscriber_key, session_count, plan_id FROM seller_subscription WHERE seller_id = %s AND subscriber_key = %s;",
                (seller_id, sub_key),
            )
            querySeller = cursor.fetchone()
            if querySeller is None:
                return None, USER_SIGNIN_FAILED

            cursor.execute(
                "SELECT max_parallel_sessions FROM subscription_plans WHERE plan_id = %s;",
                (querySeller[3],),
            )
            querySellerPlan = cursor.fetchone()

            if querySeller[2] >= querySellerPlan[0]:
                return None, USER_ALL_SESSIONS_ARE_USED
            else:
                cursor.execute(
                    """
                    UPDATE seller_subscription
                    SET session_count = session_count + 1
                    WHERE seller_id = %s;
                    """,
                    (seller_id,),
                )
                self.conn.commit()
                cursor.close()
                return (
                    Seller(
                        querySeller[0],
                        querySeller[2],
                        querySeller[3],
                    ),
                    CMD_EXECUTION_SUCCESS,
                )
        except Exception as e:
            return None, USER_SIGNIN_FAILED

    def sign_out(self, seller):
        """
        Signs out from given seller's account.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - Decrement session_count of the seller in the database.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT seller_id FROM sellers WHERE seller_id = %s;",
                (seller.seller_id,),
            )
            querySeller = cursor.fetchone()
            if querySeller is None or querySeller[0] == 0:
                return False, CMD_EXECUTION_FAILED

            cursor.execute(
                "UPDATE seller_subscription SET session_count = session_count - 1 WHERE seller_id = %s;",
                (seller.seller_id,),
            )
            self.conn.commit()
            cursor.close()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def quit(self, seller):
        """
        Quits from program.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - Remember to sign authenticated user out first.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        """
        if seller:
            res, msg = self.sign_out(seller)
            if res:
                return True, CMD_EXECUTION_SUCCESS
            else:
                return False, CMD_EXECUTION_FAILED
        return True, CMD_EXECUTION_SUCCESS

    def show_plans(self):
        """
        Retrieves all available plans and prints them.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print available plans and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        Output should be like:
        #|Name|Max Sessions|Max Stocks Per Product
        1|Basic|2|4
        2|Advanced|4|8
        3|Premium|6|12
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT plan_id, plan_name, max_parallel_sessions, max_stock_per_product FROM subscription_plans;"
            )
            queryPlans = cursor.fetchall()
            if queryPlans is None:
                return False, CMD_EXECUTION_FAILED
            print("#|Name|Max Sessions|Max Stocks Per Product")
            for row in queryPlans:
                print(f"{row[0]}|{row[1]}|{row[2]}|{row[3]}")
            cursor.close()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def show_subscription(self, seller):
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT plan_id FROM seller_subscription WHERE seller_id = %s;",
                (seller.seller_id,),
            )
            querySellerPlanId = cursor.fetchall()
            if querySellerPlanId is None:
                return False, CMD_EXECUTION_FAILED
            else:
                cursor.execute(
                    "SELECT plan_id, plan_name, max_parallel_sessions, max_stock_per_product FROM subscription_plans WHERE plan_id = %s;",
                    (querySellerPlanId[0],),
                )
                queryPlan = cursor.fetchone()
                if queryPlan is None:
                    return False, CMD_EXECUTION_FAILED
                print("#|Name|Max Sessions|Max Stocks Per Product")
                print(f"{queryPlan[0]}|{queryPlan[1]}|{queryPlan[2]}|{queryPlan[3]}")
                cursor.close()
                return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def change_stock(self, seller, product_id, change_amount):
        """
        Change stock count of a product.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If target product does not exist on the database, return tuple (False, PRODUCT_NOT_FOUND).
        - If target stock count > current plan's max_stock_per_product, return tuple (False, QUOTA_LIMIT_REACHED).
        - If the operation is successful, commit changes and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        """
        try:
            cursor = self.conn.cursor()

            # Check if product_id is valid.
            cursor.execute(
                "SELECT product_id FROM products WHERE product_id = %s;",
                (product_id,),
            )
            queryProductId = cursor.fetchall()
            if not queryProductId:
                return False, PRODUCT_NOT_FOUND

            # Get seller max stock per product.
            cursor.execute(
                "SELECT max_stock_per_product FROM subscription_plans WHERE plan_id = %s;",
                (seller.plan_id,),
            )
            queryMaxProductAmount = cursor.fetchone()
            max_product_per_stock = queryMaxProductAmount[0]

            cursor.execute(
                "SELECT product_id, stock_count FROM seller_stocks WHERE product_id = %s AND seller_id = %s;",
                (
                    product_id,
                    seller.seller_id,
                ),
            )
            queryStockCount = cursor.fetchone()
            # For remove operations
            if change_amount < 0:
                # Stock data does not exists in table
                if not queryStockCount:
                    cursor.close()
                    return False, STOCK_UPDATE_FAILURE
                queryStockCount = int(queryStockCount[1])
                if queryStockCount + change_amount < 0:
                    # Stock amount gets 0
                    cursor.close()
                    return False, STOCK_UPDATE_FAILURE
                else:
                    # Calculate successfully
                    cursor.execute(
                        "UPDATE seller_stocks SET stock_count = %s WHERE seller_id = %s AND product_id = %s;",
                        (queryStockCount + change_amount, seller.seller_id, product_id),
                    )
                    self.conn.commit()
                    cursor.close()
                    return True, CMD_EXECUTION_SUCCESS
            # For addition operations
            else:
                if not queryStockCount:
                    if change_amount > max_product_per_stock:
                        cursor.close()
                        return False, QUOTA_LIMIT_REACHED
                    else:
                        cursor.execute(
                            "INSERT INTO seller_stocks (seller_id, product_id, stock_count) VALUES (%s, %s, %s);",
                            (seller.seller_id, product_id, change_amount),
                        )
                        self.conn.commit()
                        cursor.close()
                        return True, CMD_EXECUTION_SUCCESS
                else:
                    queryStockCount = int(queryStockCount[1])
                    if queryStockCount + change_amount > max_product_per_stock:
                        cursor.close()
                        return False, QUOTA_LIMIT_REACHED
                    else:
                        # Calculate successfully
                        cursor.execute(
                            "UPDATE seller_stocks SET stock_count = %s WHERE seller_id = %s AND product_id = %s;",
                            (
                                queryStockCount + change_amount,
                                seller.seller_id,
                                product_id,
                            ),
                        )
                        self.conn.commit()
                        cursor.close()
                        return True, CMD_EXECUTION_SUCCESS

        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def show_quota(self, seller):
        """
        Retrieves authenticated seller's remaining quota for stocks and prints it.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print the authenticated seller's quota and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        If the seller is subscribed to a plan with max_stock_per_product = 12 and
        the current stock for product 92bf5d2084dfbcb57d9db7838bac5cd0 is 10, then output should be like:

        Product Id|Remaining Quota
        92bf5d2084dfbcb57d9db7838bac5cd0|2

        If the seller does not have a stock, print 'Quota limit is not activated yet.'
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT product_id, stock_count FROM seller_stocks WHERE seller_id = %s",
                (seller.seller_id,),
            )
            queryQuota = cursor.fetchall()
            cursor.close()
            if not queryQuota:
                return False, QUOTA_INACTIVE
            else:
                print("Product Id|Remaining Quota")
                for row in queryQuota:
                    print(f"{row[0]}|{row[1]}")
                return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def subscribe(self, seller, plan_id):
        """
        Subscribe authenticated seller to new plan.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If target plan does not exist on the database, return tuple (None, PRODUCT_NOT_FOUND).
        - If the new plan's max_parallel_sessions < current plan's max_parallel_sessions, return tuple (None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE).
        - If the operation is successful, commit changes and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (None, CMD_EXECUTION_FAILED).
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT seller_id FROM sellers WHERE seller_id = %s",
                (seller.seller_id,),
            )
            querySeller = cursor.fetchone()
            if querySeller is None:
                return None, CMD_EXECUTION_FAILED
            else:
                cursor.execute(
                    "SELECT max_parallel_sessions FROM subscription_plans WHERE plan_id = %s",
                    (plan_id,),
                )
                queryNewPlanMaxSessions = cursor.fetchone()
                if queryNewPlanMaxSessions is None:
                    return None, SUBSCRIBE_PLAN_NOT_FOUND
                else:
                    cursor.execute(
                        "SELECT max_parallel_sessions FROM subscription_plans WHERE plan_id = %s",
                        (seller.plan_id,),
                    )
                    queryOldPlanMaxSessions = cursor.fetchone()
                    if queryOldPlanMaxSessions is None:
                        return None, CMD_EXECUTION_FAILED
                    else:
                        if queryNewPlanMaxSessions[0] < queryOldPlanMaxSessions[0]:
                            return None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE
                        else:
                            cursor.execute(
                                "UPDATE seller_subscription SET plan_id = %s WHERE seller_id = %s;",
                                (plan_id, seller.seller_id),
                            )
                            self.conn.commit()
                            cursor.close()
                            seller.plan_id = plan_id
                            return seller, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def ship(self, seller, product_ids):
        """
        Change stock amounts for multiple distinct products.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If everything is OK and the operation is successful, return (True, CMD_EXECUTION_SUCCESS).
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any one of the product ids is incorrect; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - If any one of the products is not in the stock; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        """
        try:
            cursor = self.conn.cursor()
            # Check all valid
            for product_id in product_ids:
                cursor.execute(
                    "SELECT * FROM products WHERE product_id = %s;",
                    (product_id,),
                )
                if not cursor.fetchone():
                    return False, CMD_EXECUTION_FAILED

                cursor.execute(
                    "SELECT product_id FROM seller_stocks WHERE seller_id = %s AND product_id = %s AND stock_count <> 0;",
                    (
                        seller.seller_id,
                        product_id,
                    ),
                )
                if not cursor.fetchone():
                    return False, CMD_EXECUTION_FAILED
            # All valid -------------------
            for product_id in product_ids:
                cursor.execute(
                    "UPDATE seller_stocks SET stock_count = stock_count - 1 WHERE seller_id = %s AND product_id = %s;",
                    (
                        seller.seller_id,
                        product_id,
                    ),
                )
                self.conn.commit()

            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def calc_gross(self, seller):
        """
        Retrieves the gross income per product category for every month.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print the results and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        Output should be like:
        Gross Income|Year|Month
        123.45|2018|1
        67.8|2018|2
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT SUM(oi.price), DATE_TRUNC('year', o.order_purchase_timestamp), DATE_TRUNC('month', o.order_purchase_timestamp) FROM orders o, order_items oi WHERE oi.order_id = o.order_id AND oi.seller_id = %s GROUP BY DATE_TRUNC('year', o.order_purchase_timestamp), DATE_TRUNC('month', o.order_purchase_timestamp);",
                (seller.seller_id,),
            )
            queryGrossTotal = cursor.fetchall()
            cursor.close()
            if not queryGrossTotal:
                print("Gross Income: 0")
            else:
                print("Gross Income|Year|Month")
                for row in queryGrossTotal:
                    print(
                        f"{row[0]}|{str(row[1]).split('-')[0]}|{str(row[2]).split('-')[1].lstrip('0')}"
                    )
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def show_cart(self, customer_id):
        """
        Retrieves items on the customer's shopping cart
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print items on the cart and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        Output should be like:
        Seller Id|Product Id|Amount
        dd7ddc04e1b6c2c614352b383efe2d36|e5f2d52b802189ee658865ca93d83a8f|2
        5b51032eddd242adc84c38acab88f23d|c777355d18b72b67abbeef9df44fd0fd|3
        df560393f3a51e74553ab94004ba5c87|ac6c3623068f30de03045865e4e10089|1
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT customer_id FROM customers WHERE customer_id = %s;",
                (customer_id,),
            )
            queryCustomerId = cursor.fetchone()
            if not queryCustomerId:
                cursor.close()
                return False, CUSTOMER_NOT_FOUND
            else:
                cursor.execute(
                    "SELECT seller_id, product_id, amount FROM customer_carts WHERE customer_id = %s;",
                    (customer_id,),
                )
                queryCart = cursor.fetchall()
                print("Seller Id|Product Id|Amount")
                for row in queryCart:
                    print(f"{row[0]}|{row[1]}|{row[2]}")
                cursor.close()
                return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def change_cart(self, customer_id, product_id, seller_id, change_amount):
        """
        Change count of items in shopping cart
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If customer does not exist on the database, return tuple (False, CUSTOMER_NOT_FOUND).
        - If target product does not exist on the database, return tuple (False, PRODUCT_NOT_FOUND).
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - Consider stocks of sellers when you add items to the cart.
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT customer_id FROM customers WHERE customer_id = %s;",
                (customer_id,),
            )
            queryCustomerId = cursor.fetchone()
            if not queryCustomerId:
                cursor.close()
                return False, CUSTOMER_NOT_FOUND
            else:
                cursor.execute(
                    "SELECT product_id FROM products WHERE product_id = %s;",
                    (product_id,),
                )
                queryProductId = cursor.fetchone()
                if not queryProductId:
                    cursor.close()
                    return False, PRODUCT_NOT_FOUND
                else:
                    cursor.execute(
                        "SELECT seller_id, stock_count FROM seller_stocks WHERE seller_id = %s AND product_id = %s;",
                        (
                            seller_id,
                            product_id,
                        ),
                    )
                    stock = cursor.fetchone()
                    stock_count = stock[1]
                    cursor.execute(
                        "SELECT amount FROM customer_carts WHERE customer_id = %s AND seller_id = %s AND product_id = %s;",
                        (customer_id, seller_id, product_id),
                    )
                    cartQuery = cursor.fetchone()
                    # Remove case
                    if change_amount < 0:
                        if not cartQuery:
                            return False, CMD_EXECUTION_FAILED
                        current_amount_in_cart = cartQuery[0]
                        if change_amount + current_amount_in_cart <= 0:
                            # Remove row from seller_stocks
                            cursor.execute(
                                "DELETE FROM customer_carts WHERE customer_id = %s AND seller_id = %s AND product_id = %s;",
                                (
                                    customer_id,
                                    seller_id,
                                    product_id,
                                ),
                            )
                        else:
                            # Decrease
                            cursor.execute(
                                "UPDATE customer_carts SET amount = %s WHERE customer_id = %s AND seller_id = %s AND product_id = %s;",
                                (
                                    change_amount + current_amount_in_cart,
                                    customer_id,
                                    seller_id,
                                    product_id,
                                ),
                            )
                        self.conn.commit()
                        cursor.close()
                        return True, CMD_EXECUTION_SUCCESS
                    # Addition case
                    else:
                        if change_amount > stock_count:
                            return False, STOCK_UNAVAILABLE
                        else:
                            if not cartQuery:
                                # Add row
                                cursor.execute(
                                    "INSERT INTO customer_carts (customer_id, seller_id, product_id, amount) VALUES (%s, %s, %s, %s);",
                                    (
                                        customer_id,
                                        seller_id,
                                        product_id,
                                        change_amount,
                                    ),
                                )
                                self.conn.commit()
                            else:
                                current_amount_in_cart = cartQuery[0]
                                # Update row
                                if current_amount_in_cart + change_amount > stock_count:
                                    return False, STOCK_UNAVAILABLE
                                cursor.execute(
                                    "UPDATE customer_carts SET amount = %s WHERE customer_id = %s AND seller_id = %s AND product_id = %s;",
                                    (
                                        change_amount + current_amount_in_cart,
                                        customer_id,
                                        seller_id,
                                        product_id,
                                    ),
                                )
                                self.conn.commit()
                            cursor.close()
                            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    def purchase_cart(self, customer_id):
        """
        Purchases items on the cart
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        Actions:
        - Change stocks on seller_stocks table
        - Remove entries from customer_carts table
        - Add entries to order_items table
        - Add single entry to order table
        """
        try:
            cursor = self.conn.cursor()
            cursor.execute(
                "SELECT customer_id FROM customers WHERE customer_id = %s;",
                (customer_id,),
            )
            queryCustomerId = cursor.fetchone()
            if not queryCustomerId:
                cursor.close()
                return False, CUSTOMER_NOT_FOUND
            else:
                cursor.execute(
                    "SELECT COUNT(cc.product_id) FROM customer_carts cc, seller_stocks ss WHERE cc.customer_id = %s AND cc.product_id = ss.product_id AND cc.amount > ss.stock_count;",
                    (customer_id,),
                )
                OutOfStockCount = cursor.fetchone()[0]
                if OutOfStockCount:
                    cursor.close()
                    return False, STOCK_UNAVAILABLE
                else:
                    cursor.execute(
                        "SELECT seller_id, product_id, amount FROM customer_carts WHERE customer_id = %s;",
                        (customer_id,),
                    )
                    queryDecreaseStocks = cursor.fetchall()
                    order_item_id = 1
                    order_id = "".join(str(uuid.uuid4()).split("-"))
                    date = (
                        datetime.utcnow().today().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]
                    )
                    cursor.execute(
                        "INSERT INTO orders (order_id, customer_id, order_status, order_purchase_timestamp, order_approved_at, order_delivered_carrier_date, order_delivered_customer_date, order_estimated_delivery_date) VALUES (%s, %s, NULL, TIMESTAMP %s, NULL, NULL, NULL, NULL);",
                        (
                            order_id,
                            customer_id,
                            date,
                        ),
                    )
                    self.conn.commit()
                    for row in queryDecreaseStocks:
                        # Decrease seller stocks
                        cursor.execute(
                            """
                            DELETE FROM customer_carts WHERE customer_id = %s AND product_id = %s AND seller_id = %s;
                            """,
                            (
                                customer_id,
                                row[1],
                                row[0],
                            ),
                        )
                        self.conn.commit()

                        # Remove from cart
                        cursor.execute(
                            """
                            UPDATE seller_stocks
                            SET stock_count = stock_count - %s
                            WHERE product_id = %s AND seller_id = %s;
                            """,
                            (
                                row[2],
                                row[1],
                                row[0],
                            ),
                        )
                        self.conn.commit()

                        for oi in range(row[2]):
                            cursor.execute(
                                "INSERT INTO order_items (order_id, order_item_id, product_id, seller_id, shipping_limit_date, price, freight_value) VALUES (%s, %s, %s, %s, NULL, NULL, NULL);",
                                (
                                    order_id,
                                    order_item_id,
                                    row[1],
                                    row[0],
                                ),
                            )
                            self.conn.commit()
                            cursor.execute(
                                """
                                SELECT *
                                FROM order_items
                                WHERE order_id = %s AND order_item_id = %s AND product_id = %s;
                                """,
                                (
                                    order_id,
                                    order_item_id,
                                    row[1],
                                ),
                            )
                            order_item_id += 1
                    cursor.close()
                return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED
