#include "../include/symtable.h"

#include <iostream>
using namespace std;
#include <map>

SymTable::SymTable() {
	//entries.clear();
}

SymTable::~SymTable() {
	for (map<int, SymEntry*>::iterator it = entries.begin(); it != entries.end(); it++) {
		delete it->second;
		entries.erase(it);
	}
}

void SymTable::dumpTable() {
	if (!entries.empty()) {
		cout << "|   IME   |   SEKCIJA   |   VREDNOST   |   VIDLJIVOST   |   REDNI BROJ   |   VELICINA   |   PRAVA PRISTUPA   |" << endl;
		cout << "|------------------------------------------------------------------------------------------------------------|" << endl;
		cout << "|    /    |     UND     |      0h      |        L       |       0        |        0      |      /            |" << endl;
		cout << "|------------------------------------------------------------------------------------------------------------|" << endl;
		for (map<int, SymEntry*>::iterator it = entries.begin(); it != entries.end(); it++) {
			cout << "|  " << it->second->getName() << "  |  " << it->second->getSection() << "  |  " << it->second->getValue() << "  |  " << it->second->getLocality() << "  |  " << it->first << "  |  " << it->second->getSize() << "  |  " << it->second->getAccessRights() << "  |" << endl;
		}
	}
	else {
		cout << "Prazna tabela simbola" << endl << flush;
	}
}

SymTable* SymTable::addEntry(SymEntry *entry)
{
	entries.insert(pair<int, SymEntry*>(entries.size()+1, entry));
	return this;
}

SymEntry* SymTable::getEntry(int no)
{
	return entries.at(no);
}
