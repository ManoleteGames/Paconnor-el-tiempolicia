#include "../engine.h"

bool KEYB_Initialized;

byte kbLastScancode;
bool kbKeyState[128];

byte kbBindingUp;
byte kbBindingDown;
byte kbBindingLeft;
byte kbBindingRight;
byte kbBindingJump;
byte kbBindingFire;
byte kbBindingCombat;

static _go32_dpmi_seginfo old_handler, new_handler;

/** KEYB :: Keyboard interrupt handler
 */
static void KEYB_Handler(void) {
	kbLastScancode = inportb(0x60);
	// Ignore extended multi-byte scancodes (introduced with the IBM PS/2 line).
	if (kbLastScancode != SCANCODE_EXTENDED) {

		// Update our key state array
		if ((kbLastScancode & 0x80) != 0) {
			kbKeyState[kbLastScancode & 0x7f] = false;
		} else {
			kbKeyState[kbLastScancode & 0x7f] = true;
		}
	}

	// Send end-of-interrupt (EOI) signal to PCI (programmable interrupt controller) to resume its normal operation
	outportb(0x20, 0x20);
}

/** KEYB :: Keyboard Initialization
 *  - Initializes keyboard interrupt
 */
void KEYB_Init(void) {
	if (!KEYB_Initialized) {
		_go32_dpmi_get_protected_mode_interrupt_vector(KEYB_IRS, &old_handler);
		new_handler.pm_offset = (dword) KEYB_Handler;
		new_handler.pm_selector = _go32_my_cs();
		_go32_dpmi_allocate_iret_wrapper(&new_handler);
		_go32_dpmi_set_protected_mode_interrupt_vector(KEYB_IRS, &new_handler);
	}
	KEYB_Initialized = true;
}

void KEYB_Shutdown(void) {
	if (KEYB_Initialized) {
		if (_go32_dpmi_set_protected_mode_interrupt_vector(KEYB_IRS, &old_handler) == -1)
			Error("Failed to free the keyboard", "", "", ERROR_SYSTEM);
		else
			_go32_dpmi_free_iret_wrapper(&new_handler);
	}
	KEYB_Initialized = false;
}