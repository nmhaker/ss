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

	void finalizeTable(bool);

	SymTable* addSectionEntry(SymEntry*);
	SymTable* addSymbolEntry(SymEntry*);
	SymEntry* getEntry(int no);
	SymEntry* getEntry(std::string name);
	int getNo(std::string name);

	int getSymbolEntriesSize();
	int getFinalEntriesSize();

private:
	std::map<int, SymEntry*> section_entries;
	std::map<int, SymEntry*> symbol_entries;
	std::map<int, SymEntry*> final_entries;
};

#endif SYMTABLE_H