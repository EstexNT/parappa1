#include "prscn1.h"
#include <libetc.h>

static char rcsid[] = "@(#)prscn1.c: version 01-00 95/10/10 00:00:00";

extern VDATA_SCENE *D_801DF2FC;
extern s32 D_801DF300;
extern s32 D_801DF304;
extern BOOL D_801DF308;
extern s32 D_801DF30C;

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
void Scn1Adjust(CD_FILE *file, SCENE_INFO *scn, s32 type, BOOL snd, void (*draw)(SCENE_INFO *scn, s32 type), void (*swap)(SCENE_INFO *scn));
void Scn1End(SCENE_INFO *scn, s32 idx);
s32 Scn1GetFrameTextFromId(s32 idx);
void Scn1SetLevelChannel(SCENE_INFO *scn);
void Scn1GetRecordData(RECORD_DATA *data);
BOOL Scn1EndSave(SCENE_INFO *scn, s32 idx, s32 clear, s32 prevclear);
void Scn1InitSceneControl(void);
void Scn1ResetSceneControl(s32 i);
SCENE1_CONTROL_INFO *Scn1GetSceneControlInfo(s32 time, s32 i);


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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1Game);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1Main);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1ReadInput);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1GameSetup);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetOnWaitAnimDefault);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetPaWaitAnimDefault);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckFace);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateControlAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateControl);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1FnKeyPressed);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1FnOnKeyPressed);

void Scn1FnPerfUp(SCENE_INFO *scn) {

}

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1FnAnyKeyPressed);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetData);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetVisualLineInfo);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetBez);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetFaceSet);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetTeacherAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetPaAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1GetFaceSetInfo);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckControlFinish);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateDojoTod);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateTama);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateTeacherAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateParaAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateFace);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateCamera);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1UpdateDelayContrlInfo);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1RunChecks);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckControlInterruption);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetInvalidInputNin);

SCENE1_CONTROL_LIST *Scn1GetSceneControl(register s32 i) {
    return ((i >= 1) && (i <= 8)) ? &scn1SceneControlList[i] : NULL;
}

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1GetSceneControlDelayInfo);

BOOL Scn1SceneControlEnded(register s32 i) {
    SCENE1_CONTROL_LIST *control;

    control = Scn1GetSceneControl(i);
    if (control == NULL) {
        return FALSE;
    } else {
        return (control->cur >= control->max) ? TRUE : FALSE;
    }
}

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckTeacherLine);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckParaLine);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetOnTod);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1SetPaTod);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckParaIdle);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1CheckTeacherIdle);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1RunHighTodAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1InitPacketBuffer);

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

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1DrawScenePreGame);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1DrawSceneGame);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1InitMovieText);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1InitData);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1GameDraw);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1GameSwap);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1DrawDojoHigh);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1DrawSky);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1PlayInvalidInputAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1PlayNinTenAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1PlayNinLrAnim);

INCLUDE_ASM("asm/scene1/nonmatchings/prscn1", Scn1PlayNinShowerAnim);

u16 scn1Stupidpad = 0x61a2;
