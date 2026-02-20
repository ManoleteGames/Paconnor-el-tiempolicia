#include "../engine.h"
#include "dos.h"
#include "pc.h"
#include "audio.h"
#include "speaker.h"
#include "sblaster.h"
#include <stdio.h>

bool AUDIO_Initialized;
int AUDIO_Mode;//0: speaker, 1: Adlib; 2: Sound blaster
SoundEffect sound_effect;
Song song;

MIDI_Song midi_song;

Instrument flute;
Instrument piano;
Instrument harp;
Instrument drum1;
Instrument drum2;
Instrument drum3;

const int RealVoice[15] = {0, 1, 2, 3, 4, 5, 9, 10,
						   11, 12, 13, 14, 15, 16, 17};
//Addresses of the operators used to form voice data
const int OpAdr[9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};
//Variable for amplifying AM, VIB and percussion mode.
char ByteBD;


//This stores the FM instrument definitions
byte M2[128], M4[128], M6[128], M8[128], ME[128], MC[128], C2[128], C4[128], C6[128], C8[128], CE[128];

//List of FNumbers
const word FNr[12] = {
		/*C*/ 343,
		/*C#/Db*/ 363,
		/*D*/ 385,
		/*D#/Eb*/ 408,
		/*E*/ 432,
		/*F*/ 458,
		/*F#/Gb*/ 485,
		/*G*/ 514,
		/*G#/Ab*/ 544,
		/*A*/ 577,
		/*A#/Bb*/ 611,
		/*B*/ 647};

void AUDIO_Init(void) {
	AUDIO_Mode = 2;

	if (!AUDIO_Initialized) {
		printf(" Initializing Audio... \n");
		switch (AUDIO_Mode) {
			case 0:// PC Speaker;
				break;
			case 1:// Adlib
				   //ADLIB_Init();
				break;
			case 2:// Sound blaster
				SB_Init();
				break;
		}

		song.current_pattern = 0;
		song.current_line = 0;
		song.pattern_break = false;
		song.pattern_delay = false;
		song.tickXF = 0;
		song.ticks = 0;
		song.next_line = 0;
		song.irq_mode = true;
		song.loop = true;

		song.ticklooper = 0;
		song.macro_ticklooper = 0;
		song.speed = 10;
		song.macro_speedup = 10;

		song.loaded = false;
		song.play = false;
		song.pause = false;
	}
	AUDIO_Initialized = true;
}

void AUDIO_Shutdown(void) {
	if (AUDIO_Initialized) {
		switch (AUDIO_Mode) {
			case 0:// PC Speaker;
				break;
			case 1:// Adlib
				   //ADLIB_Init();
				break;
			case 2:// Sound blaster
				SB_Shutdown();
				break;
		}
	}
	AUDIO_Initialized = false;
}

void AUDIO_PlaySound(byte sound) {
	byte aux;
	switch (sound) {
		case AUDIO_GUN_EFFECT:
			switch (AUDIO_Mode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = speaker_note[56];
					sound_effect.buffer[1] = 1;
					sound_effect.buffer[2] = speaker_note[52];
					sound_effect.buffer[3] = 1;
					sound_effect.buffer[4] = speaker_note[51];
					sound_effect.buffer[5] = 1;
					sound_effect.buffer[6] = speaker_note[45];
					sound_effect.buffer[7] = 1;
					sound_effect.buffer[8] = 99;
					sound_effect.buffer[9] = 99;
					sound_effect.buffer[10] = 0;
					sound_effect.buffer[11] = 0;
					sound_effect.buffer[12] = 0;

					sound_effect.note_time = 0;
					sound_effect.current_note = 0;
					sound_effect.playing = true;

					SPK_Unmute();
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}
			break;
		case AUDIO_EXPLOSSION:
			switch (AUDIO_Mode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = speaker_note[10];
					sound_effect.buffer[1] = 2;
					sound_effect.buffer[2] = speaker_note[12];
					sound_effect.buffer[3] = 4;
					sound_effect.buffer[4] = speaker_note[18];
					sound_effect.buffer[5] = 2;
					sound_effect.buffer[6] = speaker_note[8];
					sound_effect.buffer[7] = 3;
					sound_effect.buffer[8] = 99;
					sound_effect.buffer[9] = 99;
					sound_effect.buffer[10] = 0;
					sound_effect.buffer[11] = 0;
					sound_effect.buffer[12] = 0;

					sound_effect.note_time = 0;
					sound_effect.current_note = 0;
					sound_effect.playing = true;
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}

			break;
		default:
			break;
	}
}

bool AUDIO_CheckAdlib(void) {
	return ADLIB_CheckFMInstalled();
}

bool AUDIO_CheckSoundBlaster(void) {
	return SB_Check();
}

void AUDIO_PlayIntro(void) {
	SB_NoteOn(4, C, 5);
	SB_NoteOn(0, C, 3);
	delay(250);
	SB_NoteOff(4, C, 5);
	SB_NoteOn(4, E, 5);
	delay(250);
	SB_NoteOff(4, E, 5);
	SB_NoteOff(0, C, 3);

	SB_NoteOn(4, C, 5);
	SB_NoteOn(0, G, 3);
	delay(250);
	SB_NoteOff(4, C, 5);
	SB_NoteOn(4, G, 4);
	delay(250);
	SB_NoteOff(4, G, 4);
	SB_NoteOff(0, G, 3);

	SB_NoteOn(4, E, 4);
	SB_NoteOn(0, C, 4);
	delay(250);
	SB_NoteOff(4, E, 4);
	SB_NoteOn(4, G, 4);
	delay(250);
	SB_NoteOff(4, G, 4);
	SB_NoteOff(0, C, 4);

	SB_NoteOn(4, E, 4);
	SB_NoteOn(0, G, 3);
	delay(250);
	SB_NoteOff(4, E, 4);
	SB_NoteOn(4, D, 4);
	delay(250);
	SB_NoteOff(4, D, 4);
	SB_NoteOff(0, G, 3);

	SB_NoteOn(4, C, 4);
	SB_NoteOn(0, C, 3);
	delay(750);
	SB_NoteOff(4, C, 4);
	SB_NoteOff(0, C, 3);
}

//This loads the MIDI file into memory
void AUDIO_LoadMIDI(const char *dat_name, const char *instruments_name, const char *song_name) {

	// Load instruments info
	//FILE_LoadInstruments(dat_name, instruments_name, &M2[0], &M4[0], &M6[0], &M8[0], &ME[0], &MC[0], &C2[0], &C4[0], &C6[0], &C8[0], &CE[0]);
	FILE_LoadMidiSong(dat_name, song_name);
	midi_song.voices = 6;
}

//Writes a value to a specified index register on the FM card
void WriteFM(byte reg, byte value) {
	word counter;

	//Select register
	outportb(sblaster.fm_port, reg);
	//Wait for card to accept value
	for (counter = 0; counter < 25; counter++) inportb(sblaster.fm_port);
	//Send value
	outportb(sblaster.fm_port + 1, value);
	//Wait for card to accept value
	for (counter = 0; counter < 100; counter++) inportb(sblaster.fm_port);
}

//Disables a note on the FM synthesizer
void DisableNote(byte voice) {
	//Write data to FM synthesizer
	WriteFM(0xA0 + voice, midi_song.note_off_data[voice][0]);
	WriteFM(0xB0 + voice, midi_song.note_off_data[voice][1]);
}

//This function reads a byte from a specific track
byte ReadByte(byte track_number) {
	if (midi_song.waiting_for[track_number] < 0xFFFFFF)
		return midi_song.track_data[track_number][midi_song.next_byte_ptr[track_number]++];
	else
		return 0;
}

//This function reads a Variable Length Encoded (VLE) number from the track
long GetVLE(byte track_number) {
	byte ByteRead;
	long Result;

	//Assume zero
	Result = 0;
	do {
		//Read first byte
		ByteRead = ReadByte(track_number);
		//Store 7bit part
		Result = (Result << 7) + (ByteRead & 0x7F);
	} while ((ByteRead & 0x80) != 0);

	return Result;
}

//This void stores the time for the next event
void GetDeltaTime(byte track_number) {
	midi_song.waiting_for[track_number] += GetVLE(track_number);
}

//Guess!!
void StopMIDI() {
	unsigned char CurrentChannel;
	for (CurrentChannel = 0; CurrentChannel <= 17; CurrentChannel++)
		if (midi_song.in_use[CurrentChannel] != 0) DisableNote(CurrentChannel);

	midi_song.play = false;
}


//This resets the drums
void EnableDrums() {
	//Enable waveform select
	WriteFM(1, 0x20);
	//Enable percussion mode, amplify AM & VIB
	ByteBD = (char) (1 << 7) | (0 << 6) | 0x20;
	WriteFM(0xBD, ByteBD);
	//Set drum frequencies
	WriteFM(0xA0, 400 & 0xFF);
	WriteFM(0xB0, (400 >> 8) + (2 << 2));
	WriteFM(0xA1, 400 & 0xFF);
	WriteFM(0xB1, (400 >> 8) + (2 << 2));
	WriteFM(0xA2, 400 & 0xFF);
	WriteFM(0xB2, (400 >> 8) + (2 << 2));
	WriteFM(0xA3, 400 & 0xFF);
	WriteFM(0xB3, (400 >> 8) + (2 << 2));
	WriteFM(0xA4, 400 & 0xFF);
	WriteFM(0xB4, (400 >> 8) + (2 << 2));
	WriteFM(0xA5, 400 & 0xFF);
	WriteFM(0xB5, (400 >> 8) + (2 << 2));
	WriteFM(0xA6, 400 & 0xFF);
	WriteFM(0xB6, (400 >> 8) + (2 << 2));
	WriteFM(0xA7, 500 & 0xFF);
	WriteFM(0xB7, (500 >> 8) + (2 << 2));
	WriteFM(0xA8, 650 & 0xFF);
	WriteFM(0xB8, (650 >> 8) + (2 << 2));
}
//This procedure compensates a given volume for the master volume
byte DoVolume(byte before) {
	long after;

	after = before;
	after *= midi_song.volume;
	after >>= 7;
	if (after > 127) after = 127;
	return (byte) after;
}

//Set the playback volume
void SetVol(byte new_volume) {
	midi_song.volume = new_volume;
}


//This procedure changes the speed of the timer to adjust the tempo of a song
//NewSpeed gives the amount of microseconds per quarter note
void ChangeSpeed(long new_speed) {


	//long divisor;

	//divisor = (midi_song.tempo << 20) / new_speed;


	//Set the appropriate values for the timer interrupt
	//midi_song.tick_counter = 0x1234DD / divisor;
	midi_song.tick_counter = 20;
	/*outportb(0x43, 0x34);
	outportb(0x40, midi_song.tick_counter & 0xFF);
	outportb(0x40, (midi_song.tick_counter >> 8) & 0xFF);*/
}

//Disables a drum note, well, it actually does nothing since drum notes
//do not need to be disabled
void DrumOff(byte MIDIChannel, byte Number, byte Velocity) {
	MIDIChannel++;
	Number++;
	Velocity++;
}

//Sets up a drum channel, in much the same way as a normal voice
void SetDrum(byte Operator, byte O2, byte O4, byte O6, byte O8, byte OE, byte OC) {
	WriteFM(0x20 + Operator, O2);
	WriteFM(0x40 + Operator, O4);
	WriteFM(0x60 + Operator, O6);
	WriteFM(0x80 + Operator, O8);
	WriteFM(0xE0 + Operator, OE);
	WriteFM(0xC0 + Operator, OC);
}

//Plays a drum note
void DrumOn(byte MIDIChannel, byte Number, byte Velocity) {
	//If velocity is 0, note is turned off, this is ignored
	if (Velocity == 0) return;
	//Convert velocity to "level" needed by SB and reduce the volume slightly
	Velocity = (int) (Velocity << 3) / 10;
	Velocity = 63 - (Velocity >> 1);
	//Bass drum
	//if (Number == 35 || Number == 36 || Number == 41 || Number == 43) {
	//if (Number == 0 || Number == 1 || Number == 41 || Number == 43) {
	//Set channel 6 to bass, allowing for volume
	//SetDrum(16, 0, 13, 248, 102, 0, 48);


	switch (MIDIChannel) {
		case 6:
			SetDrum(0x10, 0, 0, 220, 102, 0, 48);
			SetDrum(0x13, 0, Velocity, 246, 87, 0, 16);
			//Enable bass and immediately deactivate
			WriteFM(0xBD, ByteBD | 16);
			WriteFM(0xBD, ByteBD);
			break;
		case 7:
			SetDrum(0x11, 0, 0, 248, 62, 0, 48);
			SetDrum(0x14, 0, Velocity, 246, 87, 0, 16);
			//Enable bass and immediately deactivate
			WriteFM(0xBD, ByteBD | 16);
			WriteFM(0xBD, ByteBD);
			break;
		case 8:
			SetDrum(0x12, 0, 0, 220, 200, 0, 16);
			SetDrum(0x15, 0, Velocity, 246, 20, 0, 8);
			//Enable bass and immediately deactivate
			WriteFM(0xBD, ByteBD | 16);
			WriteFM(0xBD, ByteBD);
			break;
		default:
			break;
	}


	//}
	/*
	//HiHat
	if (Number == 37 || Number == 39 || Number == 42 || Number == 44 ||
		Number == 46 || Number == 56 || Number == 62 || Number == 69 ||
		Number == 70 || Number == 71 || Number == 72 || Number == 78) {
		//Set channel 7 to hihat, allowing for volume
		SetDrum(17, 0, Velocity, 240, 6, 0, 16);
		//Enable hihat and immediately deactivate
		WriteFM(0xBD, ByteBD | 1);
		WriteFM(0xBD, ByteBD);
	}
	//Snare drum
	if (Number == 38 || Number == 40) {
		//Set channel 7 to snare drum, allowing for volume
		SetDrum(20, 0, Velocity, 240, 7, 2, 16);
		//Enable hihat and immediately deactivate
		WriteFM(0xBD, ByteBD | 8);
		WriteFM(0xBD, ByteBD);
	}
	//TomTom
	if (Number == 45 || Number == 47 || Number == 48 || Number == 50 ||
		Number == 60 || Number == 61 || Number == 63 || Number == 64 ||
		Number == 65 || Number == 66 || Number == 67 || Number == 68 ||
		Number == 73 || Number == 74 || Number == 75 || Number == 76 ||
		Number == 77) {
		//Set channel 8 to tomtom, allowing for volume
		SetDrum(18, 2, Velocity, 240, 6, 0, 16);
		//Enable tomtom and immediately deactivate
		WriteFM(0xBD, ByteBD | 4);
		WriteFM(0xBD, ByteBD);
	}
	//Cymbal
	if (Number == 49 || Number == 51 || Number == 52 || Number == 53 ||
		Number == 54 || Number == 55 || Number == 57 || Number == 58 ||
		Number == 59 || Number == 79 || Number == 80 || Number == 81) {
		//Set channel 8 to cymbal, allowing for volume
		SetDrum(21, 4, Velocity, 240, 6, 0, 16);
		//Enable cymbal and immediately deactivate
		WriteFM(0xBD, ByteBD | 2);
		WriteFM(0xBD, ByteBD);
	}
	*/
	//Done to avoid "Parameter 'MIDIChannel' never used
	MIDIChannel++;
}


//Processes a "NoteOff" event for the FM synthesizer
void NoteOff(byte MIDIChannel, byte Number, byte Velocity) {
	unsigned char FoundChannel, FMChannel;

	//Assume the note can't be found
	FoundChannel = 255;
	//Scan for note on FM channels
	for (FMChannel = 0; FMChannel <= midi_song.voices; FMChannel++) {
		if (midi_song.in_use[FMChannel] != 0) {
			//Is this the correct channel?
			if (midi_song.channel[FMChannel] == MIDIChannel && midi_song.NoteNumber[FMChannel] == Number) {
				//If the correct channel has been found then report that
				FoundChannel = FMChannel;
				break;
			}
		}
	}
	if (FoundChannel != 255) {
		//Disable the note
		DisableNote(RealVoice[FoundChannel]);
		//Store appropriate information
		midi_song.in_use[FoundChannel] = 0;//InUse flag
	}
	//Done to prevent "Parameter 'Velocity' never used
	Velocity++;
}

//Cuts a note on the FM synthesizer immediately
void CutNote(byte voice) {
	//Set decay rate to fast - to avoid "plink" sound
	WriteFM(0x80 + OpAdr[voice % 9], 0xF);
	WriteFM(0x83 + OpAdr[voice % 9], 0xF);
	//Disable the note
	DisableNote(voice);
}

//Sets a channel on the FM synthesizer to a specific instrument
/*void SetInstr(byte voice, byte instrument, byte Volume) {
	word value;
	//Correction for volume
	value = 63 - (M4[instrument] & 63);
	value = value * Volume / 127;
	if (value > 63) value = 0;
	else
		value = 63 - value;
	value = (M4[instrument] & 0xC0) | value;
	//Set up voice modulator
	WriteFM(0x20 + OpAdr[voice], M2[instrument]);
	WriteFM(0x40 + OpAdr[voice], value);
	WriteFM(0x60 + OpAdr[voice], M6[instrument]);
	WriteFM(0x80 + OpAdr[voice], M8[instrument]);
	WriteFM(0xE0 + OpAdr[voice], ME[instrument]);
	//The "or 3 shl 4" is enables the voice on the OPL3
	WriteFM(0xC0 + OpAdr[voice], MC[instrument] + (3 << 4));

	//Correction for volume
	value = 63 - (C4[instrument] & 63);
	value = value * Volume / 127;
	if (value > 63) value = 0;
	else
		value = 63 - value;
	value = (C4[instrument] & 0xC0) | value;
	//Set up voice carrier
	WriteFM(0x23 + OpAdr[voice], C2[instrument]);
	WriteFM(0x43 + OpAdr[voice], value);
	WriteFM(0x63 + OpAdr[voice], C6[instrument]);
	WriteFM(0x83 + OpAdr[voice], C8[instrument]);
	WriteFM(0xE3 + OpAdr[voice], CE[instrument]);
}*/


//Enables a note on the FM synthesizer
void EnableNote(byte voice, byte number) {
	byte note, block;

	//Calculate appropriate data for FM synthesizer
	note = number;
	block = number >> 4;
	//Store data to disable the note when necessary
	midi_song.note_off_data[voice][0] = (FNr[note] & 0xFF);
	midi_song.note_off_data[voice][1] = (FNr[note] >> 8) + (block << 2);
	//Write data to FM synthesizer
	WriteFM(0xA0 + voice, (FNr[note] & 0xFF));
	WriteFM(0xB0 + voice, (FNr[note] >> 8) + (block << 2) + 32);
}

//Processes a "NoteOn" event for the FM synthesizer
void NoteOn(byte channel, byte number, byte velocity) {
	byte free_channel, FM_channel;
	long Oldest;

	//Velocity of zero means note off
	if (velocity == 0) {
		NoteOff(channel, number, velocity);
		return;
	}

	//Assume no free channel
	free_channel = 255;
	//Scan for free channel
	for (FM_channel = 0; FM_channel <= midi_song.voices; FM_channel++) {
		if (midi_song.in_use[FM_channel] == 0) {
			//If a free channel has been found then report that
			free_channel = FM_channel;
			break;
		}
	}
	//If there was no free channel, the SB's 6/15 voice polyphony
	//has been exceeded and the "oldest" note must be deactivated
	if (free_channel == 255) {
		Oldest = 0x7FFFFFFF;
		//Scan for the oldest note
		for (FM_channel = 0; FM_channel <= midi_song.voices; FM_channel++) {
			if (midi_song.activated[FM_channel] < Oldest) {
				free_channel = FM_channel;
				Oldest = midi_song.activated[FM_channel];
			}
		}
		//Disable the note currently playing
		CutNote(RealVoice[free_channel]);
	}
	//Change the instrument settings for the FM channel chosen
	//SetInstr(RealVoice[free_channel], midi_song.instrument[channel], velocity);
	//Start playing the note
	EnableNote(RealVoice[free_channel], number);
	//Store appropriate information
	midi_song.in_use[free_channel] = 1;                   //InUse flag
	midi_song.activated[free_channel] = midi_song.counter;//Activation time
	midi_song.channel[free_channel] = channel;            //Link FM channel to MIDI channel
	midi_song.NoteNumber[free_channel] = number;          //Note number (which note is being played)
	midi_song.NoteVelocity[free_channel] = velocity;      //Velocity (=volume)
}


//This void handles the MIDI events
void DoEvent(byte track_number) {
	unsigned char MIDIByte, MIDICommand, MIDIChannel, MetaEvent;
	long DataLength;
	unsigned char n1, n2;
	long Counter;
	long Data;

	//Get the MIDI event command from the track
	MIDIByte = ReadByte(track_number);

	//If this is not a command, we are in "running" mode and the last
	//command issued on the track is assumed
	if ((MIDIByte & 0x80) == 0) {
		MIDIByte = midi_song.last_command[track_number];
		midi_song.next_byte_ptr[track_number]--;
	}
	//Store the command for running mode
	midi_song.last_command[track_number] = MIDIByte;

	/*
    META-EVENTS
    ===========
    Special commands controlling timing etc.
  */
	if (MIDIByte == 0xFF) {
		MetaEvent = ReadByte(track_number);
		DataLength = GetVLE(track_number);
		if (MetaEvent == 0x2F)//End of track}
			midi_song.waiting_for[track_number] = 0xFFFFFF;
		if (MetaEvent == 0x51) {//Tempo change
			Data = ReadByte(track_number);
			Data = (Data << 8) + ReadByte(track_number);
			Data = (Data << 8) + ReadByte(track_number);
			ChangeSpeed(Data);
		}
		if ((MetaEvent != 0x2F) && (MetaEvent != 0x51)) {
			//Others (text events, track sequence numbers etc. - ignore
			for (Counter = 1; Counter <= DataLength; Counter++) ReadByte(track_number);
		}
	}

	/*
    CHANNEL COMMANDS
    ================
    Upper nibble contains command, lower contains channel
  */
	MIDIChannel = MIDIByte & 0xF;
	MIDICommand = MIDIByte >> 4;
	if (MIDICommand == 8) {//Note off
		n1 = ReadByte(track_number);
		n2 = DoVolume(ReadByte(track_number));
		//if (MIDIChannel == 9 || MIDIChannel == 15) {
		/*if ((MIDIChannel >= 0) && (MIDIChannel <= 3)) {
			DrumOff(MIDIChannel, n1, n2);
		} else
			NoteOff(MIDIChannel, n1, n2);*/
		switch (MIDIChannel) {
			case 6:
				DrumOff(MIDIChannel, n1, n2);
				break;
			case 7:
				DrumOff(MIDIChannel, n1, n2);
				break;
			case 8:
				DrumOff(MIDIChannel, n1, n2);
				break;
			default:
				NoteOff(MIDIChannel, n1, n2);
				break;
		}
	}
	if (MIDICommand == 9) {//Note on
		n1 = ReadByte(track_number);
		n2 = DoVolume(ReadByte(track_number));
		//if (MIDIChannel == 9 || MIDIChannel == 15)
		/*if ((MIDIChannel >= 0) && (MIDIChannel <= 3)) {
			DrumOn(MIDIChannel, n1, n2);
		} else
			NoteOn(MIDIChannel, n1, n2);*/
		switch (MIDIChannel) {
			case 6:
				DrumOn(MIDIChannel, n1, n2);
				break;
			case 7:
				DrumOn(MIDIChannel, n1, n2);
				break;
			case 8:
				DrumOn(MIDIChannel, n1, n2);
				break;
			default:
				NoteOn(MIDIChannel, n1, n2);
				break;
		}
	}
	if (MIDICommand == 0xA) {//Key Aftertouch - only supported for GM device
		n1 = ReadByte(track_number);
		n2 = DoVolume(ReadByte(track_number));
	}
	if (MIDICommand == 0xB) {//Control change - only supported for GM device
		n1 = ReadByte(track_number);
		n2 = ReadByte(track_number);
	}
	if (MIDICommand == 0xC) {//Patch change - this changes the instrument on a channel
		n1 = ReadByte(track_number);
		midi_song.instrument[MIDIChannel] = n1;
	}
	if (MIDICommand == 0xD) {//Channel aftertouch - only supported on GM device
		n1 = ReadByte(track_number);
	}
	if (MIDICommand == 0xE) {//Pitch wheel change - only supported on GM device
		n1 = ReadByte(track_number);
		n2 = ReadByte(track_number);
	}

	/*
    SYSTEM COMMANDS
    ===============
    These are ignored.
  */
	if (MIDICommand == 0xF) {
		if (MIDIChannel == 0) do
				;
			while (ReadByte(track_number) != 0x7F);//System Exclusive
		if (MIDIChannel == 2) {
			ReadByte(track_number);
			ReadByte(track_number);
		}//Song Position Pointer
		if (MIDIChannel == 3) { ReadByte(track_number); }// Song Select
	}
}


//This is the new timer interrupt handler and routines to install it
void AUDIO_TimerHandler(void) {
	unsigned char CurrentTrack;

	switch (AUDIO_Mode) {
		case 0:// PC Speaker;
			int note;

			if (sound_effect.playing) {
				note = sound_effect.buffer[sound_effect.current_note];
				sound_effect.note_duration = sound_effect.buffer[sound_effect.current_note + 1];

				SPK_Sound(note);

				sound_effect.note_time++;
				if (sound_effect.note_time > sound_effect.note_duration) {
					sound_effect.current_note += 2;
					sound_effect.note_time = 0;
				}
				if (sound_effect.buffer[sound_effect.current_note] == 99) {
					sound_effect.playing = false;
					sound_effect.current_note = 0;

					SPK_Mute();
				}
			} else {
				SPK_Mute();
			}
			break;
		case 1:// Adlib
			A2M_TimerHandler();
			break;
		case 2:// Sound blaster
			A2M_TimerHandler();
			break;
	}
}


//This starts playing the MIDI file}
void AUDIO_PlayMIDI(void) {
	int i;

	//MIDI might already be playing, so stop it first
	StopMIDI();

	//Clear read pointers for every track
	for (i = 1; i <= midi_song.tracks_number; i++) {
		midi_song.next_byte_ptr[i] = 0;
		midi_song.waiting_for[i] = 0;
		midi_song.last_command[i] = 0xFF;
		GetDeltaTime(i);
	}
	midi_song.counter = 0;
	midi_song.tick_counter = 0;
	EnableDrums();
	SetVol(128);
	midi_song.play = true;
}

//
void AUDIO_LoadSong(const char *dat_name, const char *asset_name) {
	A2M_LoadFile(dat_name, asset_name);
	song.loaded = true;
	song.current_line = 0;
	song.current_order = 0;
	song.speed = songinfo.speed;
	song.tempo = songinfo.tempo;
}

void AUDIO_PlaySong(void) {
	song.play = true;
}

void AUDIO_StopSong(void) {
	song.play = false;
}