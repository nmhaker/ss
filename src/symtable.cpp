#include "../include/symtable.h"

#include <iostream>
using namespace std;

SymTable::SymTable() {

}

SymTable::~SymTable() {

}

void SymTable::dumpTable() {
	cout << "|   IME   |   SEKCIJA   |   VREDNOST   |   VIDLJIVOST   |   REDNI BROJ   |" << endl;
	cout << "|------------------------------------------------------------------------|" << endl;
	cout << "|    /    |     UND     |      0h      |        L       |       0        |" << endl;
	cout << "|------------------------------------------------------------------------|" << endl;
}