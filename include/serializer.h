#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "../include/symentry.h"
#include "../include/symtable.h"
#include "../include/relentry.h"
#include "../include/reltable.h"

#include <fstream>

class Serializer {
public:

	Serializer(char* fileName, bool read=false);
	~Serializer();
	
	Serializer* serializeSymTable(SymTable*);
	Serializer* serializeRelTable(RelTable*);
	Serializer* serializeRawData(char* data, int size);

	SymTable* toSymTable();
	RelTable* toRelTable();
	char* toRawData();

private:

	Serializer* serializeSymEntry(SymEntry*);
	Serializer* serializeRelEntry(RelEntry*);
	Serializer* serializeInt(int);
	Serializer* serializeChar(char);
	Serializer* serializeString(std::string);

	std::fstream *stream;
};

#endif