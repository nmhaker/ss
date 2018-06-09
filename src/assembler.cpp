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

	assemble();
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
    delete lines;
    delete pt;
}

void Assembler::assemble() {

	pt = new ParseTree();

	//	First pass
	for(int i=1; i<=lines->getSize(); i++){
		if(pt->parse(lines->getLine(i), i)){
			if (!firstPass(i)) {
				first_pass_completed = false;
				break;
			}
		}else{
			first_pass_completed = false;
			break;
		}
	}

	if (first_pass_completed) {
		
		//	Update size of the last section
		st->getEntry(current_section)->setSize(section_size);

		//	Merge sections and symbols into final std::map
		st->finalizeTable();

		cout << endl << "FIRST PASS COMPLETED" << endl;
		cout << endl << "Symbol Table:" << endl;
		st->dumpTable();

		//	Second pass
		for (int i = 1; i < lines->getSize(); i++) {
			secondPass(i);
		}

		cout << endl << "SECOND PASS COMPLETED" << endl;
		cout << endl << "Symbol Table:" << endl;

		st->dumpTable();
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
						it++;
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
	list<string> instruction = pt->getParsedInstruction(line);
	return false;
}