all:
	clang -I ./include -I ../../kernel/include -fPIC -o ./obj/pure_wave.o -c ./src/pure_wave.cpp 
	clang -shared -o ./bin/libpure_wav.so ./obj/pure_wave.o 
	cp ./bin/libpure_wav.so ../../bin/

