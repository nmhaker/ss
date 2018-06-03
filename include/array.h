/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   array.h
 * Author: buki
 *
 * Created on June 3, 2018, 12:24 AM
 */

#ifndef ARRAY_H
#define ARRAY_H

#include <list>
#include <string>

class Array{

public:
	Array(std::list<std::string>*);
	~Array();

	std::string getLine(int number);
	void printArray();
	int getSize();
	
private:
	std::string** array;
	int size;
};


#endif /* ARRAY_H */

