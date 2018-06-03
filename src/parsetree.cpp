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
	TreeNode* tn_dot = new TreeNode('.', Symbol);
	TreeNode* tn_global_g = new TreeNode('g', Symbol);
	TreeNode* tn_global_l = new TreeNode('l', Symbol);
	TreeNode* tn_global_o = new TreeNode('o', Symbol);
	TreeNode* tn_global_b = new TreeNode('b', Symbol);
	TreeNode* tn_global_a = new TreeNode('a', Symbol);
	TreeNode* tn_global_l2 = new TreeNode('l', Directive, "global");
	tn_global_g->addChild(tn_global_l);
	tn_global_l->addChild(tn_global_o);
	tn_global_o->addChild(tn_global_b);
	tn_global_b->addChild(tn_global_a);
	tn_global_a->addChild(tn_global_l2);
	TreeNode* tn_text_t = new TreeNode('t', Symbol);
	TreeNode* tn_text_e = new TreeNode('e', Symbol);
	TreeNode* tn_text_x = new TreeNode('x', Symbol);
	TreeNode* tn_text_t2 = new TreeNode('t', Section, "text");
	tn_text_t->addChild(tn_text_e);
	tn_text_e->addChild(tn_text_x);
	tn_text_x->addChild(tn_text_t2);
	TreeNode* tn_data_d = new TreeNode('d', Symbol);
	TreeNode* tn_data_a = new TreeNode('a', Symbol);
	TreeNode* tn_data_t = new TreeNode('t', Symbol);
	TreeNode* tn_data_a2 = new TreeNode('a', Section, "data");
	tn_data_d->addChild(tn_data_a);
	tn_data_a->addChild(tn_data_t);
	tn_data_t->addChild(tn_data_a2);
	TreeNode* tn_bss_b = new TreeNode('b', Symbol);
	TreeNode* tn_bss_s = new TreeNode('s', Symbol);
	TreeNode* tn_bss_s2 = new TreeNode('s', Section, "bss");
	tn_bss_b->addChild(tn_bss_s);
	tn_bss_s->addChild(tn_bss_s2);
	TreeNode* tn_end_e = new TreeNode('e', Symbol);
	TreeNode* tn_end_n = new TreeNode('n', Symbol);
	TreeNode* tn_end_d = new TreeNode('d', Directive, "end");
	tn_end_e->addChild(tn_end_n);
	tn_end_n->addChild(tn_end_d);
	TreeNode* tn_char_c = new TreeNode('c', Symbol);
	TreeNode* tn_char_h = new TreeNode('h', Symbol);
	TreeNode* tn_char_a = new TreeNode('a', Symbol);
	TreeNode* tn_char_r = new TreeNode('r', Directive, "char");
	tn_char_c->addChild(tn_char_h);
	tn_char_h->addChild(tn_char_a);
	tn_char_a->addChild(tn_char_r);
	TreeNode* tn_word_w = new TreeNode('w', Symbol);
	TreeNode* tn_word_o = new TreeNode('o', Symbol);
	TreeNode* tn_word_r = new TreeNode('r', Symbol);
	TreeNode* tn_word_d = new TreeNode('d', Directive, "word");
	tn_word_w->addChild(tn_word_o);
	tn_word_o->addChild(tn_word_r);
	tn_word_r->addChild(tn_word_d);
	TreeNode* tn_long_l = new TreeNode('l', Symbol);
	TreeNode* tn_long_o = new TreeNode('o', Symbol);
	TreeNode* tn_long_n = new TreeNode('n', Symbol);
	TreeNode* tn_long_g = new TreeNode('g', Directive, "long");
	tn_long_l->addChild(tn_long_o);
	tn_long_o->addChild(tn_long_n);
	tn_long_n->addChild(tn_long_g);
	TreeNode* tn_align_a = new TreeNode('a', Symbol);
	TreeNode* tn_align_l = new TreeNode('l', Symbol);
	TreeNode* tn_align_i = new TreeNode('i', Symbol);
	TreeNode* tn_align_g = new TreeNode('g', Symbol);
	TreeNode* tn_align_n = new TreeNode('n', Directive,"align");
	tn_align_a->addChild(tn_align_l);
	tn_align_l->addChild(tn_align_i);
	tn_align_i->addChild(tn_align_g);
	tn_align_g->addChild(tn_align_n);
	TreeNode* tn_skip_s = new TreeNode('s', Symbol);
	TreeNode* tn_skip_k = new TreeNode('k', Symbol);
	TreeNode* tn_skip_i = new TreeNode('i', Symbol);
	TreeNode* tn_skip_p = new TreeNode('p', Directive,"skip");
	tn_skip_s->addChild(tn_skip_k);
	tn_skip_k->addChild(tn_skip_i);
	tn_skip_i->addChild(tn_skip_p);
	tn_dot->addChild(tn_global_g);
	tn_dot->addChild(tn_text_t);
	tn_dot->addChild(tn_data_d);
	tn_dot->addChild(tn_bss_b);
	tn_dot->addChild(tn_end_e);
	tn_dot->addChild(tn_char_c);
	tn_dot->addChild(tn_word_w);
	tn_dot->addChild(tn_long_l);
	tn_dot->addChild(tn_align_a);
	tn_dot->addChild(tn_skip_p);
	this->root.insert(std::pair<char, TreeNode*>(tn_dot->getSymbol(), tn_dot));
}

bool ParseTree::parse(std::string line, int lineNumber) {
	TreeNode* node = 0;
	list<string> instruction;
	instruction.push_back(to_string(lineNumber));
	string symbol;
	for(unsigned i=0; i<line.size(); i++){
		if(isspace(line[i]) && node == 0){
//			cout << line[i];
			continue;
		}
		try{
			if(node == 0)
				node = root.at(line[i]);			
			else
				node = node->getChildren()->at(line[i]);
			if(node->getValue() != "Undefined"){
//				cout << node->getSymbol() << flush;
				instruction.push_back(node->getValue());				
				symbol.clear();
				node = 0;
			}
			else{
//				cout << node->getSymbol() << flush;
				symbol+=node->getSymbol();
			}
		}catch(out_of_range e){
			if(symbol.empty()){
				if(!isalpha(line[i])){
//					cout << "Neispravna sintaksa!" << endl;
					return false;
				}else{
					symbol+=line[i];
//					cout << line[i] << flush;
					continue;
				}
			}
			if(!symbol.empty() && line[i]==','){
				instruction.push_back(symbol);
				symbol.clear();
				node = 0;
//				cout << line[i] << flush;
				continue;
			}
			
//			cout << "Sintaksa neispravna:" << endl;	
			return false;
		}
		
	}
	if(!symbol.empty())
			instruction.push_back(symbol);
//	cout << "Instrukcija: " << lineNumber << endl;
	for(list<string>::iterator it = instruction.begin(); it!=instruction.end(); it++){
		cout << *it << endl;
	}
	return true;
}

