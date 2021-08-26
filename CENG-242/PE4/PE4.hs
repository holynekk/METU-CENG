module PE4 where

import Data.Maybe -- up to you if you want to use it or not

-- Generic DictTree definition with two type arguments
data DictTree k v = Node [(k, DictTree k v)] | Leaf v deriving Show

-- Lightweight Char wrapper as a 'safe' Digit type
newtype Digit = Digit Char deriving (Show, Eq, Ord) -- derive equality and comparison too!

-- Type aliases
type DigitTree = DictTree Digit String
type PhoneNumber = [Digit]


---------------------------------------------------------------------------------------------
------------------------- DO NOT CHANGE ABOVE OR FUNCTION SIGNATURES-------------------------
--------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
--------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
---------------------------------------------------------------------------------------------


----------
-- Part I:
-- Some Maybe fun! Basic practice with an existing custom datatype.

-- toDigit: Safely convert a character to a digit
toDigit :: Char -> Maybe Digit
toDigit a = if a == '0' || a == '1' || a == '2' || a == '3' || a == '4' || a == '5' || a == '6' || a == '7' || a == '8' || a == '9' then Just (Digit a ) else Nothing

-- toDigits: Safely convert a bunch of characters to a list of digits.
--           Particularly, an empty string should fail.

isOnlyNumeric [] = True
isOnlyNumeric (a:b) = if a == '0' || a == '1' || a == '2' || a == '3' || a == '4' || a == '5' || a == '6' || a == '7' || a == '8' || a == '9' then isOnlyNumeric b else False

bumbum a = Digit a

convertAll :: String -> [Digit]
convertAll [] = []
convertAll (a:b) = (bumbum a):(convertAll b)

toDigits :: String -> Maybe PhoneNumber
toDigits [] = Nothing
toDigits a = if isOnlyNumeric a then Just (convertAll a) else Nothing

-----------
-- Part II:
-- Some phonebook business.

-- numContacts: Count the number of contacts in the phonebook...

numContacts :: DigitTree -> Int
numContacts (Leaf _) = 1
numContacts (Node []) = 0
numContacts (Node a) = 0 + numContacts (snd (head a)) + numContacts (Node (tail a))
    
-- getContacts: Generate the contacts and their phone numbers in order given a tree. 

flat_earther_flatten_DigitTree record (Leaf str) = [(record, str)]
flat_earther_flatten_DigitTree record (Node []) = []
flat_earther_flatten_DigitTree record (Node a) = [] ++ (flat_earther_flatten_DigitTree (record++[(fst (head a))]) (snd (head a)) ) ++ (flat_earther_flatten_DigitTree record (Node (tail a)) )

getContacts :: DigitTree -> [(PhoneNumber, String)]
getContacts a = flat_earther_flatten_DigitTree [] a

-- autocomplete: Create an autocomplete list of contacts given a prefix
-- e.g. autocomplete "32" areaCodes -> 
--      [([Digit '2'], "Adana"), ([Digit '6'], "Hatay"), ([Digit '8'], "Osmaniye")]

reachCommonPart _ (Leaf "xXx") = []
reachCommonPart [] b = getContacts b
reachCommonPart a (Leaf _) = []
reachCommonPart (x:y) (Node b) = let 
                                    properNode = [snd k| k<-b, (fst k) == bumbum x]
                                 in reachCommonPart y (if length properNode > 0 then properNode !! 0 else (Leaf "xXx"))
                            
autocomplete :: String -> DigitTree -> [(PhoneNumber, String)]
autocomplete a b = if length a == 0 
                    then [] 
                    else if isOnlyNumeric a
                            then [] ++ reachCommonPart a b
                            else []
-----------
-- Example Trees
-- Two example trees to play around with, including THE exampleTree from the text. 
-- Feel free to delete these or change their names or whatever!

exampleTree :: DigitTree
exampleTree = Node [
    (Digit '1', Node [
        (Digit '3', Node [
            (Digit '7', Node [
                (Digit '8', Leaf "Jones")])]),
        (Digit '5', Leaf "Steele"),
        (Digit '9', Node [
            (Digit '1', Leaf "Marlow"),
            (Digit '2', Node [
                (Digit '3', Leaf "Stewart")])])]),
    (Digit '3', Leaf "Church"),
    (Digit '7', Node [
        (Digit '2', Leaf "Curry"),
        (Digit '7', Leaf "Hughes")])]

areaCodes :: DigitTree
areaCodes = Node [
    (Digit '3', Node [
        (Digit '1', Node [
            (Digit '2', Leaf "Ankara")]),
        (Digit '2', Node [
            (Digit '2', Leaf "Adana"),
            (Digit '6', Leaf "Hatay"),
            (Digit '8', Leaf "Osmaniye")])]),
    (Digit '4', Node [
        (Digit '6', Node [
            (Digit '6', Leaf "Artvin")])])]

