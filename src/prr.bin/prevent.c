#include "prevent.h"
#include "prmemory.h"
#include "prrap.h"
#include "prvdbg.h"
#include "praction.h"

static char rcsid[] = "@(#)prevent.c: version 01-00 95/10/10 00:00:00";

extern BOOL eventOverridenLang;
extern BOOL eventOverridenLangPrev;

extern s32 eventMovieSubNum;
extern MOVIE_SUBTITLES_INFO *eventMovieSubInfo;
extern PARA_EN_T *eventMovieSubEn;
extern PARA_JP_T *eventMovieSubJp;

extern EVENT_RECORD_INFO eventRecordInfo;


s32 EventGetKeyIndexFromPad(u32 pad) {
    if (pad & PR_PAD_TRIANGLE) {
        return PR_TAP_TRIANGLE;
    }
    if (pad & PR_PAD_CIRCLE) {
        return PR_TAP_CIRCLE;
    }
    if (pad & PR_PAD_CROSS) {
        return PR_TAP_CROSS;
    }
    if (pad & PR_PAD_SQUARE) {
        return PR_TAP_SQUARE;
    }
    if (pad & PR_PAD_L1) {
        return PR_TAP_L1;
    }
    if (pad & PR_PAD_L2) {
        return PR_TAP_L1;
    }
    if (pad & PR_PAD_R1) {
        return PR_TAP_R1;
    }
    if (pad & PR_PAD_R2) {
        return PR_TAP_R1;
    } else {
        return PR_TAP_NONE;
    }
}

static u32 keyIdxToPad[PR_TAP_NUM] = {
    [PR_TAP_NONE] = PR_PAD_NONE,
    [PR_TAP_TRIANGLE] = PR_PAD_TRIANGLE,
    [PR_TAP_CIRCLE] = PR_PAD_CIRCLE,
    [PR_TAP_CROSS] = PR_PAD_CROSS,
    [PR_TAP_SQUARE] = PR_PAD_SQUARE,
    [PR_TAP_L1] = PR_PAD_L1,
    [PR_TAP_L2] = PR_PAD_L1,
    [PR_TAP_R1] = PR_PAD_R1,
    [PR_TAP_R2] = PR_PAD_R1,
};

u32 EventGetPadFromKeyIndex(s32 keyid) {
    return ((keyid > PR_TAP_NONE) && (keyid < PR_TAP_NUM)) ? keyIdxToPad[keyid] : PR_PAD_NONE;
}

void EventResetEstimate(SCENE_INFO *scn) {
    eventInfo.lvlestimate = 2;
    eventInfo.normaleval = FALSE;
    scn->flashlevel = FALSE;
    ActionResetPerfomanceScore();
}

// Returns TRUE if either we are at the ending or in high transition
BOOL EventInputsInactive(SCENE_INFO *scn) {
    if (eventInfo.coolstate == PR_COOLSTATE_COOL) {
        if (eventInfo.hightransitioning == FALSE) {
            return FALSE;
        }
        if (eventInfo.highstarttime >= scn->gametime) {
            return TRUE;
        }
        eventInfo.hightransitioning = FALSE;
        return FALSE;
    } else if (eventInfo.controldelay != 0) {
        return TRUE;
    } else {
        if (eventInfo.hightransitioning == FALSE) {
            return FALSE;
        }
        return (eventInfo.coolstate == PR_COOLSTATE_TRANSCOOL);
    }
}

void EventGetDispScore(SCENE_INFO *scn) {
    scn->dispscore = ActionGetCurScore(scn);
}

SCENE_INFO *EventUpdate(SCENE_INFO *scn) {
    s32 linetime;
    s32 tim;
    s32 lesson;
    s32 sp1C;
    s32 sp20;

    if (eventInfo.gameover == TRUE) {
        return scn;
    }

    tim = scn->gametime + 16;
    linetime = scn->gametime % (96 * 4);
    eventInfo.linetime = linetime;

    eventInfo.eighth = linetime / (96 / 8);
    if (eventInfo.preveighth == eventInfo.eighth) {
        return scn;
    }

    eventInfo.preveighth = eventInfo.eighth;
    sp20 = (scn->gametime / (96 * 4)) + 1;
    if (linetime >= ((96 * 4) - 16)) {
        sp1C = (tim / (96 * 4)) + 1;
    } else {
        sp1C = sp20;
    }

    if (sp1C < sceneInitInfo.inpinfolen) {
        scn->parappainp = &sceneInitInfo.inpinfo[sp1C];
    }
    scn->curline = sp20;
    dbgInfo.ncall = sp1C;

    lesson = 0;
    if (eventInfo.eighth == 0) {
        scn->flags |= PR_SCN_FLAG_2;
        if (scn->curline < sceneInitInfo.inpinfolen) {
            scn->teacherinp = &sceneInitInfo.inpinfo[scn->curline];
            lesson = scn->teacherinp->lesson;
            eventInfo.inpflags = (scn->teacherinp != NULL) ? scn->teacherinp->sub[scn->lvlhigh].flags : 0;
        }
    }

    if ((eventInfo.eighth % 2) == 0) {
        scn->flags |= PR_SCN_FLAG_8;
        if ((eventInfo.eighth % 8) == 0) {
            scn->flags |= PR_SCN_FLAG_4;
            if (scn->clicksound != FALSE) {
                RapPlayKey(sceneInitInfo.click);
            }
        }
    }

    if (eventInfo.eighth == sceneInitInfo.times->checkfail) {
        EventCheckGameOver(scn);
        if (eventInfo.inpflags & PR_INP_FLAG_80) {
            ActionResetKeyInfo(scn);
        }
        if ((scn->lesson != lesson) && (lesson != 0)) {
            scn->lesson = lesson;
            if (scn->level != 0) {
                scn->drawlesson = TRUE;
            }
        } else {
            scn->drawlesson = FALSE;
        }
    }

    if (eventInfo.eighth == sceneInitInfo.times->checkperf) {
        if (EventInputsInactive(scn) == FALSE) {
            EventUpdateScore(scn);
            EventUpdatePerfomance(scn);
        }
        if (eventInfo.coolstate != PR_COOLSTATE_NOTCOOL) {
            EventUpdateHigh(scn);
        }
    }

    if (eventInfo.eighth == ((8 * 4) - 1)) {
        ActionClearOnInputList(scn->curline);
    }

    if (eventInfo.eighth == sceneInitInfo.times->checkhigh) {
        EventUpdateImmHigh(scn);
    }
    return scn;
}

void EventSetMovieSubtitles(MOVIE_SUBTITLES *sub) {
    eventInfo.jptexttimer = 0;
    eventInfo.entexttimer = 0;
    eventInfo.jptext = NULL;
    eventInfo.entext = NULL;
    eventCurMovieSub = 0;
    if (sub != NULL) {
        eventMovieSubNum = sub->num;
        eventMovieSubInfo = sub->info;
        eventMovieSubJp = sub->jp;
        eventMovieSubEn = sub->en;
    } else {
        eventMovieSubNum = 0;
        eventMovieSubInfo = NULL;
        eventMovieSubJp = NULL;
    }
}

void EventSetFlashHigh(BOOL flashhigh) {
    eventInfo.flashhigh = flashhigh;
}

void EventCheckTextOverride(SCENE_INFO *scn) {
    if (eventOverridenLang == TRUE) {
        eventOverridenLang = FALSE;
        scn->drawtextlang = eventOverridenLangPrev;
    }
}

SCENE_INFO *EventUpdateText(SCENE_INFO *scn) {
    MOVIE_SUBTITLES_INFO *info;

    if (eventInfo.jptexttimer > 0) {
        if ((--eventInfo.jptexttimer) == 0) {
            scn->jptext = eventInfo.jptext = NULL;
        }
    }
    if (eventInfo.entexttimer > 0) {
        if ((--eventInfo.entexttimer) == 0) {
            scn->entext = eventInfo.entext = NULL;
        }
    }
    
    info = EventGetMovieSubInfo(&scn->gametimet);
    if (info == NULL) {
        return NULL;
    }

    if (info->seljp >= 1) {
        eventInfo.jptext = eventMovieSubJp[info->seljp];
        eventInfo.jptexttimer = info->texttime / 2;
    } else {
        if ((info->seljp == -1) && (eventOverridenLang == FALSE)) {
            eventOverridenLang = TRUE;
            eventOverridenLangPrev = scn->drawtextlang;
            scn->drawtextlang = TRUE;
        }
    }
    scn->jptext = eventInfo.jptext;

    if (info->selen > 0) {
        eventInfo.entext = eventMovieSubEn[info->selen];
        eventInfo.entexttimer = info->texttime / 2;
    }
    scn->entext = eventInfo.entext;
    return scn;
}

void EventRecordInit(BOOL init) {
    eventRecordInfo.idx = 0;
    if (init == FALSE) {
        eventRecordInfo.num = 0;
    } else if (eventRecordInfo.num == 0) {
        eventRecordInfo.num = 550;
    }
}

void EventReset(void) {
    MemoryZero(&eventInfo, sizeof(eventInfo));
    eventInfo.gameover = FALSE;
    eventInfo.coolstate = PR_COOLSTATE_NOTCOOL;
    eventInfo.controlstage = TRUE;
    eventInfo.controldelay = 0;

    MemoryZero(&sceneInfo, sizeof(sceneInfo));
    sceneInfo.leniency = 8;
    sceneInfo.level = LEVEL_NORM;
    sceneInfo.lvlhigh = FALSE;
    sceneInfo.notplayer = FALSE;
    sceneInfo.penalty = TRUE;
    sceneInfo.drawbuttons = TRUE;
    sceneInfo.teacherline.num = 0;
    sceneInfo.teacherline.keyid[0] = sceneInitInfo.visline->first;
    sceneInfo.teacherline.keyid[1] = sceneInitInfo.visline->second;
    sceneInfo.teacherline.enabled = 0;
    sceneInfo.parappaline.enabled = 0;
    sceneInfo.drawtext = TRUE;

    EventResetEstimate(&sceneInfo);
}

void EventUpdateHigh(register SCENE_INFO *scn) {
    if (eventInfo.coolstate == PR_COOLSTATE_COOL) {
        EventCheckHighEnding(scn);
    }
    if (eventInfo.coolstate == PR_COOLSTATE_LOSTCOOL) {
        EventCheckHighToNormal(scn);
    }
    if (eventInfo.coolstate == PR_COOLSTATE_TRANSCOOL) {
        if (eventInfo.inpflags & PR_INP_FLAG_40) {
            eventInfo.coolstate = PR_COOLSTATE_IMMNOTCOOL;
            eventInfo.hightransitioning = FALSE;
            eventInfo.transitionhigh = TRUE;

            scn->level = LEVEL_NORM;
            scn->leveltransition = PR_SCN_LVLTRANS_6;
            scn->lvlhigh = FALSE;
            scn->drawtext = TRUE;
            scn->drawbuttons = TRUE;
            eventInfo.lvlestimate = PR_DIR_UP;
        }
    }
}

void EventUpdateImmHigh(register SCENE_INFO *scn) {
    if (eventInfo.coolstate < PR_COOLSTATE_IMMSTART) {
        return;
    }

    if (eventInfo.coolstate == PR_COOLSTATE_IMMLOSTCOOL) {
        eventInfo.coolstate = PR_COOLSTATE_NOTCOOL;
    } else if (eventInfo.coolstate == PR_COOLSTATE_IMMGOTCOOL) {
        eventInfo.coolstate = PR_COOLSTATE_COOL;
    } else if (eventInfo.coolstate == PR_COOLSTATE_IMMNOTCOOL) {
        eventInfo.coolstate = PR_COOLSTATE_NOTCOOL;
        RapPlayKey(&sceneInitInfo.lvlchange[0]);
    }
    scn->flags |= PR_SCN_FLAG_200;
    ActionResetStats();
    ActionRestartTap(scn);
    EventResetEstimate(scn);
}

void EventUpdatePerfomance(register SCENE_INFO *scn) {
    s32 dir;

    if ((eventInfo.inpflags & PR_INP_FLAG_2) == 0) {
        return;
    }

    dir = EventCheckPerformance(scn);
    if ((eventInfo.coolstate == PR_COOLSTATE_COOL) || (eventInfo.coolstate == PR_COOLSTATE_LOSTCOOL)) {
        EventPlayScoreChange(dir);
        ActionRestartTap(scn);
    } else if (eventInfo.coolstate == PR_COOLSTATE_NOTCOOL) {
        if (EventUpdateLevel(scn, dir) == TRUE) {
            return;
        }
        EventPlayScoreChange(dir);
        ActionRestartTap(scn);
    }
    if (dir == PR_DIR_UP) {
        sceneInitInfo.perfup(scn);
    }
}

void EventUpdateScore(register SCENE_INFO *scn) {
    if ((eventInfo.inpflags & (PR_INP_FLAG_1 | PR_INP_FLAG_2)) == 0) {
        return;
    }
    ActionCalcScore(scn);
    ActionUpdateNoTapNum(scn);
}

void EventCheckGameOver(register SCENE_INFO *scn) {
    if (EventInputsInactive(scn) == TRUE) {
        return;
    }

    if ((eventInfo.inpflags & PR_INP_FLAG_10) && ((scn->level == 2) || (scn->level == 3))) {
        scn->flags |= PR_SCN_FLAG_40;
        scn->gameover = TRUE;
        scn->drawtext = TRUE;
    } else if ((eventInfo.inpflags & PR_INP_FLAG_8) && (ActionCheckGameOver(scn) == TRUE)) {
        // PSP: "GAMEOVER at judge point"
        scn->flags |= PR_SCN_FLAG_40;
        scn->gameover = TRUE;
        scn->drawtext = TRUE;
    }
}
