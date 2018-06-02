
#include <iostream>
#include "assembler.h" 

using namespace std;

int main(int argc, char** argv){

	Assembler *assembler = 0;

	if(argc <= 1){
		cout << "Fatal: no input files";
		return -1;
	}else{
		char* inputFileName = argv[1];
		assembler = new Assembler(inputFileName);	
	}

	delete assembler;

	return 0;
}
