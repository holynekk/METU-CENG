SELECT 
  p.product_category_name, 
  COUNT(p.product_category_name) AS past_order_count 
FROM 
  orders o, 
  order_items oi, 
  products p, 
  sellers s, 
  customers c 
WHERE 
  o.order_id = oi.order_id 
  AND oi.product_id = p.product_id 
  AND s.seller_id = oi.seller_id 
  AND c.customer_id = o.customer_id 
  AND c.customer_city LIKE '%sao paulo%' 
  AND p.product_category_name NOT IN (
    SELECT 
      p2.product_category_name 
    FROM 
      products p2, 
      sellers s2, 
      order_items oi2
    WHERE 
      p2.product_id = oi2.product_id 
      AND oi2.seller_id = s2.seller_id
      AND s2.seller_city LIKE '%sao paulo%'
  ) 
GROUP BY 
  p.product_category_name 
HAVING 
  COUNT(o.order_id) > 9 
ORDER BY 
  p.product_category_name ASC;
