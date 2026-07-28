#ifndef _PRINFO_H_
#define _PRINFO_H_

#include "common.h"

// TODO: Maybe move more structs over here

// Recorded input data
typedef struct RECORD_DATA {
    s32 time;
    u32 pad;
} RECORD_DATA;

typedef struct SAVE_DATA {
    u8 unk0;
    char name[8];
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 stageclear[6];
    s32 stagescore[6];
    s32 stagecur;
    s32 stageprevclear;
    s32 allhigh;
    s32 recorddatanum;
    u8 unk3C[0x4C - 0x3c];
    RECORD_DATA recorddata[600];
} SAVE_DATA;

typedef struct HIGHSCORE_INFO {
    char name[12];
    s32 score;
} HIGHSCORE_INFO;

typedef struct CARD_INFO {
    SAVE_DATA save;
    HIGHSCORE_INFO highscore[6][4];
} CARD_INFO;


#endif
