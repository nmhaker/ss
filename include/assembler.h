#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "../include/array.h"
#include "../include/parsetree.h"
#include "../include/symtable.h"
#include "../include/reltable.h"

#include <string>

class Assembler{

public:
    Assembler(char* inputFileName);
    ~Assembler();

	static unsigned char makeFirstByte(int condition, int opcode, int addressing);
	static unsigned char makeSecondByte(int dst, int addressing, int src);
	static void makeAdditionalTwoBytes(char* src, int value);
	static void printByteToHex(char byte);
	static void dumpSectionBytes(char* section, int size);

private:
	Array *lines;	 
	ParseTree *pt;

	SymTable* st;
	
	RelTable* ret_text;
	char *text_bytes;
	int size_of_text;
	int current_size_of_text;
	RelTable* ret_data;
	char *data_bytes;
	int size_of_data;
	int current_size_of_data;
	RelTable* ret_rodata;
	char *rodata_bytes;
	int size_of_rodata;
	int current_size_of_rodata;

	char* getCurrentBytePointer();
	int& getCurrentSectionSize();


	bool firstPass(int);
	bool secondPass(int);

	void assemble();

	//	First pass state variables
	int lc;		//	Location Counter	
	int section_size; //  Location Counter but for section offset
	bool inside_section;	//	To check if inside section for error parsing
	bool first_pass_completed;	//	To track if first pass has completed fully
	bool end_directive_reached; //	To stop parsing 
	std::string current_section;
	std::list<std::string> passed_sections;

	//	Second pass state variables

};

#endif
