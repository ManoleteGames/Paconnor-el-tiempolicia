#ifndef _TIMER_H
#define _TIMER_H

#include "../types/types.h"

#define TIMER_IRS 0x1C
#define MAX_TIMERS 4

#define TIMER_AUDIO_NUMBER 1
#define TIMER_AUDIO_CALLBACK 1
#define TIMER_AUDIO_TIME 10
#define TIMER_VIDEO_NUMBER 2
#define TIMER_VIDEO_CALLBACK 2
#define TIMER_VIDEO_TIME 50
#define TIMER_MISC_NUMBER 3
#define TIMER_MISC_CALLBACK 3
#define TIMER_MISC_TIME 50


/// TIMER.C //
void TIMER_Init(void);
void TIMER_Shutdown(void);
dword TIMER_GetMilliseconds(void);
void TIMER_ClearAllTimeouts(void);
void TIMER_ResetTime(void);
void TIMER_UpdateTimerTime(int timer, dword ms);


#endif// _TIMER_H