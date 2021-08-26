module PE3 where

data Cell = SpaceCraft Int | Sand | Rock Int | Pit deriving (Eq, Read, Show)

type Grid = [[Cell]]
type Coordinate = (Int, Int)

data Move = North | East | South | West | PickUp | PutDown deriving (Eq, Read, Show)

data Robot = Robot { name :: String,
                     location :: Coordinate,
                     capacity :: Int,
                     energy :: Int,
                     storage :: Int } deriving (Read, Show)

-------------------------------------------------------------------------------------------
--------------------------------- DO NOT CHANGE ABOVE -------------------------------------
------------- DUMMY IMPLEMENTATIONS ARE GIVEN TO PROVIDE A COMPILABLE TEMPLATE ------------
------------------- REPLACE THEM WITH YOUR COMPILABLE IMPLEMENTATIONS ---------------------
-------------------------------------------------------------------------------------------
-------------------------------------- PART I ---------------------------------------------

isInGrid :: Grid -> Coordinate -> Bool
isInGrid grid coor = if (fst coor) < 0 || (snd coor) < 0 then False
                                                         else if  (fst coor) >= length (grid !! 0) then False
                                                                                            else if (snd coor) >= length grid then False
                                                                                                                              else True

-------------------------------------------------------------------------------------------

whatIsIt (SpaceCraft n) = ("SpaceCraft", n)
whatIsIt (Sand) = ("Sand", 0)
whatIsIt (Rock n) = ("Rock", n)
whatIsIt (Pit) = ("Pit", 0)

flatten :: [[a]] -> [a]         
flatten xs = (\z n -> foldr (\x y -> foldr z y x) n xs) (:) []

totalCount :: Grid -> Int
totalCount grid = foldl (+) 0 (flatten [ [ snd (whatIsIt b) | b<-a, fst (whatIsIt b) == "Rock"] | a<-grid])

-------------------------------------------------------------------------------------------

coor_helper grid row col = if col >= length (grid !! 0) then []
                                                   else if row >= length grid then (coor_helper grid 0 (col+1))
                                                                              else if (fst (whatIsIt (grid !! row !! col)) == "Pit") then (col, row):(coor_helper grid (row+1) col)
                                                                                                                                     else (coor_helper grid (row+1) col)
 
coordinatesOfPits :: Grid -> [Coordinate]
coordinatesOfPits grid = coor_helper grid 0 0

-------------------------------------------------------------------------------------------
change_location num pos location = 0

change_position North grid y x location = if y == 0 then location else (fst(location), snd(location)-1)
change_position South grid y x location = if y == y then location else (fst(location), snd(location)+1)
change_position West grid y x location = if y == 0 then location else (fst(location)-1, snd(location))
change_position East grid y x location = if y == 0 then location else (fst(location)+1, snd(location))

tracePath :: Grid -> Robot -> [Move] -> [Coordinate]
tracePath grid robot [] = []
tracePath grid robot (first_move:other_moves) = if energy(robot) > 0 then (1,1):(tracePath grid robot other_moves)  else (location(robot)):(tracePath grid robot other_moves) where new_location = change_position first_move grid (length (grid)) (length (grid !! 0)) (location(robot))
                                                                                                                                                                                            
                    

------------------------------------- PART II ----------------------------------------------

find_spacecraft grid row col = if col >= length (grid !! 0) then (0,0)
                                                            else if row >= length grid then (find_spacecraft grid 0 (col+1))
                                                                                       else if (fst (whatIsIt (grid !! row !! col)) == "SpaceCraft") then (col, row)
                                                                                                                                                     else (find_spacecraft grid (row+1) col)

energise_helper robot sx sy = if new_energy >= 100 then robot {energy = 100} else robot {energy = new_energy} where calculated_energy = 100 - (abs (fst (location(robot)) - sx) + abs (snd (location(robot)) - sy)) * 20
                                                                                                                    old_energy = energy(robot)
                                                                                                                    new_energy = if old_energy + calculated_energy <= 0 then 0 else old_energy + calculated_energy

energiseRobots :: Grid -> [Robot] -> [Robot]
energiseRobots grid robots = [energise_helper robot (fst spacecraft_yx) (snd spacecraft_yx) | robot<-robots] where spacecraft_yx = (find_spacecraft grid 0 0)

-------------------------------------------------------------------------------------------

applyMoves :: Grid -> Robot -> [Move] -> (Grid, Robot)
applyMoves grid robot moves = (grid, robot)

