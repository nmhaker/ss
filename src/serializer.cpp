#include "../include/serializer.h"
#include "../include/symtable.h"
#include "../include/symentry.h"
#include "../include/reltable.h"
#include "../include/relentry.h"

#include <iostream>
#include <fstream>
using namespace std;

Serializer::Serializer(char* fileName, bool read)
{

	string str("objektni.o");
	stream = new fstream();
	if (!read)
		stream->open(str, ios::out | ios::binary);
	else
		stream->open(str, ios::in | ios::binary);
	if (!stream) {
		cout << "Error, binary stream is not open for writting binary file" << endl;
	}

}

Serializer::~Serializer()
{
	stream->flush();
	stream->close();
	delete stream;
}

Serializer * Serializer::serializeSymTable(SymTable *st)
{
	serializeInt(st->get_entries().size());
	for (map<int, SymEntry*>::iterator it = st->get_entries().begin(); it != st->get_entries().end(); it++) {
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
	*stream << a;
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

Serializer * Serializer::serializeRawData(char * data, int size)
{
	serializeInt(size);
	stream->write(data, size);
	stream->flush();
	return this;
}

SymTable * Serializer::toSymTable()
{
	int size;
	*stream >> size;

	SymTable *st = new SymTable();
	for (int i = 0; i < size; i++) {
		int size_str_name;
		*stream >> size_str_name;
		char str_name[32];
		stream->read(str_name, size_str_name);
		int size_str_section;
		*stream >> size_str_section;
		char str_section[32];
		stream->read(str_section, size_str_section);
		int value;
		int locality;
		int size;
		int accessRights;
		int no;
		*stream >> value;
		*stream >> locality;
		*stream >> size;
		*stream >> accessRights;
		*stream >> no;

		string string_name(str_name);
		string string_section(str_section);
		SymEntry *se = new SymEntry(string_name, string_section, value, (Locality)locality,  size, (AccessRights)accessRights, no);
		st->addEntry(se);
	}

	return st;
}

RelTable * Serializer::toRelTable()
{
	int size;
	*stream >> size;
		
	RelTable* relTable = new RelTable();
	for (int i = 0; i < size; i++) {
		int offset;
		int type;
		int symbol;
		*stream >> offset;
		*stream >> type;
		*stream >> symbol;
		RelEntry* entry = new RelEntry(offset, (RelocationType)type, symbol);
		relTable->addEntry(entry);
	}

	return relTable;
}

char * Serializer::toRawData()
{
	int size;
	*stream >> size;
	char *bytes = new char[size];
	stream->read(bytes, size);
	return bytes;
}
