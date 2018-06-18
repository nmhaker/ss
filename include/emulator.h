#ifndef EMULATOR_H
#define EMULATOR_H

#include "linker.h"

#include <thread>
#include <chrono>
#include <atomic>
#include <list>

const int PSW = 7;
const int PC = 7;
const int SP = 6;

enum Addressing {
	IMMED = 0,
	REGDIR,
	MEMDIR,
	REGINDPOM
};

enum Conditions {
	EQ = 0,
	NE,
	GT,
	AL
};

enum Instructions {
	ADD = 0,
	SUB,
	MUL,
	DIV,
	CMP,
	AND,
	OR,
	NOT,
	TEST,
	PUSH,
	POP,
	CALL,
	IRET,
	MOV,
	SHL,
	SHR
};

enum Interrupts{
	TIMER,
	KEY_PRESS
};

struct Limit {
	int startAddress;
	int size;
};

void foo_timer_at_1(std::list<int>* interrupts);
	;
void foo_key_listener(char*, std::list<int>* interrupts);

class Emulator {
public:

	Emulator(int argc, char** argv);
	~Emulator();

private:


	Linker* linker;
	char* memory;
	int startAddress;

	void startMainLoop();

	void decodeInstruction(int&, int&, int&, int&, int&, int&);
	short getOperand(int&, int&);
	short getPom_NoChange();
	bool hasPom(int,int);
	short getShort(unsigned short address);
	void writeShort(unsigned short address, short value);

	void setOperand(int addressing, int dst, short value);

	void updateAllFlagsAdd(short firstOperand, short secondOperand, short value);
	void updateAllFlagsSub(short firstOperand, short secondOperand, short value);
	void updateZNFlags(short value);
	void updateZNCFlags(short firstOperand, short secondOperand, short value, bool right=false);

	int* regs;
	short psw;

	void setZ(unsigned short i);
	void setO(unsigned short i);
	void setC(unsigned short i);
	void setN(unsigned short i);
	void setI(unsigned short i);

	unsigned short getZ();
	unsigned short getO();
	unsigned short getC();
	unsigned short getN();
	unsigned short getI();

	unsigned short getTimerBit();

	bool checkCondition(int condition);

	bool checkInTextSection(unsigned short address);

	std::map<int, Limit> textSectionLimits;
	void setTextSectionLimits();

	char* ivt_entry_0;	//	Start interrupt routine
	char* ivt_entry_1;	//	1 second interrupt routine
	char* ivt_entry_2;	//	incorrect instruction interrupt routine
	char* ivt_entry_3;	//	key pressed interupt routine
	char* ivt_entry_4;
	char* ivt_entry_5;
	char* ivt_entry_6;
	char* ivt_entry_7;

	char *per_out;
	char *per_in;

	std::thread* timer_at_1_second;
	std::thread* key_press_listener;

	std::list<int> interrupts;

	void checkForInterrupts();

	bool end;

};

#endif
