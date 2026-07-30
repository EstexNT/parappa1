#ifndef _PRMEMORY_H_
#define _PRMEMORY_H_

#include "common.h"

#define MEMORY_POS_SIZE 0x200

typedef struct MEM_INFO {
    u8 *base;
    u8 *ptrend;
    u8 *ptr;
    u8 *endbase;
    s32 used; 
    s32 left;
} MEM_INFO;

extern s32 memoryPosIdx;
extern u8 *memoryPos[MEMORY_POS_SIZE];

void MemoryInit(void);
void MemoryClear(void);
void *MemoryAlloc(s32 size);
void MemoryFree(void);
void *MemoryEndAlloc(s32 size);
void MemoryGetInfo(MEM_INFO *info);
u32 MemorySetPos(u8 *ptr, u32 pos, s32 size);
u32 MemoryGetPos(u8 *ptr);
void MemoryZero(u8 *ptr, s32 size);

#endif
