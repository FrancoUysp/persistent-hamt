# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g -fPIC

# Output shared libraries
HAMT_TARGET = libhamt.so
LINKEDLIST_TARGET = liblinkedlist.so

# Source files
HAMT_SRCS = hamt.c
LINKEDLIST_SRCS = linked_list_hamt.c linked_list_std.c

# Object files
HAMT_OBJS = $(HAMT_SRCS:.c=.o)
LINKEDLIST_OBJS = $(LINKEDLIST_SRCS:.c=.o)

# Default target
all: $(HAMT_TARGET) $(LINKEDLIST_TARGET)

# Create the shared library for HAMT
$(HAMT_TARGET): $(HAMT_OBJS)
	$(CC) $(CFLAGS) -shared -o $(HAMT_TARGET) $(HAMT_OBJS)

# Create the shared library for LinkedList
$(LINKEDLIST_TARGET): $(LINKEDLIST_OBJS) $(HAMT_TARGET)
	$(CC) $(CFLAGS) -shared -o $(LINKEDLIST_TARGET) $(LINKEDLIST_OBJS) -L. -lhamt

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and shared libraries
clean:
	rm -f $(HAMT_OBJS) $(LINKEDLIST_OBJS) $(HAMT_TARGET) $(LINKEDLIST_TARGET)

# Phony targets
.PHONY: all clean
