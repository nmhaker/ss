/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <list>
#include <string>
#include <iostream>

#include "../include/array.h"
#include "../include/exception.h"

using namespace std;

Array::Array(list<string> *l){
	size = l->size();
	array = new string*[size];

	int i = 0;
	for(list<string>::const_iterator it = l->begin(); it!=l->end(); it++){
		array[i] = new string;
		*array[i] = (*it);
		i++;
	}
}

void Array::printArray() {
	for(int i=0; i<size; i++){
		cout << *array[i] << endl;
	}
}


Array::~Array() {
	for(int i=0; i<size; i++)
		delete array[i];
	delete array;
}

std::string Array::getLine(int number) {
	if(number < 1 || number > size)
			throw new Exception("array.cpp: 42: Array out of bounds, array starts at 1");
	return *array[number-1];
}

int Array::getSize() {
	return size;
}
