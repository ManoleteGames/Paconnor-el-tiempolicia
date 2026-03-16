#include "../engine.h"
#include "dos.h"
#include "pc.h"
#include "audio.h"
#include "speaker.h"
#include "sblaster.h"
#include <stdio.h>

SoundEffect sound_effect;
Song song;

void AUDIO_Init(void) {

	engine.AUDIO_SelectedMode = 99;

	//
	if (engine.SB_Present) engine.AUDIO_SelectedMode = 2;
	else if (engine.ADLIB_present)
		engine.AUDIO_SelectedMode = 1;
	else
		engine.AUDIO_SelectedMode = 0;

	if (!engine.AUDIO_Initialized) {
		switch (engine.AUDIO_SelectedMode) {
			case 0:// PC Speaker;
				SPK_Init();
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
	engine.AUDIO_Initialized = true;
}

void AUDIO_Shutdown(void) {
	if (engine.AUDIO_Initialized) {
		switch (engine.AUDIO_SelectedMode) {
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
	engine.AUDIO_Initialized = false;
}

void AUDIO_PlaySound(byte sound) {
	byte aux;
	switch (sound) {
		case AUDIO_PUNCH_EFFECT:
			switch (engine.AUDIO_SelectedMode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = 400;
					sound_effect.buffer[1] = 3;
					sound_effect.buffer[2] = 200;
					sound_effect.buffer[3] = 3;
					sound_effect.buffer[4] = 150;
					sound_effect.buffer[5] = 5;
					sound_effect.buffer[6] = 99;
					sound_effect.buffer[7] = 99;

					sound_effect.note_time = 0;
					sound_effect.note_index = 0;
					sound_effect.playing_note = false;
					sound_effect.play = true;
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}
			break;
		case AUDIO_KICK_EFFECT:
			switch (engine.AUDIO_SelectedMode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = 400;
					sound_effect.buffer[1] = 3;
					sound_effect.buffer[2] = 300;
					sound_effect.buffer[3] = 3;
					sound_effect.buffer[4] = 150;
					sound_effect.buffer[5] = 5;
					sound_effect.buffer[6] = 99;
					sound_effect.buffer[7] = 99;

					sound_effect.note_time = 0;
					sound_effect.note_index = 0;
					sound_effect.playing_note = false;
					sound_effect.play = true;
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}
			break;
		case AUDIO_GUN_EFFECT:
			switch (engine.AUDIO_SelectedMode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = 1000;
					sound_effect.buffer[1] = 2;
					sound_effect.buffer[2] = 800;
					sound_effect.buffer[3] = 2;
					sound_effect.buffer[4] = 600;
					sound_effect.buffer[5] = 2;
					sound_effect.buffer[6] = 400;
					sound_effect.buffer[7] = 2;
					sound_effect.buffer[8] = 300;
					sound_effect.buffer[9] = 2;
					sound_effect.buffer[10] = 200;
					sound_effect.buffer[11] = 2;
					sound_effect.buffer[12] = 99;

					sound_effect.note_time = 0;
					sound_effect.note_index = 0;
					sound_effect.playing_note = false;
					sound_effect.play = true;
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}
			break;
		case AUDIO_EXPLOSSION:
			switch (engine.AUDIO_SelectedMode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = 200;
					sound_effect.buffer[1] = 3;
					sound_effect.buffer[2] = 190;
					sound_effect.buffer[3] = 3;
					sound_effect.buffer[4] = 180;
					sound_effect.buffer[5] = 3;
					sound_effect.buffer[6] = 170;
					sound_effect.buffer[7] = 3;
					sound_effect.buffer[8] = 160;
					sound_effect.buffer[9] = 3;
					sound_effect.buffer[10] = 150;
					sound_effect.buffer[11] = 3;
					sound_effect.buffer[12] = 140;
					sound_effect.buffer[13] = 3;
					sound_effect.buffer[14] = 130;
					sound_effect.buffer[15] = 3;
					sound_effect.buffer[16] = 120;
					sound_effect.buffer[17] = 3;
					sound_effect.buffer[18] = 110;
					sound_effect.buffer[19] = 3;
					sound_effect.buffer[20] = 100;
					sound_effect.buffer[21] = 3;
					sound_effect.buffer[22] = 90;
					sound_effect.buffer[23] = 3;
					sound_effect.buffer[24] = 80;
					sound_effect.buffer[25] = 3;
					sound_effect.buffer[26] = 70;
					sound_effect.buffer[27] = 3;
					sound_effect.buffer[28] = 60;
					sound_effect.buffer[29] = 3;
					sound_effect.buffer[30] = 50;
					sound_effect.buffer[31] = 3;
					sound_effect.buffer[32] = 40;
					sound_effect.buffer[33] = 3;
					sound_effect.buffer[34] = 99;
					sound_effect.buffer[35] = 99;

					sound_effect.note_time = 0;
					sound_effect.note_index = 0;
					sound_effect.playing_note = false;
					sound_effect.play = true;
					break;
				case 1:// Adlib

					break;
				case 2:// Sound blaster

					break;
			}
			break;
		case AUDIO_ACTOR_DEAD:
			switch (engine.AUDIO_SelectedMode) {
				case 0:// PC Speaker;
					sound_effect.buffer[0] = 440;
					sound_effect.buffer[1] = 3;
					sound_effect.buffer[2] = 0;
					sound_effect.buffer[3] = 1;
					sound_effect.buffer[4] = 420;
					sound_effect.buffer[5] = 2;
					sound_effect.buffer[6] = 1;
					sound_effect.buffer[7] = 1;
					sound_effect.buffer[8] = 400;
					sound_effect.buffer[9] = 2;
					sound_effect.buffer[10] = 1;
					sound_effect.buffer[11] = 1;
					sound_effect.buffer[12] = 380;
					sound_effect.buffer[13] = 2;
					sound_effect.buffer[14] = 1;
					sound_effect.buffer[15] = 1;
					sound_effect.buffer[16] = 360;
					sound_effect.buffer[17] = 2;
					sound_effect.buffer[18] = 1;
					sound_effect.buffer[19] = 1;
					sound_effect.buffer[20] = 340;
					sound_effect.buffer[21] = 2;
					sound_effect.buffer[22] = 1;
					sound_effect.buffer[23] = 1;
					sound_effect.buffer[24] = 320;
					sound_effect.buffer[25] = 2;
					sound_effect.buffer[26] = 1;
					sound_effect.buffer[27] = 1;
					sound_effect.buffer[28] = 300;
					sound_effect.buffer[29] = 2;
					sound_effect.buffer[30] = 1;
					sound_effect.buffer[31] = 1;
					sound_effect.buffer[32] = 280;
					sound_effect.buffer[33] = 2;
					sound_effect.buffer[34] = 270;
					sound_effect.buffer[35] = 2;
					sound_effect.buffer[36] = 260;
					sound_effect.buffer[37] = 2;
					sound_effect.buffer[38] = 250;
					sound_effect.buffer[39] = 2;
					sound_effect.buffer[40] = 240;
					sound_effect.buffer[41] = 2;
					sound_effect.buffer[42] = 230;
					sound_effect.buffer[43] = 2;
					sound_effect.buffer[44] = 220;
					sound_effect.buffer[45] = 2;
					sound_effect.buffer[46] = 210;
					sound_effect.buffer[47] = 2;
					sound_effect.buffer[48] = 99;
					sound_effect.buffer[49] = 99;

					sound_effect.note_time = 0;
					sound_effect.note_index = 0;
					sound_effect.playing_note = false;
					sound_effect.play = true;
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
	switch (engine.AUDIO_SelectedMode) {
		case 0:// PC Speaker;
			// Send note
			if (sound_effect.play & !sound_effect.playing_note) {
				sound_effect.note = sound_effect.buffer[sound_effect.note_index];
				sound_effect.note_duration = sound_effect.buffer[sound_effect.note_index + 1];

				if (sound_effect.note == 99) {
					sound_effect.play = false;
					SPK_Mute();
				} else {
					if (sound_effect.note != 0) {
						SPK_Unmute();
						SPK_Sound(sound_effect.note);
					} else
						SPK_Mute();

					sound_effect.playing_note = true;
				}
			}

			// Wait note time
			if (sound_effect.play & sound_effect.playing_note) {
				sound_effect.note_time++;
				if (sound_effect.note_time > sound_effect.note_duration) {
					sound_effect.note_index += 2;
					sound_effect.note_time = 0;
					sound_effect.playing_note = false;
					SPK_Mute();
				}
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