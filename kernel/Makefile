all:
	clang -I ./include -o ./obj/kernel.o -c ./src/kernel.cpp
	clang -I ./include -o ./obj/terminal.o -c ./src/terminal.cpp 
	clang -I ./include -I ../codecs/pure_wave/include/ -o ./obj/session.o -c ./src/session.cpp 
	clang -I ./include -I ../codecs/pure_wave/include/ -o ./obj/render.o -c ./src/render.cpp 
	clang -o ./bin/kernel ./obj/kernel.o ./obj/terminal.o ./obj/session.o ./obj/render.o -L ../codecs/pure_wave/bin/ -lpure_wav -lstdc++ -lpthread -Wl,-rpath,$(CURDIR)/../bin
	cp ./bin/kernel ../bin/
