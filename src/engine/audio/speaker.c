#include "../engine.h"
#include "speaker.h"


byte speakerMenu[4] = {56, 52, 51, 45};

byte speakerSelect[16] = {0, 0, 90, 80, 70, 60, 50, 30, 10, 5, 10, 30, 50, 60, 70, 60};
byte speakerCrash[16] = {0, 0, 69, 3, 120, 32, 39, 200, 20, 60, 16, 106, 12, 87, 8, 70};
byte speakerJump[16] = {0, 0, 30, 35, 40, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};
byte speakerGetItem[16] = {0, 0, 69, 70, 71, 72, 69, 64, 58, 40, 30, 12, 12, 16, 20, 70};
byte speakerOctave0[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 0, 0, 0, 0};
byte speakerOctave1[16] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 0, 0, 0, 0};
byte speakerOctave2[16] = {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0};
byte speakerOctave3[16] = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 0, 0, 0, 0};
byte speakerOctave4[16] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 0, 0, 0, 0};
byte speakerOctave5[16] = {60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 0, 0, 0, 0};
byte speakerOctave6[16] = {72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 0, 0, 0, 0};

/*
	NOTE VALUES
	-----------
	Octave 0    1    2    3    4    5    6    7
	Note
	 C     0    12   24   36   48   60   72   84
	 C#    1    13   25   37   49   61   73   85
	 D     2    14   26   38   50   62   74   86
	 D#    3    15   27   39   51   63   75   87
	 E     4    16   28   40   52   64   76   88
	 F     5    17   29   41   53   65   77   89
	 F#    6    18   30   42   54   66   78   90
	 G     7    19   31   43   55   67   79   91
	 G#    8    20   32   44   56   68   80   92
	 A     9    21   33   45   57   69   81   93
	 A#    10   22   34   46   58   70   82   94
	 B     11   23   35   47   59   71   83   95
*/
//1193180/Value
int speaker_note[96] = {
		// mute
		//value = 1.9MHz/f
		//Note	    C    C#     D    D#     E     F    F#     G    G#     A    A#     B
		//Octave
		/*0 */ 65535,
		65535,
		65535,
		62799,
		56818,
		54235,
		51877,
		49716,
		45892,
		44192,
		41144,
		38490,
		/*1   */ 36157,
		34091,
		32248,
		30594,
		29102,
		27118,
		25939,
		24351,
		22946,
		21694,
		20572,
		19245,
		/*2   */ 18357,
		17292,
		16345,
		15297,
		14551,
		13715,
		12969,
		12175,
		11473,
		10847,
		10286,
		9701,
		/*3   */ 9108,
		8584,
		8117,
		7698,
		7231,
		6818,
		6450,
		6088,
		5736,
		5424,
		5121,
		4870,
		/*4   */ 4554,
		4308,
		4058,
		3837,
		3616,
		3419,
		3225,
		3044,
		2875,
		2712,
		2560,
		2415,
		/*5	*/ 2281,
		2154,
		2033,
		1918,
		1811,
		1709,
		1612,
		1522,
		1436,
		1356,
		1280,
		1208,
		/*6	*/ 1141,
		1076,
		1015,
		959,
		898,
		854,
		806,
		761,
		718,
		678,
		640,
		604,
		/*7	*/ 570,
		538,
		508,
		479,
		452,
		427,
		403,
		380,
		359,
		339,
		320,
		302,
};

void SPK_Unmute(void) {
	byte data;
	// Unmute speaker
	// When audio is off, all functions are linked to speaker but buzzer will be not activated
	data = inportb(0x61);
	outportb(0x61, data | 3);
}

void SPK_Mute(void) {
	byte data;
	// Unmute speaker
	// When audio is off, all functions are linked to speaker but buzzer will be not activated
	data = inportb(0x61);
	outportb(0x61, data & 0xFC);
}

void SPK_Sound(int freq) {
	outportb(PTI_CH2, (byte) freq & 0xFF);
	outportb(PTI_CH2, (byte) (freq >> 8) & 0xFF);
}

void SPK_NoSound(void) {
	outportb(PTI_CH2, 0);
	outportb(PTI_CH2, 0);
}

void SPK_Test(void) {
	int i;
	SPK_Unmute();
	for (i = 0; i < 96; i++) {
		SPK_Sound(speaker_note[i]);
		delay(100);
	}
	for (i = 95; i > 0; i--) {
		SPK_Sound(speaker_note[i]);
		delay(100);
	}
	SPK_NoSound();
	SPK_Mute();
}
