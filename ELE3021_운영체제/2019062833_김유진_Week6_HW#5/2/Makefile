CC=gcc


all: parent child

parent: parent.o
	g++ -o parent  parent.o

parent.o: parent.c
	g++ -c -o parent.o parent.c

clean: rm *.o parent




child: child.o
	g++ -o child  child.o 

child.o: child.c
	g++ -c -o child.o child.c

clean: rm *.o child




