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

	SymEntry(std::string name, std::string section, int value, Locality locality, int size, AccessRights ar, int no = 0);
	~SymEntry();

	void setName(std::string name);
	void setSection(std::string section);
	void setValue(int value);
	void setLocality(Locality locality);
	void setSize(int size);
	void setAccessRights(AccessRights ar);
	void setNo(int no);
	
	std::string getName();
	std::string getSection();
	int getValue();
	Locality getLocality();
	int getSize();
	AccessRights getAccessRights();
	int getNo();

private:

	std::string name;
	std::string section;
	int value;
	Locality locality;
	int size;
	AccessRights accessRights;
	int no;
	
};

#endif SYMENTRY_H