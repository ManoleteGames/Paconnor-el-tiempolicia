#ifndef ITEM_H
#define ITEM_H

#include "../types/types.h"

#define ITEM_MAX_ITEMS 16

#define ITEM_WIDTH 12
#define ITEM_HEIGHT 12
#define ITEM_NUM_FRAMES 3

/// OBJECT.C ///
extern Item item[ITEM_MAX_ITEMS];
void ITEM_LoadItem(byte number, byte entity_id, byte graphics_id, int pos_x, int pos_y);
void ITEM_UpdateItems(void);
void ITEM_UnloadItems(void);
void ITEM_UnloadItem(int number);
bool ITEM_IsOnScreen(int number);
void ITEM_SetPosition(int number, int pos_x, int pos_y);
bool ITEM_IsLoaded(int number);

#endif