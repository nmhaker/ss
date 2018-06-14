
#include <iostream>
#include "../include/assembler.h" 

using namespace std;

int main(int argc, char** argv){

	Assembler *assembler = 0;

	if(argc != 3){
		cout << "Two parameters required" << endl;
		return -1;
	}else{
		char* inputFileName = argv[1];
		assembler = new Assembler(inputFileName, argv[2]);	
	}

	delete assembler;

	cout << endl << "Successfully finished assembling" << endl << flush;

	char in;
	cin >> in;

	return 0;
}
