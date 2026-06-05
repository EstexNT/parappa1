#include "prstrm.h"
#include <stdlib.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libpress.h>
#include <libcd.h>
#include "prmemory.h"

static char rcsid[] = "@(#)prstrm.c: version 01-00 95/10/10 00:00:00";


typedef struct STRM_INIT_INFO {
    s16 width;
    s16 height;
    s16 x;
    s16 y;
    s32 unk8;
    s32 vlcbufsize;
    s32 imgbufsize;
    s32 unk14;
} STRM_INIT_INFO;


#define STRM_BSIZE 0x10000
#define STRM_DSIZE sizeof(STRM_DEC_CTRL_REC)
#define STRM_ISIZE 0x4000
#define STRM_VSIZE 0x9000

STRM_INIT_INFO strmInitInfo[STRM_TYPE_NUM] = {
    { 256, 144, 35, 25,
    32, 0x10000, 0x24000, 0 },
    { 320, 240, 0, 0,
    32, 0x20000, 0x4B000, 0 },
    { 256, 144, 32, 48,
    32, 0x10000, 0x24000, 0 },
};

// From prstrm_1
void *StrmNext(void);


void func_800359d4(char *arg0, s32 arg1) {
    MEM_INFO info;

    MemoryGetInfo(&info);
    func_80037334(); // TODO: DbgDrawMenu
    FntPrint("\n\n\nBSIZ:%d DSIZ:%d\nISIZ:%d VSIZ:%d TOTAL:%dk\n\n", 
                STRM_BSIZE, STRM_DSIZE, STRM_ISIZE, STRM_VSIZE, 
                (STRM_BSIZE + STRM_DSIZE + STRM_ISIZE + STRM_VSIZE) / 1024);
    FntPrint(arg0);
    FntPrint("\nINDEX:%d  CRNT:%x\n\n", memoryPosIdx, info.ptr);
    FntPrint("LEFT:%d  ", info.left);
    if (arg1 > 0) {
        FntPrint("NECC:%d  SHORT:%d\n", arg1 / 1024, arg1 / 1024 - info.left / 1024);
    } else {
        FntPrint("\n");
    }
    FntFlush(-1);
    func_80037370(); // TODO: DbgDrawMenuOt
    func_80037A44(); // TODO: DbgWaitPad
    while (PadRead(1));
}

void StrmCallback() {
    strmDecCtrlRec->running = FALSE;
}

s32 StrmGetSize(void) {
    return STRM_BSIZE + STRM_DSIZE + STRM_ISIZE + STRM_VSIZE + 0x27000;
}

void *StrmAllocZero(s32 size, char *t) {
    u8 *ptr;

    if ((ptr = MemoryAlloc(size)) == NULL) {
        exit(1);
    }
    MemoryZero(ptr, size);
    return ptr;
}

void StrmInit(s32 type) {
    STRM_INIT_INFO *info = &strmInitInfo[type];

    strmDecCtrlRec = StrmAllocZero(STRM_DSIZE, "DecCtrlRec");
    strmSectorBuffer = StrmAllocZero(STRM_BSIZE, "S_SectorBuffer");
    strmVlcBuffer = StrmAllocZero(info->vlcbufsize, "S_VlcBuffer");
    strmImgBuffer = StrmAllocZero(info->imgbufsize, "S_ImgBuffer");

    DecDCTReset(0); // Cold reset MDEC
    StSetRing(strmSectorBuffer, 32);
    DecDCToutCallback(StrmCallback);

    strmDecCtrlRec->vlcbuf = strmVlcBuffer;
    strmDecCtrlRec->imgbuf = strmImgBuffer;
    // Width/height never set?????? 
    // TODO: Check other versions
    strmDecCtrlRec->width = 0;
    strmDecCtrlRec->height = 0;
    strmDecCtrlRec->decresult = 0;
    strmDecCtrlRec->frametaken = 0;
    strmDecCtrlRec->running = 0;
    strmDecCtrlRec->x = info->x;
    strmDecCtrlRec->y = info->y;
}

// TODO: dbgInfo
extern s32 D_80068C14;
extern s32 D_80068CB8;

void StrmNextVlc(void) {
    void *bs = StrmNext();

    if (bs == NULL) {
        strmDecCtrlRec->frametaken = FALSE;
        return;
    }

    strmDecCtrlRec->frametaken = TRUE;
    strmDecCtrlRec->decresult = DecDCTvlc(bs, strmDecCtrlRec->vlcbuf);
    D_80068CB8 = MAX(DecDCTBufSize(bs), D_80068CB8);
    StFreeRing(bs);

    if (strmDecCtrlRec->decresult == 0) {
        DecDCTin(strmDecCtrlRec->vlcbuf, 2); // 16-bit direct color, STP bit = 1
        DecDCTout(strmDecCtrlRec->imgbuf, (strmDecCtrlRec->width * strmDecCtrlRec->height) / 2);
        strmDecCtrlRec->running = TRUE;
    }
}

void StrmKick(void) {
    strmDecCtrlRec->decresult = 0;
    strmDecCtrlRec->frametaken = FALSE;
    strmDecCtrlRec->running = FALSE;
    StSetStream(0, 1, 0xffffffff, NULL, NULL); // 16-bit direct color, start frame = 1
    DecDCTvlcSize(0);
    StrmNextVlc();
    strmCurFrame = 0;
}

void StrmMain(void) {
    D_80068C14 = strmDecCtrlRec->running * 100 + strmDecCtrlRec->frametaken;
    if ((strmDecCtrlRec->running == FALSE) || (strmDecCtrlRec->frametaken == FALSE)) {
        StrmNextVlc();
    }
}

void StrmDraw(void) {
    RECT snaprect;
    s32 i;
    s32 bi;
    s32 buf;
    void *baseimgbuf;
    void *imgbuf;

    if (strmCurFrame <= 1) {
        return;
    }

    buf = GsGetActiveBuff();
    snaprect.y = strmDecCtrlRec->y + ((buf != 0) ? 240 : 0);
    snaprect.h = strmDecCtrlRec->height;
    snaprect.w = 16;

    baseimgbuf = strmDecCtrlRec->imgbuf;
    for (i = strmDecCtrlRec->width / 16, bi = 0; i > 0; i--, bi += 16) {
        snaprect.x = strmDecCtrlRec->x + bi;
        imgbuf = &((u8 *)baseimgbuf)[bi * strmDecCtrlRec->height * 2];
        LoadImage(&snaprect, imgbuf);
    }
}

void StrmEnd(void) {
    StClearRing();
    StUnSetRing();
    DecDCToutCallback(NULL);

    MemoryFree();
    MemoryFree();
    MemoryFree();
    MemoryFree();
}

void func_800362b4(void) {
    func_8001DD24(); // TODO: CDStop
}
