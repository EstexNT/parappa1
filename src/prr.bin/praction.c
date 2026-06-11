#include "praction.h"
#include "prevent.h"
#include "prvdbg.h"
// #include "prmemory.h"

void ActionBezInterpolate(s32 sf, MATRIX *refmtx, SVECTOR *ref, VECTOR *view);
void ActionSetTmdInfo(void *tmd, PARA_TMD_DATA *data);
void ActionSaveOriginalVtx(PARA_TMD_DATA *tmd, SVECTOR *orgvtx);
void ActionSetVdf(void *vdf, PARA_VDF_DATA *data);
void ActionResetMimeVtx(PARA_TMD_DATA *data);
void ActionSetMimeVtx(PARA_TMD_DATA *tmd, PARA_VDF_DATA *vdf);
s32 ActionCalcOrigScore(ON_INPUT_INFO *oninp, s32 start, s32 end);
u32 ActionRemapLR(u32 pad);
BOOL ActionCalcTapStats(s32 taptime, s32 window, s32 *nth);
BOOL ActionRegisterTap(SND_INFO *snd, SCENE_INFO *scn, s32 nth, BOOL on);
s32 ActionCalcScoreNormal(SCENE_INFO *scn);
s32 ActionCalcScoreHigh(SCENE_INFO *scn);
void ActionRecordTap(u32 pad, s32 nth, s32 time);
s32 ActionCalcActScore(ON_INPUT_INFO *oninp[], s32 type, u32 validpad);
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

s32 D_800826BC;
s32 D_800826C0;
s32 D_800826C4;
s32 D_800826C8;
s32 D_800826CC;
s32 D_800826D0;

s32 ActionFollowBez(s32 frame) {
    VECTOR view;
    s32 framediv;
    s32 curm;

    if (D_8008238C < 0) {
        // BUG: No return value
        return;
    }

    framediv = frame % D_80082390;
    if (framediv == 0) {
        if (((((D_80082388 + 2) / 3) - 2) > D_8008238C)) {
            if (frame != 0) {
                D_8008238C++;
            }
        } else {
            D_8008238C = -1;
            // BUG: No return value
            return;
        }
        curm = D_8008238C * 3;

        // Pos
        if (((D_80082858[curm].vx == D_80082858[curm + 3].vx) && (D_80082858[curm + 1].vx == 0) && (D_80082858[curm + 2].vx == 0)) && 
            ((D_80082858[curm].vy == D_80082858[curm + 3].vy) && (D_80082858[curm + 1].vy == 0) && (D_80082858[curm + 2].vy == 0)) &&
            ((D_80082858[curm].vz == D_80082858[curm + 3].vz) && (D_80082858[curm + 1].vz == 0) && (D_80082858[curm + 2].vz == 0))) {
            D_80082394 = 1;
        } else
        if (((D_80082858[curm + 1].vx == 0) && (D_80082858[curm + 1].vy == 0) && (D_80082858[curm + 1].vz == 0)) ||
            ((D_80082858[curm + 2].vx == 0) && (D_80082858[curm + 2].vy == 0) && (D_80082858[curm + 2].vz == 0))) {
            D_800826BC = (D_80082858[curm + 3].vx - D_80082858[curm].vx) / D_80082390;
            D_800826C0 = (D_80082858[curm + 3].vy - D_80082858[curm].vy) / D_80082390;
            D_800826C4 = (D_80082858[curm + 3].vz - D_80082858[curm].vz) / D_80082390;
            D_80082394 = 2;
        } else {
            D_80082818.m[0][0] = D_80082858[curm + 0].vx;
            D_80082818.m[0][1] = D_80082858[curm + 1].vx;
            D_80082818.m[0][2] = D_80082858[curm + 2].vx;
            D_80082818.m[1][0] = D_80082858[curm + 0].vy;
            D_80082818.m[1][1] = D_80082858[curm + 1].vy;
            D_80082818.m[1][2] = D_80082858[curm + 2].vy;
            D_80082818.m[2][0] = D_80082858[curm + 0].vz;
            D_80082818.m[2][1] = D_80082858[curm + 1].vz;
            D_80082818.m[2][2] = D_80082858[curm + 2].vz;
            D_80082394 = 0;
        }

        // Ref
        if (((D_80082BC8[curm].vx == D_80082BC8[curm + 3].vx) && (D_80082BC8[curm + 1].vx == 0) && (D_80082BC8[curm + 2].vx == 0)) && 
            ((D_80082BC8[curm].vy == D_80082BC8[curm + 3].vy) && (D_80082BC8[curm + 1].vy == 0) && (D_80082BC8[curm + 2].vy == 0)) &&
            ((D_80082BC8[curm].vz == D_80082BC8[curm + 3].vz) && (D_80082BC8[curm + 1].vz == 0) && (D_80082BC8[curm + 2].vz == 0))) {
            D_80082398 = 1;
        } else
        if (((D_80082BC8[curm + 1].vx == 0) && (D_80082BC8[curm + 1].vy == 0) && (D_80082BC8[curm + 1].vz == 0)) ||
            ((D_80082BC8[curm + 2].vx == 0) && (D_80082BC8[curm + 2].vy == 0) && (D_80082BC8[curm + 2].vz == 0))) {
            D_800826C8 = (D_80082BC8[curm + 3].vx - D_80082BC8[curm].vx) / D_80082390;
            D_800826CC = (D_80082BC8[curm + 3].vy - D_80082BC8[curm].vy) / D_80082390;
            D_800826D0 = (D_80082BC8[curm + 3].vz - D_80082BC8[curm].vz) / D_80082390;
            D_80082398 = 2;
        } else {
            D_80082838.m[0][0] = D_80082BC8[curm + 0].vx;
            D_80082838.m[0][1] = D_80082BC8[curm + 1].vx;
            D_80082838.m[0][2] = D_80082BC8[curm + 2].vx;
            D_80082838.m[1][0] = D_80082BC8[curm + 0].vy;
            D_80082838.m[1][1] = D_80082BC8[curm + 1].vy;
            D_80082838.m[1][2] = D_80082BC8[curm + 2].vy;
            D_80082838.m[2][0] = D_80082BC8[curm + 0].vz;
            D_80082838.m[2][1] = D_80082BC8[curm + 1].vz;
            D_80082838.m[2][2] = D_80082BC8[curm + 2].vz;
            D_80082398 = 0;
        }
    }
    // e48
    curm = D_8008238C * 3;

    // Pos
    if (D_80082394 == 1) {
        D_800A3620.vpx = D_80082858[curm].vx;
        D_800A3620.vpy = D_80082858[curm].vy;
        D_800A3620.vpz = D_80082858[curm].vz;
    } else if (D_80082394 == 2) {
        D_800A3620.vpx = D_80082858[curm].vx + D_800826BC * framediv;
        D_800A3620.vpy = D_80082858[curm].vy + D_800826C0 * framediv;
        D_800A3620.vpz = D_80082858[curm].vz + D_800826C4 * framediv;
    } else {
        ActionBezInterpolate(framediv, &D_80082818, &D_80082858[curm + 3], &view);
        D_800A3620.vpx = view.vx;
        D_800A3620.vpy = view.vy;
        D_800A3620.vpz = view.vz;
    }

    // Ref
    if (D_80082398 == 1) {
        D_800A3620.vrx = D_80082BC8[curm].vx;
        D_800A3620.vry = D_80082BC8[curm].vy;
        D_800A3620.vrz = D_80082BC8[curm].vz;
    } else if (D_80082398 == 2) {
        D_800A3620.vrx = D_80082BC8[curm].vx + D_800826C8 * framediv;
        D_800A3620.vry = D_80082BC8[curm].vy + D_800826CC * framediv;
        D_800A3620.vrz = D_80082BC8[curm].vz + D_800826D0 * framediv;
    } else {
        ActionBezInterpolate(framediv, &D_80082838, &D_80082BC8[curm + 3], &view);
        D_800A3620.vrx = view.vx;
        D_800A3620.vry = view.vy;
        D_800A3620.vrz = view.vz;
    }

    GsSetRefView2(&D_800A3620);
    return D_8008238C;
}
