# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g -fPIC

# Output shared libraries
TARGET_HAMT = libhamt.so
TARGET_LINKEDLIST = liblinkedlist.so
TARGET_LINKEDLIST_HAMT = liblinkedlisthamt.so

# Source files
SRCS_HAMT = hamt.c
SRCS_LINKEDLIST = linked_list_std.c
SRCS_LINKEDLIST_HAMT = linked_list_hamt.c

# Object files
OBJS_HAMT = $(SRCS_HAMT:.c=.o)
OBJS_LINKEDLIST = $(SRCS_LINKEDLIST:.c=.o)
OBJS_LINKEDLIST_HAMT = $(SRCS_LINKEDLIST_HAMT:.c=.o)

# Default target
all: $(TARGET_HAMT) $(TARGET_LINKEDLIST) $(TARGET_LINKEDLIST_HAMT)

# Create the shared library for HAMT
$(TARGET_HAMT): $(OBJS_HAMT)
	$(CC) $(CFLAGS) -shared -o $(TARGET_HAMT) $(OBJS_HAMT)

# Create the shared library for standard linked list
$(TARGET_LINKEDLIST): $(OBJS_LINKEDLIST)
	$(CC) $(CFLAGS) -shared -o $(TARGET_LINKEDLIST) $(OBJS_LINKEDLIST)

# Create the shared library for HAMT linked list
$(TARGET_LINKEDLIST_HAMT): $(OBJS_LINKEDLIST_HAMT) $(TARGET_HAMT)
	$(CC) $(CFLAGS) -shared -o $(TARGET_LINKEDLIST_HAMT) $(OBJS_LINKEDLIST_HAMT) $(TARGET_HAMT)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and shared libraries
clean:
	rm -f $(OBJS_HAMT) $(OBJS_LINKEDLIST) $(OBJS_LINKEDLIST_HAMT) $(TARGET_HAMT) $(TARGET_LINKEDLIST) $(TARGET_LINKEDLIST_HAMT)

# Phony targets
.PHONY: all clean
