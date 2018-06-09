#include "../include/symtable.h"

#include <iostream>
using namespace std;
#include <map>

SymTable::SymTable() {
	//entries.clear();
}

SymTable::~SymTable() {
	for (map<int, SymEntry*>::iterator it = final_entries.begin(); it != final_entries.end(); it++) {
		delete it->second;
		final_entries.erase(it);
	}
}

void SymTable::dumpTable() {
	if (!final_entries.empty()) {
		cout << "|   REDNI BROJ   |   IME   |   SEKCIJA   |   VREDNOST   |   VIDLJIVOST   |   VELICINA   |   PRAVA PRISTUPA   |" << endl;
		cout << "|------------------------------------------------------------------------------------------------------------|" << endl;
		cout << " 0 / UND 0 L 0 / " << endl;
		for (map<int, SymEntry*>::iterator it = final_entries.begin(); it != final_entries.end(); it++) {

			cout << " " << it->first <<  " " << it->second->getName() << " " << it->second->getSection() << " " << it->second->getValue() << " " << ((it->second->getLocality()==0) ? "Local" : "Global")  << " "  << it->second->getSize() << " " << it->second->getAccessRights() << " " << endl;

		}
	}
	else {
		cout << "Prazna tabela simbola" << endl << flush;
	}
}

void SymTable::finalizeTable()
{
	int counter = 1;
	for (map<int, SymEntry*>::const_iterator it = section_entries.begin(); it != section_entries.end(); it++) {
		final_entries.insert(pair<int, SymEntry*>(counter, it->second));
		counter++;
	}
	for (map<int, SymEntry*>::const_iterator it = symbol_entries.begin(); it != symbol_entries.end(); it++) {
		final_entries.insert(pair<int, SymEntry*>(counter, it->second));
		counter++;
	}
}

SymTable* SymTable::addSectionEntry(SymEntry *entry)
{
	section_entries.insert(pair<int, SymEntry*>(section_entries.size()+1, entry));
	return this;
}

SymTable* SymTable::addSymbolEntry(SymEntry *entry)
{
	symbol_entries.insert(pair<int, SymEntry*>(symbol_entries.size()+1, entry));
	return this;
}

SymEntry* SymTable::getEntry(int no)
{
	return final_entries.at(no);
}

SymEntry * SymTable::getEntry(std::string name)
{
	for (map<int, SymEntry*>::iterator it = section_entries.begin(); it != section_entries.end(); it++) {
		if (it->second->getName() == name) {
			return it->second;
		}
	}
	for (map<int, SymEntry*>::iterator it = symbol_entries.begin(); it != symbol_entries.end(); it++) {
		if (it->second->getName() == name) {
			return it->second;
		}
	}	
	return 0;
}
