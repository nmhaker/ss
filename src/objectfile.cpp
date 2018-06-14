#include "../include/objectfile.h"

ObjectFile::ObjectFile()
{
	this->symTable = 0;

	this->bytes_bss = 0;
	this->bytes_data = 0;
	this->bytes_rodata = 0;
	this->bytes_text = 0;

	this->ret_data = 0;
	this->ret_rodata = 0;
	this->ret_text = 0;

	this->size_of_bss = 0;
	this->size_of_data = 0;
	this->size_of_rodata = 0;
	this->size_of_text = 0;
}

ObjectFile::~ObjectFile()
{
	if (this->symTable != 0)
		delete symTable;
	if (this->bytes_bss != 0)
		delete bytes_bss;
	if (this->bytes_data != 0)
		delete bytes_data;
	if (this->bytes_rodata != 0)
		delete bytes_rodata;
	if (this->bytes_text != 0)
		delete bytes_text;
	if (this->ret_data != 0)
		delete this->ret_data;
	if (this->ret_rodata != 0 )
		delete this->ret_rodata;
	if (this->ret_text != 0 )
		delete this->ret_text;
}

void ObjectFile::setSymTable(SymTable *symTable)
{
	this->symTable = symTable;
}

void ObjectFile::setRetText(RelTable *ret_text)
{
	this->ret_text = ret_text;
}

void ObjectFile::setRetData(RelTable *ret_data)
{
	this->ret_data = ret_data;
}

void ObjectFile::setRetRoData(RelTable *rodata)
{
	this->ret_rodata = rodata;
}

void ObjectFile::setBytesText(char *bytes_text, int size)
{
	this->bytes_text = bytes_text;
	this->size_of_text = size;
}

void ObjectFile::setBytesData(char *bytes_data, int size)
{
	this->bytes_data = bytes_data;
	this->size_of_data = size;
}

void ObjectFile::setBytesRoData(char *rodata, int size)
{
	this->bytes_rodata = rodata;
	this->size_of_rodata = size;
}

void ObjectFile::setBytesBss(char *bss, int size)
{
	this->bytes_bss = bss;
	this->size_of_bss = size;
}

SymTable * ObjectFile::getSymTable()
{
	return this->symTable;
}

RelTable * ObjectFile::getRetText()
{
	return this->ret_text;
}

RelTable * ObjectFile::getRetData()
{
	return this->ret_data;
}

RelTable * ObjectFile::getRetRodata()
{
	return this->ret_rodata;
}

char * ObjectFile::getBytesText()
{
	return this->bytes_text;
}

char * ObjectFile::getBytesData()
{
	return this->bytes_data;
}

char * ObjectFile::getBytesRodata()
{
	return this->bytes_rodata;
}

char * ObjectFile::getBytesBss()
{
	return this->bytes_bss;
}

int ObjectFile::getTextSize()
{
	return this->size_of_text;
}

int ObjectFile::getDataSize()
{
	return this->size_of_data;
}

int ObjectFile::getRodataSize()
{
	return this->size_of_rodata;
}

int ObjectFile::getBssSize()
{
	return this->size_of_bss;
}
