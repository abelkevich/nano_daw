INC = -I ./include -I ../../3rd_party/json-develop/single_include -I ../../3rd_party/loguru -I ../items/include  -I ../include
all:
	clang $(INC) -o ./obj/api.o -c ./src/api.cpp
	clang $(INC) -o ./obj/api_audio.o -c ./src/api_audio.cpp
	clang $(INC) -o ./obj/api_codec.o -c ./src/api_codec.cpp
	clang $(INC) -o ./obj/api_effect.o -c ./src/api_effect.cpp
	clang $(INC) -o ./obj/api_fragment.o -c ./src/api_fragment.cpp
	clang $(INC) -o ./obj/api_playback.o -c ./src/api_playback.cpp
	clang $(INC) -o ./obj/api_render.o -c ./src/api_render.cpp
	clang $(INC) -o ./obj/api_session.o -c ./src/api_session.cpp
	clang $(INC) -o ./obj/api_track.o -c ./src/api_track.cpp