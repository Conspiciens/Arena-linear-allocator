CC = gcc 
CFLAGS = -c -Wall 
TARGET_DIR = bin

all: Arena

dir: 
	mkdir -p $(TARGET_DIR)

Arena: Arena.o
	$(CC) Arena.o -o $(TARGET_DIR)/Arena

# Makefile runs the tests in C
tests: Arena.o tests.o
	$(CC) $(TARGET_DIR)/Arena.o $(TARGET_DIR)/tests.o -o $(TARGET_DIR)/tests

################################################
Arena.o: Arena.c Arena.h
	$(CC) $(CFLAGS) Arena.c -o $(TARGET_DIR)/Arena.o

tests.o: tests.c Arena.h
	$(CC) $(CFLAGS) tests.c -o $(TARGET_DIR)/tests.o

clean: 
	rm -rf *.o