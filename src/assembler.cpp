#include "../include/assembler.h"
#include "../include/array.h"
#include "../include/parsetree.h"

#include <iostream>
#include <fstream>
#include <list>

using namespace std;

Assembler::Assembler(char* inputFileName){

//	cout << "Konstruktor" << endl;	
//	cout << "Input file: " << inputFileName << endl;

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

	st = new SymTable();

	lc = 0;
	section_size = 0;
	inside_section = false;
	first_pass_completed = true;
	end_directive_reached = false;

	text_bytes = 0;
	data_bytes = 0;
	rodata_bytes = 0;

	assemble();
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
    delete lines;
    delete pt;
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
		lc = 0;
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
		}
		entry = st->getEntry(".data");
		if (entry != 0) {
			size_of_data = entry->getSize();
			data_bytes = new char[size_of_data];
		}
		entry = st->getEntry(".rodata");
		if (entry != 0) {
			size_of_rodata = entry->getSize();
			rodata_bytes = new char[size_of_rodata];
		}

		//	Second pass
		for (int i = 1; i < lines->getSize(); i++) {
			//	Parse till the .end directive
			if(!end_directive_reached)
				secondPass(i);
		}

		cout << endl << "SECOND PASS COMPLETED" << endl;
		cout << endl << "Symbol Table:" << endl;

		//	Add new global symbols that were not used but defined as extern that is global
		st->finalizeTable(true);

		//	Print symbol table
		st->dumpTable();
	}
}

unsigned char Assembler::makeFirstByte(int condition, int opcode, int addressing)
{
	if (condition > 3 || condition < 0 || opcode > 15 || opcode < 0 || addressing < 0 || addressing > 3) {
		cout << "Error: wrong opcode" << endl << flush;
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
		exit(1);
	}

	char secondByte = 0;

	dst <<= 5;
	addressing <<= 2;
	//	src is rightmost two bits so its ok

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
		src[1] = value & mask_higher;
	}
	else {
		cout << "Error: passed null pointer char as src string of bytes" << endl << flush;
	}
}

void Assembler::printByteToHex(char byte)
{
	cout << hex << (unsigned short) byte;
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
					return true;
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
					if (*it != ".end"){
						cout << "Error: directive " << *it << " forbidden in text section" << endl << flush;
						return false;
					} else {
						end_directive_reached = true;	 //	Marks first pass as completed
						return true;	
					}
				} else if(current_section == ".rodata"){
					int factor = 1;
					if (*it == ".char") {
						factor = 1;
					} else if (*it == ".word") {
						factor = 2;
					} else if (*it == ".long") {
						factor = 4;
					} else if (*it == ".end") {
						end_directive_reached = true;
						return true;
					} else if (*it == ".skip") {
						cout << "Error: .skip directive not allowed in .rodata section, must have initializer, at line: " << line << endl << flush;
						return false;
					}else {
						cout << "Error: not allowed directive " << *it << " at line: " << line << endl << flush;
						return false;
					}
					int counter = 0;
					while (it != instruction.end()) {
						counter++;
						it++;
					}
					counter--;

					//	Rodata section needs to have initializers
					if (counter == 0) {
						cout << "Error: .rodata needs to have initializers, at line: " << line << endl << flush;
						return false;
					}

					lc += factor*counter;
					section_size += factor*counter;
					return true;
					
				} else if (current_section == ".bss") {
					int factor = 1;
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
						counter++;
						it++;
					}
					counter--;

					//	Bss section should not contain initializers
					if (counter > 0) {
						cout << "Error: .bss section should not contain initializers, at line: " << line << endl << flush;
						return false;
					}

					lc += factor*counter;
					section_size += factor*counter;

					return true;

				} else {
					int factor = 1;
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
						counter++;
						it++;
					}
					counter--;

					lc += factor*counter;
					section_size += factor*counter;
					return true;
				}

			//	Parse label
			} else if (*it == "LABEL") {
				//	Skip to label name
				it++;
				//	Create table entry
				SymEntry *se = new SymEntry(*it, current_section, lc, Local, 0, NONE);
				//	Add to table
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
			} else if (*it != "REGDIR" && *it != "CONST" && *it != "CONDITION" && *it!="SYMBOL") {
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
		if (*it == "DIRECTIVE") {

			//	Next -> name of directive
			it++;

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
						se = new SymEntry(*it, "UNDEFINED", 0, Global, 0, NONE);
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
			} else if (*it == ".align") {
				cout << "Error: directive not supported yet" << endl << flush;
				return false;
			} else if (*it == ".long") {
			} else if (*it == ".word") {
			} else if (*it == ".char") {
			}

			break;
		} else if (*it == "SECTION") {

			//	Skip to next token
			it++;

			//	Update current section
			current_section = *it;
			//	Reset section size counter == offset into current section
			section_size = 0;

			break;

		} else if (*it == "INSTRUCTION") {

			//	Skip to next token
			it++;

			//	Check for condition
			string condition = ParseTree::getInstructionField(instruction, "CONDITION");
			if (!condition.empty()) {
				cout << "Ima uslov: " << condition << ", at line: " << line << endl << flush;
			}
			else {
				cout << "Nema uslov, at line: " << line << endl << flush;
			}

			break;

		} else if (*it == "LABEL") {

			//	Skip label token
			it++;

		} else {
			cout << "Token of this type shouldnt appear here" << endl << flush;
			return false;
		}

		if (it == instruction.end())
			break;
	}
	return true;
}