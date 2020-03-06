BUILD_DIR = ../loguru
BIN_DIR = $(BUILD_DIR)

OUT_FILE_NAME = libloguru.a
OUT_FILE_PATH = $(BIN_DIR)/$(OUT_FILE_NAME)

# COMPILING AND LINKING FLAGS

CC = clang++
CFLAGS = -Wall -O3 -std=c++17 -I$(INCLUDE_DIR)

# TERMINAL FILES

SRC_DIR = ../loguru
INCLUDE_DIR = ../loguru
OBJ_DIR = $(BUILD_DIR)

OBJ_FILES = loguru.o

RELPATH_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(OBJ_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# LINKING
$(OUT_FILE_PATH): $(RELPATH_OBJ_FILES)
	ar ruv $@ $^
	ranlib $@

.PHONY: all clean

all: $(RELPATH_OBJ_FILES) $(OUT_FILE_PATH)

clean:
	rm -rf $(OUT_FILE_PATH)