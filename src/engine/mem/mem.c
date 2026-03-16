
#include "../engine.h"
#include "mem.h"

bool MM_Initialized = false;

word mmChunkSizes[MM_MAX_NUM_CHUNKS];
ChunkType mmChunkTypes[MM_MAX_NUM_CHUNKS];

long int *mmRawMem;
dword mmMemTotal;
dword mmMemUsed;
word mmChunksUsed;

/** MEM :: Memory manager initialization
 */
void MM_Init(void) {
	int i;

	if (!MM_Initialized) {
		// Clear chunck data and set all chunks as temporary
		for (i = 0; i < MM_MAX_NUM_CHUNKS; ++i) {
			mmChunkSizes[i] = 0;
			mmChunkTypes[i] = CT_TEMPORARY;
		}

		mmMemTotal = MM_RAM_REQ * 1000;
		mmRawMem = malloc((long) MM_RAM_REQ * 1000);
		if (mmRawMem == NULL) {
			sprintf(engine.system_error_message1, "MM_Init function error");
			sprintf(engine.system_error_message2, "Impossible allocate %u kb of memory \n", MM_RAM_REQ);
			sprintf(engine.system_error_message3, "");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_MEMORY);
		}
		mmChunksUsed = 0;
		mmMemUsed = 0;
	}

	MM_Initialized = true;
}

/** MEM :: Memory manager shutdown
 *   - Free all allocated memory
 */
void MM_Shutdown(void) {
	if (MM_Initialized) {
		// Free memory
		free(mmRawMem);
	}
}

void *MM_PushChunk(word size, ChunkType type) {
	void *mem;

	// Check if there's enough conventional memory
	if (mmMemUsed + size > mmMemTotal) {
		Error("Not enougth conventional memory", "", "", ERROR_SYSTEM);
	}

	// Check if we can allocate another chunk
	if (mmChunksUsed >= MM_MAX_NUM_CHUNKS) {
		Error("No available conventional memory chunks", "", "", ERROR_SYSTEM);
	}

	// Everything seems to be in order, perform the allocation
	// Make a note of the newly allocated chunk's properties
	mmChunkSizes[mmChunksUsed] = size;
	mmChunkTypes[mmChunksUsed] = type;

	// Use current top of memory buffer to satisfy the request
	mem = MM_CURRENT_TOP_PTR();

	//mem = MK_FP(((mmConventionalMemUsed+size)>>4)&0xF0000,((mmConventionalMemUsed+size)>>4)&0xFFFF);
	//mmChunkPointers[mmChunksUsed] = mem;

	// Update how much memory/chunks remain available
	mmMemUsed += size;
	mmChunksUsed++;
	return mem;
}

/** Frees multiple chunks
 *
 * Frees all most recently allocated chunks _iff_ the given type matches.  Stops
 * as soon as a chunk of different type is encountered.  This means that if
 * several chunks of type A have been allocated followed by a chunk of type B
 * and then more chunks of type A, only the chunks following the B chunk will be
 * freed.
 *
 * If the chunk type is CT_SPRITE, the corresponding entries in the loaded
 * sprite table will be reset to NULL. No special handling for CT_INGAME_MUSIC,
 * unlike MM_PopChunk.
 *
 * Example:
 *
 *   MM_PushChunkConventional(5, CT_TEMPORARY);    // not freed by call below
 *   MM_PushChunkConventional(5, CT_INGAME_MUSIC); // not freed
 *   MM_PushChunkConventional(5, CT_TEMPORARY);    // freed
 *   MM_PushChunkConventional(5, CT_TEMPORARY);    // freed
 *
 *   MM_PopChunksConventional(CT_TEMPORARY);
 */
void MM_PopChunks(ChunkType type) {
	while (mmChunksUsed != 0 && mmChunkTypes[mmChunksUsed - 1] == type) {
		mmChunksUsed--;
		mmMemUsed -= mmChunkSizes[mmChunksUsed];

		if (type == CT_SPRITE) {
			// [PERF] UpdateSpriteDataList internally does a linear search for the
			// address, making this an O(N^2) algorithm.  This isn't so easy to fix
			// though, because the memory manager doesn't know which sprite data index
			// is used for which address. And any extra bookkeeping data would require
			// more memory, which is in short supply. The game doesn't unload any
			// sprites during gameplay, so there's no performance impact on the game
			// itself and this is probably not a bad design overall, since it's so
			// simple.
			// dbg_jmf UpdateSpriteDataList(CURRENT_MEM_TOP_PTR());
		}
	}
}
