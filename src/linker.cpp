#include "../include/linker.h"
#include "../include/serializer.h"
#include "../include/objectfile.h"
#include "../include/assembler.h"

#include <iostream>

using namespace std;


Linker::Linker(int num, char** files)
{

	this->numOfObjectFiles = num - 1;

	this->objectFile = new ObjectFile*[numOfObjectFiles];

	for (int i = 0; i < numOfObjectFiles; i++) {
		Serializer *s = new Serializer(files[i + 1], true);
		objectFile[i] = s->makeObjectFile();
		objectFile[i]->getSymTable()->dumpTable();
		Assembler::dumpSectionBytes(objectFile[i]->getBytesText(), objectFile[i]->getTextSize(), -1);
		delete s;
	}

	resolve();
}


Linker::~Linker()
{
	for (int i = 0; i < numOfObjectFiles; i++) {
		if (objectFile[i] != 0)
			delete objectFile[i];
	}
}

bool Linker::resolve()
{

	//	Check overlapping
	if (!checkOverLapping()) {
		cout << endl << "Error: overlapping sections" << endl << flush;
		return false;
	}

	//	Strong symbol < NAME, FILE >
	map<string, int> strongSymbols;

	//	Resolve table symbols and keep track of strong symbols for preventing multiple definitions
	for (int i = 0; i < numOfObjectFiles; i++) {

		bool undefinedFound = false;
		string symbol;

		//	Iterate over current table entries and find undefined symbol

		SymTable* current = objectFile[i]->getSymTable();
		map<int, SymEntry*> current_entries = current->get_entries();

		for (map<int, SymEntry*>::iterator it = current_entries.begin(); it != current_entries.end(); it++) {

			//	Check for strong symbol
			if ((it->second->getSection() != "UNDEFINED") && (it->second->getLocality() == Global)) {
				std::pair<std::map<string, int>::iterator, bool> ret = strongSymbols.insert(std::pair<string, int>(it->second->getName(), i));
				if (ret.second == false) {
					cout << endl << "Linker error: multiple definitons of symbol: " << it->second->getName() << " , first in file " << ret.first->second << endl << flush;
					return false;
				}
			}

			//	Found undefined symbol
			if (it->second->getSection() == "UNDEFINED") {

				//	Set flags
				undefinedFound = true;
				symbol = it->second->getName();

				//	Search the symbol in current second file

				for (int j = 0; (j < numOfObjectFiles) ; j++) {

					if (i == j)
						continue;

					SymTable* other = objectFile[j]->getSymTable();
					map<int, SymEntry*> other_entries = other->get_entries();

					for (map<int, SymEntry*>::iterator it2 = other_entries.begin(); it2 != other_entries.end(); it2++) {

						if ((it->second->getName() == it2->second->getName()) && (it2->second->getSection() != "UNDEFINED")) {

							//	Reset flags
							undefinedFound = false;
							symbol.clear();

							//	Update current symbol table of first file
							it->second->setSection(it2->second->getSection());
							it->second->setValue(it2->second->getValue());
						}
					}
				}

				//	If some symbol is unresolved, that is linker error
				if (undefinedFound) {
					cout << endl << "Linker error: could not resolve symbol " << symbol << endl << flush;
					return false;
				}

			}

		}

		//	DUMP
		cout << endl << " --- RESOLVED [" << i << "] SYMBOL TABLE --- " << endl << flush;
		objectFile[i]->getSymTable()->dumpTable();
	}

	return true;

}

bool Linker::checkOverLapping()
{
	for (int i = 0; i < numOfObjectFiles-1; i++) {
		for (int j = i+1; j < numOfObjectFiles; j++) {
			if (  objectFile[j]->getHeader()->startAddress > objectFile[i]->getHeader()->startAddress){
				if (  objectFile[i]->getHeader()->startAddress + objectFile[i]->getFileSize() > objectFile[j]->getHeader()->startAddress ) {
					return false;
				}
			} else {
				if (objectFile[j]->getHeader()->startAddress + objectFile[j]->getFileSize() > objectFile[i]->getHeader()->startAddress) {
					return false;
				}
			}
		}
	}
	return true;
}
