#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../include/array.h"

class Assembler{

public:
    Assembler(char* inputFileName);
    ~Assembler();
        
private:
	Array *lines;	 

	void test();
};

#endif
