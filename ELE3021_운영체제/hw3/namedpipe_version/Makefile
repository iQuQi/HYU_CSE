all: reader_Aprocess writer_Bprocess

reader_Aprocess: reader_Aprocess.o
	gcc -o reader_Aprocess reader_Aprocess.o

reader_Aprocess.o: reader_Aprocess.c
	gcc -c -o reader_Aprocess.o reader_Aprocess.c

clean: rm *.o reader_Aproces


writer_Bprocess: writer_Bprocess.o
	gcc -o writer_Bprocess writer_Bprocess.o

writer_Bprocess.o: writer_Bprocess.c
	gcc -c -o writer_Bprocess.o writer_Bprocess.c

clean: rm *.o writer_Bprocess
