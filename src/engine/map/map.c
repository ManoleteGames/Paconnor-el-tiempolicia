#include "../engine.h"
#include "map.h"

Map map;

/** MAP :: Initialize map
 */
void MAP_Init(void) {
	printf(" Initializing Maps... \n");
	// Reserve memory for background tileset
	map.layer_tiles[MAP_BACKGROUND_LAYER].buffer = MM_PushChunk(MAP_MAX_BACK_TILESET_WIDTH * MAP_MAX_BACK_TILESET_HEIGHT, CT_MAP);
	// Reserve memory for foreground tileset
	map.layer_tiles[MAP_FOREGROUND_LAYER].buffer = MM_PushChunk(MAP_MAX_FORE_TILESET_WIDTH * MAP_MAX_FORE_TILESET_HEIGHT, CT_MAP);
	// Reserve memory for mask tileset
	map.layer_tiles[MAP_MASK_LAYER].buffer = MM_PushChunk(MAP_MAX_MASK_TILESET_WIDTH * MAP_MAX_MASK_TILESET_HEIGHT, CT_MAP);
	// Reserve memory for map background
	map.layer[MAP_BACKGROUND_LAYER] = MM_PushChunk((MAP_MAX_WIDTH * MAP_MAX_HEIGHT) << 1, CT_MAP);
	// Reserve memory for map foreground
	map.layer[MAP_FOREGROUND_LAYER] = MM_PushChunk((MAP_MAX_WIDTH * MAP_MAX_HEIGHT) << 1, CT_MAP);
	// Reserve memory for map mask
	map.layer[MAP_MASK_LAYER] = MM_PushChunk((MAP_MAX_WIDTH * MAP_MAX_HEIGHT) << 1, CT_MAP);
	// Reserve memory for map colission
	map.layer[MAP_COLISSION_LAYER] = MM_PushChunk((MAP_MAX_WIDTH * MAP_MAX_HEIGHT) << 1, CT_MAP);
	// Reserve memory for map event
	map.layer[MAP_EVENT_HSPOT_LAYER] = MM_PushChunk((MAP_MAX_WIDTH * MAP_MAX_HEIGHT) << 1, CT_MAP);
}

/** MAP :: Shutdown map data
 */
void MAP_Shutdown(void) {
	MM_PopChunks(CT_MAP);
}

int MAP_GetTileNumber(int x_px, int y_px) {
	return ((y_px >> 4) * map.width) + (x_px >> 4);
}

int MAP_CheckColissionTile(int x_px, int y_px) {
	return map.layer[MAP_COLISSION_LAYER][MAP_GetTileNumber(x_px, y_px)];
}

/** MAP :: Tileset load
 *  - Tileset must be a pcx indexed color file 
 *  - Max. number of colors 256
 *  - Colors from 220..255 are reserved to sprites
 *  - Tiles size must be 16x16 px
 */
void MAP_LoadTileset(const char *dat_file, const char *asset_file, Tileset *tset, int size) {
	dword i, j, k;
	dword src_index, dst_index;

	byte *data_loaded = MM_PushChunk(size, CT_TEMPORARY);

	FILE_LoadPCXTileset(dat_file, asset_file, data_loaded, size, &tset->width_px, &tset->height_px);

	tset->width = tset->width_px >> 4;
	tset->height = tset->height_px >> 4;
	tset->num_tiles = tset->width * tset->height;

	// Transfer data to tileset buffer
	src_index = 0;
	dst_index = 0;
	for (i = 0; i < tset->height; i++) {
		for (j = 0; j < tset->width; j++) {
			src_index = ((i << 4) * tset->width_px) + (j << 4);
			for (k = 0; k < MAP_TILE_HEIGHT; k++) {
				memcpy(&tset->buffer[dst_index], &data_loaded[src_index], MAP_TILE_WIDTH);
				src_index += tset->width_px;
				dst_index += 16;
			}
		}
	}

	MM_PopChunks(CT_TEMPORARY);
}

/** MAP :: Load map
 *  - Loads 5 layers of map from a dat file
 *  - Background file must be "back.csv"
 *  - Foreground or mask file must be "fore.csv"
 *  - ...
 */
void MAP_LoadMap(const char *dat_file, int width, int height) {

	if (width > MAP_MAX_WIDTH || height > MAP_MAX_HEIGHT) {
		Error("Map size exceeds maximum allowed", "", "", ERROR_SYSTEM);
	}
	FILE_LoadMap_CSV(dat_file, map.layer[MAP_BACKGROUND_LAYER], map.layer[MAP_FOREGROUND_LAYER], map.layer[MAP_MASK_LAYER], map.layer[MAP_COLISSION_LAYER], map.layer[MAP_EVENT_HSPOT_LAYER], width * height);

	map.width = width;
	map.height = height;
	map.width_px = map.width << 4;
	map.height_px = map.height << 4;

	map.loaded = true;
}

/** MAP :: Draw a solid zone on video buffer
 *  - No transparency, all pixels are printed
 *  - x_px: Left pixel of the zone to print
 *  - y_px: Top pixel of the zone to print
 *  - width_px: Width in pixels of the zone to print
 *  - height_px: Height in pixels of the zone to print
 *  - buffer: Source buffer
 */
void MAP_DrawToVideoBuffer(int x_px, int y_px, int width_px, int height_px, byte *src_buffer, byte *video_buffer) {
	register int i;
	register int src_index, dst_index;

	src_index = 0;
	dst_index = (y_px * video.map_buffer_width) + x_px;
	for (i = 0; i < height_px; i++) {
		memcpy(&video_buffer[dst_index], &src_buffer[src_index], width_px);
		src_index += width_px;
		dst_index += video.map_buffer_width;
	}
}

/** MAP :: Clear a solid zone on video buffer
 *  - No transparency, all pixels are printed
 *  - x_px: Left pixel of the zone to print
 *  - y_px: Top pixel of the zone to print
 *  - width_px: Width in pixels of the zone to print
 *  - height_px: Height in pixels of the zone to print
 *  - buffer: Source buffer
 */
void MAP_ClearVideoBuffer(int x_px, int y_px, int width_px, int height_px, byte *video_buffer) {
	register int i;
	register int dst_index;

	dst_index = (y_px * video.map_buffer_width) + x_px;
	for (i = 0; i < height_px; i++) {
		memset(&video_buffer[dst_index], 0, width_px);
		dst_index += video.map_buffer_width;
	}
}

/** MAP :: Draw a transparent zone on video buffer
 *  - Transparency active, transparent pixel will be skipped
 *  - x_px: Left pixel of the zone to print
 *  - y_px: Top pixel of the zone to print
 *  - width_px: Width in pixels of the zone to print
 *  - height_px: Height in pixels of the zone to print
 *  - buffer: Source buffer
 */
void MAP_DrawToVideoBuffer_WithTransparency(int x_px, int y_px, int width_px, int height_px, byte *src_buffer, byte *video_buffer) {
	register int i, j;
	register int dst_index;
	byte *src, *dst;
	byte value;

	dst_index = (y_px * video.map_buffer_width) + x_px;

	src = src_buffer;
	dst = &video_buffer[dst_index];

	for (i = 0; i < height_px; i++) {
		for (j = 0; j < width_px; j++) {
			value = *src++;
			if (value) *dst++ = value;
			else
				dst++;
		}
		dst += (video.map_buffer_width - 16);
	}
}

void MAP_DrawToScreenBuffer(int x_px, int y_px, int width_px, int height_px, byte *src_buffer, byte *screen_buffer) {
	register int i;
	register int src_index, dst_index;

	src_index = 0;
	dst_index = (y_px * video.screen_width) + x_px;
	for (i = 0; i < height_px; i++) {
		memcpy(&screen_buffer[dst_index], &src_buffer[src_index], width_px);
		src_index += width_px;
		dst_index += video.screen_width;
	}
}

void MAP_DrawToScreenBuffer_WithTransparency(int x_px, int y_px, int width_px, int height_px, byte *src_buffer, byte *screen_buffer) {
	register int i, j;
	register int dst_index;
	byte *src, *dst;
	byte value;

	dst_index = (y_px * video.screen_width) + x_px;

	src = src_buffer;
	dst = &screen_buffer[dst_index];

	for (i = 0; i < height_px; i++) {
		for (j = 0; j < width_px; j++) {
			value = *src++;
			if (value) *dst++ = value;
			else
				dst++;
		}
		dst += (video.screen_width - 16);
	}
}

/** MAP :: Draw background zone
 *  - x_px: Left pixel of the zone to print
 *  - y_px: Top pixel of the zone to print
 *  - width_px: Width in pixels of the zone to print
 *  - height_px: Height in pixels of the zone to print
 *  - buffer: Source buffer
 */
void MAP_DrawBackground(int x_px, int y_px, int width_px, int height_px, byte *buffer) {
	register int i;
	register int src_index, dst_index;

	src_index = 0;
	dst_index = (y_px * video.map_buffer_width) + x_px;
	for (i = 0; i < height_px; i++) {
		memcpy(&video.map_buffer[MAP_BACKGROUND_LAYER][dst_index], &buffer[src_index], width_px);
		src_index += width_px;
		dst_index += video.map_buffer_width;
	}
}

/** MAP:: Draw mask zone in pixels
 *  - x_px: Left pixel of the zone to print
 *  - y_px: Top pixel of the zone to print
 *  - width_px: Width in pixels of the zone to print
 *  - height_px: Height in pixels of the zone to print
 *  - buffer: Source buffer
 */
void MAP_DrawMask(int x_px, int y_px, int width_px, int height_px, byte *buffer) {
	register int i;
	register int src_index, dst_index;

	src_index = 0;
	dst_index = (y_px * video.map_buffer_width) + x_px;
	for (i = 0; i < height_px; i++) {
		memcpy(&video.map_buffer[MAP_MASK_LAYER][dst_index], &buffer[src_index], width_px);
		src_index += width_px;
		dst_index += video.map_buffer_width;
	}
}

/** MAP :: Draw background tile
 *  - Tile dimensions 16x16 px
 *  - x: Tile x position on video buffer
 *  - y: Tile y position on video buffer
 */
void MAP_DrawBackgroundTile(int x, int y, int num_tile) {
	register int src_index;
	src_index = (num_tile << 8);// num tile * 256 to get pixel offset
	MAP_DrawBackground(x << 4, y << 4, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[src_index]);
}

/** MAP :: Draw foreground tile
 *  - Tile dimensions 16x16 px
 *  - x: Tile x position on video buffer
 *  - y: Tile y position on video buffer
 */
void MAP_DrawForegroundTile(int x, int y, int num_tile) {
	register int tile_data, tileset_index;

	tile_data = map.layer[MAP_FOREGROUND_LAYER][num_tile];
	tileset_index = tile_data << 8;

	MAP_DrawToVideoBuffer(x << MAP_TILE_WIDTH_BASE2, y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
}

/** MAP :: Draw mask tile
 *  - Tile dimensions 16x16 px
 *  - x: Tile x position on video buffer
 *  - y: Tile y position on video buffer
 */
void MAP_DrawMaskTile(int x, int y, int num_tile) {
	register int src_index;
	if (num_tile == 255) { num_tile = 0; }
	src_index = (num_tile << 8);// num tile * 256 to get pixel offset
	MAP_DrawMask(x << 4, y << 4, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_MASK_LAYER].buffer[src_index]);
}

/** MAP :: Draw map column
 *  - x: Left map position
 *  - y: Top map position
 *  - tile_x: Left map tile number
 *  - tile_y: Top map tile number
 */
void MAP_DrawMapColumn(int logical_pos_x, int logical_pos_y, int tile_x, int tile_y) {
	register int i;
	register int tile_number, tile_data, tileset_index;
	register int logical_x, logical_y;

	if (map.loaded) {

		logical_x = logical_pos_x;
		logical_y = logical_pos_y;

		for (i = 0; i < (camera.visible_height >> MAP_TILE_HEIGHT_BASE2) + 1; i++) {
			tile_number = ((tile_y + i) * map.width) + tile_x;

			switch (map.layer[MAP_COLISSION_LAYER][tile_number]) {
				case 0:// Nothing
					// Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 1:// Full colission
					// Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 2:// Half colission
					   // Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 3:// Breakable
					   // Background layer
					tile_data = map.layer[MAP_FOREGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				default:
					break;
			}

			// Foreground layer
			/*tile_data = map.layer[MAP_FOREGROUND_LAYER][tile_number];
			tileset_index = tile_data << 8;
			MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_FOREGROUND_LAYER]);*/

			// Mask layer
			tile_data = map.layer[MAP_MASK_LAYER][tile_number];
			tileset_index = tile_data << 8;
			MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_MASK_LAYER].buffer[tileset_index], video.map_buffer[MAP_MASK_LAYER]);

			logical_y++;
			if (logical_y >= (video.map_buffer_height >> 4)) { logical_y = 0; }
		}
	} else {
		VGA_SetMode(VIDEO_TEXT_MODE);
		printf("Map not loaded \n");
		sleep(2);
		Error("Map not loaded \n", "", "", ERROR_SYSTEM);
	}
}


/** MAP :: Draw map row
 *  - x: Left map position
 *  - y: Top map position
 *  - tile_x: Left map tile number
 *  - tile_y: Top map tile number
  */
void MAP_DrawMapRow(int logical_pos_x, int logical_pos_y, int tile_x, int tile_y) {

	register int i;
	register int tile_number, tile_data, tileset_index;
	register int logical_x, logical_y;

	if (map.loaded) {

		logical_x = logical_pos_x;
		logical_y = logical_pos_y;

		for (i = 0; i < (camera.visible_width >> 4) + 1; i++) {
			tile_number = (tile_y * map.width) + tile_x + i;

			switch (map.layer[MAP_COLISSION_LAYER][tile_number]) {
				case 0:// Nothing
					// Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 1:// Full colission
					// Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 2:// Half colission
					   // Background layer
					tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				case 3:// Breakable
					   // Background layer
					tile_data = map.layer[MAP_FOREGROUND_LAYER][tile_number];
					tileset_index = tile_data << 8;
					MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);
					break;
				default:
					break;
			}

			// Foreground layer
			/*tile_data = map.layer[MAP_FOREGROUND_LAYER][tile_number];
			tileset_index = tile_data << 8;
			MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_FOREGROUND_LAYER]);*/

			// Mask layer
			tile_data = map.layer[MAP_MASK_LAYER][tile_number];
			tileset_index = tile_data << 8;
			MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_MASK_LAYER].buffer[tileset_index], video.map_buffer[MAP_MASK_LAYER]);

			logical_x++;
			if (logical_x >= (video.map_buffer_width >> 4)) { logical_x = 0; }
		}
	} else {
		VGA_SetMode(VIDEO_TEXT_MODE);
		printf("Map not loaded \n");
		sleep(2);
		Error("Map not loaded \n", "", "", ERROR_SYSTEM);
	}
}

/** MAP :: Draw map
 *  - x: Left map logical position
 *  - y: Top map logical position
 */
void MAP_DrawMapToMapVideoBuffer(void) {
	register int i;
	register int pos_x, pos_y;
	register int tile_x, tile_y;

	if (map.loaded) {
		// Draw column by column
		for (i = 0; i < (video.map_buffer_width >> MAP_TILE_WIDTH_BASE2); i++) {

			pos_x = camera.pos_abs_x + (i << MAP_TILE_WIDTH_BASE2);
			if (pos_x >= video.map_buffer_width) { pos_x = pos_x - video.map_buffer_width; }
			pos_y = camera.pos_abs_y;

			tile_x = (camera.pos_x >> MAP_TILE_WIDTH_BASE2) + i;
			tile_y = (camera.pos_y >> MAP_TILE_HEIGHT_BASE2);
			MAP_DrawMapColumn(pos_x >> MAP_TILE_WIDTH_BASE2, pos_y >> MAP_TILE_HEIGHT_BASE2, tile_x, tile_y);
		}
	}
}

void MAP_Update(void) {
	int tile_x, tile_y;
	int pos_x, pos_y;

	if (map.loaded) {
		// Displacement right
		if ((map.update_required) && (camera.old_pos_x < camera.pos_x)) {
			//if (camera.old_pos_x < camera.pos_x) {
			pos_x = camera.pos_abs_x + camera.visible_width;
			if (pos_x >= video.map_buffer_width) { pos_x = pos_x - video.map_buffer_width; }
			pos_y = camera.pos_abs_y;
			tile_x = (camera.pos_x >> 4) + (camera.visible_width >> 4);
			tile_y = (camera.pos_y >> 4);
			MAP_DrawMapColumn(pos_x >> 4, pos_y >> 4, tile_x, tile_y);
		}

		// Displacement left
		if ((map.update_required) && (camera.pos_x < camera.old_pos_x)) {
			//if (camera.pos_x < camera.old_pos_x) {
			pos_x = camera.pos_abs_x;
			if (pos_x < 0) { pos_x = pos_x + video.map_buffer_width; }
			pos_y = camera.pos_abs_y;
			tile_x = (camera.pos_x >> 4);
			tile_y = (camera.pos_y >> 4);
			MAP_DrawMapColumn(pos_x >> 4, pos_y >> 4, tile_x, tile_y);
		}

		// Displacement down
		if ((map.update_required) && (camera.old_pos_y < camera.pos_y)) {
			//if (camera.old_pos_y < camera.pos_y) {
			pos_y = camera.pos_abs_y + camera.visible_height;
			if (pos_y >= video.map_buffer_height) { pos_y = pos_y - video.map_buffer_height; }
			pos_x = camera.pos_abs_x;
			tile_x = (camera.pos_x >> 4);
			tile_y = (camera.pos_y >> 4) + (camera.visible_height >> 4);
			MAP_DrawMapRow(pos_x >> 4, pos_y >> 4, tile_x, tile_y);
		}

		// Displacement up
		if ((map.update_required) && (camera.pos_y < camera.old_pos_y)) {
			//if (camera.pos_y < camera.old_pos_y) {
			pos_y = camera.pos_abs_y;
			if (pos_y < 0) { pos_y = pos_y + video.map_buffer_height; }
			pos_x = camera.pos_abs_x;
			tile_x = (camera.pos_x >> 4);
			tile_y = (camera.pos_y >> 4);
			MAP_DrawMapRow(pos_x >> 4, pos_y >> 4, tile_x, tile_y);
		}

		map.update_required = false;
	}
}


/** MAP :: Update map with parallax
 *  - Just horizontal parallax
 *  - Background is the back layer
 *  - Colision is the middle layer
 *  - Mask is the front layer
 */
void MAP_Update_Paralax(void) {
	int i;
	int tile_x, tile_y;
	register int tile_number, tile_data, tileset_index;
	int pos_x, pos_y;
	register int logical_x, logical_y;
	register bool not_solid, clear;


	if (map.loaded) {

		// 1st- Back layer. Background map buffer with cam 1 to screen without transparency

		// Displacement right
		//if (((camera.old_pos_x % 16) == 0) && (camera.old_pos_x < camera.pos_x)) {
		if (cam_parallax[0].old_pos_x < cam_parallax[0].pos_x) {
			pos_x = cam_parallax[0].pos_abs_x + cam_parallax[0].visible_width;
			if (pos_x >= video.map_buffer_width) {
				pos_x = pos_x - video.map_buffer_width;
			}
			pos_y = cam_parallax[0].pos_abs_y;
			tile_x = (cam_parallax[0].pos_x >> 4) + (cam_parallax[0].visible_width >> 4);
			tile_y = (cam_parallax[0].pos_y >> 4);

			logical_x = (pos_x >> 4);
			logical_y = pos_y >> 4;

			for (i = 0; i < (cam_parallax[0].visible_height >> 4); i++) {
				tile_number = ((tile_y + i) * map.width) + tile_x;

				// Foreground layer
				tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
				not_solid = (tile_data >> 14) & 0x01;
				clear = (tile_data >> 15) & 0x01;
				if (clear) {
					MAP_ClearVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &video.map_buffer[MAP_BACKGROUND_LAYER][0]);
				} else {
					tileset_index = (tile_data & 0x3FF) << 8;
					if (not_solid) {
						MAP_DrawToVideoBuffer_WithTransparency(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], &video.map_buffer[MAP_BACKGROUND_LAYER][0]);
					} else {
						MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], &video.map_buffer[MAP_BACKGROUND_LAYER][0]);
					}
				}

				logical_y++;
				if (logical_y >= (video.map_buffer_height >> 4)) { logical_y = 0; }
			}
		}

		// 2nd- Middle layer. Foreground map buffer with cam 2 to screen with transparency

		// Displacement right
		//if (((camera.old_pos_x % 16) == 0) && (camera.old_pos_x < camera.pos_x)) {
		if (cam_parallax[1].old_pos_x < cam_parallax[1].pos_x) {
			pos_x = cam_parallax[1].pos_abs_x + cam_parallax[1].visible_width;
			if (pos_x >= video.map_buffer_width) {
				pos_x = pos_x - video.map_buffer_width;
			}
			pos_y = cam_parallax[1].pos_abs_y;
			tile_x = (cam_parallax[1].pos_x >> 4) + (cam_parallax[1].visible_width >> 4);
			tile_y = (cam_parallax[1].pos_y >> 4);

			logical_x = (pos_x >> 4);
			logical_y = pos_y >> 4;

			for (i = 0; i < (cam_parallax[1].visible_height >> 4); i++) {
				tile_number = ((tile_y + i) * map.width) + tile_x;

				// Foreground layer
				tile_data = map.layer[MAP_FOREGROUND_LAYER][tile_number];
				not_solid = (tile_data >> 14) & 0x01;
				clear = (tile_data >> 15) & 0x01;
				if (clear) {
					MAP_ClearVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &video.map_buffer[MAP_FOREGROUND_LAYER][0]);
				} else {
					tileset_index = (tile_data & 0x3FF) << 8;
					if (not_solid) {
						MAP_DrawToVideoBuffer_WithTransparency(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], &video.map_buffer[MAP_FOREGROUND_LAYER][0]);
					} else {
						MAP_DrawToVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_FOREGROUND_LAYER].buffer[tileset_index], &video.map_buffer[MAP_FOREGROUND_LAYER][0]);
					}
				}

				logical_y++;
				if (logical_y >= (video.map_buffer_height >> 4)) { logical_y = 0; }
			}
		}


		// 3rd- Front layer. Mask map buffer with main camera to screen with transparency

		// Displacement right
		//if (((camera.old_pos_x % 16) == 0) && (camera.old_pos_x < camera.pos_x)) {
		if (cam_parallax[2].old_pos_x < cam_parallax[2].pos_x) {
			pos_x = cam_parallax[2].pos_abs_x + cam_parallax[2].visible_width;
			if (pos_x >= video.map_buffer_width) { pos_x = pos_x - video.map_buffer_width; }
			pos_y = cam_parallax[2].pos_abs_y;
			tile_x = (cam_parallax[2].pos_x >> 4) + (cam_parallax[2].visible_width >> 4);
			tile_y = (cam_parallax[2].pos_y >> 4);

			logical_x = pos_x >> 4;
			logical_y = pos_y >> 4;

			for (i = 0; i < (cam_parallax[2].visible_height >> 4); i++) {
				tile_number = ((tile_y + i) * map.width) + tile_x;

				// Mask layer
				tile_data = map.layer[MAP_MASK_LAYER][tile_number];
				not_solid = (tile_data >> 14) & 0x01;
				clear = (tile_data >> 15) & 0x01;
				if (clear) {
					MAP_ClearVideoBuffer(logical_x << MAP_TILE_WIDTH_BASE2, logical_y << MAP_TILE_HEIGHT_BASE2, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &video.map_buffer[MAP_MASK_LAYER][0]);
				} else {
					tileset_index = (tile_data & 0x3FF) << 8;
					if (not_solid) {
						MAP_DrawToVideoBuffer_WithTransparency(logical_x << 4, logical_y << 4, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_MASK_LAYER].buffer[tileset_index], &video.map_buffer[MAP_MASK_LAYER][0]);
					} else {
						MAP_DrawToVideoBuffer(logical_x << 4, logical_y << 4, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_MASK_LAYER].buffer[tileset_index], &video.map_buffer[MAP_MASK_LAYER][0]);
					}
				}

				logical_y++;
				if (logical_y >= (video.map_buffer_height >> 4)) { logical_y = 0; }
			}
		}
	}
}