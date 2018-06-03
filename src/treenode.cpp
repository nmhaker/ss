/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "../include/treenode.h"

TreeNode::TreeNode(char c, Type t, std::string value) {
	this->symbol = c;
	this->type = t;
	this->value = value;
}

TreeNode::~TreeNode() {
	if(!children.empty()){
		for(std::map<char, TreeNode*>::iterator it=children.begin(); it!=children.end(); it++){
			delete it->second;
			children.erase(it);
		}		
	}
}

TreeNode* TreeNode::addChild(TreeNode* node) {
	children.insert( std::pair<char, TreeNode*>(node->symbol, node));
	return this;
}

std::map<char, TreeNode*>* TreeNode::getChildren() {
	return &children;
}

char TreeNode::getSymbol() {
	return symbol;
}

Type TreeNode::getType() {
	return type;
}

std::string TreeNode::getValue() {
	return this->value;
}
