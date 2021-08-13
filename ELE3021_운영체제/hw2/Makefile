CC=gcc

all: exec1 exec2 hw2


exec1: exec1.o
	gcc -o exec1  exec1.o

exec1.o: exec1.c
	gcc -c -o exec1.o exec1.c

clean: rm *.o exec1



exec2: exec2.o
	gcc -o exec2 exec2.o


exec2.o: exec2.c
	gcc -c -o exec2.o exec2.c

clean: rm *.o exec2



hw2: hw2.o
	gcc -o hw2 hw2.o


hw2.o: hw2.c
	gcc -c -o hw2.o hw2.c


clean: rm *.o hw2



