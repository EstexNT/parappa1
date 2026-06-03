#ifndef _PRSTRM_H_
#define _PRSTRM_H_

#include "common.h"

#define STRM_TYPE_0 0
#define STRM_TYPE_1 1
#define STRM_TYPE_2 2
#define STRM_TYPE_NUM 3

typedef struct STRM_DEC_CTRL_REC {
    void *vlcbuf;
    void *imgbuf;
    s32 width;
    s32 height;
    s32 decresult;
    s32 x;
    s32 y;
    s32 frametaken;
    s32 running;
} STRM_DEC_CTRL_REC;


extern STRM_DEC_CTRL_REC *strmDecCtrlRec;
extern void *strmSectorBuffer;
extern void *strmVlcBuffer;
extern void *strmImgBuffer;
extern u32 strmCurFrame;

void func_800359d4(char *arg0, s32 arg1);
s32 StrmGetSize(void);
void StrmInit(s32 type);
void StrmKick(void);
void StrmMain(void);
void StrmDraw(void);
void StrmEnd(void);

#endif
