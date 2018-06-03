/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   parsetree.cpp
 * Author: buki
 * 
 * Created on June 3, 2018, 1:31 AM
 */

#include "../include/parsetree.h"
#include "../include/exception.h"

using namespace std;

ParseTree::ParseTree() {
	this->size = 15;
	
	this->populateTree();
}

ParseTree::~ParseTree() {
}

void ParseTree::populateTree() {
	TreeNode* tn_dot = new TreeNode('.');
	TreeNode* tn_global = new TreeNode('g', Directive, ".global");
	TreeNode* tn_text = new TreeNode('t', Directive, ".text");
	TreeNode* tn_data = new TreeNode('d', Directive, ".data");
	TreeNode* tn_bss = new TreeNode('b', Directive, ".bss");
	TreeNode* tn_end = new TreeNode('e', Directive, ".end");
	TreeNode* tn_char = new TreeNode('c', Directive, ".char");
	TreeNode* tn_word = new TreeNode('w', Directive, ".word");
	TreeNode* tn_long = new TreeNode('l', Directive, ".long");
	TreeNode* tn_align = new TreeNode('a', Directive,".align");
	TreeNode* tn_skip = new TreeNode('s', Directive,".skip");
	tn_dot->addChild(tn_global);
	tn_dot->addChild(tn_text);
	tn_dot->addChild(tn_data);
	tn_dot->addChild(tn_bss);
	tn_dot->addChild(tn_end);
	tn_dot->addChild(tn_char);
	tn_dot->addChild(tn_word);
	tn_dot->addChild(tn_long);
	tn_dot->addChild(tn_align);
	tn_dot->addChild(tn_skip);
	this->root.insert(std::pair<char, TreeNode*>(tn_dot->getSymbol(), tn_dot));
}

bool ParseTree::parse(std::string line, int lineNumber) {
	TreeNode* node = 0;
	for(int i=0; i<line.size(); i++){
		if(isspace(line[i]) && node != 0)
			continue;
		else{
			throw new Exception("Failed parsing at line: "+(char)lineNumber+" at symbol: " +node->getSymbol());	
		}
		try{
			if(node == 0)
				node = root.at(line[i]);
			else
				node = node->getChildren().at(line[i]);
		}catch(out_of_range e){
			cout << "Out of range error:" << e.what() << endl;	
		}
	}
}

