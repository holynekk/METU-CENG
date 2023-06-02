SELECT 
  s.seller_id, 
  EXTRACT(
    MONTH 
    FROM 
      DATE_TRUNC('month', oi.shipping_limit_date)
  ) AS month, 
  SUM(oi.freight_value) 
FROM 
  sellers s, 
  order_items oi 
WHERE 
  oi.seller_id = s.seller_id 
  AND oi.shipping_limit_date < '01.01.2019' 
  AND oi.shipping_limit_date >= '01.01.2018' 
GROUP BY 
  GROUPING SETS (
    (
      s.seller_id, 
      DATE_TRUNC('month', oi.shipping_limit_date)
    ), 
    (s.seller_id)
  ) 
ORDER BY 
  s.seller_id;
