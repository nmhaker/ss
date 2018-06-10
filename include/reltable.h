#ifndef RELTABLE_H
#define RELTABLE_H

#include "relentry.h"
#include <map>

class RelTable {
public:
	RelTable();
	~RelTable();

	void dumpTable();

	RelTable* addEntry(RelEntry*);
	RelEntry* getEntry(int no);

private:
	std::map<int, RelEntry*> rel_entries;
};

#endif RELTABLE_H