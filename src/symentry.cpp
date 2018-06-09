#include "../include/symentry.h"

SymEntry::SymEntry(std::string name, std::string section, int value, Locality locality, int size, AccessRights ar)
{
	this->name = name;
	this->section = section;
	this->value = value;
	this->locality = locality;
	this->size = size;
	this->accessRights = ar;
}

SymEntry::~SymEntry()
{
}

void SymEntry::setName(std::string name)
{
	this->name = name;
}

void SymEntry::setSection(std::string section)
{
	this->section = section;
}

void SymEntry::setValue(int value)
{
	this->value = value;
}

void SymEntry::setLocality(Locality locality)
{
	this->locality = locality;
}

void SymEntry::setSize(int size)
{
	this->size = size;
}

void SymEntry::setAccessRights(AccessRights ar)
{
	this->accessRights = ar;
}

std::string SymEntry::getName()
{
	return this->name;
}

std::string SymEntry::getSection()
{
	return this->section;
}

int SymEntry::getValue()
{
	return this->value;
}

Locality SymEntry::getLocality()
{
	return this->locality;
}

int SymEntry::getSize()
{
	return this->size;
}

AccessRights SymEntry::getAccessRights()
{
	return this->accessRights;
}
