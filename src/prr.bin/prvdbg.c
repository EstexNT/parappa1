#include "prvdbg.h"
#include <stdio.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <kernel.h>
#include "prmemory.h"
#include "prrap.h"
#include "prstrm.h"
#include "prevent.h"
#include "prscene.h"
#include "praction.h"

// DBG_INFO dbgInfo = {0};


extern u8 *dbgStackPtr;


void DbgPrintMsgInfo(void);
void DbgDrawMenuTextWait(s32 n);
void DbgPrintMsgDraw(void);
void DbgPrintMsgEval(void);
void DbgPrintMsgApp(void);
void DbgPrintMsgDrawFace(void);
void DbgPrintOnInput(void);
void DbgPrintWork(void);


// TODO: .rodata
extern char D_80010CE4[]; // "Sctr:%6d END:%6d NCALL:%6d\n"
extern char D_80010D00[]; // "TC:%02d:%02d:%02d "
extern char D_80010D14[]; // " DC %6d "
extern char D_80010D20[]; // "nf %4d fe%4d\n"
extern char D_80010D30[]; // "SECTOR:%d/%d/%d "
extern char D_80010D44[]; // "PACKET:%d/%d"
extern char D_80010F74[]; // "\n\n\nError: "
extern char D_80010F80[]; // "\n\nHit Any Key To EXIT\n"
extern char D_80010DA0[]; // "P:%d/%d "
extern char D_80010DAC[]; // "ERROR:%x\n"
extern char D_80010DB8[]; //
extern char D_80010DC8[]; // 
extern char D_80010DD4[]; // 
extern char D_80010DF4[]; // 
extern char D_80010F24[]; // 
extern char D_80010F30[]; // 
extern char D_80010F40[]; //
extern char D_80010F54[]; // 
extern char D_80010F64[]; // 
extern char D_80010F98[]; // "Stack check %x\n"
extern char D_80010FA8[]; // "STACK CHECK ERROR %x\n"

// TODO: .sdata
extern char D_800825E8[]; // "%s\n"
extern char D_800825F4[]; // "SI:%d\n"
extern char D_800825FC[]; // "VD%4d\n"
extern char D_80082630[]; // 
extern char D_8008263C[]; // 
extern char D_80082640[]; // 
extern char D_80082664[]; // 
extern char D_8008266C[]; // 
extern char D_8008269C[]; // 
extern char D_800826A4[]; // 
extern char D_800826AC[]; // 


void DbgInit(void) {
    s32 id;
    
    FntLoad(960, 256);
    id = FntOpen(-156, -120, 320, 200, 0, 512);
    SetDumpFnt(id);
    SetDispMask(1);
}

void DbgDrawMenu(void) {
    func_8002EA80(&sceneInfo, NULL); // TODO: CompoDrawMenu
}

void DbgDrawMenuOt(void) {
    VSync(0);
    func_8002EE60(0, NULL); // TODO: CompoDrawMenuOt
}

void DbgReset(void) {
    MemoryZero(&dbgInfo, sizeof(dbgInfo));
    dbgInfo.unk54 = 0xffff;
    dbgInfo.tdlargest = 0;
    dbgInfo.tdsmallest = 100;
    dbgInfo.tdiff = -1;
    dbgInfo.spptr = (u8 *)-1;
}

void DbgResetRun(void) {
    dbgInfo.runframe = 0;
    dbgInfo.unk28 = 0;
    dbgInfo.unk54 = 0xffff;
    dbgInfo.unk58 = 0;
}

void DbgPrintMovie(void) {
    u32 unused;
    SCENE_INFO *info;

    info = &sceneInfo;
    
    dbgInfo.vsyncdiffabs = VSync(-1) - dbgInfo.vsyncstart;
    dbgInfo.vsyncdiff = dbgInfo.vsyncdiffabs - dbgInfo.runframe;

    FntPrint(D_80010CE4, CDGetCurrentSector(0), dbgInfo.end, dbgInfo.ncall);
    FntPrint(D_80010D00, info->gametimet.min, info->gametimet.sec, info->gametimet.frame);
    FntPrint(D_80010D14, dbgInfo.decsize_lvlscorediff);
    FntPrint(D_80010D20, dbgInfo.nf, dbgInfo.fe);
    FntPrint(D_800825F4, eventCurMovieSub);
    FntPrint(D_80010D30, dbgInfo.seccur, dbgInfo.secfirst, dbgInfo.seclast);
    FntPrint(D_800825FC, dbgInfo.vsyncdiff);
    FntPrint(D_80010D44, dbgInfo.workoffs, dbgInfo.workoffsmax);
}

// TODO: appInfo
extern s16 D_8009F832;

void DbgPrintMsgInfo(void) {
    u8 unused[0x10];
    MEM_INFO meminfo;
    SCENE_INFO *info;
    
    info = &sceneInfo;

    FntPrint(D_80010F24);
    FntPrint(D_8008269C, dbgInfo.vsyncdiff);
    if (dbgInfo.tdiff < 100) {
        FntPrint(D_80010F30, dbgInfo.tdiff, dbgInfo.tdlargest, dbgInfo.tdsmallest);
    }
    FntPrint(D_80010F40, info->gametimet.min, info->gametimet.sec, info->gametimet.frame);
    FntPrint(D_800826A4, dbgInfo.strmdiff);
    FntPrint(D_800826AC, info->level);
    MemoryGetInfo(&meminfo);
    FntPrint(D_80010F54, meminfo.left, meminfo.used, StrmGetSize());
    FntPrint(D_80010F64, eventInfo.controlstage, eventInfo.controldelay);
    switch (D_8009F832) {
        case 0: {
            DbgPrintMsgDraw();
            break;
        }
        case 1: {
            DbgPrintMsgEval();
            break;
        }
        case 2: {
            DbgPrintMsgApp();
            break;
        }
    }
}

void DbgPrintMsg(void) {
    SCENE_INFO *info;
    
    info = &sceneInfo;
    if (!info->dbgmsg) {
        return;
    }
    
    dbgInfo.vsyncdiffabs = VSync(-1) - dbgInfo.vsyncstart;
    dbgInfo.vsyncdiff = dbgInfo.vsyncdiffabs - dbgInfo.runframe;
    DbgPrintMsgInfo();
}

void DbgDrawMenuTextWait(s32 n) {
    s32 i;
    
    for (i = n * 15; i > 0; i--) {
        VSync(0);
    }
}

void DbgDrawMenuText(char *text, s32 n) {
    s32 i;
    
    for (i = 0; i < n; i++) {
        DbgDrawMenu();
        FntPrint(D_800825E8, text);
        DbgDrawMenuOt();
        DbgDrawMenuTextWait(1);
    }
}

void DbgError(char *msg) {
    DbgDrawMenu();
    FntPrint(D_80010F74);
    FntPrint(msg);
    FntPrint(D_80010F80);
    FntFlush(-1);
    DbgDrawMenuOt();

    while (PadRead(1));
    DbgWaitPad();
}

void DbgWaitPad(void) {
    while (PadRead(1));
    while (!PadRead(1));
}

void DbgStackInit(u8 *ptr) {
    s32 i;

    for (i = 64, dbgStackPtr = ptr; i > 0; i--) {
        *ptr++ = 0x69;
    }
}

void DbgStackCheck(void) {
    u8 *ptr;
    s32 i;
    s32 j;
    char tmp[32];

    for (i = 64, ptr = dbgStackPtr; i > 0; i--, ptr++) {
        if (*ptr != 0x69) {
            printf(D_80010F98, ptr);
            for (j = 200; j > 0; j--) {
                RapPlayKey(sceneInitInfo.dbgstackcheck);
                RapFlush();
                VSync(0);
            }
            sprintf(tmp, D_80010FA8, ptr);
            DbgError(tmp);
        }
    }
}

extern char *D_80068DC8[];

void DbgPrintMsgDraw(void) {
    SCENE_INFO *info;
    
    info = &sceneInfo;

    FntPrint(D_80082664, D_80068DC8[info->level]);
    FntPrint(D_80010DB8, info->unk74, info->unk76);
    FntPrint(D_8008266C, actionInfo.sub.vcount);
    FntPrint(D_80082630);
    FntPrint(D_80010DC8, dbgInfo.bezid);
    FntPrint(D_80082630);
    FntPrint(D_80010DD4, dbgInfo.paratodid, dbgInfo.paravdfid, dbgInfo.paradatid);
    FntPrint(D_80082630);
    FntPrint(D_80010DF4, dbgInfo.teachertodid, dbgInfo.teachervdfid, dbgInfo.teacherdatid);
    DbgPrintMsgDrawFace();
    FntPrint(D_80082630);
    DbgPrintWork();
    DbgPrintOnInput();
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/prvdbg", DbgPrintMsgEval);

void DbgPrintMsgApp(void) {
    
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/prvdbg", DbgPrintMsgDrawFace);

void DbgPrintOnInput(void) {
    s32 i;
    s32 j;

    FntPrint(D_80082630);

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 16; j++) {
            if (actionInfo.onlist[i][j].num != 0) {
                FntPrint(D_8008263C, actionInfo.onlist[i][j].num);
            } else {
                FntPrint(D_80082640, ((j % 4) == 0) ? '+' : '.');
            }
        }
        FntPrint(D_80082630);
    }

    dbgInfo.spptr = MIN(dbgInfo.spptr, GetSp());
}

void DbgPrintWork(void) {
    FntPrint(D_80010DA0, dbgInfo.workoffs, dbgInfo.workoffsmax);
    FntPrint(D_80010DAC, dbgInfo.err);
}
