#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <map>

#include <iostream>

#include "../include/symentry.h"

class SymTable {

public:
	SymTable();
	~SymTable();

	void dumpTable();

	SymTable* addEntry(SymEntry*);
	SymEntry* getEntry(int no);
	SymEntry* getEntry(std::string name);

private:
	std::map<int, SymEntry*> entries;
	
};

#endif SYMTABLE_H