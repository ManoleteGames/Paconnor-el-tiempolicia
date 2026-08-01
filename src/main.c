/**********************************************
* main.c
* Entry point for the game itself
* (c) 2025-2026 by J.Martin
***********************************************/

//#include <bios.h>
//#include <dpmi.h>
//#include <sys/nearptr.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "engine/actor/actor.h"
#include "engine/audio/audio.h"
#include "engine/camera/camera.h"
#include "engine/effect/effect.h"
#include "engine/enemy/enemy.h"
#include "engine/engine.h"
#include "engine/file/file.h"
#include "engine/gfx/gfx.h"
#include "engine/grenade/grenade.h"
#include "engine/item/item.h"
#include "engine/keyb/keyb.h"
#include "engine/map/map.h"
#include "engine/mem/mem.h"
#include "engine/mouse/mouse.h"
#include "engine/npc/npc.h"
#include "engine/object/object.h"
#include "engine/particle/particle.h"
#include "engine/types/types.h"
#include "engine/ui/ui.h"
#include "engine/video/video.h"
#include "pc.h"
#include "stdbool.h"
#include <stdio.h>

#define GDB_IMPLEMENTATION
#include "gdbstub.h"

bool hotspot_enabling_room1[32];
bool event_enabling_room1[32];
bool hotspot_enabling_room2[32];
bool event_enabling_room2[32];

/** Load language text files
 */
static void LoadTexts(void) {
	char language[13];
	// Load game texts
	switch (ui.language) {
		case 0:// ES
			sprintf(language, "DIALOGES.DAT");
			break;
		case 1:// EN
			sprintf(language, "DIALOGEN.DAT");
			break;
		case 2:// FR
			sprintf(language, "DIALOGFR.DAT");
			break;
	}

	// Clear dialogs buffers
	FILE_LoadTextFile(language, "INTRO.TXT", &ui.txt_file[UI_TXT_INTRO]);
	FILE_LoadTextFile(language, "GLOBAL.TXT", &ui.txt_file[UI_TXT_GLOBAL]);
	FILE_LoadTextFile(language, "CREDITS.TXT", &ui.txt_file[UI_TXT_CREDITS]);
	FILE_LoadTextFile(language, "NAMES.TXT", &ui.txt_file[UI_TXT_NAMES]);
	FILE_LoadTextFile(language, "SCN1D.TXT", &ui.txt_file[UI_TXT_SCN1D]);
	FILE_LoadTextFile(language, "SCN1H.TXT", &ui.txt_file[UI_TXT_SCN1H]);
	FILE_LoadTextFile(language, "SCN1I.TXT", &ui.txt_file[UI_TXT_SCN1I]);
	FILE_LoadTextFile(language, "SCN2D.TXT", &ui.txt_file[UI_TXT_SCN2D]);
	FILE_LoadTextFile(language, "SCN2H.TXT", &ui.txt_file[UI_TXT_SCN2H]);
	FILE_LoadTextFile(language, "SCN2I.TXT", &ui.txt_file[UI_TXT_SCN2I]);
	FILE_LoadTextFile(language, "SCN3D.TXT", &ui.txt_file[UI_TXT_SCN3D]);
	FILE_LoadTextFile(language, "SCN3H.TXT", &ui.txt_file[UI_TXT_SCN3H]);
	FILE_LoadTextFile(language, "SCN3I.TXT", &ui.txt_file[UI_TXT_SCN3I]);
	FILE_LoadTextFile(language, "SCN4D.TXT", &ui.txt_file[UI_TXT_SCN4D]);
	FILE_LoadTextFile(language, "SCN4H.TXT", &ui.txt_file[UI_TXT_SCN4H]);
	FILE_LoadTextFile(language, "SCN4I.TXT", &ui.txt_file[UI_TXT_SCN4I]);
	FILE_LoadTextFile(language, "SCN5D.TXT", &ui.txt_file[UI_TXT_SCN5D]);
	FILE_LoadTextFile(language, "SCN5H.TXT", &ui.txt_file[UI_TXT_SCN5H]);
	FILE_LoadTextFile(language, "SCN5I.TXT", &ui.txt_file[UI_TXT_SCN5I]);
	FILE_LoadTextFile(language, "SCN6D.TXT", &ui.txt_file[UI_TXT_SCN6D]);
	FILE_LoadTextFile(language, "SCN6H.TXT", &ui.txt_file[UI_TXT_SCN6H]);
	FILE_LoadTextFile(language, "SCN6I.TXT", &ui.txt_file[UI_TXT_SCN6I]);
	FILE_LoadTextFile(language, "SCN7D.TXT", &ui.txt_file[UI_TXT_SCN7D]);
}

/** Main logo sequence
 */
static void Logo(void) {
	bool spark_end;
	int logo_step;
	int spr_num;
	int spark_counter;
	int sprite_slot;
	int char_counter, aux_counter;

	logo_step = 0;

	// shares logo sequence status is running
	engine.logo = true;

	while (engine.logo) {
		switch (logo_step) {
			case 0:// Logo MS-DOS Club
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LDOSCLUB.PCX", 256);
				AUDIO_LoadSong(AUDIO_SONG_6);// Load song
				GFX_PCXImageToBuffer("ILOGO.DAT", "LDOSCLUB.PCX", 180 * 135, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 70, 32);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn_Async(1);
				VIDEO_RotatePaletteStart_Async(48, 79, 98);

				logo_step = 11;
				break;
			case 11:// Set delay time
				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 10);
				AUDIO_PlaySong(false);
				logo_step = 12;
				break;
			case 12:// Load some assets
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SPARK.PCX", SPRITE_GRAPHICS_ID_SPARK_EFFECT, 16, 16, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "PUNCHL.PCX", SPRITE_GRAPHICS_ID_PUNCH_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "PISTOLL.PCX", SPRITE_GRAPHICS_ID_PISTOL_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SHOTGUNL.PCX", SPRITE_GRAPHICS_ID_SHOTGUN_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "UZIL.PCX", SPRITE_GRAPHICS_ID_UZI_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNNEW.PCX", SPRITE_GRAPHICS_ID_BTN_NEW, 63, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNOPT.PCX", SPRITE_GRAPHICS_ID_BTN_OPT, 63, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNPASS.PCX", SPRITE_GRAPHICS_ID_BTN_PASS, 63, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNCRED.PCX", SPRITE_GRAPHICS_ID_BTN_CREDITS, 63, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNEXIT.PCX", SPRITE_GRAPHICS_ID_BTN_EXIT, 63, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNBACK.PCX", SPRITE_GRAPHICS_ID_BTN_BACK, 31, 15, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNOK.PCX", SPRITE_GRAPHICS_ID_BTN_OK, 31, 15, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNRIGHT.PCX", SPRITE_GRAPHICS_ID_BTN_RIGHT, 15, 15, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "BTNLEFT.PCX", SPRITE_GRAPHICS_ID_BTN_LEFT, 15, 15, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				logo_step = 20;
				break;
			case 20:// Load new image on second buffer
				GFX_PCXImageToBuffer("ILOGO.DAT", "LCONTEST.PCX", 320 * 100, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "IWORMS.PCX", SPRITE_GRAPHICS_ID_WORMS, 192, 24, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "EMPTY.PCX", SPRITE_GRAPHICS_ID_EMPTY, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				sprite_slot = GFX_FindEmptySpriteSlot();
				if (sprite_slot == -1) {
					sprintf(engine.system_error_message1, "Logo function error");
					sprintf(engine.system_error_message2, "No empty sprite slot available");
					sprintf(engine.system_error_message3, " ");
					Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
				} else {
					spr_num = sprite_slot;
					GFX_InitSprite(ENTITY_ID_EMPTY, 0, spr_num, 0, 192, 24);
				}

				GFX_SetSpriteGraphic(spr_num, 0, SPRITE_GRAPHICS_ID_WORMS, 0, 0);
				GFX_SetSpritePosition(spr_num, 63, 108);
				GFX_SetDefaultAnimation(spr_num, false, false, 8);
				logo_step = 21;
				break;
			case 21:// Load some assets by the way

				GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "MTITLE.PCX", SPRITE_GRAPHICS_ID_TITLE, 243, 41, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT_SMALL, 55, 30, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "CHAT_BIG.PCX", SPRITE_GRAPHICS_ID_CHAT_BIG, 140, 50, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN00.PCX", SPRITE_GRAPHICS_ID_GUN0, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN01.PCX", SPRITE_GRAPHICS_ID_GUN1, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN02.PCX", SPRITE_GRAPHICS_ID_GUN2, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN03.PCX", SPRITE_GRAPHICS_ID_GUN3, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "LIFEBAR.PCX", SPRITE_GRAPHICS_ID_LIFEBAR, 48, 6, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN04.PCX", SPRITE_GRAPHICS_ID_GUN4, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN05.PCX", SPRITE_GRAPHICS_ID_GUN5, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GRENADE.PCX", SPRITE_GRAPHICS_ID_GRENADE, 16, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				logo_step = 22;
				break;
			case 22:// wait song ended
				if (AUDIO_IsSongEnded()) {
					logo_step = 23;
					VIDEO_RotatePaletteEnd_Async();
					VIDEO_FadeOut(1);
				}
				break;
			case 23:// Print new logo
				spr_num = 0;
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LCONTEST.PCX", 256);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 0, 50);
				VIDEO_FadeIn(1);
				logo_step = 24;
				break;
			case 24:// Draw logo and effects until delay is finished
				while (!GFX_IsSpriteAnimationEnded(spr_num, 0)) {
					VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 0, 50);
					GFX_UpdateSprites();
					if (gfx_sprite_stack[spr_num].animation->current_frame == 2) AUDIO_PlaySound(AUDIO_JUMP_EFFECT, 1);
					if (gfx_sprite_stack[spr_num].animation->current_frame == 12) AUDIO_PlaySound(AUDIO_CRASH_EFFECT, 1);
					GFX_DrawSprites();
					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				logo_step = 25;
				break;
			case 25:// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ACTOR_HEAD, 12, 12, 30, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ACTOR_FEET, 27, 16, 84, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ACTOR_BODY, 20, 15, 100, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "LARM1.PCX", SPRITE_GRAPHICS_ID_ACTOR_LARM, 16, 18, 90, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM1.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, 16, 18, 135, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM2.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_PISTOL, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				logo_step = 26;
				break;
			case 26:// Unload sprites
				GFX_UnloadSprites();
				logo_step = 30;
				break;
			case 30:// Load new image and fade out
				GFX_PCXImageToBuffer("ILOGO.DAT", "LMANOL.PCX", 220 * 165, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_FadeOut(1);
				logo_step = 31;
				break;
			case 31:// Load sprites
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LMANOL.PCX", 256);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 50, 18);
				VIDEO_FadeIn(1);
				logo_step = 32;
				break;
			case 32:// Draw image and effects
				spark_counter = 0;
				spark_end = false;
				while (!spark_end) {
					VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 50, 18);
					switch (spark_counter) {
						case 0:// Load some assets
							GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 7, 12, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 15, 15, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							spark_counter++;
							break;
						case 1:
							EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_SPARK_EFFECT, 65, 130, true, 0, false, false, 7);
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							spark_counter++;
							break;
						case 2:
							if (EFFECT_NoEffectsActive()) spark_counter++;
							break;
						case 3:// Load some assets
							GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "POPTIONS.PCX", SPRITE_GRAPHICS_ID_PNL_OPTIONS, 62, 130, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "PPASS.PCX", SPRITE_GRAPHICS_ID_PNL_PASS, 120, 41, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							spark_counter++;
							break;
						case 4:
							EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_SPARK_EFFECT, 100, 128, true, 0, false, false, 7);
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							spark_counter++;
							break;
						case 5:
							if (EFFECT_NoEffectsActive()) spark_counter++;
							break;
						case 6:// Load some assets
							GFX_LoadSpriteGraphicsRLE("SMENU.DAT", "MCHAT.PCX", SPRITE_GRAPHICS_ID_MCHAT, 136, 88, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT, 220, 70, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							spark_counter++;
							break;
						case 7:
							EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_SPARK_EFFECT, 128, 130, true, 0, false, false, 7);
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							spark_counter++;
							break;
						case 8:
							if (EFFECT_NoEffectsActive()) spark_counter++;
							break;
						case 9:// Load some assets
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET1.PCX", SPRITE_GRAPHICS_ID_BULLET1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET2.PCX", SPRITE_GRAPHICS_ID_BULLET2, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUKE.PCX", SPRITE_GRAPHICS_ID_BULLET3, 16, 16, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							spark_counter++;
							break;
						case 10:
							EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_SPARK_EFFECT, 175, 130, true, 0, false, false, 7);
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							spark_counter++;
							break;
						case 11:
							if (EFFECT_NoEffectsActive()) spark_counter++;
							break;
						case 12:// Load some assets
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "GRENADE1.PCX", SPRITE_GRAPHICS_ID_GRENADE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "GRENADES.PCX", SPRITE_GRAPHICS_ID_GRENADE_SHADOW, 8, 8, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUNCH1.PCX", SPRITE_GRAPHICS_ID_PUNCH, 12, 12, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							spark_counter++;
							break;
						case 13:
							EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_SPARK_EFFECT, 210, 128, true, 0, false, false, 7);
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							spark_counter++;
							break;
						case 14:
							if (EFFECT_NoEffectsActive()) spark_counter++;
							break;
						case 15:
							// Load some more assets
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SNIPPERL.PCX", SPRITE_GRAPHICS_ID_SNIPPER_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "BLOOD.PCX", SPRITE_GRAPHICS_ID_BLOOD, 4, 4, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "FIRE1.PCX", SPRITE_GRAPHICS_ID_FIRE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "ELECTRIC1.PCX", SPRITE_GRAPHICS_ID_ELECTRIC1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "EXPLOSION.PCX", SPRITE_GRAPHICS_ID_EXPLOSION1, 32, 32, 10, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "TIMER.PCX", SPRITE_GRAPHICS_ID_TIMER, 8, 8, 10, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "CLICK.PCX", SPRITE_GRAPHICS_ID_CLICK, 5, 5, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
							aux_counter = 0;
							spark_counter++;
							break;
						case 16:
							SetDelayTime(40);
							while (!AwaitDelayTime()) {
								// Just wait
							}
							VIDEO_ClearScreenBuffer();
							VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 50, 18);
							if (VIDEO_StringToScreenBufferSteps(82, 175, ui.txt_file[UI_TXT_INTRO].line[1], FONT_BIG_WHITE, &aux_counter)) {
								spark_counter++;
							}
							AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
							break;
						case 17:
							VIDEO_StringToScreenBuffer(82, 175, ui.txt_file[UI_TXT_INTRO].line[1], FONT_BIG_WHITE);
							AUDIO_PlaySound(AUDIO_CRASH_EFFECT, 1);
							spark_end = true;
							break;
						default:
							Error("Logo function error", "Unknown logo step", "", ERROR_SYSTEM);
							break;
					}

					EFFECT_UpdateEffects();
					GFX_UpdateSprites();
					GFX_DrawSprites();
					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				logo_step = 40;
				break;
			case 40:
				GFX_PCXImageToBuffer("ILOGO.DAT", "JOY1.PCX", 120 * 160, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ILOGO.DAT", "JOY2.PCX", 120 * 160, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("ILOGO.DAT", "JOY3.PCX", 120 * 160, gfx.image_buffer3, &gfx.image_buffer3_width, &gfx.image_buffer3_height);
				VIDEO_FadeOut(1);
				logo_step = 41;
				break;
			case 41:// Load sprites
				VIDEO_ClearScreen();
				VIDEO_ClearPalette();
				GFX_LoadPalette("PALETTES.DAT", "LMANOL.PCX", 256);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_FadeIn(1);
				logo_step = 42;
				break;
			case 42:
				// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ACTORCHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 100, 50);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 43;
				break;
			case 43:
				// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM5.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_RIFLE, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 44;
				break;
			case 44:
				// Load some more assets
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, gfx.image_buffer3_width, gfx.image_buffer3_height, 100, 50);

				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 45;
			case 45:
				// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM4.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_UZI, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 46;
				break;
			case 46:
				// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM3.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_SHOTGUN, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ISPR.DAT", "ACTOR.PCX", SPRITE_GRAPHICS_ID_ACTOR_INTRO, 20, 33, 12, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "RAIN.PCX", SPRITE_GRAPHICS_ID_RAIN_EFFECT, 5, 5, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 100, 50);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 47;
				break;
			case 47:
				// Load some more assets
				GFX_LoadSpriteGraphicsRLE("ITEMS.DAT", "MEDIKIT.PCX", SPRITE_GRAPHICS_ID_ITEM_MEDIKIT, 12, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
				GFX_LoadSpriteGraphicsRLE("ITEMS.DAT", "AMMO3.PCX", SPRITE_GRAPHICS_ID_ITEM_AMMO3, 15, 13, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				char_counter = 0;
				aux_counter = 0;
				logo_step = 48;
				break;
			case 48:
				VIDEO_ClearScreenBuffer();
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);

				if (VIDEO_StringToScreenBufferRand(40, 15, ui.txt_file[UI_TXT_INTRO].line[3], FONT_BIG_WHITE, &char_counter, &aux_counter)) {
					aux_counter = 0;
					logo_step = 49;
				}
				AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);

				SetDelayTime(40);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				break;
			case 49:
				SetDelayTime(80);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				VIDEO_ClearScreenBuffer();
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_StringToScreenBuffer(40, 15, ui.txt_file[UI_TXT_INTRO].line[3], FONT_BIG_WHITE);
				if (VIDEO_StringToScreenBufferSteps(80, 30, ui.txt_file[UI_TXT_INTRO].line[4], FONT_BIG_WHITE, &aux_counter)) {
					aux_counter = 0;
					logo_step = 50;
				}
				AUDIO_PlaySound(AUDIO_TICK_EFFECT, 1);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				break;
			case 50:
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 100, 50);
				VIDEO_StringToScreenBuffer(40, 15, ui.txt_file[UI_TXT_INTRO].line[3], FONT_BIG_WHITE);
				VIDEO_StringToScreenBuffer(80, 30, ui.txt_file[UI_TXT_INTRO].line[4], FONT_BIG_WHITE);
				AUDIO_PlaySound(AUDIO_CRASH_EFFECT, 1);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				logo_step = 51;
				break;
			case 51:
				AUDIO_LoadSong(AUDIO_SONG_1);
				GFX_PCXImageToBuffer("INTRO.DAT", "INTRO1.PCX", 224 * 32, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("INTRO.DAT", "INTRO2.PCX", 640 * 32, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("INTRO.DAT", "INTRO3.PCX", 640 * 96, gfx.image_buffer3, &gfx.image_buffer3_width, &gfx.image_buffer3_height);
				logo_step = 52;
				break;
			case 52:
				engine.logo = false;
				break;
			default:
				engine.logo = false;
				break;
		}
	}

	VIDEO_FadeOut(1);
	SetDelayTime(0);
	GFX_UnloadSprites();

	// shares logo sequence is ended
	engine.logo = false;
}

static void Intro(void) {
	int step;
	int i, scroll_counter;
	int horizontal_scroll2, max_scroll2, src_index2, dst_index2;
	int horizontal_scroll3, max_scroll3, src_index3, dst_index3;
	int actor_spr_num;
	int speech_step;
	bool scroll_end;

	engine.sequence = true;
	step = 0;
	while (engine.sequence) {
		switch (step) {
			case 0:// Show screen and play music
				VIDEO_ClearScreenBuffer();
				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 50);
				AUDIO_PlaySong(true);
				GFX_LoadPalette("PALETTES.DAT", "INTRO.PCX", 256);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 230, 168, 45, 12, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 48, 16);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 224, gfx.image_buffer2_height, 48, 48);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 224, gfx.image_buffer3_height, 48, 80);
				VIDEO_StringToScreenBuffer(40, 185, ui.txt_file[UI_TXT_INTRO].line[25], FONT_SLIM_WHITE);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(4);
				step++;
				break;
			case 1:// load small actor
				actor_spr_num = GFX_FindEmptySpriteSlot();
				if (actor_spr_num == -1) {
					sprintf(engine.system_error_message1, "Intro function error");
					sprintf(engine.system_error_message2, "No empty sprite slot available");
					sprintf(engine.system_error_message3, " ");
					Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
				} else {
					GFX_InitSprite(ENTITY_ID_EMPTY, 0, actor_spr_num, 0, 20, 33);
				}
				GFX_SetSpriteGraphic(actor_spr_num, 0, SPRITE_GRAPHICS_ID_ACTOR_INTRO, 0, 0);
				GFX_SetDefaultAnimation(actor_spr_num, false, true, 10);
				GFX_SetSpritePosition(actor_spr_num, 48, 142);
				speech_step = 0;
				scroll_counter = 0;
				step++;
				break;
			case 2:// actor walk to middle screen
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 48, 16);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 224, gfx.image_buffer2_height, 48, 48);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 224, gfx.image_buffer3_height, 48, 80);
				scroll_counter++;
				if (scroll_counter >= 5) {
					GFX_SetSpritePosition(actor_spr_num, GFX_GetSpriteScreenPosX(actor_spr_num) + 1, GFX_GetSpriteScreenPosY(actor_spr_num));
					scroll_counter = 0;
				}

				// Rain effect
				if (effects_counter < 20) EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_RAIN_EFFECT, 48 + rand() % 219, 50 + rand() % 110, true, 0, false, false, 1);

				// next speech
				if (GFX_GetSpriteScreenPosX(actor_spr_num) == 52) {
					speech_step = 1;
				}

				// next speech
				if (GFX_GetSpriteScreenPosX(actor_spr_num) == 110) {
					speech_step = 2;
				}

				// next speech
				if (GFX_GetSpriteScreenPosX(actor_spr_num) == 115) {
					speech_step = 3;
				}

				// end of step
				if (GFX_GetSpriteScreenPosX(actor_spr_num) == 160) {
					src_index2 = 0;
					horizontal_scroll2 = 0;
					max_scroll2 = gfx.image_buffer2_width - 224;
					src_index3 = 0;
					horizontal_scroll3 = 0;
					max_scroll3 = gfx.image_buffer3_width - 224;
					scroll_end = false;
					scroll_counter = 0;
					step++;
				}
				break;
			case 3:// Start scrolling

				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 48, 16);

				if (scroll_counter % 32 == 0) {
					// Draw buffer 2. slow scroll
					src_index2 = horizontal_scroll2;
					dst_index2 = 48 * CAM_VISIBLE_WIDTH + 48;
					for (i = 0; i < gfx.image_buffer2_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index2], &gfx.image_buffer2[src_index2], 224);
						src_index2 += gfx.image_buffer2_width;
						dst_index2 += CAM_VISIBLE_WIDTH;
					}
					horizontal_scroll2 += 1;
				}
				if (horizontal_scroll2 >= max_scroll2) scroll_end = true;


				if (scroll_counter % 8 == 0) {

					// Draw buffer 3. fast scroll
					src_index3 = horizontal_scroll3;
					dst_index3 = 80 * CAM_VISIBLE_WIDTH + 48;
					for (i = 0; i < gfx.image_buffer3_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index3], &gfx.image_buffer3[src_index3], 224);
						src_index3 += gfx.image_buffer3_width;
						dst_index3 += CAM_VISIBLE_WIDTH;
					}
					horizontal_scroll3 += 1;
				}
				if (horizontal_scroll3 >= max_scroll3) scroll_end = true;

				if (horizontal_scroll3 == 1) {
					speech_step = 4;
				}

				if (horizontal_scroll3 == 6) {
					speech_step = 5;
				}

				if (horizontal_scroll3 == 65) {
					speech_step = 6;
				}

				if (horizontal_scroll3 == 70) {
					speech_step = 7;
				}

				if (horizontal_scroll3 == 120) {
					speech_step = 8;
				}

				if (horizontal_scroll3 == 125) {
					speech_step = 9;
				}

				if (horizontal_scroll3 == 185) {
					speech_step = 10;
				}

				if (horizontal_scroll3 == 190) {
					speech_step = 11;
				}

				if (horizontal_scroll3 == 260) {
					speech_step = 12;
				}

				if (horizontal_scroll3 == 265) {
					speech_step = 13;
				}

				if (horizontal_scroll3 == 325) {
					speech_step = 14;
				}

				if (horizontal_scroll3 == 330) {
					speech_step = 15;
				}

				if (horizontal_scroll3 == 400) {
					speech_step = 16;
				}

				if (horizontal_scroll3 == 405) {
					speech_step = 17;
				}

				// increase scroll counter
				scroll_counter++;

				if (effects_counter < 20) EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_RAIN_EFFECT, 48 + rand() % 219, 50 + rand() % 110, true, 0, false, false, 1);

				if (scroll_end) step++;
				break;
			case 4:
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 48, 16);
				src_index2 = horizontal_scroll2;
				dst_index2 = 48 * CAM_VISIBLE_WIDTH + 48;
				for (i = 0; i < gfx.image_buffer2_height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index2], &gfx.image_buffer2[src_index2], 224);
					src_index2 += gfx.image_buffer2_width;
					dst_index2 += CAM_VISIBLE_WIDTH;
				}
				src_index3 = horizontal_scroll3;
				dst_index3 = 80 * CAM_VISIBLE_WIDTH + 48;
				for (i = 0; i < gfx.image_buffer3_height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index3], &gfx.image_buffer3[src_index3], 224);
					src_index3 += gfx.image_buffer3_width;
					dst_index3 += CAM_VISIBLE_WIDTH;
				}
				scroll_counter++;
				if (scroll_counter >= 5) {
					GFX_SetSpritePosition(actor_spr_num, GFX_GetSpriteScreenPosX(actor_spr_num) + 1, GFX_GetSpriteScreenPosY(actor_spr_num));
					scroll_counter = 0;
				}

				// Rain effect
				if (effects_counter < 20) EFFECT_LoadEffect(ENTITY_ID_EMPTY, SPRITE_GRAPHICS_ID_RAIN_EFFECT, 48 + rand() % 219, 50 + rand() % 110, true, 0, false, false, 1);

				// end of step
				if (GFX_GetSpriteScreenPosX(actor_spr_num) == 250) {
					step++;
				}

				break;
			case 5:
				engine.sequence = false;
				break;
			default:
				engine.sequence = false;
				break;
		}

		switch (speech_step) {
			case 0:// no speech
				break;
			case 1:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[5], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[6], FONT_SLIM_BLACK);
				break;
			case 2:// silence
				break;
			case 3:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[7], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[8], FONT_SLIM_BLACK);
				break;
			case 4:// silence
				break;
			case 5:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[9], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[10], FONT_SLIM_BLACK);
				break;
			case 6:// silence
				break;
			case 7:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[11], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[12], FONT_SLIM_BLACK);
				break;
			case 8:// silence
				break;
			case 9:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[13], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[14], FONT_SLIM_BLACK);
				break;
			case 10:// silence
				break;
			case 11:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[15], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[16], FONT_SLIM_BLACK);
				break;
			case 12:// silence
				break;
			case 13:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[17], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[18], FONT_SLIM_BLACK);
				break;
			case 14:// silence
				break;
			case 15:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[19], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[20], FONT_SLIM_BLACK);
				break;
			case 16:
				break;
			case 17:// speech 1
				VIDEO_StringToScreenBuffer(52, 22, ui.txt_file[UI_TXT_INTRO].line[21], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(52, 30, ui.txt_file[UI_TXT_INTRO].line[22], FONT_SLIM_BLACK);
				break;
			case 18:
				break;
			default:
				break;
		}

		EFFECT_UpdateEffects();
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	VIDEO_FadeOut(4);
	AUDIO_StopSong();

	AUDIO_UnloadSong();
	EFFECT_UnloadEffects();
	GFX_UnloadSprites();
}

/** Main menu
 */
static void Menu(void) {

	bool exit_menu;
	bool show_main_menu, hide_main_menu, main_menu_shown, main_menu_hidden;
	bool show_options_menu, hide_options_menu, options_menu_shown, options_menu_hidden;
	bool show_password_menu, hide_password_menu, password_menu_shown, password_menu_hidden;
	bool show_credits, hide_credits, credits_shown, credits_hidden;
	int credits_step;
	int i;
	int title_spr_num1, title_spr_num2, opt_pnl_spr_num, pass_pnl_spr_num, chat_pnl_spr_num;
	unsigned char sound_volume[5];
	unsigned char music_volume[5];

	char password[13];
	byte password_step;
	byte key;

	exit_menu = false;

	show_main_menu = false;
	hide_main_menu = false;
	main_menu_hidden = false;
	main_menu_shown = true;

	show_options_menu = false;
	hide_options_menu = false;
	options_menu_hidden = true;
	options_menu_shown = false;

	show_password_menu = false;
	hide_password_menu = false;
	password_menu_hidden = true;
	password_menu_shown = false;

	show_credits = false;
	hide_credits = false;
	credits_shown = false;
	credits_hidden = true;
	credits_step = 0;

	int char_number = 0;
	char aux_string[40];

	VIDEO_ClearScreen();

	GFX_LoadPalette("PALETTES.DAT", "MENU.PCX", 256);
	CAM_SetPosition(0, 0);

	AUDIO_LoadSong(AUDIO_SONG_2);

	TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 50);
	AUDIO_PlaySong(true);

	VIDEO_FadeIn(4);

	// ---- Title scroll in the middle screen
	title_spr_num1 = GFX_FindEmptySpriteSlot();
	GFX_InitSprite(0, 0, title_spr_num1, 1, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_TITLE].width_px, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_TITLE].height_px);
	GFX_SetSpriteGraphic(title_spr_num1, 0, SPRITE_GRAPHICS_ID_TITLE, 0, 0);
	GFX_SetSpritePosition(title_spr_num1, 0 - gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_TITLE].width_px, 100);

	title_spr_num2 = GFX_FindEmptySpriteSlot();
	GFX_InitSprite(0, 0, title_spr_num2, 1, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_TITLE].width_px, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_TITLE].height_px);
	GFX_SetSpriteGraphic(title_spr_num2, 0, SPRITE_GRAPHICS_ID_TITLE, 0, 0);
	GFX_SetSpritePosition(title_spr_num2, video.screen_width, 100);

	for (i = 0; i < 53; i++) {
		VIDEO_ClearScreenBuffer();
		if (i < 40) {
			GFX_SetSpritePosition(title_spr_num1, gfx_sprite_stack[title_spr_num1].screen_pos_x + 6, 100);
			GFX_SetSpritePosition(title_spr_num2, gfx_sprite_stack[title_spr_num2].screen_pos_x - 6, 100);
		} else if (i < 48) {
			GFX_SetSpritePosition(title_spr_num1, gfx_sprite_stack[title_spr_num1].screen_pos_x + 4, 100);
			GFX_SetSpritePosition(title_spr_num2, gfx_sprite_stack[title_spr_num2].screen_pos_x - 4, 100);
		} else if (i < 52) {
			GFX_SetSpritePosition(title_spr_num1, gfx_sprite_stack[title_spr_num1].screen_pos_x + 2, 100);
			GFX_SetSpritePosition(title_spr_num2, gfx_sprite_stack[title_spr_num2].screen_pos_x - 2, 100);
		} else {
			GFX_SetSpritePosition(title_spr_num1, gfx_sprite_stack[title_spr_num1].screen_pos_x + 1, 100);
			GFX_SetSpritePosition(title_spr_num2, gfx_sprite_stack[title_spr_num2].screen_pos_x - 1, 100);
		}

		GFX_UpdateSprites();

		VIDEO_DrawSpriteInterlacedToScreenBufferRLE(&gfx_sprite_graphics_stack[gfx_sprite_stack[title_spr_num1].gfx[0].graphics_id], &gfx_sprite_stack[title_spr_num1].gfx[0], 0);
		VIDEO_DrawSpriteInterlacedToScreenBufferRLE(&gfx_sprite_graphics_stack[gfx_sprite_stack[title_spr_num2].gfx[0].graphics_id], &gfx_sprite_stack[title_spr_num2].gfx[0], 1);

		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}
	GFX_UnloadSprite(title_spr_num2);
	GFX_UnloadSprite(title_spr_num2);
	GFX_UpdateSprites();
	GFX_DrawSprites();
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	/// ------------------


	GFX_PCXImageToBuffer("IMENU.DAT", "MENUBACK.PCX", 360 * 200, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);

	GFX_UnloadSprite(title_spr_num2);
	GFX_UpdateSprites();
	GFX_DrawSprites();
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();

	// move title down
	while (GFX_GetSpriteScreenPosY(title_spr_num1) < 140) {
		VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_SetSpritePosition(title_spr_num1, GFX_GetSpriteScreenPosX(title_spr_num1), GFX_GetSpriteScreenPosY(title_spr_num1) + 1);
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	while (char_number < strlen(ui.txt_file[UI_TXT_INTRO].line[2])) {
		aux_string[char_number] = ui.txt_file[UI_TXT_INTRO].line[2][char_number];
		aux_string[char_number + 1] = '\0';
		VIDEO_StringToScreenBuffer(40, 180, aux_string, FONT_BIG_WHITE);
		char_number++;
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	// Main menu buttons
	UI_LoadButton(1, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_NEW, 320, 16);
	UI_LoadButton(2, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_PASS, 340, 40);
	UI_LoadButton(3, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_OPT, 360, 64);
	UI_LoadButton(4, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_CREDITS, 380, 88);
	UI_LoadButton(5, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_EXIT, 400, 112);

	// Options assets
	opt_pnl_spr_num = GFX_FindEmptySpriteSlot();
	if (opt_pnl_spr_num == -1) {
		sprintf(engine.system_error_message1, "Loading options panel function error");
		sprintf(engine.system_error_message2, "No empty sprite slot available");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}
	GFX_InitSprite(0, 0, opt_pnl_spr_num, 0, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_PNL_OPTIONS].width_px, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_PNL_OPTIONS].height_px);
	GFX_SetSpriteGraphic(opt_pnl_spr_num, 0, SPRITE_GRAPHICS_ID_PNL_OPTIONS, 0, 0);
	GFX_SetSpritePosition(opt_pnl_spr_num, 320, 5);

	UI_LoadButton(6, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_BACK, 345, 135);  // Back
	UI_LoadButton(7, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_LEFT, 324, 35);   // Sound volume left
	UI_LoadButton(8, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_RIGHT, 365, 35);  // Sound volume right
	UI_LoadButton(9, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_LEFT, 324, 75);   // Music volume left
	UI_LoadButton(10, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_RIGHT, 365, 75); // Music volume right
	UI_LoadButton(11, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_LEFT, 324, 115); // Language left
	UI_LoadButton(12, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_RIGHT, 365, 116);// Language right

	// Password assets
	pass_pnl_spr_num = GFX_FindEmptySpriteSlot();
	if (pass_pnl_spr_num == -1) {
		sprintf(engine.system_error_message1, "Loading password panel function error");
		sprintf(engine.system_error_message2, "No empty sprite slot available");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}
	GFX_InitSprite(0, 0, pass_pnl_spr_num, 0, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_PNL_PASS].width_px, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_PNL_OPTIONS].height_px);
	GFX_SetSpriteGraphic(pass_pnl_spr_num, 0, SPRITE_GRAPHICS_ID_PNL_PASS, 0, 0);
	GFX_SetSpritePosition(pass_pnl_spr_num, 320, 10);
	UI_LoadButton(13, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_OK, 321, 55);  // OK
	UI_LoadButton(14, ENTITY_ID_BTN, SPRITE_GRAPHICS_ID_BTN_BACK, 410, 55);// Back

	// Credits assets
	chat_pnl_spr_num = GFX_FindEmptySpriteSlot();
	if (chat_pnl_spr_num == -1) {
		sprintf(engine.system_error_message1, "Loading chat panel function error");
		sprintf(engine.system_error_message2, "No empty sprite slot available");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}
	GFX_InitSprite(0, 0, chat_pnl_spr_num, 0, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_MCHAT].width_px, gfx_sprite_graphics_stack[SPRITE_GRAPHICS_ID_MCHAT].height_px);
	GFX_SetSpriteGraphic(chat_pnl_spr_num, 0, SPRITE_GRAPHICS_ID_MCHAT, 0, 0);
	GFX_SetSpritePosition(chat_pnl_spr_num, 320, 20);

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
		VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		GFX_UpdateSprites();
		VIDEO_StringToScreenBuffer(40, 180, ui.txt_file[UI_TXT_INTRO].line[2], FONT_BIG_WHITE);
		GFX_DrawSprites();
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	exit_menu = false;
	show_main_menu = true;

	GFX_InitSprite(ENTITY_ID_EMPTY, 0, 20, 1, 7, 12);
	GFX_SetSpriteGraphic(20, 0, SPRITE_GRAPHICS_ID_CURSOR, 0, 0);
	GFX_SetSpritePosition(20, 20, 20);

	while (!exit_menu) {

		UI_UpdateUI(false);
		MOUSE_Update(false);
		EFFECT_UpdateEffects();

		GFX_UpdateSprites();
		VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 0, 0);
		VIDEO_StringToScreenBuffer(40, 180, ui.txt_file[UI_TXT_INTRO].line[2], FONT_BIG_WHITE);
		GFX_DrawSprites();
		GFX_DrawCursorSprite();

		// show main menu
		if (show_main_menu && options_menu_hidden && credits_hidden && password_menu_hidden) {
			if (UI_GetButtonXPosition(1) > 250) UI_SetButtonPosition(1, UI_GetButtonXPosition(1) - 4, UI_GetButtonYPosition(1));
			if (UI_GetButtonXPosition(2) > 250) UI_SetButtonPosition(2, UI_GetButtonXPosition(2) - 4, UI_GetButtonYPosition(2));
			if (UI_GetButtonXPosition(3) > 250) UI_SetButtonPosition(3, UI_GetButtonXPosition(3) - 4, UI_GetButtonYPosition(3));
			if (UI_GetButtonXPosition(4) > 250) UI_SetButtonPosition(4, UI_GetButtonXPosition(4) - 4, UI_GetButtonYPosition(4));
			if (UI_GetButtonXPosition(5) > 250) UI_SetButtonPosition(5, UI_GetButtonXPosition(5) - 4, UI_GetButtonYPosition(5));

			if (UI_GetButtonXPosition(5) <= 250) {
				show_main_menu = false;
				hide_main_menu = false;
				main_menu_shown = true;
				main_menu_hidden = false;
			}
		}

		// hide main menu
		if (hide_main_menu) {
			UI_SetButtonPosition(1, UI_GetButtonXPosition(1) + 4, UI_GetButtonYPosition(1));
			UI_SetButtonPosition(2, UI_GetButtonXPosition(2) + 4, UI_GetButtonYPosition(2));
			UI_SetButtonPosition(3, UI_GetButtonXPosition(3) + 4, UI_GetButtonYPosition(3));
			UI_SetButtonPosition(4, UI_GetButtonXPosition(4) + 4, UI_GetButtonYPosition(4));
			UI_SetButtonPosition(5, UI_GetButtonXPosition(5) + 4, UI_GetButtonYPosition(5));

			if (UI_GetButtonXPosition(5) >= 320) {
				show_main_menu = false;
				hide_main_menu = false;
				main_menu_hidden = true;
				main_menu_shown = false;
			}
		}

		// Show options menu
		if (show_options_menu && main_menu_hidden && credits_hidden && password_menu_hidden) {
			GFX_SetSpritePosition(opt_pnl_spr_num, gfx_sprite_stack[opt_pnl_spr_num].screen_pos_x - 4, gfx_sprite_stack[opt_pnl_spr_num].screen_pos_y);
			UI_SetButtonPosition(6, UI_GetButtonXPosition(6) - 4, UI_GetButtonYPosition(6));
			UI_SetButtonPosition(7, UI_GetButtonXPosition(7) - 4, UI_GetButtonYPosition(7));
			UI_SetButtonPosition(8, UI_GetButtonXPosition(8) - 4, UI_GetButtonYPosition(8));
			UI_SetButtonPosition(9, UI_GetButtonXPosition(9) - 4, UI_GetButtonYPosition(9));
			UI_SetButtonPosition(10, UI_GetButtonXPosition(10) - 4, UI_GetButtonYPosition(10));
			UI_SetButtonPosition(11, UI_GetButtonXPosition(11) - 4, UI_GetButtonYPosition(11));
			UI_SetButtonPosition(12, UI_GetButtonXPosition(12) - 4, UI_GetButtonYPosition(12));

			if (gfx_sprite_stack[opt_pnl_spr_num].screen_pos_x <= 250) {
				show_options_menu = false;
				hide_options_menu = false;
				options_menu_hidden = false;
				options_menu_shown = true;
			}
		}

		// Options menu shown
		if (options_menu_shown) {
			// Sound volume
			sprintf(sound_volume, "%03d\n", settings.sound_volume);
			VIDEO_StringToScreenBuffer(268, 40, sound_volume, FONT_SLIM_WHITE);

			// Music volume
			sprintf(music_volume, "%03d\n", settings.music_volume);
			VIDEO_StringToScreenBuffer(268, 80, music_volume, FONT_SLIM_WHITE);

			// Language
			switch (settings.language) {
				case UI_LANGUAGE_ES:// spanish
					VIDEO_StringToScreenBuffer(266, 120, ui.txt_file[UI_TXT_GLOBAL].line[18], FONT_SLIM_WHITE);
					break;
				case UI_LANGUAGE_EN:// english
					VIDEO_StringToScreenBuffer(266, 120, ui.txt_file[UI_TXT_GLOBAL].line[19], FONT_SLIM_WHITE);
					break;
				case UI_LANGUAGE_FR:// French
					VIDEO_StringToScreenBuffer(266, 120, ui.txt_file[UI_TXT_GLOBAL].line[20], FONT_SLIM_WHITE);
					break;
				default:
					break;
			}
		}

		// Hide options menu
		if (hide_options_menu) {
			options_menu_shown = false;
			GFX_SetSpritePosition(opt_pnl_spr_num, gfx_sprite_stack[opt_pnl_spr_num].screen_pos_x + 4, gfx_sprite_stack[opt_pnl_spr_num].screen_pos_y);
			UI_SetButtonPosition(6, UI_GetButtonXPosition(6) + 4, UI_GetButtonYPosition(6));
			UI_SetButtonPosition(7, UI_GetButtonXPosition(7) + 4, UI_GetButtonYPosition(7));
			UI_SetButtonPosition(8, UI_GetButtonXPosition(8) + 4, UI_GetButtonYPosition(8));
			UI_SetButtonPosition(9, UI_GetButtonXPosition(9) + 4, UI_GetButtonYPosition(9));
			UI_SetButtonPosition(10, UI_GetButtonXPosition(10) + 4, UI_GetButtonYPosition(10));
			UI_SetButtonPosition(11, UI_GetButtonXPosition(11) + 4, UI_GetButtonYPosition(11));
			UI_SetButtonPosition(12, UI_GetButtonXPosition(12) + 4, UI_GetButtonYPosition(12));
			if (gfx_sprite_stack[opt_pnl_spr_num].screen_pos_x >= 320) {
				show_options_menu = false;
				hide_options_menu = false;
				options_menu_hidden = true;
			}
		}

		// Show password menu
		if (show_password_menu && main_menu_hidden && credits_hidden && options_menu_hidden) {
			GFX_SetSpritePosition(pass_pnl_spr_num, gfx_sprite_stack[pass_pnl_spr_num].screen_pos_x - 4, gfx_sprite_stack[pass_pnl_spr_num].screen_pos_y);
			UI_SetButtonPosition(13, UI_GetButtonXPosition(13) - 4, UI_GetButtonYPosition(13));
			UI_SetButtonPosition(14, UI_GetButtonXPosition(14) - 4, UI_GetButtonYPosition(14));

			if (gfx_sprite_stack[pass_pnl_spr_num].screen_pos_x <= 190) {
				show_password_menu = false;
				hide_password_menu = false;
				password_menu_hidden = false;
				password_menu_shown = true;

				// Reset last password
				password_step = 0;
				password[0] = '*';
				password[1] = ' ';
				password[2] = '*';
				password[3] = ' ';
				password[4] = '*';
				password[5] = ' ';
				password[6] = '*';
				password[7] = ' ';
				password[8] = '*';
				password[9] = ' ';
				password[10] = '*';
				password[11] = ' ';
				password[12] = 0;
			}
		}

		// Password menu shown
		if (password_menu_shown) {
			VIDEO_StringToScreenBuffer(206, 35, password, FONT_SLIM_WHITE);
			switch (password_step) {
				case 0:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 1:// 1st char
					if (KEYB_IsAnyKeyPressed()) {
						key = KEYB_GetLastKeyPressed_ASCII();
						if (key != 0) {
							password[0] = key;
							SetDelayTime(200);
							password_step++;
						} else {
							AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
						}
					}
					break;
				case 2:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 3:// 2nd char
					if (KEYB_IsAnyKeyPressed()) {
						if (kbKeyState[SCANCODE_BACKSPACE]) {
							password[0] = '*';
							password_step = 0;
							SetDelayTime(200);
						} else {
							key = KEYB_GetLastKeyPressed_ASCII();
							if (key != 0) {
								password[2] = key;
								SetDelayTime(200);
								password_step++;
							} else {
								AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
							}
						}
					}
					break;
				case 4:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 5:// 3rd char
					if (KEYB_IsAnyKeyPressed()) {
						if (kbKeyState[SCANCODE_BACKSPACE]) {
							password[2] = '*';
							password_step = 2;
							SetDelayTime(200);
						} else {
							key = KEYB_GetLastKeyPressed_ASCII();
							if (key != 0) {
								password[4] = key;
								SetDelayTime(200);
								password_step++;
							} else {
								AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
							}
						}
					}
					break;
				case 6:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 7:// 4th char
					if (KEYB_IsAnyKeyPressed()) {
						if (kbKeyState[SCANCODE_BACKSPACE]) {
							password[4] = '*';
							password_step = 4;
							SetDelayTime(200);
						} else {
							key = KEYB_GetLastKeyPressed_ASCII();
							if (key != 0) {
								password[6] = key;
								SetDelayTime(200);
								password_step++;
							} else {
								AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
							}
						}
					}
					break;
				case 8:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 9:// 5th char
					if (KEYB_IsAnyKeyPressed()) {
						if (kbKeyState[SCANCODE_BACKSPACE]) {
							password[6] = '*';
							password_step = 6;
							SetDelayTime(200);
						} else {
							key = KEYB_GetLastKeyPressed_ASCII();
							if (key != 0) {
								password[8] = key;
								SetDelayTime(200);
								password_step++;
							} else {
								AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
							}
						}
					}
					break;
				case 10:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 11:// 6th char
					if (KEYB_IsAnyKeyPressed()) {
						if (kbKeyState[SCANCODE_BACKSPACE]) {
							password[8] = '*';
							password_step = 8;
							SetDelayTime(200);
						} else {
							key = KEYB_GetLastKeyPressed_ASCII();
							if (key != 0) {
								password[10] = key;
								SetDelayTime(200);
								password_step++;
							} else {
								AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
							}
						}
					}
					break;
				case 12:// wait
					if (AwaitDelayTime()) password_step++;
					break;
				case 13:// All chars done
					if (kbKeyState[SCANCODE_BACKSPACE]) {
						password[10] = '*';
						password_step = 10;
					}
					break;
				default:
					break;
			}
		}

		// Hide password menu
		if (hide_password_menu) {
			password_menu_shown = false;
			GFX_SetSpritePosition(pass_pnl_spr_num, gfx_sprite_stack[pass_pnl_spr_num].screen_pos_x + 4, gfx_sprite_stack[pass_pnl_spr_num].screen_pos_y);
			UI_SetButtonPosition(13, UI_GetButtonXPosition(13) + 4, UI_GetButtonYPosition(13));
			UI_SetButtonPosition(14, UI_GetButtonXPosition(14) + 4, UI_GetButtonYPosition(14));
			if (gfx_sprite_stack[pass_pnl_spr_num].screen_pos_x >= 320) {
				show_password_menu = false;
				hide_password_menu = false;
				password_menu_hidden = true;
			}
		}

		// Show credits
		if (show_credits && main_menu_hidden && options_menu_hidden && password_menu_hidden) {
			GFX_SetSpritePosition(chat_pnl_spr_num, 160, 10);
			SetDelayTime(2000);
			hide_credits = false;
			show_credits = false;
			credits_hidden = false;
			credits_shown = true;
		}

		// Hide credits
		if (hide_credits) {
			GFX_SetSpritePosition(chat_pnl_spr_num, 320, 10);
			hide_credits = false;
			show_credits = false;
			credits_shown = false;
			credits_hidden = true;
			credits_step = 0;
			show_main_menu = true;
		}

		// Credits being shown
		if (credits_shown) {
			switch (credits_step) {
				case 0:
					VIDEO_StringToScreenBuffer(180, 40, ui.txt_file[UI_TXT_CREDITS].line[1], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 60, ui.txt_file[UI_TXT_CREDITS].line[2], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 80, ui.txt_file[UI_TXT_CREDITS].line[3], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 1:
					SetDelayTime(2000);
					credits_step++;
					break;
				case 2:
					VIDEO_StringToScreenBuffer(180, 40, ui.txt_file[UI_TXT_CREDITS].line[4], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 60, ui.txt_file[UI_TXT_CREDITS].line[5], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 80, ui.txt_file[UI_TXT_CREDITS].line[6], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 3:
					SetDelayTime(2000);
					credits_step++;
					break;
				case 4:
					VIDEO_StringToScreenBuffer(180, 40, ui.txt_file[UI_TXT_CREDITS].line[7], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 60, ui.txt_file[UI_TXT_CREDITS].line[8], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 80, ui.txt_file[UI_TXT_CREDITS].line[9], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 5:
					SetDelayTime(2000);
					credits_step++;
					break;
				case 6:
					VIDEO_StringToScreenBuffer(180, 40, ui.txt_file[UI_TXT_CREDITS].line[10], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 60, ui.txt_file[UI_TXT_CREDITS].line[11], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(180, 80, ui.txt_file[UI_TXT_CREDITS].line[12], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 7:
					SetDelayTime(3000);
					credits_step++;
					break;
				case 8:
					VIDEO_StringToScreenBuffer(170, 30, ui.txt_file[UI_TXT_CREDITS].line[13], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(170, 50, ui.txt_file[UI_TXT_CREDITS].line[14], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(170, 70, ui.txt_file[UI_TXT_CREDITS].line[15], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 9:
					SetDelayTime(3000);
					credits_step++;
					break;
				case 10:
					VIDEO_StringToScreenBuffer(170, 30, ui.txt_file[UI_TXT_CREDITS].line[16], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(170, 50, ui.txt_file[UI_TXT_CREDITS].line[17], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(170, 70, ui.txt_file[UI_TXT_CREDITS].line[18], FONT_SLIM_BLACK);
					if (AwaitDelayTime()) {
						credits_step++;
					}
					break;
				case 11:
					hide_credits = true;
					break;
				default:
					break;
			}
		}

		// Option selected
		switch (ui.button_pressed) {
			case 0:// No button pressed
				break;
			case 1:// New game
				exit_menu = true;
				engine.scene = 1;
				engine.room = 1;
				break;
			case 2:// Password
				hide_main_menu = true;
				show_password_menu = true;
				break;
			case 3:// options
				hide_main_menu = true;
				show_options_menu = true;
				break;
			case 4:// credits
				hide_main_menu = true;
				show_credits = true;
				break;
			case 5:// exit game
				exit_menu = true;
				engine.exit_game = true;
				break;
			case 6:// hide options menu
				hide_options_menu = true;
				show_main_menu = true;
				break;
			case 7:// decrease sound volume
				if (settings.sound_volume > 0) settings.sound_volume--;
				break;
			case 8:// increase sound volume
				if (settings.sound_volume < 100) settings.sound_volume++;
				break;
			case 9:// decrease music volume
				if (settings.music_volume > 0) settings.music_volume--;
				break;
			case 10:// increase music volume
				if (settings.music_volume < 100) settings.music_volume++;
				break;
			case 11:// change language
				settings.language++;
				if (settings.language > 2) settings.language = 2;
				break;
			case 12:// change language
				settings.language--;
				if (settings.language < 0) settings.language = 0;
				break;
			case 13:// Password confirmation
				// Chapter 1: Room 2: 'The travel' >> TRAVEL
				if ((password[0] == 'T') && (password[2] == 'R') && (password[4] == 'A') && (password[6] == 'V') && (password[8] == 'E') && (password[10] == 'L')) {
					exit_menu = true;
					engine.scene = 1;
					engine.room = 2;
				}

				// Chapter 2: Room 1: 'Down to the hell' >> GODOWN
				if ((password[0] == 'G') && (password[2] == 'O') && (password[4] == 'D') && (password[6] == 'O') && (password[8] == 'W') && (password[10] == 'N')) {
					exit_menu = true;
					engine.scene = 2;
					engine.room = 1;
				}

				// Chapter 3: Room 1 'Behind enemy lines' >> INSIDE
				if ((password[0] == 'I') && (password[2] == 'N') && (password[4] == 'S') && (password[6] == 'I') && (password[8] == 'D') && (password[10] == 'E')) {
					exit_menu = true;
					engine.scene = 3;
					engine.room = 1;
				}

				// Chapter 4: Room 1: 'Praise for mercy' >> PRAISE
				if ((password[0] == 'P') && (password[2] == 'R') && (password[4] == 'A') && (password[6] == 'I') && (password[8] == 'S') && (password[10] == 'E')) {
					exit_menu = true;
					engine.scene = 4;
					engine.room = 1;
				}

				// Chapter 5: Room 1: 'Hit and run' >> HITRUN
				if ((password[0] == 'H') && (password[2] == 'I') && (password[4] == 'T') && (password[6] == 'R') && (password[8] == 'U') && (password[10] == 'N')) {
					exit_menu = true;
					engine.scene = 5;
					engine.room = 1;
				}
				// Chapter 6: Room 1: 'Bullet hell' >> BULLET
				if ((password[0] == 'B') && (password[2] == 'U') && (password[4] == 'L') && (password[6] == 'L') && (password[8] == 'E') && (password[10] == 'T')) {
					exit_menu = true;
					engine.scene = 6;
					engine.room = 1;
				}

				if (engine.scene == 0) {
					AUDIO_PlaySound(AUDIO_SHOT_FAIL_EFFECT, 0);
				}

				break;
			case 14:// Hide password menu
				hide_password_menu = true;
				show_main_menu = true;
				break;
			default:
				Error("Unknown button pressed", "", "", ERROR_SYSTEM);
				break;
		}

		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}

	VIDEO_FadeOut(1);
	GFX_UnloadSprites();
	UI_UnloadButtons();
	EFFECT_UnloadEffects();
	AUDIO_StopSong();
	AUDIO_UnloadSong();
}

static void GameOver(void) {

	AUDIO_StopSong();
	VIDEO_FadeOutPause(4);
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	NPC_UnloadNpcs();
	Update(false);

	VIDEO_StringToScreenBuffer(80, 90, ui.txt_file[UI_TXT_GLOBAL].line[25], FONT_BIG_WHITE);

	Update(false);

	SetDelayTime(6000);
	while (!AwaitDelayTime()) {
		// Just wait
	}

	engine.ingame = false;
	engine.room = 0;
	engine.scene = 0;
}

/** SCENARIO 1 :: Mission 1 :: The travel
 */

static void Scene1_Intro(void) {
	int step;
	int dialog_step, dialog_delay_counter;
	int char_number;
	char aux_string[40];
	int i, horizontal_scroll, max_scroll, scanline_scroll, src_index, dst_index, src_img_index;
	bool scroll_end;
	int chat_spr_num;

	engine.sequence = true;
	step = 0;
	while (engine.sequence) {
		switch (step) {
			case 0:// Draw loading screen
				MOUSE_HideCursor();
				UI_ShowLoadingScreen();
				step++;
				break;
			case 1:// Load first intro scene
				AUDIO_LoadSong(AUDIO_SONG_3);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO0.PCX", 564 * 177, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				step++;
				break;
			case 2:// Hide loading screen
				UI_HideLoadingScreen();
				step++;
				break;
			case 3:/// Scene 1 :: City with horizontal scroll
				VIDEO_ClearScreenBuffer();
				GFX_LoadPalette("PALETTES.DAT", "ISCN1.PCX", 256);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 292, 183, 13, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 286, gfx.image_buffer1_height, 16, 14);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(4);

				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 50);
				AUDIO_PlaySong(true);
				dialog_step = 0;

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
					VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 128, 46, 25, 21, 233);
					VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 120, 40, 29, 24, 208);

					// Draw text
					switch (dialog_step) {
						case 0:
							char_number = 0;
							dialog_delay_counter = 0;
							dialog_step++;
							break;
						case 1:
							aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[1][char_number];
							aux_string[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 28, aux_string, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 4) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(ui.txt_file[UI_TXT_SCN1I].line[1])) {
								dialog_step++;
							}
							break;
						case 2:
							VIDEO_StringToScreenBuffer(38, 28, ui.txt_file[UI_TXT_SCN1I].line[1], FONT_SLIM_BLACK);
							char_number = 0;
							dialog_step++;
							break;
						case 3:
							VIDEO_StringToScreenBuffer(38, 28, ui.txt_file[UI_TXT_SCN1I].line[1], FONT_SLIM_BLACK);
							aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[2][char_number];
							aux_string[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 40, aux_string, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 4) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(ui.txt_file[UI_TXT_SCN1I].line[2])) {
								dialog_step++;
							}
							break;
						case 4:
							VIDEO_StringToScreenBuffer(38, 28, ui.txt_file[UI_TXT_SCN1I].line[1], FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, ui.txt_file[UI_TXT_SCN1I].line[2], FONT_SLIM_BLACK);
							char_number = 0;
							dialog_step++;
							break;
						case 5:
							VIDEO_StringToScreenBuffer(38, 28, ui.txt_file[UI_TXT_SCN1I].line[1], FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, ui.txt_file[UI_TXT_SCN1I].line[2], FONT_SLIM_BLACK);

							aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[3][char_number];
							aux_string[char_number + 1] = '\0';
							VIDEO_StringToScreenBuffer(38, 52, aux_string, FONT_SLIM_BLACK);
							dialog_delay_counter++;
							if (dialog_delay_counter > 4) {
								char_number++;
								dialog_delay_counter = 0;
							}
							if (char_number >= strlen(ui.txt_file[UI_TXT_SCN1I].line[3])) {
								dialog_step++;
							}
							break;
						case 6:
							VIDEO_StringToScreenBuffer(38, 28, ui.txt_file[UI_TXT_SCN1I].line[1], FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 40, ui.txt_file[UI_TXT_SCN1I].line[2], FONT_SLIM_BLACK);
							VIDEO_StringToScreenBuffer(38, 52, ui.txt_file[UI_TXT_SCN1I].line[3], FONT_SLIM_BLACK);
							break;
						default:
							break;
					}

					SetDelayTime(20);
					while (!AwaitDelayTime()) {
						// Just wait
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 5;
				break;
			case 5:// Coach horizontal scroll and text
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO1H.PCX", 251 * 135, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO1L.PCX", 205 * 135, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_FadeOut(4);
				horizontal_scroll = 36;
				scroll_end = false;

				VIDEO_ClearScreenBuffer();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 211, 140, 11, 3, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 205, gfx.image_buffer1_height, 14, 6);
				VIDEO_FadeIn(4);
				while (!scroll_end & engine.sequence) {
					src_index = horizontal_scroll;
					dst_index = CAM_VISIBLE_WIDTH * 6 + 14;

					// Draw image with scroll
					for (i = 0; i < gfx.image_buffer1_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], 205);
						src_index += gfx.image_buffer1_width;
						dst_index += CAM_VISIBLE_WIDTH;
					}
					if (horizontal_scroll > 0) horizontal_scroll--;
					else
						scroll_end = true;


					SetDelayTime(30);
					while (!AwaitDelayTime()) {
						// Just wait
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 6;
				break;
			case 6:// Clock (high)
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO2H.PCX", 105 * 56, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 111, 62, 67, 117, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 70, 120);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 7;
				break;
			case 7:// Fade coach
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 14, 6);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 111, 62, 67, 117, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 70, 120);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 8;
				break;
			case 8:// Eye
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO2L.PCX", 105 * 56, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO3H.PCX", 111 * 55, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 61, 190, 100, 222);
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[23], gfx.image_buffer1_width, gfx.image_buffer1_height, 88, gfx.image_buffer1_height, 193, 103);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 9;
				break;
			case 9:// Fade clock
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 70, 120);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 61, 190, 100, 222);
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[23], gfx.image_buffer1_width, gfx.image_buffer1_height, 88, gfx.image_buffer1_height, 193, 103);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				step = 10;
				break;
			case 10:// Scroll eye
				horizontal_scroll = 23;
				scroll_end = false;
				while (!scroll_end & engine.sequence) {
					src_index = horizontal_scroll;
					dst_index = CAM_VISIBLE_WIDTH * 103 + 193;

					// Draw image with scroll
					for (i = 0; i < gfx.image_buffer1_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], 88);
						src_index += gfx.image_buffer1_width;
						dst_index += CAM_VISIBLE_WIDTH;
					}
					if (horizontal_scroll > 0) horizontal_scroll--;
					else
						scroll_end = true;


					SetDelayTime(30);
					while (!AwaitDelayTime()) {
						// Just wait
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 11;
				break;
			case 11:// Eye chat
				chat_spr_num = GFX_FindEmptySpriteSlot();
				if (chat_spr_num == -1) {
					sprintf(engine.system_error_message1, "Scene 1 Intro function error");
					sprintf(engine.system_error_message2, "No empty sprite slot available");
					sprintf(engine.system_error_message3, " ");
					Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
				} else {
					GFX_InitSprite(ENTITY_ID_EMPTY, 0, chat_spr_num, 0, 55, 30);
				}
				GFX_SetSpriteGraphic(chat_spr_num, 0, SPRITE_GRAPHICS_ID_CHAT_SMALL, 0, 0);
				GFX_SetDefaultAnimation(chat_spr_num, false, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 250, 125);
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				GFX_UpdateSprites();
				GFX_DrawSprites();
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				char_number = 0;
				dialog_delay_counter = 0;
				while ((char_number < strlen(ui.txt_file[UI_TXT_SCN1I].line[4])) & engine.sequence) {
					aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[4][char_number];
					aux_string[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(256, 134, aux_string, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 12;
				break;
			case 12:// Fuck scene
				GFX_LoadSpriteGraphicsRLE("MISC.DAT", "FUCK.PCX", SPRITE_GRAPHICS_ID_FUCK, 158, 81, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO4H.PCX", 250 * 180, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO4L.PCX", 194 * 180, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_FadeOut(4);
				GFX_UnloadSprite(chat_spr_num);

				horizontal_scroll = 50;
				scroll_end = false;

				VIDEO_ClearScreenBuffer();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 200, 186, 110, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 194, gfx.image_buffer1_height, 113, 13);
				VIDEO_FadeIn(4);
				while (!scroll_end & engine.sequence) {
					src_index = horizontal_scroll;
					dst_index = CAM_VISIBLE_WIDTH * 13 + 113;

					// Draw image with scroll
					for (i = 0; i < gfx.image_buffer1_height; i++) {
						memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], 194);
						src_index += gfx.image_buffer1_width;
						dst_index += CAM_VISIBLE_WIDTH;
					}
					if (horizontal_scroll > 0) horizontal_scroll--;
					else
						scroll_end = true;


					SetDelayTime(30);
					while (!AwaitDelayTime()) {
						// Just wait
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 13;
				break;
			case 13:// chat
				chat_spr_num = GFX_FindEmptySpriteSlot();
				if (chat_spr_num == -1) {
					sprintf(engine.system_error_message1, "Scene 1 Intro function error");
					sprintf(engine.system_error_message2, "No empty sprite slot available");
					sprintf(engine.system_error_message3, " ");
					Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
				} else {
					GFX_InitSprite(ENTITY_ID_EMPTY, 0, chat_spr_num, 0, 158, 81);
				}
				GFX_SetSpriteGraphic(chat_spr_num, 0, SPRITE_GRAPHICS_ID_FUCK, 0, 0);
				GFX_SetDefaultAnimation(chat_spr_num, false, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 120, 25);
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				GFX_UpdateSprites();
				GFX_DrawSprites();
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 14;
				break;
			case 14:// Unload sprite
				GFX_UnloadSprite(chat_spr_num);
				GFX_UpdateSprites();
				GFX_DrawSprites();
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(300);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 15;
				break;
			case 15:// Washing face (high)
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO5H.PCX", 130 * 60, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 136, 66, 10, 30, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 13, 33);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 16;
				break;
			case 16:// Fade fuck image
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 113, 13);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 136, 66, 10, 30, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 13, 33);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step = 17;
				break;
			case 17:// Dead image
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO6H.PCX", 130 * 60, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO5L.PCX", 130 * 60, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 136, 66, 10, 120, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 13, 123);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 18;
				break;
			case 18:// Fade washing face image
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 13, 33);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 136, 66, 10, 120, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 13, 123);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 19;
				break;
			case 19:// Fade out and show image
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO7H.PCX", 285 * 97, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO8H.PCX", 82 * 100, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_FadeOut(4);

				VIDEO_ClearScreenBuffer();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 291, 103, 15, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 18, 13);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				VIDEO_FadeIn(4);
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 20;
				break;
			case 20:// chat
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(ui.txt_file[UI_TXT_SCN1I].line[6])) & engine.sequence) {
					aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[6][char_number];
					aux_string[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(215, 34, aux_string, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(ui.txt_file[UI_TXT_SCN1I].line[7])) & engine.sequence) {
					aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[7][char_number];
					aux_string[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(210, 46, aux_string, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 21;
				break;
			case 21:// Getting things
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO7L.PCX", 285 * 97, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 88, 106, 20, 80, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 23, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 22;
				break;
			case 22:// Fade chat
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 18, 13);
				VIDEO_StringToScreenBuffer(215, 34, ui.txt_file[UI_TXT_SCN1I].line[6], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(210, 46, ui.txt_file[UI_TXT_SCN1I].line[7], FONT_SLIM_BLACK);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 88, 106, 20, 80, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 23, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 23;
				break;
			case 23:// Show gun
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO9H.PCX", 82 * 100, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO8L.PCX", 82 * 100, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 88, 106, 118, 80, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 121, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 24;
				break;
			case 24:// Fade getting things
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 23, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 25;
				break;
			case 25:// Show running
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO10H.PCX", 194 * 180, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN1.DAT", "INTRO9L.PCX", 194 * 180, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 88, 106, 213, 80, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 216, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 26;
				break;
			case 26:// Fade gun
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 121, 83);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();

				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 27;
				break;
			case 27:// chat
				char_number = 0;
				dialog_delay_counter = 0;

				while ((char_number < strlen(ui.txt_file[UI_TXT_SCN1I].line[8])) & engine.sequence) {
					aux_string[char_number] = ui.txt_file[UI_TXT_SCN1I].line[8][char_number];
					aux_string[char_number + 1] = '\0';
					VIDEO_StringToScreenBuffer(220, 86, aux_string, FONT_SLIM_BLACK);
					dialog_delay_counter++;
					if (dialog_delay_counter > 4) {
						char_number++;
						dialog_delay_counter = 0;
					}

					VIDEO_VSync();
					VIDEO_ScreenBufferToVRAM();
				}
				step = 28;
				break;
			case 28:// wait
				SetDelayTime(5000);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				step = 30;
				break;
			case 30://End sequence
				engine.sequence = false;
				break;
		}
	}
	VIDEO_FadeOut(1);
	GFX_UnloadSprites();
	AUDIO_StopSong();
	AUDIO_UnloadSong();
	GFX_UnloadSprite(SPRITE_GRAPHICS_ID_FUCK);
	MM_PopChunks(CT_TEMPORARY_SPRITE);
}
static void Scene1_LoadAssets(void) {
	// Load assets
	ENEMY_Init();// Initialize enemy system

	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 2 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY2_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY2_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY2_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY2_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY2.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 3 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY3_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY3_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY3_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY3_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY3_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY3.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY3_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 4 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY4_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY4_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY4_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY4_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY4_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY4_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY4.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY4_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 5 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY5_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY5_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY5_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY5_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY5_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY5_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY5.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY5_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Npcs
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "SARAF.PCX", SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "SARA.PCX", SPRITE_GRAPHICS_ID_NPC_SARA, 25, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID1.PCX", SPRITE_GRAPHICS_ID_NPC_DROID1, 24, 31, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID1F.PCX", SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "BOSS.PCX", SPRITE_GRAPHICS_ID_NPC_BOSS, 23, 20, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "BOSSF.PCX", SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID2.PCX", SPRITE_GRAPHICS_ID_NPC_DROID2, 30, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID2F.PCX", SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID3.PCX", SPRITE_GRAPHICS_ID_NPC_DROID3, 43, 42, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "DROID3F.PCX", SPRITE_GRAPHICS_ID_NPC_DROID3_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(1 << 4, 22 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 0, 0, 0, 0);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1.PCX", SPRITE_GRAPHICS_ID_BARREL1, 16, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1P.PCX", SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "TARGET1.PCX", SPRITE_GRAPHICS_ID_TARGET1, 20, 32, 6, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "TARGET2.PCX", SPRITE_GRAPHICS_ID_TARGET2, 20, 32, 7, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene1_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);

	// Enemy 2
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_LARM);

	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_LARM);

	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY4_LARM);

	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY5_LARM);

	// NPCs
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_SARA);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_BOSS);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID2);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID3);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_DROID3_PORTAIT);

	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_BARREL1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_BARREL1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_TARGET1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_TARGET2);
}
static void Scene1_LoadRoom1(void) {
	ENEMY_Load(0, 6 << 4, 17 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 0, 64);

	ENEMY_Load(1, 10 << 4, 17 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 1, 64);

	ENEMY_Load(2, 29 << 4, 17 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 2, 64);

	ENEMY_Load(3, 21 << 4, 23 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_WALKING_RANDOM_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 3, 64);

	ENEMY_Load(4, 53 << 4, 22 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_WALKING_RANDOM_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 4, 64);

	ENEMY_Load(5, 70 << 4, 12 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_WALKING_RANDOM_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 5, 64);

	MAP_LoadMap("MAPSCN11.DAT", 80, 30, "TSCN11.DAT", "SCN1_1_BACK.PCX", "SCN1_1_FORE.PCX", "SCN1_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);

	GFX_LoadPalette("PALETTES.DAT", "SCN11.PCX", 256);
}
static void Scene1_LoadRoom2(void) {

	// Enemies on jail
	ENEMY_Load(2, 44 << 4, 27 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_WALKING_RANDOM_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 0, 64);
	ENEMY_Load(3, 44 << 4, 37 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_WALKING_RANDOM_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 0, 64);

	// Enemy on ring
	ENEMY_Load(4, 38 << 4, 10 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY1_FEET, SPRITE_GRAPHICS_ID_ENEMY1_BODY, SPRITE_GRAPHICS_ID_ENEMY1_HEAD, SPRITE_GRAPHICS_ID_ENEMY1_LARM, SPRITE_GRAPHICS_ID_ENEMY1_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_STATIC_NPC, 20);
	ENEMY_LoadAnimations("ENEMANI.DAT", 0, 64);

	// Sara
	NPC_Load(1, 39 << 4, 48 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_SARA, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[1].sprite_num, false, true, 60);

	// Desk droid
	NPC_Load(2, 37 << 4, 44 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_DROID1, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[2].sprite_num, false, true, 50);

	// Agent 1
	NPC_Load(3, 9 << 4, 25 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_AGENT1, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[3].sprite_num, false, true, 50);

	// Agent 2
	NPC_Load(4, 14 << 4, 25 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_AGENT2, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[4].sprite_num, false, true, 60);

	// Boss
	NPC_Load(5, (11 << 4) + 4, (26 << 4) + 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_BOSS, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[5].sprite_num, false, true, 60);

	// Inside Bathroom droid
	NPC_Load(6, 6 << 4, 56 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_DROID1, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[6].sprite_num, false, true, 50);

	// Jail droid
	NPC_Load(7, 28 << 4, 30 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_DROID3, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[7].sprite_num, false, true, 50);

	// Training droid
	NPC_Load(8, 30 << 4, 18 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_DROID2, NPC_FACING_DOWN, NPC_STATUS_IDLE, 100);
	GFX_SetDefaultAnimation(npc[8].sprite_num, false, true, 30);
	NPC_SetRange(8, 64);

	// Outside bathroom droid
	NPC_Load(9, 7 << 4, 47 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_DROID1, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[9].sprite_num, false, true, 50);

	// Target pistol
	OBJECT_LoadObject(1, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (4 << 4) + 8, 7 << 4);

	// Target shotgun
	OBJECT_LoadObject(2, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (9 << 4) + 8, 9 << 4);

	// Target uzi
	OBJECT_LoadObject(3, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (14 << 4) + 8, 7 << 4);

	// Target snipper
	OBJECT_LoadObject(4, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (19 << 4) + 8, 6 << 4);


	MAP_LoadMap("MAPSCN12.DAT", 50, 60, "TSCN12.DAT", "SCN1_2_BACK.PCX", "SCN1_2_FORE.PCX", "SCN1_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);

	GFX_LoadPalette("PALETTES.DAT", "SCN12.PCX", 256);
}
static void Scene1_SetHotspotsAndEvents(void) {

	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = true;
	hotspot_enabling_room1[4] = true;
	hotspot_enabling_room1[5] = true;
	hotspot_enabling_room1[6] = true;
	hotspot_enabling_room1[7] = true;
	hotspot_enabling_room1[8] = true;
	hotspot_enabling_room1[9] = true;
	hotspot_enabling_room1[10] = true;
	hotspot_enabling_room1[11] = true;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = true;
	event_enabling_room1[4] = true;
	event_enabling_room1[5] = true;
	event_enabling_room1[6] = true;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = false;
	event_enabling_room1[16] = false;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = true;
	hotspot_enabling_room2[2] = true;
	hotspot_enabling_room2[3] = true;
	hotspot_enabling_room2[4] = true;
	hotspot_enabling_room2[5] = true;
	hotspot_enabling_room2[6] = true;
	hotspot_enabling_room2[7] = true;
	hotspot_enabling_room2[8] = true;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = true;// Go back to room 1
	event_enabling_room2[2] = true;
	event_enabling_room2[3] = true;
	event_enabling_room2[4] = true;
	event_enabling_room2[5] = true;
	event_enabling_room2[6] = true;
	event_enabling_room2[7] = true;
	event_enabling_room2[8] = true;
	event_enabling_room2[9] = true;
	event_enabling_room2[10] = true;
	event_enabling_room2[11] = true;// Sara conversation 1
	event_enabling_room2[12] = true;
	event_enabling_room2[13] = true;// Desktop droid
	event_enabling_room2[14] = true;
	event_enabling_room2[15] = true;
	event_enabling_room2[16] = true;
	event_enabling_room2[17] = false;// Sara conversation 2
	event_enabling_room2[18] = true;
	event_enabling_room2[19] = true;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene1_Loop(void) {
	bool end_sequence;
	bool tutorial;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "ISCN1.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN1I].line[10], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN1I].line[11], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene1_LoadAssets();
	Scene1_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	tutorial = false;
	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();


	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. Outside doscity
			Scene1_LoadRoom1();

			ACTOR_SetPosition(1 << 4, 22 << 4, ACTOR_FACING_RIGHT);
			CAM_Init(map.width_px, map.height_px, 0 << 4, 18 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();
			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		case 2:// Room 2. Inside police office
			Scene1_LoadRoom2();

			ACTOR_SetPosition(46 << 4, 49 << 4, ACTOR_FACING_LEFT);
			CAM_Init(map.width_px, map.height_px, 30 << 4, 42 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();
			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene1_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {

		switch (engine.room) {
			case 1:// Outside
				if (!ui.freeze) {
					// Hotspots
					if (!actor.mode_combat) {
						// Check point on object or enemy
						switch (cursor.point_on & 0xFF) {
							case ENTITY_ID_HSPOT:// Hotspot
								switch ((cursor.point_on >> 8) & 0xFF) {
									case 0:// No hotspot, WTF??
										break;
									case 1:// Paint
										if (hotspot_enabling_room1[1]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 101, 102, 103, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 1);
											}
										}
										break;
									case 2:// Cult door
										if (hotspot_enabling_room1[2]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 104, 105, 106, false, 100);
												while (!UI_IsSpeechFinished()) {
													Update(false);
												}
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 107, 108, 109, false, 100);
												while (!UI_IsSpeechFinished()) {
													Update(false);
												}
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 110, 111, 112, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 2);
											}
										}
										break;
									case 3:// Container
										if (hotspot_enabling_room1[3]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 113, 114, 115, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 3);
											}
										}
										break;
									case 4:// Bed
										if (hotspot_enabling_room1[4]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 116, 117, 118, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 4);
											}
										}
										break;
									case 5:// Garbage
										if (hotspot_enabling_room1[5]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 119, 120, 121, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 5);
											}
										}
										break;
									case 6:// Police office
										if (hotspot_enabling_room1[6]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 122, 123, 124, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 6);
											}
										}
										break;
									case 7:// Accident
										if (hotspot_enabling_room1[7]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 125, 126, 127, false, 100);
												while (!UI_IsSpeechFinished()) {
													Update(false);
												}
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 128, 129, 130, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 7);
											}
										}
										break;
									case 8:// Paint
										if (hotspot_enabling_room1[8]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 131, 132, 133, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 8);
											}
										}
										break;
									case 9:// Paint
										if (hotspot_enabling_room1[9]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 134, 135, 136, false, 100);
												while (!UI_IsSpeechFinished()) {
													Update(false);
												}
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 137, 138, 139, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 9);
											}
										}
										break;
									case 10:// Paint
										if (hotspot_enabling_room1[10]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 140, 141, 142, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 10);
											}
										}
										break;
									case 11:// XXX
										if (hotspot_enabling_room1[11]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 143, 144, 145, false, 100);
											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 11);
											}
										}
										break;
									case 12:// Spare
										if (hotspot_enabling_room1[12]) {
										}
										break;
									case 13:// Spare
										if (hotspot_enabling_room1[13]) {
										}
										break;
									case 14:// Spare
										if (hotspot_enabling_room1[14]) {
										}
										break;
									case 15:// Spare
										if (hotspot_enabling_room1[15]) {
										}
										break;
									case 16:// Spare
										if (hotspot_enabling_room1[16]) {
										}
										break;
									default:
										break;
								}
							default:
								break;
						}
					}

					// Events
					actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
					switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
						case 1:// Event 1. Enter door. Go to room 2
							if (event_enabling_room1[1]) {
								VIDEO_FadeOut(4);

								ENEMY_UnloadEnemies();
								OBJECT_UnloadObjects();
								EFFECT_UnloadEffects();
								BULLET_UnloadBullets();
								PARTICLE_UnloadParticles();
								GRENADE_UnloadGrenades();
								MAP_UnloadMap();

								Scene1_LoadRoom2();

								ACTOR_SetPosition(46 << 4, 49 << 4, ACTOR_FACING_LEFT);
								CAM_Init(map.width_px, map.height_px, 30 << 4, 42 << 4);// Initialize camera
								MAP_DrawMapToMapVideoBuffer();

								Update(true);
								Update(true);

								engine.room = 2;

								VIDEO_FadeIn(1);
							}
							break;
						case 2:// Event 2. Thugs
							if (event_enabling_room1[2]) {
								// Clear event 2
								event_enabling_room1[2] = false;
								sequence_step = 0;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0:// Show enemy 6
											ENEMY_Load(6, 40 << 4, 5 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_RIGHT, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
											ENEMY_LoadAnimations("ENEMANI.DAT", 6, 64);
											SetDelayTime(500);
											sequence_step++;
											break;
										case 1:// Wait
											if (AwaitDelayTime()) sequence_step++;
											break;
										case 2:// Show enemy 7
											SetDelayTime(500);
											ENEMY_Load(7, 47 << 4, 5 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY3_FEET, SPRITE_GRAPHICS_ID_ENEMY3_BODY, SPRITE_GRAPHICS_ID_ENEMY3_HEAD, SPRITE_GRAPHICS_ID_ENEMY3_LARM, SPRITE_GRAPHICS_ID_ENEMY3_RARM, ENEMY_FACING_LEFT, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
											ENEMY_LoadAnimations("ENEMANI.DAT", 7, 64);
											sequence_step++;
											break;
										case 3:// Wait
											if (AwaitDelayTime()) sequence_step++;
											break;
										case 4:// Show enemy 8
											SetDelayTime(500);
											ENEMY_Load(8, 40 << 4, 4 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY4_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY4_FEET, SPRITE_GRAPHICS_ID_ENEMY4_BODY, SPRITE_GRAPHICS_ID_ENEMY4_HEAD, SPRITE_GRAPHICS_ID_ENEMY4_LARM, SPRITE_GRAPHICS_ID_ENEMY4_RARM, ENEMY_FACING_RIGHT, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
											ENEMY_LoadAnimations("ENEMANI.DAT", 8, 64);
											sequence_step++;
											break;
										case 5:// Wait
											if (AwaitDelayTime()) sequence_step++;
											break;
										case 6:// Show enemy 9
											SetDelayTime(500);
											ENEMY_Load(9, 47 << 4, 4 << 4, 32, 32, SPRITE_GRAPHICS_ID_ENEMY5_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY5_FEET, SPRITE_GRAPHICS_ID_ENEMY5_BODY, SPRITE_GRAPHICS_ID_ENEMY5_HEAD, SPRITE_GRAPHICS_ID_ENEMY5_LARM, SPRITE_GRAPHICS_ID_ENEMY5_RARM, ENEMY_FACING_LEFT, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_SLEEP, 20);
											ENEMY_LoadAnimations("ENEMANI.DAT", 9, 64);
											sequence_step++;
											break;
										case 7:// Wait
											if (AwaitDelayTime()) sequence_step++;
											break;
										case 8://
											SetDelayTime(1000);
											sequence_step++;
										case 9://
											if (AwaitDelayTime()) sequence_step++;
											break;
										case 10://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY3_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 37, 38, 39, true, 100);
											sequence_step++;
											break;
										case 11://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 12://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY2_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 40, 41, 42, true, 60);
											sequence_step++;
											break;
										case 13://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 14://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY3_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 43, 44, 45, true, 100);
											sequence_step++;
											break;
										case 15://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 16://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 46, 47, 48, false, 100);
											sequence_step++;
											break;
										case 17://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 18://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY3_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 49, 50, 51, true, 100);
											sequence_step++;
											break;
										case 19://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 20://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY4_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 52, 53, 54, true, 100);
											sequence_step++;
											break;
										case 21://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 22://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY5_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 55, 56, 57, true, 100);
											sequence_step++;
											break;
										case 23://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 24://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 58, 59, 60, false, 100);
											sequence_step++;
											break;
										case 25://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 26:
											ENEMY_SetBehavior(6, ENEMY_STATUS_ATTACK);
											ENEMY_SetBehavior(7, ENEMY_STATUS_ATTACK);
											ENEMY_SetBehavior(8, ENEMY_STATUS_ATTACK);
											ENEMY_SetBehavior(9, ENEMY_STATUS_ATTACK);
											ACTOR_SetCombatMode(true);
											end_sequence = true;
											break;
										default:
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 3:// Event 3. Blocked path
							if (event_enabling_room1[3]) {
								// Clear event 3
								event_enabling_room1[3] = false;
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 1, 2, 3, false, 100);
							}
							break;
						case 4:// Event 4. Cultists
							if (event_enabling_room1[4]) {
								// Clear event 4
								event_enabling_room1[4] = false;
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 4, 5, 6, true, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 7, 8, 9, false, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 10, 11, 12, true, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 13, 14, 15, true, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 16, 17, 18, false, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 19, 20, 21, false, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 22, 23, 24, false, 100);
							}
							break;
						case 5:// Event 5. XXX
							if (event_enabling_room1[5]) {
								// Clear event 5
								event_enabling_room1[5] = false;
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 25, 26, 27, true, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 28, 29, 30, false, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 31, 32, 33, false, 100);
								while (!UI_IsSpeechFinished()) {
									Update(false);
								}
								UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 34, 35, 36, true, 100);
							}
							break;
						case 6:// Event 6. Open door
							if (event_enabling_room1[6]) {
								event_enabling_room1[6] = false;

								MAP_HideSwapableTile(72 << 4, 1 << 4);
								MAP_HideSwapableTile(73 << 4, 1 << 4);
								MAP_HideSwapableTile(74 << 4, 1 << 4);
								MAP_HideSwapableTile(72 << 4, 2 << 4);
								MAP_HideSwapableTile(73 << 4, 2 << 4);
								MAP_HideSwapableTile(74 << 4, 2 << 4);
								MAP_HideSwapableTile(72 << 4, 3 << 4);
								MAP_HideSwapableTile(73 << 4, 3 << 4);
								MAP_HideSwapableTile(74 << 4, 3 << 4);
							}
							break;
						default:
							break;
					}

					// Exit combat mode if there are no more enemies
					if (!enemy[6].is_loaded && !enemy[7].is_loaded && !enemy[8].is_loaded && !enemy[9].is_loaded && ACTOR_IsInCombatMode()) {
						ACTOR_SetCombatMode(false);
						UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 61, 62, 63, false, 100);
					}
				}
				break;
			case 2:// Police office
				if (!ui.freeze) {

					// Hotspots
					if (!actor.mode_combat) {
						// Check point on object or enemy
						switch (cursor.point_on & 0xFF) {
							case ENTITY_ID_HSPOT:// Hotspot
								switch ((cursor.point_on >> 8) & 0xFF) {
									case 0:// No hotspot, WTF??
										break;
									case 1:// Camera
										if (hotspot_enabling_room2[1]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 146, 147, 148, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 13);
											}
										}
										break;
									case 2:// Computer
										if (hotspot_enabling_room2[2]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 149, 150, 151, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 14);
											}
										}
										break;
									case 3:// Label
										if (hotspot_enabling_room2[3]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 152, 153, 154, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 15);
											}
										}
										break;
									case 4:// Gun
										if (hotspot_enabling_room2[4]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 155, 156, 157, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 16);
											}
										}
										break;
									case 5:// Shootgun
										if (hotspot_enabling_room2[5]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 158, 159, 160, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 17);
											}
										}
										break;
									case 6:// UZI
										if (hotspot_enabling_room2[6]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 161, 162, 163, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 18);
											}
										}
										break;
									case 7:// Rifle
										if (hotspot_enabling_room2[7]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 164, 165, 166, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 19);
											}
										}
										break;
									case 8:// Ring
										if (hotspot_enabling_room2[8]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 167, 168, 169, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 20);
											}
										}
										break;
									case 9:// Spare
										if (hotspot_enabling_room2[9]) {
											if (cursor.left_click) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 170, 171, 172, false, 100);

											} else {
												UI_ShowDescription(UI_TXT_SCN1H, 21);
											}
										}
										break;
									case 10:// Spare
										if (hotspot_enabling_room2[10]) {
										}
										break;
									case 11:// Spare
										if (hotspot_enabling_room2[11]) {
										}
										break;
									case 12:// Spare
										if (hotspot_enabling_room2[12]) {
										}
										break;
									case 13:// Spare
										if (hotspot_enabling_room2[13]) {
										}
										break;
									case 14:// Spare
										if (hotspot_enabling_room2[14]) {
										}
										break;
									case 15:// Spare
										if (hotspot_enabling_room2[15]) {
										}
										break;
									case 16:// Spare
										if (hotspot_enabling_room2[16]) {
										}
										break;
									default:
										break;
								}
								break;
							case ENTITY_ID_NPC:// NPC
								switch ((cursor.point_on >> 8) & 0xFF) {
									case 0:// No one
										break;
									case 1:// Sara
										if (cursor.left_click) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 173, 174, 175, false, 100);

										} else {
											UI_ShowDescription(UI_TXT_SCN1H, 22);
										}
										break;
									case 2:// Office droid
										if (cursor.left_click) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 190, 191, 192, false, 100);

										} else {
											UI_ShowDescription(UI_TXT_SCN1H, 23);
										}
										break;
									case 3:// Agent
									case 4:// Agent
										if (cursor.left_click) {
											if (scene_step == 0) UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 178, 179, 355, false, 100);
											else
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 194, 195, 196, false, 100);

										} else {
											if (scene_step == 0) UI_ShowDescription(UI_TXT_SCN1H, 25);
											else
												UI_ShowDescription(UI_TXT_SCN1H, 26);
										}
										break;
									case 5:// Boss
										if (cursor.left_click) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 176, 177, 355, false, 100);

										} else {
											UI_ShowDescription(UI_TXT_SCN1H, 24);
										}
										break;
									case 6:// Droid
									case 7:// Droid
									case 8:// Droid
									case 9:// Droid
										if (cursor.left_click) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1H, 190, 191, 192, false, 100);

										} else {
											UI_ShowDescription(UI_TXT_SCN1H, 23);
										}
										break;
									default:
										break;
								}
								break;
							default:
								break;
						}
					}

					// Events
					actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
					switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
						case 1:// Event 1. Go from map 2 to map 1 or end of scene
							if (event_enabling_room2[1]) {
								VIDEO_FadeOut(4);
								ENEMY_UnloadEnemies();
								OBJECT_UnloadObjects();
								EFFECT_UnloadEffects();
								BULLET_UnloadBullets();
								PARTICLE_UnloadParticles();
								GRENADE_UnloadGrenades();
								MAP_UnloadMap();

								switch (scene_step) {
									case 0:// go to map 1
										Scene1_LoadRoom1();
										ACTOR_SetPosition(73 << 4, 5 << 4, ACTOR_FACING_DOWN);
										CAM_Init(map.width_px, map.height_px, 63 << 4, 0 << 4);// Initialize camera
										MAP_DrawMapToMapVideoBuffer();

										Update(true);
										Update(true);

										engine.room = 1;

										VIDEO_FadeIn(1);
										break;
									case 1:// end of scene
										VIDEO_FadeOut(4);
										engine.ingame = false;
										engine.scene = 2;
										engine.room = 1;
										break;
									default:

										break;
								}
							}
							break;
						case 2:// Event 18. Bathroom door
							if (event_enabling_room2[2]) {
								event_enabling_room2[2] = false;

								MAP_HideSwapableTile(15 << 4, 51 << 4);
								MAP_HideSwapableTile(16 << 4, 51 << 4);
								MAP_HideSwapableTile(17 << 4, 51 << 4);

								MAP_HideSwapableTile(15 << 4, 52 << 4);
								MAP_HideSwapableTile(16 << 4, 52 << 4);
								MAP_HideSwapableTile(17 << 4, 52 << 4);

								MAP_HideSwapableTile(15 << 4, 53 << 4);
								MAP_HideSwapableTile(16 << 4, 53 << 4);
								MAP_HideSwapableTile(17 << 4, 53 << 4);
							}
							break;
						case 3:// Event 3. Private area door
							if (event_enabling_room2[3]) {
								event_enabling_room2[3] = false;

								MAP_HideSwapableTile(21 << 4, 41 << 4);
								MAP_HideSwapableTile(22 << 4, 41 << 4);
								MAP_HideSwapableTile(23 << 4, 41 << 4);

								MAP_HideSwapableTile(21 << 4, 42 << 4);
								MAP_HideSwapableTile(22 << 4, 42 << 4);
								MAP_HideSwapableTile(23 << 4, 42 << 4);

								MAP_HideSwapableTile(21 << 4, 43 << 4);
								MAP_HideSwapableTile(22 << 4, 43 << 4);
								MAP_HideSwapableTile(23 << 4, 43 << 4);
							}
							break;
						case 4:// Event 4. Boss room door
							if (event_enabling_room2[4]) {
								event_enabling_room2[4] = false;

								MAP_HideSwapableTile(7 << 4, 35 << 4);
								MAP_HideSwapableTile(8 << 4, 35 << 4);
								MAP_HideSwapableTile(9 << 4, 35 << 4);

								MAP_HideSwapableTile(7 << 4, 36 << 4);
								MAP_HideSwapableTile(8 << 4, 36 << 4);
								MAP_HideSwapableTile(9 << 4, 36 << 4);

								MAP_HideSwapableTile(7 << 4, 37 << 4);
								MAP_HideSwapableTile(8 << 4, 37 << 4);
								MAP_HideSwapableTile(9 << 4, 37 << 4);
							}
							break;
						case 5:// Event 5. Practice room door
							if (event_enabling_room2[5]) {
								event_enabling_room2[5] = false;

								if (tutorial) {
									tutorial = false;
									ENEMY_SetBehavior(7, ENEMY_STATUS_SLEEP);
								}

								MAP_HideSwapableTile(25 << 4, 21 << 4);
								MAP_HideSwapableTile(26 << 4, 21 << 4);
								MAP_HideSwapableTile(27 << 4, 21 << 4);

								MAP_HideSwapableTile(25 << 4, 22 << 4);
								MAP_HideSwapableTile(26 << 4, 22 << 4);
								MAP_HideSwapableTile(27 << 4, 22 << 4);

								MAP_HideSwapableTile(25 << 4, 23 << 4);
								MAP_HideSwapableTile(26 << 4, 23 << 4);
								MAP_HideSwapableTile(27 << 4, 23 << 4);
							}
							break;
						case 6:// Event 6. Practice room pistol
							if (event_enabling_room2[6]) {
								event_enabling_room2[6] = false;

								if (!OBJECT_IsLoaded(1)) OBJECT_LoadObject(1, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (4 << 4) + 8, 7 << 4);

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 252, 253, 254, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 255, 256, 257, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 258, 259, 355, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 260, 261, 262, true, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 263, 264, 265, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 310, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}

								// Set pistol
								ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_PISTOL);
								ACTOR_SetCombatMode(true);
								ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
							}
							break;
						case 7:// Event 7. Practice room shotgun
							if (event_enabling_room2[7]) {
								event_enabling_room2[7] = false;

								if (!OBJECT_IsLoaded(2)) OBJECT_LoadObject(2, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (9 << 4) + 8, 9 << 4);

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 266, 267, 268, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 269, 270, 355, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 264, 271, 355, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 309, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}


								// Set shotgun
								ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_SHOTGUN);
								ACTOR_SetCombatMode(true);
								ACTOR_SetBulletStatus(99, 99, 0, 0, 0);
							}
							break;
						case 8:// Event 8. Practice room uzi
							if (event_enabling_room2[8]) {
								event_enabling_room2[8] = false;

								if (!OBJECT_IsLoaded(3)) OBJECT_LoadObject(3, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (14 << 4) + 8, 7 << 4);

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 272, 273, 274, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 275, 276, 277, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 264, 278, 355, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 310, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}

								// Set uzi
								ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_UZI);
								ACTOR_SetCombatMode(true);
								ACTOR_SetBulletStatus(99, 0, 999, 0, 0);
							}
							break;
						case 9:// Event 9. Practice room rifle
							if (event_enabling_room2[9]) {
								event_enabling_room2[9] = false;

								if (!OBJECT_IsLoaded(4)) OBJECT_LoadObject(4, ENTITY_ID_TARGET, SPRITE_GRAPHICS_ID_TARGET2, (19 << 4) + 8, 6 << 4);

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 279, 280, 281, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 282, 283, 284, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 285, 286, 287, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 264, 288, 355, true, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 309, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}

								// Set snipper rifle
								ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_SNIPPER);
								ACTOR_SetCombatMode(true);
								ACTOR_SetBulletStatus(99, 0, 0, 99, 0);
							}
							break;
						case 10:// Event 10. Practice room ring
							if (event_enabling_room2[10]) {
								event_enabling_room2[10] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 289, 290, 291, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 292, 293, 355, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 294, 295, 296, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 297, 298, 355, true, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 299, 300, 301, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 302, 303, 355, true, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 304, 305, 355, true, 100);
												sequence_step++;
											}
											break;
										case 7://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 306, 307, 308, true, 100);
												sequence_step++;
											}
											break;
										case 8://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 311, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 9://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}

								NPC_SetBehavior(8, NPC_STATUS_IDLE);

								// Set pistol
								ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
								ACTOR_SetCombatMode(true);
								ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
							}
							break;
						case 11:// Event 11. Sara conversation 1
							if (event_enabling_room2[11]) {
								event_enabling_room2[11] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 80, 355, 355, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 81, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 82, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 83, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 84, 85, 86, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 87, 88, 355, false, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 89, 90, 91, false, 100);
												sequence_step++;
											}
											break;
										case 7://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 92, 93, 355, true, 100);
												sequence_step++;
											}
											break;
										case 8://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 94, 95, 96, true, 100);
												sequence_step++;
											}
											break;
										case 9://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 97, 98, 355, true, 100);
												sequence_step++;
											}
											break;
										case 10://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 99, 100, 101, false, 100);
												sequence_step++;
											}
											break;
										case 11://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 102, 103, 355, true, 100);
												sequence_step++;
											}
											break;
										case 12://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 104, 105, 355, true, 100);
												sequence_step++;
											}
											break;
										case 13://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 106, 107, 108, true, 100);
												sequence_step++;
											}
											break;
										case 14://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 109, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 15://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 110, 111, 112, false, 100);
												sequence_step++;
											}
											break;
										case 16://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 113, 114, 115, false, 100);
												sequence_step++;
											}
											break;
										case 17://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 116, 117, 355, true, 100);
												sequence_step++;
											}
											break;
										case 18://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 118, 119, 120, true, 100);
												sequence_step++;
											}
											break;
										case 19://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 121, 122, 123, true, 100);
												sequence_step++;
											}
											break;
										case 20://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 124, 125, 126, false, 100);
												sequence_step++;
											}
											break;
										case 21://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 127, 128, 129, false, 100);
												sequence_step++;
											}
											break;
										case 22://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 130, 131, 355, true, 100);
												sequence_step++;
											}
											break;
										case 23://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 132, 133, 134, true, 100);
												sequence_step++;
											}
											break;
										case 24://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 135, 136, 137, false, 100);
												sequence_step++;
											}
											break;
										case 25://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
							}
							break;
						case 12:// Event 12. Boss room
							if (event_enabling_room2[12]) {
								event_enabling_room2[12] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 140, 141, 142, false, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 143, 144, 145, false, 100);
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 146, 147, 148, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 149, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 150, 151, 152, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 13:// Event 13. Droid conversation
							if (event_enabling_room2[13]) {
								event_enabling_room2[13] = false;
								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 65, 66, 67, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 68, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 69, 70, 71, false, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 72, 73, 74, true, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 75, 76, 355, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 77, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 14:// Event 14. Bathroom droid conversation
							if (event_enabling_room2[14]) {
								event_enabling_room2[14] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 312, 313, 314, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 15:// Event 15. Enemy in jail 1
							if (event_enabling_room2[15]) {
								event_enabling_room2[15] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 315, 316, 355, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 317, 318, 319, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 320, 321, 355, false, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 16:// Event 16. Enemy in jail 2
							if (event_enabling_room2[16]) {
								event_enabling_room2[16] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 322, 323, 324, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ENEMY1_CHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 325, 326, 327, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 328, 329, 330, false, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 17:// Event 17. Sara conversation 2
							if (event_enabling_room2[17]) {
								event_enabling_room2[17] = false;
								sequence_step = 0;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 215, 355, 355, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 216, 217, 218, false, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 219, 220, 221, true, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 222, 223, 355, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 224, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 225, 226, 227, true, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 228, 229, 230, false, 100);
												sequence_step++;
											}
											break;
										case 7://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 231, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 8://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_SARA_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 232, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 9://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 233, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 11://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;
							}
							break;
						case 18:// Event 18. Agent conversation
							if (event_enabling_room2[18]) {
								event_enabling_room2[18] = false;

								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 153, 154, 355, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 213, 214, 355, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 156, 157, 158, false, 100);
												sequence_step++;
											}
											break;
										case 3://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 159, 160, 161, true, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 162, 163, 355, false, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 164, 165, 355, true, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 166, 167, 355, true, 100);
												sequence_step++;
											}
											break;
										case 7://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 168, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 8://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 169, 170, 355, true, 100);
												sequence_step++;
											}
											break;
										case 9://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 171, 172, 173, true, 100);
												sequence_step++;
											}
											break;
										case 10://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 174, 175, 355, true, 100);
												sequence_step++;
											}
											break;
										case 11://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 176, 177, 355, false, 100);
												sequence_step++;
											}
											break;
										case 12://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 178, 179, 180, true, 100);
											}
											break;
										case 13://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 181, 182, 355, true, 100);
											}
											break;
										case 14://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 183, 184, 355, true, 100);
											}
											break;
										case 15://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 185, 186, 187, true, 100);
											}
											break;
										case 16://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 188, 189, 190, false, 100);
											}
											break;
										case 17://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 191, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 18://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 192, 355, 355, false, 100);
											}
											break;
										case 19://
											if (UI_IsSpeechFinished()) {
												sequence_step++;
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 193, 194, 195, true, 100);
											}
											break;
										case 20://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 196, 197, 198, false, 100);
												sequence_step++;
											}
											break;
										case 21://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 199, 200, 201, true, 100);
												sequence_step++;
											}
											break;
										case 22://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 202, 203, 204, true, 100);
												sequence_step++;
											}
											break;
										case 23://
											if (UI_IsSpeechFinished()) sequence_step++;
											break;
										case 24:// Fade out
											VIDEO_FadeOut(4);
											NPC_UnloadNpc(3);
											NPC_UnloadNpc(4);
											Update(false);
											Update(false);
											VIDEO_FadeIn(4);
											sequence_step++;
											break;
										case 25://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 205, 206, 355, false, 100);
											sequence_step++;
											break;
										case 26://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 207, 208, 209, true, 100);
												sequence_step++;
											}
											break;
										case 27://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_BOSS_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 210, 211, 212, true, 100);
												sequence_step++;
											}
											break;
										case 28://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}
								end_sequence = false;

								scene_step = 1;

								event_enabling_room2[17] = true;
							}
							break;
						case 19:// Event 19. Tutorial start
							if (event_enabling_room2[19]) {
								event_enabling_room2[19] = false;

								tutorial = true;
								sequence_step = 0;
								end_sequence = false;
								while (!end_sequence) {
									switch (sequence_step) {
										case 0://
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 235, 236, 237, true, 100);
											sequence_step++;
											break;
										case 1://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 238, 239, 240, true, 100);
												sequence_step++;
											}
											break;
										case 2://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 241, 242, 243, false, 100);
												sequence_step++;
											}
											break;
										case 4://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 244, 355, 355, true, 100);
												sequence_step++;
											}
											break;
										case 5://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 245, 246, 247, true, 100);
												sequence_step++;
											}
											break;
										case 6://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_DROID2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 248, 249, 250, true, 100);
												sequence_step++;
											}
											break;
										case 7://
											if (UI_IsSpeechFinished()) {
												UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN1D, 251, 355, 355, false, 100);
												sequence_step++;
											}
											break;
										case 8://
											if (UI_IsSpeechFinished()) {
												end_sequence = true;
											}
											break;
										default:
											end_sequence = true;
											break;
									}
									Update(false);
								}

								NPC_SetBehavior(8, NPC_STATUS_CHASE);
							}
							break;
						default:
							break;
					}
				}

				// Close bathroom door
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 2) {
					if (!event_enabling_room2[2]) {
						event_enabling_room2[2] = true;

						MAP_ShowSwapableTile(15 << 4, 51 << 4);
						MAP_ShowSwapableTile(16 << 4, 51 << 4);
						MAP_ShowSwapableTile(17 << 4, 51 << 4);

						MAP_ShowSwapableTile(15 << 4, 52 << 4);
						MAP_ShowSwapableTile(16 << 4, 52 << 4);
						MAP_ShowSwapableTile(17 << 4, 52 << 4);

						MAP_ShowSwapableTile(15 << 4, 53 << 4);
						MAP_ShowSwapableTile(16 << 4, 53 << 4);
						MAP_ShowSwapableTile(17 << 4, 53 << 4);
					}
				}

				// Close private area door
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 3) {
					if (!event_enabling_room2[3]) {
						event_enabling_room2[3] = true;

						MAP_ShowSwapableTile(21 << 4, 41 << 4);
						MAP_ShowSwapableTile(22 << 4, 41 << 4);
						MAP_ShowSwapableTile(23 << 4, 41 << 4);

						MAP_ShowSwapableTile(21 << 4, 42 << 4);
						MAP_ShowSwapableTile(22 << 4, 42 << 4);
						MAP_ShowSwapableTile(23 << 4, 42 << 4);

						MAP_ShowSwapableTile(21 << 4, 43 << 4);
						MAP_ShowSwapableTile(22 << 4, 43 << 4);
						MAP_ShowSwapableTile(23 << 4, 43 << 4);
					}
				}

				// Close boss room door
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 4) {
					if (!event_enabling_room2[4]) {
						event_enabling_room2[4] = true;

						MAP_ShowSwapableTile(7 << 4, 35 << 4);
						MAP_ShowSwapableTile(8 << 4, 35 << 4);
						MAP_ShowSwapableTile(9 << 4, 35 << 4);

						MAP_ShowSwapableTile(7 << 4, 36 << 4);
						MAP_ShowSwapableTile(8 << 4, 36 << 4);
						MAP_ShowSwapableTile(9 << 4, 36 << 4);

						MAP_ShowSwapableTile(7 << 4, 37 << 4);
						MAP_ShowSwapableTile(8 << 4, 37 << 4);
						MAP_ShowSwapableTile(9 << 4, 37 << 4);
					}
				}

				// Close practice room door
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 5) {
					if (!event_enabling_room2[5]) {
						event_enabling_room2[5] = true;

						MAP_ShowSwapableTile(25 << 4, 21 << 4);
						MAP_ShowSwapableTile(26 << 4, 21 << 4);
						MAP_ShowSwapableTile(27 << 4, 21 << 4);

						MAP_ShowSwapableTile(25 << 4, 22 << 4);
						MAP_ShowSwapableTile(26 << 4, 22 << 4);
						MAP_ShowSwapableTile(27 << 4, 22 << 4);

						MAP_ShowSwapableTile(25 << 4, 23 << 4);
						MAP_ShowSwapableTile(26 << 4, 23 << 4);
						MAP_ShowSwapableTile(27 << 4, 23 << 4);
					}
				}

				// Leave practice pistol area
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 6) {
					if (!event_enabling_room2[6]) {
						event_enabling_room2[6] = true;

						// Set bare hands
						ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
						ACTOR_SetCombatMode(false);
						ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
					}
				}

				// Leave practice shotgun area
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 7) {
					if (!event_enabling_room2[7]) {
						event_enabling_room2[7] = true;

						// Set bare hands
						ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
						ACTOR_SetCombatMode(false);
						ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
					}
				}

				// Leave practice uzi area
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 8) {
					if (!event_enabling_room2[8]) {
						event_enabling_room2[8] = true;

						// Set bare hands
						ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
						ACTOR_SetCombatMode(false);
						ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
					}
				}

				// Leave practice rifle area
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 9) {
					if (!event_enabling_room2[9]) {
						event_enabling_room2[9] = true;

						// Set bare hands
						ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
						ACTOR_SetCombatMode(false);
						ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
					}
				}

				// Leave practice fight area
				if (((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) != 10) {
					if (!event_enabling_room2[10]) {
						event_enabling_room2[10] = true;

						NPC_SetBehavior(8, NPC_STATUS_CHASE);

						// Set bare hands
						ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
						ACTOR_SetCombatMode(false);
						ACTOR_SetBulletStatus(99, 0, 0, 0, 0);
					}
				}

				break;
		}

		Update(!actor.action_dead);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	NPC_UnloadNpcs();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene1_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);// free level temporary mem
}
static void Scene1_Outro(void) {
	int step;
	int i, horizontal_scroll, max_scroll, src_index, dst_index;
	bool scroll_end;

	engine.sequence = true;
	step = 0;
	while (engine.sequence) {
		switch (step) {
			case 0:// Draw loading screen
				MOUSE_HideCursor();
				UI_ShowLoadingScreen();
				step++;
				break;
			case 1:// Load first outro scene
				AUDIO_LoadSong(AUDIO_SONG_3);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO1H.PCX", 266 * 165, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				step++;
				break;
			case 2:// Hide loading screen
				UI_HideLoadingScreen();
				step++;
				break;
			case 3:/// Scene 1 :: Agents waiting outside
				VIDEO_ClearScreenBuffer();
				GFX_LoadPalette("PALETTES.DAT", "ISCN1.PCX", 256);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 272, 171, 13, 11, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 16, 14);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(4);

				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 50);
				AUDIO_PlaySong(true);

				step++;
				break;
			case 4:// Just wait
				SetDelayTime(600);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 5://Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 141, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 143, 82);
				step++;
				break;
			case 6:// Speech
				VIDEO_StringToScreenBuffer(28, 148, ui.txt_file[UI_TXT_SCN1I].line[15], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 158, ui.txt_file[UI_TXT_SCN1I].line[16], FONT_SLIM_BLACK);
				step++;
				break;
			case 7:// Just wait
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO2H.PCX", 300 * 80, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO2L.PCX", 300 * 80, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO3H.PCX", 100 * 100, gfx.image_buffer3, &gfx.image_buffer3_width, &gfx.image_buffer3_height);
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				VIDEO_FadeOut(4);
				step++;
				break;
			case 8:// On the way
				VIDEO_ClearScreen();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 304, 84, 8, 8, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 10, 10);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(4);
				step++;
				break;
			case 9:// Just wait
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 10:// Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 60, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 62, 82);
				step++;
				break;
			case 11:// Speech
				VIDEO_StringToScreenBuffer(28, 64, ui.txt_file[UI_TXT_SCN1I].line[17], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 74, ui.txt_file[UI_TXT_SCN1I].line[18], FONT_SLIM_BLACK);
				step++;
				break;
			case 12:// Just wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 13:// Hide chat box
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 10, 10);
				SetDelayTime(500);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 14:// Show second image
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 104, 104, 23, 60, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, gfx.image_buffer3_width, gfx.image_buffer3_height, 25, 62);
				step++;
				break;
			case 15:// Just wait
				SetDelayTime(500);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 16:// Hide first image and show again second
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 10, 10);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 104, 104, 23, 60, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, gfx.image_buffer3_width, gfx.image_buffer3_height, 25, 62);
				step++;
				break;
			case 17:// Just wait
				SetDelayTime(500);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 18:// Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 168, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 170, 82);
				step++;
				break;
			case 19:// Speech
				VIDEO_StringToScreenBuffer(28, 172, ui.txt_file[UI_TXT_SCN1I].line[19], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 182, ui.txt_file[UI_TXT_SCN1I].line[20], FONT_SLIM_BLACK);
				step++;
				break;
			case 20:// Just wait
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 21:
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO3L.PCX", 100 * 100, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO4H.PCX", 100 * 100, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 22:// Show third image image
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 104, 104, 163, 60, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, gfx.image_buffer2_width, gfx.image_buffer2_height, 165, 62);
				SetDelayTime(500);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 23:// Hide second image
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, gfx.image_buffer1_width, gfx.image_buffer1_height, 25, 62);
				step++;
				break;
			case 24:// Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 168, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 170, 82);
				step++;
				break;
			case 25:// Speech
				VIDEO_StringToScreenBuffer(28, 172, ui.txt_file[UI_TXT_SCN1I].line[21], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 182, ui.txt_file[UI_TXT_SCN1I].line[22], FONT_SLIM_BLACK);
				step++;
				break;
			case 26:// Wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 27:// Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 168, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 170, 82);
				step++;
				break;
			case 28:// Speech
				VIDEO_StringToScreenBuffer(28, 172, ui.txt_file[UI_TXT_SCN1I].line[23], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 182, ui.txt_file[UI_TXT_SCN1I].line[24], FONT_SLIM_BLACK);
				step++;
				break;
			case 29:// Wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 30:// Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 168, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 170, 82);
				step++;
				break;
			case 31:// Speech
				VIDEO_StringToScreenBuffer(28, 172, ui.txt_file[UI_TXT_SCN1I].line[25], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 182, ui.txt_file[UI_TXT_SCN1I].line[26], FONT_SLIM_BLACK);
				step++;
				break;
			case 32:// Wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 33:
				VIDEO_FadeOut(4);
				step++;
				break;
			case 34:
				VIDEO_ClearScreen();
				step++;
				break;
			case 35:/// Draw square
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 85, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 87, 82);
				VIDEO_FadeIn(4);
				step++;
				break;
			case 36:
				VIDEO_StringToScreenBuffer(28, 95, ui.txt_file[UI_TXT_SCN1I].line[28], FONT_SLIM_BLACK);
				step++;
				break;
			case 37:// Wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 38:// Load hangar
				VIDEO_ClearScreen();
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO5H.PCX", 320 * 140, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO6H.PCX", 120 * 140, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO5L.PCX", 120 * 140, gfx.image_buffer3, &gfx.image_buffer3_width, &gfx.image_buffer3_height);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 124, 144, 10, 50, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 120, 140, 12, 52);
				horizontal_scroll = 0;
				max_scroll = 200;
				step++;
				break;
			case 39:// Chat
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 10, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 12, 82);
				step++;
				break;
			case 40:// Scroll hangar
				src_index = horizontal_scroll;
				dst_index = 52 * CAM_VISIBLE_WIDTH + 12;
				for (i = 0; i < gfx.image_buffer1_height; i++) {
					memcpy(&video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK][dst_index], &gfx.image_buffer1[src_index], 120);
					src_index += gfx.image_buffer1_width;
					dst_index += CAM_VISIBLE_WIDTH;
				}
				horizontal_scroll += 1;
				SetDelayTime(20);
				while (!AwaitDelayTime()) {
					// Just wait
				}

				if (horizontal_scroll == 40) {
					VIDEO_StringToScreenBuffer(28, 16, ui.txt_file[UI_TXT_SCN1I].line[30], FONT_SLIM_BLACK);
					VIDEO_StringToScreenBuffer(28, 26, ui.txt_file[UI_TXT_SCN1I].line[31], FONT_SLIM_BLACK);
				}

				if (horizontal_scroll >= max_scroll) scroll_end = true;
				if (scroll_end) step++;
				break;
			case 41:// Wait
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 42:// Show lavoratory
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 124, 144, 160, 50, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 120, 140, 162, 52);
				step++;
				break;
			case 43:// Chat
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 10, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 12, 82);
				step++;
				break;
			case 44:// Hide picture 1
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 120, 140, 12, 52);
				step++;
				break;
			case 45:// Chat
				VIDEO_StringToScreenBuffer(28, 16, ui.txt_file[UI_TXT_SCN1I].line[32], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 26, ui.txt_file[UI_TXT_SCN1I].line[33], FONT_SLIM_BLACK);
				step++;
				break;
			case 46://
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO7H.PCX", 270 * 170, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("OSCN1.DAT", "OUTRO8H.PCX", 270 * 170, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 47:// Stop music
				VIDEO_FadeOut(4);
				VIDEO_ClearScreen();
				AUDIO_StopSong();
				AUDIO_LoadSong(AUDIO_SONG_4);
				step++;
				break;
			case 48:// Draw last picture
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 264, 164, 16, 16, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 260, 160, 18, 18);
				step++;
				break;
			case 49:
				VIDEO_FadeIn(4);
				step++;
				break;
			case 50:
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 10);
				AUDIO_PlaySong(false);
				step++;
				break;
			case 51:
				i = (rand() % 2 * gfx.image_buffer1_width) + rand() % 2;
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[i], gfx.image_buffer1_width, gfx.image_buffer1_height, 260, 160, 18, 18);
				if (AUDIO_GetCurrentLine() == 200) step++;
				break;
			case 52:
				i = (rand() % 4 * gfx.image_buffer1_width) + rand() % 4;
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[i], gfx.image_buffer1_width, gfx.image_buffer1_height, 260, 160, 18, 18);
				if (AUDIO_GetCurrentLine() == 550) step++;
				break;
			case 53:
				i = (rand() % 2 * gfx.image_buffer1_width) + rand() % 2;
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[i], gfx.image_buffer1_width, gfx.image_buffer1_height, 260, 160, 18, 18);
				SetDelayTime(10);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (AUDIO_GetCurrentLine() == 750) step++;
				break;
			case 54:
				i = (rand() % 5 * gfx.image_buffer2_width) + rand() % 5;
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer2[i], gfx.image_buffer2_width, gfx.image_buffer2_height, 260, 160, 18, 18);
				if (AUDIO_GetCurrentLine() == 1100) step++;
				break;
			case 55://
				VIDEO_FadeOutToWhite(4);
				step++;
				break;
			case 56:
				if (AUDIO_IsSongEnded()) step++;
				break;
			case 57:
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 58:
				VIDEO_ClearScreen();
				VIDEO_FadeIn(4);
				step++;
				break;
			case 59://End sequence
				engine.sequence = false;
				break;
			default:
				engine.sequence = false;
				break;
		}
		EFFECT_UpdateEffects();
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}
	VIDEO_FadeOut(1);
	ENEMY_UnloadEnemies();
	PARTICLE_UnloadParticles();
	EFFECT_UnloadEffects();
	OBJECT_UnloadObjects();
	GFX_UnloadSprites();
	AUDIO_StopSong();
	AUDIO_UnloadSong();
}

/** SCENARIO 2 :: Mission 2 :: Down to the hell 
 */
static void Scene2_Intro(void) {
	int step;
	int i, scanline_scroll;
	int chat_spr_num;

	engine.sequence = true;
	step = 0;
	while (engine.sequence) {
		switch (step) {
			case 0:// Draw loading screen
				MOUSE_HideCursor();
				UI_ShowLoadingScreen();
				step++;
				break;
			case 1:// Load first outro scene
				GFX_PCXImageToBuffer("ISCN2.DAT", "INTRO1H.PCX", 90 * 180, gfx.image_buffer1, &gfx.image_buffer1_width, &gfx.image_buffer1_height);
				GFX_PCXImageToBuffer("ISCN2.DAT", "INTRO3H.PCX", 280 * 180, gfx.image_buffer2, &gfx.image_buffer2_width, &gfx.image_buffer2_height);
				GFX_PCXImageToBuffer("ISCN2.DAT", "INTRO2H.PCX", 90 * 180, gfx.image_buffer3, &gfx.image_buffer3_width, &gfx.image_buffer3_height);
				AUDIO_LoadSong(AUDIO_SONG_3);
				step++;
				break;
			case 2:// Hide loading screen
				UI_HideLoadingScreen();
				step++;
				break;
			case 3:/// Scene 1 :: Agents waiting outside
				VIDEO_ClearScreenBuffer();
				GFX_LoadPalette("PALETTES.DAT", "ISCN2.PCX", 256);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				VIDEO_FadeIn(4);

				TIMER_UpdateTimerTime(TIMER_AUDIO_NUMBER, 50);
				AUDIO_PlaySong(true);

				scanline_scroll = 0;
				step++;
				break;
			case 4:// Opening eyes 1
				scanline_scroll++;
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 320, scanline_scroll, 0, 100 - scanline_scroll, 222);
				SetDelayTime(100);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (scanline_scroll >= 10) {
					step++;
				}
				break;
			case 5:// Opening eyes 2
				scanline_scroll--;
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 320, scanline_scroll, 0, 100 - scanline_scroll, 222);
				SetDelayTime(100);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (scanline_scroll <= 0) {
					step++;
				}
				break;
			case 6:// Opening eyes 3
				scanline_scroll++;
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 320, scanline_scroll, 0, 100 - scanline_scroll, 222);
				SetDelayTime(100);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (scanline_scroll >= 20) {
					step++;
				}
				break;
			case 7:// Opening eyes 4
				scanline_scroll--;
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 320, scanline_scroll, 0, 100 - scanline_scroll, 222);
				SetDelayTime(100);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (scanline_scroll <= 0) {
					step++;
				}
				break;
			case 8:// Clear screen
				VIDEO_ClearScreenBuffer();
				SetDelayTime(100);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				scanline_scroll = 1;
				step++;
				break;
			case 9:// Show Jenny progressive
				i = ((gfx.image_buffer1_height / 2) - scanline_scroll) * gfx.image_buffer1_width;
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, scanline_scroll * 2, 10, (90 - scanline_scroll + 12), 222);
				VIDEO_BufferToScreenBuffer(&gfx.image_buffer1[i], gfx.image_buffer1_width, gfx.image_buffer1_height, 90, scanline_scroll * 2, 12, (90 - scanline_scroll + 12));
				scanline_scroll++;
				SetDelayTime(20);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				if (scanline_scroll >= 90) step++;
				break;
			case 10:
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 90, 180, 12, 12);
				step++;
				break;
			case 11:
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 12:
				chat_spr_num = GFX_FindEmptySpriteSlot();
				if (chat_spr_num == -1) {
					sprintf(engine.system_error_message1, "Scene 2 Intro function error");
					sprintf(engine.system_error_message2, "No empty sprite slot available");
					sprintf(engine.system_error_message3, " ");
					Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
				} else {
					GFX_InitSprite(ENTITY_ID_EMPTY, 0, chat_spr_num, 0, 65, 30);
				}
				GFX_SetSpriteGraphic(chat_spr_num, 0, SPRITE_GRAPHICS_ID_CHAT_BIG, 0, 0);
				GFX_SetDefaultAnimation(chat_spr_num, false, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 100, 25);
				step++;
				break;
			case 13://Sara's chat
				VIDEO_StringToScreenBuffer(106, 30, ui.txt_file[UI_TXT_SCN2I].line[1], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(106, 45, ui.txt_file[UI_TXT_SCN2I].line[2], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(106, 60, ui.txt_file[UI_TXT_SCN2I].line[3], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 14:// End chat
				GFX_HideSprite(chat_spr_num);
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 90, 180, 12, 12);
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 15:// Show actor
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 200, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 90, 180, 202, 12);
				step++;
				break;
			case 16:
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 17:// set chat
				GFX_SetDefaultAnimation(chat_spr_num, true, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 85, 45);
				GFX_ShowSprite(chat_spr_num);
				step++;
				break;
			case 18:
				VIDEO_StringToScreenBuffer(88, 50, ui.txt_file[UI_TXT_SCN2I].line[4], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(88, 65, ui.txt_file[UI_TXT_SCN2I].line[5], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(88, 80, ui.txt_file[UI_TXT_SCN2I].line[6], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 19:// End chat
				GFX_HideSprite(chat_spr_num);
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 90, 180, 12, 12);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 200, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 90, 180, 202, 12);
				SetDelayTime(200);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 20:// Show chat box
				GFX_SetDefaultAnimation(chat_spr_num, false, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 100, 25);
				GFX_ShowSprite(chat_spr_num);
				step++;
				break;
			case 21:
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 22:// chat
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[7], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[8], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 60, ui.txt_file[UI_TXT_SCN2I].line[9], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 23:
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 24:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[10], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[11], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 25:
				SetDelayTime(1000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 26:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[12], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[13], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 27:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[14], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[15], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 28:
				SetDelayTime(2000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 29:// Show bed
				GFX_HideSprite(chat_spr_num);
				VIDEO_ClearScreen();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 90, 180, 12, 12);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 200, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 90, 180, 202, 12);
				step++;
				break;
			case 30:// set chat
				GFX_SetDefaultAnimation(chat_spr_num, true, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 85, 45);
				GFX_ShowSprite(chat_spr_num);
				step++;
				break;
			case 31:
				VIDEO_StringToScreenBuffer(88, 50, ui.txt_file[UI_TXT_SCN2I].line[16], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(88, 65, ui.txt_file[UI_TXT_SCN2I].line[17], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(88, 80, ui.txt_file[UI_TXT_SCN2I].line[18], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(3000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 32:// Show bed
				GFX_HideSprite(chat_spr_num);
				VIDEO_ClearScreen();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer1, gfx.image_buffer1_width, gfx.image_buffer1_height, 90, 180, 12, 12);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 200, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 90, 180, 202, 12);
				step++;
				break;
			case 33:
				GFX_SetDefaultAnimation(chat_spr_num, false, false, 10);
				GFX_SetSpritePosition(chat_spr_num, 100, 25);
				GFX_ShowSprite(chat_spr_num);
				step++;
				break;
			case 34:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[19], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[20], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(3000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 35:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[21], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[22], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 60, ui.txt_file[UI_TXT_SCN2I].line[23], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(3000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 36:
				VIDEO_StringToScreenBuffer(104, 30, ui.txt_file[UI_TXT_SCN2I].line[24], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 45, ui.txt_file[UI_TXT_SCN2I].line[25], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(104, 60, ui.txt_file[UI_TXT_SCN2I].line[26], FONT_SLIM_BLACK);
				VIDEO_VSync();
				VIDEO_ScreenBufferToVRAM();
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 37:
				GFX_HideSprite(chat_spr_num);
				VIDEO_ClearScreen();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 94, 184, 200, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer3, gfx.image_buffer3_width, gfx.image_buffer3_height, 90, 180, 202, 12);
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				VIDEO_FadeOut(4);
				step++;
				break;
			case 38:
				VIDEO_ClearScreenBuffer();
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 284, 184, 10, 10, 222);
				VIDEO_BufferToScreenBuffer(gfx.image_buffer2, gfx.image_buffer2_width, gfx.image_buffer2_height, 280, 180, 12, 12);
				step++;
				break;
			case 39:
				VIDEO_FadeIn(4);
				step++;
				break;
			case 40://Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 141, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 143, 82);
				step++;
				break;
			case 41:
				VIDEO_StringToScreenBuffer(28, 148, ui.txt_file[UI_TXT_SCN2I].line[27], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 158, ui.txt_file[UI_TXT_SCN2I].line[28], FONT_SLIM_BLACK);
				step++;
				break;
			case 42:// wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 43://Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 141, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 143, 82);
				step++;
				break;
			case 44:
				VIDEO_StringToScreenBuffer(28, 148, ui.txt_file[UI_TXT_SCN2I].line[29], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 158, ui.txt_file[UI_TXT_SCN2I].line[30], FONT_SLIM_BLACK);
				step++;
				break;
			case 45:// wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 46://Show chat case
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 252, 30, 23, 141, 242);
				VIDEO_DrawSquareToScreenBuffer(video.screen_buffer[VIDEO_SCREEN_BUFFER_BACK], video.screen_width, video.screen_height, 248, 26, 25, 143, 82);
				step++;
				break;
			case 47:
				VIDEO_StringToScreenBuffer(28, 148, ui.txt_file[UI_TXT_SCN2I].line[31], FONT_SLIM_BLACK);
				VIDEO_StringToScreenBuffer(28, 158, ui.txt_file[UI_TXT_SCN2I].line[32], FONT_SLIM_BLACK);
				step++;
				break;
			case 48:// wait
				SetDelayTime(4000);
				while (!AwaitDelayTime()) {
					// Just wait
				}
				step++;
				break;
			case 49://End sequence
				engine.sequence = false;
				break;
			default:
				engine.sequence = false;
				break;
		}
		EFFECT_UpdateEffects();
		GFX_UpdateSprites();
		GFX_DrawSprites();
		VIDEO_VSync();
		VIDEO_ScreenBufferToVRAM();
	}
	VIDEO_FadeOut(1);
	ENEMY_UnloadEnemies();
	PARTICLE_UnloadParticles();
	EFFECT_UnloadEffects();
	OBJECT_UnloadObjects();
	GFX_UnloadSprites();
	AUDIO_StopSong();
	AUDIO_UnloadSong();
}
static void Scene2_LoadAssets(void) {

	ENEMY_Init();// Initialize enemy system

	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 2 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY2_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY2_FEET, 24, 32, 56, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY2_BODY, 36, 21, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY2_HEAD, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_LARM, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_RARM, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Boss assets
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY3_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY3_FEET, 80, 57, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY3_BODY, 71, 68, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY3_HEAD, 71, 68, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY3_LARM, 55, 69, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BOSS1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY3_RARM, 55, 69, 6, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Npc
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(1 << 4, 22 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 19, 199, 9, 9);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene2_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);

	// Enemy 2
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_LARM);

	// Enemy 3
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY3_LARM);

	// NPCs
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT);
}
static void Scene2_LoadRoom1(void) {
	// Agent 1
	NPC_Load(1, 26 << 4, 38 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_AGENT1, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[1].sprite_num, false, true, 50);

	// Agent 2
	NPC_Load(2, 29 << 4, 38 << 4, 32, 32, SPRITE_GRAPHICS_ID_NPC_AGENT2, NPC_FACING_DOWN, NPC_STATUS_IDLE, 20);
	GFX_SetDefaultAnimation(npc[2].sprite_num, false, true, 60);

	MAP_LoadMap("MAPSCN21.DAT", 80, 54, "TSCN21.DAT", "SCN2_1_BACK.PCX", "SCN2_1_FORE.PCX", "SCN2_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);

	GFX_LoadPalette("PALETTES.DAT", "SCN21.PCX", 256);
}
static void Scene2_LoadRoom2(void) {

	MAP_LoadMap("MAPSCN22.DAT", 40, 57, "TSCN22.DAT", "SCN2_2_BACK.PCX", "SCN2_2_FORE.PCX", "SCN2_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);

	GFX_LoadPalette("PALETTES.DAT", "SCN22.PCX", 256);

	ITEM_LoadItem(0, ENTITY_ID_ITEM_MEDIKIT, SPRITE_GRAPHICS_ID_ITEM_MEDIKIT, 11 << 4, 41 << 4);
	GFX_SetDefaultAnimation(item[0].num_sprite, false, true, 20);
}
static void Scene2_SetHotspotsAndEvents(void) {

	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = false;
	hotspot_enabling_room1[4] = false;
	hotspot_enabling_room1[5] = false;
	hotspot_enabling_room1[6] = false;
	hotspot_enabling_room1[7] = false;
	hotspot_enabling_room1[8] = false;
	hotspot_enabling_room1[9] = false;
	hotspot_enabling_room1[10] = false;
	hotspot_enabling_room1[11] = false;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = false;
	event_enabling_room1[4] = false;
	event_enabling_room1[5] = false;
	event_enabling_room1[6] = false;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = false;
	event_enabling_room1[16] = false;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = false;
	hotspot_enabling_room2[2] = false;
	hotspot_enabling_room2[3] = false;
	hotspot_enabling_room2[4] = false;
	hotspot_enabling_room2[5] = false;
	hotspot_enabling_room2[6] = false;
	hotspot_enabling_room2[7] = false;
	hotspot_enabling_room2[8] = false;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = true;
	event_enabling_room2[2] = true;
	event_enabling_room2[3] = true;
	event_enabling_room2[4] = true;
	event_enabling_room2[5] = true;
	event_enabling_room2[6] = false;
	event_enabling_room2[7] = false;
	event_enabling_room2[8] = false;
	event_enabling_room2[9] = false;
	event_enabling_room2[10] = false;
	event_enabling_room2[11] = false;
	event_enabling_room2[12] = false;
	event_enabling_room2[13] = false;
	event_enabling_room2[14] = false;
	event_enabling_room2[15] = false;
	event_enabling_room2[16] = true;
	event_enabling_room2[17] = false;
	event_enabling_room2[18] = false;
	event_enabling_room2[19] = false;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene2_Loop(void) {
	bool end_sequence;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "SCN21.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN2I].line[40], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN2I].line[41], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene2_LoadAssets();
	Scene2_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();

	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. Outside mansion
			Scene2_LoadRoom1();

			ACTOR_SetPosition(1 << 4, 44 << 4, ACTOR_FACING_RIGHT);
			CAM_Init(map.width_px, map.height_px, 0 << 4, 40 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();
			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		case 2:// Room 2. Sewer
			Scene2_LoadRoom2();

			ACTOR_SetPosition(37 << 4, 3 << 4, ACTOR_FACING_LEFT); // Set actor position
			CAM_Init(map.width_px, map.height_px, 20 << 4, 0 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene2_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {

		switch (engine.room) {
			case 1:// Room 1. Outside
				// Hotspots
				// Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Sewer
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 101, 102, 103, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 1);
									}
									break;
								case 2:// Open sewer
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 104, 105, 106, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 1);
									}
									break;
								case 3:// Basement
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 107, 108, 109, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 2);
									}
									break;
								case 4:// Window
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 110, 111, 112, false, 100);
									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 3);
									}
									break;
								case 5:// Door
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 113, 114, 115, false, 100);
									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 4);
									}
									break;
								case 6:// Cult symbol
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2H, 116, 117, 118, false, 100);
									} else {
										UI_ShowDescription(UI_TXT_SCN2H, 5);
									}
									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Main door
						if (event_enabling_room1[1]) {
							event_enabling_room1[1] = false;
							event_enabling_room1[3] = true;

							sequence_step = 0;
							end_sequence = false;
							while (!end_sequence) {
								switch (sequence_step) {
									case 0://
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 1, 355, 355, true, 100);
										sequence_step++;
										break;
									case 1://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 2, 3, 355, false, 100);
											sequence_step++;
										}
										break;
									case 2://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 4, 355, 355, true, 100);
											sequence_step++;
										}
										break;
									case 3://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 5, 355, 355, false, 100);
											sequence_step++;
										}
										break;
									case 4://
										if (UI_IsSpeechFinished()) {
											SetDelayTime(1000);
											while (!AwaitDelayTime()) {
												// Just wait
												Update(false);
											}
											sequence_step++;
										}
										break;
									case 5://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 6, 7, 8, true, 100);
											sequence_step++;
										}
										break;
									case 6://
										if (UI_IsSpeechFinished()) {
											end_sequence = true;
										}
										break;
									default:
										end_sequence = true;
										break;
								}
								Update(false);
							}
							end_sequence = false;
						}
						break;
					case 2:// Event 2. Enter sewer
						if (event_enabling_room1[2]) {
							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							NPC_UnloadNpcs();
							MAP_UnloadMap();

							Scene2_LoadRoom2();
							ACTOR_SetPosition(37 << 4, 3 << 4, ACTOR_FACING_LEFT); // Set actor position
							CAM_Init(map.width_px, map.height_px, 20 << 4, 0 << 4);// Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					case 3:// Event 3. Continue searching
						if (event_enabling_room1[3]) {
							event_enabling_room1[3] = false;

							sequence_step = 0;
							end_sequence = false;
							while (!end_sequence) {
								switch (sequence_step) {
									case 0://
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 9, 10, 11, false, 100);
										sequence_step++;
										break;
									case 1://
										if (UI_IsSpeechFinished()) {
											end_sequence = true;
										}
										break;
									default:
										end_sequence = true;
										break;
								}
								Update(false);
							}
							end_sequence = false;
						}
					default:
						break;
				}

				break;
			case 2:// Room 2. Sewer
				   // Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare

									break;
								case 2:// Spare

									break;
								case 3:// Spare

									break;
								case 4:// Spare

									break;
								case 5:// Spare

									break;
								case 6:// Spare

									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Back to outside
						if (event_enabling_room2[1]) {
							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							MAP_UnloadMap();

							Scene2_LoadRoom1();
							ACTOR_SetPosition(75 << 4, 3 << 4, ACTOR_FACING_RIGHT);// Set actor position
							CAM_Init(map.width_px, map.height_px, 0 << 4, 0 << 4); // Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					case 2:// Event 2. Getting in
						if (event_enabling_room2[2]) {
							event_enabling_room2[2] = false;

							sequence_step = 0;
							end_sequence = false;
							while (!end_sequence) {
								switch (sequence_step) {
									case 0://
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 30, 31, 355, false, 100);
										sequence_step++;
										break;
									case 1://
										if (UI_IsSpeechFinished()) {
											ACTOR_SetCombatMode(true);
											ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_PISTOL);
											end_sequence = true;
										}
										break;
									default:
										end_sequence = true;
										break;
								}
								Update(false);
							}
							end_sequence = false;
						}
						break;
					case 3:// Event 3. Starts the rat party!
						if (event_enabling_room2[3]) {
							event_enabling_room2[3] = false;
							scene_step = 1;
						}
						break;
					case 4:// Event 4. BIG Surprise
						if (event_enabling_room2[4]) {
							event_enabling_room2[4] = false;
							BOSS_Load(10 << 4, 27 << 4, 98, 85, SPRITE_GRAPHICS_ID_ENEMY3_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY3_FEET, SPRITE_GRAPHICS_ID_ENEMY3_BODY, SPRITE_GRAPHICS_ID_ENEMY3_HEAD, SPRITE_GRAPHICS_ID_ENEMY3_LARM, SPRITE_GRAPHICS_ID_ENEMY3_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_BULLET3, BOSS_STATUS_CHASE, 200);
							BOSS_LoadAnimations("BOSSANI.DAT", 10);
						}
						break;
					case 5:// Event 5. Surprise
						if (event_enabling_room2[5]) {
						}
						break;
					case 6:// Event 6. Water
						if (event_enabling_room2[6]) {
							event_enabling_room2[6] = false;
						}
						break;
					case 16:// Event 16. Enter building
						if (event_enabling_room2[16]) {
							event_enabling_room2[16] = false;

							VIDEO_FadeOut(4);
							engine.ingame = false;
							engine.scene = 3;
							engine.room = 1;
						}
						break;
					default:
						break;
				}

				// Scene step 1. Rats everywhere!!
				if (scene_step == 1) {
					if (!enemy[0].is_loaded) {
						ENEMY_Load(0, 17 << 4, 2 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 0, 64);
					} else if (!enemy[1].is_loaded) {
						ENEMY_Load(1, 18 << 4, 2 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 1, 64);
					} else if (!enemy[2].is_loaded) {
						ENEMY_Load(2, 27 << 4, 8 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 2, 64);
					} else if (!enemy[3].is_loaded) {
						ENEMY_Load(3, 30 << 4, 15 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 3, 64);
					} else if (!enemy[4].is_loaded) {
						ENEMY_Load(4, 33 << 4, 21 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 4, 64);
					} else if (!enemy[5].is_loaded) {
						ENEMY_Load(5, 32 << 4, 33 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 5, 64);
					} else if (!enemy[6].is_loaded) {
						ENEMY_Load(6, 33 << 4, 33 << 4, 22, 32, SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, SPRITE_GRAPHICS_ID_ENEMY2_FEET, SPRITE_GRAPHICS_ID_ENEMY2_BODY, SPRITE_GRAPHICS_ID_ENEMY2_HEAD, SPRITE_GRAPHICS_ID_ENEMY2_LARM, SPRITE_GRAPHICS_ID_ENEMY2_RARM, ENEMY_FACING_DOWN, ENEMY_GUN_BARE_HANDS, SPRITE_GRAPHICS_ID_PUNCH, SPRITE_GRAPHICS_ID_PUNCH, ENEMY_STATUS_CHASE, 1);
						ENEMY_LoadAnimations("RATANI.DAT", 6, 64);
					}
				}

				if (scene_step == 2) {
					MAP_HideSwapableTile(15 << 4, 51 << 4);
					MAP_HideSwapableTile(16 << 4, 51 << 4);
					MAP_HideSwapableTile(17 << 4, 51 << 4);
				}
				break;
		}

		Update(true);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene2_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);// free level temporary mem
}
static void Scene2_Outro(void) {
}

/** SCENARIO 3 :: Mission 3 :: Behind enemy lines
 */
static void Scene3_Intro(void) {
}
static void Scene3_LoadAssets(void) {
	ENEMY_Init();// Initialize enemy system

	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Enemy 2 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY2_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY2_FEET, 24, 32, 56, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY2_BODY, 36, 21, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY2_HEAD, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_LARM, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY6.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY2_RARM, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	// Npc
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT1F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2, 21, 30, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("NPCS.DAT", "AGENT2F.PCX", SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(1 << 4, 22 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 19, 199, 9, 9);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene3_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);

	// Enemy 2
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY2_LARM);

	// NPCs
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_NPC_AGENT2_PORTAIT);
}
static void Scene3_LoadRoom1(void) {
	MAP_LoadMap("MAPSCN31.DAT", 70, 50, "TSCN31.DAT", "SCN3_1_BACK.PCX", "SCN3_1_FORE.PCX", "SCN3_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN31.PCX", 256);
}
static void Scene3_LoadRoom2(void) {
	MAP_LoadMap("MAPSCN32.DAT", 57, 48, "TSCN32.DAT", "SCN3_2_BACK.PCX", "SCN3_2_FORE.PCX", "SCN3_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN32.PCX", 256);
}
static void Scene3_SetHotspotsAndEvents(void) {

	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = false;
	hotspot_enabling_room1[4] = false;
	hotspot_enabling_room1[5] = false;
	hotspot_enabling_room1[6] = false;
	hotspot_enabling_room1[7] = false;
	hotspot_enabling_room1[8] = false;
	hotspot_enabling_room1[9] = false;
	hotspot_enabling_room1[10] = false;
	hotspot_enabling_room1[11] = false;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = false;
	event_enabling_room1[4] = false;
	event_enabling_room1[5] = false;
	event_enabling_room1[6] = false;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = true;
	event_enabling_room1[16] = true;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = false;
	hotspot_enabling_room2[2] = false;
	hotspot_enabling_room2[3] = false;
	hotspot_enabling_room2[4] = false;
	hotspot_enabling_room2[5] = false;
	hotspot_enabling_room2[6] = false;
	hotspot_enabling_room2[7] = false;
	hotspot_enabling_room2[8] = false;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = true;
	event_enabling_room2[2] = true;
	event_enabling_room2[3] = true;
	event_enabling_room2[4] = true;
	event_enabling_room2[5] = true;
	event_enabling_room2[6] = false;
	event_enabling_room2[7] = false;
	event_enabling_room2[8] = false;
	event_enabling_room2[9] = false;
	event_enabling_room2[10] = false;
	event_enabling_room2[11] = false;
	event_enabling_room2[12] = false;
	event_enabling_room2[13] = false;
	event_enabling_room2[14] = false;
	event_enabling_room2[15] = false;
	event_enabling_room2[16] = true;
	event_enabling_room2[17] = false;
	event_enabling_room2[18] = false;
	event_enabling_room2[19] = false;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene3_Loop(void) {
	bool end_sequence;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "SCN31.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN3I].line[40], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN3I].line[41], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene3_LoadAssets();
	Scene3_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();

	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. Lower floor
			Scene3_LoadRoom1();

			ACTOR_SetPosition(66 << 4, 7 << 4, ACTOR_FACING_LEFT);// Set actor position
			ACTOR_SetCombatMode(true);

			CAM_Init(map.width_px, map.height_px, 46 << 4, 0 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;

			break;
		case 2:// Room 2. Upper floor
			Scene3_LoadRoom2();

			ACTOR_SetPosition(27 << 4, 34 << 4, ACTOR_FACING_UP);   // Set actor position
			CAM_Init(map.width_px, map.height_px, 17 << 4, 24 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene3_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {
		switch (engine.room) {
			case 1:// Room 1. Lower floor
				// Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Garbage bin
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN3H, 120, 121, 122, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN3H, 6);
									}
									break;
								case 2:// Garbage
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN3H, 120, 121, 122, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN3H, 6);
									}
									break;
								case 3:// Sewer
									if (cursor.left_click) {
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN3H, 123, 124, 125, false, 100);

									} else {
										UI_ShowDescription(UI_TXT_SCN3H, 1);
									}
									break;
								case 4:// Spare
									break;
								case 5:// Spare
									break;
								case 6:// Spare
									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Kitchen entry
						if (event_enabling_room1[1]) {
							event_enabling_room1[1] = false;

							sequence_step = 0;
							end_sequence = false;
							while (!end_sequence) {
								switch (sequence_step) {
									case 0://
										UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 40, 41, 355, false, 100);
										sequence_step++;
										break;
									case 1://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 42, 43, 44, false, 100);
											sequence_step++;
										}
										break;
									case 2://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 45, 46, 47, false, 100);
											sequence_step++;
										}
										break;
									case 3://
										if (UI_IsSpeechFinished()) {
											UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN2D, 48, 355, 355, false, 100);
											ACTOR_SetCombatMode(false);
											end_sequence = true;
										}
										break;
									default:
										end_sequence = true;
										break;
								}
								Update(false);
							}
							end_sequence = false;
						}
						break;
					case 2:// Event 2. Kitchen entry
						if (event_enabling_room1[2]) {
							event_enabling_room1[2] = false;
						}
						break;
					case 15:// Event 15. End of level
						if (event_enabling_room1[15]) {
							event_enabling_room1[15] = false;

							VIDEO_FadeOut(4);
							engine.ingame = false;
							engine.scene = 4;
							engine.room = 1;
						}
						break;
					case 16:// Event 16. Go to second floor
						if (event_enabling_room1[16]) {
							event_enabling_room1[16] = false;
							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							NPC_UnloadNpcs();
							MAP_UnloadMap();

							Scene3_LoadRoom2();
							ACTOR_SetPosition(27 << 4, 22 << 4, ACTOR_FACING_UP);   // Set actor position
							CAM_Init(map.width_px, map.height_px, 17 << 4, 12 << 4);// Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					default:
						break;
				}
				break;

			case 2:// Room 2. Upper floor
				   // Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare

									break;
								case 2:// Spare

									break;
								case 3:// Spare

									break;
								case 4:// Spare

									break;
								case 5:// Spare

									break;
								case 6:// Spare

									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1.
						if (event_enabling_room2[1]) {
						}
						break;
					case 2:// Event 2.
						if (event_enabling_room2[2]) {
							event_enabling_room2[2] = false;
						}
						break;
					case 3:// Event 3.
						if (event_enabling_room2[3]) {
							event_enabling_room2[3] = false;
						}
						break;
					case 4:// Event 4.
						if (event_enabling_room2[4]) {
							event_enabling_room2[4] = false;
						}
						break;
					case 5:// Event 5.
						if (event_enabling_room2[5]) {
						}
						break;
					case 6:// Event 6.
						if (event_enabling_room2[6]) {
							event_enabling_room2[6] = false;
						}
						break;
					case 16:// Event 16. Go to first floor
						if (event_enabling_room2[16]) {
							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							MAP_UnloadMap();

							Scene3_LoadRoom1();
							ACTOR_SetPosition(27 << 4, 29 << 4, ACTOR_FACING_LEFT);// Set actor position

							CAM_Init(map.width_px, map.height_px, 17 << 4, 19 << 4);// Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 1;

							VIDEO_FadeIn(1);
						}
						break;
					default:
						break;
				}

				break;
		}

		Update(true);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene3_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);
}
static void Scene3_Outro(void) {
}

/** SCENARIO 4 :: Mission 4 :: Praise for mercy
 */
static void Scene4_Intro(void) {
}
static void Scene4_LoadAssets(void) {
	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(9 << 4, 47 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 19, 199, 9, 9);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene4_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);
}
static void Scene4_LoadRoom1(void) {
	MAP_LoadMap("MAPSCN41.DAT", 50, 50, "TSCN41.DAT", "SCN4_1_BACK.PCX", "SCN4_1_FORE.PCX", "SCN4_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN41.PCX", 256);
}
static void Scene4_LoadRoom2(void) {
	MAP_LoadMap("MAPSCN42.DAT", 47, 32, "TSCN42.DAT", "SCN4_2_BACK.PCX", "SCN4_2_FORE.PCX", "SCN4_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN42.PCX", 256);
}
static void Scene4_SetHotspotsAndEvents(void) {
	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = false;
	hotspot_enabling_room1[4] = false;
	hotspot_enabling_room1[5] = false;
	hotspot_enabling_room1[6] = false;
	hotspot_enabling_room1[7] = false;
	hotspot_enabling_room1[8] = false;
	hotspot_enabling_room1[9] = false;
	hotspot_enabling_room1[10] = false;
	hotspot_enabling_room1[11] = false;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = false;
	event_enabling_room1[4] = false;
	event_enabling_room1[5] = false;
	event_enabling_room1[6] = false;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = false;
	event_enabling_room1[16] = true;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = false;
	hotspot_enabling_room2[2] = false;
	hotspot_enabling_room2[3] = false;
	hotspot_enabling_room2[4] = false;
	hotspot_enabling_room2[5] = false;
	hotspot_enabling_room2[6] = false;
	hotspot_enabling_room2[7] = false;
	hotspot_enabling_room2[8] = false;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = true;
	event_enabling_room2[2] = true;
	event_enabling_room2[3] = true;
	event_enabling_room2[4] = true;
	event_enabling_room2[5] = true;
	event_enabling_room2[6] = false;
	event_enabling_room2[7] = false;
	event_enabling_room2[8] = false;
	event_enabling_room2[9] = false;
	event_enabling_room2[10] = false;
	event_enabling_room2[11] = false;
	event_enabling_room2[12] = false;
	event_enabling_room2[13] = false;
	event_enabling_room2[14] = false;
	event_enabling_room2[15] = false;
	event_enabling_room2[16] = true;
	event_enabling_room2[17] = false;
	event_enabling_room2[18] = false;
	event_enabling_room2[19] = false;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene4_Loop(void) {
	bool end_sequence;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "SCN41.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN4I].line[40], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN4I].line[41], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene4_LoadAssets();
	Scene4_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();

	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. Garden
			Scene4_LoadRoom1();

			ACTOR_SetPosition(9 << 4, 47 << 4, ACTOR_FACING_UP);// Set actor position
			ACTOR_SetCombatMode(true);

			CAM_Init(map.width_px, map.height_px, 0 << 4, 37 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;

			break;
		case 2:// Room 2. Church
			Scene4_LoadRoom2();

			ACTOR_SetPosition(23 << 4, 29 << 4, ACTOR_FACING_UP);   // Set actor position
			CAM_Init(map.width_px, map.height_px, 13 << 4, 20 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene4_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {
		switch (engine.room) {
			case 1:// Room 1. Garden
				// Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare
									if (cursor.left_click) {
										//UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN4H, 120, 121, 122, false, 100);

									} else {
										//UI_ShowDescription(UI_TXT_SCN4H, 6);
									}
									break;
								case 2:// Spare
									break;
								case 3:// Spare
									break;
								case 4:// Spare
									break;
								case 5:// Spare
									break;
								case 6:// Spare
									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Spare
						if (event_enabling_room1[1]) {
							event_enabling_room1[1] = false;
						}
						break;
					case 2:// Event 2. Spare
						if (event_enabling_room1[2]) {
							event_enabling_room1[2] = false;
						}
						break;
					case 15:// Event 15. Spare
						if (event_enabling_room1[15]) {
							event_enabling_room1[15] = false;
						}
						break;
					case 16:// Event 16. Go inside church
						if (event_enabling_room1[16]) {
							event_enabling_room1[16] = false;

							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							NPC_UnloadNpcs();
							MAP_UnloadMap();

							Scene4_LoadRoom2();
							ACTOR_SetPosition(23 << 4, 29 << 4, ACTOR_FACING_UP);   // Set actor position
							CAM_Init(map.width_px, map.height_px, 13 << 4, 20 << 4);// Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					default:
						break;
				}

				break;
			case 2:// Room 2. Inside church
				   // Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare

									break;
								case 2:// Spare

									break;
								case 3:// Spare

									break;
								case 4:// Spare

									break;
								case 5:// Spare

									break;
								case 6:// Spare

									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1.
						if (event_enabling_room2[1]) {
							event_enabling_room2[1] = false;
						}
						break;
					case 2:// Event 2.
						if (event_enabling_room2[2]) {
							event_enabling_room2[2] = false;
						}
						break;
					case 3:// Event 3.
						if (event_enabling_room2[3]) {
							event_enabling_room2[3] = false;
						}
						break;
					case 4:// Event 4.
						if (event_enabling_room2[4]) {
							event_enabling_room2[4] = false;
						}
						break;
					case 5:// Event 5.
						if (event_enabling_room2[5]) {
						}
						break;
					case 6:// Event 6.
						if (event_enabling_room2[6]) {
							event_enabling_room2[6] = false;
						}
						break;
					case 16:// Event 16. End of level
						if (event_enabling_room2[16]) {
							event_enabling_room2[16] = false;

							VIDEO_FadeOut(4);
							engine.ingame = false;
							engine.scene = 5;
							engine.room = 1;
						}
						break;
					default:
						break;
				}

				break;
		}

		Update(true);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene4_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);
}
static void Scene4_Outro(void) {
}

/** SCENARIO 5 :: Mission 5 :: Hit and run
 */
static void Scene5_Intro(void) {
}
static void Scene5_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);
}
static void Scene5_LoadAssets(void) {
	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(9 << 4, 47 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 19, 199, 9, 9);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene5_LoadRoom1(void) {
	MAP_LoadMap("MAPSCN51.DAT", 50, 21, "TSCN51.DAT", "SCN5_1_BACK.PCX", "SCN5_1_FORE.PCX", "SCN5_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN51.PCX", 256);
}
static void Scene5_LoadRoom2(void) {
	MAP_LoadMap("MAPSCN52.DAT", 50, 29, "TSCN52.DAT", "SCN5_2_BACK.PCX", "SCN5_2_FORE.PCX", "SCN5_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN52.PCX", 256);
}
static void Scene5_Update_Floor1(void) {
}
static void Scene5_Update_Floor2(void) {
}
static void Scene5_Update_Floor3(void) {
}
static void Scene5_Update_Floor4(void) {
}
static void Scene5_Update_Floor5(void) {
}
static void Scene5_SetHotspotsAndEvents(void) {
	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = false;
	hotspot_enabling_room1[4] = false;
	hotspot_enabling_room1[5] = false;
	hotspot_enabling_room1[6] = false;
	hotspot_enabling_room1[7] = false;
	hotspot_enabling_room1[8] = false;
	hotspot_enabling_room1[9] = false;
	hotspot_enabling_room1[10] = false;
	hotspot_enabling_room1[11] = false;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = false;
	event_enabling_room1[4] = false;
	event_enabling_room1[5] = false;
	event_enabling_room1[6] = false;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = false;
	event_enabling_room1[16] = true;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = false;
	hotspot_enabling_room2[2] = false;
	hotspot_enabling_room2[3] = false;
	hotspot_enabling_room2[4] = false;
	hotspot_enabling_room2[5] = false;
	hotspot_enabling_room2[6] = false;
	hotspot_enabling_room2[7] = false;
	hotspot_enabling_room2[8] = false;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = false;
	event_enabling_room2[2] = false;
	event_enabling_room2[3] = false;
	event_enabling_room2[4] = false;
	event_enabling_room2[5] = false;
	event_enabling_room2[6] = false;
	event_enabling_room2[7] = false;
	event_enabling_room2[8] = false;
	event_enabling_room2[9] = false;
	event_enabling_room2[10] = false;
	event_enabling_room2[11] = false;
	event_enabling_room2[12] = false;
	event_enabling_room2[13] = false;
	event_enabling_room2[14] = false;
	event_enabling_room2[15] = true;
	event_enabling_room2[16] = false;
	event_enabling_room2[17] = false;
	event_enabling_room2[18] = false;
	event_enabling_room2[19] = false;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene5_Loop(void) {
	bool end_sequence;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "SCN51.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN5I].line[40], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN5I].line[41], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene5_LoadAssets();
	Scene5_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();

	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. ??
			Scene5_LoadRoom1();

			ACTOR_SetPosition(44 << 4, 14 << 4, ACTOR_FACING_RIGHT);// Set actor position
			ACTOR_SetCombatMode(true);

			CAM_Init(map.width_px, map.height_px, 30 << 4, 6 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;

			break;
		case 2:// Room 2. ??
			Scene5_LoadRoom2();

			ACTOR_SetPosition(2 << 4, 10 << 4, ACTOR_FACING_RIGHT);// Set actor position
			CAM_Init(map.width_px, map.height_px, 0 << 4, 0 << 4); // Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene5_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {
		switch (engine.room) {
			case 1:// Room 1. Roof
				// Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare
									if (cursor.left_click) {
										//UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN5H, 120, 121, 122, false, 100);

									} else {
										//UI_ShowDescription(UI_TXT_SCN5H, 6);
									}
									break;
								case 2:// Spare
									break;
								case 3:// Spare
									break;
								case 4:// Spare
									break;
								case 5:// Spare
									break;
								case 6:// Spare
									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Spare
						if (event_enabling_room1[1]) {
							event_enabling_room1[1] = false;
						}
						break;
					case 2:// Event 2. Spare
						if (event_enabling_room1[2]) {
							event_enabling_room1[2] = false;
						}
						break;
					case 15:// Event 15. Spare
						if (event_enabling_room1[15]) {
							event_enabling_room1[15] = false;
						}
						break;
					case 16:// Event 16. ??
						if (event_enabling_room1[16]) {
							event_enabling_room1[16] = false;

							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							NPC_UnloadNpcs();
							MAP_UnloadMap();

							Scene5_LoadRoom2();
							ACTOR_SetPosition(2 << 4, 10 << 4, ACTOR_FACING_RIGHT);// Set actor position
							CAM_Init(map.width_px, map.height_px, 0 << 4, 6 << 4); // Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					default:
						break;
				}

				break;
			case 2:// Room 2. ??
				   // Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare

									break;
								case 2:// Spare

									break;
								case 3:// Spare

									break;
								case 4:// Spare

									break;
								case 5:// Spare

									break;
								case 6:// Spare

									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1.
						if (event_enabling_room2[1]) {
							event_enabling_room2[1] = false;
						}
						break;
					case 2:// Event 2.
						if (event_enabling_room2[2]) {
							event_enabling_room2[2] = false;
						}
						break;
					case 3:// Event 3.
						if (event_enabling_room2[3]) {
							event_enabling_room2[3] = false;
						}
						break;
					case 4:// Event 4.
						if (event_enabling_room2[4]) {
							event_enabling_room2[4] = false;
						}
						break;
					case 5:// Event 5.
						if (event_enabling_room2[5]) {
						}
						break;
					case 6:// Event 6.
						if (event_enabling_room2[6]) {
							event_enabling_room2[6] = false;
						}
						break;
					case 15:// Event 15. Go down
						if (event_enabling_room2[15]) {
							event_enabling_room2[15] = false;
							switch (scene_step) {
								case 0:// 6th floor to 5th floor
									VIDEO_FadeOut(4);
									event_enabling_room2[16] = true;
									scene_step++;
									Scene5_Update_Floor5();
									ACTOR_SetPosition(44 << 4, 11 << 4, ACTOR_FACING_UP);  // Set actor position
									CAM_Init(map.width_px, map.height_px, 30 << 4, 0 << 4);// Initialize camera
									MAP_DrawMapToMapVideoBuffer();

									Update(true);
									Update(true);
									VIDEO_FadeIn(1);
									break;
								case 2:// 4th floor to 3th floor
									VIDEO_FadeOut(4);
									event_enabling_room2[16] = true;
									scene_step++;
									Scene5_Update_Floor3();
									ACTOR_SetPosition(44 << 4, 11 << 4, ACTOR_FACING_UP);  // Set actor position
									CAM_Init(map.width_px, map.height_px, 30 << 4, 0 << 4);// Initialize camera
									MAP_DrawMapToMapVideoBuffer();

									Update(true);
									Update(true);
									VIDEO_FadeIn(1);
									break;
								case 4:// 2th floor to 1th floor
									VIDEO_FadeOut(4);
									event_enabling_room2[16] = true;
									scene_step++;
									Scene5_Update_Floor1();
									ACTOR_SetPosition(44 << 4, 11 << 4, ACTOR_FACING_UP);  // Set actor position
									CAM_Init(map.width_px, map.height_px, 30 << 4, 0 << 4);// Initialize camera
									MAP_DrawMapToMapVideoBuffer();

									Update(true);
									Update(true);
									VIDEO_FadeIn(1);
									break;
								default:
									break;
							}
						}
						break;
					case 16:// Event 16. End of level
						if (event_enabling_room2[16]) {
							event_enabling_room2[16] = false;
							switch (scene_step) {
								case 1:// 5th floor to 4th floor
									VIDEO_FadeOut(4);
									event_enabling_room2[15] = true;
									scene_step++;
									Scene5_Update_Floor4();
									ACTOR_SetPosition(2 << 4, 10 << 4, ACTOR_FACING_RIGHT);// Set actor position
									CAM_Init(map.width_px, map.height_px, 0 << 4, 6 << 4); // Initialize camera
									MAP_DrawMapToMapVideoBuffer();

									Update(true);
									Update(true);
									VIDEO_FadeIn(1);
									break;
								case 3:// 3th floor to 2th floor
									VIDEO_FadeOut(4);
									event_enabling_room2[15] = true;
									scene_step++;
									Scene5_Update_Floor2();
									ACTOR_SetPosition(2 << 4, 10 << 4, ACTOR_FACING_RIGHT);// Set actor position
									CAM_Init(map.width_px, map.height_px, 0 << 4, 6 << 4); // Initialize camera
									MAP_DrawMapToMapVideoBuffer();

									Update(true);
									Update(true);
									VIDEO_FadeIn(1);
									break;
								case 5:// 1th floor to street. End of scene
									VIDEO_FadeOut(4);
									engine.ingame = false;
									engine.scene = 6;
									engine.room = 1;
									break;
								default:
									break;
							}
						}
						break;
					default:
						break;
				}
				break;
		}

		Update(true);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene5_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);
}
static void Scene5_Outro(void) {
}

/** SCENARIO 6 :: Mission 6 :: Bullet hell
 */
static void Scene6_Intro(void) {
}
static void Scene6_UnloadAssets(void) {
	// Enemy 1
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_CHAT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_FEET);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_BODY);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_HEAD);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_RARM);
	GFX_UnloadSpriteGraphic(SPRITE_GRAPHICS_ID_ENEMY1_LARM);
}
static void Scene6_LoadAssets(void) {
	// Enemy 1 assets
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ENEMY1_CHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ENEMY1_FEET, 27, 16, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ENEMY1_BODY, 20, 15, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ENEMY1_HEAD, 12, 12, 45, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "LARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_LARM, 16, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ENEMY1.DAT", "RARM.PCX", SPRITE_GRAPHICS_ID_ENEMY1_RARM, 18, 18, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_TEMPORARY_SPRITE);

	ACTOR_Init(9 << 4, 47 << 4, SPRITE_GRAPHICS_ID_ACTOR_FEET, SPRITE_GRAPHICS_ID_ACTOR_BODY, SPRITE_GRAPHICS_ID_ACTOR_HEAD, SPRITE_GRAPHICS_ID_ACTOR_LARM, SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, ACTOR_FACING_RIGHT);
	ACTOR_SetGun(actor.sprite_num, ACTOR_GUN_TYPE_BARE_HANDS);
	ACTOR_SetBulletStatus(99, 19, 199, 9, 9);

	GFX_LoadAnimations("ACTANI.DAT", "FEET.ANI", actor.feet_animation, 150);     // feet animations
	GFX_LoadAnimations("ACTANI.DAT", "BODY.ANI", actor.body_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "HEAD.ANI", actor.head_animation, 150);     // body animations
	GFX_LoadAnimations("ACTANI.DAT", "LARM.ANI", actor.left_arm_animation, 150); // left arm animations
	GFX_LoadAnimations("ACTANI.DAT", "RARM.ANI", actor.right_arm_animation, 150);// right arm animations

	NPC_Init();

	UI_SetStatusPanels();// Set status panels

	AUDIO_LoadSong(AUDIO_SONG_3);// Load song
}
static void Scene6_LoadRoom1(void) {
	MAP_LoadMap("MAPSCN61.DAT", 50, 50, "TSCN61.DAT", "SCN6_1_BACK.PCX", "SCN6_1_FORE.PCX", "SCN6_1_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN61.PCX", 256);
}
static void Scene6_LoadRoom2(void) {
	MAP_LoadMap("MAPSCN62.DAT", 47, 32, "TSCN62.DAT", "SCN6_2_BACK.PCX", "SCN6_2_FORE.PCX", "SCN6_2_MASK.PCX", 320 * 416, 128 * 128, 128 * 128);
	GFX_LoadPalette("PALETTES.DAT", "SCN62.PCX", 256);
}
static void Scene6_SetHotspotsAndEvents(void) {
	// Initialize hotspots and events
	// Update hotspots
	hotspot_enabling_room1[0] = false;
	hotspot_enabling_room1[1] = true;
	hotspot_enabling_room1[2] = true;
	hotspot_enabling_room1[3] = false;
	hotspot_enabling_room1[4] = false;
	hotspot_enabling_room1[5] = false;
	hotspot_enabling_room1[6] = false;
	hotspot_enabling_room1[7] = false;
	hotspot_enabling_room1[8] = false;
	hotspot_enabling_room1[9] = false;
	hotspot_enabling_room1[10] = false;
	hotspot_enabling_room1[11] = false;
	hotspot_enabling_room1[12] = false;
	hotspot_enabling_room1[13] = false;
	hotspot_enabling_room1[14] = false;
	hotspot_enabling_room1[15] = false;
	hotspot_enabling_room1[16] = false;
	hotspot_enabling_room1[17] = false;
	hotspot_enabling_room1[18] = false;
	hotspot_enabling_room1[19] = false;
	hotspot_enabling_room1[20] = false;
	hotspot_enabling_room1[21] = false;
	hotspot_enabling_room1[22] = false;
	hotspot_enabling_room1[23] = false;
	hotspot_enabling_room1[24] = false;
	hotspot_enabling_room1[25] = false;
	hotspot_enabling_room1[26] = false;
	hotspot_enabling_room1[27] = false;
	hotspot_enabling_room1[28] = false;
	hotspot_enabling_room1[29] = false;
	hotspot_enabling_room1[30] = false;
	hotspot_enabling_room1[31] = false;

	// Update events
	event_enabling_room1[0] = false;
	event_enabling_room1[1] = true;
	event_enabling_room1[2] = true;
	event_enabling_room1[3] = false;
	event_enabling_room1[4] = false;
	event_enabling_room1[5] = false;
	event_enabling_room1[6] = false;
	event_enabling_room1[7] = false;
	event_enabling_room1[8] = false;
	event_enabling_room1[9] = false;
	event_enabling_room1[10] = false;
	event_enabling_room1[11] = false;
	event_enabling_room1[12] = false;
	event_enabling_room1[13] = false;
	event_enabling_room1[14] = false;
	event_enabling_room1[15] = true;
	event_enabling_room1[16] = true;
	event_enabling_room1[17] = false;
	event_enabling_room1[18] = false;
	event_enabling_room1[19] = false;
	event_enabling_room1[20] = false;
	event_enabling_room1[21] = false;
	event_enabling_room1[22] = false;
	event_enabling_room1[23] = false;
	event_enabling_room1[24] = false;
	event_enabling_room1[25] = false;
	event_enabling_room1[26] = false;
	event_enabling_room1[27] = false;
	event_enabling_room1[28] = false;
	event_enabling_room1[29] = false;
	event_enabling_room1[30] = false;
	event_enabling_room1[31] = false;

	// Update hotspots
	hotspot_enabling_room2[0] = false;
	hotspot_enabling_room2[1] = false;
	hotspot_enabling_room2[2] = false;
	hotspot_enabling_room2[3] = false;
	hotspot_enabling_room2[4] = false;
	hotspot_enabling_room2[5] = false;
	hotspot_enabling_room2[6] = false;
	hotspot_enabling_room2[7] = false;
	hotspot_enabling_room2[8] = false;
	hotspot_enabling_room2[9] = false;
	hotspot_enabling_room2[10] = false;
	hotspot_enabling_room2[11] = false;
	hotspot_enabling_room2[12] = false;
	hotspot_enabling_room2[13] = false;
	hotspot_enabling_room2[14] = false;
	hotspot_enabling_room2[15] = false;
	hotspot_enabling_room2[16] = false;
	hotspot_enabling_room2[17] = false;
	hotspot_enabling_room2[18] = false;
	hotspot_enabling_room2[19] = false;
	hotspot_enabling_room2[20] = false;
	hotspot_enabling_room2[21] = false;
	hotspot_enabling_room2[22] = false;
	hotspot_enabling_room2[23] = false;
	hotspot_enabling_room2[24] = false;
	hotspot_enabling_room2[25] = false;
	hotspot_enabling_room2[26] = false;
	hotspot_enabling_room2[27] = false;
	hotspot_enabling_room2[28] = false;
	hotspot_enabling_room2[29] = false;
	hotspot_enabling_room2[30] = false;
	hotspot_enabling_room2[31] = false;

	// Update events
	event_enabling_room2[0] = false;
	event_enabling_room2[1] = true;
	event_enabling_room2[2] = true;
	event_enabling_room2[3] = true;
	event_enabling_room2[4] = true;
	event_enabling_room2[5] = true;
	event_enabling_room2[6] = false;
	event_enabling_room2[7] = false;
	event_enabling_room2[8] = false;
	event_enabling_room2[9] = false;
	event_enabling_room2[10] = false;
	event_enabling_room2[11] = false;
	event_enabling_room2[12] = false;
	event_enabling_room2[13] = false;
	event_enabling_room2[14] = false;
	event_enabling_room2[15] = false;
	event_enabling_room2[16] = true;
	event_enabling_room2[17] = false;
	event_enabling_room2[18] = false;
	event_enabling_room2[19] = false;
	event_enabling_room2[20] = false;
	event_enabling_room2[21] = false;
	event_enabling_room2[22] = false;
	event_enabling_room2[23] = false;
	event_enabling_room2[24] = false;
	event_enabling_room2[25] = false;
	event_enabling_room2[26] = false;
	event_enabling_room2[27] = false;
	event_enabling_room2[28] = false;
	event_enabling_room2[29] = false;
	event_enabling_room2[30] = false;
	event_enabling_room2[31] = false;
}
static void Scene6_Loop(void) {
	bool end_sequence;
	int actor_tile_num;
	int sequence_step;
	int scene_step;

	VIDEO_ClearScreenBuffer();
	GFX_LoadPalette("PALETTES.DAT", "SCN61.PCX", 256);

	VIDEO_StringToScreenBuffer(60, 80, ui.txt_file[UI_TXT_SCN6I].line[40], FONT_BIG_WHITE);
	VIDEO_StringToScreenBuffer(80, 105, ui.txt_file[UI_TXT_SCN6I].line[41], FONT_BIG_WHITE);
	VIDEO_VSync();
	VIDEO_ScreenBufferToVRAM();
	VIDEO_FadeIn(4);

	Scene6_LoadAssets();
	Scene6_SetHotspotsAndEvents();

	VIDEO_FadeOut(4);

	scene_step = 0;
	sequence_step = 0;
	end_sequence = false;
	engine.ingame = true;

	MOUSE_ShowCursor();

	// Initialize the Scene room
	switch (engine.room) {
		case 1:// Room 1. ??
			Scene6_LoadRoom1();

			ACTOR_SetPosition(9 << 4, 47 << 4, ACTOR_FACING_UP);// Set actor position
			ACTOR_SetCombatMode(true);

			CAM_Init(map.width_px, map.height_px, 0 << 4, 37 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;

			break;
		case 2:// Room 2. ??
			Scene6_LoadRoom2();

			ACTOR_SetPosition(23 << 4, 29 << 4, ACTOR_FACING_UP);   // Set actor position
			CAM_Init(map.width_px, map.height_px, 13 << 4, 20 << 4);// Initialize camera
			MAP_DrawMapToMapVideoBuffer();

			Update(true);
			Update(true);
			VIDEO_FadeIn(1);
			break;
		default:
			Error("Scene5_Loop function error", "Undefined room", "", ERROR_SYSTEM);
			break;
	}

	// Loop until the game is over
	while (engine.ingame) {
		switch (engine.room) {
			case 1:// Room 1. Garden
				// Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare
									if (cursor.left_click) {
										//UI_ShowSpeech(&gfx_chat_panel, SPRITE_GRAPHICS_ID_ACTORCHAT, SPRITE_GRAPHICS_ID_CHAT, UI_TXT_SCN5H, 120, 121, 122, false, 100);

									} else {
										//UI_ShowDescription(UI_TXT_SCN5H, 6);
									}
									break;
								case 2:// Spare
									break;
								case 3:// Spare
									break;
								case 4:// Spare
									break;
								case 5:// Spare
									break;
								case 6:// Spare
									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1. Spare
						if (event_enabling_room1[1]) {
							event_enabling_room1[1] = false;
						}
						break;
					case 2:// Event 2. Spare
						if (event_enabling_room1[2]) {
							event_enabling_room1[2] = false;
						}
						break;
					case 15:// Event 15. Spare
						if (event_enabling_room1[15]) {
							event_enabling_room1[15] = false;
						}
						break;
					case 16:// Event 16. ??
						if (event_enabling_room1[16]) {
							event_enabling_room1[16] = false;

							VIDEO_FadeOut(4);
							ENEMY_UnloadEnemies();
							OBJECT_UnloadObjects();
							EFFECT_UnloadEffects();
							BULLET_UnloadBullets();
							PARTICLE_UnloadParticles();
							GRENADE_UnloadGrenades();
							NPC_UnloadNpcs();
							MAP_UnloadMap();

							Scene6_LoadRoom2();
							ACTOR_SetPosition(23 << 4, 29 << 4, ACTOR_FACING_UP);   // Set actor position
							CAM_Init(map.width_px, map.height_px, 13 << 4, 20 << 4);// Initialize camera
							MAP_DrawMapToMapVideoBuffer();

							Update(true);
							Update(true);

							engine.room = 2;

							VIDEO_FadeIn(1);
						}
						break;
					default:
						break;
				}

				break;
			case 2:// Room 2. ??
				   // Hotspots
				if (!actor.mode_combat) {
					// Check point on object or enemy
					switch (cursor.point_on & 0xFF) {
						case ENTITY_ID_HSPOT:// Hotspot
							switch ((cursor.point_on >> 8) & 0xFF) {
								case 0:// No hotspot, WTF??
									break;
								case 1:// Spare

									break;
								case 2:// Spare

									break;
								case 3:// Spare

									break;
								case 4:// Spare

									break;
								case 5:// Spare

									break;
								case 6:// Spare

									break;
								default:
									break;
							}
						default:
							break;
					}
				}

				// Events
				actor_tile_num = MAP_GetTileNumber(actor.pos_x + (gfx_sprite_stack[actor.sprite_num].width_px >> 1), actor.pos_y + (gfx_sprite_stack[actor.sprite_num].height_px >> 1));
				switch ((map.layer[MAP_EVENT_HSPOT_LAYER][actor_tile_num] >> 8) & 0xFF) {
					case 1:// Event 1.
						if (event_enabling_room2[1]) {
							event_enabling_room2[1] = false;
						}
						break;
					case 2:// Event 2.
						if (event_enabling_room2[2]) {
							event_enabling_room2[2] = false;
						}
						break;
					case 3:// Event 3.
						if (event_enabling_room2[3]) {
							event_enabling_room2[3] = false;
						}
						break;
					case 4:// Event 4.
						if (event_enabling_room2[4]) {
							event_enabling_room2[4] = false;
						}
						break;
					case 5:// Event 5.
						if (event_enabling_room2[5]) {
						}
						break;
					case 6:// Event 6.
						if (event_enabling_room2[6]) {
							event_enabling_room2[6] = false;
						}
						break;
					case 16:// Event 16. End of level
						if (event_enabling_room2[16]) {
							event_enabling_room2[16] = false;

							VIDEO_FadeOut(4);
							engine.ingame = false;
							engine.scene = 7;
							engine.room = 1;
						}
						break;
					default:
						break;
				}

				break;
		}

		Update(true);

		// Finish all if actor is dead
		if (actor.status_dead) GameOver();
	}

	VIDEO_FadeOut(4);
	VIDEO_ClearScreen();
	AUDIO_StopSong();
	ENEMY_UnloadEnemies();
	OBJECT_UnloadObjects();
	EFFECT_UnloadEffects();
	BULLET_UnloadBullets();
	PARTICLE_UnloadParticles();
	GRENADE_UnloadGrenades();
	ACTOR_UnloadActor();
	MAP_UnloadMap();
	Scene6_UnloadAssets();
	MM_PopChunks(CT_TEMPORARY_SPRITE);
}
static void Scene6_Outro(void) {
}

/** SCENARIO 7 :: End credits
 */
static void Scene7_EndCredits(void) {

	// end

	engine.exit_game = true;
}

static void Test(void) {
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SPARK.PCX", SPRITE_GRAPHICS_ID_SPARK_EFFECT, 16, 16, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "PUNCHL.PCX", SPRITE_GRAPHICS_ID_PUNCH_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "PISTOLL.PCX", SPRITE_GRAPHICS_ID_PISTOL_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SHOTGUNL.PCX", SPRITE_GRAPHICS_ID_SHOTGUN_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "UZIL.PCX", SPRITE_GRAPHICS_ID_UZI_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "POINTER.PCX", SPRITE_GRAPHICS_ID_CURSOR, 7, 12, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("CURSOR.DAT", "AIM.PCX", SPRITE_GRAPHICS_ID_AIM, 15, 15, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT, 220, 70, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET1.PCX", SPRITE_GRAPHICS_ID_BULLET1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "BULLET2.PCX", SPRITE_GRAPHICS_ID_BULLET2, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "GRENADE1.PCX", SPRITE_GRAPHICS_ID_GRENADE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "GRENADES.PCX", SPRITE_GRAPHICS_ID_GRENADE_SHADOW, 8, 8, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUNCH1.PCX", SPRITE_GRAPHICS_ID_PUNCH, 12, 12, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("BULLETS.DAT", "PUKE.PCX", SPRITE_GRAPHICS_ID_BULLET3, 16, 16, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "SNIPPERL.PCX", SPRITE_GRAPHICS_ID_SNIPPER_EFFECT, 24, 16, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "BLOOD.PCX", SPRITE_GRAPHICS_ID_BLOOD, 4, 4, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "FIRE1.PCX", SPRITE_GRAPHICS_ID_FIRE1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "ELECTRIC1.PCX", SPRITE_GRAPHICS_ID_ELECTRIC1, 8, 8, 4, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "EXPLOSION.PCX", SPRITE_GRAPHICS_ID_EXPLOSION1, 32, 32, 10, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "TIMER.PCX", SPRITE_GRAPHICS_ID_TIMER, 8, 8, 10, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "CLICK.PCX", SPRITE_GRAPHICS_ID_CLICK, 5, 5, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("EFFECTS.DAT", "RAIN.PCX", SPRITE_GRAPHICS_ID_RAIN_EFFECT, 5, 5, 5, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("ITEMS.DAT", "MEDIKIT.PCX", SPRITE_GRAPHICS_ID_ITEM_MEDIKIT, 12, 14, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ITEMS.DAT", "AMMO3.PCX", SPRITE_GRAPHICS_ID_ITEM_AMMO3, 15, 13, 3, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1.PCX", SPRITE_GRAPHICS_ID_BARREL1, 16, 32, 14, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "BARREL1P.PCX", SPRITE_GRAPHICS_ID_BARREL1_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN00.PCX", SPRITE_GRAPHICS_ID_GUN0, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN01.PCX", SPRITE_GRAPHICS_ID_GUN1, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN02.PCX", SPRITE_GRAPHICS_ID_GUN2, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN03.PCX", SPRITE_GRAPHICS_ID_GUN3, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "LIFEBAR.PCX", SPRITE_GRAPHICS_ID_LIFEBAR, 48, 6, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN04.PCX", SPRITE_GRAPHICS_ID_GUN4, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GUN05.PCX", SPRITE_GRAPHICS_ID_GUN5, 32, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("PANEL.DAT", "GRENADE.PCX", SPRITE_GRAPHICS_ID_GRENADE, 16, 16, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "FACE.PCX", SPRITE_GRAPHICS_ID_ACTORCHAT, 80, 82, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "PORTAIT.PCX", SPRITE_GRAPHICS_ID_ACTOR_PORTAIT, 32, 32, 2, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "HEAD.PCX", SPRITE_GRAPHICS_ID_ACTOR_HEAD, 12, 12, 30, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "FEET.PCX", SPRITE_GRAPHICS_ID_ACTOR_FEET, 27, 16, 84, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "BODY.PCX", SPRITE_GRAPHICS_ID_ACTOR_BODY, 20, 15, 100, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "LARM1.PCX", SPRITE_GRAPHICS_ID_ACTOR_LARM, 16, 18, 90, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM1.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_BARE_HANDS, 16, 18, 135, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM2.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_PISTOL, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM3.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_SHOTGUN, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM4.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_UZI, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("ACTOR.DAT", "RARM5.PCX", SPRITE_GRAPHICS_ID_ACTOR_RARM_RIFLE, 20, 20, 72, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "EMPTY.PCX", SPRITE_GRAPHICS_ID_EMPTY, 1, 1, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "TARGET1.PCX", SPRITE_GRAPHICS_ID_TARGET1, 20, 32, 6, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "TARGET2.PCX", SPRITE_GRAPHICS_ID_TARGET2, 20, 32, 7, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);

	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "CHAT.PCX", SPRITE_GRAPHICS_ID_CHAT_SMALL, 55, 30, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
	GFX_LoadSpriteGraphicsRLE("MISC.DAT", "CHAT_BIG.PCX", SPRITE_GRAPHICS_ID_CHAT_BIG, 140, 50, 1, SPRITE_TRANSP_COLOR, SPRITE_HIT_COLOR, CT_SPRITE);
}

void set_video_mode(int mode) {
	__dpmi_regs regs = {0};
	regs.x.ax = mode;
	__dpmi_int(0x10, &regs);
}

/** MAIN FUNCTION*******
 */
int main(int argc, char **argv) {
	//gdb_start();

	engine.good_mode = false;
	engine.debug_mode = false;

	// Check for debug mode argument
	switch (argc) {
		case 3:
			// Two arguments
			if (strcmp(argv[2], "-god") == 0) {
				engine.good_mode = true;
				printf("God mode enabled\n");
				sleep(1);
			}

			if (strcmp(argv[2], "-debug") == 0) {
				engine.debug_mode = true;
				printf("Debug mode enabled\n");
				sleep(1);
			}
		case 2:
			// One argument
			if (strcmp(argv[1], "-god") == 0) {
				engine.good_mode = true;
				printf("God mode enabled\n");
				sleep(1);
			}

			if (strcmp(argv[1], "-debug") == 0) {
				engine.debug_mode = true;
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

	/****************************
	*    GAME STARTS HERE!!
	*
	****************************/
	LoadTexts();

	//Test();

	Logo();

	Intro();

	engine.exit_game = false;
	engine.scene = 0;
	engine.room = 1;

	MOUSE_InitCursorSprite(SPRITE_GRAPHICS_ID_CURSOR);// Initialize mouse cursor

	while (!engine.exit_game) {
		switch (engine.scene) {
			case 0:// main menu
				Menu();
				break;
			case 1:// scene 1 :: Mission 1 :: The travel
				Scene1_Intro();
				Scene1_Loop();
				if (!actor.status_dead) Scene1_Outro();
				break;
			case 2:// scene 2 :: Mission 2 :: Down to the hell
				Scene2_Intro();
				Scene2_Loop();
				if (!actor.status_dead) Scene2_Outro();
				break;
			case 3:// scene 3 :: Mission 3 :: Behind enemy lines
				Scene3_Intro();
				Scene3_Loop();
				if (!actor.status_dead) Scene3_Outro();
				break;
			case 4:// scene 4 :: Mission 4 :: Praise for mercy
				Scene4_Intro();
				Scene4_Loop();
				if (!actor.status_dead) Scene4_Outro();
				break;
			case 5:// scene 5 :: Mission 5 :: Hit and run
				Scene5_Intro();
				Scene5_Loop();
				if (!actor.status_dead) Scene5_Outro();
				break;
			case 6:// scene 6 :: Mission 6 :: Bullet hell
				Scene6_Intro();
				Scene6_Loop();
				if (!actor.status_dead) Scene6_Outro();
				break;
			case 7:// scene 7 :: End scene
				Scene7_EndCredits();
				break;
			default:
				// Unknown level
				engine.exit_game = true;
				break;
		}
	}
	ExitDos();
	return 0;
}