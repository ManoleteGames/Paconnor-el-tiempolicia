#include "../engine.h"
#include "ui.h"

bool ui_show_panel;
byte *ui_panel;
UI ui;

void UI_ShowPanel(void) {
}

void UI_UpdateUI(void) {
	int x, y;

	// Switch mode combat
	if (kbKeyState[SCANCODE_P]) {
		if (ui.pause) {
			ui.pause = false;
			VIDEO_SetPalette(gfxPaletteLoaded);
		} else {
			ui.pause = true;
			VIDEO_FadeOutPause(4);
		}
		kbKeyState[SCANCODE_P] = false;
	}

	if (ui.pause) {
		x = (video.screen_width >> 1) - 3;
		y = (video.screen_height >> 1) - 3;
		ui.speech_length = FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 2, ui.speech);
		VIDEO_StringToScreenBuffer(x, y, ui.speech_length, ui.speech);
	}

	if (ui.show_description) {
		x = cursor.pos_x - ((ui.name_length << 3) >> 1);
		y = cursor.pos_y - 16;
		if (x < 0) x = 0;
		if (x + (ui.name_length << 3) > camera.visible_width) x = camera.visible_width - (ui.name_length << 3);
		if (y < 0) y = 0;
		VIDEO_StringToScreenBuffer(x, y, ui.name_length, ui.object_name);
	}

	if (ui.show_speech) {
		x = gfx_sprite_stack[actor.sprite_num].screen_pos_x - ((ui.speech_length << 3) >> 1);
		y = gfx_sprite_stack[actor.sprite_num].screen_pos_y - 16;
		if (gfx_sprite_stack[actor.sprite_num].screen_pos_x - ((ui.speech_length << 3) >> 1) < 0) x = 0;
		if (gfx_sprite_stack[actor.sprite_num].screen_pos_x - ((ui.speech_length << 3) >> 1) + (ui.speech_length << 3) > camera.visible_width) x = camera.visible_width - (ui.speech_length << 3);
		if (y < 0) y = 0;
		VIDEO_StringToScreenBuffer(x, y, ui.speech_length, ui.speech);
		ui.speech_time++;
		if (ui.speech_time > ui.speech_timeout) {
			ui.show_speech = false;
		}
	}
}