#include "../engine.h"
#include "gfx.h"
#include "../video/vga.h"
#include "string.h"

byte *gfxPaletteShown;
byte *gfxPaletteLoaded;
byte *gfxFont;

StatusPanel gfx_actor_status_panel;
StatusPanel gfx_enemy_status_panel;

//int gfx_stack_index;
//byte gfx_sprites_stack[MAX_SPRITES];

int gfx_sprites_priority_index;
byte gfx_sprites_priority_stack[SPRITE_MAX_STACK];
SpriteGraphic gfx_sprite_graphics_stack[SPRITE_MAX_GRAPHICS];
Sprite gfx_sprite_stack[SPRITE_MAX_STACK];

/** GFX :: Initialize graphics
 */
void GFX_Init(void) {
	printf(" Initializing Graphics... \n");

	// Reserve memory for palette
	gfxPaletteShown = MM_PushChunk(256 * 3, CT_PALETTE);
	gfxPaletteLoaded = MM_PushChunk(256 * 3, CT_PALETTE);

	// Reserve memory for font
	gfxFont = MM_PushChunk(FONT_GFX_WIDTH * FONT_GFX_HEIGHT, CT_FONT);
}

/** GFX :: Shutdown graphics
 */
void GFX_Shutdown(void) {
	// Free all chunks
	MM_PopChunks(CT_FONT);
	MM_PopChunks(CT_PALETTE);
}

/** GFX :: Load sprite graphics on the stack (deprecated)
 */
void GFX_LoadSpriteGraphics(const char *dat_name, const char *asset_name, int id, int width_px, int height_px, int num_frames, int transparent_color, int hit_color) {

	word image_width_px, image_height_px;
	int rows, columns;
	register int src_index;
	register int dst_index;
	register int i, j, k;
	byte *dataLoaded = MM_PushChunk(width_px * height_px * num_frames, CT_TEMPORARY);

	// Check validity of sprite number
	if ((id < 0) || (id >= SPRITE_MAX_GRAPHICS)) {
		MM_PopChunks(CT_TEMPORARY);
		sprintf(engine.system_error_message1, "GFX_LoadSpriteGraphics function error");
		sprintf(engine.system_error_message2, "Invalid sprite stack graphic id number %u", id);
		sprintf(engine.system_error_message3, "DAT filename: %s, Asset filename: %s", dat_name, asset_name);
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Check if sprite is already loaded
	if (gfx_sprite_graphics_stack[id].loaded) {
		MM_PopChunks(CT_TEMPORARY);
		sprintf(engine.system_error_message1, "GFX_LoadSpriteGraphics function error");
		sprintf(engine.system_error_message2, "Sprite stack graphic id number %u already in use", id);
		sprintf(engine.system_error_message3, "DAT filename: %s, Asset filename: %s", dat_name, asset_name);
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	gfx_sprite_graphics_stack[id].buffer = MM_PushChunk(width_px * height_px * num_frames, CT_SPRITE);

	// Load graphics file
	FILE_LoadPCXSprite(dat_name, asset_name, dataLoaded, width_px * height_px * num_frames, &image_width_px, &image_height_px, SPRITE_TRANSP_COLOR);

	gfx_sprite_graphics_stack[id].num_frames = num_frames;
	gfx_sprite_graphics_stack[id].width_px = width_px;
	gfx_sprite_graphics_stack[id].height_px = height_px;
	gfx_sprite_graphics_stack[id].transparent_color = transparent_color;
	gfx_sprite_graphics_stack[id].hit_color = hit_color;

	rows = (image_height_px / height_px);
	columns = (image_width_px / width_px);

	// Transfer data to sprite raw graphics
	src_index = 0;
	dst_index = 0;
	for (i = 0; i < rows; i++) {
		for (j = 0; j < columns; j++) {
			src_index = ((i * height_px) * image_width_px) + (j * width_px);
			for (k = 0; k < height_px; k++) {
				memcpy(&gfx_sprite_graphics_stack[id].buffer[dst_index], &dataLoaded[src_index], width_px);
				src_index += image_width_px;
				dst_index += width_px;
			}
		}
	}

	// Set loaded flag
	gfx_sprite_graphics_stack[id].loaded = true;

	MM_PopChunks(CT_TEMPORARY);
}

/** GFX :: Load sprite graphics on the stack
 */
void GFX_LoadSpriteGraphicsRLE(const char *dat_name, const char *asset_name, int id, int width_px, int height_px, int num_frames, int transparent_color, int hit_color) {

	word image_width_px, image_height_px;
	int rows, columns;
	register int src_index;
	register int dst_index;
	register int i, j, k, l;
	register int tranp_color_counter;
	register int num_frame;
	register int row_offset;
	register byte color;
	byte *s, *d;
	byte *dataLoaded;

	gfx_sprite_graphics_stack[id].buffer = MM_PushChunk(width_px * height_px * num_frames, CT_SPRITE);
	gfx_sprite_graphics_stack[id].frame_offset = MM_PushChunk(num_frames << 1, CT_SPRITE);
	gfx_sprite_graphics_stack[id].row_offset = MM_PushChunk((num_frames * height_px) << 1, CT_SPRITE);

	dataLoaded = MM_PushChunk(width_px * height_px * num_frames, CT_TEMPORARY);

	// Check validity of sprite number
	if ((id < 0) || (id >= SPRITE_MAX_GRAPHICS)) {
		MM_PopChunks(CT_TEMPORARY);
		sprintf(engine.system_error_message1, "GFX_LoadSpriteGraphics function error");
		sprintf(engine.system_error_message2, "Invalid sprite stack graphic id number %u", id);
		sprintf(engine.system_error_message3, "DAT filename: %s, Asset filename: %s", dat_name, asset_name);
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Check if sprite is already loaded
	if (gfx_sprite_graphics_stack[id].loaded) {
		MM_PopChunks(CT_TEMPORARY);
		sprintf(engine.system_error_message1, "GFX_LoadSpriteGraphics function error");
		sprintf(engine.system_error_message2, "Sprite stack graphic id number %u already in use", id);
		sprintf(engine.system_error_message3, "DAT filename: %s, Asset filename: %s", dat_name, asset_name);
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Load graphics file
	FILE_LoadPCXSprite(dat_name, asset_name, dataLoaded, width_px * height_px * num_frames, &image_width_px, &image_height_px, SPRITE_TRANSP_COLOR);

	gfx_sprite_graphics_stack[id].num_frames = num_frames;
	gfx_sprite_graphics_stack[id].width_px = width_px;
	gfx_sprite_graphics_stack[id].height_px = height_px;
	gfx_sprite_graphics_stack[id].transparent_color = transparent_color;
	gfx_sprite_graphics_stack[id].hit_color = hit_color;

	rows = (image_height_px / height_px);
	columns = (image_width_px / width_px);

	// Transfer data to sprite raw graphics
	// Set destination
	dst_index = 0;
	num_frame = 0;

	for (i = 0; i < rows; i++) {
		for (j = 0; j < columns; j++) {
			tranp_color_counter = 0;
			row_offset = 0;

			// Set source
			src_index = ((i * height_px) * image_width_px) + (j * width_px);
			gfx_sprite_graphics_stack[id].frame_offset[num_frame] = dst_index;
			num_frame++;

			s = &dataLoaded[src_index];
			d = &gfx_sprite_graphics_stack[id].buffer[dst_index];

			for (k = 0; k < height_px; k++) {
				gfx_sprite_graphics_stack[id].row_offset[(num_frame * height_px) + k] = row_offset;
				for (l = 0; l < width_px; l++) {
					color = *s++;
					switch (color) {
						case SPRITE_TRANSP_COLOR:
							if (tranp_color_counter == 0) {// first transparent bunch
								*d++ = 1;
								dst_index++;
								row_offset++;
							}
							tranp_color_counter++;
							break;
						default:                           // just a color
							if (tranp_color_counter != 0) {// end of transparent bunch
								*d++ = tranp_color_counter;
								dst_index++;
								row_offset += tranp_color_counter;
								tranp_color_counter = 0;
							}
							*d++ = color;
							dst_index++;
							row_offset++;
							break;
					}
				}
				if (tranp_color_counter != 0) {
					*d++ = tranp_color_counter;
					dst_index++;
					row_offset += tranp_color_counter;
					tranp_color_counter = 0;
				}
				*d++ = 0;// Set end of scanline
				dst_index++;
				row_offset++;
				s += image_width_px - width_px;
			}
		}
	}


	// Set loaded flag
	gfx_sprite_graphics_stack[id].loaded = true;

	MM_PopChunks(CT_TEMPORARY);
}

void GFX_SetPanelGraphics(StatusPanel *panel, int portait_graphics_id, int lifebar_graphics_id) {
	panel->portait_graphics_id = portait_graphics_id;
	panel->lifebar_graphics_id = lifebar_graphics_id;
	panel->portait_frame = 0;
}

void GFX_SetPanelPosition(StatusPanel *panel, int x, int y, int portait_x, int portait_y, int lifebar_x, int lifebar_y) {

	panel->pos_x = x;
	panel->pos_y = y;

	panel->portait_x = portait_x;
	panel->portait_y = portait_y;
	panel->lifebar_x = lifebar_x;
	panel->lifebar_y = lifebar_y;
}

void GFX_ShowPanel(StatusPanel *panel, bool show, int timeout) {

	panel->life_speed = 1;
	panel->life_speed_counter = 0;
	panel->shown = show;
	panel->timeout = timeout;
}

void GFX_UpdatePanel(StatusPanel *panel, int life, int old_life, int max_life, int portait_frame) {
	panel->life_speed_counter = 0;
	panel->old_life = (old_life * 45) / max_life;
	panel->current_life = (life * 45) / max_life;
	panel->portait_frame = portait_frame - 1;
	panel->shown = true;
}

void GFX_SetPanelPortait(StatusPanel *panel, int portait_frame) {
	panel->portait_frame = portait_frame - 1;
}

/** Unload all sprite graphics
 *
 */
void GFX_UnloadSpriteGraphics(void) {
	int i;
	for (i = 0; i < SPRITE_MAX_STACK; i++) {
		gfx_sprite_stack[i].loaded = false;
	}
	for (i = 0; i < SPRITE_MAX_GRAPHICS; i++) {
		gfx_sprite_graphics_stack[i].loaded = false;
	}
	MM_PopChunks(CT_SPRITE);
}

/** GFX :: Initialize sprite on the stack
 */
bool GFX_InitSprite(byte graphics_id, byte entity_id, byte entity_num, int *number) {
	int i;

	// Check if graphics id is already loaded
	if (!gfx_sprite_graphics_stack[graphics_id].loaded) {
		sprintf(engine.system_error_message1, "GFX_InitSprite function error");
		sprintf(engine.system_error_message2, "Graphics id %u not loaded", graphics_id);
		sprintf(engine.system_error_message3, "");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Search for an empty space
	for (i = 0; i < SPRITE_MAX_STACK; i++) {
		if (!gfx_sprite_stack[i].loaded) {
			gfx_sprite_stack[i].loaded = true;
			gfx_sprite_stack[i].graphics_id = graphics_id;
			gfx_sprite_stack[i].entity_id = entity_id;
			gfx_sprite_stack[i].id = entity_num << 8 | entity_id;
			gfx_sprite_stack[i].animation_loop = false;
			gfx_sprite_stack[i].animation_end = false;
			gfx_sprite_stack[i].animation_inverted = false;
			gfx_sprite_stack[i].animation_counter = 0;
			gfx_sprite_stack[i].animation_base_frame = 0;
			gfx_sprite_stack[i].animation_speed = 1;
			gfx_sprite_stack[i].animation_frames = 1;
			gfx_sprite_stack[i].current_frame = 1;
			gfx_sprite_stack[i].screen_pos_x = -99;
			gfx_sprite_stack[i].screen_pos_y = -99;
			gfx_sprite_stack[i].width_px = gfx_sprite_graphics_stack[graphics_id].width_px;
			gfx_sprite_stack[i].height_px = gfx_sprite_graphics_stack[graphics_id].height_px;
			gfx_sprite_stack[i].blink = false;
			gfx_sprite_stack[i].unmasked = false;
			gfx_sprite_stack[i].shown = false;
			*number = i;

			return true;
		}
	}

	return false;
}

/** GFX :: Unload sprite 
 */
void GFX_UnloadSprite(int number) {
	gfx_sprite_stack[number].loaded = false;
	gfx_sprite_stack[number].shown = false;
}

/** GFX :: Unload all sprites 
 */
void GFX_UnloadSprites(void) {
	int i;
	for (i = 0; i < SPRITE_MAX_STACK; i++) {
		gfx_sprite_stack[i].loaded = false;
	}
}

/** GFX :: Update sprites
 */
void GFX_UpdateSprites(void) {
	byte aux;
	int i, j;
	int index;

	// Reset stack
	gfx_sprites_priority_index = 0;
	memset(gfx_sprites_priority_stack, 0, SPRITE_MAX_STACK);

	// Calculate screen pos and evaluate visibility
	for (i = 0; i < SPRITE_MAX_STACK; i++) {

		if (gfx_sprite_stack[i].loaded) {
			if (((gfx_sprite_stack[i].screen_pos_x + gfx_sprite_graphics_stack[gfx_sprite_stack[i].graphics_id].width_px) > 0)

				&& (gfx_sprite_stack[i].screen_pos_x < camera.visible_width)

				&& ((gfx_sprite_stack[i].screen_pos_y + gfx_sprite_graphics_stack[gfx_sprite_stack[i].graphics_id].height_px) > 0)

				&& (gfx_sprite_stack[i].screen_pos_y < camera.visible_height)) {

				gfx_sprite_stack[i].shown = true;

				// Add to stack
				gfx_sprites_priority_stack[gfx_sprites_priority_index] = i;
				gfx_sprites_priority_index++;
			} else {
				gfx_sprite_stack[i].shown = false;
			}
		} else {
			gfx_sprite_stack[i].shown = false;
		}
	}

	// Set sprite priority stack
	for (i = 0; i < gfx_sprites_priority_index; i++) {
		for (j = 0; j < gfx_sprites_priority_index - 1; j++) {
			if ((gfx_sprite_stack[gfx_sprites_priority_stack[j]].screen_pos_y > gfx_sprite_stack[gfx_sprites_priority_stack[j + 1]].screen_pos_y)

				|| (gfx_sprite_stack[gfx_sprites_priority_stack[j]].graphics_id == SPRITE_GRAPHICS_ID_CURSOR)) {

				aux = gfx_sprites_priority_stack[j + 1];
				gfx_sprites_priority_stack[j + 1] = gfx_sprites_priority_stack[j];
				gfx_sprites_priority_stack[j] = aux;
			}
		}
	}

	// Update animation frames
	for (i = 0; i < gfx_sprites_priority_index; i++) {
		index = gfx_sprites_priority_stack[i];

		gfx_sprite_stack[index].animation_counter++;
		if (gfx_sprite_stack[index].animation_counter >= gfx_sprite_stack[index].animation_speed) {
			gfx_sprite_stack[index].animation_counter = 0;

			if (gfx_sprite_stack[index].current_frame < gfx_sprite_stack[index].animation_frames) {
				gfx_sprite_stack[index].current_frame++;
			} else {
				if (gfx_sprite_stack[index].animation_loop) {
					gfx_sprite_stack[index].current_frame = 1;
				} else {
					gfx_sprite_stack[index].animation_end = true;
				}
			}
		}
	}
}

/** GFX :: Draw sprites 
 */
void GFX_DrawSprites(void) {
	int i, sprite_num;
	for (i = 0; i < gfx_sprites_priority_index; i++) {
		if (gfx_sprite_stack[gfx_sprites_priority_stack[i]].shown) {
			sprite_num = gfx_sprites_priority_stack[i];
			VIDEO_DrawSpriteToScreenBufferRLE(&gfx_sprite_graphics_stack[gfx_sprite_stack[sprite_num].graphics_id], gfx_sprite_stack[sprite_num]);
		}
	}
}

/** GFX :: Load image
 *  - PCX file inside a DAT file required
 *  - Loads image into RAM video buffer   
 */
void GFX_LoadImage(const char *filename, const char *subfile, int size) {
	int i;
	int src_index, dst_index;
	word width, height;

	byte *dataLoaded = MM_PushChunk(size, CT_TEMPORARY);
	FILE_LoadPCXImage(filename, subfile, dataLoaded, size, gfxPaletteLoaded, &width, &height);
	// Transfer data to video buffer
	src_index = 0;
	dst_index = 0;
	for (i = 0; i < height; i++) {
		memcpy(&video.map_buffer[MAP_BACKGROUND_LAYER][dst_index], &dataLoaded[src_index], width);
		src_index += width;
		dst_index += video.map_buffer_width;
	}

	MM_PopChunks(CT_TEMPORARY);
}

/** GFX :: Load font
 *  - PCX file inside a DAT file required
 *  - Loads image into RAM video buffer   
 */
void GFX_LoadFont(const char *filename, const char *subfile, int size) {

	word width;
	word height;
	register int src_index;
	register int dst_index;
	register int c, r, j, k;
	register int num_char_columns, num_char_rows;

	byte *dataLoaded = MM_PushChunk(size, CT_TEMPORARY);
	FILE_LoadPCXImage(filename, subfile, dataLoaded, size, gfxPaletteLoaded, &width, &height);

	num_char_columns = (width >> 3);
	num_char_rows = (height >> 3);
	src_index = 0;
	dst_index = 0;

	// regular mode
	for (r = 0; r < num_char_rows; r++) {
		for (c = 0; c < num_char_columns; c++) {
			for (j = 0; j < 8; j++) {
				for (k = 0; k < 8; k++) {
					src_index = ((width << 3) * r) + (c << 3) + (j * width) + k;
					gfxFont[dst_index] = dataLoaded[src_index] + 220;// font colors are allocated from color 220
					dst_index++;
				}
			}
		}
	}

	MM_PopChunks(CT_TEMPORARY);
}

/** GFX :: Load palette
 */
void GFX_LoadPalette(const char *dat_file, const char *asset_file, int size) {
	FILE_LoadPCXPalette(dat_file, asset_file, gfxPaletteLoaded, size);
}
