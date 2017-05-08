
CC = gcc
RM = rm -rf
CFLAGS = -g -Wall -std=gnu99
LDFLAGS =

# General
INCLUDES := -Iinclude/
BUILD_DIR := build
TESTS_BUILD_DIR := $(BUILD_DIR)/tests

# Library
LIBRARY = lib
LIBRARY_TARGET := libeloop.a
LIB_SOURCES = $(shell find src/ -name "*.c")
LIB_OBJS = $(LIB_SOURCES:.c=.o)

# Tests
TESTS = tests
TESTS_DIR = tests
TESTS_SOURCES = $(shell find $(TESTS_DIR)/ -name "*.c")
TESTS_TARGETS = $(TESTS_SOURCES:.c=)

.PHONY: all clean $(LIBRARY) $(TESTS)

# Recepies
all: clean $(LIBRARY_TARGET)

$(LIBRARY): $(LIBRARY_TARGET)
$(LIBRARY_TARGET): $(LIB_OBJS)

%.a:
	@mkdir -p $(BUILD_DIR)
	@echo "Linking $@"
	@$(AR) $(ARFLAGS) -o $(BUILD_DIR)/$@ $^

$(TESTS): $(LIBRARY_TARGET) $(TESTS_TARGETS) 
%: %.c
	@mkdir -p $(TESTS_BUILD_DIR)
	@echo "compiling $<"
	@$(CC) $(CFLAGS) $< -o $(BUILD_DIR)/$@ $(BUILD_DIR)/$(LIBRARY_TARGET) $(INCLUDES)

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	@echo "Cleaning..."
	@$(RM) $(BUILD_DIR) $(LIB_OBJS)
