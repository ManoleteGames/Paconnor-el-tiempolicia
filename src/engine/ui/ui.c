#include "../engine.h"
#include "ui.h"

bool ui_show_panel;
byte *ui_panel;
UI ui;
Button ui_button[UI_MAX_BUTTONS];

void UI_ShowPanel(void) {
}

void UI_ButtonOver(int button_number) {
	int sprite_num;
	if (ui_button[button_number].is_loaded) {

		sprite_num = ui_button[button_number].num_sprite;

		switch (ui_button[button_number].type) {
			case ENTITY_ID_BTN:
				// Update current screen position
				gfx_sprite_stack[sprite_num].current_frame = 2;
				break;
		}
	}
}

void UI_ButtonClick(int button_number) {
	int sprite_num;
	if (ui_button[button_number].is_loaded) {

		sprite_num = ui_button[button_number].num_sprite;

		switch (ui_button[button_number].type) {
			case ENTITY_ID_BTN:
				gfx_sprite_stack[sprite_num].current_frame = 3;
				ui.button_pressed = button_number;
				break;
		}
	}
}

/** UI :: Update buttons
 */
void UI_UpdateButtons(void) {
	int i;
	int sprite_num;

	// Reset pressed button
	ui.button_pressed = 0;

	// Calculate screen pos and evaluate visibility
	for (i = 0; i < UI_MAX_BUTTONS; i++) {
		if (ui_button[i].is_loaded) {

			sprite_num = ui_button[i].num_sprite;

			switch (ui_button[i].type) {
				case ENTITY_ID_BTN:
					// Clear status
					gfx_sprite_stack[sprite_num].blink = false;
					gfx_sprite_stack[sprite_num].current_frame = 1;

					// Update current screen position
					gfx_sprite_stack[sprite_num].screen_pos_x = ui_button[i].pos_x - camera.pos_x;
					gfx_sprite_stack[sprite_num].screen_pos_y = ui_button[i].pos_y - camera.pos_y;
					break;
			}
		}
	}

	// Check mouse pointing
	switch (cursor.point_on & 0xFF) {
		case ENTITY_ID_BTN:
			UI_ButtonOver((cursor.point_on >> 8) & 0xFF);
			if (cursor.left_click) UI_ButtonClick((cursor.point_on >> 8) & 0xFF);
			break;
		default:
			break;
	}
}

void UI_UpdateUI(bool combat_mode) {
	int x, y;

	UI_UpdateButtons();

	// Pause
	if (engine.ingame & kbKeyState[SCANCODE_P]) {
		if (ui.pause) {
			ui.pause = false;
			ui.freeze = false;
			VIDEO_SetPalette(gfx.palette_loaded);
		} else {
			ui.pause = true;
			ui.freeze = true;
			VIDEO_FadeOutPause(4);
		}
		kbKeyState[SCANCODE_P] = false;
	}

	// Exit ingame request
	if (engine.ingame & kbKeyState[SCANCODE_ESC]) {
		if (!ui.exit_request) {
			ui.exit_request = true;
			ui.freeze = true;
			VIDEO_FadeOutPause(4);
		}
		kbKeyState[SCANCODE_ESC] = false;
	}

	// Exit ingame
	if (engine.ingame & ui.exit_request & kbKeyState[SCANCODE_Y]) {
		engine.ingame = false;
		engine.exit_game = true;
		ui.freeze = false;
		kbKeyState[SCANCODE_Y] = false;

		ui.exit_request = false;
		ui.freeze = false;
		VIDEO_SetPalette(gfx.palette_loaded);
	}

	// NOT Exit ingame
	if (engine.ingame & ui.exit_request & kbKeyState[SCANCODE_N]) {
		engine.ingame = true;
		ui.freeze = false;
		kbKeyState[SCANCODE_N] = false;

		ui.exit_request = false;
		ui.freeze = false;
		VIDEO_SetPalette(gfx.palette_loaded);
	}

	// Show exit request
	if (ui.exit_request) {
		x = (video.screen_width >> 1) - 48;
		y = (video.screen_height >> 1) - 8;
		ui.speech_length = FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 8, ui.speech);
		VIDEO_StringToScreenBuffer(x, y, ui.speech, FONT_BIG_WHITE);
	}

	// Show pause
	if (ui.pause) {
		x = (video.screen_width >> 1) - 24;
		y = (video.screen_height >> 1) - 8;
		ui.speech_length = FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 7, ui.speech);
		VIDEO_StringToScreenBuffer(x, y, ui.speech, FONT_BIG_WHITE);
	}

	// Show description
	if (ui.show_description) {
		ui.show_description = false;
		x = cursor.pos_x - ((ui.name_length << 3) >> 1);
		y = cursor.pos_y - 16;
		if (x < 0) x = 0;
		if (x + (ui.name_length << 3) > camera.visible_width) x = camera.visible_width - (ui.name_length << 3);
		if (y < 0) y = 0;
		VIDEO_StringToScreenBuffer(x, y, ui.object_name, FONT_SLIM_WHITE);
	}

	if (ui.show_speech) {
		VIDEO_ChatToScreenBuffer(&gfx_chat_panel);

		ui.speech_time++;
		if (ui.speech_time > ui.speech_timeout) {
			ui.show_speech = false;
			ui.freeze = false;
		}
	}

	if (combat_mode) VIDEO_PanelToScreenBuffer(&gfx_actor_status_panel);
	if (combat_mode) VIDEO_PanelToScreenBuffer(&gfx_enemy_status_panel);
}

void UI_ShowSpeech(ChatPanel *pannel, int portait_graphics_id, int box_graphics_id, const char *dat_name, const char *asset_name, int line1, int line2, int line3, bool inverted, int timeout) {
	GFX_SetChatGraphics(pannel, portait_graphics_id, box_graphics_id);
	GFX_SetChatPosition(pannel, 10, 0, 10, 10, inverted, 80, 10, inverted);
	FILE_LoadText(dat_name, asset_name, line1, ui.speech);
	GFX_SetLineChatPanel(&gfx_chat_panel, ui.speech, 0);
	FILE_LoadText(dat_name, asset_name, line2, ui.speech);
	GFX_SetLineChatPanel(&gfx_chat_panel, ui.speech, 1);
	FILE_LoadText(dat_name, asset_name, line3, ui.speech);
	GFX_SetLineChatPanel(&gfx_chat_panel, ui.speech, 2);
	ui.freeze = true;
	ui.show_speech = true;
	ui.speech_time = 0;
	ui.speech_timeout = timeout;
}

void UI_ShowDescription(const char *dat_name, const char *asset_name, int line) {
	ui.name_length = FILE_LoadText(dat_name, asset_name, line, ui.object_name);
	ui.show_description = true;
}

int UI_GetButtonXPosition(byte number) {
	return ui_button[number].pos_x;
}

int UI_GetButtonYPosition(byte number) {
	return ui_button[number].pos_y;
}

void UI_SetButtonPosition(byte number, int x, int y) {
	ui_button[number].pos_x = x;
	ui_button[number].pos_y = y;
	gfx_sprite_stack[ui_button[number].num_sprite].screen_pos_x = x;
	gfx_sprite_stack[ui_button[number].num_sprite].screen_pos_y = y;
}

/** UI :: Load button
 */
void UI_LoadButton(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y) {
	ui_button[number].type = entity_id;
	ui_button[number].is_loaded = true;
	ui_button[number].pos_x = pos_x;
	ui_button[number].pos_y = pos_y;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "UI_LoadButton function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Initialize cursor sprite
	if (!GFX_InitSprite(graphics_id, entity_id, number, &ui_button[number].num_sprite)) {
		sprintf(engine.system_error_message1, "UI_LoadButton function error");
		sprintf(engine.system_error_message2, "Cannot initialize button sprite");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Set hit box
	//  ----------------
	// |  1          2  |
	// |                |
	// |                |
	// |                |
	// |                |
	// |  3          4  |
	//  ----------------
	// Point 1
	ui_button[number].hit_area.points[0][0] = 0;
	ui_button[number].hit_area.points[0][1] = 0;
	// Point 2
	ui_button[number].hit_area.points[1][0] = gfx_sprite_stack[ui_button[number].num_sprite].width_px;
	ui_button[number].hit_area.points[1][1] = 0;
	// Point 3
	ui_button[number].hit_area.points[2][0] = 0;
	ui_button[number].hit_area.points[2][1] = gfx_sprite_stack[ui_button[number].num_sprite].height_px;
	// Point 4
	ui_button[number].hit_area.points[3][0] = gfx_sprite_stack[ui_button[number].num_sprite].width_px;
	ui_button[number].hit_area.points[3][1] = gfx_sprite_stack[ui_button[number].num_sprite].height_px;


	// Set initial screen position
	gfx_sprite_stack[ui_button[number].num_sprite].screen_pos_x = pos_x - camera.pos_x;
	gfx_sprite_stack[ui_button[number].num_sprite].screen_pos_y = pos_y - camera.pos_y;
	gfx_sprite_stack[ui_button[number].num_sprite].animation_base_frame = 0;
	gfx_sprite_stack[ui_button[number].num_sprite].animation_frames = 1;
	gfx_sprite_stack[ui_button[number].num_sprite].current_frame = 1;
}


/** UI :: Uload buttons
 */
void UI_UnloadButtons(void) {
	int i;
	for (i = 0; i < UI_MAX_BUTTONS; i++) {
		ui_button[i].is_loaded = false;
	}
}