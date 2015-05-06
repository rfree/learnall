
all: oneinstance_test.bin

run: oneinstance_test.bin
	# ./oneinstance_test.bin -X
	./oneinstance_test.bin rafal35

oneinstance_test.bin: liboneinstance.o oneinstance_test.o msg_mutex.o libsimpleconvert.o
	g++ --std=c++11 -Wall --pedantic -o oneinstance_test.bin   liboneinstance.o oneinstance_test.o  msg_mutex.o libsimpleconvert.o   -lz -lboost_system
	
liboneinstance.o: liboneinstance.cpp liboneinstance.hpp msg_mutex.hpp libsimpleconvert.hpp
	g++ --std=c++11 -Wall --pedantic  -c liboneinstance.cpp -o liboneinstance.o

libsimpleconvert.o: libsimpleconvert.cpp liboneinstance.hpp msg_mutex.hpp libsimpleconvert.hpp
	g++ --std=c++11 -Wall --pedantic  -c libsimpleconvert.cpp -o libsimpleconvert.o

oneinstance_test.o: oneinstance_test.cpp liboneinstance.hpp msg_mutex.hpp libsimpleconvert.hpp
	g++ --std=c++11 -Wall --pedantic  -c oneinstance_test.cpp -o oneinstance_test.o

msg_mutex.o: msg_mutex.cpp msg_mutex.hpp libsimpleconvert.hpp
	g++ --std=c++11 -Wall --pedantic  -c msg_mutex.cpp -o msg_mutex.o

