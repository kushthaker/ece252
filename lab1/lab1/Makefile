CC=gcc
CFLAGS=-I.

all: findpng catpng

findpng: findpng.o 
	$(CC) -o findpng findpng.o

catpng: catpng.o zutil.o crc.o
	$(CC) -o -lz catpng catpng.o zutil.o crc.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o findpng catpng

