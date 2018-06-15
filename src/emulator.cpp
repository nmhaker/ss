#include "..\include\emulator.h"

#include <iostream>

using namespace std;

Emulator::Emulator(int argc, char** argv)
{
	linker = new Linker(argc, argv);

	memory = linker->makeExecutable();
	startAddress = linker->getStartLocation();

	regs = new short[8]{ 0 };

	startMainLoop();
}

Emulator::~Emulator()
{
	if (linker != 0)
		delete linker;
	if (memory != 0)
		delete memory;
}

void Emulator::startMainLoop()
{
	//	Entry point into program
	regs[PC] = startAddress;

	while (1) {

		int condition = 0;
		int opcode = 0;
		int dstaddr = 0;
		int dst = 0;
		int srcaddr = 0;
		int src = 0;

		decodeInstruction(condition, opcode, dstaddr, dst, srcaddr, src );

		short firstOperand = getOperand(dstaddr, dst);
		short secondOperand = getOperand(srcaddr, src);

		switch (opcode) {

			case ADD:
				setOperand(dstaddr, dst, firstOperand + secondOperand);
				break;
			case SUB:
				setOperand(dstaddr, dst, firstOperand - secondOperand);
				break;
			case MUL:
				setOperand(dstaddr, dst, firstOperand * secondOperand);
				break;
			case DIV:
				setOperand(dstaddr, dst, firstOperand / secondOperand);
				break;
			case CMP:
				updateFlags(firstOperand - secondOperand);
				break;
			case AND:
				setOperand(dstaddr, dst, firstOperand & secondOperand);
				break;
			case OR:
				setOperand(dstaddr, dst, firstOperand | secondOperand);
				break;
			case NOT:
				setOperand(dstaddr, dst, ~secondOperand);
				break;
			case TEST:
				updateFlags(firstOperand & secondOperand);
				break;
			case PUSH:
				regs[SP] -= 2;
				writeShort(regs[SP], secondOperand);
				break;
			case POP:
				switch (dstaddr) {
				case IMMED:
					if (dst == PSW) {
						psw = getShort(regs[SP]);
						regs[SP] += 2;
					}
					break;
					//else{}  Forbidden putting in immed in syntax parsing
				case REGDIR:
					regs[dst] = getShort(regs[SP]);
					regs[SP] += 2;
					break;
				case MEMDIR:
					writeShort(getPom_NoChange(), getShort(regs[SP]));
					regs[SP] += 2;
					break;
				case REGINDPOM:
					writeShort(getPom_NoChange() + regs[dst], regs[SP]);
					regs[SP] += 2;
					break;
				}
				break;
			case CALL:
				regs[SP] -= 2;
				writeShort(regs[SP], regs[PC]);
				regs[PC] = secondOperand;
				break;
			case IRET:
				psw = getShort(regs[SP]);
				regs[SP] += 2;
				regs[PC] = getShort(regs[SP]);
				regs[SP] += 2;
				break;
			case MOV:
				setOperand(dstaddr, dst, secondOperand);
				break;
			case SHL:
				setOperand(dstaddr, dst, firstOperand << secondOperand);
				break;
			case SHR:
				setOperand(dstaddr, dst, firstOperand >> secondOperand);
				break;
		}
		if (opcode == PUSH || opcode == CALL) {
			if (hasPom(srcaddr, src))
				regs[PC] += 2;
		}
		else if (opcode == POP) {
			if (hasPom(dstaddr, dst))
				regs[PC] += 2;
		}else if (hasPom(dstaddr, dst) || hasPom(srcaddr, src))
			regs[PC] += 2;
	}
}

void Emulator::decodeInstruction(int& cond, int& opcode, int& addrdest, int& dest, int& addrsrc, int& src)
{
	unsigned char firstByte = memory[regs[PC]++];
	unsigned char secondByte = memory[regs[PC]++];

	cond = (firstByte >> 6);
	opcode = ((firstByte >> 2) & 0xF);
	addrdest = (firstByte & 0x3);

	dest = (secondByte >> 5);
	addrsrc = ( (secondByte >> 3) & 3);
	src = (secondByte & 7);
}



short Emulator::getOperand(int& addressing, int& src)
{
	switch (addressing) {
		case IMMED:
			if (src == PSW) 
				return psw;
			 else 
				return getPom_NoChange();
		case REGDIR:
			return regs[src];
		case MEMDIR:
			return getShort(getPom_NoChange());
		case REGINDPOM:
			return getShort(getPom_NoChange() + regs[src]);
		default: {
			cout << "Emulator error: Not supported addressing" << endl;
			exit(1);
		}
	}
}

short Emulator::getPom_NoChange()
{
	char firstByte = memory[regs[PC]];
	char secondByte = memory[regs[PC]+1];

	short coded_value = 0;
	coded_value |= firstByte;
	coded_value |= (secondByte << 8);

	return coded_value;
}

bool Emulator::hasPom(int addressing, int dst)
{
	if ((addressing != REGDIR) && !(addressing == IMMED && dst == PSW))
		return true;
	else
		return false;
}

short Emulator::getShort(short address)
{
	char firstByte = memory[address];
	char secondByte = memory[address+1];

	short coded_value = 0;
	coded_value |= firstByte;
	coded_value |= (secondByte << 8);

	return coded_value;
}

void Emulator::writeShort(short address, short value)
{
	char firstByte = value & 0xff;
	char secondByte = ((value & (0xff << 8)) >> 8);

	memory[address] = firstByte;
	memory[address + 1] = secondByte;
}

void Emulator::setOperand(int addressing, int dst, short value)
{
	switch (addressing) {
		case IMMED:
			if (dst == PSW)
				psw = value;
			//else{}  Forbidden putting in immed in syntax parsing
		case REGDIR:
			regs[dst] = value;
		case MEMDIR:
			writeShort(getPom_NoChange(), value);
		case REGINDPOM:
			writeShort(getPom_NoChange() + regs[dst], value);
	}
	updateFlags(value);
}

void Emulator::updateFlags(short value)
{
	cout << endl << "Flags updating not implemented" << endl << flush;
}
