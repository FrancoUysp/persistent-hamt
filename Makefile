# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g -fPIC

# Output shared library
TARGET = libhamt.so

# Source files
SRCS = hamt.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Create the shared library
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -shared -o $(TARGET) $(OBJS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and shared library
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
