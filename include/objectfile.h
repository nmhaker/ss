#ifndef OBJECTFILE_H
#define OBJECTFILE_H

//#include "serializer.h"
#include "symtable.h"
#include "reltable.h"

struct elf_header;

class ObjectFile {
public:

	ObjectFile();
	~ObjectFile();

	void setHeader(elf_header*);
	void setSymTable(SymTable*);
	void setRetText(RelTable*);
	void setRetData(RelTable*);
	void setRetRoData(RelTable*);
	void setBytesText(char*, int);
	void setBytesData(char*, int);
	void setBytesRoData(char*, int);
	void setBytesBss(char*, int);

	elf_header* getHeader();
	SymTable* getSymTable();
	RelTable* getRetText();
	RelTable* getRetData();
	RelTable* getRetRodata();
	char* getBytesText();
	char* getBytesData();
	char* getBytesRodata();
	char* getBytesBss();
	int getTextSize();
	int getDataSize();
	int getRodataSize();
	int getBssSize();

	int getFileSize();

private:

	elf_header* header;

	SymTable* symTable;
	RelTable* ret_text;
	RelTable* ret_data;
	RelTable* ret_rodata;
	char* bytes_text;
	int size_of_text;
	char* bytes_data;
	int size_of_data;
	char* bytes_rodata;
	int size_of_rodata;
	char* bytes_bss;
	int size_of_bss;

};


#endif
