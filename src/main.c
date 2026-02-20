/**********************************************
* main.c
* Entry point for the game itself
* (c) 2025-2026 by J.Martin
***********************************************/

#include "engine/engine.h"
#include "engine/types/types.h"
#include <stdio.h>

//#include <bios.h>
//#include <dpmi.h>
//#include <sys/nearptr.h>
//#include <stdlib.h>
//#include <stdio.h>

#define GDB_IMPLEMENTATION
#include "gdbstub.h"


// Intro
static void Intro(void) {
	bool intro_end, sequence_end;
	bool faded_in;
	int fade_in_step;
	int intro_step, sequence_step;
	int dialog_step, dialog_delay_counter;
	int palette_step;
	int char_number;
	int delay, delay_counter;
	unsigned char src_string[40];
	unsigned char dst_string[40];

	byte *image_loaded[4];
	int image_index;
	word width, height;
	int i, horizontal_scroll, scanline_scroll, src_index, dst_index, src_img_index, dst_img_index;

	intro_step = 0;
	intro_end = false;

	while (!intro_end) {
		VIDEO_VSync();
		switch (intro_step) {
			case 0:// Clear screen
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				intro_step = 10;
				break;
			case 10:// Logo 1
				GFX_LoadPalette("PALETTES.DAT", "LOGO.PCX", 256);
				VIDEO_PCXImageToScreenBuffer("LOGO.DAT", "LDOSCLUB.PCX", 320 * 200);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(1);
				intro_step = 11;
				break;
			case 11:// Asyncronous fade in, wait time and fade out
				VGA_RotatePalette(48, 83, 98);
				VIDEO_FadeOut(1);
				intro_step = 20;
				break;
			case 20:// Logo 2
				GFX_LoadPalette("PALETTES.DAT", "LOGO.PCX", 256);
				VIDEO_PCXImageToScreenBuffer("LOGO.DAT", "LCONTEST.PCX", 320 * 200);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(1);
				intro_step = 21;
				break;
			case 21:// Wait time
				AUDIO_LoadSong("MINTRO.DATA", "intro1.a2m");
				GFX_LoadFont("FONTS.DAT", "SLIM.PCX", 128 * 32);
				FILE_LoadText("DIALOG.DAT", "INTRO.TXT", 1, src_string);
				VIDEO_PCXImageToScreenBuffer("INTRO.DAT", "INTRO0F.PCX", 320 * 200);
				image_loaded[0] = MM_PushChunk(564 * 168, CT_TEMPORARY);
				image_loaded[1] = MM_PushChunk(564 * 168, CT_TEMPORARY);
				image_loaded[2] = MM_PushChunk(564 * 168, CT_TEMPORARY);
				image_loaded[3] = MM_PushChunk(564 * 168, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO01.PCX", image_loaded[0], 564 * 168, gfxPaletteLoaded, &width, &height);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO02.PCX", image_loaded[1], 564 * 168, gfxPaletteLoaded, &width, &height);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO03.PCX", image_loaded[2], 564 * 168, gfxPaletteLoaded, &width, &height);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO04.PCX", image_loaded[3], 564 * 168, gfxPaletteLoaded, &width, &height);
				VIDEO_FadeOut(1);
				intro_step = 30;
				break;
			case 30:// Load song and first image
				AUDIO_PlaySong();
				GFX_LoadPalette("PALETTES.DAT", "INTRO.PCX", 256);
				// Load first frame
				src_index = 0;
				dst_index = CAM_VISIBLE_WIDTH * 12 + 10;
				for (i = 0; i < height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_index], CAM_VISIBLE_WIDTH - 20);
					src_index += width;
					dst_index += CAM_VISIBLE_WIDTH;
				}
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(1);
				intro_step = 35;
				break;

			case 35:// Play song and show image
				dialog_step = 0;
				char_number = 0;
				palette_step = 0;
				intro_step = 40;
				break;

			case 40:// Horizontal scroll and text

				src_index = 0;
				horizontal_scroll = 10;
				image_index = 0;
				while (horizontal_scroll < width - 320) {

					delay = 0;
					src_index = horizontal_scroll;
					dst_index = CAM_VISIBLE_WIDTH * 12 + 10;

					for (i = 0; i < height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[image_index][src_index], CAM_VISIBLE_WIDTH - 20);
						src_index += width;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					horizontal_scroll++;
					if (horizontal_scroll == 30) image_index = 1;
					if (horizontal_scroll == 80) image_index = 2;
					if (horizontal_scroll == 120) image_index = 3;
					if (horizontal_scroll == 180) image_index = 2;
					if (horizontal_scroll == 210) image_index = 3;
					if (horizontal_scroll == 240) image_index = 2;

					// Draw text
					switch (dialog_step) {
						case 0:
							dst_string[char_number] = src_string[char_number];
							VIDEO_StringToScreenBuffer(36, 190, char_number, dst_string);
							dialog_delay_counter++;
							if (dialog_delay_counter > 2) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(src_string)) {
								dialog_step++;
							}
							break;
						case 1:
							VIDEO_StringToScreenBuffer(36, 190, char_number, dst_string);
							delay = 5;
							dialog_step++;
							break;
						case 2:
							VIDEO_StringToScreenBuffer(36, 190, char_number, dst_string);
							delay--;
							if (delay == 0) {
								dialog_step++;
							}
						case 3:
							break;
						default:
							break;
					}

					sleep(1);

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				MM_PopChunks(CT_TEMPORARY);
				intro_step = 45;
				break;
			case 45:
				// Load image onto temporary mem
				image_loaded[0] = MM_PushChunk(320 * 200, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO1.PCX", image_loaded[0], 320 * 200, gfxPaletteLoaded, &width, &height);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				scanline_scroll = 1;
				intro_step = 46;
				break;
			case 46:// Make image appear from bottom to top
				src_img_index = 0;
				while (scanline_scroll < 200) {

					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 200; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				intro_step = 50;
				MM_PopChunks(CT_TEMPORARY);
				break;
			case 50:
				// Load image onto temporary mem
				image_loaded[0] = MM_PushChunk(118 * 61, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO2.PCX", image_loaded[0], 118 * 61, gfxPaletteLoaded, &width, &height);
				intro_step = 51;
				break;
			case 51:// Paste image into current screen
				src_index = 0;
				dst_index = CAM_VISIBLE_WIDTH * 130 + 8;
				for (i = 0; i < height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_index], width);
					src_index += width;
					dst_index += CAM_VISIBLE_WIDTH;
				}
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				MM_PopChunks(CT_TEMPORARY);
				intro_step = 55;
				break;
			case 55:
				// Load image onto temporary mem
				image_loaded[0] = MM_PushChunk(112 * 62, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO3.PCX", image_loaded[0], 112 * 62, gfxPaletteLoaded, &width, &height);
				intro_step = 56;
				break;
			case 56:
				src_index = 0;
				dst_index = CAM_VISIBLE_WIDTH * 130 + 126;
				for (i = 0; i < height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_index], width);
					src_index += width;
					dst_index += CAM_VISIBLE_WIDTH;
				}
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				MM_PopChunks(CT_TEMPORARY);
				intro_step = 60;
				break;
			case 60:
				// Load image onto temporary mem
				image_loaded[0] = MM_PushChunk(82 * 63, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO4.PCX", image_loaded[0], 82 * 63, gfxPaletteLoaded, &width, &height);
				intro_step = 61;
				break;
			case 61:
				src_index = 0;
				dst_index = CAM_VISIBLE_WIDTH * 130 + 238;
				for (i = 0; i < height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_index], width);
					src_index += width;
					dst_index += CAM_VISIBLE_WIDTH;
				}
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				MM_PopChunks(CT_TEMPORARY);
				intro_step = 65;
				break;
			case 65:
				// Load image onto temporary mem
				image_loaded[0] = MM_PushChunk(320 * 200, CT_TEMPORARY);
				FILE_LoadPCXImage("INTRO.DAT", "INTRO5.PCX", image_loaded[0], 320 * 200, gfxPaletteLoaded, &width, &height);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				scanline_scroll = 1;
				intro_step = 66;
				break;
			case 66:// Make image appear from bottom to top
				src_img_index = 0;
				while (scanline_scroll < 200) {

					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 200; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &image_loaded[0][src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				delay = 100;
				MM_PopChunks(CT_TEMPORARY);
				intro_step = 70;
				break;
			case 70:// delay
				delay--;
				if (delay == 0) {
					intro_step = 71;
				}
				break;
			case 71://Fade out
				VIDEO_FadeOut(1);
				intro_step = 80;
				break;
			case 80:// Stop music
				MM_PopChunks(CT_TEMPORARY);
				AUDIO_StopSong();
				intro_end = true;
				break;
			default:
				break;
		}
	}
}

// Main game loop
static void GameLoop(void) {
	// Load sprite graphics onto the graphics stack
	A2M_LoadFile("MSCENE1.DAT", "scene1.a2m");
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTOR2.PCX", SPRITE_GRAPHICS_ID_ACTOR2, 32, 32, 90, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTORP.PCX", SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET1.PCX", SPRITE_GRAPHICS_ID_BULLET1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1.PCX", SPRITE_GRAPHICS_ID_BARREL1, 16, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL2.PCX", SPRITE_GRAPHICS_ID_BARREL2, 32, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL2P.PCX", SPRITE_GRAPHICS_ID_BARREL2_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "FIRE1.PCX", SPRITE_GRAPHICS_ID_FIRE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "ELECTRIC1.PCX", SPRITE_GRAPHICS_ID_ELECTRIC1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "LIFEBAR.PCX", SPRITE_GRAPHICS_ID_LIFEBAR, 48, 6, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1.PCX", SPRITE_GRAPHICS_ID_ENEMY1, 32, 32, 54, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1P.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);

	// Initialize mouse cursor
	MOUSE_InitCursorSprite(SPRITE_GRAPHICS_ID_CURSOR);

	// Initialize actor
	ACTOR_Init(700, 470, SPRITE_GRAPHICS_ID_ACTOR2, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1, 86, 88);
	ACTOR_SetGun(ACTOR_GUN_SHOTGUN, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1);

	ENEMY_Init(0, 400, 500, ENEMY_FACING_LEFT, SPRITE_GRAPHICS_ID_ENEMY1, ENEMY_GUN_PISTOL, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1);

	OBJECT_LoadObject(1, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 34 << 4, 30 << 4);
	OBJECT_LoadObject(2, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 35 << 4, 30 << 4);
	OBJECT_LoadObject(3, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 36 << 4, 30 << 4);
	OBJECT_LoadObject(4, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 37 << 4, 30 << 4);
	OBJECT_LoadObject(5, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 38 << 4, 30 << 4);
	OBJECT_LoadObject(6, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 25 << 4, 30 << 4);

	MAP_LoadTileset("OFFICET.DAT", "OFFICE_BACK.PCX", &map.layer_tiles[MAP_BACKGROUND_LAYER], 320 * 416);
	MAP_LoadTileset("OFFICET.DAT", "OFFICE_FORE.PCX", &map.layer_tiles[MAP_FOREGROUND_LAYER], 128 * 128);
	MAP_LoadTileset("OFFICET.DAT", "OFFICE_MASK.PCX", &map.layer_tiles[MAP_MASK_LAYER], 128 * 128);

	MAP_LoadMap("OFFICE.DAT", 50, 40);

	CAM_SetLimits(map.width_px, map.height_px);
	CAM_SetPosition(30 << 4, 22 << 4);

	MAP_DrawMapToMapVideoBuffer();

	GFX_LoadFont("FONTS.DAT", "SLIM.PCX", 128 * 32);
	GFX_LoadPalette("PALETTES.DAT", "OFFICE.PCX", 256);

	CAM_MainPositionControl(true, &map.update_required);// Place new cam position
	MAP_Update();

	VIDEO_MapBufferToScreenBuffer();// Fills a second buffer with the area shown

	// Set status panel graphics
	GFX_SetPanelGraphics(&gfx_actor_status_panel, SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
	GFX_SetPanelPosition(&gfx_actor_status_panel, 0, 0, 0, 0, 32, 0);
	GFX_ShowPanel(&gfx_actor_status_panel, true, 0);
	GFX_UpdatePanel(&gfx_actor_status_panel, actor.life, actor.life, actor.max_life, 1);
	VIDEO_PanelToScreenBuffer(&gfx_actor_status_panel);

	GFX_SetPanelGraphics(&gfx_enemy_status_panel, SPRITE_GRAPHICS_ID_BARREL2_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
	GFX_SetPanelPosition(&gfx_enemy_status_panel, 236, 0, 48, 0, 0, 0);
	GFX_ShowPanel(&gfx_enemy_status_panel, false, 200);
	VIDEO_PanelToScreenBuffer(&gfx_enemy_status_panel);

	// Update
	MOUSE_Update(actor.mode_combat);
	ACTOR_Update();
	ACTOR_UpdateBullets();
	OBJECT_UpdateObjects();
	PARTICLE_UpdateParticles();

	// Draw all sprites and effects
	GFX_UpdateSprites();
	GFX_DrawSprites();

	VIDEO_VSync();// Wait Vsync
	// Draws screen buffer to video ram
	VIDEO_ScreenBufferToVRAM();

	VIDEO_FadeIn(4);
	//VGA_SetPalette(gfxPaletteLoaded);


	song.current_pattern = 0;
	song.current_line = 0;
	song.pattern_break = false;
	song.pattern_delay = false;
	song.tickXF = 0;
	song.ticks = 0;
	song.next_line = 0;
	song.irq_mode = true;
	song.loop = true;

	song.ticklooper = 0;
	song.macro_ticklooper = 0;
	song.speed = songinfo.speed;
	song.macro_speedup = songinfo.macro_speedup;

	song.play = true;

	// Loop until the game is over
	while (kbKeyState[SCANCODE_ESC] == false) {
		Update(true);
	}
}


int main(int argc, char **argv) {
	//gdb_start();
	// Check for debug mode argument
	switch (argc) {
		case 3:
			// Two arguments
			if (strcmp(argv[2], "-god") == 0) {
				printf("God mode enabled\n");
				sleep(1);
			}

			if (strcmp(argv[2], "-debug") == 0) {
				printf("Debug mode enabled\n");
				sleep(1);
			}
		case 2:
			// One argument
			if (strcmp(argv[1], "-god") == 0) {
				printf("God mode enabled\n");
				sleep(1);
			}

			if (strcmp(argv[1], "-debug") == 0) {
				printf("Debug mode enabled\n");
				sleep(1);
			}
		case 1:
			// Normal start, no arguments
			break;
		default:
			break;
	}

	// Initialize the game engine
	InitEngine();
	Intro();
	GameLoop();
	ExitDos();
	return 0;
}
