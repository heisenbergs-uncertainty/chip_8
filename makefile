CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)

SRC_DIR := src
TEST_DIR := test
UNITY := test-framework/unity.c

BIN := chip8-emulator

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:.c=.o)

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(TEST_SRCS:.c=.o)
SRC_OBJS := $(wildcard $(SRC_DIR)/*.c)
SRC_OBJS := $(SRC_OBJS:.c=.o)

ASANFLAGS = -fsanitize=address -fno-common -fno-omit-frame-pointer

.PHONY: all clean test memcheck

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(SDL_LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

test: tests.out
	@./tests.out

tests.out: $(SRC_OBJS) $(TEST_OBJS) $(UNITY)
	@echo Compiling tests
	$(CC) $(CFLAGS) $(UNITY) $(SRC_OBJS) $(TEST_OBJS) -o tests.out

memcheck: $(SRC_OBJS) $(TEST_OBJS) $(UNITY)
	@echo Compiling memcheck
	$(CC) $(ASANFLAGS) $(CFLAGS) $(UNITY) $(SRC_OBJS) $(TEST_OBJS) -o memcheck.out
	@./memcheck.out
	@echo "Memory check passed"

clean:
	rm -f $(BIN) $(OBJS) $(SRC_OBJS) $(TEST_OBJS) tests.out memcheck.out
