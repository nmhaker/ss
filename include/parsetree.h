/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   parsetree.h
 * Author: buki
 *
 * Created on June 3, 2018, 1:31 AM
 */

#ifndef PARSETREE_H
#define PARSETREE_H

#include <map>
#include <list>
#include <string>

#include "treenode.h"

class ParseTree {
public:
    ParseTree();
    virtual ~ParseTree();
	
	bool parse(std::string line, int lineNumber);
	std::list<std::string> getParsedInstruction(int line);

	static std::string getInstructionField(std::list<std::string>,std::string);

private:
	std::map<char, TreeNode*> root;
	std::map<std::string, int> req_op;

	std::map<int, std::list<std::string>> parsed_instructions;
	
	int size;
	
	bool checkAddressing(std::list<std::string> list, std::string addressing);
	
	void populateTree();
	ParseTree* addChild(TreeNode*);
	ParseTree* addReqOp(std::string, int);
};

#endif /* PARSETREE_H */

