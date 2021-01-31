CC=g++
CFLAGS=-c -ansi -Wall -pedantic-errors -O0 -std=c++11

all: linkedlist socialnetwork

socialnetwork: main_socialnetwork.o post.o profile.o socialnetwork.o
	$(CC) main_socialnetwork.o post.o profile.o socialnetwork.o -o socialnetwork

linkedlist: main_linkedlist.o
	$(CC) main_linkedlist.o -o linkedlist

main_socialnetwork.o: main_socialnetwork.cpp
	$(CC) $(CFLAGS) main_socialnetwork.cpp -o main_socialnetwork.o

main_linkedlist.o: main_linkedlist.cpp
	$(CC) $(CFLAGS) main_linkedlist.cpp -o main_linkedlist.o

post.o: Post.cpp Post.h
	$(CC) $(CFLAGS) Post.cpp -o post.o

profile.o: Profile.cpp Profile.h
	$(CC) $(CFLAGS) Profile.cpp -o profile.o

socialnetwork.o: SocialNetwork.cpp SocialNetwork.h Post.cpp Post.h Profile.cpp Profile.h LinkedList.h Node.h
	$(CC) $(CFLAGS) SocialNetwork.cpp -o socialnetwork.o

clean:
	rm *o
	rm linkedlist
	rm socialnetwork
