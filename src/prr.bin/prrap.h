#ifndef _PRRAP_H_
#define _PRRAP_H_

#include "common.h"
#include <libsnd.h>


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
