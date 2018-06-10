#include "../include/reltable.h"

#include <iostream>
using namespace std;

RelTable::RelTable()
{
}

RelTable::~RelTable()
{
	for (map<int, RelEntry*>::iterator it = rel_entries.begin(); it != rel_entries.end(); it++) {
		delete it->second;
		rel_entries.erase(it);
	}
}

void RelTable::dumpTable()
{
	cout << "  OFFSET  |    TIP    |    VREDNOST  " << endl << flush;
	for (map<int, RelEntry*>::iterator it = rel_entries.begin(); it != rel_entries.end(); it++) {
		cout << it->second->getOffset() << " " << ((it->second->getRelocationType()==R_386_32) ? "R_386_32" : "R_386_PC32") << " " << it->second->getValue() << endl << flush;
	}
}

RelTable * RelTable::addEntry(RelEntry *re)
{
	rel_entries.insert(pair<int, RelEntry*>(rel_entries.size()+1,re));
	return this;
}

RelEntry * RelTable::getEntry(int no)
{
	return rel_entries.at(no);
}
