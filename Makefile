
CC = gcc
RM = rm -f
CFLAGS = -g -Wall
LDFLAGS =

# General
INCLUDES := -Iinclude/

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
	@echo "Linking $@"
	@$(AR) $(ARFLAGS) -o $@ $^

$(TESTS): $(LIBRARY_TARGET) $(TESTS_TARGETS) 
%: %.c
	@echo "compiling $<"
	@$(CC) $(CFLAGS) $< -o $@ $(LIBRARY_TARGET) $(INCLUDES)

%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

clean:
	@echo "Cleaning..."
	@$(RM) $(LIBRARY_TARGET) $(LIB_OBJS) $(TESTS_TARGETS)
