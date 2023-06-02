SELECT 
  p.product_category_name, 
  COUNT (*) AS late_delivery_count 
FROM 
  products p, 
  customers c, 
  orders o, 
  order_items oi 
WHERE 
  oi.order_id = o.order_id 
  AND oi.product_id = p.product_id 
  AND o.customer_id = c.customer_id 
  AND c.customer_city ILIKE '%rio de janeiro%' 
  AND o.order_purchase_timestamp >= '01.06.2018' 
  AND o.order_purchase_timestamp < '01.09.2018' 
  AND o.order_delivered_customer_date > o.order_estimated_delivery_date 
GROUP BY 
  p.product_category_name 
ORDER BY 
  late_delivery_count DESC, 
  p.product_category_name ASC;
