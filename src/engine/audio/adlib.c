#include "../engine.h"
#include "adlib.h"

bool adlib_present;
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

/**************************************************************************
** Activates a voice on the FM card                                      **
***************************************************************************
** Voice selects one of the 9 FM voices                                  **
** FNumber selects the note to be played                                 **
** Block selects the octave for the specified note                       **
**************************************************************************/
void ADLIB_NoteOn(unsigned char Voice, unsigned char Note, unsigned char Block) {
	ADLIB_WriteFM(0xA0 + Voice, FNr[Note] & 0xFF);
	ADLIB_WriteFM(0xB0 + Voice, (FNr[Note] >> 8) + (Block << 2) + 32);
}

/**************************************************************************
** Deactivates a voice on the FM card                                    **
***************************************************************************
** Make sure to give the same values for Note and Block or this will     **
** sound very odd.                                                       **
**************************************************************************/
void ADLIB_NoteOff(unsigned char Voice, unsigned char Note, unsigned char Block) {
	ADLIB_WriteFM(0xA0 + Voice, FNr[Note] & 0xFF);
	ADLIB_WriteFM(0xB0 + Voice, (FNr[Note] >> 8) + (Block << 2));
}

/**************************************************************************
** Sets instrument settings for a voice on the FM card                   **
**************************************************************************/
void ADLIB_SetInstrument(byte voice, Instrument instr) {
	const
			//Addresses of the operators used to form voice data
			byte OpAdr[9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};

	unsigned char Value;

	//Set up voice modulator
	Value = instr.modulator.tremolo << 8 | instr.modulator.vibrato << 7 | instr.modulator.eg_type << 6 | instr.modulator.ksr << 5 | instr.modulator.multi;
	ADLIB_WriteFM(0x20 + OpAdr[voice], Value);

	Value = instr.modulator.ksl << 7 | instr.modulator.total_level;
	ADLIB_WriteFM(0x40 + OpAdr[voice], Value);

	Value = instr.modulator.attack << 4 | instr.modulator.decay;
	ADLIB_WriteFM(0x60 + OpAdr[voice], Value);

	Value = instr.modulator.sustain << 4 | instr.modulator.release;
	ADLIB_WriteFM(0x80 + OpAdr[voice], Value);

	ADLIB_WriteFM(0xE0 + OpAdr[voice], instr.modulator.wave_shape);

	Value = instr.feedback << 1 | instr.syn_type;
	ADLIB_WriteFM(0xC0 + OpAdr[voice], Value);

	//Set up voice carrier
	Value = instr.carrier.tremolo << 8 | instr.carrier.vibrato << 7 | instr.carrier.eg_type << 6 | instr.carrier.ksr << 5 | instr.carrier.multi;
	ADLIB_WriteFM(0x23 + OpAdr[voice], Value);

	Value = instr.carrier.ksl << 7 | instr.carrier.total_level;
	ADLIB_WriteFM(0x43 + OpAdr[voice], Value);

	Value = instr.carrier.attack << 4 | instr.carrier.decay;
	ADLIB_WriteFM(0x63 + OpAdr[voice], Value);

	Value = instr.carrier.sustain << 4 | instr.carrier.release;
	ADLIB_WriteFM(0x83 + OpAdr[voice], Value);

	ADLIB_WriteFM(0xE3 + OpAdr[voice], instr.carrier.wave_shape);
}

void ADLIB_FM_Test(void) {
	//Set up voices 0 - 3 to a "harp" - note, only the values other than 0 are set
	harp.modulator.attack = 15;
	harp.modulator.decay = 5;
	harp.modulator.sustain = 8;
	harp.modulator.release = 5;
	harp.modulator.multi = 2;
	harp.modulator.total_level = 41;

	harp.carrier.attack = 15;
	harp.carrier.decay = 2;
	harp.carrier.release = 3;
	harp.carrier.multi = 1;
	harp.carrier.ksl = 2;
	harp.carrier.total_level = 3;

	ADLIB_SetInstrument(0, harp);
	ADLIB_SetInstrument(1, harp);
	ADLIB_SetInstrument(2, harp);

	ADLIB_NoteOn(0, C, 4);
	ADLIB_NoteOn(1, E, 4);
	ADLIB_NoteOn(2, G, 4);
	delay(1000);//Wait 1 second
	ADLIB_NoteOff(0, C, 4);
	ADLIB_NoteOff(1, E, 4);
	ADLIB_NoteOff(2, G, 4);
	//Wait a while
	delay(500);

	//Set up voice 3 to a "piano" - this doesn't really sound like a piano,
	//but that's what they call it!
	piano.modulator.attack = 15;
	piano.modulator.decay = 1;
	piano.modulator.sustain = 10;
	piano.modulator.release = 3;
	piano.modulator.multi = 1;
	piano.modulator.ksl = 1;
	piano.modulator.total_level = 16;

	piano.carrier.attack = 13;
	piano.carrier.decay = 2;
	piano.carrier.sustain = 8;
	piano.carrier.release = 4;
	piano.carrier.multi = 1;
	piano.carrier.total_level = 0;
	piano.carrier.ksr = 1;

	piano.feedback = 3;

	ADLIB_SetInstrument(3, piano);

	adlib.current_block = 4;
	for (adlib.current_note = 0; adlib.current_note < 12; adlib.current_note++) {
		ADLIB_NoteOn(3, adlib.current_note, adlib.current_block);
		delay(100);//Wait .1 seconds
		ADLIB_NoteOff(3, adlib.current_note, adlib.current_block);
	}

	//Wait a while
	delay(500);

	//Set up voice 4 to a "flute"
	flute.modulator.attack = 6;
	flute.modulator.decay = 14;
	flute.modulator.sustain = 7;
	flute.modulator.release = 15;
	flute.modulator.vibrato = 1;
	flute.modulator.tremolo = 1;
	flute.modulator.eg_type = 1;
	flute.modulator.ksl = 3;
	flute.modulator.total_level = 44;

	flute.carrier.attack = 6;
	flute.carrier.decay = 5;
	flute.carrier.sustain = 13;
	flute.carrier.release = 10;
	flute.carrier.vibrato = 1;
	flute.carrier.eg_type = 1;
	flute.carrier.multi = 1;
	flute.carrier.total_level = 0;

	flute.feedback = 7;
	flute.syn_type = 3;

	ADLIB_SetInstrument(4, flute);

	ADLIB_NoteOn(4, C, 5);
	ADLIB_NoteOn(0, C, 3);
	delay(250);
	ADLIB_NoteOff(4, C, 5);
	ADLIB_NoteOn(4, E, 5);
	delay(250);
	ADLIB_NoteOff(4, E, 5);
	ADLIB_NoteOff(0, C, 3);

	ADLIB_NoteOn(4, C, 5);
	ADLIB_NoteOn(0, G, 3);
	delay(250);
	ADLIB_NoteOff(4, C, 5);
	ADLIB_NoteOn(4, G, 4);
	delay(250);
	ADLIB_NoteOff(4, G, 4);
	ADLIB_NoteOff(0, G, 3);

	ADLIB_NoteOn(4, E, 4);
	ADLIB_NoteOn(0, C, 4);
	delay(250);
	ADLIB_NoteOff(4, E, 4);
	ADLIB_NoteOn(4, G, 4);
	delay(250);
	ADLIB_NoteOff(4, G, 4);
	ADLIB_NoteOff(0, C, 4);

	ADLIB_NoteOn(4, E, 4);
	ADLIB_NoteOn(0, G, 3);
	delay(250);
	ADLIB_NoteOff(4, E, 4);
	ADLIB_NoteOn(4, D, 4);
	delay(250);
	ADLIB_NoteOff(4, D, 4);
	ADLIB_NoteOff(0, G, 3);

	ADLIB_NoteOn(4, C, 4);
	ADLIB_NoteOn(0, C, 3);
	delay(750);
	ADLIB_NoteOff(4, C, 4);
	ADLIB_NoteOff(0, C, 3);
}