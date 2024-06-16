# Makefile for timed_practice.c

# Compiler
CC = gcc

# Flags for compiler
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0 libxml-2.0)

# Flags for linker
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 libxml-2.0)

# Executable name
TARGET = timed_practice

# Source file
SRCS = timed_practice.c

# Object files (automatically generated from source)
OBJS = $(SRCS:.c=.o)

# Targets
all: $(TARGET)

# Rule to link object files into executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
