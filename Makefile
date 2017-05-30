
CC = gcc
LD = ld
RM = rm -rf
CFLAGS = -g -fPIC -Wall -std=gnu99
LDFLAGS = -fPIC -shared -lrt

# General
INCLUDES := -Iinclude/
TEST_INCLUDES := $(INCLUDES) -Isrc/
BUILD_DIR := build
TESTS_BUILD_DIR := $(BUILD_DIR)/tests

# Library
LIBRARY = lib
LIBRARY_TARGET := $(BUILD_DIR)/libeloop.so
LIB_SOURCES = $(shell find src/ -name "*.c")
LIB_OBJS = $(LIB_SOURCES:.c=.o)

# Tests
TESTS = tests
TESTS_DIR = tests
TESTS_SOURCES = $(shell find $(TESTS_DIR)/ -name "*.c")
TESTS_TARGETS = $(TESTS_SOURCES:.c=)

.PHONY: all clean build_dir tests_build_dir $(LIBRARY) $(TESTS)

# Recepies
all: clean $(LIBRARY)

$(LIBRARY): build_dir $(LIBRARY_TARGET)
$(LIBRARY_TARGET): $(LIB_OBJS)

%.so:
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) -o $@ $^

$(TESTS): $(LIBRARY) tests_build_dir $(TESTS_TARGETS)
%: %.c
	@echo "compiling $<"
	@$(CC) $(CFLAGS) $< -o $(BUILD_DIR)/$@ $(LIBRARY_TARGET) $(TEST_INCLUDES)

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

build_dir:
	@mkdir -p $(BUILD_DIR)

tests_build_dir:
	@mkdir -p $(TESTS_BUILD_DIR)

clean:
	@echo "Cleaning..."
	@$(RM) $(BUILD_DIR) $(LIB_OBJS)
