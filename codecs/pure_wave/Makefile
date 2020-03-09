BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

OUT_FILE_NAME = libpure_wave.so
OUT_FILE_PATH = $(BIN_DIR)/$(OUT_FILE_NAME)

# COMPILING AND LINKING FLAGS

CC = clang++
CFLAGS = -fPIC -Wall -O3 -std=c++17 -I$(INCLUDE_DIR) -I$(KERNEL_INCLUDE_DIR) -I$(LOGURU_INCLUDE_DIR)
LDFLAGS = -shared -lstdc++ -lpthread -lloguru -L$(LOGURU_LIB_DIR)

# TERMINAL FILES

SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = $(BUILD_DIR)/obj

OBJ_FILES = pure_wave.o

RELPATH_OBJ_FILES = $(patsubst %,$(OBJ_DIR)/%,$(OBJ_FILES))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# LINKING
$(OUT_FILE_PATH): $(RELPATH_OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

# KERNEL FILES
KERNEL_DIR = ../../kernel
KERNEL_INCLUDE_DIR = $(KERNEL_DIR)/include

# THIRD PARTY
THIRD_PARTY_DIR = ../../3rd_party

LOGURU_DIR = $(THIRD_PARTY_DIR)/loguru
LOGURU_INCLUDE_DIR = $(LOGURU_DIR)
LOGURU_LIB_DIR = $(LOGURU_DIR)

.PHONY: all clean path_builder

all: path_builder $(RELPATH_OBJ_FILES) $(OUT_FILE_PATH)

clean:
	rm -rf $(BUILD_DIR)

# CREATING DIRS

path_builder:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)