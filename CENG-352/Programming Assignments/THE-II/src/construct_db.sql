CREATE TABLE IF NOT EXISTS product_category
(
	product_category_name VARCHAR(50),
	product_category_name_english VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS customers
(
	customer_id VARCHAR(32),
	customer_unique_id VARCHAR(32),
	customer_zip_code_prefix VARCHAR(5),
	customer_city VARCHAR(40),
	customer_state VARCHAR(3)
);

CREATE TABLE IF NOT EXISTS geolocation
(
	geolocation_zip_code_prefix VARCHAR(5),
	geolocation_lat DECIMAL,
	geolocation_lng DECIMAL,
	geolocation_city VARCHAR(40),
	geolocation_state VARCHAR(3)
);

CREATE TABLE IF NOT EXISTS order_payments
(
	order_id VARCHAR(32),
	payment_sequential INT,
	payment_type VARCHAR(20),
	payment_installments INT,
	payment_value DECIMAL
);


CREATE TABLE IF NOT EXISTS order_items
(
	order_id VARCHAR(32),
	order_item_id INT,
	product_id VARCHAR(32),
	seller_id VARCHAR(32),
	shipping_limit_date TIMESTAMP,	
	price DECIMAL,
	freight_value DECIMAL
);


CREATE TABLE IF NOT EXISTS orders
(
	order_id VARCHAR(32),
	customer_id VARCHAR(32),
	order_status VARCHAR(30),
	order_purchase_timestamp TIMESTAMP,
	order_approved_at TIMESTAMP,
	order_delivered_carrier_date TIMESTAMP,
	order_delivered_customer_date TIMESTAMP,
	order_estimated_delivery_date TIMESTAMP
);


CREATE TABLE IF NOT EXISTS sellers
(
	seller_id VARCHAR(32),
	seller_zip_code_prefix VARCHAR(5),
	seller_city VARCHAR(40),
	seller_state VARCHAR(3)
);


CREATE TABLE IF NOT EXISTS products
(
	product_id VARCHAR(32),
	product_category_name VARCHAR(50),
	product_name_length INT,
	product_description_length INT,
	product_photos_qty INT,
	product_weight_g INT,
	product_length_cm INT,
	product_height_cm INT,
	product_width_cm INT
);


CREATE TABLE IF NOT EXISTS order_reviews
(
	review_id VARCHAR(32),
	order_id VARCHAR(32),
	review_score DECIMAL,
	review_comment_title VARCHAR(1000),
	review_comment_message VARCHAR(1000),
	review_creation_date TIMESTAMP,
	review_answer_timestamp TIMESTAMP
);


CREATE TABLE IF NOT EXISTS seller_subscription
(
	seller_id VARCHAR(32),
	subscriber_key TEXT,
	session_count INT,
	plan_id INT
);

CREATE TABLE IF NOT EXISTS subscription_plans
(
	plan_id INT,
	plan_name TEXT,
	max_parallel_sessions INT,
	max_stock_per_product INT
);

CREATE TABLE IF NOT EXISTS seller_stocks
(
	seller_id VARCHAR(32),
	product_id VARCHAR(32),
	stock_count INT
);

CREATE TABLE IF NOT EXISTS customer_carts
(
	customer_id VARCHAR(32),
	seller_id VARCHAR(32),
	product_id VARCHAR(32),
	amount INT
);



/* you can use these values for testing purposes, you can play with the database as much as you want. */ 
INSERT INTO sellers(seller_id, seller_zip_code_prefix, seller_city, seller_state)
VALUES 
	('test_ali', '20920', 'rio de janeiro', 'RJ'),
	('test_burcak', '20920', 'rio de janeiro', 'RJ'),
	('test_cemre', '20920', 'rio de janeiro', 'RJ');

INSERT INTO seller_subscription(seller_id, subscriber_key, session_count, plan_id) 
VALUES 
	('test_ali', 'ali123', 0, 1),
	('test_burcak', 'burcak123', 0, 2),
	('test_cemre','cemre123', 0, 3);


INSERT INTO subscription_plans(plan_id, plan_name, max_parallel_sessions, max_stock_per_product) 
VALUES 
	(1, 'Basic', 2, 4),
	(2, 'Advanced', 4, 8),
	(3, 'Premium', 6, 12);

