#include "praction.h"
#include "prevent.h"
#include "prvdbg.h"
// #include "prmemory.h"

static char rcsid[] = "@(#)praction.c: version 01-00 95/10/10 00:00:00";

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


extern SVECTOR *actionViewSubframe;

extern MATRIX actionBezPosMtx;
extern MATRIX actionBezRefMtx;
extern SVECTOR actionBezPos[110];
extern SVECTOR actionBezRef[110];
extern s32 actionBezInt[110];
extern PARA_TMD_OBJECT actionTmdObj[10][4];
extern PARA_VDF_OBJECT actionVdfObj[10][128];
extern PARA_TMD_DATA actionTmdData[10];
extern PARA_VDF_DATA actionVdfData[10];


void ActionInitView(SVECTOR *sf) {
    actionViewSubframe = sf;
    GsInitCoordinate2(NULL, actionView.super);
}

static u32 actionBezLen = 0;
static s32 actionBezCur = 0;
static s32 actionBezDiv = 11;

void ActionSetBez(u32 *bezdata, s32 divisor) {
    s32 i;
    s16 *p;
    s32 *data;

    actionBezDiv = divisor;
    actionBezCur = 0;

    data = bezdata;
    actionBezLen = *data++;
    for (i = 0; i < actionBezLen; i++) {
        p = (s16 *)data;
        actionBezPos[i].vx = p[0];
        actionBezPos[i].vy = p[1];
        actionBezPos[i].vz = p[2];
        actionBezRef[i].vx = p[3];
        actionBezRef[i].vy = p[4];
        actionBezRef[i].vz = p[5];
        data = (u8 *)data + sizeof(s16) * 6;
        actionBezInt[i] = *data++;
    }
}

void ActionStartBez(void) {
    actionView.vpx = actionBezPos[0].vx;
    actionView.vpy = actionBezPos[0].vy;
    actionView.vpz = actionBezPos[0].vz;
    actionView.vrx = actionBezRef[0].vx;
    actionView.vry = actionBezRef[0].vy;
    actionView.vrz = actionBezRef[0].vz;
    GsSetRefView2(&actionView);
}

static s32 actionBezPosState = 0;
static s32 actionBezRefState = 0;

s32 actionBezAddPx;
s32 actionBezAddPy;
s32 actionBezAddPz;
s32 actionBezAddRx;
s32 actionBezAddRy;
s32 actionBezAddRz;

s32 ActionFollowBez(s32 frame) {
    VECTOR view;
    s32 framediv;
    s32 curm;

    if (actionBezCur < 0) {
        // BUG: No return value
        return;
    }

    framediv = frame % actionBezDiv;
    if (framediv == 0) {
        if (((((actionBezLen + 2) / 3) - 2) > actionBezCur)) {
            if (frame != 0) {
                actionBezCur++;
            }
        } else {
            actionBezCur = -1;
            // BUG: No return value
            return;
        }
        curm = actionBezCur * 3;

        // Pos
        if (((actionBezPos[curm].vx == actionBezPos[curm + 3].vx) && (actionBezPos[curm + 1].vx == 0) && (actionBezPos[curm + 2].vx == 0)) && 
            ((actionBezPos[curm].vy == actionBezPos[curm + 3].vy) && (actionBezPos[curm + 1].vy == 0) && (actionBezPos[curm + 2].vy == 0)) &&
            ((actionBezPos[curm].vz == actionBezPos[curm + 3].vz) && (actionBezPos[curm + 1].vz == 0) && (actionBezPos[curm + 2].vz == 0))) {
            actionBezPosState = 1;
        } else
        if (((actionBezPos[curm + 1].vx == 0) && (actionBezPos[curm + 1].vy == 0) && (actionBezPos[curm + 1].vz == 0)) ||
            ((actionBezPos[curm + 2].vx == 0) && (actionBezPos[curm + 2].vy == 0) && (actionBezPos[curm + 2].vz == 0))) {
            actionBezAddPx = (actionBezPos[curm + 3].vx - actionBezPos[curm].vx) / actionBezDiv;
            actionBezAddPy = (actionBezPos[curm + 3].vy - actionBezPos[curm].vy) / actionBezDiv;
            actionBezAddPz = (actionBezPos[curm + 3].vz - actionBezPos[curm].vz) / actionBezDiv;
            actionBezPosState = 2;
        } else {
            actionBezPosMtx.m[0][0] = actionBezPos[curm + 0].vx;
            actionBezPosMtx.m[0][1] = actionBezPos[curm + 1].vx;
            actionBezPosMtx.m[0][2] = actionBezPos[curm + 2].vx;
            actionBezPosMtx.m[1][0] = actionBezPos[curm + 0].vy;
            actionBezPosMtx.m[1][1] = actionBezPos[curm + 1].vy;
            actionBezPosMtx.m[1][2] = actionBezPos[curm + 2].vy;
            actionBezPosMtx.m[2][0] = actionBezPos[curm + 0].vz;
            actionBezPosMtx.m[2][1] = actionBezPos[curm + 1].vz;
            actionBezPosMtx.m[2][2] = actionBezPos[curm + 2].vz;
            actionBezPosState = 0;
        }

        // Ref
        if (((actionBezRef[curm].vx == actionBezRef[curm + 3].vx) && (actionBezRef[curm + 1].vx == 0) && (actionBezRef[curm + 2].vx == 0)) && 
            ((actionBezRef[curm].vy == actionBezRef[curm + 3].vy) && (actionBezRef[curm + 1].vy == 0) && (actionBezRef[curm + 2].vy == 0)) &&
            ((actionBezRef[curm].vz == actionBezRef[curm + 3].vz) && (actionBezRef[curm + 1].vz == 0) && (actionBezRef[curm + 2].vz == 0))) {
            actionBezRefState = 1;
        } else
        if (((actionBezRef[curm + 1].vx == 0) && (actionBezRef[curm + 1].vy == 0) && (actionBezRef[curm + 1].vz == 0)) ||
            ((actionBezRef[curm + 2].vx == 0) && (actionBezRef[curm + 2].vy == 0) && (actionBezRef[curm + 2].vz == 0))) {
            actionBezAddRx = (actionBezRef[curm + 3].vx - actionBezRef[curm].vx) / actionBezDiv;
            actionBezAddRy = (actionBezRef[curm + 3].vy - actionBezRef[curm].vy) / actionBezDiv;
            actionBezAddRz = (actionBezRef[curm + 3].vz - actionBezRef[curm].vz) / actionBezDiv;
            actionBezRefState = 2;
        } else {
            actionBezRefMtx.m[0][0] = actionBezRef[curm + 0].vx;
            actionBezRefMtx.m[0][1] = actionBezRef[curm + 1].vx;
            actionBezRefMtx.m[0][2] = actionBezRef[curm + 2].vx;
            actionBezRefMtx.m[1][0] = actionBezRef[curm + 0].vy;
            actionBezRefMtx.m[1][1] = actionBezRef[curm + 1].vy;
            actionBezRefMtx.m[1][2] = actionBezRef[curm + 2].vy;
            actionBezRefMtx.m[2][0] = actionBezRef[curm + 0].vz;
            actionBezRefMtx.m[2][1] = actionBezRef[curm + 1].vz;
            actionBezRefMtx.m[2][2] = actionBezRef[curm + 2].vz;
            actionBezRefState = 0;
        }
    }
    // e48
    curm = actionBezCur * 3;

    // Pos
    if (actionBezPosState == 1) {
        actionView.vpx = actionBezPos[curm].vx;
        actionView.vpy = actionBezPos[curm].vy;
        actionView.vpz = actionBezPos[curm].vz;
    } else if (actionBezPosState == 2) {
        actionView.vpx = actionBezPos[curm].vx + actionBezAddPx * framediv;
        actionView.vpy = actionBezPos[curm].vy + actionBezAddPy * framediv;
        actionView.vpz = actionBezPos[curm].vz + actionBezAddPz * framediv;
    } else {
        ActionBezInterpolate(framediv, &actionBezPosMtx, &actionBezPos[curm + 3], &view);
        actionView.vpx = view.vx;
        actionView.vpy = view.vy;
        actionView.vpz = view.vz;
    }

    // Ref
    if (actionBezRefState == 1) {
        actionView.vrx = actionBezRef[curm].vx;
        actionView.vry = actionBezRef[curm].vy;
        actionView.vrz = actionBezRef[curm].vz;
    } else if (actionBezRefState == 2) {
        actionView.vrx = actionBezRef[curm].vx + actionBezAddRx * framediv;
        actionView.vry = actionBezRef[curm].vy + actionBezAddRy * framediv;
        actionView.vrz = actionBezRef[curm].vz + actionBezAddRz * framediv;
    } else {
        ActionBezInterpolate(framediv, &actionBezRefMtx, &actionBezRef[curm + 3], &view);
        actionView.vrx = view.vx;
        actionView.vry = view.vy;
        actionView.vrz = view.vz;
    }

    GsSetRefView2(&actionView);
    return actionBezCur;
}
