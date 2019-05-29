CC=gcc
CFLAGS=-I.

all: findpng catpng

findpng: findpng.c
	$(CC) -o findpng findpng.c

catpng: catpng.c zutil.c crc.c
	$(CC) crc.c zutil.c catpng.c -o catpng -lz
# .c.o:
#	$(CC) $(CFLAGS) -c $<
 
clean:
	rm findpng catpng

