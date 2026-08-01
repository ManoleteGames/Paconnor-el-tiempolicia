
#include "../engine.h"
#include "object.h"

Object object[OBJECT_MAX_OBJECTS];
static byte object_update_counter;
byte object_counter;

/** OBJECT :: Load object
 */
void OBJECT_LoadObject(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y) {
	int i;

	object[number].type = entity_id;
	object[number].is_loaded = true;
	object[number].pos_x = pos_x;
	object[number].pos_y = pos_y;
	object[number].is_hit = false;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "OBJECT_LoadObject function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	object[number].num_sprite = GFX_FindEmptySpriteSlot();
	if (object[number].num_sprite == -1) {
		sprintf(engine.system_error_message1, "OBJECT_LoadObject function error");
		sprintf(engine.system_error_message2, "No empty sprite slot available");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	} else {
		GFX_InitSprite(entity_id, number, object[number].num_sprite, 0, gfx_sprite_graphics_stack[graphics_id].width_px, gfx_sprite_graphics_stack[graphics_id].height_px);
	}

	// Set colission points
	//  ----------------
	// |                |
	// |                |
	// |                |
	// |  0           1 |
	// |                |
	// |                |
	// |                |
	// |  2           3 |
	//  ----------------
	// Point 0
	object[number].colission_area.points[0][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.points[0][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);
	// Point 1
	object[number].colission_area.points[1][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.points[1][1] = (gfx_sprite_stack[object[number].num_sprite].height_px >> 1) - (gfx_sprite_stack[object[number].num_sprite].height_px >> 4);

	// Point 10
	object[number].colission_area.points[2][0] = gfx_sprite_stack[object[number].num_sprite].width_px >> 4;
	object[number].colission_area.points[2][1] = gfx_sprite_stack[object[number].num_sprite].height_px;
	// Point 12
	object[number].colission_area.points[3][0] = gfx_sprite_stack[object[number].num_sprite].width_px - (gfx_sprite_stack[object[number].num_sprite].width_px >> 4);
	object[number].colission_area.points[3][1] = gfx_sprite_stack[object[number].num_sprite].height_px;

	// Set hit box
	//  ----------------
	// |                |
	// |                |
	// |  0          1  |
	// |                |
	// |                |
	// |                |
	// |                |
	// |  2          3  |
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

	GFX_SetSpriteGraphic(object[number].num_sprite, 0, graphics_id, 0, 0);// feet
	for (i = 1; i < 5; i++) {
		gfx_sprite_stack[object[number].num_sprite].gfx[i].graphics_id = -1;
		gfx_sprite_stack[object[number].num_sprite].gfx[i].offset_x = 0;
		gfx_sprite_stack[object[number].num_sprite].gfx[i].offset_y = 0;
		gfx_sprite_stack[object[number].num_sprite].gfx[i].screen_pos_x = -1;
		gfx_sprite_stack[object[number].num_sprite].gfx[i].screen_pos_y = -1;
	}

	// Set initial screen position
	gfx_sprite_stack[object[number].num_sprite].screen_pos_x = pos_x - camera.pos_x;
	gfx_sprite_stack[object[number].num_sprite].screen_pos_y = pos_y - camera.pos_y;

	// Load object data
	switch (entity_id) {
		case ENTITY_ID_BARREL:
			object[number].max_life = 10;
			object[number].life = 10;
			object[number].steps = 12;
			object[number].current_step = 1;
			break;
		case ENTITY_ID_TARGET:
			object[number].max_life = 100;
			object[number].life = 100;
			object[number].steps = 7;
			object[number].current_step = 1;
			break;
		case ENTITY_ID_BTN:
			object[number].max_life = 1;
			object[number].life = 1;
			object[number].steps = 2;
			object[number].current_step = 1;
			break;
		case ENTITY_ID_NPC:
			object[number].max_life = 20;
			object[number].life = 20;
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
	bool update_object;

	object_update_counter++;
	if (object_update_counter > 6) {
		object_update_counter = 0;
	}

	object_counter = 0;
	// Calculate screen pos and evaluate visibility
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			object_counter++;

			// Update enemy position on screen
			GFX_SetSpritePosition(object[i].num_sprite, object[i].pos_x - camera.pos_x, object[i].pos_y - camera.pos_y);

			// On screen
			object[i].on_screen = GFX_IsSpriteOnScreen(object[i].num_sprite);

			// DEBUG: Draw collision and hit pixels
			//if (enemy[i].on_screen) ENEMY_DrawColissionPixels(enemy[i]);
			//if (enemy[i].on_screen) ENEMY_DrawHitPixels(enemy[i]);

			// Avoid to update objects each cycle
			update_object = (i + object_update_counter) & 1;
			if (update_object) {

				sprite_num = object[i].num_sprite;
				switch (object[i].type) {
					case ENTITY_ID_BARREL:

						// hit detection
						if ((!object[i].explode) && (object[i].is_hit)) {
							object[i].is_hit = false;
							gfx_sprite_stack[object[i].num_sprite].blink.active = true;
							if (object[i].damage > object[i].life) object[i].damage = object[i].life;
							object[i].life -= object[i].damage;
							// Check if hit by actor bullet and update enemy panel
							switch (object[i].hit_by) {
								case ENTITY_ID_ACTOR_BULLET:
									GFX_SetPanelGraphics(&gfx_enemy_status_panel, SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR, SPRITE_GRAPHICS_ID_GUN0);
									GFX_UpdatePanel(&gfx_enemy_status_panel, object[i].life, object[i].life + object[i].damage, object[i].max_life, 1);
									gfx_enemy_status_panel.shown = true;
									break;
								default:
									break;
							}

							if (object[i].life <= 0) {
								object[i].life = 0;
								object[i].explode = true;
								GFX_SetDefaultAnimation(object[i].num_sprite, false, false, 20);

								AUDIO_PlaySound(AUDIO_EXPLOSSION, 16);
								//gfx_sprite_stack[sprite_num].animation_frames = 12;
								//gfx_sprite_stack[sprite_num].animation_current_frame = 1;
								//gfx_sprite_stack[sprite_num].animation_speed = 4;
								//gfx_sprite_stack[sprite_num].animation_end = false;
								gfx_sprite_stack[sprite_num].entity_id = ENTITY_ID_EXPLOSION;
								gfx_sprite_stack[sprite_num].id = ENTITY_ID_EXPLOSION << 8 | i;
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 10, object[i].pos_y, 1, 10, 8, 8);
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 16, object[i].pos_y + 16, 1, 10, 8, 8);
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x - 8, object[i].pos_y + 16, object[i].pos_x - 10, object[i].pos_y + 32, 1, 10, 8, 8);
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 8, object[i].pos_x + 21, object[i].pos_y, 1, 10, 8, 8);
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 16, object[i].pos_x + 24, object[i].pos_y + 16, 1, 10, 8, 8);
								PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_FIRE1, ENTITY_ID_EXPLOSION, object[i].pos_x + 18, object[i].pos_y + 32, object[i].pos_x + 21, object[i].pos_y + 32, 1, 10, 8, 8);

								GFX_SetPanelPortait(&gfx_enemy_status_panel, 2);
							}
							object[i].damage = 0;

						} else {
							gfx_sprite_stack[object[i].num_sprite].blink.active = false;
						}

						// Explossion
						if (object[i].explode) {
							gfx_sprite_stack[sprite_num].blink.active = false;

							if (GFX_IsSpriteAnimationEnded(object[i].num_sprite, 0)) {
								// Generate particles arround
								GFX_UnloadSprite(sprite_num);
								object[i].explode = false;
								object[i].is_loaded = false;
							}
						}
						break;
					case ENTITY_ID_TARGET:
						// hit detection
						if ((!object[i].explode) && (object[i].is_hit)) {
							object[i].is_hit = false;
							gfx_sprite_stack[object[i].num_sprite].blink.active = true;
							if (object[i].damage > object[i].life) object[i].damage = object[i].life;
							object[i].life -= object[i].damage;

							if (object[i].life <= 0) {
								object[i].life = 0;
								object[i].explode = true;
								GFX_SetDefaultAnimation(object[i].num_sprite, false, false, 3);
							}
							object[i].damage = 0;

						} else {
							gfx_sprite_stack[object[i].num_sprite].blink.active = false;
						}

						// Explossion
						if (object[i].explode) {
							gfx_sprite_stack[sprite_num].blink.active = false;
							if (GFX_IsSpriteAnimationEnded(object[i].num_sprite, 0)) {
								// Generate particles arround
								GFX_UnloadSprite(sprite_num);
								object[i].explode = false;
								object[i].is_loaded = false;
							}
						}
						break;
					case ENTITY_ID_BTN:
						break;
					case ENTITY_ID_NPC:

						// hit detection
						if (object[i].is_hit) {
							object[i].is_hit = false;
							gfx_sprite_stack[object[i].num_sprite].blink.active = true;
							if (object[i].damage > object[i].life) object[i].damage = object[i].life;
							object[i].life -= object[i].damage;

							if (object[i].life <= 0) {
								object[i].life = 0;
							}
							object[i].damage = 0;

						} else {
							gfx_sprite_stack[object[i].num_sprite].blink.active = false;
						}

						break;
					default:
						break;
				}
			}
		}
	}
}

void OBJECT_UnloadObjects(void) {
	int i;
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			GFX_UnloadSprite(object[i].num_sprite);
			object[i].is_loaded = false;
			object[i].num_sprite = -1;
		}
	}
}

void OBJECT_UnloadObject(int number) {

	if (object[number].is_loaded) {
		GFX_UnloadSprite(object[number].num_sprite);
		object[number].is_loaded = false;
		object[number].num_sprite = -1;
	}
}

bool OBJECT_IsLoaded(int number) {
	return object[number].is_loaded;
}

bool OBJECT_IsOnScreen(int number) {
	if (object[number].is_loaded) {
		return object[number].on_screen;
	} else {
		return false;
	}
}

void OBJECT_SetPosition(int number, int pos_x, int pos_y) {
	if (object[number].is_loaded) {
		object[number].pos_x = pos_x;
		object[number].pos_y = pos_y;
	}
}