# Makefile for compiling the HAMT program

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = hamt
SRC = hamt.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
