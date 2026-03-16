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

void AUDIO_Init(void) {
	AUDIO_Mode = 2;

	if (!AUDIO_Initialized) {
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

//This is the new timer interrupt handler and routines to install it
void AUDIO_TimerHandler(void) {
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
	if (song.loaded) song.play = true;
}


void AUDIO_StopSong(void) {
	song.play = false;
	A2M_Stop();
}

void AUDIO_UnloadSong(void) {
	if (song.loaded) MM_PopChunks(CT_MUSIC);
}