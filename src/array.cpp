/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <list>

#include "../include/array.h"

Array::Array(std::list l){
	size = l.size;
	array = new char*[size];

	for(std::list<string>::const_iterator it = l.begin(), int i=0; it!=l.end(); l++,i++){
			array[i] = new char[(*it).size()];
			array[i] = (*it).t
	}
}

Array::~Array() {

}

