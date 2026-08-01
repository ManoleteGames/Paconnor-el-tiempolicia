#ifndef EGA_H
#define EGA_H

#include "../types/types.h"

#define EGA_RESOLUTION_WIDTH 320
#define EGA_RESOLUTION_HEIGHT 200

extern byte *EGA;
void EGA_VSync(void);
bool EGA_CheckGraphicsCard(void);
void EGA_ScreenMainBufferToVRAM(void);
void EGA_ImageToVRAM(byte *buffer, int width, int height);

#endif