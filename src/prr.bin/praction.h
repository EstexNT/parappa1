#ifndef _PRACTION_H_
#define _PRACTION_H_

#include "common.h"
#include "prscene.h"
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "prmime.h"


#define PR_INP_TYPE_0 0
#define PR_INP_TYPE_1 1
#define PR_INP_TYPE_2 2


typedef struct ON_INPUT_INFO {
    u32 pad;
    s16 keyid;
    s16 num; // Number of inputs done
    SND_INFO *snd;
} ON_INPUT_INFO;

typedef struct ACTION_INFO {
    ON_INPUT_INFO onlist[4][32];
    struct ACTION_INFO_SUB {
        s32 unk600;
        s32 unk604;
        u32 tappad;
        u32 tappadprev;
        s16 tapnum;
        s16 taponnum;
        s16 tapoffnum;
        s16 score;
        s16 perfscoreprev;
        s16 lvlestscore;
        s16 lvlestscoreprev;
        s16 lostscoreonedgeranks;
        s16 tapnonenum;
        s16 addscore;
        s16 vcount;
        s16 highended;
        s16 highfailcount;
        s16 highenabled;
    } sub;
} ACTION_INFO;


extern ACTION_INFO actionInfo;
extern GsRVIEW2 D_800A3620;
extern BOOL D_800823A4;


void ActionInitView(SVECTOR *sf);
void ActionSetBez(u32 *bezdata, s32 divisor);
void ActionStartBez(void);
s32 ActionFollowBez(s32 frame);
s32 ActionInitModel(s32 i, void *tmd, void *vdf, SVECTOR *orgvtx);
s32 ActionSetVdfData(s32 i, void *vdf);
void ActionResetMimeVdf(s32 i);
void ActionVtxMime(s32 i);
void ActionSetVdfForTmd(s32 tmd, s32 vdf);
void ActionInit(void);
void ActionSetHighEnabled(BOOL enabled);
s32 ActionGetCurScore(SCENE_INFO *scn);
void ActionResetStats(void);
void ActionUpdateNoTapNum(SCENE_INFO *scn);
void ActionResetPerfomanceScore(void);
BOOL ActionCheckGameOver(SCENE_INFO *scn);
BOOL ActionGetHighEnded(SCENE_INFO *scn);
s32 ActionCalcScoreLvlEstimate(SCENE_INFO *scn);
s32 ActionUpdateInput(SCENE_INFO *scn);
void ActionClearOnInputList(u32 i);
void ActionRestartTap(SCENE_INFO *scn);
void ActionAddScore(s32 score);
s32 ActionCheckPerformanceHigh(SCENE_INFO *scn);
s32 ActionCheckPerformanceNormal(SCENE_INFO *scn);
void ActionCalcScore(SCENE_INFO *scn);
void ActionResetKeyInfo(SCENE_INFO *scn);
void ActionUpdateVcount(SCENE_INFO *scn, s32 vcount);

#endif
