/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   treenode.h
 * Author: buki
 *
 * Created on June 3, 2018, 1:32 AM
 */

#ifndef TREENODE_H
#define TREENODE_H

#include <map>

enum Type{
	Undefined,
	Directive,
	Section,
	Instruction,
	Condition,
	Symbol,
	Forbidden,
	Expression,
	Immediate,
	Memdir,
	Pcrel,
	Address,
	Memdir_imm,
	Regdir,
	Regindpom,
	Hex
};

class TreeNode{
public:
	TreeNode(char c, Type t=Undefined, std::string value="Undefined");
	~TreeNode();
		
	TreeNode* addChild(TreeNode*);
	
	char getSymbol();
	Type getType();
	std::map<char, TreeNode*>* getChildren();
	std::string getValue();
	

private:
	char symbol;
	std::map<char, TreeNode*> children;
	Type type;
	std::string value;
};


#endif /* TREENODE_H */

