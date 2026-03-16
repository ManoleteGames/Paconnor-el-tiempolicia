#include "../engine.h"
#include "mouse.h"
#include <string.h>

bool mouse_initialized;
bool mouse_present;

_go32_dpmi_registers mouse_regs;
int old_mouse_handler_mask;
int old_mouse_handler_seg;
int old_mouse_handler_off;

MouseData cursor;

/** MOUSE :: Interrupt handler
 */
void MOUSE_handler(void) {
	cursor.pos_x = mouse_regs.x.cx;
	cursor.pos_y = mouse_regs.x.dx;
	cursor.left_click = mouse_regs.x.bx & 0x01;
	cursor.right_click = mouse_regs.x.bx & 0x02;
}

/** MOUSE :: Restrict movement zone 
 */
void MOUSE_RestrictZone(int x1, int x2, int y1, int y2) {
	__dpmi_regs regs;
	regs.x.ax = 7;
	regs.x.cx = x1;
	regs.x.dx = x2;
	__dpmi_int(0x33, &regs);

	regs.x.ax = 8;
	regs.x.cx = y1;
	regs.x.dx = y2;
	__dpmi_int(0x33, &regs);
}

/** MOUSE :: Initialize interrupt and memory reservation 
 */
void MOUSE_Init(void) {
	__dpmi_regs r;
	_go32_dpmi_seginfo mouse_seginfo;

	if (!mouse_initialized) {
		mouse_seginfo.pm_offset = (int) MOUSE_handler;

		_go32_dpmi_allocate_real_mode_callback_retf(&mouse_seginfo,
													&mouse_regs);
		r.x.ax = 0x14;
		r.x.cx = 0x1F;
		r.x.dx = mouse_seginfo.rm_offset;
		r.x.es = mouse_seginfo.rm_segment;

		__dpmi_int(0x33, &r);

		old_mouse_handler_mask = r.x.cx;
		old_mouse_handler_off = r.x.dx;
		old_mouse_handler_seg = r.x.es;
	}

	// Set cursor initial values
	cursor.pos_x = -99;
	cursor.pos_y = -99;
	cursor.left_click = false;
	cursor.right_click = false;

	mouse_initialized = true;
}

/** MOUSE :: Shutdown interrupt and free graphics 
 */
void MOUSE_Shutdown(void) {
	__dpmi_regs r;
	if (mouse_initialized) {
		r.x.ax = 0xC;
		r.x.cx = (short) old_mouse_handler_mask;
		r.x.dx = (short) old_mouse_handler_off;
		r.x.es = (short) old_mouse_handler_seg;
		__dpmi_int(0x33, &r);
	}

	mouse_initialized = false;
}

/** MOUSE :: Show cursor 
 */
void MOUSE_ShowCursor(void) {
	__dpmi_regs regs;
	regs.x.ax = 1;
	__dpmi_int(0x33, &regs);
}

/** MOUSE :: Check if availablity
 *  - Returns true if mouse is available
 */
bool MOUSE_CheckIfAvailable(void) {
	__dpmi_regs regs;
	regs.x.ax = 0;
	__dpmi_int(0x33, &regs);

	if (regs.x.ax == 0xFFFF) {
		return true;
	} else {
		return false;
	}
}

void MOUSE_InitCursorSprite(byte graphics_id) {

	// Initialize cursor sprite
	if (!GFX_InitSprite(graphics_id, ENTITY_ID_CURSOR, 0, &cursor.sprite_num)) {
		sprintf(engine.system_error_message1, "MOUSE_InitCursorSprite function error");
		sprintf(engine.system_error_message2, "Cannot initialize cursor sprite");
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	gfx_sprite_stack[cursor.sprite_num].animation_base_frame = 0;
	gfx_sprite_stack[cursor.sprite_num].animation_frames = 1;
	gfx_sprite_stack[cursor.sprite_num].animation_speed = 0;
	gfx_sprite_stack[cursor.sprite_num].current_frame = 1;
	gfx_sprite_stack[cursor.sprite_num].unmasked = true;
}

/** MOUSE :: Set cursor graphic frame
 */
void MOUSE_SetCursorFrame(int frame_number) {
	if (frame_number > gfx_sprite_graphics_stack[gfx_sprite_stack[cursor.sprite_num].graphics_id].num_frames) {
		sprintf(engine.system_error_message1, "MOUSE_SetCursorFrame function error");
		sprintf(engine.system_error_message2, "Frame number %u out of limits ", frame_number);
		sprintf(engine.system_error_message3, "Max frame number %u", gfx_sprite_graphics_stack[gfx_sprite_stack[cursor.sprite_num].graphics_id].num_frames);
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}
	gfx_sprite_stack[cursor.sprite_num].current_frame = frame_number;
	gfx_sprite_stack[cursor.sprite_num].animation_frames = 1;
}

/** MOUSE :: Check cursor colission
 */
int MOUSE_CheckCursorColission(void) {
	int i, hspot;
	int point1_x, point1_y;

	point1_x = cursor.pos_x;
	point1_y = cursor.pos_y;


	/////// BUTTONS COLISSIONS ///////////
	for (i = 0; i < UI_MAX_BUTTONS; i++) {
		if (ui_button[i].is_loaded) {
			if (gfx_sprite_stack[ui_button[i].num_sprite].shown) {

				//Point 1
				if (point1_x < ui_button[i].pos_x + ui_button[i].hit_area.points[1][0]

					&& point1_x > ui_button[i].pos_x + ui_button[i].hit_area.points[0][0]

					&& point1_y > ui_button[i].pos_y + ui_button[i].hit_area.points[0][1]

					&& point1_y < ui_button[i].pos_y + ui_button[i].hit_area.points[2][1]) {
					return gfx_sprite_stack[ui_button[i].num_sprite].id;
				}
			}
		}
	}


	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x < gfx_sprite_stack[object[i].num_sprite].screen_pos_x + object[i].hit_area.points[1][0]

					&& point1_x > gfx_sprite_stack[object[i].num_sprite].screen_pos_x + object[i].hit_area.points[0][0]

					&& point1_y > gfx_sprite_stack[object[i].num_sprite].screen_pos_y + object[i].hit_area.points[0][1]

					&& point1_y < gfx_sprite_stack[object[i].num_sprite].screen_pos_y + object[i].hit_area.points[2][1]) {
					return ((i << 8) | object[i].type);
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].hit_area.points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].hit_area.points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].hit_area.points[0][1]

					&& point1_y < enemy[i].pos_y + enemy[i].hit_area.points[1][1]) {
					return gfx_sprite_stack[enemy[i].sprite_num].id;
				}
			}
		}
	}

	/////// HOTSPOT COLISSIONS ///////////
	hspot = 0;
	if (map.loaded) {
		hspot = MAP_CheckHotspotTile(camera.pos_x + cursor.pos_x, camera.pos_y + cursor.pos_y);
		if (hspot != 255) return ((hspot) << 8) | ENTITY_ID_HSPOT;
		else
			return 0;
	}


	return 0;
}

void MOUSE_Update(bool combat_mode) {
	if (mouse_initialized) {
		gfx_sprite_stack[cursor.sprite_num].screen_pos_x = cursor.pos_x;
		gfx_sprite_stack[cursor.sprite_num].screen_pos_y = cursor.pos_y;

		if (ui.freeze) {
			gfx_sprite_stack[cursor.sprite_num].graphics_id = SPRITE_GRAPHICS_ID_CURSOR;
			return;
		}

		if (!combat_mode) {
			gfx_sprite_stack[cursor.sprite_num].graphics_id = SPRITE_GRAPHICS_ID_CURSOR;
			cursor.point_on = MOUSE_CheckCursorColission();

			if (cursor.left_click) {
				gfx_sprite_stack[cursor.sprite_num].animation_end = false;
				gfx_sprite_stack[cursor.sprite_num].animation_counter = 0;
				gfx_sprite_stack[cursor.sprite_num].animation_frames = 7;
				gfx_sprite_stack[cursor.sprite_num].current_frame = 1;
				gfx_sprite_stack[cursor.sprite_num].animation_speed = 2;
			}

			// Update animation
			if (gfx_sprite_stack[cursor.sprite_num].animation_end) {
				gfx_sprite_stack[cursor.sprite_num].animation_counter = 0;
				gfx_sprite_stack[cursor.sprite_num].animation_frames = 1;
				gfx_sprite_stack[cursor.sprite_num].current_frame = 1;
			}
		} else {
			gfx_sprite_stack[cursor.sprite_num].graphics_id = SPRITE_GRAPHICS_ID_AIM;
		}
	}
}
