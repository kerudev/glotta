SRC = glotta.c
HEADERS = glotta.h
BUILD_DIR = build
BIN = $(BUILD_DIR)/glotta

all: $(BIN)

$(BIN): $(SRC) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	gcc -I. -I/usr/include/structypes $(SRC) -o $(BIN)

clean:
	rm -rf $(BUILD_DIR)/*
