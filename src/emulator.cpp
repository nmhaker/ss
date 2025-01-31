#include "../include/emulator.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
#include <stropts.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include <list>

using namespace std;

int _kbhit() {
	static const int STDIN = 0;
	static bool initialized = false;

	if (! initialized) {
		// Use termios to turn off line buffering
		termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		termios newterm = term;
		newterm.c_lflag &= ~ECHO;
		tcsetattr(STDIN, TCSANOW, &newterm);
		setbuf(stdin, NULL);
		initialized = true;
	}

	int bytesWaiting;
	ioctl(STDIN, FIONREAD, &bytesWaiting);
	return bytesWaiting;
}

void foo_timer_at_1(list<int>* interrupts){
	while(1){
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		interrupts->push_back(TIMER);
	}
}

void foo_key_listener(char* dst, list<int>* interrupts){
	while(1){
		while (! _kbhit()) { } 

		char c = getchar();
		*dst = c;

		interrupts->push_back(KEY_PRESS);
	}
}

Emulator::Emulator(int argc, char** argv)
{
	linker = new Linker(argc, argv);

	memory = linker->makeExecutable();
	startAddress = linker->getStartLocation();

	regs = new int[8]{ 0 };
	
	regs[SP] = 0xff80;

	psw = 1;
	psw |= (1 << 12);
	setI(1);

	ivt_entry_0 = memory;
	ivt_entry_1 = &(memory[2]);
	ivt_entry_2 = &(memory[4]);
	ivt_entry_3 = &(memory[6]);
	ivt_entry_4 = &(memory[8]);
	ivt_entry_5 = &(memory[10]);
	ivt_entry_6 = &(memory[12]);
	ivt_entry_7 = &(memory[14]);

	per_out = &memory[0xfffe];
	per_in = &memory[0xfffc];

	end = false;

	setTextSectionLimits();

	timer_at_1_second = new thread(foo_timer_at_1, &interrupts);
	key_press_listener = new thread(foo_key_listener, per_in, &interrupts);

	startMainLoop();
}

Emulator::~Emulator()
{
	if (linker != 0)
		delete linker;
	if (memory != 0)
		delete memory;
	if (timer_at_1_second != 0){
		cout << endl << "Waiting for timer thread to finish" << endl << flush;
		timer_at_1_second->join();
		delete timer_at_1_second;
	}
	if (key_press_listener != 0){
		cout << endl << "Waiting for listener thread to finish" << endl << flush;
		key_press_listener->join();
		delete key_press_listener;
	}
}

void Emulator::startMainLoop()
{

	cout << endl << "EXECUTING PROGRAM..." << endl << flush;

	//	Entry point into program
	regs[PC] = startAddress;

	bool iret = false;

	while (!end && !iret) {

		int condition = 0;
		int opcode = 0;
		int dstaddr = 0;
		int dst = 0;
		int srcaddr = 0;
		int src = 0;

		decodeInstruction(condition, opcode, dstaddr, dst, srcaddr, src );

		short firstOperand = getOperand(dstaddr, dst);
		short secondOperand = getOperand(srcaddr, src);

		if (!checkCondition(condition)) {

			if (opcode == PUSH || opcode == CALL) {
				if (hasPom(srcaddr, src))
					regs[PC] += 2;
			}
			else if (opcode == POP) {
				if (hasPom(dstaddr, dst))
					regs[PC] += 2;
			}
			else if (hasPom(dstaddr, dst) || hasPom(srcaddr, src))
				regs[PC] += 2;

			continue;
		}

		switch (opcode) {

			case ADD:
				if (dstaddr == 1 && dst == 7){
//					cout << "PSEUDO JUMP" << endl;
				}
				else{
//					cout << "ADD" << endl;
				}
				setOperand(dstaddr, dst, firstOperand + secondOperand);
				updateAllFlagsAdd(firstOperand, secondOperand, firstOperand + secondOperand);
				break;
			case SUB:
//				cout << "SUB" << endl;
				setOperand(dstaddr, dst, firstOperand - secondOperand);
				updateAllFlagsSub(firstOperand, secondOperand, firstOperand - secondOperand);
				break;
			case MUL:
//				cout << "MUL" << endl;
				setOperand(dstaddr, dst, firstOperand * secondOperand);
				updateZNFlags(firstOperand*secondOperand);
				break;
			case DIV:
//				cout << "DIV" << endl;
				setOperand(dstaddr, dst, firstOperand / secondOperand);
				updateZNFlags(firstOperand/secondOperand);
				break;
			case CMP:
//				cout << "CMP" << endl;
				updateAllFlagsSub(firstOperand, secondOperand, firstOperand - secondOperand);
				break;
			case AND:
//				cout << "AND" << endl;
				setOperand(dstaddr, dst, firstOperand & secondOperand);
				updateZNFlags(firstOperand&secondOperand);
				break;
			case OR:
//				cout << "OR" << endl;
				setOperand(dstaddr, dst, firstOperand | secondOperand);
				updateZNFlags(firstOperand|secondOperand);
				break;
			case NOT:
//				cout << "NOT" << endl;
				setOperand(dstaddr, dst, ~secondOperand);
				updateZNFlags(~secondOperand);
				break;
			case TEST:
//				cout << "TEST" << endl;
				updateZNFlags(firstOperand & secondOperand);
				break;
			case PUSH:
//				cout << "PUSH" << endl;
				regs[SP] -= 2;
				writeShort(regs[SP], secondOperand);
				break;
			case POP:
//				cout << "POP" << endl;
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
//				cout << "CALL" << endl;
				regs[SP] -= 2;
				if (hasPom(srcaddr, src))
					writeShort(regs[SP], regs[PC] + 2);
				else
					writeShort(regs[SP], regs[PC]);
				regs[PC] = secondOperand;
				continue;
			case IRET:
//				cout << "IRET" << endl;
				psw = getShort(regs[SP]);
				regs[SP] += 2;
				regs[PC] = getShort(regs[SP]);
				regs[SP] += 2;
				iret = true;
				continue;
			case MOV:
				if ((dstaddr == srcaddr) && (srcaddr == REGDIR) && (dst == src) && (src == PC)) {
//				cout << "HALT" << endl;
					end = true;
					continue;
				}else if (dstaddr == REGDIR && dst == PC){
//					cout << "PSEUDO JUMP" << endl;
				}
				else{
//					cout << "MOV" << endl;
				}
				setOperand(dstaddr, dst, secondOperand);
				updateZNFlags(secondOperand);
				break;
			case SHL:
//				cout << "SHL" << endl;
				setOperand(dstaddr, dst, firstOperand << secondOperand);
				updateZNCFlags(firstOperand, secondOperand, firstOperand << secondOperand, false);
				break;
			case SHR:
//				cout << "SHR" << endl;
				setOperand(dstaddr, dst, firstOperand >> secondOperand);
				updateZNCFlags(firstOperand, secondOperand, firstOperand >> secondOperand, true);
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
		if(getI()){
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			checkForInterrupts();
		}else{
			//std::this_thread::sleep_for(std::chrono::milliseconds(200));	
		}
	}
}

void Emulator::checkForInterrupts(){


	if(interrupts.size() > 0){

		int inter = interrupts.front();
		interrupts.pop_front();

		if(!getI())
			return;

		cout << endl << "ENTERING INTERRUPT ROUTINE" << endl << flush;

		if( inter == TIMER && getTimerBit()){
			short address = 0;
			address |= ivt_entry_1[0];
			address |= (ivt_entry_1[1] << 8);
			//cout << "OBRADJUJEM JEDAN PREKID TAJMERA" << endl;
			startAddress = address;
			setI(0);
			regs[SP] -= 2;
			writeShort(regs[SP], regs[PC]);
			regs[SP] -= 2;
			writeShort(regs[SP], psw);
			startMainLoop();
			setI(1);
			//exit(1);
		}else if( inter == KEY_PRESS ){
			short address = 0;
			address |= ivt_entry_3[0];
			address |= (ivt_entry_3[1] << 8);
			//cout << "OBRADJUJEM JEDAN PREKID TASTATURE" << endl;
			startAddress = address;
			setI(0);
			regs[SP] -= 2;
			writeShort(regs[SP], regs[PC]);
			regs[SP] -= 2;
			writeShort(regs[SP], psw);
			startMainLoop();
			setI(1);
			//exit(1);
		}
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

short Emulator::getShort(unsigned short address)
{
	char firstByte = memory[address];
	short secondByte = memory[address+1];

	short coded_value = 0;
	coded_value = (coded_value | firstByte) & 0x00ff;
	coded_value = (coded_value | ((secondByte << 8)&0xff00));

	return coded_value;
}

void Emulator::writeShort(unsigned short address, short value)
{
	if (address < 0 || address >= (1 << 16)) {
		cout << "Emulator error: out of Virtual Address space write requested" << endl << flush;
		short address = 0;
		address |= ivt_entry_2[0];
		address |= (ivt_entry_2[1] << 8);
		startAddress = address;
		startMainLoop();
		//exit(1);
	}
	if (checkInTextSection(address)) {
		cout << "Emulator error: segmentation fault, requested to write into text section" << endl << flush;
		short address = 0;
		address |= ivt_entry_2[0];
		address |= (ivt_entry_2[1] << 8);
		startAddress = address;
		startMainLoop();
		//exit(1);
	}
	
	if(address != 0xfffe){

		char firstByte = value & 0xff;
		char secondByte = ((value & (0xff << 8)) >> 8);

		memory[address] = firstByte;
		memory[address + 1] = secondByte;

	}else{

		char firstByte = value & 0xff;

		memory[address] = firstByte;

		if(firstByte == 0x10)
			cout << endl;
		else
			cout << firstByte << flush;
	}

}

void Emulator::setOperand(int addressing, int dst, short value)
{
	switch (addressing) {
		case IMMED:
			if (dst == PSW)
				psw = value;
			break;
			//else{}  Forbidden putting in immed in syntax parsing
		case REGDIR:
			regs[dst] = value;
			break;
		case MEMDIR:
			writeShort(getPom_NoChange(), value);
			break;
		case REGINDPOM:
			writeShort(getPom_NoChange() + regs[dst], value);
			break;
	}
}

void Emulator::updateAllFlagsSub(short firstOperand, short secondOperand, short value)
{
	//	Update flags
	if (firstOperand - secondOperand == 0)
		setZ(1);
	else {
		setZ(0);
		if (firstOperand - secondOperand < 0)
			setN(1);
		else
			setN(0);
	}
	if ((firstOperand == -2 && secondOperand == 32767) || (firstOperand == 32767 && secondOperand == -1)) {
		setO(1);
	}
	else {
		setO(0);
	}

	if ((firstOperand == 32766 && secondOperand == -1) || (firstOperand == -2 && secondOperand == -1) || (firstOperand == 32767 && secondOperand == -1)) {
		setC(1);
	}
	else {
		setC(0);
	}
}

void Emulator::updateZNFlags(short value)
{
	//	Update flags
	if (value == 0)
		setZ(1);
	else {
		setZ(0);
		if (value < 0)
			setN(1);
		else
			setN(0);
	}
}

void Emulator::updateZNCFlags(short firstOperand, short secondOperand, short value, bool right)
{
	//	Update flags
	if (value == 0)
		setZ(1);
	else {
		setZ(0);
		if (value < 0)
			setN(1);
		else
			setN(0);
	}
	int _firstOperand = firstOperand;
	int _secondOperand = secondOperand;
	//	Left shift
	if(!right){
		unsigned short mask = (1 << 15);
		for(int i=0; i<_secondOperand; i++){
			if (((unsigned short)(_firstOperand & mask) > 0) ) {
				setC(1);
			}
			else {
				setC(0);
			}
			_firstOperand <<= 1;
		}
	}
	//	Right shift
	else {
		unsigned short mask = 1;
		for (int i = 0; i<_secondOperand; i++) {
			if (((unsigned short)(_firstOperand & mask) > 0)) {
				setC(1);
			}
			else {
				setC(0);
			}
			_firstOperand >>= 1;
		}
	}
}

void Emulator::updateAllFlagsAdd(short firstOperand, short secondOperand, short value)
{
	//	Update flags
	if (firstOperand + secondOperand == 0)
		setZ(1);
	else {
		setZ(0);
		if (firstOperand + secondOperand < 0)
			setN(1);
		else
			setN(0);
	}

	if ((firstOperand == -1 && secondOperand == -32768) || (firstOperand == -32768 && secondOperand == -32768) || (firstOperand == 32767 && secondOperand == 32767)) {
		setO(1);
	}
	else {
		setO(0);
	}

	if ((firstOperand == -1 && secondOperand == 32767) || (firstOperand == -1 && secondOperand == 1) || (firstOperand == -1 && secondOperand == -1) || (firstOperand == -1 && secondOperand == 32767) || (firstOperand == -32768 && secondOperand == -32768)) {
		setC(1);
	}
	else {
		setC(0);
	}

}

void Emulator::setZ(unsigned short i)
{
	//	Clear bit
	psw = psw & (~1);
	//	Set bit
	psw |= i;
}

void Emulator::setO(unsigned short i)
{
	//	Clear bit
	psw = psw & (~2);
	//	Set bit
	psw |= (i<<1);
}

void Emulator::setC(unsigned short i)
{
	//	Clear bit
	psw = psw & (~4);
	//	Set bit
	psw |= (i<<2);
}

void Emulator::setN(unsigned short i)
{
	//	Clear bit
	psw = psw & (~8);
	//	Set bit
	psw |= (i<<3);
}

void Emulator::setI(unsigned short i)
{
	//	Clear bit
	psw = psw & (~(1<<15));
	//	Set bit
	psw |= (i<<15);
}

unsigned short Emulator::getZ()
{
	return psw & 1;
}

unsigned short Emulator::getO()
{
	return psw & 2;
}

unsigned short Emulator::getC()
{
	return psw & 4;
}

unsigned short Emulator::getN()
{
	return psw & 8;
}

unsigned short Emulator::getI()
{
	return psw & (1<<15);
}

bool Emulator::checkCondition(int condition)
{
	if (condition == EQ) {
		if (getZ())
			return true;
		else
			return false;
	}
	else if (condition == NE) {
		if (getN())
			return true;
		else
			return false;
	}
	else if (condition == GT) {
		if ((!getN()) && (!getZ()))
			return true;
		else
			return false;
	}
	else if (condition == AL) {
		return true;
	}
	else {
		cout << "Emulator error: unrecognized condition" << endl << flush;
		exit(1);
	}
}

bool Emulator::checkInTextSection(unsigned short address)
{
	for (map<int, Limit>::iterator it = textSectionLimits.begin(); it != textSectionLimits.end(); it++) {
		if ((address >= it->second.startAddress) && (address < (it->second.startAddress + it->second.size ) ) )
			return true;
	}
	return false;
}

void Emulator::setTextSectionLimits()
{

	int numFiles = linker->getNumFiles();
	ObjectFile** files = linker->getObjectFiles();

	for (int i = 0; i < numFiles; i++) {

		ObjectFile* file = files[i];
		map<int, SymEntry*> entries = file->getSymTable()->get_entries();

		int	offset = file->getHeader()->startAddress;

		int counter = 1;

		try {
			while (entries.at(counter)->getName() == entries.at(counter)->getSection()) {

				if (entries.at(counter)->getName() == ".text") {

					Limit l;
					l.startAddress = offset;
					l.size = files[i]->getTextSize();

					textSectionLimits.insert(pair<int, Limit>(l.startAddress, l));

				}

				int size = 0;

				if (entries.at(counter)->getName() == ".rodata") {
					size = file->getRodataSize();
				}
				else if (entries.at(counter)->getName() == ".data") {
					size = file->getDataSize();
				}
				else if (entries.at(counter)->getName() == ".bss") {
					size = file->getBssSize();
				}
				else if (entries.at(counter)->getName() == ".text") {
					size = file->getTextSize();
				}
				offset += size;
				counter++;
			}
		}
		catch (out_of_range e) {}
	}

	cout << endl << "EMULATOR TEXT SECTIONS" << endl << flush;
	for (map<int, Limit>::iterator it = textSectionLimits.begin(); it != textSectionLimits.end(); it++) {
		cout << "Text section starts at: " << it->second.startAddress << " , size: " << it->second.size << " ,end address is: " << it->second.startAddress + it->second.size << endl << flush;
	} 

}

unsigned short Emulator::getTimerBit(){
	return psw & (1 << 12);
}

