#include "prapp.h"
#include <string.h>
#include <libetc.h>
#include <kernel.h>
#include <libgte.h>
#include <libgpu.h>
#include "prvdatal.h"
#include "prrap.h"
#include "prmemory.h"
#include "prvdbg.h"
#include "prcompo.h"


typedef struct APP_SCENEFN {
    void (*setData)(void);
    void (*init)(VDATA_SCENE *scene, s32 i);
    s32 (*main)(s32 i);
} APP_SCENEFN;


// static char rcsid[] = "@(#)prapp.c: version 01-00 95/10/10 00:00:00";

extern APP_SCENEFN D_80062598[];

extern BOOL D_800826D8;

extern SAVE_DATA D_80088640;
extern CARD_INFO D_800A2174;

extern void * const D_80010000;


void AppFrameLoadingRunner(void) {
    if (D_800826D8 == FALSE) {
        CompoFrameDraw((void *)1, 0); // TODO: ???
        RapFlush();
        D_800826D8++;
    } else {
        CompoFrameSwap(NULL);
        D_800826D8 = FALSE;
    }
}

void AppFrameLoadingStart(s32 frametext) {
    appInfo.frametext = frametext;
    D_800826D8 = FALSE;
    VSyncCallback(AppFrameLoadingRunner);
}

void AppFrameLoadingEnd(void) {
    VSyncCallback(NULL);
    if (D_800826D8 == TRUE) {
        CompoFrameSwap(NULL);
    }
}

void AppReadMod(CD_FILE *comod, s32 arg1) {
    CDFileReadBuf(comod, D_80010000);
}

void AppInit(void) {
    SetMem(2);
    ResetCallback();
    ResetGraph(0);
    MemoryInit();
    CDInit();
    RapInit();
    CompoInit();
    SetGraphDebug(0);
    DbgInit();
    DbgResetRun();
    AppSaveStageStats(SCENE_IDX_1, STAGECLEAR_1, STAGECLEAR_1, 0);
}

void AppReadZCompo(s32 i) {
    AppFrameLoadingStart(COMPO_FRAME_TEXT_TYPE_3);
    CDFileRead(&D_80067E90[i].files[VDATA_SCENE_FILE_ZCOMPO], TRUE);
    AppFrameLoadingEnd();
}

void AppReadS0YCompo(s32 i) {
    MemoryClear();
    CDFileRead(&D_80067E38, TRUE);
}

void AppReadCompo(s32 i, s32 frametext, BOOL snd) {
    appInfo.frametext = frametext;
    AppFrameLoadingStart(frametext);
    CDFileRead(&D_80067E90[i].files[VDATA_SCENE_FILE_COMPO], snd);
    AppFrameLoadingEnd();
}

void AppReplaySaveDataSave(SAVE_DATA *data) {
    bcopy(data, &D_80088640, sizeof(*data));
}

void AppReplaySaveDataLoad(SAVE_DATA *data) {
    bcopy(&D_80088640, data, sizeof(*data));
}

s32 AppRunMainMenu(SCENE_INFO *scn, s32 i) {
    s32 menuret;
    s32 stageindex;
    HIGH_SCORE_VISUAL *highscorevisual;

    RapStopSeq();
    RapPlayKey(sceneInitInfo.intermission);
    RapFlush();
    AppReadZCompo(i);
    while (TRUE) {
        // 44
        while (PadRead(1) != 0);
        menuret = func_80034DC0(PR_MENU_MAIN, &appInfo);
        if (menuret == 1) {
            highscorevisual = CardMenuHighScore(&D_800A2174);
            if (highscorevisual != NULL) {
                func_80034DC0(PR_MENU_HIGHSCORE, highscorevisual);
            }
        } else if (menuret == 2) {
            AppReplaySaveDataSave(&D_800A2174.save);
            if ((stageindex = CardMenuReplay(&D_800A2174)) >= 0) {
                appInfo.playmode = PLAY_MODE_2;
                return AppStageIndexToStageNum(stageindex);
            }
        } else if (menuret == 3) {
            PracticeRun(&sceneInfo, i);
        } else if (menuret == 4) {
            menuret = func_80034DC0(PR_MENU_STAGE, &stageindex);
            if (menuret == 1) {
                return stageindex;
            }
        } else if (menuret == 6) {
            CardMenuLoad(&D_800A2174);
        } else if (menuret == 7) {
            return 0;
        }
    }
}

void AppScreenSonyIn(void) {
    s32 i;

    CompoSonyLoad();
    for (i = 0; i < 30; i++) {
        CompoSonyDraw(i);
        VSync(2);
        CompoSonySwap();
    }
}

void AppScreenSonyOut(void) {
    s32 i;

    for (i = 120; i < 180; i++) {
        CompoSonyDraw(i);
        VSync(2);
        CompoSonySwap();
    }
}

void AppMain(void) {
    s32 i;
    s32 menui;
    s32 nexti;

    AppScreenSonyIn();
    MemoryClear();
    CDFileRead(&D_80067DE0, FALSE);
    AppScreenSonyOut();
    AppCheckScoreCheat(&sceneInfo, 0);
    appInfo.scenei = 0;
    menui = 0;
    i = 0;
    appInfo.entermenu = FALSE;

    while (TRUE) {
        if (appInfo.entermenu == TRUE) {
            appInfo.entermenu = FALSE;
            if (i == 0) {
                menui = 0;
            }
            CompoFrameResetInactive();
            i = AppRunMainMenu(&sceneInfo, menui);
            menui = i;
        }
        MemoryClear();
        CDFileSearch(&D_80067E90[i].files[VDATA_SCENE_FILE_COMOD]);
        AppReadMod(&D_80067E90[i].files[VDATA_SCENE_FILE_COMOD], 0);
        MemoryClear();
        D_80062598[i].setData();
        D_80062598[i].init(&D_80067E90[i], i);
        nexti = D_80062598[i].main(i);
        if (nexti < 0) {
            appInfo.entermenu = TRUE;
        } else {
            appInfo.scenei = i = nexti;
        }
        CompoResetPacket();
    }
}

void AppSortHighScoreInfo(HIGHSCORE_INFO *info, s32 n) {
    s32 i;
    s32 j;

    for (i = 0; i < (n - 1); i++) {
        for (j = i + 1; j < n; j++) {
            if (info[i].score < info[j].score) {
                AppSwapHighScoreInfo(&info[i], &info[j]);
            }
        }
    }
}

extern s32 D_80062604[];
extern s32 D_80062624[];

s32 AppIndexToStageIndex(s32 i) {
    return D_80062604[i];
}

s32 AppStageIndexToStageNum(s32 stage) {
    return D_80062624[stage];
}

BOOL AppStageAllHigh(u8 *stageclear) {
    s32 i;

    for (i = 0; i < 6; i++) {
        if (stageclear[i] != STAGECLEAR_3) {
            return FALSE;
        }
    }
    return TRUE;
}

void AppUnlockStage(s32 i) {
    CARD_INFO *cardinfo;
    s32 stageindex;

    stageindex = AppIndexToStageIndex(i);
    cardinfo = &D_800A2174;

    if (cardinfo->save.stageclear[stageindex] == STAGECLEAR_0) {
        cardinfo->save.stageclear[stageindex] = STAGECLEAR_1;
    }
}

s32 AppGetCurrentStageNum(void) {
    return AppStageIndexToStageNum(D_800A2174.save.stagecur);
}

void AppSaveStageStats(s32 i, s32 clear, s32 prevclear, s32 score) {
    s32 unused;
    SAVE_DATA *savedata;
    s32 stageindex;

    stageindex = AppIndexToStageIndex(i);
    savedata = &D_800A2174.save;

    if ((savedata->stageprevclear = prevclear) == STAGECLEAR_0) {
        savedata->stageprevclear = STAGECLEAR_1;
    }
    if (savedata->stageclear[stageindex] < clear) {
        savedata->stageclear[stageindex] = clear;
    }
    if (savedata->stagescore[stageindex] < score) {
        savedata->stagescore[stageindex] = score;
    }
    savedata->stagecur = stageindex;
    savedata->recorddatanum = eventRecordInfo.num;
    bcopy(&eventRecordInfo, savedata->recorddata, sizeof(savedata->recorddata));
    savedata->allhigh = AppStageAllHigh(savedata->stageclear);
    
}

void AppCacheSaveData(SAVE_DATA *data) {
    bcopy(data, &D_800A2174.save, sizeof(*data));
}

void AppUpdateHighScore(SAVE_DATA *data) {
    HIGHSCORE_INFO *selhighscoreinfo;
    HIGHSCORE_INFO *highscoreinfo;
    s32 i;
    s32 j;
    s32 score;
    char *name;

    for (i = 0; i < 6; i++) {
        score = data->stagescore[i];
        name = data->name;
        for (j = 0; j < 3; j++) {
            highscoreinfo = &D_800A2174.highscore[i][j];
            if ((score == highscoreinfo->score) && (strcmp(name, highscoreinfo->name) == 0)) {
                goto skipsort;
            }
        }
        selhighscoreinfo = &D_800A2174.highscore[i][3];
        selhighscoreinfo->score = score;
        strcpy(selhighscoreinfo->name, name);
        AppSortHighScoreInfo(D_800A2174.highscore[i], 4);
skipsort:;
    }
}

s32 AppGetStageClear(s32 i) {
    s32 stageindex;

    stageindex = AppIndexToStageIndex(i);
    return D_800A2174.save.stageclear[stageindex];
}

BOOL AppStageCleared(s32 i) {
    return (AppGetStageClear(i) >= STAGECLEAR_2) ? TRUE : FALSE;
}

BOOL AppPrevStageCleared(s32 i) {
    s32 stageprevclear;

    stageprevclear = D_800A2174.save.stageprevclear;
    return (stageprevclear >= STAGECLEAR_2) ? TRUE : FALSE;
}

void func_80017b28(s32 i, BOOL arg1) {
    s32 stageindex;

    stageindex = AppIndexToStageIndex(i);
    D_800A2174.save.stageclear[stageindex] = (arg1 == TRUE) ? STAGECLEAR_2 : STAGECLEAR_1;
}

void AppGetReplayRecordData(void) {
    s32 i;

    eventRecordInfo.num = D_800A2174.save.recorddatanum;
    for (i = 0; i < eventRecordInfo.num; i++) {
        eventRecordInfo.data[i] = D_800A2174.save.recorddata[i];
    }
}

void AppResetHighScore(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            D_800A2174.highscore[i][j].score = 0;
            D_800A2174.highscore[i][j].name[0] = '\0';
        }
    }
}

void AppCheckScoreCheat(register SCENE_INFO *scn, register s32 arg1) {
    s32 i;
    u32 pad;

    pad = 0;
    for (i = 0; i < 30; i++) {
        if ((pad = PadRead(1)) != 0) {
            break;
        }
        VSync(0);
    }
    if (pad == 0) {
        return;
    }
    if (pad == (PR_PAD_SELECT | PR_PAD_TRIANGLE | PR_PAD_R1 | PR_PAD_L1 | PR_PAD_R2 | PR_PAD_L2)) {
        appInfo.scorecheat = TRUE;
    }
}

void AppSwapHighScoreInfo(register HIGHSCORE_INFO *a, register HIGHSCORE_INFO *b) {
    s32 tempscore;
    char tempname[12];

    tempscore = a->score;
    strcpy(tempname, a->name);

    a->score = b->score;
    strcpy(a->name, b->name);

    b->score = tempscore;
    strcpy(b->name, tempname);
}
