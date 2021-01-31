CC=g++
CFLAGS=-c -std=c++11 -Wall -pedantic-errors -O0

all: moviestore

moviestore: main.o movie.o moviestore.o
	$(CC) main.o movie.o moviestore.o -o moviestore

main.o: main_moviestore.cpp
	$(CC) $(CFLAGS) main_moviestore.cpp -o main.o

movie.o: movie.cpp movie.h
	$(CC) $(CFLAGS) movie.cpp

moviestore.o: moviestore.cpp moviestore.h bst.h movie.h director_comparator.h title_comparator.h
	$(CC) $(CFLAGS) moviestore.cpp

clean:
	rm *o
