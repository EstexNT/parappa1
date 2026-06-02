#include <stdio.h>
#include <string.h>
#include "prrap.h"

static char rcsid[] = "@(#)prrap.c: version 01-00 95/10/10 00:00:00";

s16 rapVabId = 0;
s16 rapInterVoiceNum = -1;
s16 rapVoiceNum = -1;
s32 rapNoFlush = 0;
s32 rapFrameActiveFrames = 60;
SND_INFO rapSndInfoFrameCreate = {
    0, 12, 36, 80,
    0
};
s32 rapFrameActiveTimer = 0;

extern SND_INFO rapInterKey;


void RapInit(void) {
    SsInit();
    SsSetTickMode(SS_NOTICK);
    SsStart();
    SsSetMVol(90, 90);
    SsSetSerialAttr(SS_SERIAL_A, SS_MIX, SS_SON);
    SsSetSerialVol(SS_SERIAL_A, 127, 127);
}

void RapFlush(void) {
    if (!rapNoFlush) {
        SsUtFlush();
    }
}

void RapPlayKey(SND_INFO *key) {
    key->note = key->tone + 24;
    rapVoiceNum = SsUtKeyOn(rapVabId, key->prog, key->tone, key->note, 0, key->vol, key->vol);
}

void func_80035464(SND_INFO *key) {
    rapVoiceNum = SsUtKeyOn(rapVabId, key->prog, key->tone, key->note, 0, key->vol, key->vol);
}

void RapStopSeq(void) {
    SsUtAllKeyOff(0);
}

void RapPlayInterruptableKey(SND_INFO *key) {
    if (key == NULL) {
        return;
    }
    SsUtKeyOff(rapInterVoiceNum, rapVabId, rapInterKey.prog, rapInterKey.tone, rapInterKey.note);
    key->note = key->tone + 24;
    rapInterVoiceNum = SsUtKeyOn(rapVabId, key->prog, key->tone, key->note, 0, key->vol, key->vol);
    bcopy(key, &rapInterKey, sizeof(SND_INFO));
}

// TODO: dbgInfo
extern s32 D_80068C24;

s32 RapOpenHead(void *ptr) {
    rapVabId = SsVabOpenHead(ptr, -1);
    if (rapVabId < 0) {
        D_80068C24 = 0x3333;
        return 0;
    } else {
        return 1;
    }
}

void RapTransferPartly(void *ptr, u32 size) {
    s32 ret;
    char tmp[32];
    
    ret = SsVabTransBodyPartly(ptr, size, rapVabId);
    if (ret != rapVabId) {
        sprintf(tmp, "TransferPartly: %d\n", ret);
        // TODO: DbgError
        func_800379A4(tmp);
    }
}

void RapTransferAll(void *ptr) {
    s32 ret;
    char tmp[32];
    
    ret = SsVabTransBody(ptr, rapVabId);
    if (ret < 0) {
        sprintf(tmp, "TransferAll: %d\n", ret);
        // TODO: DbgError
        func_800379A4(tmp);
    }
}

void RapTransferCompleted(s32 wait) {
    SsVabTransCompleted(wait == SS_WAIT_COMPLETED ? SS_WAIT_COMPLETED : SS_IMMEDIATE);
}

void RapCloseHead(void) {
    if (rapVabId >= 0) {
        SsVabClose(rapVabId);
        rapVabId = -1;
    }
}

void RapReset(void) {
    RapCloseHead();
    RapInit();
}

// Play the sound heard during transitions - background tiles
void RapPlayFrameActive(s32 freq) {
    if (freq == 0) {
        freq = 10;
    }
    
    if ((rapFrameActiveFrames / freq) <= rapFrameActiveTimer) {
        RapPlayKey(&rapSndInfoFrameCreate);
        RapFlush();
        rapFrameActiveTimer = 0;
        // return;
    }
    rapFrameActiveTimer++;
}

static SND_INFO D_80067D2C[2] = {
    { 
      0, 10, 34, 90,
      0
    },
    {
      0, 11, 35, 90,
      0
    }
};

// Play the sound heard during transitions - movie gui
// 0 = text box
// 1 = screen
void RapPlayFrameMovieGuiActive(s32 snd) {
    RapPlayKey(D_80067D2C + snd);
    RapFlush();
}
