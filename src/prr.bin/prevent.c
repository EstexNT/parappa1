#include "prevent.h"
#include "prmemory.h"
#include "prrap.h"
#include "prvdbg.h"

extern BOOL D_80082784;
extern BOOL D_80082788;

extern s32 D_800827D8;
extern MOVIE_SUBTITLES_INFO *D_800827DC;
extern PARA_EN_T *D_800827E8;
extern PARA_JP_T *D_800827EC;
extern s32 D_800827F8;

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

extern int D_80067904[];

u32 EventGetPadFromKeyIndex(s32 keyid) {
    return ((keyid > PR_TAP_NONE) && (keyid < PR_TAP_NUM)) ? D_80067904[keyid] : PR_PAD_NONE;
}

void EventResetEstimate(SCENE_INFO *scn) {
    eventInfo.lvlestimate = 2;
    eventInfo.normaleval = FALSE;
    scn->flashlevel = FALSE;
    func_80014770();
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
    scn->dispscore = func_80014678(scn);
}

// INCLUDE_ASM("asm/prr.bin/nonmatchings/prevent", EventUpdate);
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

    if (sp1C < D_800A08F4) {
        scn->parappainp = &D_800A08F0[sp1C];
    }
    scn->curline = sp20;
    dbgInfo.ncall = sp1C;

    lesson = 0;
    if (eventInfo.eighth == 0) {
        scn->flags |= PR_SCN_FLAG_2;
        if (scn->curline < D_800A08F4) {
            scn->teacherinp = &D_800A08F0[scn->curline];
            lesson = scn->teacherinp->lesson;
            eventInfo.inpflags = (scn->teacherinp != NULL) ? scn->teacherinp->sub[scn->lvlhigh].flags : 0;
        }
    }

    if ((eventInfo.eighth % 2) == 0) {
        scn->flags |= PR_SCN_FLAG_8;
        if ((eventInfo.eighth % 8) == 0) {
            scn->flags |= PR_SCN_FLAG_4;
            if (scn->clicksound != FALSE) {
                RapPlayKey(D_800A0930);
            }
        }
    }

    if (eventInfo.eighth == D_800A08EC->checkfail) {
        EventCheckGameOver(scn);
        if (eventInfo.inpflags & PR_INP_FLAG_80) {
            func_800159BC(scn);
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

    if (eventInfo.eighth == D_800A08EC->checkperf) {
        if (EventInputsInactive(scn) == FALSE) {
            EventUpdateScore(scn);
            EventUpdatePerfomance(scn);
        }
        if (eventInfo.coolstate != PR_COOLSTATE_NOTCOOL) {
            EventUpdateHigh(scn);
        }
    }

    if (eventInfo.eighth == ((8 * 4) - 1)) {
        func_800155E4(scn->curline);
    }

    if (eventInfo.eighth == D_800A08EC->checkhigh) {
        EventUpdateImmHigh(scn);
    }
    return scn;
}

void EventSetMovieSubtitles(MOVIE_SUBTITLES *sub) {
    eventInfo.jptexttimer = 0;
    eventInfo.entexttimer = 0;
    eventInfo.jptext = NULL;
    eventInfo.entext = NULL;
    D_800827F8 = 0;
    if (sub != NULL) {
        D_800827D8 = sub->num;
        D_800827DC = sub->info;
        D_800827EC = sub->jp;
        D_800827E8 = sub->en;
    } else {
        D_800827D8 = 0;
        D_800827DC = NULL;
        D_800827EC = NULL;
    }
}

void EventSetFlashHigh(BOOL flashhigh) {
    eventInfo.flashhigh = flashhigh;
}

void EventCheckTextOverride(SCENE_INFO *scn) {
    if (D_80082784 == 1) {
        D_80082784 = 0;
        scn->drawtextlang = D_80082788;
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
        eventInfo.jptext = D_800827EC[info->seljp];
        eventInfo.jptexttimer = info->texttime / 2;
    } else {
        if ((info->seljp == -1) && (D_80082784 == FALSE)) {
            D_80082784 = TRUE;
            D_80082788 = scn->drawtextlang;
            scn->drawtextlang = TRUE;
        }
    }
    scn->jptext = eventInfo.jptext;

    if (info->selen > 0) {
        eventInfo.entext = D_800827E8[info->selen];
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
    eventInfo.controlstage = 1;
    eventInfo.controldelay = 0;

    MemoryZero(&sceneInfo, sizeof(sceneInfo));
    sceneInfo.leniency = 8;
    sceneInfo.level = LEVEL_NORM;
    sceneInfo.lvlhigh = 0;
    sceneInfo.notplayer = 0;
    sceneInfo.penalty = TRUE;
    sceneInfo.drawbuttons = TRUE;
    sceneInfo.teacherline.num = 0;
    sceneInfo.teacherline.keyid[0] = D_800A08FC->first;
    sceneInfo.teacherline.keyid[1] = D_800A08FC->second;
    sceneInfo.teacherline.enabled = FALSE;
    sceneInfo.parappaline.enabled = FALSE;
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
        RapPlayKey(&D_800A0928[0]);
    }
    scn->flags |= PR_SCN_FLAG_200;
    func_800146B4();
    func_8001564C(scn);
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
        func_8001564C(scn);
    } else if (eventInfo.coolstate == PR_COOLSTATE_NOTCOOL) {
        if (EventUpdateLevel(scn, dir) == TRUE) {
            return;
        }
        EventPlayScoreChange(dir);
        func_8001564C(scn);
    }
    if (dir == PR_DIR_UP) {
        D_800A0964(scn);
    }
}

void EventUpdateScore(register SCENE_INFO *scn) {
    if ((eventInfo.inpflags & (PR_INP_FLAG_1 | PR_INP_FLAG_2)) == 0) {
        return;
    }
    func_800158B8(scn);
    func_8001471C(scn);
}

void EventCheckGameOver(register SCENE_INFO *scn) {
    if (EventInputsInactive(scn) == TRUE) {
        return;
    }

    if ((eventInfo.inpflags & PR_INP_FLAG_10) && ((scn->level == 2) || (scn->level == 3))) {
        scn->flags |= PR_SCN_FLAG_40;
        scn->unk74 = TRUE;
        scn->drawtext = TRUE;
    } else if ((eventInfo.inpflags & PR_INP_FLAG_8) && (func_800147B4(scn) == TRUE)) {
        // PSP: "GAMEOVER at judge point"
        scn->flags |= PR_SCN_FLAG_40;
        scn->unk74 = TRUE;
        scn->drawtext = TRUE;
    }
}
