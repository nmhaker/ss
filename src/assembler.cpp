#include "../include/assembler.h"
#include "../include/array.h"
#include "../include/parsetree.h"
#include "../include/relentry.h"

#include <iostream>
#include <fstream>
#include <list>

using namespace std;

Assembler::Assembler(char* inputFileName, char* startingPosition){

//	cout << "Konstruktor" << endl;	
//	cout << "Input file: " << inputFileName << endl;
	this->startingPosition = stoi(startingPosition);
	this->inputFileName = inputFileName;

	cout << endl << "Pocetna pozicija: " << this->startingPosition << endl << endl << flush;

	ifstream inputStream(inputFileName, ios::in);
	if(inputStream.is_open()){
	
		list<string> l_lines;

		while(!inputStream.eof()){
			string new_line;	
			getline(inputStream, new_line);
			l_lines.push_back(new_line);
		}

		inputStream.close();

		lines = new Array(&l_lines);

	}else{
		cout << "Nije otvoren ulazni stream" << endl;
		int a;
		cin >> a;
		exit(1);
	}
	inputStream.close();

	st = new SymTable();

	lc = this->startingPosition;
	section_size = 0;
	inside_section = false;
	first_pass_completed = true;
	end_directive_reached = false;

	ret_text = 0;
	ret_data = 0;
	ret_rodata = 0;
	text_bytes = 0;
	data_bytes = 0;
	rodata_bytes = 0;
	bss_bytes = 0;
	current_size_of_text = 0;
	current_size_of_data = 0;
	current_size_of_rodata = 0;

	ret_text = new RelTable();
	ret_data = new RelTable();
	ret_rodata = new RelTable();


	assemble();
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
	if(lines!=0)
		delete lines;
	if(pt!=0)
		delete pt;
	if(st!=0)
		delete st;
	if(ret_text!=0)
		delete ret_text;
	if(ret_data!=0)
		delete ret_data;
	if(ret_rodata!=0)
		delete ret_rodata;
	if(text_bytes!=0)
		delete text_bytes;
	if(data_bytes!=0)
		delete data_bytes;
	if(rodata_bytes!=0)
		delete rodata_bytes;
	if (bss_bytes != 0)
		delete bss_bytes;
	if (serializer != 0)
		delete serializer;
}

void Assembler::assemble() {

	pt = new ParseTree();

	//	First pass, go through all read lines from the file
	for(int i=1; i<=lines->getSize(); i++){
		//	Do first pass of line, if successfully parsed 
		if(pt->parse(lines->getLine(i), i)){
			//	First pass
			if (!firstPass(i)) {
				first_pass_completed = false;
				break;
			}
			//	If .end directive reached stop parsing first pass
			if (end_directive_reached)
				break;
		}else{
			first_pass_completed = false;
			break;
		}
	}

	//	Proceed to second pass after successfully completed first pass
	if (first_pass_completed) {

		//	Update size of the last section
		st->getEntry(current_section)->setSize(section_size);

		//	Merge sections and symbols into final std::map
		st->finalizeTable(false);

		cout << endl << "FIRST PASS COMPLETED" << endl;
		cout << endl << "Symbol Table:" << endl;

		//	Print symbol table
		st->dumpTable();

		//	For second parse the same, reset needed
		lc = startingPosition;
		section_size = 0;
		inside_section = false;
		end_directive_reached = false;
		current_section.clear();
		passed_sections.clear();

		//	Prepare byte buffers for storing section bytes
		SymEntry* entry = st->getEntry(".text");
		if (entry != 0) {
			size_of_text = entry->getSize();
			text_bytes = new char[size_of_text];
			//	Clear bytes
			for (int i = 0; i < size_of_text; i++)
				text_bytes[i] = 0;
		}
		entry = st->getEntry(".data");
		if (entry != 0) {
			size_of_data = entry->getSize();
			data_bytes = new char[size_of_data];
			//	Clear bytes
			for (int i = 0; i < size_of_data; i++)
				data_bytes[i] = 0;
		}
		entry = st->getEntry(".rodata");
		if (entry != 0) {
			size_of_rodata = entry->getSize();
			rodata_bytes = new char[size_of_rodata];
			//	Clear bytes
			for (int i = 0; i < size_of_rodata; i++)
				rodata_bytes[i] = 0;
		}
		entry = st->getEntry(".bss");
		if (entry != 0) {
			size_of_bss = entry->getSize();
			bss_bytes = new char[size_of_bss];
		}

		int i;
		//	Second pass
		for (i = 1; i < lines->getSize(); i++) {
			//	Parse till the .end directive
			if(!end_directive_reached)
				if (!secondPass(i)) {
					cout << endl << "SECOND PASS BROKEN" << endl << flush;
					return;
				}
		}

		cout << endl << endl << "SECOND PASS COMPLETED" << endl;
		cout << endl << "Symbol Table:" << endl;

		//	Add new global symbols that were not used but defined as extern that is global
		st->finalizeTable(true);

		//	Print symbol table
		st->dumpTable();

		cout << endl << flush;

		cout << endl << endl << "Bytes of text section" << endl << endl << flush;
		dumpSectionBytes(text_bytes, size_of_text, -1);
		cout << endl << endl << "Bytes of data section" << endl << endl << flush;
		dumpSectionBytes(data_bytes, size_of_data, -1);
		cout << endl << endl << "Bytes of rodata section" << endl << endl << flush;
		dumpSectionBytes(rodata_bytes, size_of_rodata, -1);
		cout << endl << endl << "Bytes of bss section -> random bytes" << endl << endl << flush;
		dumpSectionBytes(bss_bytes, size_of_bss, -1);

		cout << endl << flush;
		
		cout << endl << "Text Relocation Table" << endl << flush;
		ret_text->dumpTable();
		cout << endl << "Data Relocation Table" << endl << flush;
		ret_data->dumpTable();
		cout << endl << "Rodata Relocation Table" << endl << flush;
		ret_rodata->dumpTable();

		elf_header header;

		if (ret_data->get_entries().size() > 0) {
			header.has_data_ret = 1;
		}
		if (ret_rodata->get_entries().size() > 0) {
			header.has_rodata_ret = 1;
		}
		if (ret_text->get_entries().size() > 0) {
			header.has_text_ret = 1;
		}
		if (size_of_data > 0) {
			header.has_raw_data = 1;
		}
		if (size_of_rodata > 0) {
			header.has_raw_rodata = 1;
		}
		if (size_of_text > 0) {
			header.has_raw_text = 1;
		}
		if (size_of_bss > 0) {
			header.has_raw_bss = 1;
		}
		header.startAddress = this->startingPosition;

		serializer = new Serializer("object.o", false, &header);
		serializer->serializeSymTable(st);

		if (ret_data->get_entries().size() > 0) {
			serializer->serializeRelTable(ret_data);
		}
		if (ret_rodata->get_entries().size() > 0) {
			serializer->serializeRelTable(ret_rodata);
		}
		if (ret_text->get_entries().size() > 0) {
			serializer->serializeRelTable(ret_text);
		}
		if (size_of_data > 0) {
			serializer->serializeRawData(data_bytes, size_of_data);
		}
		if (size_of_rodata > 0) {
			serializer->serializeRawData(rodata_bytes, size_of_rodata);
		}
		if (size_of_text > 0) {
			serializer->serializeRawData(text_bytes, size_of_text);
		}
		if (size_of_bss > 0) {
			serializer->serializeRawData(bss_bytes, size_of_bss);
		}

		//delete serializer;

		//serializer = new Serializer("object.o", true);

		//ObjectFile *of = serializer->makeObjectFile();

	}
}

unsigned char Assembler::makeFirstByte(int condition, int opcode, int addressing)
{
	if (condition > 3 || condition < 0 || opcode > 15 || opcode < 0 || addressing < 0 || addressing > 3) {
		cout << "Error: wrong opcode" << endl << flush; 
		cout << "Enter anything to continue: " << flush;
		int a;
		cin >> a;
		exit(1);
	}

	char firstByte = 0;

	condition <<= 6;
	opcode <<= 2;
	//	addressing is rightmost two bits so its ok

	firstByte |= condition;
	firstByte |= opcode;
	firstByte |= addressing;

	return firstByte;
}

unsigned char Assembler::makeSecondByte(int dst, int addressing, int src)
{
	if (dst > 7 || dst < 0 || src > 7 || src < 0 || addressing < 0 || addressing > 3) {
		cout << "Error: wrong opcode" << endl << flush;
		cout << "Enter anything to continue: " << flush;
		int a;
		cin >> a;
		exit(1);
	}

	char secondByte = 0;

	dst <<= 5;
	addressing <<= 3;
	//	src is rightmost three bits so its ok

	secondByte |= dst;
	secondByte |= addressing;
	secondByte |= src;

	return secondByte;
}

void Assembler::makeAdditionalTwoBytes(char * src, int value)
{
	if (src != 0) {
		int mask_lower = 255;
		int mask_higher = mask_lower << 8;

		src[0] = value & mask_lower;
		src[1] = ((value & mask_higher)>>8);
	}
	else {
		cout << "Error: passed null pointer char as src string of bytes" << endl << flush;
	}
}

void Assembler::printByteToHex(char byte )
{
	if (byte >= 0 && byte < 16)
		cout << "0";

	cout << hex << (unsigned short)(unsigned char)byte;
	
}

void Assembler::dumpSectionBytes(char * section, int size, int startingPosition)
{
	int count = 0;
	if (startingPosition == -1) {
		if (section != 0 && size > 0) {
			for (int i = 0; i < size; i++) {
				printByteToHex(section[i]);
			}
		}
	}
	else {
		if (section != 0 && size > 0) {
			for (int i = startingPosition; i < size; i++) {
				printByteToHex(section[i]);
			}
			cout << " " << flush;
		}
	}
	cout << dec;
}

char * Assembler::getCurrentBytePointer()
{
	if (current_section == ".text")
		return this->text_bytes;
	else if (current_section == ".data")
		return this->data_bytes;
	else if (current_section == ".rodata")
		return this->rodata_bytes;
	else {
		return 0;
	}
}

RelTable * Assembler::getCurrentRelSection()
{
	if (current_section == ".text")
		return ret_text;
	else if (current_section == ".data")
		return ret_data;
	else if (current_section == ".rodata")
		return ret_rodata;
	else 
		return 0;
}

int& Assembler::getCurrentSectionSize()
{
	if (current_section == ".text")
		return current_size_of_text;
	else if (current_section == ".data")
		return current_size_of_data;
	else if (current_section == ".rodata")
		return current_size_of_rodata;
	else {
		cout << "Not allowed current section" << endl << flush;
		cout << "Enter anything to continue: " << flush;
		int a;
		cin >> a;
		exit(1);
	}
}

bool Assembler::firstPass(int line) {

	list<string> instruction = pt->getParsedInstruction(line);

	for (list<string>::const_iterator it = instruction.begin(); it != instruction.end(); it++) {
		if ( !inside_section ) {
			if ((*it) == "DIRECTIVE") {

				it++;

				if (*it == ".global") {
					return true;	//	Stop parsing this, it will be parsed in second pass
				}
				else if (*it == ".end") {
					end_directive_reached = true;	 //	Marks first pass as completed
					return true;	
				}
				else if (*it == ".align") {
					cout << "Error: cannot align outside sections, at line: " << line << endl << flush;
					return false;
				}
				else {
					cout << "Error: forbidden DIRECTIVE: " << *it << " outside sections, at line: " << line << endl << flush;
					return false;
				}

			}
			else if ((*it) == "SECTION") {

				it++;

				inside_section = true;

				SymEntry *entry = 0;

				if (*it == ".text" || *it == ".rodata" || *it==".bss") {
					entry = new SymEntry(*it, *it, lc, Local, section_size, READ);
				}
				else if (*it == ".data") {
					entry = new SymEntry(*it, *it, lc, Local, section_size, READ_WRITE);
				}

				st->addSectionEntry(entry);

				current_section = *it;

				passed_sections.push_back(*it);

			}
			else {
				cout << "Error: unexpected " << (*it) << " , at line: " << line << endl << flush;
				return false;
			}
		} else {

			//	Check for multiple definition of same section
			if (*it == "SECTION") {
				it++;
				for (list<string>::const_iterator it2 = passed_sections.begin(); it2 != passed_sections.end(); it2++) {
					if (*it == *it2) {
						cout << "Error: section already defined, at line: " << line << endl << flush;
						return false;
					}
				}

				SymEntry* entry_old = st->getEntry(current_section);
				if (entry_old != 0) {
					entry_old->setSize(section_size);
				}

				current_section = *it;

				section_size = 0;

				SymEntry *entry = 0;
				
				if (*it == ".text" || *it == ".rodata" || *it==".bss") {
					entry = new SymEntry(*it, *it, lc, Local, 0, READ);
				} else if (*it == ".data") {
					entry = new SymEntry(*it, *it, lc, Local, 0, READ_WRITE);
				}

				st->addSectionEntry(entry);
				passed_sections.push_back(*it);

			//	Check for directive global rules
			} else if (*it == "DIRECTIVE") {

				it++;

				//	Check for bad directives in text section
				if (current_section == ".text") {
					if (*it == ".align") {
						it++; it++;
						int factor = stoi(*it);
						if (section_size== 0) {
							while (lc % factor != 0) {
								lc++;
								section_size++;
							}
							return true;
						}
						else {
							cout << "Error: .align directive allowed only at beginning of section" << endl << flush;
							return false;
						}
					}
					if (*it != ".end"){
						cout << "Error: directive " << *it << " forbidden in text section" << endl << flush;
						return false;
					} else {
						end_directive_reached = true;	 //	Marks first pass as completed
						return true;	
					}
				} else if(current_section == ".rodata"){

					int factor = 0;

					if (*it == ".char") {
						factor = 1;
					} else if (*it == ".word") {
						factor = 2;
					} else if (*it == ".long") {
						factor = 4;
					} else if (*it == ".end") {
						end_directive_reached = true;
						return true;
					}
					else if (*it == ".skip") {
						cout << "Error: .skip directive not allowed in .rodata section, must have initializer, at line: " << line << endl << flush;
						return false;
					}else if( *it == ".align"){

						it++; it++;
						int factor = stoi(*it);
						while (lc % factor != 0) {
							lc++;
							section_size++;
						}
						return true;

					}else {
						cout << "Error: not allowed directive " << *it << " at line: " << line << endl << flush;
						return false;
					}
					int counter = 0;
					while (it != instruction.end()) {
						it++;
						if (it == instruction.end())
							break;
						it++;
						counter += factor;
					}

					//	Rodata section needs to have initializers
					if (counter == 0) {
						cout << "Error: .rodata needs to have initializers, at line: " << line << endl << flush;
						return false;
					}

					lc += counter;
					section_size += counter;
					return true;
					
				} else if (current_section == ".bss") {

					int factor = 0;

					if (*it == ".char") {
						factor = 1;
					}
					else if (*it == ".word") {
						factor = 2;
					}
					else if (*it == ".long") {
						factor = 4;
					}
					else if (*it == ".end") {
						end_directive_reached = true;
						return true;
					} else if (*it == ".align") {

						it++; it++;
						int factor = stoi(*it);
						while (lc % factor != 0) {
							lc++;
							section_size++;
						}
						return true;

					} else if (*it == ".skip") {
						it++; 
						if (it == instruction.end()) {
							cout << "Error: skip instruction requires operand, at line: " << line << endl << flush;
							return false;
						}
						if (*it != "IMMEDIATE") {
							cout << "Error: only immediate operand type allowed, at line: " << line << endl << flush;
							return false;
						}
						it++;
						section_size += stoi(*it);
						lc += stoi(*it);
						return true;

					} else {
						cout << "Error: not allowed directive " << *it << " at line: " << line << endl << flush;
						return false;
					}
					int counter = 0;
					while (it != instruction.end()) {
						it++;
						if (it == instruction.end())
							break;
						it++;
						counter += factor;
					}

					//	Bss section should not contain initializers
					if (counter > 0) {
						cout << "Error: .bss section should not contain initializers, at line: " << line << endl << flush;
						return false;
					}

					if (counter == 0) {
						lc += factor;
						section_size += factor;
					}
					else {
						lc += counter;
						section_size += counter;
					}

					return true;

				} else {

					int factor = 0;

					if (*it == ".char") {
						factor = 1;
					}
					else if (*it == ".word") {
						factor = 2;
					}
					else if (*it == ".long") {
						factor = 4;
					} else if (*it == ".end") {
			
						end_directive_reached = true;
						return true;
					} else if (*it == ".align") {

						it++; it++;
						int factor = stoi(*it);
						while (lc % factor != 0) {
							lc++;
							section_size++;
						}
						return true;

					} else if (*it == ".skip") {
						it++; 
						if (it == instruction.end()) {
							cout << "Error: skip instruction requires operand, at line: " << line << endl << flush;
							return false;
						}
						if (*it != "IMMEDIATE") {
							cout << "Error: only immediate operand type allowed, at line: " << line << endl << flush;
							return false;
						}
						it++;

						section_size += stoi(*it);
						lc += stoi(*it);
						return true;

					}else {
						cout << "Error: not allowed directive " << *it << " at line: " << line << endl << flush;
						return false;
					}
					int counter = 0;
					while (it != instruction.end()) {
						it++;
						if (it == instruction.end())
							break;
						it++;
						counter += factor;
					}
					if (counter == 0) {
						cout << "Error: .data section needs to have initializer, at line: " << line << endl << flush;
						return false;
					}
					else {
						lc += counter;
						section_size += counter;
					}
					return true;
				}

			//	Parse label
			} else if (*it == "LABEL") {
				//	Skip to label name
				it++;
				//	Create table entry
				SymEntry *se = new SymEntry(*it, current_section, lc, Local, 0, NONE, line);
				//	Add to table if not already present otherwise error
				if (st->getEntry(*it) != 0) {
					cout << "Error: symbol ,," << *it << ",, is already defined, tried again at line: " << line << endl << flush;
					return false;
				}
				st->addSymbolEntry(se);
			//	All instructions are 2 bytes, if some operand is referencing memory or is immediate it will be added later
			} else if (*it == "INSTRUCTION") {
				if (current_section != ".text") {
					cout << "Error: instructions allowed only in .text section, at line: " << line << endl << flush;
					return false;
				}
				lc += 2;
				section_size += 2;
				it++;
			//	All addressing modes requires additional 2 bytes except regdir and need to skip helper symbols ,,CONST'' ,,CONDITION'' ,,SYMBOL'' 
			} else if (*it != "REGDIR" && *it != "CONST" && *it != "CONDITION" && *it!="SYMBOL" && *it!="PSW") {
				lc += 2;
				section_size += 2;
				it++;
			}
			//	Otherwise it is not a type but rather name of some type, so just skip it
			else {
				it++;
			}
		}
		//	For loop crashes when iterator pass end point
		if (it == instruction.end())
			break;
	}
	return true;
}

bool Assembler::secondPass(int line) {

	//	Get instruction at line
	list<string> instruction = pt->getParsedInstruction(line);

	//	Handle parsed instruction
	for (list<string>::const_iterator it = instruction.begin(); it != instruction.end(); it++) {

		//	Skip bss section till the SECTION directive appears so we can change it
		if ( (current_section == ".bss") && (*it != "SECTION"))
			return true;

		//	All addressing modes run with absolute relocation type, except PCRELPOM which uses relative relication type
		RelocationType relocation_type = R_386_16;

		string mem_operand;
		mem_operand.clear();

		if (*it == "DIRECTIVE") {

			//	Next -> name of directive
			it++;


			// When putting label into .long or .word, you must increment size of section accordingly
			int increment = 0;

			if (*it == ".global") {
				//	Next -> operand type
				it++;
				while (it != instruction.end()) {
					//	Next -> actual symbol
					it++;
					SymEntry* se = st->getEntry(*it);
					if(se !=0)
						se->setLocality(Global);
					else {
						//	No must be calculated dynamically because otherwise No value of this entry will be used before finalizeTable() is called which is being called after second pass completes
						se = new SymEntry(*it, "UNDEFINED", 0, Global, 0, NONE, st->getSymbolEntriesSize()+st->getFinalEntriesSize()+1);
						st->addSymbolEntry(se);
					}
					//	Next -> operand type if exists otherwis it=instruction.end()
					it++;
				}
			}
			//	Stop parsing when .end directive reached
			else if (*it == ".end") {

				end_directive_reached = true;
				return true;

			}
			else if (*it == ".align") {

				it++; it++;
				int factor = stoi(*it);
				while (lc % factor != 0) {
					lc++;
					getCurrentSectionSize()++;
				}

				return true;

			}else if(*it == ".skip"){
				
				//	Skip to next token
				it++;
				//	This token always IMMEDIATE so skip it also
				it++;

				getCurrentSectionSize() += stoi(*it);
				increment = stoi(*it);

				dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize() - increment);

				return true;

			} else if (*it == ".long") {
				//	Skip to next token
				it++;

				increment = 4;

				//	It can be without initializator
				if (it != instruction.end()) {

					//	Parse all tokens, variable operand length
					while (it != instruction.end()) {

						//	Check against CONST or SYMBOL
						if (*it == "CONST") {
							it++;

							//	Convert to integer
							int num = stoi(*it);

							//	Put 4 bytes
							int mask_first_8_bits = 255;

							getCurrentBytePointer()[getCurrentSectionSize()++] = num & mask_first_8_bits;
							getCurrentBytePointer()[getCurrentSectionSize()++] = (num & (mask_first_8_bits << 8)) >> 8;
							getCurrentBytePointer()[getCurrentSectionSize()++] = (num & (mask_first_8_bits << 16)) >> 16;
							getCurrentBytePointer()[getCurrentSectionSize()++] = (num & (mask_first_8_bits << 24)) >> 24;

							//cout << "BYTES FROM " << current_section << endl << flush;
							dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize()-4);

							it++;
						}
						else {
							//	MEMDIR
							it++;

							mem_operand = *it;

							it++;
						}
					}
				}
				else {
					getCurrentSectionSize() += 4;
					dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize() - 4);
				}

			} else if (*it == ".word") {
				//	Skip to next token
				it++;

				increment = 2;

				//	It can be without initializator
				if (it != instruction.end()) {
					
					while (it != instruction.end()) {
						//	Check against CONST or SYMBOL
						if (*it == "CONST") {
							it++;

							//	Convert to integer
							int num = stoi(*it);

							makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), num);
							getCurrentSectionSize() += 2;

							//cout << "BYTES FROM " << current_section << endl << flush;
							dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize()-2);
							
							it++;
						}
						else {
							it++;

							mem_operand = *it;

							it++;
						}
					}
				}
				else {
					getCurrentSectionSize() += 2;
					dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize()-2);
				}
			} else if (*it == ".char") {
				//	Skip to next token
				it++;

				increment = 1;

				//	It can be without initializator
				if (it != instruction.end()) {
					
					while (it != instruction.end()) {
						//	Check against CONST or SYMBOL
						if (*it == "CONST") {
							it++;

							//	Convert to integer
							int num = stoi(*it);

							//	Add to section
							getCurrentBytePointer()[getCurrentSectionSize()++] = num;

							//cout << "BYTES FROM " << current_section << endl << flush;
							dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize()-1);

							it++;
						}
						else {
							cout << endl << "Error: you can't use 2 byte value LABEL for 1 byte char, at line: " << line << endl << flush;
							return false;
						}
					}
				}
				else {
					getCurrentSectionSize()++;
					dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize()-1);
				}
			}

			SymEntry* symbol = st->getEntry(mem_operand);

			if (symbol != 0) {

				if (relocation_type == R_386_16) {

					//	Symbol is global and undefined
					if (symbol->getSection() == "UNDEFINED" || symbol->getLocality() == Global) {

						RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, symbol->getNo());

						getCurrentRelSection()->addEntry(newEntry);

						makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), 0);
						getCurrentSectionSize() += increment;
					}
					//	Symbol is defined in current section and is local
					else if (symbol->getSection() == current_section) {

						RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(current_section)->getNo());

						getCurrentRelSection()->addEntry(newEntry);

						makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue() - st->getEntry(current_section)->getValue());
						getCurrentSectionSize() += increment;

					}
					//	Symbol is defined in other section and is local
					else {
						RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(symbol->getSection())->getNo());

						getCurrentRelSection()->addEntry(newEntry);

						makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue() - st->getEntry(symbol->getSection())->getValue());
						getCurrentSectionSize() += increment;
					}

				}

				dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize() - increment);
			} 


			break;
		} else if (*it == "SECTION") {

			//	Skip to next token
			it++;

			//	Update current section
			current_section = *it;

			break;

		} else if (*it == "INSTRUCTION") {

			//	Skip to next token
			it++;

			string instruction_string = *it;
			int condition_opcode = 3;

			int expected_operands = pt->getReqOp(instruction_string);
			int parsed_operands = 0;

			int dst_addressing = 0;
			int dst = 0;
			int src_addressing = 0;
			int src = 0;
			int pom = 0;

			int tmp_addressing = 0;
			int tmp_src_dst = 0;

			bool has_additional_two_bytes = false;
			//	Parse instruction opcode
			int instruction_opcode = 0; 

			bool jmp_instruction = false;

			if (*it == "add")
				instruction_opcode = 0;
			else if (*it == "sub")
				instruction_opcode = 1;
			else if (*it == "mul")
				instruction_opcode = 2;
			else if (*it == "div")
				instruction_opcode = 3;
			else if (*it == "cmp")
				instruction_opcode = 4;
			else if (*it == "and")
				instruction_opcode = 5;
			else if (*it == "or")
				instruction_opcode = 6;
			else if (*it == "not")
				instruction_opcode = 7;
			else if (*it == "test")
				instruction_opcode = 8;
			else if (*it == "push")
				instruction_opcode = 9;
			else if (*it == "pop")
				instruction_opcode = 10;
			else if (*it == "call")
				instruction_opcode = 11;
			else if (*it == "iret")
				instruction_opcode = 12;
			else if (*it == "mov")
				instruction_opcode = 13;
			else if (*it == "shl")
				instruction_opcode = 14;
			else if (*it == "shr")
				instruction_opcode = 15;
			else if (*it == "jmp") {
				//	If pcrel addressing use ADD instruction with relative R_386_PC16 relocation, otherwise for all others use MOV instruction with absolute R_386_16 relocation
				if (!ParseTree::getInstructionField(instruction, "PCRELPOM").empty()) {
					instruction_opcode = 0;	//	ADD
				} else {
					instruction_opcode = 13; //	MOV
				}

				jmp_instruction = true;
			}
			else if (*it == "ret") {
				instruction_opcode = 10;
				dst_addressing = 1;
				dst = 7;
			}else {
				cout << "Error: unknown instruction, at line: " << line << endl << flush;
				return false;
			}
			
			//	Skip to next token
			it++;

			while (it != instruction.end() && parsed_operands != expected_operands) {

				//	Check for condition
				if (*it == "CONDITION") {

					it++;

					if (*it == "eq")
						condition_opcode = 0;
					else if (*it == "ne")
						condition_opcode = 1;
					else if (*it == "gt")
						condition_opcode = 2;
					else if (*it == "al")
						condition_opcode = 3;

					it++;

				}

				//	Parse addressing 
				if (*it == "REGDIR") {

					tmp_addressing = 1;

					it++;

					if (*it == "r0")
						tmp_src_dst = 0;
					else if (*it == "r1")
						tmp_src_dst = 1;
					else if (*it == "r2")
						tmp_src_dst = 2;
					else if (*it == "r3")
						tmp_src_dst = 3;
					else if (*it == "r4")
						tmp_src_dst = 4;
					else if (*it == "r5")
						tmp_src_dst = 5;
					else if (*it == "r6")
						tmp_src_dst = 6;
					else if (*it == "r7")
						tmp_src_dst = 7;

				}
				else if (*it == "REGINDPOM") {

					tmp_addressing = 3;
					has_additional_two_bytes = true;

					it++;

					if (*it == "r0")
						tmp_src_dst = 0;
					else if (*it == "r1")
						tmp_src_dst = 1;
					else if (*it == "r2")
						tmp_src_dst = 2;
					else if (*it == "r3")
						tmp_src_dst = 3;
					else if (*it == "r4")
						tmp_src_dst = 4;
					else if (*it == "r5")
						tmp_src_dst = 5;
					else if (*it == "r6")
						tmp_src_dst = 6;
					else if (*it == "r7")
						tmp_src_dst = 7;

					it++;

					if (*it == "CONST") {
						it++;
						pom = stoi(*it);
					}
					else if(*it == "SYMBOL") {
						it++;
						mem_operand = *it;
					}

				}
				else if (*it == "MEMDIR") {

					it++;

					if (instruction_opcode == 11) { //	Need to set immed for CALL instruction, otherwise, doesnt jump on label but on value that label points to
						tmp_addressing = 0;
					}else
						tmp_addressing = 2;
					has_additional_two_bytes = true;

					mem_operand = *it;

				}
				else if (*it == "MEMDIRIMMED") {
					tmp_addressing = 2; // MEMDIR
					has_additional_two_bytes = true;

					it++;

					pom = stoi(*it);
				}
				else if (*it == "REFERENCE") {
					it++;
					mem_operand = *it;

					tmp_addressing = 0; // IMMED
					has_additional_two_bytes = true;
				}
				else if (*it == "PCRELPOM") {

					relocation_type = R_386_PC16;

					it++;
					mem_operand = *it;
				
					if (!jmp_instruction) {
						tmp_addressing = 3;
						tmp_src_dst = 7;
					}

					has_additional_two_bytes = true;

				}
				else if (*it == "IMMEDIATE") {
					tmp_addressing = 0;
					has_additional_two_bytes = true;

					it++;

					pom = stoi(*it);
				}
				else if (*it == "PSW") {
					tmp_addressing = 0;
					tmp_src_dst = 7;
					it++;
				}
				else {
					cout << "Error: unknown addressing type, at line: " << line << endl << flush;
					return false;
				}

				//	Update operands
				if (parsed_operands == 0) {
					dst_addressing = tmp_addressing;
					dst = tmp_src_dst;
				}
				else {
					src_addressing = tmp_addressing;
					src = tmp_src_dst;
				}

				//	CALL and PUSH use src operand
				if (ParseTree::getInstructionField(instruction, "INSTRUCTION") == "push" || ParseTree::getInstructionField(instruction, "INSTRUCTION") == "call" ) {
					dst_addressing = 0;
					dst = 0;

					src_addressing = tmp_addressing;
					src = tmp_src_dst;
				}

				//	Increment operands parsed
				parsed_operands++;
				
				//	Skip to next token
				it++;
			}

			char firstByte = makeFirstByte(condition_opcode, instruction_opcode, dst_addressing);
			char secondByte = makeSecondByte(dst, src_addressing, src);

			getCurrentBytePointer()[ getCurrentSectionSize()++ ] = firstByte;
			getCurrentBytePointer()[ getCurrentSectionSize()++ ] = secondByte;

			if (has_additional_two_bytes) {

				if (mem_operand.empty()) {

					makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), pom);
					getCurrentSectionSize() += 2;

				} else {

					SymEntry* symbol = st->getEntry(mem_operand);

					if (symbol != 0) {

						if (relocation_type == R_386_16) {

							//	Symbol is global and undefined
							if (symbol->getSection() == "UNDEFINED" || symbol->getLocality() == Global) {

								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, symbol->getNo());
								
								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]),0);
								getCurrentSectionSize() += 2;
							}
							//	Symbol is defined in current section and is local
							else if (symbol->getSection() == current_section) {
								
								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(current_section)->getNo());

								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue()-st->getEntry(current_section)->getValue());
								getCurrentSectionSize() += 2;
								
							}
							//	Symbol is defined in other section and is local
							else {
								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(symbol->getSection())->getNo());

								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue()-st->getEntry(symbol->getSection())->getValue());
								getCurrentSectionSize() += 2;
							}

						} else if(relocation_type == R_386_PC16) {
							//	Symbol is global and undefined
							if (symbol->getSection() == "UNDEFINED" || symbol->getLocality() == Global) {

								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, symbol->getNo());
								
								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]),-2);
								getCurrentSectionSize() += 2;
							}
							//	Symbol is defined in current section
							else if (symbol->getSection() == current_section) {

								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(current_section)->getNo());

								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue() - st->getEntry(current_section)->getValue() - 2);
								getCurrentSectionSize() += 2;

							}
							//	Symbol is defined in other section
							else {

								RelEntry* newEntry = new RelEntry(getCurrentSectionSize(), relocation_type, st->getEntry(symbol->getSection())->getNo());

								getCurrentRelSection()->addEntry(newEntry);

								makeAdditionalTwoBytes(&(getCurrentBytePointer()[getCurrentSectionSize()]), symbol->getValue()-st->getEntry(symbol->getSection())->getValue()-2);
								getCurrentSectionSize() += 2;
							}	
						}
					} else {
						cout << "Error: unknown operand ,," << mem_operand << ",, required, at line: " << line << endl << flush;
						return false;
					}
				}
			}

			dumpSectionBytes(getCurrentBytePointer(), getCurrentSectionSize(), getCurrentSectionSize() - (has_additional_two_bytes ? 4 : 2));

			break;

		} else if (*it == "LABEL") {

			//	Skip label token
			it++;

		} else {
			cout << "Token of this type shouldn't appear here" << endl << flush;
			return false;
		}

		if (it == instruction.end())
			break;
	}
	return true;
}