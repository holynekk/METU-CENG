
CXX = g++
CXX_FLAGS = -std=c++11 -Wall -O0

FILES = HashUtils.o Graph.o main.cpp

all:
	$(CXX) $(CXX_FLAGS) -c HashUtils.cpp -o HashUtils.o
	$(CXX) $(CXX_FLAGS) -c Graph.cpp -o Graph.o
	$(CXX) $(CXX_FLAGS) $(FILES) -o Test


clean:
	rm -f Test
	rm -f *.o