#include <bios.h>
#include <dpmi.h>
#include <sys/nearptr.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/farptr.h>
#include <go32.h>
#include <io.h>
#include <dos.h>
#include <stdint.h>
#include <string.h>
#include <pc.h>

#include "settings.h"
#include "types/types.h"
#include "video/video.h"
#include "file/file.h"
#include "gfx/gfx.h"
#include "timer/timer.h"
#include "keyb/keyb.h"
#include "mem/mem.h"
#include "actor/actor.h"
#include "camera/camera.h"
#include "map/map.h"
#include "object/object.h"
#include "enemy/enemy.h"
#include "audio/audio.h"
#include "audio/a2m.h"
#include "particle/particle.h"
#include <errno.h>


#define DOS_MAJOR_VERSION 5
#define DOS_MINOR_VERSION 0


#define SPRITE_MAX_GRAPHICS 32
#define SPRITE_MAX_STACK 64
#define SPRITE_TRANSP_COLOR 220

#define ACTOR_MAX_BULLETS 64

#define ERROR_SYSTEM 1
#define ERROR_MEMORY 2
#define ERROR_VIDEO 3
#define ERROR_FILE 4
#define ERROR_SOUND 5
#define ERROR_GRAPHICS 6
#define ERROR_MOUSE 7
#define ERROR_BINARY_FILE 99

#define ENTITY_ID_EMPTY 0x00
#define ENTITY_ID_BACKGROUND 0x01
#define ENTITY_ID_CURSOR 0x20
#define ENTITY_ID_ACTOR 0x30
#define ENTITY_ID_ACTOR_BULLET 0x40
#define ENTITY_ID_BARREL 0x50
#define ENTITY_ID_ENEMY 0x60
#define ENTITY_ID_ENEMY_BULLET 0x70
#define ENTITY_ID_EXPLOSION 0x80

extern System engine;
extern int errno;
void InitEngine(void);
void AwaitInput(void);
void Error(const char *quit_message1, const char *quit_message2, const char *quit_message3, int exit_code);
void ExitDos(void);
void Update(int player_follow);
void UpdateStatusPannel(void);
void FPS(void);

/// VIDEO.C ///
extern Video video;
void VIDEO_Init(void);
void VIDEO_Shutdown(void);
extern void (*VIDEO_VSync)(void);
extern void (*VIDEO_ScreenBufferToVRAM)(void);
void VIDEO_SetActiveBuffer(int bufferNumber);
void VIDEO_CharToScreenBuffer(int x, int y, word width, word height, byte transparent_color, byte *data);
void VIDEO_StringToScreenBuffer(int x, int y, int length, unsigned char *string);
void VIDEO_DrawSpriteToScreenBufferRLE(SpriteGraphic *spr_graphics, Sprite spr);
void VIDEO_MapBufferToScreenBuffer(void);
void VIDEO_MapBufferToScreenBuffer_Paralax(void);
void VIDEO_DrawLifeBarOnScreenBuffer(int x, int y, int length, int life);
void VIDEO_PanelToScreenBuffer(StatusPanel *panel);
void VIDEO_DrawLifeBarOnScreenPanel(int x, int y, int length, int life, StatusPanel panel);
void VIDEO_BinaryImageToVRAM(const char *dat_name, const char *asset_name);
void VIDEO_PCXImageToVRAM(const char *dat_name, const char *asset_name, int size);
void VIDEO_PCXImageToScreenBuffer(const char *dat_name, const char *asset_name, int size);
void VIDEO_SetPalette(byte *palette);
void VIDEO_ClearScreen(void);
void VIDEO_ClearPalette(void);
bool VIDEO_FadeIn_Async(int speed, int *step);
bool VIDEO_FadeOut_Async(void);
void VIDEO_FadeIn(int speed);
void VIDEO_FadeOut(int speed);
void VIDEO_DrawPixelOnScreenBuffer(int x, int y, byte color);
void VIDEO_FadeOutPause(int atenuation);

/// CGA.C //
extern byte *CGA;
void CGA_VSync(void);
void CGA_ScreenMainBufferToVRAM(void);
void CGA_ImageToVRAM(byte *buffer, int width, int height);

/// EGA.C //
extern byte *EGA;
void EGA_VSync(void);
void EGA_ScreenMainBufferToVRAM(void);
void EGA_ImageToVRAM(byte *buffer, int width, int height);

/// VGA.C //
extern byte *VGA;
void VGA_HideCursor(void);
bool VGA_CheckGraphicsCard(void);
void VGA_SetPalette(byte *palette);
void VGA_SetPaletteRange(byte *palette, int start, int end);
void VGA_ClearPalete(void);
void VGA_SetMode(int mode);
void VGA_VSync(void);
void VGA_ScreenMainBufferToVRAM(void);
void VGA_ImageToVRAM(byte *buffer, int width, int height);
void VGA_ClearScreen(void);
void VGA_ClearPalette(void);
void VGA_FadeIn(int speed);
void VGA_FadeOut(int speed);
bool VGA_FadeIn_Async(int speed, int *step);
void VGA_RotatePaletteAsync(int index1, int index2);
void VGA_RotatePalette(int index1, int index2, int speed);

/// CAMERA.C ///
extern Camera camera;
extern CameraParallax cam_parallax[3];
void CAM_Init(void);
void CAM_MainPositionControl(bool player_follow, bool *map_update_flag);
void CAM_SetPosition(int x, int y);
void CAM_SetLimits(int x, int y);
void CAM_UpdateParalax(int delay1, int delay2, int delay3);

/// GFX.C ///
extern byte *gfxPaletteShown;
extern byte *gfxPaletteLoaded;
extern byte *gfxFont;
extern SpriteGraphic gfx_sprite_graphics_stack[SPRITE_MAX_GRAPHICS];
extern Sprite gfx_sprite_stack[SPRITE_MAX_STACK];
extern StatusPanel gfx_actor_status_panel;
extern StatusPanel gfx_enemy_status_panel;
void GFX_Init(void);
void GFX_Shutdown(void);
void GFX_LoadSpriteGraphics(const char *dat_name, const char *asset_name, int id, int width_px, int height_px, int num_frames, int transparent_color, int hit_color);
void GFX_LoadSpriteGraphicsRLE(const char *dat_name, const char *asset_name, int id, int width_px, int height_px, int num_frames, int transparent_color, int hit_color);
void GFX_UnloadSpriteGraphics(void);
bool GFX_InitSprite(byte graphics_id, byte entity_id, byte entity_num, int *number);
void GFX_UnloadSprite(int number);
void GFX_UnloadSprites(void);
void GFX_DrawSprites(void);
void GFX_LoadImage(const char *filename, const char *subfile, int size);
void GFX_LoadFont(const char *filename, const char *subfile, int size);
void GFX_LoadPalette(const char *dat_file, const char *asset_file, int size);
void GFX_UpdateSprites(void);
void GFX_SetPanelGraphics(StatusPanel *panel, int portait_graphics_id, int lifebar_graphics_id);
void GFX_SetPanelPosition(StatusPanel *panel, int x, int y, int portait_x, int portait_y, int lifebar_x, int lifebar_y);
void GFX_ShowPanel(StatusPanel *panel, bool show, int timeout);
void GFX_UpdatePanel(StatusPanel *panel, int life, int old_life, int max_life, int portait_frame);
void GFX_SetPanelPortait(StatusPanel *panel, int portait_frame);

/// OBJECT.C ///
extern Object object[OBJECT_MAX_OBJECTS];
void OBJECT_LoadObject(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y);
void OBJECT_UpdateObjects(void);

/// MAP.C ///
extern Map map;
void MAP_Init(void);
void MAP_Shutdown(void);
void MAP_LoadTileset(const char *filename, const char *subfile, Tileset *tset, int size);
void MAP_LoadMap(const char *dat_file, int width, int height);
void MAP_DrawMapToMapVideoBuffer(void);
void MAP_Update(void);
void MAP_Update_Paralax(void);
int MAP_GetTileNumber(int x_px, int y_px);
int MAP_CheckColissionTile(int x_px, int y_px);
void MAP_DrawForegroundTile(int x, int y, int num_tile);
void MAP_DrawToVideoBuffer(int x_px, int y_px, int width_px, int height_px, byte *src_buffer, byte *video_buffer);

/// FILE.C ///
void FILE_LoadBinaryImage(const char *dat_name, const char *asset_name, byte *buffer);
void FILE_LoadPCXImage(const char *dat_name, const char *asset_name, byte *buffer, long size, byte *pal, word *width, word *height);
void FILE_LoadPCXTileset(const char *dat_name, const char *asset_name, byte *buffer, long size, word *width, word *height);
void FILE_LoadMap_CSV(const char *dat_name, int *back, int *fore, int *mask, int *col, int *event_hspot, int size);
void FILE_LoadPCXSprite(const char *dat_name, const char *asset_name, byte *buffer, long size, word *width, word *height, int palette_offset);
void FILE_LoadPCXPalette(const char *dat_name, const char *asset_name, byte *buffer, long size);
dword FILE_SeekAssetOffset(FILE *fp, const char *filename);
int FILE_LoadText(const char *dat_name, const char *asset_name, int line, unsigned char *str);
void FILE_LoadInstruments(const char *dat_name, const char *asset_name, byte *inst1, byte *inst2, byte *inst3, byte *inst4, byte *inst5, byte *inst6, byte *inst7, byte *inst8, byte *inst9, byte *inst10, byte *inst11);
void FILE_LoadMidiSong(const char *dat_name, const char *asset_name);
byte *FILE_LoadA2MSongInfo(const char *dat_name, const char *asset_name, dword *size);

/// UTILS.C ///
void WriteWord(word value, int fd);
word ReadWord(int fd);


/// SOUND.C //
extern bool SOUND_Initialized;

/// TIMER.C //
extern bool TIMER_Initialized;
void TIMER_Init(void);
void TIMER_Shutdown(void);
dword TIMER_GetMilliseconds(void);
byte TIMER_SetTimeout(void (*callback)(byte), byte param, dword ms);
void TIMER_ClearAllTimeouts(void);
void TIMER_ResetTime(void);

/// KEYB.c //
extern bool KEYB_Initialized;
extern byte kbBindingUp;
extern byte kbBindingDown;
extern byte kbBindingLeft;
extern byte kbBindingRight;
extern byte kbBindingJump;
extern byte kbBindingFire;
extern byte kbBindingCombat;
void KEYB_Shutdown(void);
#define NO_KEY_PRESSED() (kbLastScancode & 0x80)
#define ANY_KEY_PRESSED() !(kbLastScancode & 0x80)
#define LAST_SCANCODE() (kbLastScancode & 0x7F)

extern byte kbLastScancode;

extern byte kbBindingUp;
extern byte kbBindingDown;
extern byte kbBindingLeft;
extern byte kbBindingRight;
extern byte kbBindingJump;
extern byte kbBindingFire;
extern byte kbBindingCombat;

extern bool kbKeyState[128];
extern byte lastKeyPressed;
void KEYB_Init(void);
void KEYB_Free(void);

/// MEM.c //
extern bool MM_Initialized;
extern dword mmMemTotal;
extern dword mmMemUsed;
void MM_Init(void);
void MM_Shutdown(void);
void *MM_PushChunk(word size, ChunkType type);
void MM_PopChunks(ChunkType type);

/// MOUSE.c //
extern bool mouse_present;
extern bool mouse_initialized;
extern MouseData cursor;
void MOUSE_Init(void);
void MOUSE_Shutdown(void);
bool MOUSE_CheckIfAvailable(void);
void MOUSE_ShowCursor(void);
void MOUSE_RestrictZone(int x1, int x2, int y1, int y2);
void MOUSE_SetCursorFrame(int number);
void MOUSE_SetClick(int number);
void MOUSE_SetOrigin(int x, int y);
void MOUSE_InitCursorSprite(byte graphics_id);
void MOUSE_Update(bool combat_mode);

/// ACTOR.c //
extern Actor actor;
void ACTOR_SetGun(int type, int graphics_id, int bullet_graphics_id);
void ACTOR_Init(int x, int y, int graphics_id, int gun_graphics_id, int bullet_graphics_id, int colission_frame, int hit_frame);
void ACTOR_Update(void);
void ACTOR_InitBullet(int source_x, int source_y, int target_x, int target_y, int accuracy);
void ACTOR_UpdateBullets(void);
void ACTOR_DrawColissionPixels(void);
void ACTOR_DrawHitPixels(void);

/// UTILS.c //
word ISQRT(dword n);

/// AUDIO.c //
extern bool AUDIO_Initialized;
extern const word FNr[12];
extern Instrument flute;
extern Instrument piano;
extern Instrument harp;
extern Instrument flute;
extern Instrument piano;
extern Instrument harp;
extern Instrument drum1;
extern Instrument drum2;
extern Instrument drum3;
extern MIDI_Song midi_song;
extern Song song;
void AUDIO_Init(void);
void AUDIO_Shutdown(void);
void AUDIO_PlaySound(byte sound);
int AUDIO_TestOPL(int port);
bool AUDIO_CheckSoundBlaster(void);
bool AUDIO_CheckAdlib(void);
void AUDIO_PlayIntro(void);
void AUDIO_TimerHandler(void);
void AUDIO_PlayMIDI(void);
void AUDIO_LoadMIDI(const char *dat_name, const char *instruments_name, const char *song_name);
void AUDIO_LoadSong(const char *dat_name, const char *asset_name);
void AUDIO_PlaySong(void);
void AUDIO_StopSong(void);

void SetInstr(unsigned char Voice, unsigned char I, unsigned char Volume);
void A2M_UnpackFile(void);
void A2M_LoadFile(const char *dat_name, const char *asset_name);
void A2M_TimerHandler(void);
void A2M_SetInstrument(byte chan, A2M_INSTR_DATA instr);
void A2M_UpdateEffectsSlot(int slot, int chan);
/// A2M.c //
extern A2M_SONGINFO songinfo;

/// SBLASTER.C //
extern bool sblaster_present;
extern SBlaster sblaster;
bool SB_Check(void);
void SB_Init(void);
void SB_Shutdown(void);
void SB_NoteOn(byte voice, byte note, byte block);
void SB_NoteOff(byte voice, byte note, byte block);
void SB_WriteFM(byte reg, byte value);

bool SB_ResetDSP(word Port);
word SB_GetDSPVersion(void);
byte SB_TestLoDMA(void);

/// ADLIB.c //
extern bool adlib_present;
extern Adlib adlib;
bool ADLIB_CheckFMInstalled(void);
void ADLIB_FM_Test(void);

/// SPEAKER.c //
extern int speaker_note[96];
void SPK_Unmute(void);
void SPK_Mute(void);
void SPK_Sound(int freq);
void SPK_NoSound(void);
void SPK_Test(void);

//// ENEMY.c //
extern Enemy enemy[ENEMY_MAX_ENEMIES];
void ENEMY_Init(byte number, int x, int y, int facing, int graphics_id, int gun_type, int gun_graphics_id, int bullet_graphics_id);
void ENEMY_Update(void);
void ENEMY_DrawColissionPixels(Enemy e);
void ENEMY_DrawHitPixels(Enemy e);

/// PARTICLES.c //
extern Particle particle[PARTICLE_MAX_PARTICLES];
void PARTICLE_InitParticle(int graphics_id, int source_x, int source_y, int target_x, int target_y, int speed, int damage);
int PARTICLE_CheckParticleColission(Particle obj);
void PARTICLE_UpdateParticles(void);

/// UI.c //
extern UI ui;
void UI_UpdateUI(void);