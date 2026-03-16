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

/** LOGO :: Main logo sequence
 */
static void Logo(void) {
	bool spark_end;
	int logo_step;
	int spr_num;
	int spark_counter;
	unsigned char src_string[40];

	logo_step = 0;

	// shares logo sequence status is running
	engine.logo = true;

	while (engine.logo) {
		switch (logo_step) {
			case 0:// Logo MS-DOS Club
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LDOSCLUB.PCX", 256);
				GFX_PCXImageToBuffer("ILOGO.DAT", "LDOSCLUB.PCX", 180 * 135, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 70, 32);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				VIDEO_FadeIn_Async(1);
				VIDEO_RotatePaletteStart_Async(48, 79, 98);
				logo_step = 11;
				break;
			case 11:// Wait until async fade in is finished
				if (VIDEO_AwaitFadedIn()) logo_step = 12;
				break;
			case 12:// Set delay time
				SetDelayTime(2000);
				logo_step = 20;
				break;
			case 20:// Load new image on second buffer
				GFX_PCXImageToBuffer("ILOGO.DAT", "LCONTEST.PCX", 320 * 100, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "IWORMS.PCX", SPRITE_GRAPHICS_ID_TEMP, 192, 24, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
				GFX_InitSprite(SPRITE_GRAPHICS_ID_TEMP, 0, 0, &spr_num);
				GFX_SetSpriteAnimation(spr_num, 0, 14, 15, false, false);
				GFX_SetSpritePosition(spr_num, 63, 108);
				logo_step = 21;
				break;
			case 21:// Wait until delay is finished and fade out
				while (engine.delay_time_ms > 0) {
					// just wait
				}
				VIDEO_FadeOut(1);
				VIDEO_RotatePaletteEnd_Async();
				logo_step = 22;
				break;
			case 22:// Print new logo
				spr_num = 0;
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LCONTEST.PCX", 256);
				SetDelayTime(3000);
				VIDEO_FadeIn_Async(1);
				logo_step = 23;
				break;
			case 23:// Draw logo and effects until delay is finished
				while (engine.delay_time_ms > 0) {
					VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 0, 50);
					GFX_UpdateSprites();
					GFX_DrawSprites();
					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				logo_step = 24;
				break;
			case 24:// Unload sprites
				GFX_UnloadSprites();
				GFX_UnloadSpriteGraphics();
				logo_step = 30;
				break;
			case 30:// Load new image and fade out
				GFX_PCXImageToBuffer("ILOGO.DAT", "LMANOL.PCX", 220 * 165, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "SPARK.PCX", SPRITE_GRAPHICS_ID_TEMP, 16, 16, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
				FILE_LoadText("DIALOG.DAT", "INTRO.TXT", 1, src_string);
				GFX_InitSprite(SPRITE_GRAPHICS_ID_TEMP, 0, 0, &spr_num);
				GFX_SetSpriteAnimation(spr_num, 0, 4, 10, false, false);
				GFX_SetSpritePosition(spr_num, 72, 130);
				VIDEO_FadeOut(1);
				logo_step = 31;
				break;
			case 31:// Load sprites
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LMANOL.PCX", 256);
				SetDelayTime(4000);
				VIDEO_FadeIn_Async(1);
				logo_step = 32;
				break;
			case 32:// Draw image and effects
				spark_counter = 0;
				spark_end = false;
				while (!spark_end) {
					VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 50, 18);

					switch (spark_counter) {
						case 0:
							if (gfx_sprite_stack[spr_num].animation_end) spark_counter++;
							break;
						case 1:
							GFX_SetSpritePosition(spr_num, 100, 135);
							GFX_SetSpriteAnimation(spr_num, 0, 4, 10, false, false);
							spark_counter++;
							break;
						case 2:
							if (gfx_sprite_stack[spr_num].animation_end) spark_counter++;
							break;
						case 3:
							GFX_SetSpritePosition(spr_num, 180, 130);
							GFX_SetSpriteAnimation(spr_num, 0, 4, 10, false, false);
							spark_counter++;
							break;
						case 4:
							if (gfx_sprite_stack[spr_num].animation_end) spark_counter++;
							break;
						case 5:
							GFX_SetSpritePosition(spr_num, 218, 128);
							GFX_SetSpriteAnimation(spr_num, 0, 4, 10, false, false);
							spark_counter++;
							break;
						case 6:
							if (gfx_sprite_stack[spr_num].animation_end) spark_counter++;
							break;
						case 7:
							GFX_UnloadSprite(spr_num);
							GFX_UnloadSpriteGraphics();
							spark_counter++;
							break;
						case 8:
							spark_end = true;
							VIDEO_StringToScreenBuffer(72, 175, src_string, FONT_BIG_WHITE);
							break;
					}

					GFX_UpdateSprites();
					GFX_DrawSprites();
					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				logo_step = 50;
				break;
			case 50:
				engine.logo = false;
				break;
			default:
				break;
		}
	}

	SetDelayTime(0);
	VIDEO_RotatePaletteEnd_Async();
	GFX_UnloadSprites();
	GFX_UnloadSpriteGraphics();

	// shares logo sequence is ended
	engine.logo = false;
}

/** SCENARIO 0 :: Intro
 */
static void Scene0_Intro(void) {
	int step;
	int dialog_step, dialog_delay_counter;
	int char_number;
	unsigned char string1[40];
	unsigned char string2[40];
	unsigned char string3[40];
	unsigned char string4[40];

	int i, horizontal_scroll, max_scroll, scanline_scroll, src_index, dst_index, src_img_index;
	bool scroll_end;

	step = 0;

	engine.sequence = true;

	VIDEO_FadeOut(4);

	while (engine.sequence) {
		switch (step) {
			case 0:// Draw loading text
				VIDEO_ClearScreen();
				engine.loading = true;
				GFX_LoadPalette("PALETTES.DAT", "LOADING.PCX", 256);
				GFX_PCXImageToBuffer("IUI.DAT", "LOADING.PCX", 128 * 48, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 70);
				FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 6, string1);
				VIDEO_StringToScreenBuffer(115, 90, string1, FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn_Async(1);
				step = 1;
				break;
			case 1:// Load first intro scene
				AUDIO_LoadSong("MSCENE0.DAT", "intro0.a2m");
				GFX_LoadPalette("PALETTES.DAT", "ISCN0.PCX", 256);
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO0F.PCX", 320 * 200, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO01.PCX", 564 * 177, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO02.PCX", 128 * 48, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);

				engine.loading = false;
				VIDEO_FadeOut(1);

				dialog_step = 0;

				VIDEO_FadeIn_Async(1);
				AUDIO_PlaySong();
				step = 5;
				break;
			case 5:// Horizontal scroll and text

				src_index = 0;
				horizontal_scroll = 0;
				max_scroll = gfx.image_buffer1_width - 280;
				scroll_end = false;
				dialog_step = 0;
				while (!scroll_end & engine.sequence) {
					src_index = horizontal_scroll;
					dst_index = CAM_VISIBLE_WIDTH * 14 + 16;

					// Draw image with scroll
					for (i = 0; i < gfx.image_buffer1_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], CAM_VISIBLE_WIDTH - 35);
						src_index += gfx.image_buffer1_width;
						dst_index += CAM_VISIBLE_WIDTH;
					}
					if (horizontal_scroll < max_scroll) horizontal_scroll++;
					else
						scroll_end = true;

					// Draw text background
					VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 25, 20);

					// Draw text
					switch (dialog_step) {
						case 0:
							FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 1, string2);
							char_number = 0;
							dialog_step++;
							break;
						case 1:
							string1[char_number] = string2[char_number];
							string1[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 2) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(string2)) {
								dialog_step++;
							}
							break;
						case 2:
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);

							FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 2, string3);
							char_number = 0;
							dialog_step++;
							break;
						case 3:
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);

							string2[char_number] = string3[char_number];
							string2[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 40, string2, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 2) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(string3)) {
								dialog_step++;
							}
							break;
						case 4:
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, string2, FONT_SLIM_BLACK);

							FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 3, string4);
							char_number = 0;
							dialog_step++;
							break;
						case 5:
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, string2, FONT_SLIM_BLACK);

							string3[char_number] = string4[char_number];
							string3[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 52, string3, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 2) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(string4)) {
								dialog_step++;
							}
							break;
						case 6:
							VIDEO_StringToScreenBuffer(38, 28, string1, FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, string2, FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 52, string3, FONT_SLIM_BLACK);
							break;
						default:
							break;
					}


					SetDelayTime(50);
					while (!AwaitDelayTime()) {
						// just wait
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 6;
				break;
			case 6:
				// Load image onto temporary mem
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO1.PCX", 320 * 200, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				scanline_scroll = 1;
				step = 7;
				break;
			case 7:// Make image appear from bottom to top
				src_img_index = 0;
				while ((scanline_scroll < 200) & engine.sequence) {

					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 199; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 8;
				break;
			case 8:
				// Load image onto temporary mem
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO2.PCX", 112 * 62, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				step = 9;
				break;
			case 9:// Paste image into current screen
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 70, 120);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				step = 10;
				break;
			case 10:
				// Load image
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO3.PCX", 118 * 61, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				step = 11;
				break;
			case 11:// Paste image into current screen
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 190, 100);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 4, string2);
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(string2)) & engine.sequence) {
					string1[char_number] = string2[char_number];
					string1[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(270, 134, string1, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 14;
				break;
			case 14:
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO4.PCX", 320 * 200, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				scanline_scroll = 1;
				step = 15;
				break;
			case 15:// Make image appear from bottom to top
				src_img_index = 0;
				while ((scanline_scroll < 200) & engine.sequence) {

					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 200; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 16;
				break;
			case 16:
				SetDelayTime(300);
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO5.PCX", 320 * 160, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				step = 17;
				break;
			case 17:// Await delay and fade out
				while (!AwaitDelayTime()) {
					// just wait
				}
				VIDEO_FadeOut(1);
				VIDEO_ClearScreen();
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn_Async(1);
				horizontal_scroll = 320;
				scroll_end = false;
				step = 18;
				break;
			case 18:// Second page, first row horizontal scroll
				while ((horizontal_scroll > 0) & engine.sequence) {

					src_index = 0;
					dst_index = CAM_VISIBLE_WIDTH * 40 + horizontal_scroll;

					for (i = 0; i < gfx.image_buffer1_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], CAM_VISIBLE_WIDTH - horizontal_scroll);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					horizontal_scroll--;
					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				step = 19;
				break;
			case 19:
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO6.PCX", 320 * 143, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				scanline_scroll = 1;
				step = 20;
				break;
			case 20:// Scroll up
				src_img_index = 0;
				while ((scanline_scroll < gfx.image_buffer1_height) & engine.sequence) {

					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 200; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 5, string1);
				VIDEO_StringToScreenBuffer(215, 70, string1, FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();


				FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 6, string2);
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(string2)) & engine.sequence) {
					string1[char_number] = string2[char_number];
					string1[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(210, 104, string1, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 7, string2);
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(string2)) & engine.sequence) {
					string1[char_number] = string2[char_number];
					string1[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(210, 116, string1, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 21;
				break;
			case 21:
				GFX_PCXImageToBuffer("ISCN0.DAT", "INTRO7.PCX", 320 * 180, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				scanline_scroll = 1;
				step = 22;
				break;
			case 22:// Scroll up
				src_img_index = 0;
				while ((scanline_scroll < gfx.image_buffer1_height) & engine.sequence) {
					src_index = CAM_VISIBLE_WIDTH;
					dst_index = 0;

					for (i = 0; i < 200; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][src_index], CAM_VISIBLE_WIDTH);
						src_index += CAM_VISIBLE_WIDTH;
						dst_index += CAM_VISIBLE_WIDTH;
					}

					scanline_scroll++;

					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_img_index], 320);
					src_img_index += 320;

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				FILE_LoadText("DIALOG.DAT", "SCN0I.TXT", 8, string2);
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(string2)) & engine.sequence) {
					string1[char_number] = string2[char_number];
					string1[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(210, 40, string1, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				step = 23;
				break;
			case 23:
				SetDelayTime(5000);
				while (!AwaitDelayTime()) {
					// just wait
				}
				step = 30;
				break;
			case 30://Fade out
				VIDEO_FadeOut(1);
				step = 31;
				break;
			case 31:// Stop music
				engine.sequence = false;
				break;
		}
	}
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UpdateObjects();
	GFX_UnloadSprites();
	GFX_UnloadSpriteGraphics();
	AUDIO_UnloadSong();
}

/** SCENARIO 1 :: Game loop
 */
static void Scene0_Loop(void) {

	int actor_tile_num;
	unsigned char string1[40];

	VIDEO_ClearScreen();
	engine.loading = true;
	GFX_LoadPalette("PALETTES.DAT", "LOADING.PCX", 256);
	GFX_PCXImageToBuffer("IUI.DAT", "LOADING.PCX", 128 * 48, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
	VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 70);
	FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 6, string1);
	VIDEO_StringToScreenBuffer(115, 90, string1, FONT_SLIM_BLACK);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn_Async(1);

	// Load sprite graphics onto the graphics stack
	A2M_LoadFile("MSCENE0.DAT", "scene0.a2m");
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTOR.PCX", SPRITE_GRAPHICS_ID_ACTOR, 32, 32, 105, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTORP.PCX", SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET1.PCX", SPRITE_GRAPHICS_ID_BULLET1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUNCH1.PCX", SPRITE_GRAPHICS_ID_BULLET2, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1.PCX", SPRITE_GRAPHICS_ID_BARREL1, 16, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1P.PCX", SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "PUNCHL.PCX", SPRITE_GRAPHICS_ID_PUNCH_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BLOOD.PCX", SPRITE_GRAPHICS_ID_BLOOD, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "FIRE1.PCX", SPRITE_GRAPHICS_ID_FIRE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "LIFEBAR.PCX", SPRITE_GRAPHICS_ID_LIFEBAR, 48, 6, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "ACTORF.PCX", SPRITE_GRAPHICS_ID_ACTORCHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT, 220, 70, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1.PCX", SPRITE_GRAPHICS_ID_ENEMY1, 32, 32, 60, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1P.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);

	// Initialize mouse cursor
	MOUSE_InitCursorSprite(SPRITE_GRAPHICS_ID_CURSOR);

	// Initialize actor
	ACTOR_Init(1 << 4, 21 << 4, SPRITE_GRAPHICS_ID_ACTOR, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_PUNCH_EFFECT, SPRITE_GRAPHICS_ID_BULLET1, 86, 88);
	ACTOR_SetGun(ACTOR_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH_EFFECT, SPRITE_GRAPHICS_ID_PUNCH_EFFECT, SPRITE_GRAPHICS_ID_BULLET1);

	ENEMY_Init(0, 23 << 4, 6 << 4, ENEMY_FACING_LEFT, SPRITE_GRAPHICS_ID_ENEMY1, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1, ENEMY_STATUS_WAKEUP, 10);
	ENEMY_Init(1, 25 << 4, 6 << 4, ENEMY_FACING_DOWN, SPRITE_GRAPHICS_ID_ENEMY1, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1, ENEMY_STATUS_WAKEUP, 10);

	MAP_LoadTileset("TSCN0.DAT", "SCN0_BACK.PCX", &map.layer_tiles[MAP_BACKGROUND_LAYER], 320 * 416);
	MAP_LoadTileset("TSCN0.DAT", "SCN0_FORE.PCX", &map.layer_tiles[MAP_FOREGROUND_LAYER], 128 * 128);
	MAP_LoadTileset("TSCN0.DAT", "SCN0_MASK.PCX", &map.layer_tiles[MAP_MASK_LAYER], 128 * 128);

	MAP_LoadMap("MAPSCN0.DAT", 60, 25);

	CAM_SetLimits(map.width_px, map.height_px);
	CAM_SetPosition(0 << 4, 10 << 4);

	MAP_DrawMapToMapVideoBuffer();

	GFX_LoadPalette("PALETTES.DAT", "SCN0.PCX", 256);

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

	engine.loading = false;
	VIDEO_FadeOut(1);

	engine.ingame = true;
	VIDEO_FadeIn_Async(1);
	GFX_LoadPalette("PALETTES.DAT", "SCN0.PCX", 256);

	AUDIO_PlaySong();

	// Loop until the game is over
	while (engine.ingame) {
		if (!ui.freeze) {
			if (!actor.mode_combat) {
				// Check point on object or enemy
				switch (cursor.point_on & 0xFF) {
					case ENTITY_ID_HSPOT:// Hotspot
						switch ((cursor.point_on >> 8) & 0xFF) {
							case 0:// No hotspot, WTF??
								break;
							case 1:// Paint
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 101, 102, 103, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 1);
								}
								break;
							case 2:// XXX
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 105, 106, 107, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 2);
								}
								break;
							case 3:// Container
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 109, 110, 111, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 3);
								}
								break;
							case 4:// Bed
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 113, 114, 115, false, 100);
								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 4);
								}
								break;
							case 5:// Garbage
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 117, 118, 119, false, 100);
								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 5);
								}
								break;
							case 6:// Police office
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN0H.TXT", 121, 122, 123, false, 100);
								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN0H.TXT", 6);
								}
								break;
							default:
								break;
						}


					default:
						break;
				}
			}
			// Check if actor triggers an event
			actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
			engine.debug1_INT = actor_tile_num;
			engine.debug2_INT = map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num];
			switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
				case 1:// Event 1. Enter door
					engine.ingame = false;
					// Go to scene 2
					engine.scene++;
					break;
				default:
					break;
			}
		}

		Update(true);
	}

	VIDEO_FadeOut(4);
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UpdateObjects();
	GFX_UnloadSprites();
	GFX_UnloadSpriteGraphics();
	AUDIO_UnloadSong();
}

/** SCENARIO 1 :: Intro
 */
static void Scene1_Intro(void) {
}


/** SCENARIO 1 :: Game loop
 */
static void Scene1_Loop(void) {

	int actor_tile_num;
	unsigned char string1[40];

	VIDEO_ClearScreen();
	engine.loading = true;
	GFX_LoadPalette("PALETTES.DAT", "LOADING.PCX", 256);
	GFX_PCXImageToBuffer("IUI.DAT", "LOADING.PCX", 128 * 48, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
	VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 70);
	FILE_LoadText("DIALOG.DAT", "GLOBAL.TXT", 6, string1);
	VIDEO_StringToScreenBuffer(115, 90, string1, FONT_SLIM_BLACK);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn_Async(1);

	// Load sprite graphics onto the graphics stack
	A2M_LoadFile("MSCENE1.DAT", "scene1.a2m");
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTOR.PCX", SPRITE_GRAPHICS_ID_ACTOR, 32, 32, 105, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "ACTORP.PCX", SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET1.PCX", SPRITE_GRAPHICS_ID_BULLET1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUNCH1.PCX", SPRITE_GRAPHICS_ID_BULLET2, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1.PCX", SPRITE_GRAPHICS_ID_BARREL1, 16, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1P.PCX", SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "PUNCHL.PCX", SPRITE_GRAPHICS_ID_PUNCH_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BLOOD.PCX", SPRITE_GRAPHICS_ID_BLOOD, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "FIRE1.PCX", SPRITE_GRAPHICS_ID_FIRE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "ELECTRIC1.PCX", SPRITE_GRAPHICS_ID_ELECTRIC1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "LIFEBAR.PCX", SPRITE_GRAPHICS_ID_LIFEBAR, 48, 6, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "ACTORF.PCX", SPRITE_GRAPHICS_ID_ACTORCHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT, 220, 70, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1.PCX", SPRITE_GRAPHICS_ID_ENEMY1, 32, 32, 60, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("ENEMY.DAT", "ENEMY1P.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);

	// Initialize mouse cursor
	MOUSE_InitCursorSprite(SPRITE_GRAPHICS_ID_CURSOR);

	// Initialize actor
	ACTOR_Init(700, 470, SPRITE_GRAPHICS_ID_ACTOR, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_PUNCH_EFFECT, SPRITE_GRAPHICS_ID_BULLET1, 86, 88);
	ACTOR_SetGun(ACTOR_GUN_PISTOL, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_PUNCH_EFFECT, SPRITE_GRAPHICS_ID_BULLET1);

	ENEMY_Init(0, 23 << 4, 6 << 4, ENEMY_FACING_LEFT, SPRITE_GRAPHICS_ID_ENEMY1, ENEMY_GUN_PISTOL, SPRITE_GRAPHICS_ID_BULLET1, SPRITE_GRAPHICS_ID_BULLET1, ENEMY_STATUS_WAKEUP, 10);

	OBJECT_LoadObject(1, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 34 << 4, 30 << 4);
	OBJECT_LoadObject(2, ENTITY_ID_BARREL, SPRITE_GRAPHICS_ID_BARREL1, 35 << 4, 30 << 4);

	MAP_LoadTileset("TSCN1.DAT", "SCN1_BACK.PCX", &map.layer_tiles[MAP_BACKGROUND_LAYER], 320 * 416);
	MAP_LoadTileset("TSCN1.DAT", "SCN1_FORE.PCX", &map.layer_tiles[MAP_FOREGROUND_LAYER], 128 * 128);
	MAP_LoadTileset("TSCN1.DAT", "SCN1_MASK.PCX", &map.layer_tiles[MAP_MASK_LAYER], 128 * 128);

	MAP_LoadMap("MAPSCN1.DAT", 50, 40);

	CAM_SetLimits(map.width_px, map.height_px);
	CAM_SetPosition(30 << 4, 22 << 4);

	MAP_DrawMapToMapVideoBuffer();

	GFX_LoadPalette("PALETTES.DAT", "SCN1.PCX", 256);

	CAM_MainPositionControl(true, &map.update_required);// Place new cam position
	MAP_Update();

	VIDEO_MapBufferToScreenBuffer();// Fills a second buffer with the area shown

	// Set status panel graphics
	GFX_SetPanelGraphics(&gfx_actor_status_panel, SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
	GFX_SetPanelPosition(&gfx_actor_status_panel, 0, 0, 0, 0, 32, 0);
	GFX_ShowPanel(&gfx_actor_status_panel, true, 0);
	GFX_UpdatePanel(&gfx_actor_status_panel, actor.life, actor.life, actor.max_life, 1);
	VIDEO_PanelToScreenBuffer(&gfx_actor_status_panel);

	GFX_SetPanelGraphics(&gfx_enemy_status_panel, SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
	GFX_SetPanelPosition(&gfx_enemy_status_panel, 236, 0, 48, 0, 0, 0);
	GFX_ShowPanel(&gfx_enemy_status_panel, false, 200);
	VIDEO_PanelToScreenBuffer(&gfx_enemy_status_panel);

	// Update
	MOUSE_Update(actor.mode_combat);
	ACTOR_Update();
	OBJECT_UpdateObjects();
	PARTICLE_UpdateParticles();

	// Draw all sprites and effects
	GFX_UpdateSprites();
	GFX_DrawSprites();

	engine.loading = false;
	VIDEO_FadeOut(1);

	engine.ingame = true;
	VIDEO_FadeIn_Async(1);
	GFX_LoadPalette("PALETTES.DAT", "SCN1.PCX", 256);

	AUDIO_PlaySong();

	// Loop until the game is over
	while (engine.ingame) {

		if (!ui.freeze) {
			if (!actor.mode_combat) {
				// Check point on object or enemy
				switch (cursor.point_on & 0xFF) {
					case ENTITY_ID_HSPOT:// Hotspot
						switch ((cursor.point_on >> 8) & 0xFF) {
							case 0:// No hotspot, WTF??
								break;
							case 1:// Computer
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN1H.TXT", 101, 102, 103, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN1H.TXT", 1);
								}
								break;
							case 2:// Alarm system
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN1H.TXT", 105, 106, 107, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN1H.TXT", 2);
								}
								break;
							case 3:// Camera
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN1H.TXT", 109, 110, 111, false, 100);

								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN1H.TXT", 3);
								}
								break;
							case 4:// Banner
								if (cursor.left_click) {
									UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, "DIALOG.DAT", "SCN1H.TXT", 113, 114, 115, false, 100);
								} else {
									UI_ShowDescription("DIALOG.DAT", "SCN1H.TXT", 4);
								}
								break;
							default:
								break;
						}


					default:
						break;
				}
			}

			// Check if actor triggers an event
			actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
			switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
				case 1:// Event 1. Enter door
					engine.ingame = false;
					// Go to back outside
					engine.scene = 1;
					break;
				default:
					break;
			}
		}
		Update(true);
	}

	VIDEO_FadeOut(4);
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UpdateObjects();
	GFX_UnloadSprites();
	GFX_UnloadSpriteGraphics();
	AUDIO_UnloadSong();
}

/** SCENARIO 2 :: Intro
 */
static void Scene2_Intro(void) {
}

/** SCENARIO 2 :: Game loop
 */
static void Scene2_Loop(void) {
}

/** SCENARIO 3 :: Intro
 */
static void Scene3_Intro(void) {
}

/** SCENARIO 3 :: Game loop
 */
static void Scene3_Loop(void) {
}

/** SCENARIO 4 :: Intro
 */
static void Scene4_Intro(void) {
}

/** SCENARIO 4 :: Game loop
 */
static void Scene4_Loop(void) {
}

/** SCENARIO 5 :: Intro
 */
static void Scene5_Intro(void) {
}

/** SCENARIO 5 :: Game loop
 */
static void Scene5_Loop(void) {
}

/** SCENARIO 6 :: Intro
 */
static void Scene6_Intro(void) {
}

/** SCENARIO 5 :: Game loop
 */
static void Scene6_Loop(void) {
}

static void Credits(void) {
	unsigned char src_string[40];
	VIDEO_FadeOut(1);
	SetDelayTime(1000);
	VIDEO_ClearScreen();

	while (!AwaitDelayTime()) {
		// just wait
	}
	VIDEO_FadeIn(1);

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 1, src_string);
	VIDEO_StringToScreenBuffer(0, 10, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 2, src_string);
	VIDEO_StringToScreenBuffer(0, 20, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 3, src_string);
	VIDEO_StringToScreenBuffer(0, 30, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 4, src_string);
	VIDEO_StringToScreenBuffer(0, 50, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();


	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 5, src_string);
	VIDEO_StringToScreenBuffer(0, 60, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 6, src_string);
	VIDEO_StringToScreenBuffer(0, 70, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 7, src_string);
	VIDEO_StringToScreenBuffer(0, 90, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 8, src_string);
	VIDEO_StringToScreenBuffer(0, 100, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 9, src_string);
	VIDEO_StringToScreenBuffer(0, 110, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	FILE_LoadText("DIALOG.DAT", "CREDITS.TXT", 10, src_string);
	VIDEO_StringToScreenBuffer(0, 120, src_string, FONT_SLIM_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	SetDelayTime(5000);
	while (!AwaitDelayTime()) {
		// just wait
	}

	VIDEO_FadeOut(1);
	VIDEO_ClearScreen();

	VIDEO_FadeIn_Async(1);
}

/** MENU :: Main menu
 */
static void Menu(void) {

	bool exit_menu;
	int i, spr_num;
	unsigned char src_string[40];
	AUDIO_LoadSong("MINTRO.DAT", "intro1.a2m");
	GFX_LoadPalette("PALETTES.DAT", "MENU.PCX", 256);
	GFX_PCXImageToBuffer("IMENU.DAT", "MENUBACK.PCX", 360 * 200, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 16, 16, 8, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BUTTON.DAT", "BTNNEW.PCX", SPRITE_GRAPHICS_ID_BTN_NEW, 64, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BUTTON.DAT", "BTNLOAD.PCX", SPRITE_GRAPHICS_ID_BTN_LOAD, 64, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BUTTON.DAT", "BTNOPT.PCX", SPRITE_GRAPHICS_ID_BTN_OPTIONS, 64, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BUTTON.DAT", "BTNCRED.PCX", SPRITE_GRAPHICS_ID_BTN_CREDITS, 64, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("BUTTON.DAT", "BTNEXIT.PCX", SPRITE_GRAPHICS_ID_BTN_EXIT, 64, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "MTITLE.PCX", SPRITE_GRAPHICS_ID_TITLE, 243, 41, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR);

	VIDEO_FadeOut(1);
	GFX_LoadPalette("PALETTES.DAT", "MENU.PCX", 256);
	AUDIO_PlaySong();
	VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn_Async(1);

	for (i = 0; i < 20; i++) {
		VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[i], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		SetDelayTime(50);
		while (!AwaitDelayTime()) {
			// just wait
		}
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}
	GFX_InitSprite(SPRITE_GRAPHICS_ID_TITLE, 0, 0, &spr_num);
	GFX_SetSpritePosition(spr_num, 40, 200);

	for (i = 0; i < 60; i++) {
		VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[20], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_SetSpritePosition(spr_num, 40, gfx_sprite_stack[spr_num].screen_pos_y - 1);
		GFX_UpdateSprites();
		GFX_DrawSprites();
		SetDelayTime(10);
		while (!AwaitDelayTime()) {
			// just wait
		}
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}
	FILE_LoadText("DIALOG.DAT", "INTRO.TXT", 2, src_string);
	VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[20], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
	GFX_UpdateSprites();
	GFX_DrawSprites();
	VIDEO_StringToScreenBuffer(35, 175, src_string, FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	UI_LoadButton(1, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_NEW, 320, 16);
	UI_LoadButton(2, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_LOAD, 330, 40);
	UI_LoadButton(3, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_OPTIONS, 340, 64);
	UI_LoadButton(4, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_CREDITS, 350, 88);
	UI_LoadButton(5, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_EXIT, 360, 112);

	for (i = 0; i < 20; i++) {
		UI_UpdateButtons();
		if (UI_GetButtonXPosition(1) > 300) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 4, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(1) > 255) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 2, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(1) > 250) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 1, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(2) > 300) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 4, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(2) > 255) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 2, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(2) > 250) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 1, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(3) > 300) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 4, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(3) > 255) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 2, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(3) > 250) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 1, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(4) > 300) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 4, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(4) > 255) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 2, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(4) > 250) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 1, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(5) > 300) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 4, UI_GetButtonYPosition(5));
		if (UI_GetButtonXPosition(5) > 255) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 2, UI_GetButtonYPosition(5));
		if (UI_GetButtonXPosition(5) > 250) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 1, UI_GetButtonYPosition(5));
		VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[20 + i], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_SetSpritePosition(spr_num, 40 - i, gfx_sprite_stack[spr_num].screen_pos_y);
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_StringToScreenBuffer(35 - i, 175, src_string, FONT_BIG_WHITE);
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	while (UI_GetButtonXPosition(5) != 250) {
		UI_UpdateButtons();
		if (UI_GetButtonXPosition(1) > 300) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 4, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(1) > 255) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 2, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(1) > 250) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 1, UI_GetButtonYPosition(1));
		if (UI_GetButtonXPosition(2) > 300) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 4, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(2) > 255) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 2, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(2) > 250) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 1, UI_GetButtonYPosition(2));
		if (UI_GetButtonXPosition(3) > 300) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 4, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(3) > 255) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 2, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(3) > 250) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 1, UI_GetButtonYPosition(3));
		if (UI_GetButtonXPosition(4) > 300) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 4, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(4) > 255) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 2, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(4) > 250) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 1, UI_GetButtonYPosition(4));
		if (UI_GetButtonXPosition(5) > 300) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 4, UI_GetButtonYPosition(5));
		if (UI_GetButtonXPosition(5) > 255) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 2, UI_GetButtonYPosition(5));
		if (UI_GetButtonXPosition(5) > 250) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 1, UI_GetButtonYPosition(5));
		VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[40], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_StringToScreenBuffer(15, 175, src_string, FONT_BIG_WHITE);
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	MOUSE_InitCursorSprite(SPRITE_GRAPHICS_ID_CURSOR);

	exit_menu = false;
	while (!exit_menu) {
		UI_UpdateUI(false);
		MOUSE_Update(false);

		GFX_UpdateSprites();

		VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[40], gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_DrawSprites();
		VIDEO_StringToScreenBuffer(15, 175, src_string, FONT_BIG_WHITE);

		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();

		// new game selected
		switch (ui.button_pressed) {
			case 0:// No button pressed
				break;
			case 1:// New game
				exit_menu = true;
				engine.scene = 1;
				break;
			case 2:// Load game
				exit_menu = true;
				break;
			case 3:// options
				exit_menu = true;
				break;
			case 4:// credits
				Credits();
				break;
			case 5:// exit game
				exit_menu = true;
				engine.exit_game = true;
				break;
		}
	}

	VIDEO_FadeOut(1);
	// Unload all menu assets
	UI_UnloadButtons();
	GFX_UnloadSprites();
	GFX_UnloadSpriteGraphics();
	AUDIO_StopSong();
	AUDIO_UnloadSong();
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

	Logo();

	engine.exit_game = false;
	engine.scene = 0;
	while (!engine.exit_game) {
		switch (engine.scene) {
			case 0:// main menu
				Menu();
				break;
			case 1:// scene 1 :: Street
				Scene0_Intro();
				Scene0_Loop();
				break;
			case 2:// scene 1 :: Police office
				Scene1_Intro();
				Scene1_Loop();
				break;
			case 3:// scene 3 :: Hospital
				Scene2_Intro();
				Scene2_Loop();
				break;
			case 4:// scene 4 :: Meeting
				Scene3_Intro();
				Scene3_Loop();
				break;
			case 5:// scene 5 :: Sewers
				Scene4_Intro();
				Scene4_Loop();
				break;
			case 6:// scene 6 :: Junkyard
				Scene5_Intro();
				Scene5_Loop();
				break;
			case 7:// scene 6 :: ....
				Scene6_Intro();
				Scene6_Loop();
				break;
		}
	}
	ExitDos();
	return 0;
}
