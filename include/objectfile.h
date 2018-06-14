#ifndef OBJECTFILE_H
#define OBJECTFILE_H

#include "symtable.h"
#include "reltable.h"

class ObjectFile {
public:

	ObjectFile();
	~ObjectFile();

	void setSymTable(SymTable*);
	void setRetText(RelTable*);
	void setRetData(RelTable*);
	void setRetRoData(RelTable*);
	void setBytesText(char*, int);
	void setBytesData(char*, int);
	void setBytesRoData(char*, int);
	void setBytesBss(char*, int);

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

private:

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
