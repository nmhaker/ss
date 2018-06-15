#ifndef LINKER_H
#define LINKER_H

#include "objectfile.h"
#include "serializer.h"

class Linker
{
public:
	Linker(int num, char** files);
	~Linker();

	char* makeExecutable();

private:

	bool resolve();
	bool checkOverLapping();
	bool resolveUndefinedSymbols();
	bool relocateEntries();

	ObjectFile** objectFile;
	int numOfObjectFiles;

	int START;
};

#endif
