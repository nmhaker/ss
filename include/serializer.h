#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "symentry.h"
#include "symtable.h"
#include "relentry.h"
#include "reltable.h"
#include "objectfile.h"

#include <fstream>

struct elf_header {
	elf_header() {
		has_data_ret = 0;
		has_rodata_ret = 0;
		has_text_ret = 0;
		has_raw_data = 0;
		has_raw_rodata = 0;
		has_raw_text = 0;
		has_raw_bss = 0;
		startAddress = 0;
	}
	char has_data_ret;
	char has_rodata_ret;
	char has_text_ret;
	char has_raw_data;
	char has_raw_rodata;
	char has_raw_text;
	char has_raw_bss;
	int startAddress;
};

class Serializer {
public:

	Serializer(char* fileName, bool read, elf_header* flags=0);
	~Serializer();
	
	Serializer* serializeSymTable(SymTable*);
	Serializer* serializeRelTable(RelTable*);
	Serializer* serializeRawData(char* data, int size);

	SymTable* toSymTable();
	RelTable* toRelTable();
	char* toRawData();
	
	ObjectFile* makeObjectFile();

private:

	elf_header* flags;

	Serializer* serializeSymEntry(SymEntry*);
	Serializer* serializeRelEntry(RelEntry*);
	Serializer* serializeInt(int);
	Serializer* serializeChar(char);
	Serializer* serializeString(std::string);

	void serializeHeader(elf_header*);
	elf_header* readHeader();


	int readInt();
	char readChar();
	std::string readString();

	std::fstream *stream;

	int lastRawSize;
};

#endif