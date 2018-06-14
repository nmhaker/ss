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

	std::map<int, RelEntry*>& get_entries();

private:
	std::map<int, RelEntry*> rel_entries;
};

#endif RELTABLE_H