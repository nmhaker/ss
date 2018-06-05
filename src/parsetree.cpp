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
	
	// ASSEMBLER DIRECTIVES
	TreeNode* tn_dot = new TreeNode('.', Symbol);
	TreeNode* tn_global_g = new TreeNode('g', Symbol);
	TreeNode* tn_global_l = new TreeNode('l', Symbol);
	TreeNode* tn_global_o = new TreeNode('o', Symbol);
	TreeNode* tn_global_b = new TreeNode('b', Symbol);
	TreeNode* tn_global_a = new TreeNode('a', Symbol);
	TreeNode* tn_global_l2 = new TreeNode('l', Directive, ".global");
	tn_global_g->addChild(tn_global_l);
	tn_global_l->addChild(tn_global_o);
	tn_global_o->addChild(tn_global_b);
	tn_global_b->addChild(tn_global_a);
	tn_global_a->addChild(tn_global_l2);
	TreeNode* tn_text_t = new TreeNode('t', Symbol);
	TreeNode* tn_text_e = new TreeNode('e', Symbol);
	TreeNode* tn_text_x = new TreeNode('x', Symbol);
	TreeNode* tn_text_t2 = new TreeNode('t', Section, ".text");
	tn_text_t->addChild(tn_text_e);
	tn_text_e->addChild(tn_text_x);
	tn_text_x->addChild(tn_text_t2);
	TreeNode* tn_data_d = new TreeNode('d', Symbol);
	TreeNode* tn_data_a = new TreeNode('a', Symbol);
	TreeNode* tn_data_t = new TreeNode('t', Symbol);
	TreeNode* tn_data_a2 = new TreeNode('a', Section, ".data");
	tn_data_d->addChild(tn_data_a);
	tn_data_a->addChild(tn_data_t);
	tn_data_t->addChild(tn_data_a2);
	TreeNode* tn_bss_b = new TreeNode('b', Symbol);
	TreeNode* tn_bss_s = new TreeNode('s', Symbol);
	TreeNode* tn_bss_s2 = new TreeNode('s', Section, ".bss");
	tn_bss_b->addChild(tn_bss_s);
	tn_bss_s->addChild(tn_bss_s2);
	TreeNode* tn_end_e = new TreeNode('e', Symbol);
	TreeNode* tn_end_n = new TreeNode('n', Symbol);
	TreeNode* tn_end_d = new TreeNode('d', Directive, ".end");
	tn_end_e->addChild(tn_end_n);
	tn_end_n->addChild(tn_end_d);
	TreeNode* tn_char_c = new TreeNode('c', Symbol);
	TreeNode* tn_char_h = new TreeNode('h', Symbol);
	TreeNode* tn_char_a = new TreeNode('a', Symbol);
	TreeNode* tn_char_r = new TreeNode('r', Directive, ".char");
	tn_char_c->addChild(tn_char_h);
	tn_char_h->addChild(tn_char_a);
	tn_char_a->addChild(tn_char_r);
	TreeNode* tn_word_w = new TreeNode('w', Symbol);
	TreeNode* tn_word_o = new TreeNode('o', Symbol);
	TreeNode* tn_word_r = new TreeNode('r', Symbol);
	TreeNode* tn_word_d = new TreeNode('d', Directive, ".word");
	tn_word_w->addChild(tn_word_o);
	tn_word_o->addChild(tn_word_r);
	tn_word_r->addChild(tn_word_d);
	TreeNode* tn_long_l = new TreeNode('l', Symbol);
	TreeNode* tn_long_o = new TreeNode('o', Symbol);
	TreeNode* tn_long_n = new TreeNode('n', Symbol);
	TreeNode* tn_long_g = new TreeNode('g', Directive, ".long");
	tn_long_l->addChild(tn_long_o);
	tn_long_o->addChild(tn_long_n);
	tn_long_n->addChild(tn_long_g);
	TreeNode* tn_align_a = new TreeNode('a', Symbol);
	TreeNode* tn_align_l = new TreeNode('l', Symbol);
	TreeNode* tn_align_i = new TreeNode('i', Symbol);
	TreeNode* tn_align_g = new TreeNode('g', Symbol);
	TreeNode* tn_align_n = new TreeNode('n', Directive,".align");
	tn_align_a->addChild(tn_align_l);
	tn_align_l->addChild(tn_align_i);
	tn_align_i->addChild(tn_align_g);
	tn_align_g->addChild(tn_align_n);
	TreeNode* tn_skip_s = new TreeNode('s', Symbol);
	TreeNode* tn_skip_k = new TreeNode('k', Symbol);
	TreeNode* tn_skip_i = new TreeNode('i', Symbol);
	TreeNode* tn_skip_p = new TreeNode('p', Directive,".skip");
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
	tn_dot->addChild(tn_skip_s);	
	this->addChild(tn_dot);
	
	TreeNode* tn_a = new TreeNode('a', Symbol);
	TreeNode* tn_add_d = new TreeNode('d', Symbol);
	TreeNode* tn_add_d1 = new TreeNode('d', Instruction, "add");
	TreeNode* tn_and_n = new TreeNode('n', Symbol);
	TreeNode* tn_and_d = new TreeNode('d', Instruction, "and");
	TreeNode* tn_al_l = new TreeNode('l', Condition, "al");
	tn_a->addChild(tn_add_d)->addChild(tn_and_n)->addChild(tn_al_l);
	tn_add_d->addChild(tn_add_d1);
	tn_and_n->addChild(tn_and_d);
	this->addChild(tn_a);
	
	TreeNode* tn_s = new TreeNode('s', Symbol);
	TreeNode* tn_sub_u = new TreeNode('u', Symbol);
	TreeNode* tn_sub_b = new TreeNode('b', Instruction, "sub");
	TreeNode* tn_s_h = new TreeNode('h', Symbol);
	TreeNode* tn_shl_l = new TreeNode('l', Instruction, "shl");
	TreeNode* tn_shr_r = new TreeNode('r', Instruction, "shr");
	tn_s->addChild(tn_sub_u)->addChild(tn_s_h);
	tn_sub_u->addChild(tn_sub_b);
	tn_s_h->addChild(tn_shl_l)->addChild(tn_shr_r);
	this->addChild(tn_s);
	
	TreeNode* tn_m = new TreeNode('m', Symbol);
	TreeNode* tn_mul_u = new TreeNode('u', Symbol);
	TreeNode* tn_mul_l = new TreeNode('l', Instruction, "mul");
	TreeNode* tn_mov_o = new TreeNode('o', Symbol);
	TreeNode* tn_mov_v = new TreeNode('v', Instruction, "mov");	
	tn_m->addChild(tn_mul_u)->addChild(tn_mov_o);
	tn_mul_u->addChild(tn_mul_l);
	tn_mov_o->addChild(tn_mov_v);
	this->addChild(tn_m);
	
	TreeNode* tn_d = new TreeNode('d', Symbol);
	TreeNode* tn_div_i = new TreeNode('i', Symbol);
	TreeNode* tn_div_v = new TreeNode('v', Instruction, "div");
	tn_d->addChild(tn_div_i);
	tn_div_i->addChild(tn_div_v);
	this->addChild(tn_d);	
	
	TreeNode* tn_c = new TreeNode('c', Symbol);
	TreeNode* tn_cmp_m = new TreeNode('m', Symbol);
	TreeNode* tn_cmp_p = new TreeNode('p', Instruction, "cmp");
	TreeNode* tn_call_a = new TreeNode('a', Symbol);
	TreeNode* tn_call_l = new TreeNode('l', Symbol);
	TreeNode* tn_call_l1 = new TreeNode('l', Instruction, "call");
	tn_c->addChild(tn_cmp_m)->addChild(tn_call_a);
	tn_cmp_m->addChild(tn_cmp_p);
	tn_call_a->addChild(tn_call_l);
	tn_call_l->addChild(tn_call_l1);
	this->addChild(tn_c);
	
	TreeNode* tn_o = new TreeNode('o', Symbol);
	TreeNode* tn_or_r = new TreeNode('r', Instruction, "or");
	tn_o->addChild(tn_or_r);
	this->addChild(tn_o);
	
	TreeNode* tn_n = new TreeNode('n', Symbol);
	TreeNode* tn_not_o = new TreeNode('o', Symbol);
	TreeNode* tn_not_t = new TreeNode('t', Instruction, "not");
	TreeNode* tn_ne_e = new TreeNode('e', Condition, "ne");
	tn_n->addChild(tn_not_o)->addChild(tn_ne_e);
	tn_not_o->addChild(tn_not_t);
	this->addChild(tn_n);
	
	TreeNode* tn_t = new TreeNode('t', Symbol);
	TreeNode* tn_test_e = new TreeNode('e', Symbol);
	TreeNode* tn_test_s = new TreeNode('s', Symbol);
	TreeNode* tn_test_t = new TreeNode('t', Instruction, "test");
	tn_t->addChild(tn_test_e);
	tn_test_e->addChild(tn_test_s);
	tn_test_s->addChild(tn_test_t);
	this->addChild(tn_t);
	
	TreeNode* tn_p = new TreeNode('p', Symbol);
	TreeNode* tn_push_u = new TreeNode('u', Symbol);
	TreeNode* tn_push_s = new TreeNode('s', Symbol);
	TreeNode* tn_push_h = new TreeNode('h', Instruction, "push");
	TreeNode* tn_pop_o = new TreeNode('o', Symbol);
	TreeNode* tn_pop_p = new TreeNode('p', Instruction, "pop");
	tn_p->addChild(tn_push_u)->addChild(tn_pop_o);
	tn_push_u->addChild(tn_push_s);
	tn_push_s->addChild(tn_push_h);
	tn_pop_o->addChild(tn_pop_p);	
	this->addChild(tn_p);
	
	TreeNode* tn_i = new TreeNode('i', Symbol);
	TreeNode* tn_iret_r = new TreeNode('r', Symbol);
	TreeNode* tn_iret_e = new TreeNode('e', Symbol);
	TreeNode* tn_iret_t = new TreeNode('t', Instruction, "iret");
	tn_i->addChild(tn_iret_r);
	tn_iret_r->addChild(tn_iret_e);
	tn_iret_e->addChild(tn_iret_t);
	this->addChild(tn_i);
	
	TreeNode* tn_r = new TreeNode('r', Symbol);
	TreeNode* tn_ret_e = new TreeNode('e', Symbol);
	TreeNode* tn_ret_t = new TreeNode('t', Instruction);
	TreeNode* tn_r0_0 = new TreeNode('0', Regdir, "r0");
	TreeNode* tn_r1_1 = new TreeNode('1', Regdir, "r1");
	TreeNode* tn_r2_2 = new TreeNode('2', Regdir, "r2");
	TreeNode* tn_r3_3 = new TreeNode('3', Regdir, "r3");
	TreeNode* tn_r4_4 = new TreeNode('4', Regdir, "r4");
	TreeNode* tn_r5_5 = new TreeNode('5', Regdir, "r5");
	TreeNode* tn_r6_6 = new TreeNode('6', Regdir, "r6");
	TreeNode* tn_r7_7 = new TreeNode('7', Regdir, "r7");
	tn_r->addChild(tn_ret_e)->addChild(tn_r0_0)->addChild(tn_r1_1)->addChild(tn_r2_2)->addChild(tn_r3_3)->addChild(tn_r4_4)->addChild(tn_r5_5)->addChild(tn_r6_6)->addChild(tn_r7_7);
	tn_ret_e->addChild(tn_ret_t);	
	this->addChild(tn_r);
	
	TreeNode* tn_j = new TreeNode('j', Symbol);
	TreeNode* tn_jump_m = new TreeNode('m', Symbol);
	TreeNode* tn_jump_p = new TreeNode('p', Instruction, "jmp");
	tn_j->addChild(tn_jump_m);
	tn_jump_m->addChild(tn_jump_p);
	this->addChild(tn_j);
	
	TreeNode* tn_e = new TreeNode('e', Symbol);
	TreeNode* tn_eq_q = new TreeNode('q', Condition, "eq");
	tn_e->addChild(tn_eq_q);
	this->addChild(tn_e);
	
	TreeNode* tn_g = new TreeNode('g', Symbol);
	TreeNode* tn_gt_t = new TreeNode('t', Condition, "gt");
	tn_g->addChild(tn_gt_t);
	this->addChild(tn_g);
	
	TreeNode* tn_ampersand = new TreeNode('&', Address, "&");
	TreeNode* tn_asterisk = new TreeNode('*', Memdir, "*");
	TreeNode* tn_dollar = new TreeNode('$', Pcrel, "$");
	this->addChild(tn_ampersand)->addChild(tn_asterisk)->addChild(tn_dollar);
	
	TreeNode* tn_0 = new TreeNode('0', Symbol);
	TreeNode* tn_hex_x = new TreeNode('x', Hex, "0x");
	tn_0->addChild(tn_hex_x);
	this->addChild(tn_0);
	
	//Num of operands expected for instructions 
	this->addReqOp("iret", 0);
	this->addReqOp("ret", 0);
	this->addReqOp(".text", 0);
	this->addReqOp(".data", 0);
	this->addReqOp(".bss", 0);
	this->addReqOp(".end", 0);
	this->addReqOp("push", 1);
	this->addReqOp("pop", 1);
	this->addReqOp("call", 1);
	this->addReqOp("jmp", 1);
	this->addReqOp(".skip", 1);
	this->addReqOp(".align", 1);
	this->addReqOp(".char", 1);
	this->addReqOp(".word", 1);
	this->addReqOp(".long", 1);
	this->addReqOp("add", 2);
	this->addReqOp("sub", 2);
	this->addReqOp("mul", 2);
	this->addReqOp("div", 2);
	this->addReqOp("cmp", 2);
	this->addReqOp("and", 2);
	this->addReqOp("or", 2);
	this->addReqOp("not", 2);
	this->addReqOp("test", 2);
	this->addReqOp("mov", 2);
	this->addReqOp("shl", 2);
	this->addReqOp("shr", 2);
	this->addReqOp(".global", 3); // 3 means variable operand length
	
}

bool ParseTree::parse(std::string line, int lineNumber) {
	
	TreeNode* node = 0;
	list<string> instruction;
	instruction.push_back(to_string(lineNumber));
	string symbol;
	string arithmetic;
	bool whiteSpaceExpected = false;
	bool newLineExpected = false;
	bool conditionExpected = false;
	bool conditionWasExpected = false;
	bool conditionHit = false;
	bool symbolExpected = false;

	for(unsigned i=0; i<line.size(); i++){
		
		if(newLineExpected && !conditionExpected){
			if(isspace(line[i]))
				continue;
			else{
				cout << endl << "Error: New line expected at line: " << lineNumber << endl << flush;
				return false;
			}
		}
		
		if(!isspace(line[i]) && whiteSpaceExpected && !conditionExpected){
			cout << "Unrecognized instruction at line: " << lineNumber << endl;
			return false;
		}
		
		if(isspace(line[i]) && conditionExpected){
			conditionExpected = false;
//			whiteSpaceExpected = true;
			symbolExpected = true;
			whiteSpaceExpected = false;
			continue;
		}
		
		if(isspace(line[i]) && symbolExpected){
			symbolExpected = false;
			newLineExpected = true;
			continue;
		}
		
		if(isspace(line[i]) && conditionWasExpected && !conditionHit){
			cout << "Error: irregular instruction suffix" << endl << flush;
			return false;
		}
		
		if(isspace(line[i]) && node == 0){
//			cout << line[i] << flush;
			arithmetic.clear();
			whiteSpaceExpected = false;
			continue;
		}
		
		if(isspace(line[i] && node != 0)){
			whiteSpaceExpected = true;
			continue;
		}
		
		try{
			if(node == 0)
				node = root.at(line[i]);			
			else
				node = node->getChildren()->at(line[i]);
			if(node->getValue() != "Undefined"){
//				cout << node->getSymbol() << flush;
				symbol += node->getSymbol();
				if(symbol != node->getValue()){
					cout << "Unrecognized instruction, instruction has wrong bytes at beginning" << endl;
					return false;
				}
				instruction.push_back(node->getValue());				
				symbol.clear();
				
				// WhiteSpace expected if it is not regdir, memdir, address, pcrel, regindpom, hex
				if( (node->getType() != Regdir) && (node->getType() != Memdir) && (node->getType() != Address) && (node->getType() != Pcrel) && (node->getType() != Regindpom) && (node->getType() != Hex))
					whiteSpaceExpected = true;	
				
				// Forbid placing code after section directive
				if( (node->getType() == Section ))
					newLineExpected = true;
				
				// Enable condition suffix
				if( (node->getType() == Instruction )){
					conditionExpected = true;
					conditionHit = false;
				}
				
				// Forbid condition suffix
				if( (node->getType() == Condition )){
					conditionExpected = false;
					conditionWasExpected = true;
					conditionHit = true;
				}
				node = 0;
				
				
			}
			else{
//				cout << node->getSymbol() << flush;
				symbol+=node->getSymbol();
			}
		}catch(out_of_range e){
			if(symbol.empty() && line[i] == ','){
				continue;
			}
			if(symbol.empty()){
				if(isdigit(line[i])){
					symbol+=line[i];
//					cout << line[i] << flush;
					continue;
				}
				if(isalpha(line[i])){
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
			if(!symbol.empty() && line[i]==':'){
				instruction.push_back(symbol);
				symbol.clear();
				node=0;
//				cout << line[i] << flush;
				continue;
			}
			if(line[i] == '+' || line[i]=='-'){
				if(arithmetic.empty()){
					instruction.push_back("+");
					instruction.push_back(symbol);
					symbol.clear();
					node=0;
					arithmetic = line[i];
					continue;
				}else{
					instruction.push_back(arithmetic);
					instruction.push_back(symbol);
					symbol.clear();
					node=0;
					arithmetic = line[i];
					continue;
				}
			}
			try{
				if(node != 0){
					node->getChildren()->at(line[i]);
					continue;
				}else{
					cout << "Bad syntax at line: " << lineNumber << endl;	
					return false;
				}
			}catch(out_of_range e){
				symbol+=line[i];
				continue;
			}									
		}
		
	}
	if(!arithmetic.empty()){
		instruction.push_back(arithmetic);
		arithmetic.clear();
	}
	if(!symbol.empty())
		instruction.push_back(symbol);
//	cout << "Instrukcija: " << lineNumber << endl;
	cout << endl << flush;
	
	//PARSED INSTRUCTION
	for(list<string>::iterator it = instruction.begin(); it!=instruction.end(); it++){
		cout << *it << endl;
	}
	
	return true;
}

ParseTree* ParseTree::addChild(TreeNode* node) {
	root.insert( std::pair<char, TreeNode*>(node->getSymbol(), node));
	return this;
}

ParseTree* ParseTree::addReqOp(std::string instruction, int num_of_operands) {
	req_op.insert( std::pair<string, int>(instruction, num_of_operands));
	return this;
}
