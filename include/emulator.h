#ifndef EMULATOR_H
#define EMULATOR_H

#include "linker.h"

#define PSW 7
#define PC 7
#define SP 6

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
	short getShort(short address);
	void writeShort(unsigned short address, short value);

	void setOperand(int addressing, int dst, short value);
	void updateFlags(short value);

	int* regs;
	short psw;
};

#endif