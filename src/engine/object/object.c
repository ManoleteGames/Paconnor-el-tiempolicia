
#include "../engine.h"
#include "object.h"

Object object[OBJECT_MAX_OBJECTS];

/** OBJECT :: Load object
 */
void OBJECT_LoadObject(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y) {
	object[number].type = entity_id;
	object[number].is_loaded = true;
	object[number].pos_x = pos_x;
	object[number].pos_y = pos_y;
	object[number].is_hit = false;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "GFX_LoadObject function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Initialize cursor sprite
	if (!GFX_InitSprite(graphics_id, entity_id, number, &object[number].num_sprite)) {
		sprintf(engine.system_error_message1, "GFX_LoadObject function error");
		sprintf(engine.system_error_message2, "Cannot initialize object sprite");
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Set colission points
	//  ----------------
	// |                |
	// |                |
	// |                |
	// |   1    2    3  |
	// |  4           5 |
	// |  6           7 |
	// |  8           9 |
	// |   10   11  12  |
	//  ----------------
	// Point 1
	object[number].colission_area.up_points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.up_points[0][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);
	// Point 2
	object[number].colission_area.up_points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 1;
	object[number].colission_area.up_points[1][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);
	// Point 3
	object[number].colission_area.up_points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.up_points[2][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);

	// Point 4
	object[number].colission_area.left_points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.left_points[0][1] = gfx_sprite_stack[object[number].num_sprite].height_px >> 1;
	// Point 6
	object[number].colission_area.left_points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.left_points[1][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) + (gfx_sprite_stack[object[number].num_sprite].height_px >> 2);
	// Point 8
	object[number].colission_area.left_points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.left_points[2][1] = gfx_sprite_stack[object[number].num_sprite].height_px - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);

	// Point 5
	object[number].colission_area.right_points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.right_points[0][1] = gfx_sprite_stack[object[number].num_sprite].height_px >> 1;
	// Point 7
	object[number].colission_area.right_points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.right_points[1][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) + (gfx_sprite_stack[object[number].num_sprite].height_px >> 2);
	// Point 9
	object[number].colission_area.right_points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.right_points[2][1] = gfx_sprite_stack[object[number].num_sprite].height_px - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);

	// Point 10
	object[number].colission_area.down_points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.down_points[0][1] = gfx_sprite_stack[object[number].num_sprite].height_px;
	// Point 11
	object[number].colission_area.down_points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 1;
	object[number].colission_area.down_points[1][1] = gfx_sprite_stack[object[number].num_sprite].height_px;
	// Point 12
	object[number].colission_area.down_points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.down_points[2][1] = gfx_sprite_stack[object[number].num_sprite].height_px;

	// Set hit box
	//  ----------------
	// |                |
	// |                |
	// |  1          2  |
	// |                |
	// |                |
	// |                |
	// |                |
	// |  3          4  |
	//  ----------------
	// Point 1
	object[number].hit_area.points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 3;
	object[number].hit_area.points[0][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 3);
	// Point 2
	object[number].hit_area.points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 3);
	object[number].hit_area.points[1][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 3);
	// Point 3
	object[number].hit_area.points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 3;
	object[number].hit_area.points[2][1] = gfx_sprite_stack[object[number].num_sprite].height_px - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);
	// Point 4
	object[number].hit_area.points[3][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 3);
	object[number].hit_area.points[3][1] = gfx_sprite_stack[object[number].num_sprite].height_px - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);


	// Set initial screen position
	gfx_sprite_stack[object[number].num_sprite].screen_pos_x = pos_x - camera.pos_x;
	gfx_sprite_stack[object[number].num_sprite].screen_pos_y = pos_y - camera.pos_y;
	gfx_sprite_stack[object[number].num_sprite].animation_base_frame = 0;
	gfx_sprite_stack[object[number].num_sprite].animation_frames = 1;
	gfx_sprite_stack[object[number].num_sprite].current_frame = 1;

	// Load object data
	switch (entity_id) {
		case ENTITY_ID_BARREL:
			object[number].max_life = 10;
			object[number].life = 10;
			object[number].steps = 12;
			object[number].current_step = 1;
			break;
		case ENTITY_ID_BTN:
			object[number].max_life = 1;
			object[number].life = 1;
			object[number].steps = 2;
			object[number].current_step = 1;
			break;
		default:
			sprintf(engine.system_error_message1, "GFX_LoadObject function error");
			sprintf(engine.system_error_message2, "Unknown object type %u ", entity_id);
			sprintf(engine.system_error_message3, "");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
			break;
	}
}

/** GFX :: Update objects
 */
void OBJECT_UpdateObjects(void) {
	int i;
	int sprite_num;

	// Calculate screen pos and evaluate visibility
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {

			sprite_num = object[i].num_sprite;

			switch (object[i].type) {
				case ENTITY_ID_BARREL:
					// Update current screen position
					gfx_sprite_stack[sprite_num].screen_pos_x = object[i].pos_x - camera.pos_x;
					gfx_sprite_stack[sprite_num].screen_pos_y = object[i].pos_y - camera.pos_y;

					// hit detection
					if ((!object[i].explode) && (object[i].is_hit)) {
						object[i].is_hit = false;
						gfx_sprite_stack[object[i].num_sprite].blink = true;
						if (object[i].damage > object[i].life) object[i].damage = object[i].life;
						object[i].life -= object[i].damage;
						// Check if hit by actor bullet and update enemy panel
						switch (object[i].hit_by) {
							case ENTITY_ID_ACTOR_BULLET:
								GFX_SetPanelGraphics(&gfx_enemy_status_panel, SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
								GFX_UpdatePanel(&gfx_enemy_status_panel, object[i].life, object[i].life + object[i].damage, object[i].max_life, 1);
								gfx_enemy_status_panel.shown = true;
								break;
							default:
								break;
						}

						if (object[i].life <= 0) {
							object[i].life = 0;
							object[i].explode = true;

							AUDIO_PlaySound(AUDIO_EXPLOSSION);
							gfx_sprite_stack[sprite_num].animation_frames = 12;
							gfx_sprite_stack[sprite_num].current_frame = 1;
							gfx_sprite_stack[sprite_num].animation_speed = 4;
							gfx_sprite_stack[sprite_num].animation_end = false;
							gfx_sprite_stack[sprite_num].entity_id = ENTITY_ID_EXPLOSION;
							gfx_sprite_stack[sprite_num].id = ENTITY_ID_EXPLOSION << 8 | i;
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 10, object[i].pos_y, 1, 10);
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 16, object[i].pos_y + 16, 1, 10);
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 10, object[i].pos_y + 32, 1, 10);
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 8, object[i].pos_x + 21, object[i].pos_y, 1, 10);
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 16, object[i].pos_x + 24, object[i].pos_y + 16, 1, 10);
							PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 32, object[i].pos_x + 21, object[i].pos_y + 32, 1, 10);

							GFX_SetPanelPortait(&gfx_enemy_status_panel, 2);
						}
						object[i].damage = 0;

					} else {
						gfx_sprite_stack[object[i].num_sprite].blink = false;
					}

					// Explossion
					if (object[i].explode) {
						gfx_sprite_stack[sprite_num].blink = false;

						if (gfx_sprite_stack[sprite_num].animation_end) {
							// Generate particles arround

							gfx_sprite_stack[sprite_num].loaded = false;
							gfx_sprite_stack[sprite_num].shown = false;
							object[i].explode = false;
							object[i].is_loaded = false;
						}
					}
					break;
				default:
					break;
			}
		}
	}
}

void OBJECT_UnloadObjects(void) {
	int i;
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		object[i].is_loaded = false;
	}
}