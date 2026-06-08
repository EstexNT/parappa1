#include "praction.h"
#include "prevent.h"
#include "prvdbg.h"
// #include "prmemory.h"

void ActionBezInterpolate(s32 sf, MATRIX *refmtx, SVECTOR *ref, VECTOR *view);
void ActionSetTmdInfo(void *tmd, PARA_TMD_DATA *data);
void ActionSaveOriginalVtx(PARA_TMD_DATA *tmd, SVECTOR *orgvtx);
void ActionSetVdf(void *vdf, PARA_VDF_DATA *data);
void ActionResetMimeVtx(PARA_TMD_DATA *data);
void ActionSetMimeVtx(PARA_TMD_DATA *tmd, PARA_VDF_DATA *mdf);
s32 ActionCalcOrigScore(ON_INPUT_INFO *oninp, s32 start, s32 end);
u32 ActionRemapLR(u32 pad);
BOOL ActionCalcTapStats(s32 taptime, s32 window, s32 *nth);
BOOL ActionRegisterTap(SND_INFO *snd, SCENE_INFO *scn, s32 nth, BOOL on);
s32 ActionCalcScoreNormal(SCENE_INFO *scn);
s32 ActionCalcScoreHigh(SCENE_INFO *scn);
void ActionRecordTap(u32 pad, s32 nth, s32 time);
s32 ActionCalcActScore(ON_INPUT_INFO *oninp[], s32 type, u32 validPad);
void ActionGetOnInput(ON_INPUT_INFO *oninp[], s32 type, u32 ncall);


extern SVECTOR *D_800826D4;

extern MATRIX D_80082818;
extern MATRIX D_80082838;
extern SVECTOR D_80082858[110];
extern SVECTOR D_80082BC8[110];
extern s32 D_80082F38[110];
extern PARA_TMD_OBJECT D_800830F0[10][4];
extern PARA_VDF_OBJECT D_80083550[10][128];
extern PARA_TMD_DATA D_80088550[10];
extern PARA_VDF_DATA D_800885C8[10];


void ActionInitView(SVECTOR *sf) {
    D_800826D4 = sf;
    GsInitCoordinate2(NULL, D_800A3620.super);
}

static u32 D_80082388 = 0;
static s32 D_8008238C = 0;
static s32 D_80082390 = 11;

void ActionSetBez(u32 *bezdata, s32 divisor) {
    s32 i;
    s16 *p;
    s32 *data;

    D_80082390 = divisor;
    D_8008238C = 0;

    data = bezdata;
    D_80082388 = *data++;
    for (i = 0; i < D_80082388; i++) {
        p = (s16 *)data;
        D_80082858[i].vx = p[0];
        D_80082858[i].vy = p[1];
        D_80082858[i].vz = p[2];
        D_80082BC8[i].vx = p[3];
        D_80082BC8[i].vy = p[4];
        D_80082BC8[i].vz = p[5];
        data = (u8 *)data + sizeof(s16) * 6;
        D_80082F38[i] = *data++;
    }
}

void ActionStartBez(void) {
    D_800A3620.vpx = D_80082858[0].vx;
    D_800A3620.vpy = D_80082858[0].vy;
    D_800A3620.vpz = D_80082858[0].vz;
    D_800A3620.vrx = D_80082BC8[0].vx;
    D_800A3620.vry = D_80082BC8[0].vy;
    D_800A3620.vrz = D_80082BC8[0].vz;
    GsSetRefView2(&D_800A3620);
}

static s32 D_80082394 = 0;
static s32 D_80082398 = 0;

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction", ActionFollowBez);
