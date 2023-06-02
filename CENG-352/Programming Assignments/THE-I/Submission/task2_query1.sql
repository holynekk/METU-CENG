SELECT 
  DISTINCT c.customer_city 
FROM 
  customers c, 
  orders o 
WHERE 
  c.customer_id = o.customer_id 
  AND NOT EXISTS (
    SELECT 
      * 
    FROM 
      order_payments op 
    WHERE 
      o.order_id = op.order_id 
      AND op.payment_type NOT LIKE '%voucher%'
  ) 
ORDER BY 
  c.customer_city;
