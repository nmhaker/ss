#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <map>

#include <iostream>

#include "symentry.h"

class SymTable {

public:
	SymTable();
	~SymTable();

	void dumpTable();

	void finalizeTable(bool);

	SymTable* addSectionEntry(SymEntry*);
	SymTable* addSymbolEntry(SymEntry*);
	SymTable* addEntry(SymEntry*);

	//	Needed by linker
	SymTable* addPairEntry(int, SymEntry*);

	SymEntry* getEntry(int no);
	SymEntry* getEntry(std::string name);

	int getNo(std::string name);

	int getSymbolEntriesSize();
	int getFinalEntriesSize();

	std::map<int, SymEntry*>& get_entries();

private:
	std::map<int, SymEntry*> section_entries;
	std::map<int, SymEntry*> symbol_entries;
	std::map<int, SymEntry*> final_entries;
};

#endif SYMTABLE_H