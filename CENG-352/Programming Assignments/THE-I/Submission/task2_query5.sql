SELECT 
  cart_per_month.customer_id, 
  EXTRACT(YEAR FROM cart_per_month.group_date) AS year, 
  EXTRACT(MONTH FROM cart_per_month.group_date) AS month, 
  total_price AS max_total_for_month 
FROM 
  (
    SELECT 
      o.customer_id, 
      DATE_TRUNC(
        'month', o.order_purchase_timestamp
      ) AS group_date, 
      SUM(oi.price) AS total_price, 
      RANK() OVER (
        PARTITION BY DATE_TRUNC(
          'month', o.order_purchase_timestamp
        ) 
        ORDER BY 
          SUM(oi.price) DESC
      ) AS rank_of_customer 
    FROM 
      order_items oi, 
      orders o 
    WHERE 
      oi.order_id = o.order_id 
    GROUP BY 
      DATE_TRUNC(
        'month', o.order_purchase_timestamp
      ), 
      o.customer_id
  ) AS cart_per_month
WHERE 
  rank_of_customer = 1 
ORDER BY 
  cart_per_month.group_date, 
  cart_per_month.customer_id;


    
   
   
   
