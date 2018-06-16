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

	bool checkCondition(int condition);
};

#endif