#include "../include/serializer.h"
#include "../include/symtable.h"
#include "../include/symentry.h"
#include "../include/reltable.h"
#include "../include/relentry.h"

#include <iostream>
#include <fstream>
using namespace std;

Serializer::Serializer(char* fileName, bool read, elf_header* flags)
{
	this->flags = 0;

	string str(fileName);
	stream = new fstream();

	if (!read) {
		stream->open(str, ios::out | ios::binary);
		serializeHeader(flags);
	}
	else {
		stream->open(str, ios::in | ios::binary);
	}

	if (!stream) {
		cout << "Error, binary stream is not open for writting binary file" << endl;
		exit(1);
	}

	if(read)
		readHeader();

	lastRawSize = 0;
}

Serializer::~Serializer()
{
	stream->flush();
	stream->close();
	if(stream!=0)
		delete stream;
}

Serializer * Serializer::serializeSymTable(SymTable *st)
{
	int counter = 0;
	for (map<int, SymEntry*>::iterator it = st->get_entries().begin(); it != st->get_entries().end(); it++) {
		if ((it->second->getName() != it->second->getSection()) && (it->second->getLocality() == Local))
			continue;
		counter++;
	}
	serializeInt(counter);
	for (map<int, SymEntry*>::iterator it = st->get_entries().begin(); it != st->get_entries().end(); it++) {
		
		//	Skip local symbols during serialization
		if ( (it->second->getName() != it->second->getSection()) && (it->second->getLocality() == Local))
			continue;

		serializeSymEntry(it->second);
	}
	return this;
}

Serializer * Serializer::serializeSymEntry(SymEntry *se)
{
	serializeString(se->getName());
	serializeString(se->getSection());
	serializeInt(se->getValue());
	serializeInt(se->getLocality());
	serializeInt(se->getSize());
	serializeInt(se->getAccessRights());
	serializeInt(se->getNo());
	return this;
}

Serializer * Serializer::serializeRelTable(RelTable *rt)
{
	serializeInt(rt->get_entries().size());
	for (map<int, RelEntry*>::iterator it = rt->get_entries().begin(); it != rt->get_entries().end(); it++) {
		serializeRelEntry(it->second);
	}
	return this;
}

Serializer * Serializer::serializeRelEntry(RelEntry *re)
{
	serializeInt(re->getOffset());
	serializeInt(re->getRelocationType());
	serializeInt(re->getValue());

	return this;
}

Serializer * Serializer::serializeInt(int a)
{
	stream->write((char*)&a, sizeof(int));
	stream->flush();

	return this;
}

Serializer * Serializer::serializeChar(char c)
{
	stream->write(to_string(c).c_str(), to_string(c).size());
	stream->flush();
	
	return this;
}

Serializer * Serializer::serializeString(string str)
{
	serializeInt(str.size());

	stream->write(str.c_str(), str.size());
	stream->flush();

	return this;

}

void Serializer::serializeHeader(elf_header* flags)
{
	stream->write((char*)flags, sizeof(elf_header));
}

elf_header * Serializer::readHeader()
{
	char *elfHeader = new char[sizeof(elf_header)];
	stream->read(elfHeader, sizeof(elf_header));
	this->flags = (elf_header*)elfHeader;
	return this->flags;
}

ObjectFile * Serializer::makeObjectFile()
{
	ObjectFile* objFile = new ObjectFile();

	objFile->setHeader(flags);

	objFile->setSymTable(toSymTable());
	objFile->setRetData(toRelTable());
	objFile->setRetRoData(toRelTable());
	objFile->setRetText(toRelTable());

	char*data = toRawData();

	objFile->setBytesData(data, lastRawSize);
	data = toRawData();
	objFile->setBytesRoData(data, lastRawSize);
	data = toRawData();
	objFile->setBytesText(data, lastRawSize);
	data = toRawData();
	objFile->setBytesBss(data, lastRawSize);

	return objFile;
}

int Serializer::readInt()
{
	int a;
	stream->read((char*)&a, sizeof(int));
	return a;
}

char Serializer::readChar()
{
	char c;
	stream->read((char*)&c, sizeof(char));
	return c;
}

std::string Serializer::readString()
{
	int size;
	stream->read((char*)&size, sizeof(int));
	char bytes[32];
	stream->read(bytes, size);
	bytes[size] = '\0';
	string str(bytes);
	return str;
}

Serializer * Serializer::serializeRawData(char * data, int size)
{
	serializeInt(size);
	stream->write(data, size);
	stream->flush();
	return this;
}

SymTable * Serializer::toSymTable()
{
	int size = readInt();

	SymTable *st = new SymTable();
	for (int i = 0; i < size; i++) {
		string name = readString();
		string section = readString();
		int value = readInt();
		int locality = readInt();
		int size = readInt();
		int accessRights = readInt();
		int no = readInt();

		string string_name(name);
		string string_section(section);

		SymEntry *se = new SymEntry(name, section, value, (Locality)locality,  size, (AccessRights)accessRights, no);
		st->addPairEntry(no, se);
	}

	return st;
}

RelTable * Serializer::toRelTable()
{
	int size = readInt();
		
	RelTable* relTable = new RelTable();
	for (int i = 0; i < size; i++) {
		int offset = readInt();
		int type = readInt();
		int symbol = readInt();
		RelEntry* entry = new RelEntry(offset, (RelocationType)type, symbol);
		relTable->addEntry(entry);
	}

	return relTable;
}

char * Serializer::toRawData()
{
	int size = readInt();
	lastRawSize = size;
	char *bytes = new char[size];
	stream->read(bytes, size);
	return bytes;
}
