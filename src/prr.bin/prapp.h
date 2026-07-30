#ifndef _PRAPP_H_
#define _PRAPP_H_

#include "common.h"
#include "prinfo.h"
#include "prcard.h"
#include "prcd.h"

#define SCENE_IDX_0 0
#define SCENE_IDX_1 1
#define SCENE_IDX_2 2
#define SCENE_IDX_3 3
#define SCENE_IDX_4 4
#define SCENE_IDX_5 5
#define SCENE_IDX_6 6
#define SCENE_IDX_7 7

#define STAGE_IDX_0 0
#define STAGE_IDX_1 1
#define STAGE_IDX_2 2
#define STAGE_IDX_3 3
#define STAGE_IDX_4 4
#define STAGE_IDX_5 5

#define STAGECLEAR_0 0
#define STAGECLEAR_1 1
#define STAGECLEAR_2 2
#define STAGECLEAR_3 3

#define PLAY_MODE_0 0
#define PLAY_MODE_1 1
#define PLAY_MODE_2 2


typedef struct APP_INFO {
    s16 playmode;
    s16 watchedintro;
    s16 scenei;
    s16 demohalf;
    s16 lang;
    s16 difficulty;
    s16 testmode;
    s16 dbgmsg;
    s16 entermenu;
    s16 nosave;
    s16 minuspnt;
    s16 printmode;
    s16 frametext;
    s16 erasegui;
    s16 scorecheat;
    u8 unk1e[2];
} APP_INFO;


extern APP_INFO appInfo;
extern CARD_INFO D_800A2174;


void AppFrameLoadingRunner(void);
void AppFrameLoadingStart(s32 frametext);
void AppFrameLoadingEnd(void);
void AppReadMod(CD_FILE *comod, s32 arg1);
void AppInit(void);
void AppReadZCompo(s32 i);
void AppReadS0YCompo(s32 i);
void AppReadCompo(s32 i, s32 frametext, BOOL snd);
void AppReplaySaveDataSave(SAVE_DATA *data);
void AppReplaySaveDataLoad(SAVE_DATA *data);
s32 AppRunMainMenu(SCENE_INFO *scn, s32 i);
void AppScreenSonyIn(void);
void AppScreenSonyOut(void);
void AppMain(void);
void AppSortHighScoreInfo(HIGHSCORE_INFO *info, s32 n);
s32 AppIndexToStageIndex(s32 i);
s32 AppStageIndexToStageNum(s32 stage);
BOOL AppStageAllHigh(u8 *stageclear);
void AppUnlockStage(s32 i);
s32 AppGetCurrentStageNum(void);
void AppSaveStageStats(s32 i, s32 clear, s32 prevclear, s32 score);
void CardCacheSaveData(SAVE_DATA *data);
void AppUpdateHighScore(SAVE_DATA *data);
s32 AppGetStageClear(s32 i);
BOOL AppStageCleared(s32 i);
BOOL AppPrevStageCleared(s32 i);
void func_80017b28(s32 i, BOOL arg1);
void AppGetReplayRecordData(void);
void AppResetHighScore(void);
void AppCheckScoreCheat(SCENE_INFO *scn, s32 arg1);
void AppSwapHighScoreInfo(HIGHSCORE_INFO *a, HIGHSCORE_INFO *b);

#endif
