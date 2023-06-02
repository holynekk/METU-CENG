SELECT 
  oi2.seller_id, 
  SUM(oi2.price) AS total_earned 
FROM 
  order_items oi2, 
  orders o2, 
  (
    SELECT 
      s.seller_id 
    FROM 
      sellers s, 
      order_items oi, 
      orders o 
    WHERE 
      s.seller_id = oi.seller_id 
      AND o.order_id = oi.order_id 
      AND o.order_purchase_timestamp >= '01.01.2018' 
      AND o.order_purchase_timestamp < '01.02.2018' 
    GROUP BY 
      s.seller_id 
    HAVING 
      COUNT(*) > 49 
    INTERSECT 
    SELECT 
      s.seller_id 
    FROM 
      sellers s, 
      order_items oi, 
      orders o 
    WHERE 
      s.seller_id = oi.seller_id 
      AND o.order_id = oi.order_id 
      AND o.order_purchase_timestamp >= '01.02.2018' 
      AND o.order_purchase_timestamp < '01.03.2018' 
    GROUP BY 
      s.seller_id 
    HAVING 
      COUNT(*) > 49 
    INTERSECT 
    SELECT 
      s.seller_id 
    FROM 
      sellers s, 
      order_items oi, 
      orders o 
    WHERE 
      s.seller_id = oi.seller_id 
      AND o.order_id = oi.order_id 
      AND o.order_purchase_timestamp >= '01.03.2018' 
      AND o.order_purchase_timestamp < '01.04.2018' 
    GROUP BY 
      s.seller_id 
    HAVING 
      COUNT(*) > 49
  ) AS T 
WHERE 
  oi2.seller_id = T.seller_id 
  AND o2.order_id = oi2.order_id 
  AND o2.order_purchase_timestamp >= '01.01.2018' 
  AND o2.order_purchase_timestamp < '01.04.2018' 
GROUP BY 
  oi2.seller_id 
ORDER BY 
  total_earned DESC;
