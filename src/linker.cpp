#include "../include/linker.h"
#include "../include/serializer.h"
#include "../include/objectfile.h"


Linker::Linker(char* inputFileName)
{
	this->serializer = new Serializer(inputFileName, true);
	this->objectFile = this->serializer->makeObjectFile();
}


Linker::~Linker()
{
	if (this->serializer != 0)
		delete this->serializer;
	if (this->objectFile != 0)
		delete this->objectFile;
}
