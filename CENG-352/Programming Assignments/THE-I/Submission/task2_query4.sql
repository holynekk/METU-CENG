SELECT 
  s.seller_id, 
  AVG(or2.review_score) AS avg_rating, 
  COUNT(or2.review_id) AS review_count 
FROM 
  sellers s,
  orders o,
  order_items oi,
  order_reviews or2,
  products p
WHERE 
  oi.seller_id = s.seller_id
  AND s.seller_city LIKE '%sao paulo%'
  AND oi.order_id = o.order_id 
  AND p.product_id = oi.product_id 
  AND p.product_category_name LIKE '%automotivo%' 
  AND or2.order_id = o.order_id 
GROUP BY 
  s.seller_id 
HAVING 
  COUNT(or2.review_id) > 9
ORDER BY 
  avg_rating DESC 
LIMIT 
  5;
