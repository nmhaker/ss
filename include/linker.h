#ifndef LINKER_H
#define LINKER_H

#include "objectfile.h"
#include "serializer.h"

class Linker
{
public:
	Linker(char* inputFileName);
	~Linker();

private:
	Serializer* serializer;
	ObjectFile* objectFile;
};

#endif
