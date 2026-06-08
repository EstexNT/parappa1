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

static s32 D_800826BC;
static s32 D_800826C0;
static s32 D_800826C4;
static s32 D_800826C8;
static s32 D_800826CC;
static s32 D_800826D0;

SVECTOR *D_800826D4;

extern MATRIX D_80082818;
extern MATRIX D_80082838;
extern SVECTOR D_80082858[110];
extern SVECTOR D_80082BC8[110];
extern s32 D_80082F38[110];
extern PARA_TMD_OBJECT D_800830F0[10][4];
extern PARA_VDF_OBJECT D_80083550[10][128];
extern PARA_TMD_DATA D_80088550[10];
extern PARA_VDF_DATA D_800885C8[10];


void ActionBezInterpolate(register s32 sf, register MATRIX *refmtx, register SVECTOR *ref, register VECTOR *view) {
    SVECTOR interp2;
    VECTOR interp1;
    MATRIX mtx;

    ApplyMatrix(refmtx, &D_800826D4[sf], &interp1);
    interp2.vx = ONE;
    interp2.vy = D_800826D4[sf].pad; // Hmm
    interp2.vz = 0;

    mtx.m[0][0] = interp1.vx;
    mtx.m[0][1] = ref->vx;
    mtx.m[0][2] = 0;
    mtx.m[1][0] = interp1.vy;
    mtx.m[1][1] = ref->vy;
    mtx.m[1][2] = 0;
    mtx.m[2][0] = interp1.vz;
    mtx.m[2][1] = ref->vz;
    mtx.m[2][2] = 0;
    ApplyMatrix(&mtx, &interp2, &interp1);
    view->vx = interp1.vx;
    view->vy = interp1.vy;
    view->vz = interp1.vz;
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionSetTmdInfo);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionSaveOriginalVtx);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionSetVdf);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionInitModel);

s32 ActionSetVdfData(s32 i, void *vdf) {
    if (vdf != NULL) {
        ActionSetVdf(vdf, &D_800885C8[i]);
    }
    return D_800885C8[i].objnum;
}

void ActionResetMimeVdf(s32 i) {
    ActionResetMimeVtx(&D_80088550[i]);
}

void ActionVtxMime(s32 i) {
    ActionResetMimeVtx(&D_80088550[i]);
    ActionSetMimeVtx(&D_80088550[i], &D_800885C8[i]);

}

void ActionSetVdfForTmd(s32 tmd, s32 vdf) {
    ActionSetMimeVtx(&D_80088550[tmd], &D_800885C8[vdf]);
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionResetMimeVtx);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionSetMimeVtx);


typedef ON_INPUT_INFO ON_INPUT_INFO_LIST[32];
static ON_INPUT_INFO_LIST *D_8008239C = actionInfo.onlist;
static BOOL D_800823A0 = FALSE;
BOOL D_800823A4 = FALSE;

void ActionInit(void) {
    s32 i;

    D_8008239C = actionInfo.onlist;
    for (i = 0; i < 4; i++) {
        MemoryZero(&actionInfo.onlist[i], sizeof(actionInfo.onlist[i]));
    }
    // BUG: Passing the whole struct instead of using the address
#if 1
    MemoryZero(actionInfo.sub, sizeof(actionInfo.sub));
#else
    MemoryZero(&actionInfo.sub, sizeof(actionInfo.sub));
#endif
    actionInfo.sub.lvlestscore = 0;
    actionInfo.sub.lvlestscoreprev = 0;
    actionInfo.sub.score = 0;
    actionInfo.sub.perfscoreprev = 0;
    ActionRestartTap(&sceneInfo);
    ActionResetPerfomanceScore();
    D_800823A0 = FALSE;
    actionInfo.sub.unk600 = 0;
}

void ActionSetHighEnabled(BOOL enabled) {
    actionInfo.sub.highenabled = enabled;
}

s32 ActionGetCurScore(SCENE_INFO *scn) {
    return actionInfo.sub.score;
}

void ActionResetStats(void) {
    actionInfo.sub.tapnonenum = 0;
    actionInfo.sub.lostscoreonedgeranks = FALSE;
    actionInfo.sub.tapoffnum = 0;
    actionInfo.sub.taponnum = 0;
    actionInfo.sub.tapnum = 0;
    actionInfo.sub.highended = FALSE;
    actionInfo.sub.highfailcount = 0;
    ActionResetPerfomanceScore();
}

void ActionUpdateNoTapNum(SCENE_INFO *scn) {
    if (actionInfo.sub.tapnum == 0) {
        actionInfo.sub.tapnonenum++;
    }
}

void ActionResetPerfomanceScore(void) {
    actionInfo.sub.lvlestscoreprev = actionInfo.sub.lvlestscore = actionInfo.sub.perfscoreprev = actionInfo.sub.score;
}

BOOL ActionCheckGameOver(SCENE_INFO *scn) {
    BOOL gameover;
    s32 level;

    gameover = FALSE;
    level = scn->level;
    if (level == LEVEL_LOW2) {
        if ((actionInfo.sub.tapnonenum >= 1) && (scn->penalty != FALSE)) {
            gameover = TRUE;
        } else {
            if ((actionInfo.sub.lostscoreonedgeranks == TRUE) && (scn->penalty != FALSE)) {
                actionInfo.sub.lostscoreonedgeranks = FALSE;
                gameover = TRUE;
            } else {
                gameover = FALSE;
            }
        }
    } else if (level == LEVEL_HIGH) {
        if ((actionInfo.sub.lostscoreonedgeranks == TRUE) && (scn->penalty != FALSE)) {
            actionInfo.sub.lostscoreonedgeranks = FALSE;
            gameover = TRUE;
        } else {
            gameover = FALSE;
        }
    } else {
        gameover = FALSE;
    }
    actionInfo.sub.lostscoreonedgeranks = FALSE;
    actionInfo.sub.tapnonenum = 0;
    return gameover;
}

BOOL ActionGetHighEnded(SCENE_INFO *scn) {
    return actionInfo.sub.highended;
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionCalcScoreLvlEstimate);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionUpdateInput);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionCalcOrigScore);

void ActionClearOnInputList(u32 i) {
    D_8008239C = actionInfo.onlist[i % 4];
    MemoryZero(D_8008239C, sizeof(*D_8008239C));
}

void ActionRestartTap(SCENE_INFO *scn) {
    actionInfo.sub.tapnum = 0;
    actionInfo.sub.taponnum = 0;
    actionInfo.sub.tapoffnum = 0;
    actionInfo.sub.addscore = 0;
    actionInfo.sub.tappadprev = actionInfo.sub.tappad;
    actionInfo.sub.tappad = 0;
}

void ActionAddScore(s32 score) {
    actionInfo.sub.addscore = score;
    actionInfo.sub.tapnum++;
}

s32 ActionCheckPerformanceHigh(SCENE_INFO *scn) {
    s32 diff;
    BOOL ch;

    diff = actionInfo.sub.score - actionInfo.sub.perfscoreprev;
    actionInfo.sub.perfscoreprev = actionInfo.sub.score;
    dbgInfo.perfdiff = diff;

    // Likely was supposed to return 2 (Dropping to awful) if this is TRUE
    ch = actionInfo.sub.tapoffnum > actionInfo.sub.taponnum;
    if (diff < 66) {
        // Player not keeping up
        actionInfo.sub.highfailcount++;
        if (actionInfo.sub.highfailcount >= 2) {
            // Second chance lost
            actionInfo.sub.highended = TRUE;
        }
        return 1;
    } else if (actionInfo.sub.highfailcount >= 2) {
        actionInfo.sub.highended = TRUE;
        return 1;
    } else {
        // Player is keeping up
        actionInfo.sub.highended = FALSE;
        actionInfo.sub.highfailcount = 0;
        return 0;
    }
}

s32 ActionCheckPerformanceNormal(SCENE_INFO *scn) {
    s32 diff;

    diff = actionInfo.sub.score - actionInfo.sub.perfscoreprev;
    actionInfo.sub.perfscoreprev = actionInfo.sub.score;
    dbgInfo.perfdiff = diff;
    return (diff > 0) ? PR_DIR_UP : PR_DIR_DOWN;
}

void ActionCalcScore(SCENE_INFO *scn) {
    s32 diff;

    if (actionInfo.sub.tapnum == 0) {
        diff = -1;
    } else if (scn->lvlhigh == FALSE) {
        diff = ActionCalcScoreNormal(scn);
    } else {
        diff = ActionCalcScoreHigh(scn);
    }

    diff += actionInfo.sub.addscore;
    actionInfo.sub.score += diff;
    if (actionInfo.sub.score < 0) {
        actionInfo.sub.score = 0;
    }
    dbgInfo.unkCC = actionInfo.sub.score;
    dbgInfo.scorediff = diff;
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionResetKeyInfo);
#else
void ActionResetKeyInfo(SCENE_INFO *scn) {
    s32 i;
    s32 j;
    INPUT_KEY_INFO *keyinfo;

    for (i = 0; i < sceneInitInfo.keyinfolen; i++) {
        for (j = 0; j < 9; j++) {
            // NON MATCHING:
            // Weird indexing for .keyinfo
            if ((keyinfo = sceneInitInfo.keyinfo[i][j]) != NULL) {
                keyinfo->mapnum = 0;
            }
        }
    }
}
#endif

void ActionUpdateVcount(SCENE_INFO *scn, s32 vcount) {
    if (actionInfo.sub.vcount > 0) {
        actionInfo.sub.vcount -= vcount;
    }
}

u32 ActionRemapLR(register u32 pad) {
    if (pad == PR_PAD_L2) {
        return PR_PAD_L1;
    } else if (pad == PR_PAD_R2) {
        return PR_PAD_R1;
    } else {
        return pad;
    }
}

// Returns true if an input is on-beat
BOOL ActionCalcTapStats(register s32 taptime, register s32 window, register s32 *nth) {
    s32 n;
    s32 m;

    m = (taptime + window) % (96 * 4); // Bar wrapping
    n = m % (96 / 4); // Dot time
    *nth = m / (96 / 4);
    dbgInfo.dif = n - window;
    return (window * 2) >= n;
}

BOOL ActionRegisterTap(register SND_INFO *snd, register SCENE_INFO *scn, register s32 nth, register BOOL on) {
    BOOL tapok;
    u32 unused;
    u32 pad;

    pad = ActionRemapLR(scn->pad);
    if (scn->notplayer == FALSE) {
        ActionRecordTap(pad, nth, scn->taptime);
    }
    if (on == TRUE) {
        D_8008239C[0][nth].pad = pad;
        D_8008239C[0][nth].keyid = EventGetKeyIndexFromPad(pad);
        D_8008239C[0][nth].num++;
        D_8008239C[0][nth].snd = snd;
        tapok = TRUE;
        actionInfo.sub.taponnum++;
    } else {
        tapok = FALSE;
        actionInfo.sub.tapoffnum++;
    }

    actionInfo.sub.tapnum++;
    actionInfo.sub.tappad |= pad;

    dbgInfo.tapok = tapok;
    dbgInfo.tapon = actionInfo.sub.taponnum;
    dbgInfo.tapoff = actionInfo.sub.tapoffnum;
    dbgInfo.ntap = actionInfo.sub.tapnum;
    dbgInfo.unk60 = scn->prs;
    return tapok;
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionCalcScoreNormal);

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionCalcScoreHigh);

void ActionRecordTap(register u32 pad, register s32 nth, register s32 time) {
    if (eventRecordInfo.idx >= 550) {
        return;
    }

    eventRecordInfo.data[eventRecordInfo.idx].pad = pad;
    eventRecordInfo.data[eventRecordInfo.idx].time = time;
    eventRecordInfo.idx++;
    eventRecordInfo.num = eventRecordInfo.idx;
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/praction_1", ActionCalcActScore);

void ActionGetOnInput(register ON_INPUT_INFO *oninp[], register s32 type, register u32 ncall) {
    u32 unused;

    if (type == PR_INP_TYPE_1) {
        oninp[0] = actionInfo.onlist[(ncall - 1) % 4];
    } else if (type == PR_INP_TYPE_2) {
        oninp[0] = actionInfo.onlist[(ncall - 2) % 4];
        oninp[1] = actionInfo.onlist[(ncall - 1) % 4];
    }
}
