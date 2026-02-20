#include "video.h"
#include "vga.h"
#include "cga.h"
#include "ega.h"
#include "../engine.h"
#include <string.h>

Video video;

/** VIDEO :: Initialize video buffers
 *  - Set video variables
 *  - Allocate video memory
 */
void VIDEO_Init(void) {
	printf(" Initializing Video... \n");

	video.selected_mode = 1;// Default to VGA

	switch (video.selected_mode) {
		case 0:// EGA
			// Screen buffers. Must be the same size as the EGA resolution
			video.screen_width = EGA_RESOLUTION_WIDTH;
			video.screen_height = EGA_RESOLUTION_HEIGHT;
			VIDEO_ScreenBufferToVRAM = EGA_ScreenMainBufferToVRAM;
			VIDEO_VSync = EGA_VSync;
			break;
		case 1:// VGA
			// Screen buffers. Must be the same size as the VGA resolution
			video.screen_width = VGA_RESOLUTION_WIDTH;
			video.screen_height = VGA_RESOLUTION_HEIGHT;
			VIDEO_ScreenBufferToVRAM = VGA_ScreenMainBufferToVRAM;
			VIDEO_VSync = VGA_VSync;
			break;
		case 2:// CGA
			// Screen buffers. Must be the same size as the CGA resolution
			video.screen_width = CGA_RESOLUTION_WIDTH;
			video.screen_height = CGA_RESOLUTION_HEIGHT;
			VIDEO_ScreenBufferToVRAM = CGA_ScreenMainBufferToVRAM;
			VIDEO_VSync = CGA_VSync;
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_Init function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_FILE);
			break;
	}

	// Reserve RAM memory for screen buffers
	video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK] = MM_PushChunk(video.screen_width * video.screen_height, CT_VIDEO_BUFFER);
	video.screen_buffer[VIDEO_SCREEN_BUFFER_FORE] = MM_PushChunk(video.screen_width * video.screen_height, CT_VIDEO_BUFFER);
	video.screen_buffer[VIDEO_SCREEN_BUFFER_MASK] = MM_PushChunk(video.screen_width * video.screen_height, CT_VIDEO_BUFFER);

	// Reserve RAM memory for screen map chunks
	video.screen_chunks_buffer[MAP_BACKGROUND_LAYER] = MM_PushChunk(32 * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);
	video.screen_chunks_buffer[MAP_FOREGROUND_LAYER] = MM_PushChunk(32 * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);
	video.screen_chunks_buffer[MAP_MASK_LAYER] = MM_PushChunk(32 * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);

	// Map buffers
	video.map_buffer_width = VIDEO_MAP_BUFFER_WIDTH;
	video.map_buffer_height = VIDEO_MAP_BUFFER_HEIGHT;
	// Reserve RAM memory for map buffers
	video.map_buffer[MAP_BACKGROUND_LAYER] = MM_PushChunk(VIDEO_MAP_BUFFER_WIDTH * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);
	video.map_buffer[MAP_FOREGROUND_LAYER] = MM_PushChunk(VIDEO_MAP_BUFFER_WIDTH * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);
	video.map_buffer[MAP_MASK_LAYER] = MM_PushChunk(VIDEO_MAP_BUFFER_WIDTH * VIDEO_MAP_BUFFER_HEIGHT, CT_VIDEO_BUFFER);
}

/** VIDEO :: Shutdown video system
 */
void VIDEO_Shutdown(void) {
	MM_PopChunks(CT_VIDEO_BUFFER);
}

/** VIDEO :: Vertical sync
 *  - Depending on the video mode, call the correct function
 */
void (*VIDEO_VSync)(void);

/** VIDEO :: Text image on Video memory
 *  - Loads a binary file and print it directly on video memory
 */
/** Load and display specified text-mode screen */
void VIDEO_BinaryImageToVRAM(const char *dat_name, const char *asset_name) {
	system("cls");
	FILE_LoadBinaryImage(dat_name, asset_name, EGA);
}

/** VIDEO :: PCX Image on Video memory
 *  - Loads a pcx image file and print it directly on video memory
 */
/** Load and display specified text-mode screen */
void VIDEO_PCXImageToVRAM(const char *dat_name, const char *asset_name, int size) {
	word width, height;

	byte *data_loaded = MM_PushChunk(size, CT_TEMPORARY);
	FILE_LoadPCXImage(dat_name, asset_name, data_loaded, size, gfxPaletteLoaded, &width, &height);

	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			EGA_ImageToVRAM(data_loaded, width, height);
			break;
		case 1:// VGA
			VGA_ImageToVRAM(data_loaded, width, height);
			break;
		case 2:// CGA
			CGA_ImageToVRAM(data_loaded, width, height);
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_PCXImageToVRAM function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}

	MM_PopChunks(CT_TEMPORARY);
}

/** VIDEO :: Draws screen buffer on Video memory
 *  - Depending on the video mode, call the correct function
 */
void (*VIDEO_ScreenBufferToVRAM)(void);

/** VIDEO :: Fills screen buffer with the data on the current cam position
 */
void VIDEO_MapBufferToScreenBuffer(void) {
	int length1, length2;
	byte *s, *d;
	int scanline = 0;
	int scanline_counter;

	length1 = video.map_buffer_width - camera.pos_abs_x;
	if (length1 >= camera.visible_width) {
		length1 = camera.visible_width;
		length2 = 0;
	} else {
		length2 = camera.visible_width - length1;
	}

	/////// BACKGROUND ///////////////

	s = &video.map_buffer[MAP_BACKGROUND_LAYER][(camera.pos_abs_y * video.map_buffer_width) + camera.pos_abs_x];
	d = video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK];

	scanline_counter = 0;
	scanline = camera.pos_abs_y;
	while (scanline_counter < camera.visible_height) {
		memcpy(d, s, length1);
		s += video.map_buffer_width;
		d += camera.visible_width;
		scanline_counter++;
		scanline++;
		if (scanline == video.map_buffer_height) {
			scanline = 0;
			s = &video.map_buffer[MAP_BACKGROUND_LAYER][camera.pos_abs_x];
		}
	}

	s = &video.map_buffer[MAP_BACKGROUND_LAYER][(camera.pos_abs_y * video.map_buffer_width)];
	d = video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK] + length1;

	scanline_counter = 0;
	scanline = camera.pos_abs_y;
	while (scanline_counter < camera.visible_height) {
		memcpy(d, s, length2);
		s += video.map_buffer_width;
		d += camera.visible_width;
		scanline_counter++;
		scanline++;
		if (scanline == video.map_buffer_height) {
			scanline = 0;
			s = video.map_buffer[MAP_BACKGROUND_LAYER];
		}
	}

	/////// FOREGROUND ///////////////
	// Not needed

	/////// MASK ///////////////

	s = &video.map_buffer[MAP_MASK_LAYER][(camera.pos_abs_y * video.map_buffer_width) + camera.pos_abs_x];
	d = video.screen_buffer[VIDEO_SCREEN_BUFFER_MASK];

	scanline_counter = 0;
	scanline = camera.pos_abs_y;
	while (scanline_counter < camera.visible_height) {
		memcpy(d, s, length1);
		s += video.map_buffer_width;
		d += camera.visible_width;
		scanline_counter++;
		scanline++;
		if (scanline == video.map_buffer_height) {
			scanline = 0;
			s = &video.map_buffer[MAP_MASK_LAYER][camera.pos_abs_x];
		}
	}

	s = &video.map_buffer[MAP_MASK_LAYER][(camera.pos_abs_y * video.map_buffer_width)];
	d = video.screen_buffer[VIDEO_SCREEN_BUFFER_MASK] + length1;

	scanline_counter = 0;
	scanline = camera.pos_abs_y;
	while (scanline_counter < camera.visible_height) {
		memcpy(d, s, length2);
		s += video.map_buffer_width;
		d += camera.visible_width;
		scanline_counter++;
		scanline++;
		if (scanline == video.map_buffer_height) {
			scanline = 0;
			s = video.map_buffer[MAP_MASK_LAYER];
		}
	}
}

void VIDEO_MapLayerToScreenBuffer_Parallax(CameraParallax cam, int layer, int scanline_start, int scanline_end) {
	int i, j, k;
	int src_index, dst_index;
	int tile_index;
	int break_point_x;
	int length1, length2;
	byte *s, *d;
	int tile;
	int tile_counter;
	int solid_counter, transparent_counter, no_print_counter;
	int print_command[32];
	int print_command_index;

	break_point_x = cam.pos_abs_x;

	if ((CAM_VISIBLE_WIDTH + break_point_x) <= video.map_buffer_width) {
		length1 = CAM_VISIBLE_WIDTH;
		length2 = CAM_VISIBLE_WIDTH - length1;
	} else {
		length1 = CAM_VISIBLE_WIDTH - ((CAM_VISIBLE_WIDTH + break_point_x) - video.map_buffer_width);
		length2 = CAM_VISIBLE_WIDTH - length1;
	}

	// Get scanline print command
	for (i = scanline_start; i < scanline_end; i++) {
		tile_counter = 0;
		print_command_index = 0;
		transparent_counter = 0;
		solid_counter = 0;
		no_print_counter = 0;

		while (tile_counter < (length1 >> 4)) {
			tile_index = ((i >> 4) * (map.width)) + (cam.pos_x >> 4) + tile_counter;
			tile = map.layer[layer][tile_index];
			if (tile_counter == 0) {
				print_command[print_command_index] = 1;
				print_command_index++;
				print_command[print_command_index] = 16 - cam.tile_offset_x;
				print_command_index++;
			}
			if ((tile_counter > 0) && (tile_counter < (length1 - 2))) {
				switch ((tile >> 14) & 0x03) {
					case 0:// solid
						if (transparent_counter != 0) {
							print_command[print_command_index] = 1;
							print_command_index++;
							print_command[print_command_index] = transparent_counter;
							print_command_index++;
							transparent_counter = 0;
						}
						if (no_print_counter != 0) {
							print_command[print_command_index] = 2;
							print_command_index++;
							print_command[print_command_index] = no_print_counter;
							print_command_index++;
							no_print_counter = 0;
						}
						solid_counter += 16;
						break;
					case 1:// transparent
						if (solid_counter != 0) {
							print_command[print_command_index] = 0;
							print_command_index++;
							print_command[print_command_index] = solid_counter;
							print_command_index++;
							solid_counter = 0;
						}
						if (no_print_counter != 0) {
							print_command[print_command_index] = 2;
							print_command_index++;
							print_command[print_command_index] = no_print_counter;
							print_command_index++;
							no_print_counter = 0;
						}
						transparent_counter += 16;
						break;
					case 2:// no print
						if (transparent_counter != 0) {
							print_command[print_command_index] = 1;
							print_command_index++;
							print_command[print_command_index] = transparent_counter;
							print_command_index++;
							transparent_counter = 0;
						}
						if (solid_counter != 0) {
							print_command[print_command_index] = 0;
							print_command_index++;
							print_command[print_command_index] = solid_counter;
							print_command_index++;
							solid_counter = 0;
						}
						no_print_counter += 16;
						break;
					case 3:// no print neither
						Error("Tile type not defined ", "", "", tile);
						break;
					default:
						Error("Unknown map code", "", "", (tile >> 14) & 0x03);
						break;
				}
			}
			if (tile_counter == ((length1 >> 4) - 1)) {
				print_command[print_command_index] = 1;
				print_command_index++;
				print_command[print_command_index] = cam.tile_offset_x;
				print_command_index++;
			}
			tile_counter++;
		}
		if (solid_counter != 0) {
			print_command[print_command_index] = 0;
			print_command_index++;
			print_command[print_command_index] = solid_counter;
			print_command_index++;
			solid_counter = 0;
		}
		if (no_print_counter != 0) {
			print_command[print_command_index] = 2;
			print_command_index++;
			print_command[print_command_index] = no_print_counter;
			print_command_index++;
			no_print_counter = 0;
		}
		if (transparent_counter != 0) {
			print_command[print_command_index] = 1;
			print_command_index++;
			print_command[print_command_index] = transparent_counter;
			print_command_index++;
			transparent_counter = 0;
		}
		print_command[print_command_index] = 99;// end of commands

		// Print
		src_index = (i * video.map_buffer_width) + break_point_x;
		dst_index = i * CAM_VISIBLE_WIDTH;

		s = &video.map_buffer[layer][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		print_command_index = 0;
		while (print_command[print_command_index] != 99) {
			switch (print_command[print_command_index]) {
				case 0:// solid
					memcpy(d, s, print_command[print_command_index + 1] << 4);
					s += print_command[print_command_index + 1];
					d += print_command[print_command_index + 1];
					break;
				case 1:// transparent
					for (k = 0; k < print_command[print_command_index + 1]; k++) {
						if (*s) {
							*d++ = *s++;
						} else {
							s++;
							d++;
						}
					}
					break;
				case 2:// no print
					s += print_command[print_command_index + 1];
					d += print_command[print_command_index + 1];
					break;
				case 99:// end
					break;
			}
			print_command_index += 2;
		}

		// second side
		/*tile_counter = 0;
		print_command_index = 0;
		transparent_counter = 0;
		solid_counter = 0;
		no_print_counter = 0;

		while (tile_counter < (length2 >> 4)) {
			tile_index = ((i >> 4) * (map.width)) + ((cam.pos_x + length1) >> 4) + tile_counter;
			tile = map.layer[layer][tile_index];
			tile_counter++;
			switch ((tile >> 14) & 0x03) {
				case 0:// solid
					if (transparent_counter != 0) {
						print_command[print_command_index] = 0;
						print_command_index++;
						print_command[print_command_index] = transparent_counter;
						print_command_index++;
					}
					if (no_print_counter != 0) {
						print_command[print_command_index] = 2;
						print_command_index++;
						print_command[print_command_index] = no_print_counter;
						print_command_index++;
					}
					if (solid_counter == 0) {
						print_command[print_command_index] = 0;
						print_command_index++;
					}

					solid_counter++;
					break;
				case 1:// transparent
					if (solid_counter != 0) {
						print_command[print_command_index] = 0;
						print_command_index++;
						print_command[print_command_index] = solid_counter;
						print_command_index++;
					}
					if (no_print_counter != 0) {
						print_command[print_command_index] = 2;
						print_command_index++;
						print_command[print_command_index] = no_print_counter;
						print_command_index++;
					}
					if (transparent_counter == 0) {
						print_command[print_command_index] = 1;
						print_command_index++;
					}
					transparent_counter++;
					break;
				case 2:// no print
					if (transparent_counter != 0) {
						print_command[print_command_index] = 1;
						print_command_index++;
						print_command[print_command_index] = transparent_counter;
						print_command_index++;
					}
					if (solid_counter != 0) {
						print_command[print_command_index] = 0;
						print_command_index++;
						print_command[print_command_index] = solid_counter;
						print_command_index++;
					}
					if (no_print_counter == 0) {
						print_command[print_command_index] = 2;
						print_command_index++;
					}
					no_print_counter++;
					break;
				case 3:// no print neither
					if (transparent_counter != 0) {
						print_command[print_command_index] = 1;
						print_command_index++;
						print_command[print_command_index] = transparent_counter;
						print_command_index++;
					}
					if (solid_counter != 0) {
						print_command[print_command_index] = 0;
						print_command_index++;
						print_command[print_command_index] = solid_counter;
						print_command_index++;
					}
					if (no_print_counter == 0) {
						print_command[print_command_index] = 2;
						print_command_index++;
					}
					no_print_counter++;
					break;
				default:
					Error("Unknown map code", "", "", (tile >> 14) & 0x03);
					break;
			}
		}
		if (solid_counter != 0) {
			print_command[print_command_index] = solid_counter;
			print_command_index++;
		}
		if (no_print_counter != 0) {
			print_command[print_command_index] = no_print_counter;
			print_command_index++;
		}
		if (transparent_counter != 0) {
			print_command[print_command_index] = transparent_counter;
			print_command_index++;
		}
		print_command[print_command_index] = 99;// end of commands

		// Print
		src_index = (i * video.map_buffer_width);
		dst_index = (i * CAM_VISIBLE_WIDTH) + length1;

		s = &video.map_buffer[layer][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		print_command_index = 0;
		while (print_command[print_command_index] != 99) {
			switch (print_command[print_command_index]) {
				case 0:// solid
					memcpy(d, s, print_command[print_command_index + 1] << 4);
					s += print_command[print_command_index + 1] << 4;
					d += print_command[print_command_index + 1] << 4;
					break;
				case 1:// transparent
					for (k = 0; k < print_command[print_command_index + 1] << 4; k++) {
						if (*s) {
							*d++ = *s++;
						} else {
							s++;
							d++;
						}
					}
					break;
				case 2:// no print
					s += print_command[print_command_index + 1] << 4;
					d += print_command[print_command_index + 1] << 4;
					break;
				case 99:// end
					break;
			}
			print_command_index += 2;
		}*/

		/*

		// Second side
		src_index = (i * video.map_buffer_width);
		dst_index = (i * CAM_VISIBLE_WIDTH) + length1;

		s = &video.map_buffer[layer][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		memcpy(d, s, length2);

		break_point_y++;
		if (break_point_y >= video.map_buffer_height) {
			break_point_y = 0;
		}*/
	}
}

/** VIDEO :: Fills screen buffer with the data on the current cam position
 */
void VIDEO_MapBufferToScreenBuffer_Paralax(void) {
	int i, j, k;
	int src_index, dst_index;
	int tile_index;
	int break_point_x, break_point_y;
	int length1, length2;
	byte *s, *d;
	int tile;
	int tile_counter;

	// 1st. Layer
	/*break_point_x = cam_parallax[0].pos_abs_x;
	break_point_y = cam_parallax[0].pos_abs_y;

	if ((CAM_VISIBLE_WIDTH + break_point_x) <= video.map_buffer_width) {
		length1 = CAM_VISIBLE_WIDTH;
		length2 = CAM_VISIBLE_WIDTH - length1;
	} else {
		length1 = CAM_VISIBLE_WIDTH - ((CAM_VISIBLE_WIDTH + break_point_x) - video.map_buffer_width);
		length2 = CAM_VISIBLE_WIDTH - length1;
	}

	for (i = 0; i < camera.visible_height; i++) {

		// First side
		src_index = (i * video.map_buffer_width) + break_point_x;
		dst_index = i * CAM_VISIBLE_WIDTH;

		s = &video.map_buffer[MAP_BACKGROUND_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		memcpy(d, s, length1);

		// Second side
		src_index = (i * video.map_buffer_width);
		dst_index = (i * CAM_VISIBLE_WIDTH) + length1;

		s = &video.map_buffer[MAP_BACKGROUND_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		memcpy(d, s, length2);

		break_point_y++;
		if (break_point_y >= video.map_buffer_height) {
			break_point_y = 0;
		}
	}*/

	VIDEO_MapLayerToScreenBuffer_Parallax(cam_parallax[0], MAP_BACKGROUND_LAYER, 0, 208);
	VIDEO_MapLayerToScreenBuffer_Parallax(cam_parallax[1], MAP_FOREGROUND_LAYER, 16, 160);

	return;

	// 2nd. Layer
	break_point_x = cam_parallax[1].pos_abs_x;
	break_point_y = cam_parallax[1].pos_abs_y;

	if ((CAM_VISIBLE_WIDTH + break_point_x) <= video.map_buffer_width) {
		length1 = CAM_VISIBLE_WIDTH;
		length2 = CAM_VISIBLE_WIDTH - length1;
	} else {
		length1 = CAM_VISIBLE_WIDTH - ((CAM_VISIBLE_WIDTH + break_point_x) - video.map_buffer_width);
		length2 = CAM_VISIBLE_WIDTH - length1;
	}

	for (i = 16; i < 160; i++) {

		// First side
		src_index = (i * video.map_buffer_width) + break_point_x;
		dst_index = i * CAM_VISIBLE_WIDTH;

		s = &video.map_buffer[MAP_FOREGROUND_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		tile_counter = 0;

		// first tile
		if (cam_parallax[1].tile_offset_x > 0) {
			for (k = 0; k < cam_parallax[1].tile_offset_x; k++) {
				if (*s) {
					*d++ = *s++;
				} else {
					s++;
					d++;
				}
			}
		} else {
			for (k = 0; k < 16; k++) {
				if (*s) {
					*d++ = *s++;
				} else {
					s++;
					d++;
				}
			}
		}
		tile_counter++;

		// tiles 2..n
		while (tile_counter < (length1 >> 4) - 1) {
			tile_index = ((i >> 4) * (map.width)) + (cam_parallax[1].pos_x >> 4) + tile_counter;
			tile = map.layer[MAP_FOREGROUND_LAYER][tile_index];
			tile = 1;
			switch ((tile >> 14) & 0x03) {
				case 0:// solid
					memcpy(d, s, 16);
					d += 16;
					s += 16;
					break;
				case 1:// transparent
					for (k = 0; k < 16; k++) {
						if (*s) {
							*d++ = *s++;
						} else {
							s++;
							d++;
						}
					}
					break;
				case 2:// no print
					d += 16;
					s += 16;
					break;
				default:
					d += 16;
					s += 16;
					break;
			}
			tile_counter++;
		}

		// last tile
		if (cam_parallax[0].tile_offset_x > 0) {
			for (k = 0; k < cam_parallax[1].tile_offset_x; k++) {
				if (*s) {
					*d++ = *s++;
				} else {
					s++;
					d++;
				}
			}
		} else {
			for (k = 0; k < 16; k++) {
				if (*s) {
					*d++ = *s++;
				} else {
					s++;
					d++;
				}
			}
		}
		tile_counter++;

		// Second side
		src_index = (i * video.map_buffer_width);
		dst_index = (i * CAM_VISIBLE_WIDTH) + length1;

		s = &video.map_buffer[MAP_FOREGROUND_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		// Check tile type
		for (j = 0; j < (length2 >> 4); j++) {
			// first tile
			if (cam_parallax[0].tile_offset_x > 0) {
				for (k = 0; k < cam_parallax[1].tile_offset_x; k++) {
					if (*s) {
						*d++ = *s++;
					} else {
						s++;
						d++;
					}
				}
				tile_counter++;
			} else {
				for (k = 0; k < 16; k++) {
					if (*s) {
						*d++ = *s++;
					} else {
						s++;
						d++;
					}
				}
				tile_counter++;
			}

			while (tile_counter < (length1 >> 4) - 1) {
				tile_index = ((i >> 4) * (map.width)) + (cam_parallax[1].pos_x >> 4) + tile_counter;

				switch ((tile >> 14) & 0x03) {
					case 0:// solid
						//memcpy(d, s, 16);
						d += 16;
						s += 16;
						break;
					case 1:// transparent
						for (k = 0; k < 16; k++) {
							if (*s) {
								*d++ = *s++;
							} else {
								s++;
								d++;
							}
						}
						break;
					case 2:// no print
						d += 16;
						s += 16;
						break;
					default:
						d += 16;
						s += 16;
						break;
				}

				tile_counter++;
			}

			// last tile
			if (cam_parallax[0].tile_offset_x > 0) {
				for (k = 0; k < cam_parallax[1].tile_offset_x; k++) {
					if (*s) {
						*d++ = *s++;
					} else {
						s++;
						d++;
					}
				}
				tile_counter++;
			} else {
				for (k = 0; k < 16; k++) {
					if (*s) {
						*d++ = *s++;
					} else {
						s++;
						d++;
					}
				}
				tile_counter++;
			}
		}

		/*for (j = 0; j < length2; j++) {
			if (*s) {
				*d++ = *s++;
			} else {
				s++;
				d++;
			}
		}*/

		break_point_y++;
		if (break_point_y >= video.map_buffer_height) {
			break_point_y = 0;
		}
	}

	// 3rd. Layer
	break_point_x = cam_parallax[2].pos_abs_x;
	break_point_y = cam_parallax[2].pos_abs_y;

	if ((CAM_VISIBLE_WIDTH + break_point_x) <= video.map_buffer_width) {
		length1 = CAM_VISIBLE_WIDTH;
		length2 = CAM_VISIBLE_WIDTH - length1;
	} else {
		length1 = CAM_VISIBLE_WIDTH - ((CAM_VISIBLE_WIDTH + break_point_x) - video.map_buffer_width);
		length2 = CAM_VISIBLE_WIDTH - length1;
	}

	for (i = 16; i < 160; i++) {

		// First side
		src_index = (i * video.map_buffer_width) + break_point_x;
		dst_index = i * CAM_VISIBLE_WIDTH;

		s = &video.map_buffer[MAP_MASK_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		for (j = 0; j < length1; j++) {
			if (*s) {
				*d++ = *s++;
			} else {
				s++;
				d++;
			}
		}

		// Second side
		src_index = (i * video.map_buffer_width);
		dst_index = (i * CAM_VISIBLE_WIDTH) + length1;

		s = &video.map_buffer[MAP_MASK_LAYER][src_index];
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		for (j = 0; j < length2; j++) {
			if (*s) {
				*d++ = *s++;
			} else {
				s++;
				d++;
			}
		}

		break_point_y++;
		if (break_point_y >= video.map_buffer_height) {
			break_point_y = 0;
		}
	}
}

/** VIDEO :: PCX Image on Video memory
 *  - Loads a pcx image file and print it directly on video memory
 */
/** Load and display specified text-mode screen */
void VIDEO_PCXImageToScreenBuffer(const char *dat_name, const char *asset_name, int size) {
	int i, src_index, dst_index;
	word width, height;

	byte *data_loaded = MM_PushChunk(size, CT_TEMPORARY);
	FILE_LoadPCXImage(dat_name, asset_name, data_loaded, size, gfxPaletteLoaded, &width, &height);

	//disable();
	src_index = 0;
	dst_index = 0;

	for (i = 0; i < height; i++) {
		memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &data_loaded[src_index], width);
		src_index += width;
		dst_index += CAM_VISIBLE_WIDTH;
	}


	MM_PopChunks(CT_TEMPORARY);
}

void VIDEO_DrawPixelOnScreenBuffer(int x, int y, byte color) {
	int dst_index;

	dst_index = y * video.screen_width + x;
	video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index] = color;
}

/** VIDEO :: Fills screen buffer with the data on the sent data buffer
 *  - x: Position x on the current screen
 *  - y: Position y on the current screen
 *  - width: Width of the status panel
 *  - height: Height of the status panel
 */
void VIDEO_PanelToScreenBuffer(StatusPanel *panel) {
	int i;
	int src_index, dst_index;
	byte *s, *d;
	byte color, count;
	bool end_of_scanline;

	if (panel->shown) {

		// Draw portait
		src_index = gfx_sprite_graphics_stack[panel->portait_graphics_id].frame_offset[panel->portait_frame];
		s = &gfx_sprite_graphics_stack[panel->portait_graphics_id].buffer[src_index];
		dst_index = ((panel->pos_y + panel->portait_y) * video.screen_width) + (panel->pos_x + panel->portait_x);
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		for (i = 0; i < gfx_sprite_graphics_stack[panel->portait_graphics_id].height_px; i++) {
			end_of_scanline = false;
			while (!end_of_scanline) {
				color = *s++;
				switch (color) {
					case 1:// start of transparent bunch
						count = *s++;
						d += count;
						break;
					case 0:// end of scanline
						d += video.screen_width - gfx_sprite_graphics_stack[panel->portait_graphics_id].width_px;
						end_of_scanline = true;
						break;
					default:// just a color!
						*d++ = color;
						break;
				}
			}
		}

		// Draw lifebar
		src_index = 0;
		s = &gfx_sprite_graphics_stack[panel->lifebar_graphics_id].buffer[src_index];
		dst_index = ((panel->pos_y + panel->lifebar_y) * video.screen_width) + (panel->pos_x + panel->lifebar_x);
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		for (i = 0; i < gfx_sprite_graphics_stack[panel->lifebar_graphics_id].height_px; i++) {
			end_of_scanline = false;
			while (!end_of_scanline) {
				color = *s++;
				switch (color) {
					case 1:// start of transparent bunch
						count = *s++;
						d += count;
						break;
					case 0:// end of scanline
						d += video.screen_width - gfx_sprite_graphics_stack[panel->lifebar_graphics_id].width_px;
						end_of_scanline = true;
						break;
					default:// just a color!
						*d++ = color;
						break;
				}
			}
		}

		// Draw current life
		dst_index = ((panel->pos_y + panel->lifebar_y + 1) * video.screen_width) + (panel->pos_x + panel->lifebar_x + 1);
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

		for (i = 0; i < 45; i++) {
			if (i < panel->current_life)
				*d++ = 228;
			if (i > panel->current_life && i < panel->old_life)
				*d++ = 253;
			if (i > panel->old_life)
				*d++ = 252;
		}
		src_index = ((panel->pos_y + panel->lifebar_y + 1) * video.screen_width) + (panel->pos_x + panel->lifebar_x + 1);
		s = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index];
		dst_index = ((panel->pos_y + panel->lifebar_y + 2) * video.screen_width) + (panel->pos_x + panel->lifebar_x + 1);
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];
		memcpy(d, s, 45);
		src_index = ((panel->pos_y + panel->lifebar_y + 1) * video.screen_width) + (panel->pos_x + panel->lifebar_x + 1);
		s = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index];
		dst_index = ((panel->pos_y + panel->lifebar_y + 3) * video.screen_width) + (panel->pos_x + panel->lifebar_x + 1);
		d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];
		memcpy(d, s, 45);

		// Update life counter
		if (panel->old_life > panel->current_life) {
			panel->life_speed_counter++;
			if (panel->life_speed_counter > panel->life_speed) {
				panel->life_speed_counter = 0;
				panel->old_life--;
			}
		}

		// Update time
		if (panel->timeout != 0) {
			panel->current_time++;

			if (panel->current_time >= panel->timeout) {
				panel->shown = false;
				panel->current_time = 0;
			}
		}
	}
}

/** VIDEO :: Draw char on screen buffer
 *  - x: Video buffer x pixel position
 *  - y: Video buffer y pixel position
 *  - width: Character width in pixels
 *  - height: Character height in pixels
 *  - data: character pixels buffer
 */
void VIDEO_CharToScreenBuffer(int x, int y, word width, word height, byte transparent_color, byte *data) {
	register int src_index, dst_index;
	register int i, j;
	register byte color;

	dst_index = y * camera.visible_width + x;
	src_index = 0;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			color = data[src_index];
			if (color != transparent_color) { video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index] = color; }
			src_index++;
			dst_index++;
		}
		dst_index += (camera.visible_width - width);
	}
}

/** VIDEO :: Print string on screen buffer
 *  - x: Position x on the current screen
 *  - y: Position y on the current screen
 *  - length: String length
 *  - string: String data buffer
 */
void VIDEO_StringToScreenBuffer(int x, int y, int length, unsigned char *string) {
	register int i;
	register int src_index;
	register int pos_x, pos_y;
	register byte chr;

	pos_x = x;
	pos_y = y;

	for (i = 0; i < length; i++) {
		chr = string[i];
		switch (chr) {
			case 0xD1:// Ñ
				chr = 0x40;
				break;
			case 0xA1:// ¡
				chr = 0x5F;
				break;
			case 0xBF:// ¿
				chr = 0x5C;
				break;
			default:
				break;
		}

		src_index = (chr - 32) << 6;
		VIDEO_CharToScreenBuffer(pos_x, pos_y, 8, 8, VIDEO_TRANSPARENT_COLOR, &gfxFont[src_index]);
		pos_x += 8;
	}
}

/** VIDEO :: Draw sprite on screen buffer
 *  - x_px = x position in pixels
 *  - y_px = y position  in pixels
 */
void VIDEO_DrawSpriteToScreenBufferRLE(SpriteGraphic *spr_graphics, Sprite spr) {
	int i;
	int src_index, dst_index;
	int dst_x_index, dst_y_index;
	byte *s, *d, *m;
	byte color, count;
	bool end_of_scanline;
	bool hide_color;
	int horizontal_pixel_count;
	int break_point_x_right, break_point_x_left;

	if ((spr.screen_pos_x + spr_graphics->width_px) > video.screen_width) {
		break_point_x_right = spr_graphics->width_px - ((spr.screen_pos_x + spr_graphics->width_px) - video.screen_width);
	} else {
		break_point_x_right = spr_graphics->width_px;
	}

	if (spr.screen_pos_x < 0) {
		break_point_x_left = abs(spr.screen_pos_x);
	} else {
		break_point_x_left = 0;
	}

	// Calculate pointers
	if (!spr.animation_inverted) {
		src_index = spr_graphics->frame_offset[spr.current_frame - 1 + spr.animation_base_frame];
		dst_y_index = spr.screen_pos_y;
		dst_x_index = spr.screen_pos_x;
		dst_index = (dst_y_index * video.screen_width) + dst_x_index;
	} else {
		src_index = spr_graphics->frame_offset[spr.current_frame - 1 + spr.animation_base_frame] + spr_graphics->row_offset[0];
		dst_y_index = spr.screen_pos_y;
		dst_x_index = spr.screen_pos_x + spr_graphics->width_px;
		dst_index = (dst_y_index * video.screen_width) + dst_x_index;
	}

	s = &spr_graphics->buffer[src_index];
	m = &video.screen_buffer[VIDEO_SCREEN_BUFFER_MASK][dst_index];
	d = &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index];

	// Print sprite
	if (!spr.animation_inverted) {
		for (i = 0; i < spr_graphics->height_px; i++) {
			end_of_scanline = false;
			horizontal_pixel_count = 0;
			while (!end_of_scanline) {
				hide_color = false;
				if (horizontal_pixel_count >= break_point_x_right) {
					hide_color = true;
				}
				if (horizontal_pixel_count < break_point_x_left) {
					hide_color = true;
				}

				// Print sprite
				color = *s++;
				switch (color) {
					case 1:// start of transparent bunch
						count = *s++;
						d += count;
						m += count;
						horizontal_pixel_count += count;
						break;
					case 0:// end of scanline
						d += video.screen_width - spr_graphics->width_px;
						m += video.screen_width - spr_graphics->width_px;
						end_of_scanline = true;
						break;
					default:// just a color!
						// Print unmasked pixels
						if (((*m++ == 0) || (spr.unmasked)) && !spr.invisible) {
							if (spr.blink) {
								if (!hide_color) { *d++ = spr_graphics->hit_color; }// hit color
								else {
									d++;
								}
							} else {
								if (!hide_color) {
									*d++ = color;
								} else {
									d++;
								}
							}
						} else {
							d++;
						}
						horizontal_pixel_count++;
						break;
				}
			}
		}
	} else {
		// print sprite
		for (i = 0; i < spr_graphics->height_px; i++) {
			end_of_scanline = false;
			horizontal_pixel_count = spr_graphics->width_px;
			while (!end_of_scanline) {
				hide_color = false;
				if (horizontal_pixel_count >= break_point_x_right) {
					hide_color = true;
				}
				if (horizontal_pixel_count < break_point_x_left) {
					hide_color = true;
				}

				// Print sprite
				color = *s++;
				switch (color) {
					case 1:// start of transparent bunch
						count = *s++;
						d -= count;
						m -= count;
						horizontal_pixel_count -= count;
						break;
					case 0:// end of scanline
						d += video.screen_width + spr_graphics->width_px;
						m += video.screen_width + spr_graphics->width_px;
						end_of_scanline = true;
						break;
					default:// just a color!
						// Print unmasked pixels
						if (((*m-- == 0) || (spr.unmasked)) && !spr.invisible) {
							if (spr.blink) { *d-- = spr_graphics->hit_color; }// hit color
							else {
								if (!hide_color) {
									*d-- = color;
								} else {
									d--;
								}
							}
						} else {
							d--;
						}
						horizontal_pixel_count--;
						break;
				}
			}
		}
	}
}

/** VIDEO :: Set palette
 */
void VIDEO_SetPalette(byte *palette) {
	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			VGA_SetPalette(palette);
			break;
		case 1:// VGA
			VGA_SetPalette(palette);
			break;
		case 2:// CGA
			VGA_SetPalette(palette);
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_SetPalette function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}
}

/** VIDEO :: Clear palette
 */
void VIDEO_ClearPalette(void) {
	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			VGA_ClearPalette();
			break;
		case 1:// VGA
			VGA_ClearPalette();
			break;
		case 2:// CGA
			VGA_ClearPalette();
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_SetPalette function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}
}


/** VIDEO :: Clear screen
 */
void VIDEO_ClearScreen(void) {
	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			VGA_ClearScreen();
			break;
		case 1:// VGA
			VGA_ClearScreen();
			break;
		case 2:// CGA
			VGA_ClearScreen();
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_ClearScreen function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}
}

/** VIDEO :: Asyncronous fade in
 */
bool VIDEO_FadeIn_Async(int speed, int *step) {
	return VGA_FadeIn_Async(speed, step);
}

/** VIDEO :: Asyncronous fade out
 */
bool VIDEO_FadeOut_Async(void) {
	VGA_FadeOut(1);
	return true;
}

/** VIDEO :: Syncronous fade in
 */
void VIDEO_FadeIn(int speed) {
	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			VGA_FadeIn(speed);
			break;
		case 1:// VGA
			VGA_FadeIn(speed);
			break;
		case 2:// CGA
			VGA_FadeIn(speed);
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_FadeIn function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}
}

/** VIDEO :: Syncronous fade out
 */
void VIDEO_FadeOut(int speed) {
	// Depending on the video mode, call the correct function
	switch (video.selected_mode) {
		case 0:// EGA
			VGA_FadeOut(speed);
			break;
		case 1:// VGA
			VGA_FadeOut(speed);
			break;
		case 2:// CGA
			VGA_FadeOut(speed);
			break;
		default:
			sprintf(engine.system_error_message1, "VIDEO_FadeOut function error");
			sprintf(engine.system_error_message2, "Unknown video mode selected");
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_VIDEO);
			break;
	}
}

void VIDEO_FadeOutPause(int atenuation) {
	int i;
	for (i = 0; i < 219 * 3; i++) {
		if ((gfxPaletteShown[i] - atenuation) > 0) {
			gfxPaletteShown[i] -= atenuation;
		} else {
			gfxPaletteShown[i] = 0;
		}
		if ((gfxPaletteShown[i + 1] - atenuation) > 0) {
			gfxPaletteShown[i + 1] -= atenuation;
		} else {
			gfxPaletteShown[i + 1] = 0;
		}
		if ((gfxPaletteShown[i + 2] - atenuation) > 0) {
			gfxPaletteShown[i + 2] -= atenuation;
		} else {
			gfxPaletteShown[i + 2] = 0;
		}
	}
	VGA_SetPalette(gfxPaletteShown);
}
