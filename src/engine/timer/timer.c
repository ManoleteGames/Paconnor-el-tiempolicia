#include "../engine.h"
#include "timer.h"

bool TIMER_Initialized;

volatile static dword milliseconds = 0;

volatile static Timeout timeouts[MAX_TIMERS] = {0};// Array to track multiple timeouts

static _go32_dpmi_seginfo old_handler, new_handler;

void TIMER_Callback(byte param) {
	switch (param) {
		case TIMER_AUDIO_CALLBACK:
			AUDIO_TimerHandler();
			break;
		case TIMER_VIDEO_CALLBACK:
			VIDEO_TimerHandler();
			break;
		case TIMER_MISC_CALLBACK:
			if (engine.logo & kbKeyState[SCANCODE_ESC]) {
				SetDelayTime(0);
				AUDIO_StopSong();
				engine.logo = false;
				VIDEO_FadeOut(1);
			}
			if (engine.sequence & kbKeyState[SCANCODE_ESC]) {
				SetDelayTime(0);
				AUDIO_StopSong();
				engine.sequence = false;
				VIDEO_FadeOut(1);
			}
			break;
		default:
			break;
	}
}

static void TimerHandler(void) {
	disable();
	//milliseconds += 55;// ~55 ms per tick at 18.2 Hz
	milliseconds += 1;// ~1 ms per tick at 1000 Hz

	// Check each registered timeout
	for (int i = 0; i < MAX_TIMERS; i++) {
		timeouts[i].current_time += 1;
		if (timeouts[i].callback && (timeouts[i].current_time >= timeouts[i].end_time)) {
			timeouts[i].current_time = 0;
			timeouts[i].callback(timeouts[i].param);
		}
	}

	// evaluate delay time
	if (engine.delay_time_ms > 0) engine.delay_time_ms--;

	enable();
	outportb(0x20, 0x20);// End-of-interrupt (EOI) signal to PIC
}

/** TIMER :: TIMER_Init
 *  - Sets PIT timer interrupt at a custom frecuency
 *  - Sets second timer callback for audio issues
 *  - Sets third timer callback for video effects
 */
void TIMER_Init(void) {
	dword new_freq;

	if (!TIMER_Initialized) {

		disable();
		_go32_dpmi_get_protected_mode_interrupt_vector(USR_TIMER, &old_handler);

		new_handler.pm_offset = (uint32_t) TimerHandler;
		new_handler.pm_selector = _go32_my_cs();
		_go32_dpmi_chain_protected_mode_interrupt_vector(USR_TIMER, &new_handler);

		// Reconfigure timer
		// - by default 18.2 Hz wich is 55 ms. aprox.
		// - 10 ms -> 100 Hz
		// - 1 ms. -> 1000 Hz
		engine.interrupt_time_ms = 1;
		new_freq = 1193182 / (dword) 1000;
		outportb(0x43, 0x36);

		// Set divisor low byte.
		outp(0x40, (byte) (new_freq & 0xff));
		// Set divisor high byte.
		outp(0x40, (byte) ((new_freq >> 8) & 0xff));

		enable();
	}
	TIMER_Initialized = true;

	// Initialize audio callback
	TIMER_SetTimeout(TIMER_Callback, TIMER_AUDIO_CALLBACK, TIMER_AUDIO_TIME);

	// Initialize video callback
	TIMER_SetTimeout(TIMER_Callback, TIMER_VIDEO_CALLBACK, TIMER_VIDEO_TIME);

	// Initialize misc callback
	TIMER_SetTimeout(TIMER_Callback, TIMER_MISC_CALLBACK, TIMER_MISC_TIME);
}

void TIMER_Shutdown(void) {
	if (TIMER_Initialized) {
		TIMER_ClearAllTimeouts();
		if (_go32_dpmi_set_protected_mode_interrupt_vector(IRS_TIMER, &old_handler) == -1)
			Error("Failed to free the timer", "", "", ERROR_SYSTEM);
	}
	TIMER_Initialized = false;
}

void TIMER_ResetTime(void) {
	milliseconds = 0;
}

dword TIMER_GetMilliseconds(void) {
	return milliseconds;
}

// Register a timeout callback
byte TIMER_SetTimeout(void (*callback)(byte), byte param, dword ms) {
	if (!callback) return -1;// Ignore if NULL function

	disable();

	// Find an empty slot
	for (byte i = 0; i < MAX_TIMERS; i++) {
		if (timeouts[i].callback == NULL) {
			timeouts[i].callback = callback;
			timeouts[i].param = param;
			// Prevent overflow
			if (UINT32_MAX - milliseconds < ms) {
				timeouts[i].end_time = ms - (UINT32_MAX - milliseconds);
			} else {
				timeouts[i].end_time = milliseconds + ms;
			}

			enable();
			return 0;// Success
		}
	}

	enable();
	return 1;// No available slot
}

void TIMER_ClearAllTimeouts() {
	memset((void *) timeouts, 0, sizeof(timeouts));
}