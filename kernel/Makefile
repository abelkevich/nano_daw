BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

OUT_FILE_NAME = kernel
OUT_FILE_PATH = $(BIN_DIR)/$(OUT_FILE_NAME)

# COMPILING AND LINKING FLAGS

CC = clang++
CFLAGS = -Wall -O3 -std=c++17 -I$(KERNEL_INCLUDE_DIR) -I$(ITEMS_MANAGER_INCLUDE_DIR) -I$(ITEMS_INCLUDE_DIR) -I$(API_INCLUDE_DIR) -I$(LOGURU_INCLUDE_DIR) -I$(JSON_INCLUDE_DIR) -I$(PORTAUDIO_INCLUDE_DIR)
LDFLAGS = -lstdc++ -lstdc++fs -L$(PORTAUDIO_LIB_DIR) -L$(LOGURU_LIB_DIR) -lloguru -lportaudio -lasound -ljack -lrt -lm -ldl -lstdc++ -lpthread

# KERNEL COMMON

KERNEL_SRC_DIR = src
KERNEL_INCLUDE_DIR = include
KERNEL_OBJ_DIR = $(BUILD_DIR)/obj

KERNEL_OBJ_FILES = 	codec_api.o \
				   	codec_manager.o \
					codec_file_info.o \
					render.o \
					rt_output.o \
					resampler.o \
					utils.o \
					kernel.o

RELPATH_KERNEL_OBJ_FILES = $(patsubst %,$(KERNEL_OBJ_DIR)/%,$(KERNEL_OBJ_FILES))

$(KERNEL_OBJ_DIR)/%.o: $(KERNEL_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# ITEMS MANAGER

ITEMS_MANAGER_DIR = items_manager
ITEMS_MANAGER_SRC_DIR = $(ITEMS_MANAGER_DIR)/src
ITEMS_MANAGER_INCLUDE_DIR = $(ITEMS_MANAGER_DIR)/include
ITEMS_MANAGER_OBJ_DIR = $(KERNEL_OBJ_DIR)/$(ITEMS_MANAGER_DIR)

ITEMS_MANAGER_OBJ_FILES = audios_manager.o \
						  fragments_manager.o \
						  sessions_manager.o \
						  tracks_manager.o \
						  items_manager.o

RELPATH_ITEMS_MANAGER_OBJ_FILES = $(patsubst %,$(ITEMS_MANAGER_OBJ_DIR)/%,$(ITEMS_MANAGER_OBJ_FILES))

$(ITEMS_MANAGER_OBJ_DIR)/%.o: $(ITEMS_MANAGER_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@


# ITEMS

ITEMS_DIR = items
ITEMS_SRC_DIR = $(ITEMS_DIR)/src
ITEMS_INCLUDE_DIR = $(ITEMS_DIR)/include
ITEMS_OBJ_DIR = $(KERNEL_OBJ_DIR)/$(ITEMS_DIR)

ITEMS_OBJ_FILES = audio.o \
				  fragment.o \
				  session.o \
				  track.o

RELPATH_ITEMS_OBJ_FILES = $(patsubst %,$(ITEMS_OBJ_DIR)/%,$(ITEMS_OBJ_FILES))

$(ITEMS_OBJ_DIR)/%.o: $(ITEMS_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# API

API_DIR = api
API_SRC_DIR = $(API_DIR)/src
API_INCLUDE_DIR = $(API_DIR)/include
API_OBJ_DIR = $(KERNEL_OBJ_DIR)/$(API_DIR)

API_OBJ_FILES = api.o \
				api_audio.o \
				api_codec.o \
				api_fragment.o \
				api_playback.o \
				api_render.o \
				api_session.o \
				api_track.o


RELPATH_API_OBJ_FILES = $(patsubst %,$(API_OBJ_DIR)/%,$(API_OBJ_FILES))

$(API_OBJ_DIR)/%.o: $(API_SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# THIRD PARTY

THIRD_PARTY_DIR = ../3rd_party

LOGURU_DIR = $(THIRD_PARTY_DIR)/loguru
LOGURU_INCLUDE_DIR = $(LOGURU_DIR)
LOGURU_LIB_DIR = $(LOGURU_DIR)

JSON_INCLUDE_DIR = $(THIRD_PARTY_DIR)/json/single_include

PORTAUDIO_DIR = $(THIRD_PARTY_DIR)/portaudio
PORTAUDIO_INCLUDE_DIR = $(PORTAUDIO_DIR)/include
PORTAUDIO_LIB_DIR = $(PORTAUDIO_DIR)/build

# LINKING
$(OUT_FILE_PATH): $(RELPATH_KERNEL_OBJ_FILES) $(RELPATH_ITEMS_OBJ_FILES) $(RELPATH_API_OBJ_FILES) $(RELPATH_ITEMS_MANAGER_OBJ_FILES)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: all clean path_builder

all: path_builder $(RELPATH_KERNEL_OBJ_FILES) $(RELPATH_ITEMS_OBJ_FILES) $(RELPATH_API_OBJ_FILES) $(RELPATH_ITEMS_MANAGER_OBJ_FILES) $(OUT_FILE_PATH)

clean:
	rm -rf build

# CREATING DIRS

path_builder:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(KERNEL_OBJ_DIR)
	mkdir -p $(ITEMS_OBJ_DIR)
	mkdir -p $(ITEMS_MANAGER_OBJ_DIR)
	mkdir -p $(API_OBJ_DIR)
