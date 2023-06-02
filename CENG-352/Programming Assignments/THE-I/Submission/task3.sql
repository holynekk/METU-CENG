-- Creating the user_review_scores database table
DROP 
  TABLE IF EXISTS user_review_scores;
CREATE TABLE IF NOT EXISTS user_review_scores(
  product_id VARCHAR(32), 
  review_score DECIMAL(10, 7)
);
-- Inserting review averages with existing data
INSERT INTO user_review_scores (product_id, review_score) 
SELECT 
  oi.product_id, 
  AVG(or2.review_score) 
FROM 
  order_items oi, 
  orders o, 
  order_reviews or2 
WHERE 
  oi.order_id = o.order_id 
  AND o.order_id = or2.order_id 
GROUP BY 
  oi.product_id;
 
 
-- Triger for updating average review_score in table user_review_scores
CREATE 
OR REPLACE FUNCTION FunctionReviewUpdateScore() RETURNS trigger AS $$ BEGIN 
UPDATE 
  user_review_scores AS urs 
SET 
  review_score = T.bumbum 
FROM 
  (
    SELECT 
      AVG(or2.review_score) AS bumbum, 
      oi.product_id 
    FROM 
      order_reviews or2, 
      order_items oi, 
      orders o 
    WHERE 
      oi.order_id = o.order_id 
      AND or2.order_id = o.order_id 
    GROUP BY 
      oi.product_id
  ) AS T 
WHERE 
  urs.product_id = T.product_id;
RETURN NEW;
END;
$$ LANGUAGE 'plpgsql';
CREATE 
OR REPLACE TRIGGER TriggerUpdateScore 
AFTER 
  INSERT ON order_reviews EXECUTE PROCEDURE FunctionReviewUpdateScore();
 
-- Triger to prevent data insertion for sellers table with 00000 seller_zip_code_prefix
CREATE 
OR REPLACE FUNCTION trg_zero_zip_code_check() RETURNS trigger AS $func$ BEGIN IF NEW.seller_zip_code_prefix = '00000' THEN RAISE EXCEPTION 'Zip code of the seller CAN NOT be zero!';
END IF;
RETURN NEW;
END $func$ LANGUAGE 'plpgsql';
CREATE 
OR REPLACE TRIGGER zero_zip_code_check BEFORE INSERT ON sellers FOR EACH ROW EXECUTE PROCEDURE trg_zero_zip_code_check();


-- Views with name order_product_customer_review. The first one is STANDARD and the second one is MATERIALIZED.

-- STANDARD VIEWS are computed on-demand (when used in a query), but MATERIALIZED VIEWS are pre-computed. This means that STANDARD ones always
-- return the most updated data in a query. On the other hand, MATERIALIZED ones cannot be up-to-date for all changes in the database. They have stale data.
-- Therefore, we can say that it's better to use STANDARD VIEWS when the data should be accessed infrequently. MATERIALIZED VIEWS are for frequent access.

-- For the STANDARD VIEW, overall execute time for this script is 205ms.
-- For the MATERIALIZED one, overall execute time for this script is 813ms.


-- [STANDARD VIEW]: order_product_customer_review

CREATE 
OR REPLACE VIEW order_product_customer_review AS 
SELECT 
  o.order_id, 
  p.product_id, 
  c.customer_id, 
  or2.review_score 
FROM 
  products p, 
  order_items oi, 
  orders o, 
  order_reviews or2, 
  customers c 
WHERE 
  p.product_id = oi.product_id 
  AND o.order_id = oi.order_id 
  AND or2.order_id = o.order_id 
  AND c.customer_id = o.customer_id;

 
-- [MATERIALIZED VIEW]: order_product_customer_review -- I HAVE COMMENTED IT OUT BECAUSE OF NAMING ISSUES
 
--CREATE MATERIALIZED VIEW order_product_customer_review AS 
--SELECT 
--  o.order_id, 
--  p.product_id, 
--  c.customer_id, 
--  or2.review_score 
--FROM 
--  products p, 
--  order_items oi, 
--  orders o, 
--  order_reviews or2, 
--  customers c 
--WHERE 
--  p.product_id = oi.product_id 
--  AND o.order_id = oi.order_id 
--  AND or2.order_id = o.order_id 
--  AND c.customer_id = o.customer_id;


 
