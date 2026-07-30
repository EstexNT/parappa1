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
// #include "prcompo.h"
#include "prapp.h"

static char rcsid[] = "@(#)prvdbg.c: version 01-00 95/10/10 00:00:00";

DBG_INFO dbgInfo = {0};

extern u8 *dbgStackPtr;


void DbgPrintMsgInfo(void);
void DbgDrawMenuTextWait(s32 n);
void DbgPrintMsgDraw(void);
void DbgPrintMsgEval(void);
void DbgPrintMsgApp(void);
void DbgPrintMsgDrawFace(void);
void DbgPrintOnInput(void);
void DbgPrintWork(void);


const char D_80010CD8[] = "STRING:\n";
const char D_80010CE4[] = "Sctr:%6d END:%6d NCALL:%6d\n";
const char D_80010D00[] = "TC:%02d:%02d:%02d ";
const char D_80010D14[] = " DC %6d ";
const char D_80010D20[] = "nf %4d fe%4d\n";
const char D_80010D30[] = "SECTOR:%d/%d/%d ";
const char D_80010D44[] = "PACKET:%d/%d";
const char D_80010D54[] = "ParaCurs:%2d  PNTH:%2d NUMEXLN:%d\n";
const char D_80010D78[] = "PARFACE:%3d: ";
const char D_80010D88[] = "TEAFACE: ";
const char D_80010D94[] = "MCFACE: ";
const char D_80010DA0[] = "P:%d/%d ";
const char D_80010DAC[] = "ERROR:%x\n";
const char D_80010DB8[] = "GO: %d SC: %d ";
const char D_80010DC8[] = "CAM:%3d\n";
const char D_80010DD4[] = "PARA: TOD:%3d VDF:%3d WAV:%3d\n";
const char D_80010DF4[] = "TCHR: TOD:%3d VDF:%3d WAV:%3d\n";
const char D_80010E14[] = "NTAP:%2d ON:%2d OFF:%2d ";
const char D_80010E30[] = "Key: %08x\n";
const char D_80010E3C[] = "PRS: %2d:%2d:%2d  ";
const char D_80010E50[] = "DIF:%3d ";
const char D_80010E5C[] = "EXCL: %d  \n";
const char D_80010E68[] = "1ST:%2d COR:%2d ACT:%3d ORG:%3d MNU:%3d\n";
const char D_80010E94[] = "PM:%3d DF:%d Tr:%d 1SP:%d ";
const char D_80010EB0[] = "\nKEY:%2d ";
const char D_80010EBC[] = "NTH: %2d ";
const char D_80010EC8[] = "Eval:%d Evnt:%d\n";
const char D_80010EDC[] = "  Ap.pm:%d  Ap.tm:%d";
const char D_80010EF4[] = "\nNFAL:%2d HEVL:%2d CSTT:%2d NEVL:%d ESTT:%d\n";
const char D_80010F24[] = "\n\n\n\n\n\n\n\n\n";
const char D_80010F30[] = "TD:%2d/%2d/%2d ";
const char D_80010F40[] = "MS:%02d:%02d:%02d ";
const char D_80010F54[] = "F:%d U:%d S:%d ";
const char D_80010F64[] = "ST:%2d DY:%2d\n";
const char D_80010F74[] = "\n\n\nError: ";
const char D_80010F80[] = "\n\nHit Any Key To EXIT\n";
const char D_80010F98[] = "Stack check %x\n";
const char D_80010FA8[] = "STACK CHECK ERROR %x\n";

const char D_800825E8[] = "%s\n";
const char D_800825EC[] = "NULL\n";
const char D_800825F4[] = "SI:%d\n";
const char D_800825FC[] = "VD%4d\n";
const char D_80082604[] = "  ";
const char D_80082608[] = "R ";
const char D_8008260C[] = "L ";
const char D_80082610[] = "# ";
const char D_80082614[] = "X ";
const char D_80082618[] = "O ";
const char D_8008261C[] = "A ";
const char D_80082620[] = "- ";
const char D_80082624[] = "BAR:";
const char D_8008262C[] = "*";
const char D_80082630[] = "\n";
const char D_80082634[] = "%3d ";
const char D_8008263C[] = "%2d";
const char D_80082640[] = " %c";
const char D_80082644[] = "LOW2";
const char D_8008264C[] = "LOW1";
const char D_80082654[] = "NORM";
const char D_8008265C[] = "HIGH";
const char D_80082664[] = "LVL:%s ";
const char D_8008266C[] = "VC:%d ";
const char D_80082674[] = "OK";
const char D_80082678[] = "--";
const char D_8008267C[] = "KCTRL:";
const char D_80082684[] = "%c";
const char D_80082688[] = "TAP:%s ";
const char D_80082690[] = "CLR: ";
const char D_80082698[] = "%d";
const char D_8008269C[] = "VD:%4d ";
const char D_800826A4[] = "SD:%d ";
const char D_800826AC[] = "%d\n";


void DbgInit(void) {
    s32 id;
    
    FntLoad(960, 256);
    id = FntOpen(-156, -120, 320, 200, 0, 512);
    SetDumpFnt(id);
    SetDispMask(1);
}

void DbgDrawMenu(void) {
    CompoDrawMenu(&sceneInfo, NULL);
}

void DbgDrawMenuOt(void) {
    VSync(0);
    CompoDrawMenuOt(0, NULL);
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
    switch (appInfo.printmode) {
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

char *D_80068DA0[] = {
    [PR_TAP_NONE] = D_80082620,
    [PR_TAP_TRIANGLE] = D_8008261C,
    [PR_TAP_CIRCLE] = D_80082618,
    [PR_TAP_CROSS] = D_80082614,
    [PR_TAP_SQUARE] = D_80082610,
    [PR_TAP_L1] = D_8008260C,
    [PR_TAP_L2] = D_8008260C,
    [PR_TAP_R1] = D_80082608,
    [PR_TAP_R2] = D_80082608,
    [PR_TAP_NUM] = D_80082604,
};
char *D_80068DC8[] = {
    [LEVEL_HIGH] = D_8008265C,
    [LEVEL_NORM] = D_80082654,
    [LEVEL_LOW1] = D_8008264C,
    [LEVEL_LOW2] = D_80082644,
};
char *D_80068DD8[] = {
    D_80082678, D_80082674,
    D_80082678, D_80082674,
};

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

void DbgPrintMsgEval(void) {
    SCENE_INFO *info;
    s32 i;
    u32 pad;
    u32 ctrlid;
    u32 keyctrl;
    u32 ctrlid1;
    s32 j;

    info = &sceneInfo;
    ctrlid = info->parappainp->sub[info->lvlhigh].ctrlid;
    FntPrint(D_8008267C);
    for (i = 0; i < 8; i++) {
        keyctrl = sceneInitInfo.keyctrl[ctrlid].ctrl[i];
        FntPrint(D_80082684, (keyctrl != 0) ? '1': '-');
    }
    FntPrint(D_80082630);
    FntPrint(D_80010E14, dbgInfo.ntap, dbgInfo.tapon, dbgInfo.tapoff);
    pad = info->pad;
    FntPrint(D_80010E30, pad);
    FntPrint(D_80082688, D_80068DD8[dbgInfo.tapok]);
    FntPrint(D_80010E3C, info->prs.min, info->prs.sec, info->prs.frame);
    FntPrint(D_80010E50, dbgInfo.dif);
    FntPrint(D_80010E5C, EventInputsInactive(info));
    FntPrint(D_80010E68, dbgInfo.firstpressed, dbgInfo.coreinputs, dbgInfo.actscore, dbgInfo.origscore, dbgInfo.offpenalty);
    FntPrint(D_80010E94, dbgInfo.scorediff, dbgInfo.decsize_lvlscorediff, dbgInfo.tr, dbgInfo.err);
    FntPrint(D_80010EB0, dbgInfo.key);
    ctrlid1 = info->parappainp->sub[info->lvlhigh].ctrlid;
    FntPrint(D_80010EBC, dbgInfo.nth);
    FntPrint(D_80010EC8, actionInfo.sub.highenabled, eventInfo.flashhigh);
    FntPrint(D_80082690);
    for (j = 0; j < 6; j++) {
        FntPrint(D_80082698, D_800A2174.save.stageclear[j]);
    }
    FntPrint(D_80010EDC, appInfo.playmode, appInfo.testmode);
    FntPrint(D_80010EF4, actionInfo.sub.highfailcount, actionInfo.sub.highended, eventInfo.coolstate, eventInfo.normaleval, eventInfo.lvlestimate);
    DbgPrintOnInput();
}

void DbgPrintMsgApp(void) {

}

void DbgPrintMsgDrawFace(void) {
    SCENE_INFO *info;
    u32 unused;
    s32 i;
    
    info = &sceneInfo;

    FntPrint(D_80010D78, dbgInfo.paraface);
    for (i = 0; info->parappaface[i] != 0; i++) {
        FntPrint(D_80082634, info->parappaface[i]);
    }
    FntPrint(D_80082630);

    FntPrint(D_80010D88);
    for (i = 0; info->teacherface[i] != 0; i++) {
        FntPrint(D_80082634, info->teacherface[i]);
    }
    FntPrint(D_80082630);
    
    FntPrint(D_80010D94);
    for (i = 0; info->mcface[i] != 0; i++) {
        FntPrint(D_80082634, info->mcface[i]);
    }
}

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
