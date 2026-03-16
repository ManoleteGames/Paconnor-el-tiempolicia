#include "../engine.h"
#include "adlib.h"

Adlib adlib;

/**************************************************************************
** Writes a value to a specified index register on the FM card           **
**************************************************************************/
void ADLIB_WriteFM(byte Register, byte Value) {
	byte Counter;

	//Select register
	outportb(ADLIB_FM_PORT, Register);
	//Wait for card to accept value
	for (Counter = 1; Counter < 25; Counter++) { inportb(ADLIB_FM_PORT); }
	//Send value
	outportb(ADLIB_FM_DATA, Value);
	//Wait for card to accept value
	for (Counter = 1; Counter < 100; Counter++) { inportb(ADLIB_FM_PORT); }
}

/**************************************************************************
** Checks for the presence of an FM card                                 **
**************************************************************************/
bool ADLIB_CheckFMInstalled(void) {
	byte value_a, value_b;

	ADLIB_WriteFM(1, 0);
	ADLIB_WriteFM(4, 0x60);
	ADLIB_WriteFM(4, 0x80);
	value_a = inportb(ADLIB_FM_PORT);
	ADLIB_WriteFM(2, 0xFF);
	ADLIB_WriteFM(4, 0x21);
	value_b = inportb(ADLIB_FM_PORT);
	ADLIB_WriteFM(4, 0x60);
	ADLIB_WriteFM(4, 0x80);
	if ((value_a & 0xE0) == 0 && (value_b & 0xE0) == 0xC0) {
		adlib.opl = 2;
		if ((inportb(ADLIB_FM_PORT) & 0x06) == 0) { adlib.opl = 3; }
		return true;
	} else {
		return false;
	}
}
