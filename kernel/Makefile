all:
	$(MAKE) -C ./api
	$(MAKE) -C ./items
	clang -I ./items/include -I ./include -I ./api/include -I ../3rd_party/loguru -I ../3rd_party/json-develop/single_include -o ./obj/kernel.o -c ./src/kernel.cpp
	clang -I ./items/include -I ./include -o ./obj/codec_api.o -c ./src/codec_api.cpp 
	clang -I ./items/include -I ./include -I ../3rd_party/loguru -o ./obj/items_manager.o -c ./src/items_manager.cpp 
	clang -I ./items/include -I ./include -o ./obj/codec_file_info.o -c ./src/codec_file_info.cpp 
	clang -I ./items/include -I ./include -I ../3rd_party/loguru  -o ./obj/codec_manager.o -c ./src/codec_manager.cpp -std=c++17
	clang -I ./items/include -I ./include -I ../3rd_party/loguru -I ../3rd_party/portaudio/include -o ./obj/rt_output.o -c ./src/rt_output.cpp 
	clang -I ./items/include -I ./include -I ../codecs/pure_wave/include/ -I ../3rd_party/loguru  -o ./obj/render.o -c ./src/render.cpp 
	clang -o ./bin/kernel ../3rd_party/loguru/obj/loguru.o  api/obj/api.o api/obj/api_audio.o api/obj/api_codec.o api/obj/api_effect.o api/obj/api_fragment.o api/obj/api_playback.o api/obj/api_render.o api/obj/api_session.o api/obj/api_track.o items/obj/audio.o items/obj/effect.o items/obj/fragment.o items/obj/session.o items/obj/track.o obj/codec_api.o obj/codec_manager.o obj/items_manager.o obj/codec_file_info.o obj/render.o obj/rt_output.o obj/kernel.o -L ../3rd_party/portaudio/lib/.libs/ -lportaudio -L -lasound  -L -ljack  -lrt -lm -pthread -L ../codecs/pure_wave/bin/ -ldl -std=c++17 -lstdc++ -lpure_wav  -lpthread -Wl,-rpath,$(CURDIR)/../bin
	cp ./bin/kernel ../bin/
