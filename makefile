CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2
SDL_CFLAGS := $(shell pkg-config --cflags sdl2)
SDL_LIBS := $(shell pkg-config --libs sdl2)

SRC_DIR := src
BIN := chip8-emulator
SRCS := $(SRC_DIR)/main.c
OBJS := $(SRCS:.c=.o)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(SDL_LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(SDL_CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJS)
