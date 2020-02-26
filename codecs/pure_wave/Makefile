all:
	clang -I ./include -I ../../kernel/include -fPIC -o ./obj/pure_wave.o -c ./src/pure_wave.cpp -lstdc++ -std=c++17 
	clang -shared -o ./bin/libpure_wav.so ./obj/pure_wave.o 
	cp ./bin/libpure_wav.so ../../bin/
