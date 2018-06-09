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
		cout << "|   REDNI BROJ   |   IME   |   SEKCIJA   |   VREDNOST   |   VIDLJIVOST   |   VELICINA   |   PRAVA PRISTUPA   |" << endl;
		cout << "|------------------------------------------------------------------------------------------------------------|" << endl;
		cout << "|   0    |    /    |     UND     |      0h      |        L       |       0        |        /      |" << endl;
		cout << "|------------------------------------------------------------------------------------------------------------|" << endl;
		for (map<int, SymEntry*>::iterator it = entries.begin(); it != entries.end(); it++) {
			cout << "|  " << it->first <<  "|  " << it->second->getName() << "  |  " << it->second->getSection() << "  |  " << it->second->getValue() << "  |  " << it->second->getLocality() << "  |  "  << it->second->getSize() << "  |  " << it->second->getAccessRights() << "  |" << endl;
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

SymEntry * SymTable::getEntry(std::string name)
{
	for (map<int, SymEntry*>::iterator it = entries.begin(); it != entries.end(); it++) {
		if (it->second->getName() == name) {
			return it->second;
		}
	}
	return 0;
}
