#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../include/array.h"
#include "../include/parsetree.h"
#include "../include/symtable.h"

#include <string>

class Assembler{

public:
    Assembler(char* inputFileName);
    ~Assembler();
        
private:
	Array *lines;	 
	ParseTree *pt;

	SymTable* st;

	bool firstPass(int);
	bool secondPass(int);

	void assemble();

	//	First pass state variables
	int lc;		//	Location Counter	
	bool inside_section;	//	To check if inside section for error parsing
	bool first_pass_completed;	//	To track if first pass has completed fully
	bool end_directive_reached; //	To stop parsing 

	std::string current_section;

	std::list<std::string> passed_sections;

};

#endif
