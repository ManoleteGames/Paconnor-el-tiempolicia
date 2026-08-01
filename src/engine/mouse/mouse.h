
#ifndef MOUSE_H
#define MOUSE_H

#include "../types/types.h"


#define MOUSE_TRANSP_COLOR 220
#define MOUSE_SPRITE_WIDTH 16
#define MOUSE_SPRITE_HEIGHT 16
#define MOUSE_TILESET_WIDTH 128
#define MOUSE_TILESET_HEIGHT 16
#define MOUSE_PALETTE_OFFSET 220

#define MOUSE_RIGHT_CLICK_DELAY 100

#define MOUSE_IRQ 0x33

extern MouseData cursor;
void MOUSE_Init(void);
void MOUSE_Shutdown(void);
bool MOUSE_CheckIfAvailable(void);
void MOUSE_EnableCursor(void);
void MOUSE_RestrictZone(int x1, int x2, int y1, int y2);
void MOUSE_SetCursorFrame(int number);
void MOUSE_SetClick(int number);
void MOUSE_SetOrigin(int x, int y);
void MOUSE_InitCursorSprite(byte graphics_id);
void MOUSE_Update(bool combat_mode);
void MOUSE_SetCursorGraphics(int graphics_id);
void MOUSE_MaskRightClick(void);
void MOUSE_HideCursor(void);
void MOUSE_ShowCursor(void);

#endif