-- Mert Kaan YILMAZ - 2381093

CREATE table IF NOT EXISTS product_category(
	product_category_name VARCHAR(50),
	product_category_name_english VARCHAR(50),
	PRIMARY KEY (product_category_name)
);
CREATE table IF NOT EXISTS customers(
	customer_id VARCHAR(32) PRIMARY KEY,
	customer_unique_id VARCHAR(32),
	customer_zip_code_prefix VARCHAR(64),
	customer_city VARCHAR(64),
	customer_state VARCHAR(64)
);
CREATE table IF NOT EXISTS geolocation(
	geolocation_zip_code_prefix VARCHAR(64),
	geolocation_lat DECIMAL(10, 7),
	geolocation_lng DECIMAL(10, 7),
	geolocation_city VARCHAR(64),
	geolocation_state VARCHAR(64)
);
CREATE table IF NOT EXISTS orders(
	order_id VARCHAR(32) PRIMARY KEY,
	customer_id VARCHAR(32),
	order_status VARCHAR(16),
	order_purchase_timestamp TIMESTAMP,
	order_approved_at TIMESTAMP,
	order_delivered_carrier_date TIMESTAMP,
	order_delivered_customer_date TIMESTAMP,
	order_estimated_delivery_date TIMESTAMP,
	FOREIGN KEY (customer_id) REFERENCES customers(customer_id)
);
CREATE table IF NOT EXISTS sellers(
	seller_id VARCHAR(32),
	seller_zip_code_prefix VARCHAR(16),
	seller_city VARCHAR(64),
	seller_state VARCHAR(8),
	PRIMARY KEY (seller_id)
);
CREATE table IF NOT EXISTS order_payments(
	order_id VARCHAR(32),
	payment_sequential INT,
	payment_type VARCHAR(32),
	payment_installments INT,
	payment_value DECIMAL(10, 2),
	UNIQUE (order_id, payment_sequential),
	FOREIGN KEY (order_id) REFERENCES orders(order_id)
);
CREATE table IF NOT EXISTS products(
	product_id VARCHAR(32),
	product_category_name VARCHAR(64),
	product_name_length INT,
	product_description_length INT,
	product_photos_qty INT,
	product_weight_g INT,
	product_length_cm INT,
	product_height_cm INT,
	product_width_cm INT,
	PRIMARY KEY (product_id)
);
CREATE table IF NOT EXISTS order_items(
	order_id VARCHAR(32),
	order_item_id INT,
	product_id VARCHAR(32),
	seller_id VARCHAR(32),
	shipping_limit_date TIMESTAMP,
	price DECIMAL(10, 2),
	freight_value DECIMAL(10, 2),
	UNIQUE (order_id, order_item_id),
	FOREIGN KEY (order_id) REFERENCES orders(order_id),
	FOREIGN KEY (product_id) REFERENCES products(product_id),
	FOREIGN KEY (seller_id) REFERENCES sellers(seller_id)
);
CREATE table IF NOT EXISTS order_reviews(
	review_id VARCHAR(32),
	order_id VARCHAR(32),
	review_score INT,
	review_comment_title VARCHAR(64),
	review_comment_message VARCHAR(256),
	review_creation_date TIMESTAMP,
	review_answer_timestamp TIMESTAMP,
	UNIQUE (order_id, review_id),
	FOREIGN KEY (order_id) REFERENCES orders(order_id)
);

