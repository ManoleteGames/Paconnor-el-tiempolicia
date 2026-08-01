#ifndef CGA_H
#define CGA_H

#include "../types/types.h"

#define CGA_RESOLUTION_WIDTH 320
#define CGA_RESOLUTION_HEIGHT 200

extern byte *CGA;
void CGA_VSync(void);
bool CGA_CheckGraphicsCard(void);
void CGA_ScreenMainBufferToVRAM(void);
void CGA_ImageToVRAM(byte *buffer, int width, int height);

#endif