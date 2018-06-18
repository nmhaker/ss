#include <iostream>

#include "emulator.h"

using namespace std;

int main(int argc, char** argv) {

	Emulator *emulator = 0;

	emulator = new Emulator(argc, argv);

	if(emulator!=0)
		delete emulator;

	cout << endl;

	return 0;
}
