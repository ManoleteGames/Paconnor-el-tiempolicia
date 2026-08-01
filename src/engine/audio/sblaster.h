
#ifndef SBLASTER_H
#define SBLASTER_H

#include "../types/types.h"

#define ISR_VECT_71 0x71
#define ISR_VECT_72 0x72
#define ISR_VECT_73 0x73
#define ISR_VECT_11 11
#define ISR_VECT_13 13
#define ISR_VECT_15 15

#define SB_RESET 0x6
#define SB_READ_DATA 0xA
#define SB_READ_DATA_STATUS 0xE
#define SB_WRITE_DATA 0xC

#define SB_FM_PORT 0x388
#define SB_FM_DATA 0x389

extern SBlaster sblaster;
bool SB_Check(void);
void SB_Init(void);
void SB_Shutdown(void);
void SB_NoteOn(byte voice, byte note, byte block);
void SB_NoteOff(byte voice, byte note, byte block);
void SB_WriteFM(byte reg, byte value);

bool SB_ResetDSP(word Port);
word SB_GetDSPVersion(void);
byte SB_TestLoDMA(void);
void SB_Handler(SoundEffect *sound, Song *song);
void SB_PlaySound(SoundEffect *sound, int number);
void SB_LoadSong(Song *song, int number);
void SB_PlaySong(Song *song);
void SB_StopSong(Song *song);

#endif
