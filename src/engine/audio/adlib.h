
#ifndef ADLIB_H
#define ADLIB_H

#include "../types/types.h"

#define ADLIB_FM_PORT 0x388
#define ADLIB_FM_DATA 0x389

void ADLIB_Init(void);
void ADLIB_Shutdown(void);
bool ADLIB_CheckFMInstalled(void);
void ADLIB_Handler(SoundEffect *sound, Song *song);
void ADLIB_PlaySound(SoundEffect *sound, int number);
void ADLIB_LoadSong(Song *song, int number);
void ADLIB_PlaySong(Song *song);
void ADLIB_StopSong(Song *song);

#endif