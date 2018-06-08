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

	assemble();
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
    delete lines;
    delete pt;
}

void Assembler::assemble() {

	pt = new ParseTree();

	bool success = true;
	//	First pass
	for(int i=1; i<=lines->getSize(); i++){
		if(pt->parse(lines->getLine(i), i)){
			firstPass(i);
		}else{
			success = false;
			break;
		}
	}

	cout << endl << "FIRST PASS COMPLETED" << endl;
	cout << endl << "Symbol Table:" << endl;

	st->dumpTable();

	//	Second pass
	if(success)
		for (int i = 1; i < lines->getSize(); i++) {
			secondPass(i);
		}
}

void Assembler::firstPass(int line) {
	list<string> instruction = pt->getParsedInstruction(line);
}

void Assembler::secondPass(int line) {
	list<string> instruction = pt->getParsedInstruction(line);
}