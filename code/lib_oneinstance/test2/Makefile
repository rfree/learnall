
all: run

run: oneinstance_test.bin
	./oneinstance_test.bin rafal28

oneinstance_test.bin: liboneinstance.o oneinstance_test.o
	g++ --std=c++11 --pedantic -o oneinstance_test.bin   liboneinstance.o oneinstance_test.o   -lz -lboost_system
	
	
liboneinstance.o: liboneinstance.cpp
	g++ --std=c++11 --pedantic  -c liboneinstance.cpp -o liboneinstance.o

oneinstance_test.o: oneinstance_test.cpp
	g++ --std=c++11 --pedantic  -c oneinstance_test.cpp -o oneinstance_test.o

