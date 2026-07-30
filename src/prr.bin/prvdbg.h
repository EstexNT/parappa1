#ifndef _PRVDBG_H_
#define _PRVDBG_H_

#include "common.h"

typedef struct DBG_INFO {
    u32 key;
    u32 unk4;
    s32 ncall;
    u32 unkC;
    s32 unk10;
    s32 paraface;
    s32 err;
    s32 nth;
    s32 nf;
    s32 fe;
    s32 unk28;
    s32 runframe;
    s32 vsyncdiffabs;
    s32 vsyncstart;
    s32 vsyncdiff;
    u8 unk3C[0x44 - 0x3C];
    char *fname;
    s32 pos;
    s32 end;
    u32 unk50;
    s32 unk54;
    s32 unk58;
    u32 unk5C;
    PARA_TIME unk60;
    s32 actscore;
    s32 origscore;
    s32 scorediff;
    s32 offpenalty;
    s32 startscore;
    u32 unk78;
    s32 dif;
    u8 unk80[8];
    s32 tapok;
    PARA_JP_TEXT *jptext;
    s32 origscoreused;
    u32 unk94;
    s16 origscorenum[4];
    u8 unkA0[0xc];
    s32 decsize_lvlscorediff; // TODO: union?
    s32 perfdiff;
    u8 unkB4[0xC4 - 0xB4];
    s32 coreinputs;
    u32 unkC8;
    s32 unkCC;
    u32 unkD0;
    s32 firstpressed;
    s32 tr;
    u8 unkDC[0xE8 - 0xDC];
    s32 teachertodid;
    s32 paratodid;
    s32 paradatid;
    s32 paravdfid;
    s32 teacherdatid;
    s32 teachervdfid;
    u8 unk100[0x150 - 0x100];
    s32 ntap;
    s32 tapon;
    s32 tapoff;
    s32 tdsmallest;
    s32 tdlargest;
    s32 tdiff;
    s32 strmdiff;
    s32 bezid;
    s32 workoffs;
    s32 workoffsmax;
    u32 unk178;
    s32 seccur;
    s32 secfirst;
    s32 seclast;
    u32 unk188;
    u8 *spptr;
    u32 unk190;
} DBG_INFO;

extern DBG_INFO dbgInfo;


void DbgInit(void);
void DbgDrawMenu(void);
void DbgDrawMenuOt(void);
void DbgReset(void);
void DbgResetRun(void);
void DbgPrintMovie(void);
void DbgPrintMsg(void);
void DbgDrawMenuText(char *text, s32 n);
void DbgError(char *msg);
void DbgWaitPad(void);
void DbgStackInit(u8 *ptr);
void DbgStackCheck(void);


#endif
