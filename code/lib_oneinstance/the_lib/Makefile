
all: run

run: oneinstance_test.bin
	./oneinstance_test.bin rafal33

oneinstance_test.bin: liboneinstance.o oneinstance_test.o msg_mutex.o
	g++ --std=c++11 --pedantic -o oneinstance_test.bin   liboneinstance.o oneinstance_test.o  msg_mutex.o   -lz -lboost_system
	
liboneinstance.o: liboneinstance.cpp liboneinstance.hpp msg_mutex.hpp
	g++ --std=c++11 --pedantic  -c liboneinstance.cpp -o liboneinstance.o

oneinstance_test.o: oneinstance_test.cpp liboneinstance.hpp msg_mutex.hpp
	g++ --std=c++11 --pedantic  -c oneinstance_test.cpp -o oneinstance_test.o

msg_mutex.o: msg_mutex.cpp msg_mutex.hpp
	g++ --std=c++11 --pedantic  -c msg_mutex.cpp -o msg_mutex.o

