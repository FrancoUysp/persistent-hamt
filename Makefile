CC = gcc
CFLAGS = -Wall -g

# Define the object files
OBJS = main.o associative_array.o linked_list.o hamt.o

# Define the target executable
TARGET = hamt

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c associative_array.h linked_list.h hamt.h
	$(CC) $(CFLAGS) -c main.c

associative_array.o: associative_array.c associative_array.h hamt.h
	$(CC) $(CFLAGS) -c associative_array.c

linked_list.o: linked_list.c linked_list.h hamt.h
	$(CC) $(CFLAGS) -c linked_list.c

hamt.o: hamt.c hamt.h
	$(CC) $(CFLAGS) -c hamt.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
