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
	
private:
	std::map<char, TreeNode*> root;
	std::map<std::string, int> req_op;
	
	int size;
	
	void populateTree();
	std::string getInstructionField(std::list<std::string>,std::string);
	
	ParseTree* addChild(TreeNode*);
	ParseTree* addReqOp(std::string, int);
};

#endif /* PARSETREE_H */

