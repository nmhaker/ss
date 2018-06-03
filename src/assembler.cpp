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
	}

	test();
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
    delete lines;
}

void Assembler::test() {
	ParseTree*  pt = new ParseTree();
	for(int i=1; i<=lines->getSize(); i++){
		if(pt->parse(lines->getLine(i), i)){
			cout << "Successful parse" << endl;	
		}else{
			cout << "Unsuccessful parse" << endl;
		}
	}
    delete pt;
}
