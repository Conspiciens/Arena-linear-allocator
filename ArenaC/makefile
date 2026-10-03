CC = gcc 
CFLAGS = -c -Wall 

all: Arena

Arena: Arena.o 	
	$(CC) Arena.o -o Arena

Arena.o: Arena.c 
	$(CC) $(CFLAGS) Arena.c

clean: 
	rm -rf *.o