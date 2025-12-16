CC = gcc
CCFLAGS = -std=c99 \
	-fsanitize=address -ggdb \
	-Wall -Wextra

SRC = glotta.c
HEADERS = glotta.h
BUILD_DIR = build
BIN = $(BUILD_DIR)/glotta

all: $(BIN)

$(BIN): $(SRC) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) $(SRC) -o $(BIN)

clean:
	rm -rf $(BUILD_DIR)/*
