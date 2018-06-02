#include "assembler.h"
#include "array.h"

#include <iostream>
#include <fstream>
#include <list>

using namespace std;

Assembler::Assembler(char* inputFileName){

//	cout << "Konstruktor" << endl;	
//	cout << "Input file: " << inputFileName << endl;

	ifstream inputStream(inputFileName, ios::in);
	if(inputStream.is_open()){
	
		list<string> lines;

		while(!inputStream.eof()){
			string new_line;	
			getline(inputStream, new_line);
			lines.push_back(new_line);
		}

		inputStream.close();

		lines = new Array(&lines);

	}else{
		cout << "Nije otvoren ulazni stream" << endl;
	}
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
    delete lines;
}
