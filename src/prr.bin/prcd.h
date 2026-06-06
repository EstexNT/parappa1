#ifndef _PRCD_H_
#define _PRCD_H_

#include "common.h"
#include <libcd.h>


typedef struct CD_FILE {
    char *fname;
    s16 chan;
    s16 vol;
    s32 search;
    CdlFILE cdl;
    s32 pos;
    s32 end;
} CD_FILE;

typedef struct CD_INFO {
    s32 unk0;
    CdlLOC strmloc;
    s32 unk8;
    CdlLOC curloc;
    s32 unk10;
    s32 strmsec;
    s32 unk18;
    s32 cursec;
    s32 relsec;
    CdlFILTER strmfilter;
    s32 unk28;
    u8 result[8];
    s32 unk34;
} CD_INFO;

typedef struct UNK_DATA {
    s32 seq;
    s32 size;
    s32 nelm;
    u8 cp[1]; // u8 cp[size];
} UNK_DATA;


extern CD_INFO *cdInfo;


void CDInit(void);
void func_8001d590(void);
void CDSetSyncCallback(void);
CdlFILE *CDSearch(CdlFILE *file, char *fname);
s32 CDFileSearch(CD_FILE *cdf);
BOOL CDGetCurLoc(void);
void CDSetVol(s32 vol);
void CDToggleVol(BOOL disable);
void CDStartStream(CD_FILE *cdf, BOOL fast, BOOL arg2);
void CDSetFilter(s32 chan);
u8 CDGetFilterChan(void);
void CDStop(void);
void CDPause(void);
s32 CDGetCurrentSector(s32 pos);
s32 CDGetCurrentStreamSector(s32 pos);
BOOL CDFileEnded(CD_FILE *cdf);
void func_8001e0ec(UNK_DATA *ptr, char *name);
BOOL CDFileRead(CD_FILE *cdf, BOOL snd);
BOOL CDFileReadBuf(CD_FILE *cdf, u8 *buf);

#endif
