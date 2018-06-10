#include "../include/relentry.h"

RelEntry::RelEntry(int offset, RelocationType relocationType, int value)
{
	this->offset = offset;
	this->relocationType = relocationType;
	this->value = value;
}

RelEntry::~RelEntry()
{
}

void RelEntry::setOffset(int offset)
{
	this->offset = offset;
}

void RelEntry::setRelocationType(RelocationType rt)
{
	this->relocationType = relocationType;
}

void RelEntry::setValue(int value)
{
	this->value = value;
}

int RelEntry::getOffset()
{
	return this->offset;
}

RelocationType RelEntry::getRelocationType()
{
	return this->relocationType;
}

int RelEntry::getValue()
{
	return this->value;
}
