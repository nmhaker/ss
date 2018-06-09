#ifndef SYMENTRY_H
#define SYMENTRY_H

#include <string>

enum Locality {
	Local,
	Global
};

enum AccessRights {
	NONE,
	READ,
	WRITE,
	READ_WRITE
};

class SymEntry {
public:

	SymEntry(std::string name, std::string section, int value, Locality locality, int size, AccessRights ar);
	~SymEntry();

	void setName(std::string name);
	void setSection(std::string section);
	void setValue(int value);
	void setLocality(Locality locality);
	void setSize(int size);
	void setAccessRights(AccessRights ar);
	
	std::string getName();
	std::string getSection();
	int getValue();
	Locality getLocality();
	int getSize();
	AccessRights getAccessRights();

private:

	std::string name;
	std::string section;
	int value;
	Locality locality;
	int size;
	AccessRights accessRights;
	
};

#endif SYMENTRY_H