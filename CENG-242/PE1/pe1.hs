module PE1 where

import Text.Printf


-- This function takes a Double and rounds it to 2 decimal places as requested in the PE --
getRounded :: Double -> Double
getRounded x = read s :: Double
               where s = printf "%.2f" x

-------------------------------------------------------------------------------------------
----------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------

convertTL :: Double -> String -> Double
convertTL money "USD" = getRounded (money / 8.18)
convertTL money "EUR" = getRounded (money / 9.62)
convertTL money "BTC" = getRounded (money / 473497.31)

-------------------------------------------------------------------------------------------

countOnWatch :: [String] -> String -> Int -> Int
countOnWatch schedule employee 0 = 0
countOnWatch (x:schedule) employee days = (if x == employee then 1 else 0) + countOnWatch schedule employee (days-1)

-------------------------------------------------------------------------------------------

digit_function :: Int -> Int
digit_function x
    | x `mod` 3 == 0 = x - 1
    | x `mod` 4 == 0 = if((2*x) > 9) then (2*x)`mod`10 else 2*x
    | x `mod` 5 == 0 = x + 3
    | otherwise = if((4 + x) > 9) then (4 + x)`mod`10 else 4 + x

encrypt :: Int -> Int
encrypt 0 = 0
encrypt x =
    let last_digit = x`mod`10
        result = digit_function last_digit
    in result + 10 * (encrypt (x`div`10))

-------------------------------------------------------------------------------------------

calculate :: (Double, Int) -> Double
calculate (money, year)
    | (money >= 10000) && (year >= 2) = money * ((1+11.5/1200)^(12*year))
    | (money >= 10000) && (year <= 2) = money * ((1+10.5/1200)^(12*year))
    | (money <= 10000) && (year >= 2) = money * ((1+9.5/1200)^(12*year))
    | (money <= 10000) && (year <= 2) = money * ((1+9.0/1200)^(12*year))

compoundInterests :: [(Double, Int)] -> [Double]
compoundInterests [] = []
compoundInterests (x:investments) = (getRounded (calculate x) : (compoundInterests investments))
