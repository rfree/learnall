
all: run

run: a.elf
	./a.elf
	
a.elf: a.cpp
	g++ --std=c++11 --pedantic a.cpp -lz -lboost_system  -o a.elf


