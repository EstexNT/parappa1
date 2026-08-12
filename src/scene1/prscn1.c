#include "prscn1.h"
#include <libetc.h>

static char rcsid[] = "@(#)prscn1.c: version 01-00 95/10/10 00:00:00";

extern s32 D_801DF2F8;
extern VDATA_SCENE *D_801DF2FC;
extern s32 D_801DF300;
extern BOOL D_801DF304;
extern BOOL D_801DF308;
extern s32 D_801DF30C;
extern s8 *D_801DF310;
extern s8 *D_801DF314;

u32 D_801D5D9C = 0;
s16 D_801D5DA0[] = {
    [SCENE_IDX_0] = COMPO_FRAME_TEXT_TYPE_1,
    [SCENE_IDX_1] = COMPO_FRAME_TEXT_TYPE_1,
    [SCENE_IDX_2] = COMPO_FRAME_TEXT_TYPE_2,
    [SCENE_IDX_3] = COMPO_FRAME_TEXT_TYPE_3,
    [SCENE_IDX_4] = COMPO_FRAME_TEXT_TYPE_1,
    [SCENE_IDX_5] = COMPO_FRAME_TEXT_TYPE_2,
    [SCENE_IDX_6] = COMPO_FRAME_TEXT_TYPE_3,
    [SCENE_IDX_7] = COMPO_FRAME_TEXT_TYPE_4,
    [SCENE_IDX_8] = COMPO_FRAME_TEXT_TYPE_2,
};

void Scn1InitFiles(s32 idx);
void Scn1UpdateTime(void);
void Scn1RunFrame(SCENE_INFO *scn);
BOOL Scn1UpdateInput(SCENE_INFO *scn);
void Scn1ReadInput(SCENE_INFO *scn);
void Scn1Adjust(CD_FILE *file, SCENE_INFO *scn, s32 type, BOOL snd, void (*draw)(SCENE_INFO *scn, s32 type), void (*swap)(SCENE_INFO *scn));
void Scn1GameSetup(SCENE_INFO *scn, s32 idx);
void Scn1End(SCENE_INFO *scn, s32 idx);
s32 Scn1GetFrameTextFromId(s32 idx);
void Scn1SetLevelChannel(SCENE_INFO *scn);
void Scn1GetRecordData(RECORD_DATA *data);
BOOL Scn1EndSave(SCENE_INFO *scn, s32 idx, s32 clear, s32 prevclear);
void Scn1InitSceneControl(void);
SCENE_INFO *Scn1UpdateControl(SCENE_INFO *scn);
void Scn1ResetSceneControl(s32 i);
SCENE1_CONTROL_INFO *Scn1GetSceneControlInfo(s32 time, s32 i);
void Scn1SetVisualLineInfo(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
void Scn1SetBez(SCENE_INFO *scn, s32 bezidx);
void Scn1SetFaceSet(SCENE_INFO *scn, FACE_SET *set, s32 i, s32 setnum);
void Scn1SetTeacherAnim(SCENE_INFO *scn, s32 datidx, s32 vdfidx, s32 idletime);
void Scn1SetPaAnim(SCENE_INFO *scn, s32 datidx, s32 vdfidx);
FACE_SET_INFO *Scn1GetFaceSetInfo(SCENE_INFO *scn, s32 i);
void Scn1CheckControlFinish(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
void Scn1UpdateDojoTod(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
void Scn1UpdateTama(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
void Scn1UpdateTeacherAnim(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info, SCENE1_TOD_SET *set);
void Scn1UpdateParaAnim(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info, SCENE1_TOD_SET *set);
void Scn1UpdateFace(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
void Scn1UpdateCamera(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info);
SCENE1_CONTROL_INFO *Scn1UpdateDelayControlInfo(SCENE_INFO *scn);
void Scn1RunChecks(SCENE_INFO *scn);
SCENE_INFO *Scn1CheckControlInterruption(SCENE_INFO *scn);
void Scn1SetInvalidInputNin(SCENE_INFO *scn);
SCENE1_CONTROL_LIST *Scn1GetSceneControl(s32 i);
SCENE1_CONTROL_INFO *Scn1GetSceneControlDelayInfo(s32 time, s32 i);
BOOL Scn1SceneControlEnded(s32 i);
void Scn1CheckTeacherLine(SCENE_INFO *scn);
void Scn1CheckParaLine(SCENE_INFO *scn);
void Scn1SetOnTod(SCENE_INFO *scn, s32 todidx);
void Scn1SetPaTod(SCENE_INFO *scn, s32 todidx);
void Scn1CheckParaIdle(SCENE_INFO *scn);
void Scn1CheckTeacherIdle(SCENE_INFO *scn);
void Scn1RunHighTodAnim(SCENE_INFO *scn);
void Scn1InitMovieText(void);
void Scn1InitData(void);
void Scn1GameDraw(SCENE_INFO *scn, s32 type);
void Scn1GameSwap(SCENE_INFO *scn);
void Scn1DrawDojoHigh(s32 frame);
void Scn1DrawSky(void);
void Scn1PlayInvalidInputAnim(void);
void Scn1PlayNinTenAnim(void);
void Scn1PlayNinLrAnim(void);
void Scn1PlayNinShowerAnim(void);



void Scn1Init(VDATA_SCENE *vscene, s32 idx) {
    u32 unused;
    s32 sp14;

    MemoryClear();
    Scn1InitSceneControl();
    EventReset();
    ActionInit();
    D_801DF2FC = vscene;

    sp14 = D_801DF2FC->tracklen * 96;
    D_801DF2FC->timeoffs = D_801DF2FC->trackoffs + D_801DF2FC->tracksmalloffs;
    D_801DF2FC->tempo = (D_801DF2FC->tracklen * 96) / 100;
    D_801DF2FC->smalloffs = (((D_801DF2FC->tracklen * 96) / (60 * 60)) + 50) / 100;
    func_8002fa2c();
    Scn1InitFiles(idx);
}

BOOL Scn1MovieUpdateTime(CD_FILE *file, SCENE_INFO *scn) {
    s32 sectorcurrent;
    s32 sp14;
    u32 unused;

    sectorcurrent = CDGetCurrentSector(file->pos);
    dbgInfo.seccur = sectorcurrent;
    dbgInfo.secfirst = MIN(dbgInfo.secfirst, sectorcurrent);
    dbgInfo.seclast = MAX(dbgInfo.seclast, sectorcurrent);
    if (sectorcurrent < 0) {
        return TRUE;
    }

    if (((sectorcurrent - D_801DF300) > 0) ? ((sectorcurrent - D_801DF300) > 300) : ((D_801DF300 - sectorcurrent) > 300)) {
        return TRUE;
    }

    D_801DF300 = sectorcurrent;
    sp14 = sectorcurrent / 5;
    scn->gametimet.min = sp14 / 1800;
    scn->gametimet.sec = (sp14 % 1800) / 30;
    scn->gametimet.frame = sp14 % 30;
    return (CDFileEnded(file) != TRUE) ? TRUE : FALSE;
}

BOOL Scn1MovieRunFrame(CD_FILE *file, SCENE_INFO *scn) {
    BOOL run;

    VSync(4);
    dbgInfo.runframe += 4;
    CDGetCurLoc();
    CdControlF(CdlGetlocP, NULL);
    run = Scn1MovieUpdateTime(file, scn);
    return run;
}

void Scn1VSyncCallback(void) {
    RapFlush();
    Scn1UpdateTime();
}

void Scn1StrmFileInit(CD_FILE *file, s32 type) {
    dbgInfo.vsyncstart = VSync(-1);
    dbgInfo.pos = file->pos;
    dbgInfo.end = file->end;
    dbgInfo.unk10 = 0;
    RapStopSeq();
    Scn1InitMovieText();
    EventReset();
    D_801DF308 = (type != STRM_TYPE_1) ? TRUE : FALSE;
    CDSetVol(file->vol);
    StrmInit(type);
    CDStartStream(file, TRUE, TRUE);
    StrmKick();
}

BOOL Scn1MoviePlay(CD_FILE *file, SCENE_INFO *scn, s32 subsel) {
    u32 pad;
    u32 unused;
    BOOL ret;
    BOOL playing;
    u32 unused1;

    dbgInfo.fname = file->fname;
    dbgInfo.vsyncstart = VSync(-1);
    dbgInfo.pos = file->pos;
    dbgInfo.end = file->end;
    dbgInfo.unk10 = 0;

    if ((subsel >= 0) && (subsel <= 3)) {
        EventSetMovieSubtitles(&sceneInitInfo.moviesub[subsel]);
    } else {
        EventSetMovieSubtitles(NULL);
    }
    D_801DF300 = 0;
    Scn1Adjust(file, scn, PR_MOVIE_12, FALSE, CompoFrameMovieGuiDraw, CompoFrameMovieGuiSwap);
    scn->drawtextlang = appInfo.lang;

    ret = FALSE;
    playing = TRUE;
    while (playing == TRUE) {
        scn->flags = 0;
        pad = PadRead(1);
        if (pad != 0) {
            if (pad == PR_PAD_SELECT) {
                ret = TRUE;
                break;
            } else if ((pad == PR_PAD_CIRCLE) || (pad == PR_PAD_CROSS)) {
                break;
            }
        }
        EventUpdateText(scn);
        StrmMain();
        if (D_801DF308 == TRUE) {
            CompoFrameMovieGuiDraw(scn, PR_MOVIE_12);
        }
        playing = Scn1MovieRunFrame(file, scn);
        StrmDraw();
        if (D_801DF308 == TRUE) {
            CompoFrameMovieGuiSwap(scn);
        }
    }

    StrmEnd();
    CDToggleVol(TRUE);
    CDStop();
    EventCheckTextOverride(scn);
    CompoFlip(TRUE);
    return ret;
}

void Scn1GameInit(CD_FILE *file, SCENE_INFO *scn) {
    D_801DF300 = 0;
    RapStopSeq();
    Scn1InitData();
    CDSetVol(file->vol);
    CDSetFilter(file->chan);
    ActionInit();
    EventReset();
    EventGetDispScore(scn);
    Scn1InitSceneControl();
}

s32 Scn1Game(CD_FILE *file, SCENE_INFO *scn, s32 idx) {
    BOOL running;
    s32 i;
    s32 sp20;
    s32 clear;

    Scn1GameInit(file, scn);
    Scn1GameSetup(scn, idx);
    dbgInfo.tdlargest = -1;
    dbgInfo.tdsmallest = 100;
    DbgReset();
    dbgInfo.fname = file->fname;
    dbgInfo.vsyncstart = VSync(-1);
    dbgInfo.pos = file->pos;
    dbgInfo.end = file->end;
    dbgInfo.startscore = ActionGetCurScore(scn);
    Scn1Adjust(file, scn, 12, TRUE, Scn1GameDraw, Scn1GameSwap);
    VSyncCallback(Scn1VSyncCallback);
    D_801DF2F8 = scn->gametime;
    scn->unk60 = 1;
    scn->ingame = TRUE;
    sp20 = 1;
    D_801D5D9C = 0;
    
    for (running = TRUE; running == TRUE; running = (CDFileEnded(file) != TRUE) ? TRUE : FALSE) {
        Scn1GameDraw(scn, 12);
        scn->flags = 0;
        Scn1ReadInput(scn);
        EventUpdate(scn);
        Scn1UpdateControl(scn);
        if (Scn1UpdateInput(scn) == FALSE) {
            sp20 = 3;
            break;
        }
        Scn1RunFrame(scn);
        Scn1GameSwap(scn);
    }

    scn->ingame = FALSE;
    scn->drawtext = FALSE;
    scn->drawbuttons = FALSE;
    for (i = 4; i > 0; i--) {
        Scn1GameDraw(scn, 12);
        VSync(2);
        Scn1GameSwap(scn);
    }
    CompoFlip(TRUE);
    RapStopSeq();
    VSyncCallback(NULL);
    CDStop();

    if (scn->gameover == TRUE) {
        return 2;
    } else if (scn->cancontinue == TRUE) {
        clear = AppGetStageClear(idx);
        if (appInfo.difficulty == TRUE) {
            return 1;
        } else if (clear <= STAGECLEAR_1) {
            return 1;
        } else if (scn->level == LEVEL_HIGH) {
            return 1;
        } else {
            return 2;
        }
    } else {
        return 3;
    }
}

s32 Scn1Main(s32 idx) {
    SCENE_INFO *scn;
    long tryagain;
    BOOL runret;
    s32 sp1C;

    scn = &sceneInfo;
    Scn1InitMovieText();
    if (appInfo.testmode == TRUE) {
        MenuRunnerProcess(PR_MENU_DEBUG, NULL);
        MenuUpdateDebugOptions(scn);
    }
    RapStopSeq();
    if ((appInfo.playmode != PLAY_MODE_2) && (appInfo.playmode != PLAY_MODE_1)) {
        CompoFrameMovieGuiMake(scn, PR_FRAME_9, COMPO_FRAME_TYPE_3, COMPO_FRAME_TYPE_1);
        Scn1StrmFileInit(&D_801DF2FC->files[VDATA_SCENE_FILE_MOVIE], STRM_TYPE_0);
        runret = Scn1MoviePlay(&D_801DF2FC->files[VDATA_SCENE_FILE_MOVIE], scn, 0);
        if (runret == TRUE) {
            RapStopSeq();
            CompoFrameMovieGuiMake(scn, PR_FRAME_8, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
            CompoFrameResetInactive();
            appInfo.frametext = COMPO_FRAME_TEXT_TYPE_3;
            return -1;
        }
        RapStopSeq();
        CompoFrameMovieGuiMake(scn, PR_FRAME_8, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
    }

    for (sp1C = 0; ; CompoFrameResetInactive(), sp1C++) {
        CDStartStream(&D_801DF2FC->files[VDATA_SCENE_FILE_XA], FALSE, FALSE);
        runret = Scn1Game(&D_801DF2FC->files[VDATA_SCENE_FILE_XA], scn, idx);
        if (appInfo.playmode == PLAY_MODE_1) {
            appInfo.playmode = PLAY_MODE_0;
            appInfo.difficulty = D_801DF304;
            CompoFrameMake(scn, PR_FRAME_4, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
            CompoFrameResetInactive();
            return 0;
        } else if (appInfo.playmode == PLAY_MODE_2) {
            AppReplaySaveDataLoad(&D_800A2174.save);
            appInfo.playmode = PLAY_MODE_0;
            CompoFrameMake(scn, PR_FRAME_4, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
            CompoFrameResetInactive();
            appInfo.frametext = COMPO_FRAME_TEXT_TYPE_3;
            appInfo.difficulty = D_801DF304;
            return -1;
        }
        
        if (runret == 1) {
            CompoFrameMake(scn, PR_FRAME_4, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
            Scn1End(scn, idx);
            CompoFrameResetInactive();
            if (appInfo.difficulty == TRUE) {
                return (idx < SCENE_IDX_3) ? idx + 1 : 0;
            } else {
                return (idx < SCENE_IDX_6) ? idx + 1 : 0;
            }
            continue;
        }

        if ((tryagain = MenuRunnerProcess(PR_MENU_START, NULL)) == 2) {
            CompoFrameMake(scn, PR_FRAME_4, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
            CompoFrameResetInactive();
            appInfo.frametext = COMPO_FRAME_TEXT_TYPE_2;
            return -1;
        }
    }
    return idx;
}

void Scn1InitFiles(register s32 idx) {
    s32 i;
    s32 frametext;

    for (i = 0; i < 7; i++) {
        CDFileSearch(&D_801DF2FC->files[i]);
    }
    MemoryInit();
    frametext = Scn1GetFrameTextFromId(idx);
    CompoFrameResetInactive();
    AppReadCompo(idx, frametext, TRUE);
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateTime);
#else
void Scn1UpdateTime(void) {
    CD_FILE *xafile;
    SCENE_INFO *scn;
    s32 sectorcurrent;

    xafile = &D_801DF2FC->files[VDATA_SCENE_FILE_XA];
    scn = &sceneInfo;
    sectorcurrent = CDGetCurrentStreamSector(xafile->pos);
    dbgInfo.strmdiff = sectorcurrent - D_801DF300;
    dbgInfo.seccur = MIN(sectorcurrent, dbgInfo.seccur);

    if (sectorcurrent > D_801DF300) {
        // XA stream advanced. Update
        // NOT MATCHING: Double constant loads use addiu instead of ori for li expansion
        scn->gametime = 
        ((s32)((((double)(D_801DF2FC->tempo * sectorcurrent)) / (60.0 * 75.0)) + 0.5))
        + D_801DF2FC->timeoffs;
    } else {
        // XA stream did not advance. Update by a small margin
        scn->gametime += D_801DF2FC->smalloffs;
    }

    D_801DF300 = sectorcurrent;
    // TODO: Possible macro?
    (&scn->gametimet)->min = (scn->gametime / (96 * 4)) + 1;
    (&scn->gametimet)->sec = ((scn->gametime % (96 * 4)) / 96) + 1;
    (&scn->gametimet)->frame = ((scn->gametime % (96 * 4)) % 96) + 1;
}
#endif

void Scn1RunFrame(register SCENE_INFO *scn) {
    ActionUpdateVcount(scn, D_801DF2FC->smalloffs * 2);
    VSync(2);
    dbgInfo.runframe += 2;
}

BOOL Scn1UpdateInput(register SCENE_INFO *scn) {
    s32 actionret;
    u32 pad;
    BOOL running;

    running = TRUE;
    pad = scn->pad & (PR_PAD_L2 | PR_PAD_R2 | PR_PAD_L1 | PR_PAD_R1 |
        PR_PAD_TRIANGLE | PR_PAD_CIRCLE | PR_PAD_CROSS | PR_PAD_SQUARE |
        PR_PAD_SELECT | PR_PAD_START);

    if (((scn->flags & PR_SCN_FLAG_1) != 0) && (pad != D_801D5D9C)) {
        if (scn->notplayer == FALSE) {
            scn->prs = scn->gametimet;
            scn->taptime = scn->gametime;
        }
        actionret = ActionUpdateInput(scn);
        if (actionret == 0) {
            sceneInitInfo.onkeypressed(scn, 1);
        }
        sceneInitInfo.anykeypressed(scn);
        dbgInfo.key = actionret;
        if ((appInfo.scorecheat == TRUE) && ((scn->pad & PR_PAD_START) != 0)) {
            scn->unk3C = 1;
            ActionAddScore(250);
        } else if (scn->pad == PR_PAD_SELECT) {
            running = FALSE;
        } else if (scn->pad == (PR_PAD_L1 | PR_PAD_R1)) {

        }
    }

    if ((scn->flags & PR_SCN_FLAG_200) != 0) {
        Scn1SetLevelChannel(scn);
    } else if ((scn->flags & PR_SCN_FLAG_40) != 0) {
        RapPlayKey(sceneInitInfo.endlow);
        CDToggleVol(TRUE);
    } else if ((scn->flags & PR_SCN_FLAG_100) != 0) {
        running = FALSE;
    }

    D_801D5D9C = pad;
    return running;
}

void Scn1ReadInput(register SCENE_INFO *scn) {
    u32 pad;
    u32 unused;
    RECORD_DATA data;

    if (scn->notplayer == TRUE) {
        if (Scn1RecordDataOn(scn->gametime) == TRUE) {
            Scn1GetRecordData(&data);
            scn->pad = Scn1RemapLR(data.pad);
            scn->keyid = EventGetKeyIndexFromPad(scn->pad);
            scn->flags |= PR_SCN_FLAG_1;
            scn->taptime = data.time;
            scn->prs.min = (data.time / (96 * 4)) + 1;
            scn->prs.sec = ((data.time % (96 * 4)) / 96) + 1;
            scn->prs.frame = ((data.time % (96 * 4)) % 96) + 1;
        } else {
            pad = PadRead(1);
            if ((pad != 0) && (pad == PR_PAD_SELECT)) {
                scn->pad = Scn1RemapLR(pad);
                scn->keyid = EventGetKeyIndexFromPad(scn->pad);
                scn->flags |= PR_SCN_FLAG_1;
            } else {
                scn->pad = 0;
            }
        }
    } else {
        if ((scn->pad = Scn1RemapLR(PadRead(1))) != 0) {
            scn->keyid = EventGetKeyIndexFromPad(scn->pad);
            scn->flags |= PR_SCN_FLAG_1;
        } else {
            scn->keyid = PR_TAP_NONE;
        }
    }
}

void Scn1Adjust(register CD_FILE *file, register SCENE_INFO *scn, register s32 type, register BOOL snd,
    register void (*draw)(SCENE_INFO *scn, s32 type),
    register void (*swap)(SCENE_INFO *scn)) {
    s32 sectorcurrent;
    s32 i;

    D_801DF300 = 0;
    for (i = 0; (i < 1800); i++) {
        sectorcurrent = CDGetCurrentStreamSector(file->pos);
        if ((sectorcurrent >= 0) && (sectorcurrent < 150)) {
            break;
        }

        D_801DF300 = sectorcurrent;
        draw(scn, type);
        if ((snd == TRUE) && (CompoFrameFinished() != FALSE)) {
            RapPlayFrameActive(30);
        }
        VSync(2);
        swap(scn);
    }
    dbgInfo.seccur = sectorcurrent;
    scn->gametime = ((D_801DF2FC->tempo * sectorcurrent) / (60 * 75)) + D_801DF2FC->timeoffs;
}

void Scn1GameSetup(register SCENE_INFO *scn, register s32 idx) {
    BOOL cleared;

    if (appInfo.playmode == PLAY_MODE_1) {
        D_801DF304 = appInfo.difficulty;
        appInfo.difficulty = FALSE;
        EventRecordInit(FALSE);
        scn->notplayer = TRUE;
        sceneInitInfo.demoinit();
        ActionSetHighEnabled(FALSE);
        EventSetFlashHigh(FALSE);
    } else if (appInfo.playmode == PLAY_MODE_2) {
        D_801DF304 = appInfo.difficulty;
        appInfo.difficulty = FALSE;
        EventRecordInit(FALSE);
        scn->notplayer = TRUE;
        AppGetReplayRecordData();
        cleared = AppPrevStageCleared(idx);
        ActionSetHighEnabled(cleared);
        EventSetFlashHigh(cleared);
    } else {
        scn->notplayer = FALSE;
        EventRecordInit(scn->notplayer);
        cleared = AppStageCleared(idx);
        ActionSetHighEnabled(cleared);
        EventSetFlashHigh(cleared);
    }
    scn->drawtextlang = appInfo.lang;
    scn->leniency = (appInfo.difficulty == FALSE) ? 8 : (8 * 3);
    if ((scn->dbgmsg = appInfo.dbgmsg) == TRUE) {
        scn->clicksound = TRUE;
    }
    if (appInfo.difficulty == TRUE) {
        ActionSetHighEnabled(FALSE);
        EventSetFlashHigh(FALSE);
    }
}

void Scn1End(register SCENE_INFO *scn, register s32 idx) {
    CD_FILE *strfile;
    s32 moviesub;
    s32 clear;
    s32 prevclear;

    prevclear = AppGetStageClear(idx);
    if (scn->level == LEVEL_HIGH) {
        strfile = &D_801DF2FC->files[VDATA_SCENE_FILE_XMOVIE];
        moviesub = 1;
        clear = STAGECLEAR_3;
    } else if (scn->level == LEVEL_NORM) {
        strfile = &D_801DF2FC->files[VDATA_SCENE_FILE_YMOVIE];
        moviesub = 2;
        clear = STAGECLEAR_2;
    }
    Scn1StrmFileInit(strfile, STRM_TYPE_0);

    RapStopSeq();
    CompoFrameMovieGuiMake(scn, PR_FRAME_9, COMPO_FRAME_TYPE_3, COMPO_FRAME_TYPE_1);
    Scn1MoviePlay(strfile, scn, moviesub);

    RapStopSeq();
    CompoFrameMovieGuiMake(scn, PR_FRAME_8, COMPO_FRAME_TYPE_1, COMPO_FRAME_TYPE_3);
    CompoFrameResetInactive();
    RapPlayKey(sceneInitInfo.intermission);
    RapFlush();
    if (appInfo.difficulty == FALSE) {
        Scn1EndSave(scn, idx, clear, prevclear);
    }
}

s32 Scn1GetFrameTextFromId(register s32 idx) {
    return D_801D5DA0[idx];
}

void Scn1SetLevelChannel(register SCENE_INFO *scn) {
    s32 chan;
    
    chan = D_80068A3C[scn->level].channel;
    CDSetFilter(chan);
}

BOOL Scn1RecordDataOn(register s32 time) {
    if (eventRecordInfo.idx >= eventRecordInfo.num) {
        return FALSE;
    } else {
        time += 3;
        return (eventRecordInfo.data[eventRecordInfo.idx].time <= time) ? TRUE : FALSE;
    }
}

void Scn1GetRecordData(register RECORD_DATA *data) {
    s32 i;

    i = eventRecordInfo.idx;
    eventRecordInfo.idx++;
    *data = eventRecordInfo.data[i];
    return;
}

u32 Scn1RemapLR(register u32 pad) {
    if (pad & PR_PAD_L2) {
        return (pad & ~PR_PAD_L2) | PR_PAD_L1;
    }
    if (pad & PR_PAD_R2) {
        return (pad & ~PR_PAD_R2) | PR_PAD_R1;
    } else {
        return pad;
    }
}

BOOL Scn1EndSave(register SCENE_INFO *scn, register s32 idx, register s32 clear, register s32 prevclear) {
    AppSaveStageStats(idx, clear, prevclear, ActionGetCurScore(scn));
    if (idx < SCENE_IDX_6) {
        AppUnlockStage(idx + 1);
    }
    if (appInfo.nosave == TRUE) {
        return TRUE;
    }
    AppReadZCompo(idx);
    return CardMenuSave(&D_800A2174);
}

void Scn1SceneControlDelayStart(SCENE_INFO *scn, s32 i) {
    Scn1ResetSceneControl(i);
    D_801DF30C = scn->gametime;
}

void Scn1InitSceneControl(void) {
    s32 i;

    for (i = 0; i < 9; i++) {
        scn1SceneControlList[i].cur = 0;
    }
}

void Scn1SetText(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info) {
    if (info->jpsel != 0) {
        eventInfo.jptext = scn1StageTextJp[info->jpsel];
        eventInfo.jptexttimer = 120;
    }
    scn->jptext = eventInfo.jptext;

    if (info->ensel != 0) {
        eventInfo.entext = scn1StageTextEn[info->ensel];
        eventInfo.entexttimer = 120;
    }
    scn->entext = eventInfo.entext;
}

void Scn1FnDemoInit(void) {
    s32 i;

    for (i = 0; i < scn1DemoInputLen; i++) {
        eventRecordInfo.data[i].time = scn1DemoInput[i].time * ((96 / 16) * 4);
        eventRecordInfo.data[i].pad = EventGetPadFromKeyIndex(scn1DemoInput[i].keyid);
    }
    eventRecordInfo.num = scn1DemoInputLen;
}

void Scn1SetOnWaitAnimDefault(register SCENE_INFO *scn) {
    s32 i;

    i = D_801DAB58;
    Scn1SetTeacherAnim(scn, D_801DAA9C[i].datidx, D_801DAA9C[i].vdfidx, 33);
}

void Scn1SetPaWaitAnimDefault(register SCENE_INFO *scn) {
    Scn1SetPaAnim(scn, D_801DAA9C[D_801DAB54].datidx, D_801DAA9C[D_801DAB54].vdfidx);
}

void Scn1CheckFace(SCENE_INFO *scn, s32 i) {
    FACE_SET_INFO *setinfo;

    setinfo = Scn1GetFaceSetInfo(scn, i);
    if (setinfo == NULL) {
        return;
    }
    
    if (i == 0) {
        scn->flags |= PR_SCN_FLAG_8000;
        scn->parappaface = setinfo->ids;
    } else if (i == 1) {
        scn->flags |= PR_SCN_FLAG_10000;
        scn->teacherface = setinfo->ids;
    } else if (i == 2) {
        scn->flags |= PR_SCN_FLAG_800000;
        scn->unkB4 = setinfo->ids;
    }
}

void Scn1UpdateControlAnim(SCENE_INFO *scn, SCENE1_CONTROL_INFO *info, s32 setidx) {
    SCENE1_TOD_SET *set;

    set = &scn1TodSet[setidx];
    if ((info->flags & SCENE1_CTRL_FLAG_20) != 0) {
        scn->flags |= PR_SCN_FLAG_20;
    }
    Scn1UpdateParaAnim(scn, info, set);
    Scn1UpdateTeacherAnim(scn, info, set);
    Scn1UpdateTama(scn, info);
    Scn1UpdateDojoTod(scn, info);
    Scn1UpdateCamera(scn, info);
    Scn1UpdateFace(scn, info);
}

SCENE_INFO *Scn1UpdateControl(SCENE_INFO *scn) {
    SCENE1_CONTROL_INFO *maininfo;
    SCENE1_CONTROL_INFO *endinfo;

    endinfo = NULL;
    maininfo = NULL;
    Scn1RunChecks(scn);
    if (Scn1CheckControlInterruption(scn) == NULL) {
        return scn;
    }
    if (eventInfo.controlstage != 0) {
        maininfo = Scn1GetSceneControlInfo(scn->gametime, eventInfo.controlstage);
    }
    if ((eventInfo.controlstage != 0) && (maininfo != NULL)) {
        Scn1SetVisualLineInfo(scn, maininfo);
        Scn1CheckControlFinish(scn, maininfo);
    }
    if (eventInfo.controldelay != 0) {
        endinfo = Scn1UpdateDelayControlInfo(scn);
        if (endinfo != NULL) {
            Scn1UpdateControlAnim(scn, endinfo, eventInfo.controldelay);
            Scn1SetText(scn, endinfo);
        }
    } else {
        if ((eventInfo.controlstage != 0) && (maininfo != NULL)) {
            Scn1UpdateControlAnim(scn, maininfo, eventInfo.controlstage);
            if (scn->level != LEVEL_HIGH) {
                Scn1SetText(scn, maininfo);
            }
        }
    }
    return scn;
}

void Scn1FnKeyPressed(SCENE_INFO *scn, s32 keyid) {
    s32 fxidx;

    fxidx = keyid;
    scn->flags |= PR_SCN_FLAG_10;
    scn->padat = memoryPos[scn1InputEffectList[fxidx].datidx];
    scn->pavdf = memoryPos[scn1InputEffectList[fxidx].vdfidx];
    scn->paframe = scn1InputEffectList[fxidx].frame;
    eventInfo.paraidletimer = scn->paframenum = scn1InputEffectList[fxidx].time;
    Scn1SetFaceSet(scn, scn1FaceSet, 0, scn1InputEffectList[fxidx].faceset);
    dbgInfo.paradatid = scn1InputEffectList[fxidx].datidx;
    dbgInfo.paravdfid = scn1InputEffectList[fxidx].vdfidx;
}

void Scn1FnOnKeyPressed(SCENE_INFO *scn, s32 arg1) {
    u32 parappapad;
    u32 teacherpad;

    if (scn->level == LEVEL_HIGH) {
        return;
    }
    parappapad = (scn->parappainp == NULL) ? NULL : scn->parappainp->validpad;
    teacherpad = (scn->teacherinp == NULL) ? NULL : scn->teacherinp->validpad;
    if (((scn->pad & parappapad) != 0) || ((scn->pad & teacherpad) != 0)) {
        return;
    }
    Scn1SetInvalidInputNin(scn);
}

void Scn1FnPerfUp(SCENE_INFO *scn) {

}

void Scn1FnAnyKeyPressed(SCENE_INFO *scn) {
    u32 unused;
    PARA_TIME *gt;
    s32 infoid;

    gt = &scn->gametimet;
    infoid = scn->parappainp->sub[scn->lvlhigh].infoid;
    if ((scn->pad & PR_PAD_UP) == 0) {
        return;
    }
    if (infoid == 0) {
        return;
    }
    if (D_801DF31C > 0) {
        return;
    }
    if (scn->level == LEVEL_HIGH) {
        return;
    }

    if ((gt->sec == 4) && ((gt->min == 37) || (gt->min == 42))) {
        scn->nintype = 2;
        Scn1SetBez(scn, COMPO01_CM_TEN_BEZ);
        D_801DF31C = 132;
    } else if ((gt->sec == 2) && ((scn->pad & (PR_PAD_R1 | PR_PAD_R2)) != 0)) {
        scn->nintype = 3;
        Scn1SetBez(scn, COMPO01_CM_SHJ_BEZ);
        D_801DF31C = 132 / 2;
    } else if ((gt->sec == 4) && ((scn->pad & PR_PAD_CROSS) != 0)) {
        scn->nintype = 4;
        D_801DF31C = 132;
    } else {
        scn->nintype = 0;
        return;
    }
    scn->flags |= PR_SCN_FLAG_1000000;
}

void Scn1SetData(void) {
    sceneInitInfo.times = &scn1EighthTimes;
    
    sceneInitInfo.inpinfo = scn1InputInfoSet;
    sceneInitInfo.inpinfolen = 66;

    sceneInitInfo.moviesub = scn1MovieSubtitles;

    sceneInitInfo.visline = scn1VisualLineInfo;
    sceneInitInfo.vislinelen = scn1VisualLineInfoLen;

    sceneInitInfo.keyinfo = scn1InputKeyInfo;
    sceneInitInfo.keyinfolen = scn1InputKeyInfoLen;

    sceneInitInfo.keyctrl = scn1KeyCtrl;

    sceneInitInfo.demoinp = scn1DemoInput;
    sceneInitInfo.demoinplen = scn1DemoInputLen;

    sceneInitInfo.endlow = &scn1SndEndLow;
    sceneInitInfo.hightonorm = &scn1SndHighToNormal;
    sceneInitInfo.tohighfirst = &scn1SndToHighFirst;
    sceneInitInfo.tohigh = &scn1SndToHighRest;
    sceneInitInfo.lvlchange = &scn1SndLvlChange;
    sceneInitInfo.scorechange = &scn1SndScoreChange;
    sceneInitInfo.click = &scn1SndClick;
    sceneInitInfo.dbgstackcheck = &scn1SndDbgStackCheck;
    sceneInitInfo.unk4C = &D_801D5E2C;
    sceneInitInfo.intermission = &scn1SndIntermission;
    sceneInitInfo.unk54 = &D_801D5E3C;
    sceneInitInfo.unk58 = &D_801D5E44;
    sceneInitInfo.tryagain = scn1SndTryAgain;
    sceneInitInfo.menudpad = &scn1SndMenuDpad;
    sceneInitInfo.menucross = &scn1SndMenuCross;
    sceneInitInfo.menucircle = &scn1SndMenuCircle;
    sceneInitInfo.menuselect = &scn1SndMenuSelect;

    sceneInitInfo.keypressed = Scn1FnKeyPressed;
    sceneInitInfo.onkeypressed = Scn1FnOnKeyPressed;
    sceneInitInfo.perfup = Scn1FnPerfUp;
    sceneInitInfo.anykeypressed = Scn1FnAnyKeyPressed;
    sceneInitInfo.demoinit = Scn1FnDemoInit;
}

void Scn1ResetSceneControl(register s32 i) {
    SCENE1_CONTROL_LIST *control;

    control = Scn1GetSceneControl(i);
    if (control != NULL) {
        control->cur = 0;
    }
}

SCENE1_CONTROL_INFO *Scn1GetSceneControlInfo(register s32 time, register s32 i) {
    SCENE1_CONTROL_LIST *control;
    SCENE1_CONTROL_INFO *baseinfo;
    SCENE1_CONTROL_INFO *info;

    control = Scn1GetSceneControl(i);
    if (control == NULL) {
        return NULL;
    }
    if (control->cur >= control->max) {
        return NULL;
    }
    baseinfo = control->info;
    info = &baseinfo[control->cur];
    if (info->starttime > time) {
        return NULL;
    }
    control->cur++;
    return info;
}

void Scn1SetVisualLineInfo(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    s32 linenum;

    if ((info->flags & SCENE1_CTRL_FLAG_800) != 0) {
        scn->flags |= PR_SCN_FLAG_800;

        linenum = scn1VisualLineInfo[info->visfirst].linenum;
        if (linenum > 0) {
            scn->teacherline.num = scn1VisualLineInfo[info->visfirst].linenum;
            scn->teacherline.dot[0] = scn1VisualLineInfo[info->visfirst].start;
            scn->teacherline.dot[1] = -1;
            scn->teacherline.enabled = 1;
            scn->teacherline.keyid[0] = scn1VisualLineInfo[info->visfirst].first;
            scn->teacherline.keyid[1] = scn1VisualLineInfo[info->vissecond].second;
            D_801DF310 = scn->teacherline.keyid[0];
            D_801DF314 = scn->teacherline.keyid[1];
            scn->teacherline.enabled = 1;
        } else {
            scn->teacherline.num = -linenum;
            scn->teacherline.keyid[0] = scn1VisualLineInfo[info->visfirst].first;
            scn->teacherline.keyid[1] = scn1VisualLineInfo[info->vissecond].second;
            D_801DF310 = scn->teacherline.keyid[0];
            D_801DF314 = scn->teacherline.keyid[1];
            scn->teacherline.dot[0] = -1;
            scn->teacherline.dot[1] = -1;
            scn->teacherline.enabled = 0;
        }
        if (scn->teacherline.keyid[0][0] == PR_TAP_UNK) {
            scn->parappaline.dot[0] = -1;
            scn->parappaline.dot[1] = -1;
            scn->parappaline.enabled = 0;
        }
        if (scn->teacherline.num == 0) {
            scn->teacherline.dot[0] = -1;
            scn->teacherline.dot[1] = -1;
            scn->teacherline.enabled = 0;
            scn->parappaline.dot[0] = -1;
            scn->parappaline.dot[1] = -1;
            scn->parappaline.enabled = 0;
        }
        Scn1CheckTeacherLine(scn);
    } else if (((info->flags & SCENE1_CTRL_FLAG_1000) != 0) || ((scn->flags & PR_SCN_FLAG_2000) != 0)) {
        Scn1ResetVisualLineInfo(scn);
    }

    if ((info->flags & SCENE1_CTRL_FLAG_200000) != 0) {
        scn->flags |= PR_SCN_FLAG_200000;
        scn->parappaline.enabled = 1;
        scn->parappaline.dot[0] = 0;
        scn->parappaline.dot[1] = -1;
        scn->parappaline.keyid[0] = D_801DF310;
        scn->parappaline.keyid[1] = D_801DF314;
        linenum = scn1VisualLineInfo[info->visfirst].linenum;
        if (linenum < 0) {
            CompoLineButtonInitScalingPara();
        }
        Scn1CheckParaLine(scn);
    }
}

void Scn1SetBez(register SCENE_INFO *scn, register s32 bezidx) {
    scn->flags |= PR_SCN_FLAG_400;
    scn->nextbez = memoryPos[bezidx];
    dbgInfo.bezid = bezidx;
}

void Scn1SetFaceSet(register SCENE_INFO *scn, register FACE_SET *set, register s32 i, register s32 setnum) {
    FACE_SET_USE *eventset;

    eventset = &eventInfo.faceset[i];
    if (setnum == 0) {
        return;
    }
    eventset->starttime = scn->gametime;
    eventset->num = setnum;
    eventset->set = &set[setnum];
    eventset->set->cur = 0;
    if (i == 0) {
        dbgInfo.paraface = setnum;
    }
}

void Scn1SetTeacherAnim(register SCENE_INFO *scn, register s32 datidx, register s32 vdfidx, register s32 idletime) {
    if ((datidx == 0) || (vdfidx == 0)) {
        return;
    }

    scn->flags |= PR_SCN_FLAG_40000;
    scn->teacherdat = memoryPos[datidx];
    scn->teachervdf = memoryPos[vdfidx];
    eventInfo.teacheridletimer = idletime;
    dbgInfo.teacherdatid = datidx;
    dbgInfo.teachervdfid = vdfidx;
}

void Scn1SetPaAnim(register SCENE_INFO *scn, register s32 datidx, register s32 vdfidx) {
    if ((datidx == 0) || (vdfidx == 0)) {
        return;
    }

    scn->flags |= PR_SCN_FLAG_20000;
    scn->padat = memoryPos[datidx];
    scn->pavdf = memoryPos[vdfidx];
    dbgInfo.paradatid = datidx;
    dbgInfo.paravdfid = vdfidx;
}

FACE_SET_INFO *Scn1GetFaceSetInfo(register SCENE_INFO *scn, register s32 i) {
    FACE_SET *set;
    FACE_SET_INFO *setinfo;

    if (eventInfo.faceset[i].num == 0) {
        return NULL;
    }

    if ((set = eventInfo.faceset[i].set) == NULL) {
        return NULL;
    }
    if (set->cur >= set->max) {
        eventInfo.faceset[i].set->cur = 0;
        eventInfo.faceset[i].set = NULL;
        eventInfo.faceset[i].num = 0;
        return NULL;
    }
    setinfo = &set->info[set->cur];
    if ((eventInfo.faceset[i].starttime + setinfo->time) > scn->gametime) {
        return NULL;
    }
    set->cur++;
    return setinfo;
}

void Scn1CheckControlFinish(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    if ((info->flags & SCENE1_CTRL_FLAG_80) != 0) {
        eventInfo.finished = TRUE;
        scn->drawtext = TRUE;
    }
}

void Scn1UpdateDojoTod(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    s32 setidx;
    s32 i;
    SCENE1_TOD_SET *set;

    if ((scn->flags & PR_SCN_FLAG_20) != 0) {
        setidx = 1;
        set = &scn1TodSet[setidx];
        for (i = 0; i < 5; i++) {
            scn->dojotod[i] = memoryPos[scn->dojotodidx[i] = set->dojotod[i]];
        }
    } else if ((scn->flags & PR_SCN_FLAG_2000) != 0) {
        setidx = (scn->highcnt < 2) ? 6 : 8;
        set = &scn1TodSet[setidx];
        for (i = 0; i < 5; i++) {
            scn->dojotod[i] = memoryPos[scn->dojotodidx[i] = set->dojotod[i]];
        }
    } else if ((scn->flags & PR_SCN_FLAG_40) != 0) {
        setidx = (eventInfo.unk44 == TRUE) ? 5 : 4;
        set = &scn1TodSet[setidx];
        for (i = 0; i < 5; i++) {
            scn->dojotod[i] = memoryPos[scn->dojotodidx[i] = set->dojotod[i]];
        }
    } else if ((scn->flags & PR_SCN_FLAG_4000) != 0) {
        setidx = 7;
        set = &scn1TodSet[setidx];
        for (i = 0; i < 5; i++) {
            scn->dojotod[i] = memoryPos[scn->dojotodidx[i] = set->dojotod[i]];
        }
    }
}

void Scn1UpdateTama(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    scn->tamasel = info->tamasel;
}

void Scn1UpdateTeacherAnim(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info, register SCENE1_TOD_SET *set) {
    if ((info->flags & SCENE1_CTRL_FLAG_100000) != 0) {
        Scn1SetOnTod(scn, set->ontod);
    }
    if (D_801DF318 > 0) {
        return;
    }

    if ((info->flags & SCENE1_CTRL_FLAG_40000) != 0) {
        if ((scn->level == LEVEL_HIGH) && (info->teacheranimhigh != 0)) {
            Scn1SetTeacherAnim(scn, D_801DAA9C[info->teacheranimhigh].datidx, D_801DAA9C[info->teacheranimhigh].vdfidx, 132);
        } else if ((scn->level == LEVEL_NORM) && (info->teacheranimnormal != 0)) {
            Scn1SetTeacherAnim(scn, D_801DAA9C[info->teacheranimnormal].datidx, D_801DAA9C[info->teacheranimnormal].vdfidx, 0);
        } else if ((scn->level == LEVEL_LOW1) && (info->teacheranimlow1 != 0)) {
            Scn1SetTeacherAnim(scn, D_801DAA9C[info->teacheranimlow1].datidx, D_801DAA9C[info->teacheranimlow1].vdfidx, 0);
        } else if ((scn->level == LEVEL_LOW2) && (info->teacheranimlow2 != 0)) {
            Scn1SetTeacherAnim(scn, D_801DAA9C[info->teacheranimlow2].datidx, D_801DAA9C[info->teacheranimlow2].vdfidx, 0);
        }
    }
}

void Scn1UpdateParaAnim(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info, register SCENE1_TOD_SET *set) {
    s32 animidx;
    s32 todidx;

    if ((info->flags & SCENE1_CTRL_FLAG_80000) != 0) {
        todidx = (scn->level == LEVEL_HIGH) ? set->patodhigh : set->patodnormal;
        Scn1SetPaTod(scn, todidx);
    }
    if (eventInfo.controldelay != 0) {
        eventInfo.paraidletimer = 0;
    }
    if (eventInfo.paraidletimer > 0) {
        return;
    }
    if ((info->flags & SCENE1_CTRL_FLAG_20000) == 0) {
        return;
    }
    animidx = (scn->level == LEVEL_HIGH) ? info->paraanimhigh : info->paraanimnormal;
    Scn1SetPaAnim(scn, D_801DAA9C[animidx].datidx, D_801DAA9C[animidx].vdfidx);
}

void Scn1UpdateFace(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    s32 setidx;

    if ((info->flags & SCENE1_CTRL_FLAG_8000) != 0) {
        Scn1SetFaceSet(scn, scn1FaceSet, 0, info->paraface);
    }
    if ((info->flags & SCENE1_CTRL_FLAG_10000) != 0) {
        setidx = 0;
        if ((scn->level == LEVEL_HIGH) && (info->teacherfacehigh != 0)) {
            setidx = info->teacherfacehigh;
        } else if ((scn->level == LEVEL_NORM) && (info->teacherfacenormal != 0)) {
            setidx = info->teacherfacenormal;
        } else if ((scn->level == LEVEL_LOW1) && (info->teacherfacelow1 != 0)) {
            setidx = info->teacherfacelow1;
        } else if ((scn->level == LEVEL_LOW2) && (info->teacherfacelow2 != 0)) {
            setidx = info->teacherfacelow2;
        }
        if (setidx != 0) {
            Scn1SetFaceSet(scn, scn1FaceSet, 1, setidx);
        }
    }
        if ((info->flags & SCENE1_CTRL_FLAG_800000) != 0) {
        Scn1SetFaceSet(scn, scn1FaceSet, 2, info->mcface);
    }
}

void Scn1UpdateCamera(register SCENE_INFO *scn, register SCENE1_CONTROL_INFO *info) {
    if ((scn->level == LEVEL_HIGH) && (info->bezhigh > 0)) {
        Scn1SetBez(scn, info->bezhigh);
    } else if ((scn->level == LEVEL_LOW1) && (info->bezlow1 > 0)) {
        Scn1SetBez(scn, info->bezlow1);
    } else if ((scn->level == LEVEL_LOW2) && (info->bezlow2 > 0)) {
        Scn1SetBez(scn, info->bezlow2);
    } else if ((scn->level == LEVEL_NORM) && (info->beznormal > 0)) {
        Scn1SetBez(scn, info->beznormal);
    }
}

SCENE1_CONTROL_INFO *Scn1UpdateDelayControlInfo(register SCENE_INFO *scn) {
    SCENE1_CONTROL_INFO *info;

    info = Scn1GetSceneControlDelayInfo(scn->gametime, eventInfo.controldelay);
    if (info != NULL) {
        return info;
    }

    if (Scn1SceneControlEnded(eventInfo.controldelay) == FALSE) {
        return NULL;
    }

    if ((eventInfo.controldelay == 4) || (eventInfo.controldelay == 5) || (eventInfo.controldelay == 2) || (eventInfo.controldelay == 3)) {
        scn->flags |= PR_SCN_FLAG_100;
    } else if ((eventInfo.controldelay == 6) || (eventInfo.controldelay == 8)) {
        eventInfo.transitionhigh = TRUE;
    }
    Scn1ResetSceneControl(eventInfo.controldelay);
    eventInfo.controldelay = 0;
    return NULL;
}

void Scn1RunChecks(register SCENE_INFO *scn) {
    s32 i;

    Scn1CheckText(scn);
    Scn1CheckParaIdle(scn);
    Scn1CheckTeacherIdle(scn);
    Scn1CheckTeacherLine(scn);
    Scn1CheckParaLine(scn);
    for (i = 0; i < 3; i++) {
        Scn1CheckFace(scn, i);
    }
    if (eventInfo.transitionhigh == TRUE) {
        eventInfo.transitionhigh = FALSE;
        Scn1RunHighTodAnim(scn);
    }
    if (D_801DF318 > 0) {
        D_801DF318--;
    }
    if (D_801DF31C > 0) {
        D_801DF31C--;
    }
}

SCENE_INFO *Scn1CheckControlInterruption(register SCENE_INFO *scn) {
    s32 datidx;
    s32 vdfidx;

    if ((scn->flags & PR_SCN_FLAG_40) != 0) {
        eventInfo.controlstage = 0;
        eventInfo.controldelay = (scn->level == LEVEL_HIGH) ? 5 : 4;
        Scn1SceneControlDelayStart(scn, eventInfo.controldelay);
        eventInfo.gameover = TRUE;
        eventInfo.paraidletimer = 0;
        Scn1ResetVisualLineInfo(scn);
        return NULL;
    } else if (eventInfo.finished == TRUE) {
        eventInfo.finished = FALSE;
        eventInfo.controlstage = 0;
        scn->cancontinue = TRUE;
        if (scn->level == LEVEL_HIGH) {
            eventInfo.controldelay = 3;
        } else if (scn->level == LEVEL_NORM) {
            eventInfo.controldelay = 2;
        } else {
            eventInfo.controldelay = 4;
            scn->gameover = TRUE;
            scn->flags |= PR_SCN_FLAG_40;
        }
        Scn1SceneControlDelayStart(scn, eventInfo.controldelay);
        Scn1ResetVisualLineInfo(scn);
    } else if ((scn->flags & PR_SCN_FLAG_2000) != 0) {
        eventInfo.controldelay = (scn->highcnt < 2) ? 6 : 8;
        Scn1SceneControlDelayStart(scn, eventInfo.controldelay);
        Scn1ResetVisualLineInfo(scn);
    } else if ((scn->flags & PR_SCN_FLAG_4000) != 0) {
        eventInfo.controldelay = 7;
        Scn1SceneControlDelayStart(scn, eventInfo.controldelay);
    }

    if ((scn->flags & PR_SCN_FLAG_200) != 0) {
        if (scn->leveltransition == PR_SCN_LVLTRANS_1) {
            vdfidx = COMPO01_ON_TOLW1_VDF;
            datidx = COMPO01_ON_TOLW1_DAT;
        } else if (scn->leveltransition == PR_SCN_LVLTRANS_2) {
            vdfidx = COMPO01_ON_TOLW2_VDF;
            datidx = COMPO01_ON_TOLW2_DAT;
        } else if (scn->leveltransition == PR_SCN_LVLTRANS_4) {
            vdfidx = COMPO01_ON_TONM1_VDF;
            datidx = COMPO01_ON_TONM1_DAT;
        } else if (scn->leveltransition == PR_SCN_LVLTRANS_3) {
            vdfidx = COMPO01_ON_TONM2_VDF;
            datidx = COMPO01_ON_TONM2_DAT;
        } else {
            vdfidx = 0;
            datidx = 0;
        }
        if (vdfidx != 0) {
            D_801DF318 = 41;
            Scn1SetTeacherAnim(scn, datidx, vdfidx, 33);
        }
    }
    return scn;
}

void Scn1SetInvalidInputNin(register SCENE_INFO *scn) {
    scn->flags |= PR_SCN_FLAG_1000000;
    scn->nintype = 1;
}

SCENE1_CONTROL_LIST *Scn1GetSceneControl(register s32 i) {
    return ((i >= 1) && (i <= 8)) ? &scn1SceneControlList[i] : NULL;
}

SCENE1_CONTROL_INFO *Scn1GetSceneControlDelayInfo(register s32 time, register s32 i) {
    SCENE1_CONTROL_LIST *control;
    SCENE1_CONTROL_INFO *baseinfo;
    SCENE1_CONTROL_INFO *info;

    control = Scn1GetSceneControl(i);
    if (control == NULL) {
        return NULL;
    }
    if (control->cur >= control->max) {
        return NULL;
    }
    baseinfo = control->info;
    info = &baseinfo[control->cur];
    if ((D_801DF30C + info->starttime) > time) {
        return NULL;
    }
    control->cur++;
    return info;
}

BOOL Scn1SceneControlEnded(register s32 i) {
    SCENE1_CONTROL_LIST *control;

    control = Scn1GetSceneControl(i);
    if (control == NULL) {
        return FALSE;
    } else {
        return (control->cur >= control->max) ? TRUE : FALSE;
    }
}

void Scn1CheckTeacherLine(register SCENE_INFO *scn) {
    if ((scn->teacherline.enabled == 0) || ((scn->flags & PR_SCN_FLAG_8) == 0)) {
        return;
    }
    if (scn->teacherline.num == 1) {
        if (scn->teacherline.enabled == 1) {
            if ((scn->teacherline.dot[0] <= 18) && 
                (scn->teacherline.keyid[0][scn->teacherline.dot[0]] != PR_TAP_ICON_END)) {
                scn->teacherline.dot[0]++;
            } else {
                scn->teacherline.enabled = 0;
                scn->teacherline.dot[0] = -1;
                CompoLineButtonInitScalingTeacher();
            }
        }
    } else if (scn->teacherline.num == 2) {
        if (scn->teacherline.enabled == 1) {
            if ((scn->teacherline.dot[0] <= 18) && 
                (scn->teacherline.keyid[0][scn->teacherline.dot[0]] != PR_TAP_ICON_END)) {
                scn->teacherline.dot[0]++;
            } else {
                scn->teacherline.dot[0] = -1;
                scn->teacherline.dot[1] = 2;
                scn->teacherline.enabled = 2;
            }
        }
        if (scn->teacherline.enabled == 2) {
            if ((scn->teacherline.dot[1] <= 18) && 
                (scn->teacherline.keyid[1][scn->teacherline.dot[1]] != PR_TAP_ICON_END)) {
                scn->teacherline.dot[1]++;
            } else {
                scn->teacherline.dot[1] = -1;
                scn->teacherline.enabled = 0;
                CompoLineButtonInitScalingTeacher();
            }
        }
    }
}

void Scn1CheckParaLine(register SCENE_INFO *scn) {
    if ((scn->parappaline.enabled == 0) || ((scn->flags & PR_SCN_FLAG_8) == 0)) {
        return;
    }
    if (scn->teacherline.num == 1) {
        if (scn->parappaline.enabled == 1) {
            if ((scn->parappaline.dot[0] <= 18) && 
                (scn->parappaline.keyid[0][scn->parappaline.dot[0]] != PR_TAP_ICON_END)) {
                scn->parappaline.dot[0]++;
            } else {
                scn->parappaline.enabled = 0;
                scn->parappaline.dot[0] = -1;
                CompoLineButtonInitScalingPara();
            }
        }
    } else if (scn->teacherline.num == 2) {
        if (scn->parappaline.enabled == 1) {
            if ((scn->parappaline.dot[0] <= 18) && 
                (scn->parappaline.keyid[0][scn->parappaline.dot[0]] != PR_TAP_ICON_END)) {
                scn->parappaline.dot[0]++;
            } else {
                scn->parappaline.dot[0] = -1;
                scn->parappaline.dot[1] = 2;
                scn->parappaline.enabled = 2;
            }
        }
        if (scn->parappaline.enabled == 2) {
            if ((scn->parappaline.dot[1] <= 18) && 
                (scn->parappaline.keyid[1][scn->parappaline.dot[1]] != PR_TAP_ICON_END)) {
                scn->parappaline.dot[1]++;
            } else {
                scn->parappaline.dot[1] = -1;
                scn->parappaline.enabled = 0;
                CompoLineButtonInitScalingPara();
            }
        }
    }
}

void Scn1ResetVisualLineInfo(register SCENE_INFO *scn) {
    scn->teacherline.dot[1] = -1;
    scn->teacherline.dot[0] = -1;
    scn->teacherline.keyid[0] = scn1VisualLineInfo[0].first;
    scn->teacherline.keyid[1] = scn1VisualLineInfo[0].second;
    scn->teacherline.num = 0;
    scn->teacherline.enabled = FALSE;

    scn->parappaline.dot[1] = -1;
    scn->parappaline.dot[0] = -1;
    scn->parappaline.keyid[0] = scn1VisualLineInfo[0].first;
    scn->parappaline.keyid[1] = scn1VisualLineInfo[0].second;
    scn->teacherline.num = 0; // BUG?
    scn->parappaline.enabled = FALSE;

    CompoLineButtonInitScalingPara();
    CompoLineButtonInitScalingTeacher();
}

void Scn1CheckText(register SCENE_INFO *scn) {
    if (eventInfo.jptexttimer > 0) {
        if (--eventInfo.jptexttimer == 0) {
            eventInfo.jptext = NULL;
            scn->jptext = NULL;
        }
    }
    if (eventInfo.entexttimer > 0) {
        if (--eventInfo.entexttimer == 0) {
            eventInfo.entext = NULL;
            scn->entext = NULL;
        }
    }
    
}

void Scn1SetOnTod(register SCENE_INFO *scn, register s32 todidx) {
    if (todidx == 0) {
        return;
    }
    scn->flags |= PR_SCN_FLAG_100000;
    scn->teachertod = memoryPos[todidx];
    dbgInfo.teachertodid = todidx;
}

void Scn1SetPaTod(register SCENE_INFO *scn, register s32 todidx) {
    if (todidx == 0) {
        return;
    }
    scn->flags |= PR_SCN_FLAG_80000;
    scn->patod = memoryPos[todidx];
    dbgInfo.paratodid = todidx;
}

void Scn1CheckParaIdle(register SCENE_INFO *scn) {
    if (eventInfo.paraidletimer > 0) {
        if (--eventInfo.paraidletimer == 0) {
            if (eventInfo.controlstage != 0) {
                Scn1SetPaWaitAnimDefault(scn);
                Scn1SetFaceSet(scn, scn1FaceSet, 0, D_801DBCDC);
            }
        }
    }
}

void Scn1CheckTeacherIdle(register SCENE_INFO *scn) {
    if ((eventInfo.coolstate == PR_COOLSTATE_TRANSCOOL) && (eventInfo.teacheridletimer > 0)) {
        if (--eventInfo.teacheridletimer == 0) {
            Scn1SetOnWaitAnimDefault(scn);
            eventInfo.teacheridletimer = 33;
        }
    }
}

void Scn1RunHighTodAnim(register SCENE_INFO *scn) {
    SCENE1_TOD_SET *set;

    set = &scn1TodSet[eventInfo.controlstage];
    if (scn->level == LEVEL_HIGH) {
        Scn1SetPaTod(scn, set->patodhigh);
        Scn1SetOnTod(scn, 0);
    } else {
        Scn1SetPaTod(scn, set->patodnormal);
        Scn1SetOnTod(scn, set->ontod);
    }
}

void Scn1InitPacketBuffer(void) {
    u8 *ptr;
    s32 i;

    ptr = MemoryEndAlloc(57600 * 2);
    if (ptr == NULL) {
        DbgError("Packet buffer malloc failed.");
        exit(1);
    }
    for (i = 0; i < 2; i++) {
        D_801DF320[i] = (PACKET *)(&ptr[i * 57600]);
    }
}

void Scn1DrawSkyGradient(s32 x, s32 y, s32 w, s32 h, u32 color0, u32 color1, u32 pri, GsOT *ot) {
    POLY_G4 poly;
    s32 r;
    s32 g;
    s32 b;

    SetPolyG4(&poly);
    setXY4(&poly,
        x - (320 / 2),     y - (240 / 2),
        x - (320 / 2) + w, y - (240 / 2),
        x - (320 / 2),     y - (240 / 2) + h,
        x - (320 / 2) + w, y - (240 / 2) + h
    );
    r = (color0 >> 0x10) & 0xff;
    g = (color0 >> 0x08) & 0xff;
    b = (color0 >> 0x00) & 0xff;
    setRGB0(&poly, r, g, b);
    setRGB1(&poly, r, g, b);
    r = (color1 >> 0x10) & 0xff;
    g = (color1 >> 0x08) & 0xff;
    b = (color1 >> 0x00) & 0xff;
    setRGB2(&poly, r, g, b);
    setRGB3(&poly, r, g, b);
    GsSortPoly(&poly, ot, pri);
}

void Scn1DrawScenePreGame(void) {
    if (D_801DF2E0 == FALSE) {
        CompoRegisterDObj(D_801E5BA4, NELEM(D_801E5BA4), &D_801E3630[D_800827D4], 10);
    }
    CompoRegisterDObj(D_801E7588, NELEM(D_801E7588), &D_801E3630[D_800827D4], 10);
    CompoRegisterDObj(D_801E5B64, NELEM(D_801E5B64), &D_801E3630[D_800827D4], 10);
    CompoRegisterDObj(&D_801E5B84[D_801DF2B4], 1, &D_801E3630[D_800827D4], 10);
    if (D_801DF2DC != FALSE) {
        CompoRegisterDObj(D_801E5A94, 1, &D_801DF508[D_800827D4], 10);
        CompoRegisterDObj(&D_801E5A94[9], 3, &D_801DF508[D_800827D4], 10);
    } else {
        CompoRegisterDObj(D_801E5A94, NELEM(D_801E5A94), &D_801DF508[D_800827D4], 10);
    }
    Scn1DrawSky();
    CompoRegisterDObj(D_801E5B54, NELEM(D_801E5B54), &D_801E2DA8[D_800827D4], 8);
}

extern s32 D_800827F4;

void Scn1DrawSceneGame(void) {
    if (D_801DF2E0 == FALSE) {
        CompoApplyDat(&D_801E7374, D_801DF2BC++, 1);
        D_800827F4 = 6;
        if ((CompoSetTod(D_801DF2C4++, &D_801E7584, &D_801DF2C0, &D_801E5BA4[0]) < 1) && (D_801DF2B8 != FALSE)) {
            D_801DF2B8 = FALSE;
            D_801DF2E0 = TRUE;
        }
        if (D_801DF2E8 != FALSE) {
            CompoRegisterDObj(D_801E5BA4, NELEM(D_801E5BA4), &D_801DF508[D_800827D4], 10);
        } else {
            CompoRegisterDObj(D_801E5BA4, NELEM(D_801E5BA4), &D_801E3630[D_800827D4], 10);
        }
    }
    CompoApplyMainDat();
    D_800827F4 = 7;
    CompoSetTod(D_801DF2CC++, &D_801E75E8, &D_801DF2C8, &D_801E7588[0]);
    if (D_801DF2E8 != FALSE) {
        CompoRegisterDObj(D_801E7588, NELEM(D_801E7588), &D_801DF508[D_800827D4], 10);
    } else {
        CompoRegisterDObj(D_801E7588, NELEM(D_801E7588), &D_801E3630[D_800827D4], 10);
    }

    if ((D_801E75EC == LEVEL_HIGH) && (D_801DF2DC == FALSE)) {
        Scn1DrawDojoHigh(D_801DF2C4);
    }
    if (D_801DF2EC != FALSE) {
        Scn1PlayNinTenAnim();
    }
    if (D_801DF2E8 != FALSE) {
        Scn1PlayInvalidInputAnim();
    }
    if (D_801DF2E4 != FALSE) {
        Scn1PlayNinShowerAnim();
    }
    if (D_801DF2F0 != FALSE) {
        Scn1PlayNinLrAnim();
    }

    CompoRegisterDObj(D_801E5B64, NELEM(D_801E5B64), &D_801E3630[D_800827D4], 10);
    if (D_801DF2E8 != FALSE) {
        CompoRegisterDObj(&D_801E5B84[D_801DF2B4], 1, &D_801DF508[D_800827D4], 10);
    } else {
        CompoRegisterDObj(&D_801E5B84[D_801DF2B4], 1, &D_801E3630[D_800827D4], 10);
    }
    if (D_801DF2DC != FALSE) {
        CompoRegisterDObj(&D_801E5A94[0], 1, &D_801DF508[D_800827D4], 10);
        CompoRegisterDObj(&D_801E5A94[9], 3, &D_801DF508[D_800827D4], 10);
    } else {
        CompoRegisterDObj(&D_801E5A94[0], 1, &D_801E2580[D_800827D4], 8);
        CompoRegisterDObj(&D_801E5A94[1], 1, &D_801E1530[D_800827D4], 8);
        CompoRegisterDObj(&D_801E5A94[2], 2, &D_801E1D58[D_800827D4], 8);
        CompoRegisterDObj(&D_801E5A94[4], 1, &D_801E2580[D_800827D4], 8);
        CompoRegisterDObj(&D_801E5A94[5], 8, &D_801DF508[D_800827D4], 10);
    }
    CompoRegisterDObj(D_801E5B54, NELEM(D_801E5B54), &D_801E2DA8[D_800827D4], 8);
}

void Scn1InitMovieText(void) {
    CompoSetTextBox(28, 189, 272, 480);
    CompoSetFontJp(832, 256, 20, 21);
    func_80022e88();
}

void Scn1InitData(void) {
    s32 i;

    Scn1InitPacketBuffer();
    CompoSetPacket(D_801DF320[0], D_801DF320[1]);
    for (i = 0; i < 2; i++) {
        D_801E2DA8[i].length = 8;
        D_801E2DA8[i].org = D_801E2DD0[i];
        D_801E2DA8[i].offset = 0;
        D_801E1530[i].length = 8;
        D_801E1530[i].org = D_801E1558[i];
        D_801E1530[i].offset = 0;
        D_801E1D58[i].length = 8;
        D_801E1D58[i].org = D_801E1D80[i];
        D_801E1D58[i].offset = 0;
        D_801E2580[i].length = 8;
        D_801E2580[i].org = D_801E25A8[i];
        D_801E2580[i].offset = 0;
        D_801E3630[i].length = 10;
        D_801DF508[i].length = 10;
        D_801E5658[i].length = 1;
        D_801E3630[i].org = D_801E3658[i];
        D_801DF508[i].org = D_801DF530[i];
        D_801E5658[i].org = D_801E5680[i];
        D_801E3630[i].offset = D_801DF508[i].offset = D_801E5658[i].offset = 0;
    }
    CompoSetTurnIcon(COMPO_TURN_ICON_1);
    CompoRemoveLevelGrayTextBorder();
    D_801DF2D8 = FALSE;
    CompoSetTextBox(20, 198, 272, 480);
    CompoSetFontJp(832, 256, 20, 21);
    CompoResetLineButton();
    D_801DF2DC = FALSE;
    D_801DF2E0 = FALSE;
    D_801DF2B8 = FALSE;
    D_80082810 = 1;
    D_801DF2E4 = FALSE;
    D_801DF2E8 = FALSE;
    D_801DF2EC = FALSE;
    D_801DF2F0 = FALSE;
    GsInitCoordinate2(NULL, &D_801DF328);
    GsInitCoordinate2(NULL, &D_801E5BB4);
    GsInitCoordinate2(NULL, &D_801E7598);
    GsInitCoordinate2(NULL, &D_801E35D0);
    for (i = 0; i < 5; i++) {
        GsInitCoordinate2(NULL, &D_801DF378[i]);
    }
    for (i = 0; i < 3; i++) {
        GsInitCoordinate2(NULL, &D_801E5690[i]);
    }
    for (i = 0; i < 2; i++) {
        GsInitCoordinate2(&D_801DF378[4], &D_801E58D4[i]);
        GsInitCoordinate2(&D_801DF378[4], &D_801E598C[i]);
    }
    GsInitCoordinate2(&D_801E598C[0], &D_801E5780[0]);
    GsInitCoordinate2(&D_801E598C[1], &D_801E5780[1]);

    if (D_801DF2D4 != FALSE) {
        CompoInitTmd(memoryPos[COMPO01_ON_TMD], &D_801E5BA4[0], &D_801E5BB4);
        CompoInitTmd(memoryPos[COMPO01_PA_TMD], &D_801E7588[0], &D_801E7598);
        CompoInitTmd(memoryPos[COMPO01_TAMA_TMD], &D_801E5B84[0], &D_801DF328);
        CompoInitTmd(memoryPos[COMPO01_TOUROU_TMD], &D_801E5A94[0], &D_801DF378[0]);
        CompoInitTmd(memoryPos[COMPO01_KABE_B_TMD], &D_801E5A94[4], &D_801DF378[4]);
        CompoInitTmd(memoryPos[COMPO01_KAMON_L_TMD], &D_801E5A94[5], &D_801E598C[0]);
        CompoInitTmd(memoryPos[COMPO01_KAMON_R_TMD], &D_801E5A94[6], &D_801E598C[1]);
        CompoInitTmd(memoryPos[COMPO01_MADO_L_TMD], &D_801E5A94[7], &D_801E58D4[0]);
        CompoInitTmd(memoryPos[COMPO01_MADO_R_TMD], &D_801E5A94[8], &D_801E58D4[1]);
        CompoInitTmd(memoryPos[COMPO01_ISDN_TMD], &D_801E5A94[9], &D_801DF328);
        CompoInitTmd(memoryPos[COMPO01_ENNGAWA_TMD], &D_801E5A94[11], &D_801DF328);
        CompoInitTmd(memoryPos[COMPO01_TENNJOU_TMD], &D_801E5A94[1], &D_801DF378[1]);
        CompoInitTmd(memoryPos[COMPO01_KABE_R_TMD], &D_801E5A94[2], &D_801DF378[2]);
        CompoInitTmd(memoryPos[COMPO01_KABE_L_TMD], &D_801E5A94[3], &D_801DF378[3]);
        CompoInitTmd(memoryPos[COMPO01_YUKA_TMD], &D_801E5A94[10], &D_801DF328);
        CompoInitTmd(memoryPos[COMPO01_KAGE_TMD], &D_801E5B54[0], &D_801DF328);
        CompoInitTmd(memoryPos[COMPO01_NIN_TEN_TMD], &D_801E5834[0], &D_801E5690[0]);
        CompoInitTmd(memoryPos[COMPO01_TATAMI_TMD], &D_801E5B64[0], &D_801E35D0);
        CompoInitTmd(memoryPos[COMPO01_NIN_TTM_TMD], &D_801E5844[0], &D_801E5690[1]);
        CompoInitTmd(memoryPos[COMPO01_NIN_SHJ_TMD], &D_801E5854[0], &D_801E5690[2]);
        CompoInitTmd(memoryPos[COMPO01_NIN_L_TMD], &D_801E5864[0], &D_801E5780[0]);
        CompoInitTmd(memoryPos[COMPO01_NIN_R_TMD], &D_801E5864[1], &D_801E5780[1]);
        CompoInitTmd(memoryPos[COMPO01_WAKU_L_TMD], &D_801E5A2C[0], &D_801E598C[0]);
        CompoInitTmd(memoryPos[COMPO01_WAKU_R_TMD], &D_801E5A2C[1], &D_801E598C[1]);
        CompoInitTmd(memoryPos[COMPO01_DONDE_L_TMD], &D_801E5A2C[2], &D_801E598C[0]);
        CompoInitTmd(memoryPos[COMPO01_DONDE_R_TMD], &D_801E5A2C[3], &D_801E598C[1]);
        D_801DF2D4 = FALSE;
    } else {
        CompoResetFirstTmdVdf();
        ActionResetMimeVdf(1);
    }

    for (i = 0; i < 5; i++) {

    }
    GsSetProjection(440);
    CompoFrameResetActive();
    CompoLoadTimClut(memoryPos[COMPO01_F_ONKU_0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONMAL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONMAR0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONMEL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONMER0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONTBL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONTBR0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONTFL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONTFR0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONYB_0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_ONYF_0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PAKU_0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PAMEL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PAMER0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PATBL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PATBR0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PATFL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_PATFR0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_YUMEL0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_YUMER0_TIM], TRUE);
    CompoLoadTimClut(memoryPos[COMPO01_F_YUKU_0_TIM], TRUE);

    D_801DF2C4 = 0;
    CompoInitTod(memoryPos[COMPO01_ON_LOC_TOD], &D_801DF2C0, &D_801E7584);
    CompoSetTod(D_801DF2C4, &D_801E7584, &D_801DF2C0, &D_801E5BA4[0]);
    D_801DF2CC = 0;
    CompoInitTod(memoryPos[COMPO01_PA_LOC_TOD], &D_801DF2C8, &D_801E75E8);
    CompoSetTod(D_801DF2CC, &D_801E75E8, &D_801DF2C8, &D_801E7588[0]);
    CompoInitMainModelSet(memoryPos[COMPO01_PA_WT0_DAT], memoryPos[COMPO01_PA_WT0_VDF], memoryPos[COMPO01_PA_TMD], FALSE);
    CompoApplyMainDat();

    CompoInitTod(memoryPos[COMPO01_NIN_TEN_TOD], &D_801E5884, &D_801E58AC);
    D_801E5898 = D_801E5884;
    D_801E58C0 = D_801E58AC;
    CompoInitTod(memoryPos[COMPO01_NIN_L_TOD], &D_801E5890, &D_801E58B8);
    D_801E58A4 = D_801E5890;
    D_801E58CC = D_801E58B8;
    CompoInitTod(memoryPos[COMPO01_NIN_R_TOD], &D_801E5894, &D_801E58BC);
    D_801E58A8 = D_801E5894;
    D_801E58D0 = D_801E58BC;
    CompoInitTod(memoryPos[COMPO01_NIN_TTM_TOD], &D_801E5888, &D_801E58B0);
    D_801E589C = D_801E5888;
    D_801E58C4 = D_801E58B0;
    CompoInitTod(memoryPos[COMPO01_TATAMI_TOD], &D_801E3620, &D_801E3628);
    D_801E3624 = D_801E3620;
    D_801E362C = D_801E3628;
    CompoInitTod(memoryPos[COMPO01_NIN_SHJ_TOD], &D_801E588C, &D_801E58B4);
    D_801E58A0 = D_801E588C;
    D_801E58C8 = D_801E58B4;
    CompoInitTod(memoryPos[COMPO01_MADO_L_TOD], &D_801E5974, &D_801E5984);
    CompoInitTod(memoryPos[COMPO01_MADO_R_TOD], &D_801E5978, &D_801E5984);
    D_801E597C = D_801E5974;
    D_801E5980 = D_801E5978;
    D_801E5988 = D_801E5984;
    
    ActionInitModel(1, memoryPos[COMPO01_ON_TMD], memoryPos[COMPO01_ON_GONE_VDF], D_801E5C04);
    MimeDatInit(&D_801E7374, D_801E7384, memoryPos[COMPO01_ON_GONE_DAT], FALSE);
    CompoApplyDat(&D_801E7374, 0, 1);
    ActionInitView(scn1ViewSubframe);
    ActionSetBez(memoryPos[COMPO01_CM_OP1_BEZ], 11);
    ActionStartBez();
    CompoResetMime();
}

void Scn1GameDraw(SCENE_INFO *scn, s32 type) {
    s32 i;

    D_800827D4 = GsGetActiveBuff();
    GsSetWorkBase(D_801DF320[D_800827D4]);
    GsClearOt(0, 0, &D_801E5658[D_800827D4]);
    GsClearOt(0, 0, &D_801DF508[D_800827D4]);
    GsClearOt(0, 0, &D_801E1530[D_800827D4]);
    GsClearOt(0, 0, &D_801E1D58[D_800827D4]);
    GsClearOt(0, 0, &D_801E2580[D_800827D4]);
    GsClearOt(0, 0, &D_801E2DA8[D_800827D4]);
    GsClearOt(0, 0, &D_801E3630[D_800827D4]);
    CompoClear(D_800827D4);
    if ((scn->flags & PR_SCN_FLAG_20) != 0) {
        D_801DF2D8 = TRUE;
    }

    if (type == 4) {
        CompoFrameSetAnimActive(4, COMPO_FRAME_ANIM_TYPE_4);
        CompoFrameDrawAnimMovie(2, D_800827D4);
        Scn1DrawScenePreGame();
        return;
    }
    if (D_801DF2D8 == FALSE) {
        CompoFrameSetAnimInactive(4, COMPO_FRAME_ANIM_TYPE_3);
        CompoFrameDrawAnimMovie(2, D_800827D4);
        Scn1DrawScenePreGame();
        return;
    }

    if ((scn->flags & PR_SCN_FLAG_1000000) != 0) {
        switch (scn->nintype) {
            case 1: {
                D_801E5824 = 0;
                D_801E3620 = D_801E3624;
                D_801E3628 = D_801E362C;
                D_801E5888 = D_801E589C;
                D_801E58B0 = D_801E58C4;
                D_801DF2E8 = TRUE;
                break;
            }
            case 2: {
                D_801E5820 = 0;
                D_801E5884 = D_801E5898;
                D_801E58AC = D_801E58C0;
                D_801DF2EC = TRUE;
                break;
            }
            case 4: {
                D_801E582C = 0;
                D_801E5890 = D_801E58A4;
                D_801E5894 = D_801E58A8;
                D_801E58B8 = D_801E58CC;
                D_801E58BC = D_801E58D0;
                D_801DF2F0 = TRUE;
                break;
            }
            case 3: {
                D_801E5828 = 0;
                D_801E5974 = D_801E597C;
                D_801E5978 = D_801E5980;
                D_801E5984 = D_801E5988;
                D_801E588C = D_801E58A0;
                D_801E58B4 = D_801E58C8;
                D_801DF2E4 = TRUE;
                break;
            }
            default: {
                break;
            }
        }
    }
    
    if ((scn->flags & PR_SCN_FLAG_80000) != 0) {
        CompoInitTod(scn->patod, &D_801DF2C8, &D_801E75E8);
        D_801DF2CC = 0;
    }
    if ((scn->flags & PR_SCN_FLAG_10) != 0) {
        if (scn->padat != NULL) {
            CompoSetMainModelAnim(scn->padat, scn->pavdf, scn->paframe, scn->paframenum);
        }
    } else {
        if ((scn->flags & PR_SCN_FLAG_20000) != 0) {
            if (scn->padat != NULL) {
                CompoSetMainModelAnim(scn->padat, scn->pavdf, 0, 999);
            }
        }
    }
    
    if ((scn->flags & PR_SCN_FLAG_100000) != 0) {
        CompoInitTod(scn->teachertod, &D_801DF2C0, &D_801E7584);
        D_801DF2C4 = 0;
    }
    if ((scn->flags & PR_SCN_FLAG_40000) != 0) {
        if (scn->teacherdat != NULL) {
            CompoSetModelAnim(1, scn->teacherdat, scn->teachervdf, &D_801E7374, D_801E7384, &D_801DF2BC);
        }
    }

    if ((scn->flags & PR_SCN_FLAG_2000) != 0) {
        for (i = 0; i < 5; i++) {
            CompoInitTod(scn->dojotod[i], &D_801E5A6C[i], &D_801E5A90);
        }
        if (scn->highcnt > 1) {
            D_801DF2B8 = TRUE;
            D_801DF2E0 = FALSE;
        }
    }
    if ((scn->flags & PR_SCN_FLAG_4000) != 0) {
        D_801DF2E0 = FALSE;
    }
    if ((scn->flags & PR_SCN_FLAG_40) != 0) {
        D_801DF2E0 = FALSE;
    }
    
    if ((scn->flags & PR_SCN_FLAG_10000) != 0) {
        for (i = 0; scn->teacherface[i] != 0; i++) {
            CompoLoadTimClut(memoryPos[scn->teacherface[i]], FALSE);
        }
    }
    if ((scn->flags & PR_SCN_FLAG_8000) != 0) {
        for (i = 0; scn->parappaface[i] != 0; i++) {
            CompoLoadTimClut(memoryPos[scn->parappaface[i]], TRUE);
        }
    }
    if ((scn->flags & PR_SCN_FLAG_800000) != 0) {
        for (i = 0; scn->unkB4[i] != 0; i++) {
            CompoLoadTimClut(memoryPos[scn->unkB4[i]], FALSE);
        }
    }
    
    D_801E75EC = scn->level;
    D_801DF2B4 = scn->tamasel;

    if ((scn->flags & PR_SCN_FLAG_400) != 0) {
        D_801DF2D0 = 0;
        ActionSetBez(scn->nextbez, 11);
    }
    if (D_801DF2D8 != FALSE) {
        ActionFollowBez(D_801DF2D0++);
    }

    Scn1DrawSky();

    if (scn->drawtext != FALSE) {
        if (scn->drawtextlang != 0) {
            CompoDrawStageTextEn(scn->entext, D_800827D4);
        } else {
            CompoDrawStageTextJp(scn->jptext, D_800827D4);
        }
    }

    if (scn->ingame != FALSE) {
        if ((scn->flags & PR_SCN_FLAG_200) != 0) {
            switch (scn->leveltransition) {
                case PR_SCN_LVLTRANS_1: {
                    D_801DF2F4 = PR_DIR_DOWN;
                    break;
                }
                case PR_SCN_LVLTRANS_2: {
                    D_801DF2F4 = PR_DIR_DOWN;
                    break;
                }
                case PR_SCN_LVLTRANS_3: {
                    D_801DF2F4 = PR_DIR_UP;
                    break;
                }
                case PR_SCN_LVLTRANS_4: {
                    D_801DF2F4 = PR_DIR_UP;
                    break;
                }
                case PR_SCN_LVLTRANS_5: {
                    D_801DF2F4 = PR_DIR_UP;
                    break;
                }
                case PR_SCN_LVLTRANS_6: {
                    D_801DF2F4 = PR_DIR_DOWN;
                    break;
                }
                default: {
                    break;
                }
            }
            CompoDrawHand(TRUE, D_801DF2F4, D_800827D4);
        }
        CompoDrawHand(FALSE, D_801DF2F4, D_800827D4);
        if (scn->drawlesson != FALSE) {
            CompoDrawLesson(scn->lesson, D_800827D4);
        }
        CompoDrawScore(scn->flashlevel, scn->level, scn->leveltoflash, scn->dispscore, scn->lesson, D_800827D4);
    }
    CompoDrawLine(scn, D_800827D4);
    Scn1DrawSceneGame();
}

void Scn1GameSwap(SCENE_INFO *scn) {
    GsSwapDispBuff();
    if (D_801DF2DC != FALSE) {
        GsSortClear(134, 103, 83, &D_801E5658[D_800827D4]);
    } else {
        GsSortClear(101, 58, 90, &D_801E5658[D_800827D4]);
    }
    CompoUpdateWorkOfs(D_801DF320[D_800827D4]);
    GsDrawOt(&D_801E5658[D_800827D4]);
    GsDrawOt(&D_801E2580[D_800827D4]);
    GsDrawOt(&D_801E1D58[D_800827D4]);
    GsDrawOt(&D_801E1530[D_800827D4]);
    GsDrawOt(&D_801E2DA8[D_800827D4]);
    GsDrawOt(&D_801DF508[D_800827D4]);
    GsDrawOt(&D_801E3630[D_800827D4]);
    CompoDrawOt(D_800827D4);
    FntFlush(-1);
}


void Scn1DrawDojoHigh(register s32 frame) {
    s32 framenum;
    s32 i;

    if (D_801E5A6C[0] == NULL) {
        return;
    }
    if (D_801E5A90 < 1) {
        D_801DF2DC = TRUE;
        D_801DF2E0 = TRUE;
        return;
    }
    for (i = 0; i < 4; i++) {
        D_800827F4 = i + 1;
        framenum = D_801E5A90;
        CompoSetTod(frame, &framenum, &D_801E5A6C[i], &D_801E5A94[i]);
    }
    D_800827F4 = 5;
    CompoSetTod(frame, &D_801E5A90, &D_801E5A6C[4], &D_801E5A94[4]);
        
}

void Scn1DrawSky(void) {
    VECTOR sp20;
    VECTOR sp30;
    s32 sp40;
    s32 sp44;
    s32 i;
    s32 sp4C;

    sp20.vx = actionView.vrx - actionView.vpx;
    sp20.vy = actionView.vry - actionView.vpy;
    sp20.vz = actionView.vrz - actionView.vpz;
    VectorNormal(&sp20, &sp30);
    sp40 = -(sp30.vy * 440) >> 12;
    if ((sp30.vx == 0) && (sp30.vz == 0)) {
        sp44 = 0;
    } else {
        sp44 = (ratan2(sp30.vz, sp30.vx) >> 1) - 512;
    }
    Scn1DrawSkyGradient(0, 0, 320, 145 + sp40, 0x901800, 0xe8803f, 1, &D_801E5658[D_800827D4]);
    sp4C = -640;
    for (i = 0; i < 5; i++, sp4C += 320) {
        CompoFastSpr(sp44 + sp4C + 240, sp40 + 35, &D_801DF224, 0, 0, 0, &D_801E5658[D_800827D4]);
        CompoFastSpr(sp44 + sp4C - 30, sp40, &D_801DF224, 0, 0, 0, &D_801E5658[D_800827D4]);
    }
    sp4C = -708;
    for (i = 0; i < 7; i++, sp4C += 256) {
        CompoFastSpr(sp44 + sp4C, sp40 + 103, &D_801DF260, 0, 0, 0, &D_801E5658[D_800827D4]);
    }
    CompoFastSpr(sp44 + 110, sp40 + 10, &D_801DF238, 0, 0, 0, &D_801E5658[D_800827D4]);
    if (D_801DF2E0 != FALSE) {
        CompoFastSpr(sp44 + 40, sp40 - 2, &D_801DF1FC, 0, 0, 0, &D_801E5658[D_800827D4]);
        CompoFastSpr(sp44 + 40, sp40 + 93, &D_801DF210, 0, 0, 0, &D_801E5658[D_800827D4]);
    }
}

void Scn1PlayInvalidInputAnim(void) {
    s32 i;

    if (D_801E3628 < 1) {
        D_801DF2E8 = FALSE;
        return;
    }

    if (D_801E5824 == 16) {
        for (i = 0; i < 39; i++) {
            CompoSetTod(D_801E5824, &D_801E58B0, &D_801E5888, &D_801E5844[0]);
            CompoSetTod(D_801E5824++, &D_801E3628, &D_801E3620, &D_801E5B64[0]);
        }
    }
    CompoSetTod(D_801E5824, &D_801E58B0, &D_801E5888, &D_801E5844[0]);
    CompoSetTod(D_801E5824++, &D_801E3628, &D_801E3620, &D_801E5B64[0]);
    CompoRegisterDObj(D_801E5844, NELEM(D_801E5844), &D_801DF508[D_800827D4], 10);
}

void Scn1PlayNinTenAnim(void) {
    if (D_801DF2DC != FALSE) {
        return;
    }

    if (D_801E58AC < 1) {
        D_801DF2EC = FALSE;
        return;
    }

    CompoSetTod(D_801E5820++, &D_801E58AC, &D_801E5884, &D_801E5834[0]);
    CompoRegisterDObj(D_801E5834, NELEM(D_801E5834), &D_801DF508[D_800827D4], 10);
}

void Scn1PlayNinLrAnim(void) {
    if (D_801DF2DC != FALSE) {
        return;
    }

    if (D_801E58B8 < 1) {
        D_801DF2F0 = FALSE;
        return;
    }

    CompoSetTod(D_801E582C, &D_801E58B8, &D_801E5890, &D_801E5A94[5]);
    CompoSetTod(D_801E582C++, &D_801E58BC, &D_801E5894, &D_801E5A94[6]);
    CompoRegisterDObj(D_801E5A2C, NELEM(D_801E5A2C), &D_801DF508[D_800827D4], 10);
    CompoRegisterDObj(D_801E5864, NELEM(D_801E5864), &D_801DF508[D_800827D4], 10);
}

void Scn1PlayNinShowerAnim(void) {
    s32 framenum;

    if (D_801DF2DC != FALSE) {
        return;
    }

    if (D_801E5984 < 1) {
        D_801DF2E4 = FALSE;
        return;
    }

    framenum = D_801E5984;
    CompoSetTod(D_801E5828, &framenum, &D_801E5974, &D_801E5A94[7]);
    CompoSetTod(D_801E5828, &D_801E5984, &D_801E5978, &D_801E5A94[8]);
    CompoSetTod(D_801E5828++, &D_801E58B4, &D_801E588C, &D_801E5854[0]);
    CompoRegisterDObj(D_801E5854, NELEM(D_801E5854), &D_801DF508[D_800827D4], 10);
}

u16 scn1Stupidpad = 0x61a2;
