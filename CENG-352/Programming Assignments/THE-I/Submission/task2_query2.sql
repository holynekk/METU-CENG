SELECT 
  DISTINCT s.seller_id, 
  AVG(
    o.order_delivered_carrier_date - o.order_purchase_timestamp
  ) AS avg_duration 
FROM 
  sellers s, 
  orders o, 
  order_items oi, 
  products p 
WHERE 
  p.product_id = oi.product_id 
  AND p.product_category_name LIKE '%beleza_saude%' 
  AND s.seller_id = oi.seller_id 
  AND o.order_id = oi.order_id 
  AND s.seller_city LIKE '%curitiba%' 
GROUP BY 
  s.seller_id 
HAVING 
  AVG(
    o.order_delivered_carrier_date - o.order_purchase_timestamp
  ) < '48:00:00' 
ORDER BY 
  avg_duration DESC;
