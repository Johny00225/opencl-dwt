#  Name:          Makefile
#  Author:        Jan Štěpánek
#  Date:          2026-10-04

CC := gcc

CFLAGS := -O2 -Wall -Wextra -Iinclude
LDFLAGS := -Llib
LDLIBS := -lOpenCL

TARGET := opencl_test.exe

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run