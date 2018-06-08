#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../include/array.h"
#include "../include/parsetree.h"
#include "../include/symtable.h"

class Assembler{

public:
    Assembler(char* inputFileName);
    ~Assembler();
        
private:
	Array *lines;	 
	ParseTree *pt;

	SymTable* st;

	void firstPass(int);
	void secondPass(int);

	void assemble();
};

#endif
