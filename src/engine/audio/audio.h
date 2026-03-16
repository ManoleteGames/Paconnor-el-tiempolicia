#ifndef _AUDIO_H
#define _AUDIO_H

#define IRS_SPEAKER 0x1C

#define AUDIO_GUN_EFFECT 1
#define AUDIO_PUNCH_EFFECT 2
#define AUDIO_KICK_EFFECT 3
#define AUDIO_EXPLOSSION 10
#define AUDIO_ACTOR_DEAD 20

//Flat              Whole         Sharp}
#define C 0
#define CSharp 1
#define DFlat 1
#define D 2
#define DSharp 3
#define EFlat 3
#define E 4
#define F 5
#define FSharp 6
#define GFlat 6
#define G 7
#define GSharp 8
#define AFlat 8
#define A 9
#define ASharp 10
#define BFlat 10
#define B 11

#define AM 1;
#define VIB 0;

#define WORDLE(A) (word)(((A)[0]) | ((A)[1] << 8) | ((A)[2] << 16) | ((A)[3] << 24))

#endif// _TIMER_H