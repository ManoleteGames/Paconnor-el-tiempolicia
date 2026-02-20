/*  VGA header
    - Contains all VGA registers and values
 */

#define VGA_VIDEO_INT 0x10// Video interrupt number
#define VGA_TEXT_MODE 0x03// use to set back to 80x25 text mode.
#define VGA_GRAPHIC_MODE 0x13
#define VGA_ADC_PORT 0x3DA// Attribute controller data port

#define VGA_PALETTE_INDEX_RD 0x03c7
#define VGA_PALETTE_INDEX_WR 0x03c8
#define VGA_PALETTE_DATA 0x03c9

#define VGA_RESOLUTION_WIDTH 320
#define VGA_RESOLUTION_HEIGHT 208//208

#define VGA_TRANSPARENT_COLOR 220