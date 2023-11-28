# Compiler
CC := gcc
# Executable name
TARGET := bmm350_example.bin
# Source files
SRC_DIR1 := .
SRC_DIR2 := BMM350-SensorAPI/

# Compiler flags
CFLAGS := -Wall -Wextra -Iinclude -I${SRC_DIR2} -Wno-unused-parameter

SRCS1 := $(wildcard $(SRC_DIR1)/*.c)
SRCS2 := $(wildcard $(SRC_DIR2)/*.c)

# Object files
OBJ_DIR := build
OBJS1 := $(patsubst $(SRC_DIR1)/%.c, $(OBJ_DIR)/%.o, $(SRCS1))
OBJS2 := $(patsubst $(SRC_DIR2)/%.c, $(OBJ_DIR)/%.o, $(SRCS2))

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS1) $(OBJS2)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Rule to build object files for source folder 1
$(OBJ_DIR)/%.o: $(SRC_DIR1)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to build object files for source folder 2
$(OBJ_DIR)/%.o: $(SRC_DIR2)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony target to avoid conflicts with file names
.PHONY: all clean