
#include <iostream>
#include "../include/assembler.h" 

using namespace std;

int main(int argc, char** argv){

	while(1){
		
		char run;
		cin >> run;
		if(run == 'r'){

			Assembler *assembler = 0;

			if(argc <= 1){
				cout << "Fatal: no input files";
				return -1;
			}else{
				char* inputFileName = argv[1];
				assembler = new Assembler(inputFileName);	
			}

			delete assembler;
		}else{
			break;
		}
	}

	return 0;
}
