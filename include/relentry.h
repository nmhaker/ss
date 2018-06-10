#ifndef RELENTRY_H
#define RELENTRY_H

enum RelocationType {
	R_386_32,
	R_386_PC32
};

class RelEntry {
public:
	RelEntry(int offset, RelocationType relocationType, int value);
	~RelEntry();

	void setOffset(int offset);
	void setRelocationType(RelocationType rt);
	void setValue(int value);

	int getOffset();
	RelocationType getRelocationType();
	int getValue();

private:
	int offset;
	RelocationType relocationType;
	int value;

};

#endif RELENTRY_H