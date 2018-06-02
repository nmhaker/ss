all:
	g++ -o prog -std=c++11 main.cpp assembler.cpp 
run:
	./prog fajl.asm
