SELECT 
  p.product_id, 
  COUNT(or2.review_id) AS review_count, 
  AVG(or2.review_score) AS review_avg 
FROM 
  products p, 
  order_items oi, 
  orders o, 
  order_reviews or2 
WHERE 
  p.product_id = oi.product_id 
  AND oi.order_id = o.order_id 
  AND or2.order_id = o.order_id 
  AND p.product_category_name LIKE '%eletronicos%' 
GROUP BY 
  p.product_id 
HAVING 
  COUNT(or2.review_id) > 4 
ORDER BY 
  review_avg DESC, 
  review_count DESC 
LIMIT 
  10;
