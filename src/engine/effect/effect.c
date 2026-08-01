#include "../engine.h"
#include "effect.h"

static Effect effect[EFFECT_MAX_EFFECTS];
int effects_counter;

void EFFECT_InitEffects(void) {
	int i;
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		effect[i].is_loaded = false;
		effect[i].num_sprite = -1;
	}
}

/** EFFECT :: Load effect
 */
bool EFFECT_LoadEffect(byte entity_id, byte graphics_id, int pos_x, int pos_y, bool full_animation, int frame, bool inverted, bool loop, int speed) {
	int i, number, sprite_slot;

	// Search for a slot
	number = 99;
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		if (!effect[i].is_loaded) {
			number = i;
			break;
		}
	}

	if (number >= EFFECT_MAX_EFFECTS) return false;

	effect[number].is_loaded = true;
	effect[number].type = entity_id;
	effect[number].pos_x = pos_x;
	effect[number].pos_y = pos_y;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "EFFECT_LoadEffect function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Search for an empty sprite slot and initialize it
	sprite_slot = GFX_FindEmptySpriteSlot();

	// debug
	if (actor.is_loaded && (actor.sprite_num == sprite_slot)) {
		sprintf(engine.system_error_message1, "EFFECT_LoadEffect function error");
		sprintf(engine.system_error_message2, "Effect sprite number %u already in use by actor", sprite_slot);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	if (sprite_slot < 0) {
		sprintf(engine.system_error_message1, "EFFECT_LoadEffect function error");
		sprintf(engine.system_error_message2, "No empty sprite slot available");
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	} else {

		// Initialize effect sprite
		GFX_InitSprite(entity_id, number, sprite_slot, 1, gfx_sprite_graphics_stack[graphics_id].width_px, gfx_sprite_graphics_stack[graphics_id].height_px);
	}

	effect[number].num_sprite = sprite_slot;

	// Set effect graphics
	GFX_SetSpriteGraphic(effect[number].num_sprite, 0, graphics_id, 0, 0);

	// Set initial screen position
	GFX_SetSpritePosition(effect[number].num_sprite, pos_x - camera.pos_x, pos_y - camera.pos_y);
	if (full_animation) GFX_SetDefaultAnimation(effect[number].num_sprite, inverted, loop, speed);
	else
		GFX_SetSingleFrameAnimation(effect[number].num_sprite, frame);
	return true;
}

/** EFFECT :: Update effects
 */
void EFFECT_UpdateEffects(void) {
	int i;
	effects_counter = 0;
	// Calculate screen pos and evaluate visibility
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		if (effect[i].is_loaded) {

			effects_counter++;

			// Update current screen position
			GFX_SetSpritePosition(effect[i].num_sprite, effect[i].pos_x - camera.pos_x, effect[i].pos_y - camera.pos_y);
			if (GFX_IsSpriteAnimationEnded(effect[i].num_sprite, 0)) {
				GFX_UnloadSprite(effect[i].num_sprite);
				effect[i].is_loaded = false;
				effect[i].num_sprite = -1;
			}
		}
	}
}

/** EFFECT :: Unload effects
 */
void EFFECT_UnloadEffects(void) {
	int i;
	// Calculate screen pos and evaluate visibility
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		if (effect[i].is_loaded) {
			GFX_UnloadSprite(effect[i].num_sprite);
			effect[i].is_loaded = false;
			effect[i].num_sprite = -1;
		}
	}
}

bool EFFECT_NoEffectsActive(void) {
	int i;
	for (i = 0; i < EFFECT_MAX_EFFECTS; i++) {
		if (effect[i].is_loaded) {
			return false;
		}
	}
	return true;
}