CC=gcc
CFLAGS=-I.

all: catpng

catpng: catpng.c zutil.c crc.c
	$(CC) -g crc.c zutil.c catpng.c -o catpng -lz -lcurl 
# .c.o:
#	$(CC) $(CFLAGS) -c $<
 
clean:
	rm catpng

