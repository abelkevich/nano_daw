INC = -I ./include -I ../../3rd_party/json-develop/include -I ../../3rd_party/loguru -I ../items/include  -I ../include
all:
	clang $(INC) -o ./obj/audio.o -c ./src/audio.cpp
	clang $(INC) -o ./obj/effect.o -c ./src/effect.cpp
	clang $(INC) -o ./obj/fragment.o -c ./src/fragment.cpp
	clang $(INC) -o ./obj/session.o -c ./src/session.cpp
	clang $(INC) -o ./obj/track.o -c ./src/track.cpp