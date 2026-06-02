#include "prmemory.h"

static char rcsid[] = "@(#)prmemory.c: version 01-00 95/10/10 00:00:00";

#define MEMORY_DATA_SIZE 0x128000

u8 *memoryPosEndBase = NULL;
s32 memoryPosIdx = 0;

extern u8 *memoryPosBase;
extern u8 *memoryPosPtr;
extern u8 *memoryPosPtrEnd;

// u8 *memoryPos[MEMORY_POS_SIZE];

extern u8 memoryData[MEMORY_DATA_SIZE];

void MemoryInit(void) {
    memoryPosPtr = memoryPosBase = memoryData;
    memoryPosPtrEnd = &memoryPosBase[MEMORY_DATA_SIZE];
    memoryPosIdx = 0;
    memoryPos[memoryPosIdx] = memoryPosPtr;
    memoryPos[0] = NULL;
}

void MemoryClear(void) {
    s32 i;
  
    for (i = 0; i < MEMORY_POS_SIZE; i++) {
        memoryPos[i] = NULL;
    }
    MemoryInit();
}

#define MEMALIGNUP(n, al) ((((n) + ((al) - 1)) / (al)) * (al))

void *MemoryAlloc(s32 size) {
    u8 *ptr;
    void *ret;
    s32 alsize;

    if (memoryPosIdx >= MEMORY_POS_SIZE) {
        return NULL;
    }

    if ((size <= 0) || (size >= MEMORY_DATA_SIZE)) {
        return NULL;
    }

    alsize = MEMALIGNUP(size, 8);
    ptr = &memoryPosPtr[alsize];
    if (ptr >= memoryPosPtrEnd) {
        return NULL;
    }

    memoryPosIdx++;
    memoryPos[memoryPosIdx] = memoryPosPtr;
    ret = memoryPosPtr;
    memoryPosPtr = ptr;
    return ret;
}

void MemoryFree(void) {
    memoryPosPtr = memoryPos[memoryPosIdx];
    memoryPos[memoryPosIdx] = NULL;
    memoryPosIdx--;
}

void *MemoryEndAlloc(s32 size) {
    if (&memoryPosPtr[MEMALIGNUP(size, 8)] >= memoryPosPtrEnd) {
        return NULL;
    } else {
        memoryPosEndBase = (memoryPosEndBase < &memoryPosPtrEnd[-MEMALIGNUP(size, 8)]) ? &memoryPosPtrEnd[-MEMALIGNUP(size, 8)] : memoryPosEndBase;
        return &memoryPosPtrEnd[-MEMALIGNUP(size, 8)];
    }
}

void MemoryGetInfo(MEM_INFO *info) {
    info->base = memoryPosBase;
    info->ptrend = memoryPosPtrEnd;
    info->ptr = memoryPosPtr;
    info->endbase = memoryPosEndBase;
    info->left = memoryPosPtrEnd - memoryPosPtr;
    info->used = memoryPosPtr - memoryPosBase;
}

u32 MemorySetPos(u8 *ptr, u32 pos, s32 size) {
    u8 *ptrend = ptr + size;
    if (ptrend >= memoryPosPtrEnd) {
        return 0;
    }

    memoryPos[pos] = ptr;
    memoryPosIdx = pos;
    memoryPosPtr = ptrend;
    return 1;
}

u32 MemoryGetPos(u8 *ptr) {
    s32 i;

    for (i = 1; !(i > memoryPosIdx); i++) {
        if (memoryPos[i] == ptr) {
            return i;
        }
    }
    return 0;
}

void MemoryZero(u8 *ptr, s32 size) {
    for (size; size > 0; size--) {
        *ptr++ = 0;
    }
}
