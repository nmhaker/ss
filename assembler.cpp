#include "assembler.h"

#include <iostream>
#include <fstream>
#include <array>

using namespace std;

Assembler::Assembler(char* inputFileName){

//	cout << "Konstruktor" << endl;	
//	cout << "Input file: " << inputFileName << endl;

	ifstream inputStream(inputFileName, ios::in);
	if(inputStream.is_open()){

		char** rows = new char*[100];

		int num_rows = 0;
		while(!inputStream.eof()){
			rows[num_rows] = new char[100];
			inputStream.getline(rows[num_rows++], 100);
		}
				
		inputStream.close();

		for(int i=0; i<num_rows; i++){
			cout << rows[i]  << endl;
		}

		for(int i=0; i<num_rows; i++){
			delete rows[i];
		}

		delete rows;
	}else{
		cout << "Nije otvoren ulazni stream" << endl;
	}
}

Assembler::~Assembler(){
//	cout << "Destruktor" << endl;
}
