#ifndef LINKER_H
#define LINKER_H

#include "objectfile.h"
#include "serializer.h"

class Linker
{
public:
	Linker(int num, char** files);
	~Linker();

	bool resolve();

private:

	bool checkOverLapping();

	ObjectFile** objectFile;
	int numOfObjectFiles;
};

#endif
