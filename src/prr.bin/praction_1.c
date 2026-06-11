#include "praction.h"
#include "prevent.h"
#include "prvdbg.h"
#include "prrap.h"
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
s32 ActionCalcActScore(ON_INPUT_INFO *oninp[], s32 type, u32 validPad);
void ActionGetOnInput(ON_INPUT_INFO *oninp[], s32 type, u32 ncall);

SVECTOR *actionViewSubframe;

extern MATRIX actionBezPosMtx;
extern MATRIX actionBezRefMtx;
extern SVECTOR actionBezPos[110];
extern SVECTOR actionBezRef[110];
extern s32 actionBezInt[110];
extern PARA_TMD_OBJECT actionTmdObj[10][4];
extern PARA_VDF_OBJECT actionVdfObj[10][128];
extern PARA_TMD_DATA actionTmdData[10];
extern PARA_VDF_DATA actionVdfData[10];


void ActionBezInterpolate(register s32 sf, register MATRIX *refmtx, register SVECTOR *ref, register VECTOR *view) {
    SVECTOR interp2;
    VECTOR interp1;
    MATRIX mtx;

    ApplyMatrix(refmtx, &actionViewSubframe[sf], &interp1);
    interp2.vx = ONE;
    interp2.vy = actionViewSubframe[sf].pad; // Hmm
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

void ActionSetTmdInfo(void *tmd, PARA_TMD_DATA *data) {
    u32 size;
    u32 *dop;
    s32 i;
    s32 n;

    dop = tmd;
    dop++;
    dop++;
    n = data->objnum = *dop;
    dop++;
    for (i = 0; i < n; i++) {
        data->obj[i].vtxtop = (SVECTOR *)*dop++;
        data->obj[i].vtxtotal = *dop++;
        data->obj[i].nrmtop = (SVECTOR *)*dop++;
        data->obj[i].nrmtotal = *dop++;
        data->obj[i].prmtop = (u32 *)*dop++;
        data->obj[i].prmtotal = *dop++;
        data->obj[i].scale  = *dop++;
    }
}

void ActionSaveOriginalVtx(PARA_TMD_DATA *tmd, SVECTOR *orgvtx) {
    SVECTOR *otp;
    SVECTOR *bsp;
    SVECTOR *dfp;
    s32 i;
    s32 j;
    s32 n;
    s32 m;

    bsp = tmd->orgvtx = orgvtx;
    m = tmd->objnum;
    n = 0;
    for (j = 0; j < m; j++) {
        otp = tmd->obj[j].vtxtop;
        bsp += n;
        n = tmd->obj[j].vtxtotal;
        for (i = 0; i < n; i++) {
            *(bsp + i) = *(otp + i);
        }
    }
}

void ActionSetVdf(void *vdf, PARA_VDF_DATA *data) {
    s32 i;
    s32 n;
    u32 *dop2;
    
    dop2 = vdf;
    n = data->objnum = *dop2++;
    
    for (i = 0; i < n; i++) {
        data->obj[i].object = *dop2++;
        data->obj[i].offset = *dop2++;
        data->obj[i].total  = *dop2++;
        data->obj[i].top    = (SVECTOR *)dop2;
        dop2 += data->obj[i].total * 2;
    }
}

s32 ActionInitModel(s32 i, void *tmd, void *vdf, SVECTOR *orgvtx) {
    s32 n;

    actionTmdData[i].objnum = 1;
    actionTmdData[i].orgvtx = orgvtx;
    actionTmdData[i].obj = actionTmdObj[i];

    actionVdfData[i].objnum = 1;
    actionVdfData[i].obj = actionVdfObj[i];
    actionVdfData[i].mime = actionMimeList[i];
    ActionSetTmdInfo(tmd, &actionTmdData[i]);
    if (vdf != NULL) {
        ActionSaveOriginalVtx(&actionTmdData[i], orgvtx);
        ActionSetVdf(vdf, &actionVdfData[i]);
    }
    return actionVdfData[i].objnum;
}

s32 ActionSetVdfData(s32 i, void *vdf) {
    if (vdf != NULL) {
        ActionSetVdf(vdf, &actionVdfData[i]);
    }
    return actionVdfData[i].objnum;
}

void ActionResetMimeVdf(s32 i) {
    ActionResetMimeVtx(&actionTmdData[i]);
}

void ActionVtxMime(s32 i) {
    ActionResetMimeVtx(&actionTmdData[i]);
    ActionSetMimeVtx(&actionTmdData[i], &actionVdfData[i]);

}

void ActionSetVdfForTmd(s32 tmd, s32 vdf) {
    ActionSetMimeVtx(&actionTmdData[tmd], &actionVdfData[vdf]);
}

void ActionResetMimeVtx(register PARA_TMD_DATA *data) {
    SVECTOR *otp;
    SVECTOR *bsp;
    s32 i;
    s32 j;
    s32 n;
    s32 m;
    
    bsp = data->orgvtx;
    m = data->objnum;
    n = 0;
    for (j = 0; j < m; j++) {
        otp = data->obj[j].vtxtop;
        bsp += n;
        n = data->obj[j].vtxtotal;
        for (i = 0; i < n; i++) {
            *(otp + i) = *(bsp + i);
        }
    }
}

void ActionSetMimeVtx(register PARA_TMD_DATA *tmd, register PARA_VDF_DATA *vdf) {
    SVECTOR *otp;
    SVECTOR *bsp;
    SVECTOR *dfp;
    s32 i;
    s32 n;

    n = vdf->objnum;

    for (i = 0; i < n; i++) {
        otp = tmd->obj[vdf->obj[i].object].vtxtop + vdf->obj[i].offset;
        dfp = vdf->obj[i].top;
        if (vdf->mime[i] != 0) {
            gteMIMefunc(otp, dfp, vdf->obj[i].total, vdf->mime[i]);
        }
    }
}

typedef ON_INPUT_INFO ON_INPUT_INFO_LIST[32];
static ON_INPUT_INFO_LIST *actionOnInputList = actionInfo.onlist;
static BOOL D_800823A0 = FALSE;
BOOL D_800823A4 = FALSE;

void ActionInit(void) {
    s32 i;

    actionOnInputList = actionInfo.onlist;
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

s32 ActionCalcScoreLvlEstimate(SCENE_INFO *scn) {
    s32 diff;
    s32 highthresh;

    if (scn->teacherinp == NULL) {
        return PR_DIR_IDLE;
    }

    actionInfo.sub.lvlestscore = actionInfo.sub.score;
    
    diff = actionInfo.sub.lvlestscore - actionInfo.sub.lvlestscoreprev;
    actionInfo.sub.lvlestscoreprev = actionInfo.sub.lvlestscore;
    dbgInfo.decsize_lvlscorediff = diff;
    if (scn->level == LEVEL_NORM) {
        highthresh = scn->teacherinp->highthreshold;
        if (diff > highthresh) {
            return PR_DIR_UP;
        } else if (diff < 0) {
            return PR_DIR_DOWN;
        } else if (actionInfo.sub.lostscoreonedgeranks == TRUE) {
            return PR_DIR_DOWN;
        } else {
            return PR_DIR_IDLE;
        }
        // Unreachable
        return;
    }
    if (scn->level == LEVEL_LOW1) {
        if (diff > 0) {
            return PR_DIR_UP;
        } else if (diff < 0) {
            return PR_DIR_DOWN;
        } else if (actionInfo.sub.lostscoreonedgeranks == TRUE) {
            return PR_DIR_DOWN;
        } else {
            return PR_DIR_IDLE;
        }
        // Unreachable
        return;
    }
    if (diff > 0) {
        return PR_DIR_UP;
    } else if (diff < 0) {
        actionInfo.sub.lostscoreonedgeranks = TRUE;
        return PR_DIR_DOWN;
    } else {
        return PR_DIR_IDLE;
    }
}

s32 ActionUpdateInput(SCENE_INFO *scn) {
    INPUT_KEY_INFO *inp;
    INPUT_KEY_SND_MAP *sndmap;
    s32 infoid;
    s32 ctrlid;
    s32 kctrl;
    s32 nth;
    BOOL on;
    BOOL tapon;
    u32 pad;

    if (EventInputsInactive(scn) == TRUE) {
        return -1;
    }
    if (eventInfo.controlstage == FALSE) {
        return -2;
    }
    if (scn->parappainp == NULL) {
        return -3;
    }

    pad = ActionRemapLR(scn->pad);
    if ((infoid = scn->parappainp->sub[scn->lvlhigh].infoid) == 0) {
        return -5;
    }

    if ((inp = sceneInitInfo.keyinfo[infoid].info[scn->keyid]) == NULL) {
        return -6;
    }

    if ((ctrlid = scn->parappainp->sub[scn->lvlhigh].ctrlid) == 0) {
        return -7;
    }

    on = ActionCalcTapStats(scn->taptime, scn->leniency, &nth);
    dbgInfo.nth = nth;

    if ((kctrl = sceneInitInfo.keyctrl[ctrlid].ctrl[nth / 2]) == 0) {
        return -8;
    }

    if ((actionInfo.sub.unk600 != scn->keyid) && (scn->keyid != PR_TAP_NONE)) {
        inp->mapnum = 0;
        D_800823A0 = TRUE; 
    } else {
        D_800823A0 = FALSE;
    }
    actionInfo.sub.unk600 = scn->keyid;

    if ((D_800823A0 == FALSE) && (inp->mapmax > 1) && (pad & PR_PAD_RIGHT)) {
        inp->mapnum = ((inp->mapnum >= 1) ? inp->mapnum : inp->mapmax) - 1;
    }
    if (pad & PR_PAD_LEFT) {
        inp->mapnum = 0;
    }
    sndmap = &inp->keymap[inp->mapnum];
    if (inp->mapmax > 1) {
        inp->mapnum = (++inp->mapnum) % inp->mapmax;
    }
    RapPlayInterruptableKey(&sndmap->snd);
    if (sndmap->keyid != PR_TAP_NONE) {
        sceneInitInfo.keypressed(scn, sndmap->keyid);
    }
    
    if (kctrl != 2) {
        return -9;
    }
    tapon = ActionRegisterTap(&sndmap->snd, scn, nth, on);
    if (tapon == TRUE) {
        actionInfo.sub.vcount = sndmap->vcount;
    }
    return 0;
}

s32 ActionCalcOrigScore(ON_INPUT_INFO *oninp, s32 start, s32 end) {
    s32 num11;
    s32 num10;
    s32 num01;
    s32 num00;
    s32 total;
    u32 used;
    s32 i;

    for (i = 0; i < 4; i++) {
        dbgInfo.origscorenum[i] = 0;
    }
    dbgInfo.origscoreused = 0;
    dbgInfo.spptr = MIN(dbgInfo.spptr, GetSp());

    if (actionInfo.sub.highenabled == FALSE) {
        return 0;
    }

    num00 = 0;
    num01 = 0;
    num10 = 0;
    num11 = 0;
    used = 0;
    total = 0;

    for (i = start; i <= end; i += 2) {
        if ((oninp[i + 0].num != 0) && (oninp[i + 1].num != 0)) {
            used |= 8;
            num11++;
        } else 
        if ((oninp[i + 0].num != 0) && (oninp[i + 1].num == 0)) {
            used |= 4;
            num10++;
        } else 
        if ((oninp[i + 0].num == 0) && (oninp[i + 1].num != 0)) {
            used |= 2;
            num01++;
        } else {
            num00++;
        }
        
    }

    if (used == (8 | 4 | 2)) {
        total += (num01 * 15) + (num10 * 6) + (num11 * 9);
        if ((start == 0) && (num00 > 0)) {
            total += 18;
        }
    } else if (used == (8 | 4)) {
        total += (num11 * 9) + (num10 * 6);
    } else if (used == (8 | 2)) {
        total += (num11 * 9) + (num01 * 15);
    } else if (used == (4 | 2)) {
        total += (num01 * 15) + (num10 * 6);
    }

    dbgInfo.origscoreused = used;
    dbgInfo.origscorenum[0] = num11;
    dbgInfo.origscorenum[1] = num10;
    dbgInfo.origscorenum[2] = num01;
    dbgInfo.origscorenum[3] = num00;
    return total;
}

void ActionClearOnInputList(u32 i) {
    actionOnInputList = actionInfo.onlist[i % 4];
    MemoryZero(actionOnInputList, sizeof(*actionOnInputList));
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

void ActionResetKeyInfo(SCENE_INFO *scn) {
    s32 i;
    s32 j;
    INPUT_KEY_INFO *keyinfo;

    for (i = 0; i < sceneInitInfo.keyinfolen; i++) {
        for (j = 0; j < PR_TAP_NUM; j++) {
            if ((keyinfo = sceneInitInfo.keyinfo[i].info[j]) != NULL) {
                keyinfo->mapnum = 0;
            }
        }
    }
}

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
        actionOnInputList[0][nth].pad = pad;
        actionOnInputList[0][nth].keyid = EventGetKeyIndexFromPad(pad);
        actionOnInputList[0][nth].num++;
        actionOnInputList[0][nth].snd = snd;
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


// TODO: appInfo
#define SCENE_IDX_3 3
#define SCENE_IDX_5 5
extern s16 D_8009F820;

s32 ActionCalcScoreNormal(register SCENE_INFO *scn) {
    s32 i;
    s32 actscore;
    s32 origscore;
    s32 scorediff;
    s32 offpenalty;
    s32 coreinputs;
    s32 sp28;
    s32 sp2C;
    s32 firstinp;
    s32 type;
    s32 firstkeyid;
    s32 tapl;
    BOOL tapnumok;
    BOOL firstpressed;
    BOOL notspecialscene;
    BOOL levellow;
    ON_INPUT_INFO *oninp[4];

    origscore = 0;
    actscore = 0;
    offpenalty = 0;
    scorediff = 0;

    firstinp = scn->teacherinp->unk1;
    if ((type = scn->teacherinp->sub[scn->lvlhigh].type) <= 0) {
        // BUG: No return value
        return;
    }

    tapl = type * 12;
    tapnumok = actionInfo.sub.tapnum <= tapl;
    ActionGetOnInput(oninp, type, scn->curline);

    firstkeyid = oninp[0][firstinp].keyid;
    firstpressed = (firstkeyid == scn->teacherinp->firstinput);
    coreinputs = ActionCalcActScore(oninp, type, scn->teacherinp->validpad);

    if ((firstpressed == TRUE) && (tapnumok == TRUE)) {
        actscore = coreinputs * 3;
        for (i = 0; i < type; i++) {
            if (coreinputs > 0) {
                origscore += ActionCalcOrigScore(oninp[i], 0, 15);
            }
        }
    } else {
        actscore = coreinputs * 3 - scn->teacherinp->teacherinputnum;
    }
    offpenalty = ((-(scn->teacherinp->teacherinputnum * actionInfo.sub.tapoffnum)) < (-(scn->teacherinp->teacherinputnum * 3 + 1)))
            ? (-(scn->teacherinp->teacherinputnum * 3 + 1))
            : (-(scn->teacherinp->teacherinputnum * actionInfo.sub.tapoffnum));
    levellow = ((scn->level == LEVEL_LOW1) || (scn->level == LEVEL_LOW2));
    notspecialscene = ((D_8009F820 != SCENE_IDX_3) && (D_8009F820 != SCENE_IDX_5));

    if ((actionInfo.sub.highenabled == FALSE) || (levellow == TRUE) || (notspecialscene == TRUE)) {
        if (actionInfo.sub.tapnum < scn->teacherinp->teacherinputnum) {
            offpenalty = 0;
            origscore = 0;
            actscore = 0;
        }
    } else if (tapl < actionInfo.sub.tapnum) {
        offpenalty = 0;
        origscore = 0;
        actscore = 0;
    }
    scorediff = actscore + origscore + offpenalty;

    dbgInfo.coreinputs = coreinputs;
    dbgInfo.actscore = actscore;
    dbgInfo.origscore = origscore;
    dbgInfo.firstpressed = firstpressed;
    dbgInfo.offpenalty = offpenalty;
    dbgInfo.scorediff = scorediff;

    return scorediff;
}

s32 ActionCalcScoreHigh(register SCENE_INFO *scn) {
    s32 actscore;
    s32 origscore;
    s32 scorediff;
    s32 offpenalty;
    s32 coreinputs;
    s32 origstart;
    s32 origend;
    s32 sp2C;
    s32 i;
    s32 type;
    s32 tapl;
    s32 tapnumok;
    ON_INPUT_INFO *oninp[4];

    type = scn->teacherinp->sub[scn->lvlhigh].type;
    offpenalty = 0;
    origscore = 0;

    tapl = type * 12;
    tapnumok = actionInfo.sub.tapnum <= tapl;

    ActionGetOnInput(oninp, type, scn->curline);
    origstart = 0;
    origend = 15;
    coreinputs = ActionCalcActScore(oninp, type, PR_PAD_NONE);
    if (tapnumok == TRUE) {
        for (i = 0; i < type; i++) {
            origscore += ActionCalcOrigScore(oninp[i], origstart, origend);
        }
    } else if (tapl < actionInfo.sub.tapnum) {
        offpenalty = 0;
        origscore = 0;
        actscore = 0;
    }
    actscore = (coreinputs * 3) + (actionInfo.sub.tapoffnum * -2);
    scorediff = actscore + origscore;
    dbgInfo.coreinputs = coreinputs;
    dbgInfo.actscore = actscore;
    dbgInfo.origscore = origscore;
    dbgInfo.offpenalty = offpenalty;
    return scorediff;
}

void ActionRecordTap(register u32 pad, register s32 nth, register s32 time) {
    if (eventRecordInfo.idx >= 550) {
        return;
    }

    eventRecordInfo.data[eventRecordInfo.idx].pad = pad;
    eventRecordInfo.data[eventRecordInfo.idx].time = time;
    eventRecordInfo.idx++;
    eventRecordInfo.num = eventRecordInfo.idx;
}

s32 ActionCalcActScore(register ON_INPUT_INFO *oninp[], register s32 type, register u32 validpad) {
    s32 i;
    s32 j;
    s32 score;
    u32 unused;
    u32 pad;
    ON_INPUT_INFO *on;

    pad = PR_PAD_NONE;
    score = 0;
    for (i = 0; i < type; i++) {
        on = oninp[i];
        for (j = 0; j < 16; j++) {
            pad |= on[j].pad;
            if (on[j].num == 0) {
                continue;
            }
            if (validpad == PR_PAD_NONE) {
                score++;
            } else if (validpad & on[j].pad) {
                score++;
            }
        }
    }

    if (validpad == PR_PAD_NONE) {
        return score;
    } else {
        return ((validpad & pad) == validpad) ? score : 0;
    }
}

void ActionGetOnInput(register ON_INPUT_INFO *oninp[], register s32 type, register u32 ncall) {
    u32 unused;

    if (type == PR_INP_TYPE_1) {
        oninp[0] = actionInfo.onlist[(ncall - 1) % 4];
    } else if (type == PR_INP_TYPE_2) {
        oninp[0] = actionInfo.onlist[(ncall - 2) % 4];
        oninp[1] = actionInfo.onlist[(ncall - 1) % 4];
    }
}
