
#include <iostream>
#include "../include/assembler.h" 

using namespace std;

int main(int argc, char** argv){

	bool first = true;
	
	while(1){
		
		if(!first){
			char run;
			cin >> run;
			if(run == 'r'){

				Assembler *assembler = 0;

				if(argc != 3){
					cout << "Two parameters required" << endl;
					return -1;
				}else{
					char* inputFileName = argv[1];
					assembler = new Assembler(inputFileName, argv[2]);	
				}

				delete assembler;
			}else{
				break;
			}
		}else{

			Assembler *assembler = 0;

			if(argc != 3){
				cout << "Two parameters required" << endl;
				return -1;
			}else{
				char* inputFileName = argv[1];
				assembler = new Assembler(inputFileName, argv[2]);	
			}

			delete assembler;
			
			first = false;
		}
	}

	return 0;
}
