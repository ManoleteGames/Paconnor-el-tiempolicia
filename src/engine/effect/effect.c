#include "../engine.h"
#include "effect.h"

Effect effect[EFFECT_MAX_EFFECTS];

/** EFFECT :: Load effect
 */
void EFFECT_LoadEffect(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y) {
	effect[number].type = entity_id;
	effect[number].is_loaded = true;
	effect[number].pos_x = pos_x;
	effect[number].pos_y = pos_y;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "EFFECT_LoadEffect function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Initialize cursor sprite
	if (!GFX_InitSprite(graphics_id, entity_id, number, &effect[number].num_sprite)) {
		sprintf(engine.system_error_message1, "EFFECT_LoadEffect function error");
		sprintf(engine.system_error_message2, "Cannot initialize effect sprite");
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Set initial screen position
	gfx_sprite_stack[effect[number].num_sprite].screen_pos_x = pos_x - camera.pos_x;
	gfx_sprite_stack[effect[number].num_sprite].screen_pos_y = pos_y - camera.pos_y;
	gfx_sprite_stack[effect[number].num_sprite].animation_base_frame = 0;
	gfx_sprite_stack[effect[number].num_sprite].animation_frames = 1;
	gfx_sprite_stack[effect[number].num_sprite].current_frame = (rand() % (gfx_sprite_graphics_stack[graphics_id].num_frames + 1));

	gfx_sprite_stack[effect[number].num_sprite].shown = true;

	effect[number].current_step = 0;
	effect[number].steps = 5;
}


/** EFFECT :: Update effects
 */
void EFFECT_UpdateEffects(void) {
	int i;
	int sprite_num;

	// Calculate screen pos and evaluate visibility
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		if (effect[i].is_loaded) {

			sprite_num = effect[i].num_sprite;

			// Update current screen position
			gfx_sprite_stack[sprite_num].screen_pos_x = effect[i].pos_x - camera.pos_x;
			gfx_sprite_stack[sprite_num].screen_pos_y = effect[i].pos_y - camera.pos_y;

			//gfx_sprite_stack[sprite_num].screen_pos_x = gfx_sprite_stack[actor.sprite_num].screen_pos_x;
			//gfx_sprite_stack[sprite_num].screen_pos_y = gfx_sprite_stack[actor.sprite_num].screen_pos_y;

			effect[i].current_step++;

			if (effect[i].current_step >= effect[i].steps) {
				gfx_sprite_stack[sprite_num].loaded = false;
				gfx_sprite_stack[sprite_num].shown = false;

				effect[i].is_loaded = false;
			}
		}
	}
}