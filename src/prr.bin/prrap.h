#ifndef _PRRAP_H_
#define _PRRAP_H_

#include <libsnd.h>
#include "common.h"

// Describes a sound inside a vab
typedef struct SND_INFO {
    u8 prog;
    u8 tone;
    u8 note;
    u8 vol;
    s16 time; // Doesn't do anything usually
    // Only used to describe:
    // - When should good->cool rank transition end (in game time)
    // - In practice, for how long to stall when playing Kat's voice lines (in frames)
} SND_INFO;


void RapInit(void);
void RapFlush(void);
void RapPlayKey(SND_INFO *key);
void func_80035464(SND_INFO *key);
void RapStopSeq(void);
void RapPlayInterruptableKey(SND_INFO *key);
s32 RapOpenHead(void *ptr);
void RapTransferPartly(void *ptr, u32 size);
void RapTransferAll(void *ptr);
void RapTransferCompleted(s32 wait);
void RapCloseHead(void);
void RapReset(void);
void RapPlayFrameActive(s32 freq);
void RapPlayFrameMovieGuiActive(s32 snd);

#endif
