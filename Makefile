.PHONY: path_builder pack all clean kernel client_terminal codec_pure_wave portaudio loguru kernel_clean client_terminal_clean codec_pure_wave_clean portaudio_clean loguru_clean

THIRD_PARTY_DIR = 3rd_party
LOGURU_CUSTOM_DIR = $(THIRD_PARTY_DIR)/loguru_custom_build
PORTAUDIO_DIR = $(THIRD_PARTY_DIR)/portaudio

KERNEL_DIR = kernel

CLIENTS_DIR = clients
CLIENT_TERMINAL_DIR = $(CLIENTS_DIR)/terminal

CODECS_DIR = codecs
CODEC_PURE_WAVE_DIR = $(CODECS_DIR)/pure_wave

kernel: portaudio loguru
	$(MAKE) -C $(KERNEL_DIR) all

kernel_clean: 
	$(MAKE) -C $(KERNEL_DIR) clean

client_terminal: loguru
	$(MAKE) -C $(CLIENT_TERMINAL_DIR) all

client_terminal_clean: 
	$(MAKE) -C $(CLIENT_TERMINAL_DIR) clean

codec_pure_wave: loguru
	$(MAKE) -C $(CODEC_PURE_WAVE_DIR) all

codec_pure_wave_clean: 
	$(MAKE) -C $(CODEC_PURE_WAVE_DIR) clean

portaudio:
	cmake -S $(PORTAUDIO_DIR) -B $(PORTAUDIO_DIR)/build
	$(MAKE) -C $(PORTAUDIO_DIR)/build

portaudio_clean:
	rm -rf $(PORTAUDIO_DIR)/build

loguru:
	$(MAKE) -C $(LOGURU_CUSTOM_DIR) all

loguru_clean:
	$(MAKE) -C $(LOGURU_CUSTOM_DIR) clean

all: portaudio loguru kernel client_terminal codec_pure_wave
clean: portaudio_clean loguru_clean kernel_clean client_terminal_clean codec_pure_wave_clean

path_builder:
	mkdir -p build_pack
	mkdir -p build_pack/clients
	mkdir -p build_pack/codecs
	
pack: path_builder
	cp $(KERNEL_DIR)/build/bin/kernel build_pack/
	cp $(CLIENT_TERMINAL_DIR)/build/bin/libterminal.so build_pack/clients
	cp $(CODEC_PURE_WAVE_DIR)/build/bin/libpure_wave.so build_pack/codecs