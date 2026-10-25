#  Name:          Makefile
#  Author:        Jan Štěpánek
#  Date:          2026-10-04

CC := gcc

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))

QDBMP_SRC := include/QDBMP/qdbmp.c
QDBMP_OBJ := build/qdbmp.o

OBJ += $(QDBMP_OBJ)


CPPFLAGS := -Iinclude
CFLAGS := -std=c11 -O2 -Wall -Wextra
LDLIBS := -lOpenCL


# ------------------------------------------------------------
# Platform-specific settings
# ------------------------------------------------------------

ifeq ($(OS),Windows_NT)

    TARGET := opencl.exe

    LDFLAGS := -Llib

    MKDIR := C:/msys64/usr/bin/mkdir.exe
    RM := C:/msys64/usr/bin/rm.exe

else

    TARGET := opencl

    LDFLAGS :=

    MKDIR := mkdir
    RM := rm

endif


# ------------------------------------------------------------
# Build
# ------------------------------------------------------------

all: $(TARGET)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@


build/%.o: src/%.c | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


$(QDBMP_OBJ): $(QDBMP_SRC) | build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


build:
	$(MKDIR) -p build


run: $(TARGET)
	./$(TARGET)


clean:
	$(RM) -rf build
	$(RM) -f $(TARGET)


rebuild: clean all


.PHONY: all run clean rebuild