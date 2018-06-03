/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   exception.cpp
 * Author: buki
 * 
 * Created on June 3, 2018, 1:26 AM
 */

#include "../include/exception.h"

#include <iostream>

Exception::Exception(const char* str) {
	std::cout << str << std::endl;
}

Exception::~Exception() {
}

