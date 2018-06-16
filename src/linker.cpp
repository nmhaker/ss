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

	if (!resolve())
		exit(1);
}


Linker::~Linker()
{
	for (int i = 0; i < numOfObjectFiles; i++) {
		if (objectFile[i] != 0)
			delete objectFile[i];
	}
	delete[] objectFile;
}

bool Linker::resolve()
{

	//	Check overlapping
	if (!checkOverLapping()) {
		cout << endl << "Error: overlapping sections" << endl << flush;
		return false;
	}

	//	Resolve undefined symbols
	if (!resolveUndefinedSymbols())
		return false;
	
	//	Rewrite bytes from relocation entries
	if (!relocateEntries())
		return false;

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

bool Linker::resolveUndefinedSymbols()
{
	//	Strong symbol < NAME, FILE >
	map<string, int> strongSymbols;

	//	Symbol start must be defined for emulator
	START = -1;
	
	//	Resolve table symbols and keep track of strong symbols for preventing multiple definitions
	for (int i = 0; i < numOfObjectFiles; i++) {

		bool undefinedFound = false;
		string symbol;

		//	Iterate over current table entries and find undefined symbol

		SymTable* current = objectFile[i]->getSymTable();
		map<int, SymEntry*> current_entries = current->get_entries();

		for (map<int, SymEntry*>::iterator it = current_entries.begin(); it != current_entries.end(); it++) {

			if (it->second->getName() == "START")
				START = it->second->getValue();

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

	if(START >= 0)
		return true;
	else {
		cout << endl << "Linker error: could not find START symbol" << endl << flush;
		return false;
	}
}

bool Linker::relocateEntries()
{
	//	Take object files
	for (int i = 0; i < numOfObjectFiles; i++) {
		
		ObjectFile* object = objectFile[i];

		SymTable* symTable = object->getSymTable();

		elf_header* header = object->getHeader();
				
		if (header->has_rodata_ret) {

			RelTable* rodata_ret = object->getRetRodata();
			map<int, RelEntry*> rodata_entries = rodata_ret->get_entries();

			for (map<int, RelEntry*>::iterator it = rodata_entries.begin(); it != rodata_entries.end(); it++) {

				//	Byte array
				char* bytes = object->getBytesRodata();

				//	Get offset to bytes which need modification
				int offset = it->second->getOffset();

				//	Get value of resolved symbol
				int value = symTable->getEntry(it->second->getValue())->getValue();

				char first_byte = bytes[offset];
				char second_byte = bytes[offset+1];
				
				short coded_value = 0;
				coded_value |= first_byte;
				coded_value |= (second_byte << 8);

				//	Absolute relocation
				if (it->second->getRelocationType() == R_386_16) {

					value += coded_value;

				} 
				//	PC Relative relocation
				else if (it->second->getRelocationType() == R_386_PC16) {
					
					value += coded_value - (symTable->getEntry(".rodata")->getValue() + offset);

				} else {
					cout << endl << "Linker error: unsupported relocation type" << endl << flush;
					return false;
				}

				first_byte = value & 255;
				second_byte = ( (value & (255 << 8)) >> 8);

				bytes[offset] = first_byte;
				bytes[offset + 1] = second_byte;

			}

			cout << endl << "DUMPING RELOCATED BYTES FOR SECTION RODATA" << endl << flush;
			Assembler::dumpSectionBytes(objectFile[i]->getBytesRodata(), objectFile[i]->getRodataSize(), -1);
			cout << endl << flush;

		} 
		if (header->has_data_ret) {

			RelTable* data_ret = object->getRetData();
			map<int, RelEntry*> data_entries = data_ret->get_entries();

			for (map<int, RelEntry*>::iterator it = data_entries.begin(); it != data_entries.end(); it++) {

				//	Byte array
				char* bytes = object->getBytesData();

				//	Get offset to bytes which need modification
				int offset = it->second->getOffset();

				//	Get value of resolved symbol
				int value = symTable->getEntry(it->second->getValue())->getValue();

				char first_byte = bytes[offset];
				char second_byte = bytes[offset+1];

				short coded_value = 0;
				coded_value |= first_byte;
				coded_value |= (second_byte << 8);

				//	Absolute relocation
				if (it->second->getRelocationType() == R_386_16) {

					value += coded_value;

				}
				//	PC Relative relocation
				else if (it->second->getRelocationType() == R_386_PC16) {

					value += coded_value - (symTable->getEntry(".data")->getValue() + offset);

				}
				else {
					cout << endl << "Linker error: unsupported relocation type" << endl << flush;
					return false;
				}

				first_byte = value & 255;
				second_byte = ((value & (255 << 8)) >> 8);

				bytes[offset] = first_byte;
				bytes[offset + 1] = second_byte;

			}

			cout << endl << "DUMPING RELOCATED BYTES FOR SECTION DATA" << endl << flush;
			Assembler::dumpSectionBytes(objectFile[i]->getBytesData(), objectFile[i]->getDataSize(), -1);
			cout << endl << flush;

		} 
		if (header->has_text_ret) {

			RelTable* text_ret = object->getRetText();
			map<int, RelEntry*> text_entries = text_ret->get_entries();
			for (map<int, RelEntry*>::iterator it = text_entries.begin(); it != text_entries.end(); it++) {

				//	Byte array
				char* bytes = object->getBytesText();

				//	Get offset to bytes which need modification
				int offset = it->second->getOffset();

				//	Get value of resolved symbol
				int value = symTable->getEntry(it->second->getValue())->getValue();

				char first_byte = bytes[offset];
				char second_byte = bytes[offset+1];

				short coded_value = 0;
				coded_value |= first_byte;
				coded_value |= (second_byte << 8);

				//	Absolute relocation
				if (it->second->getRelocationType() == R_386_16) {

					value += coded_value;

				}
				//	PC Relative relocation
				else if (it->second->getRelocationType() == R_386_PC16) {

					value += coded_value - (symTable->getEntry(".text")->getValue() + offset);

				}
				else {
					cout << endl << "Linker error: unsupported relocation type" << endl << flush;
					return false;
				}

				first_byte = value & 255;
				second_byte = ((value & (255 << 8)) >> 8);

				bytes[offset] = first_byte;
				bytes[offset + 1] = second_byte;

			}

			cout << endl << "DUMPING RELOCATED BYTES FOR SECTION TEXT" << endl << flush;
			Assembler::dumpSectionBytes(objectFile[i]->getBytesText(), objectFile[i]->getTextSize(), -1);
			cout << endl << flush;
		}

	}


	return true;
}

char* Linker::makeExecutable()
{
	//	Memory for emulator, that is its virtual address space
	char* v_space = new char[1 << 16];
	for (int i = 0; i < (1 << 16); i++)
		v_space[i] = 0;

	for (int i = 0; i < numOfObjectFiles; i++) {

		ObjectFile* file = objectFile[i];
		map<int, SymEntry*> entries = file->getSymTable()->get_entries();

		int	offset = file->getHeader()->startAddress;

		int counter = 1;

		try {
			while (entries.at(counter)->getName() == entries.at(counter)->getSection()) {

				char* bytes = 0;
				int size = 0;

				if (entries.at(counter)->getName() == ".rodata") {
					bytes = file->getBytesRodata();
					size = file->getRodataSize();
				}
				else if (entries.at(counter)->getName() == ".data") {
					bytes = file->getBytesData();
					size = file->getDataSize();
				}
				else if (entries.at(counter)->getName() == ".bss") {
					bytes = file->getBytesBss();
					size = file->getBssSize();
				}
				else if (entries.at(counter)->getName() == ".text") {
					bytes = file->getBytesText();
					size = file->getTextSize();
				}

				for (int j = offset; j < offset + size; j++) {
					v_space[j] = bytes[j-offset];
				}

				offset += size;
				counter++;
			}
		} catch (out_of_range e) {}
	}

	return v_space;
}

int Linker::getStartLocation()
{
	return this->START;
}

ObjectFile ** Linker::getObjectFiles()
{
	return this->objectFile;
}

int Linker::getNumFiles()
{
	return this->numOfObjectFiles;
}
