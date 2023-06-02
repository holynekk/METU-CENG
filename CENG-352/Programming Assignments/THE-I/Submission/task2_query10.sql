SELECT 
  p.product_id, 
  COUNT(p.product_id) AS total_sales, 
  RANK() OVER (
    PARTITION BY p.product_category_name 
    ORDER BY 
      COUNT(p.product_id) DESC
  ) AS sales_rank, 
  p.product_category_name 
FROM 
  products p, 
  order_items oi 
WHERE 
  p.product_id = oi.product_id 
  AND p.product_category_name NOT LIKE '' 
GROUP BY 
  p.product_category_name, 
  p.product_id 
HAVING 
  COUNT(p.product_id) > 9;
