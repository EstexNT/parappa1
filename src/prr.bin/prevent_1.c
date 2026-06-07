#include "prevent.h"


s32 D_800827D8;
MOVIE_SUBTITLES_INFO *D_800827DC;
PARA_EN_T *D_800827E8;
PARA_JP_T *D_800827EC;
s32 D_800827F8;


MOVIE_SUBTITLES_INFO *EventGetMovieSubInfo(register PARA_TIME *t) {
    MOVIE_SUBTITLES_INFO *p;

    if (D_800827F8 >= D_800827D8) {
        return NULL;
    }
    p = D_800827DC + D_800827F8;
    if (
        (((p->start.min << 0x10) + (p->start.sec << 0x08) + p->start.frame) -
        ((t->min << 0x10) + (t->sec << 0x08) + t->frame))
        > 0
    ) {
        return NULL;
    }
    D_800827F8++;
    return p;
}

void EventCheckHighEnding(register SCENE_INFO *scn) {
    if (EventInputsInactive(scn) == TRUE) {
        return;
    }
    if ((eventInfo.inpflags & PR_INP_FLAG_20) == 0) {
        return;
    }
    if (func_8001490C(scn) == FALSE) {
        return;
    }
    eventInfo.coolstate = PR_COOLSTATE_LOSTCOOL;
}

BOOL EventCheckHighToNormal(register SCENE_INFO *scn) {
    if ((eventInfo.inpflags & PR_INP_FLAG_20) == 0) {
        return FALSE;
    }
    RapPlayKey(D_800A091C);
    scn->flags |= PR_SCN_FLAG_4000;
    EventSetHighTransitionEnd(scn);
    eventInfo.coolstate = PR_COOLSTATE_TRANSCOOL;
    return TRUE;
}

extern s16 D_800A08EA;

BOOL EventUpdateLevel(register SCENE_INFO *scn, register s32 dir) {
    s32 est;

    if ((eventInfo.inpflags & PR_INP_FLAG_4) == 0) {
        return FALSE;
    }

    est = EventCalcLvlEstimate(scn);
    EventResetEstimate(scn);
    if (est == PR_DIR_IDLE) {
        return FALSE;
    }
    if (scn->level == LEVEL_LOW2) {
        if (est == PR_DIR_DOWN) {
            return FALSE;
        }
    }
    if (scn->level == LEVEL_NORM) {
        if (est == PR_DIR_UP) {
            if (D_800A08EA == FALSE) {
                return FALSE;
            }
        }
    }
    RapPlayKey(&D_800A0928[est]);
    if ((scn->level == LEVEL_NORM) && (est == PR_DIR_UP)) {
        EventBeginHighMode(scn);
        scn->lvlhigh = TRUE;
        scn->teacherline.num = 0;
        eventInfo.coolstate = PR_COOLSTATE_IMMGOTCOOL;
        eventInfo.unk44 = TRUE;
    } else {
        eventInfo.coolstate = PR_COOLSTATE_IMMLOSTCOOL;
        scn->lvlhigh = FALSE;
    }
    EventTransitionLevel(scn, est);
    return TRUE;
}

typedef struct LVL_INFO {
    char *title;
    u32 channel;
    s16 neighbours[2];
} LVL_INFO;

extern LVL_INFO D_80068A3C[];

s32 EventCheckPerformance(register SCENE_INFO *scn) {
    s32 dir;
    BOOL tmp;
    s32 highcheck;

    if (scn->level == LEVEL_HIGH) {
        highcheck = func_800156FC(scn);
        if (highcheck == 0) {
            dir = PR_DIR_UP;
            scn->flashlevel = FALSE;
        } else if (highcheck == 1) {
            dir = PR_DIR_DOWN;
            scn->leveltoflash = LEVEL_NORM;
            scn->flashlevel = TRUE;
        } else if (highcheck == 2) {
            dir = PR_DIR_DOWN;
            scn->leveltoflash = LEVEL_LOW2;
            scn->flashlevel = TRUE;
        }
    } else {
        dir = func_80015838(scn);
        if (eventInfo.normaleval == 0) {
            tmp = ((scn->level == LEVEL_NORM) && (dir == PR_DIR_UP) && (eventInfo.flashhigh == FALSE));
            if (!tmp) {
                scn->flashlevel = TRUE;
                scn->leveltoflash = D_80068A3C[scn->level].neighbours[dir];
            }
            eventInfo.lvlestimate = dir;
            eventInfo.normaleval++;
        } else {
            if (eventInfo.lvlestimate != dir) {
                eventInfo.lvlestimate = PR_DIR_IDLE;
            }
            eventInfo.normaleval = 0;
            scn->flashlevel = FALSE;
        }
    }
    EventGetDispScore(scn);
    return dir;
}

void EventPlayScoreChange(register s32 dir) {
    RapPlayKey(&D_800A092C[dir]);
}

s32 EventCalcLvlEstimate(register SCENE_INFO *scn) {
    if (eventInfo.lvlestimate == PR_DIR_UP) {
        if (scn->level == LEVEL_NORM) {
            if (func_80014948(scn) == PR_DIR_UP) {
                return PR_DIR_UP;
            } else {
                return PR_DIR_IDLE;
            }
        } else {
            return PR_DIR_UP;
        }
    } else if (eventInfo.lvlestimate == PR_DIR_DOWN) {
        if (scn->level == LEVEL_LOW2) {
            if (func_80014948(scn) == PR_DIR_DOWN) {
                return PR_DIR_DOWN;
            } else {
                return PR_DIR_IDLE;
            }
        } else {
            return PR_DIR_DOWN;
        }
    } else {
        return PR_DIR_IDLE;
    }
}

void EventTransitionLevel(register SCENE_INFO *scn, register s32 dir) {
    if (scn->level == LEVEL_NORM) {
        if (dir == PR_DIR_UP) {
            // Good -> Cool
            scn->level = LEVEL_HIGH;
            scn->leveltransition = PR_SCN_LVLTRANS_5;
            scn->drawbuttons = FALSE;
        } else {
            // Good -> Bad
            scn->level = LEVEL_LOW1;
            scn->leveltransition = PR_SCN_LVLTRANS_1;
            scn->lownum++;
        }
    } else if (scn->level == LEVEL_LOW1) {
        if (dir == PR_DIR_UP) {
            // Bad -> Good
            scn->level = LEVEL_NORM;
            scn->leveltransition = PR_SCN_LVLTRANS_4;
        } else {
            // Bad -> Awful
            scn->level = LEVEL_LOW2;
            scn->leveltransition = PR_SCN_LVLTRANS_2;
        }
    } else if (scn->level == LEVEL_LOW2) {
        if (dir == PR_DIR_UP) {
            // Awful -> Bad
            scn->level = LEVEL_LOW1;
            scn->leveltransition = PR_SCN_LVLTRANS_3;
        }
    } 
    if (scn->level == LEVEL_HIGH) {
        if (dir == PR_DIR_DOWN) {
            // Cool -> Good
            scn->level = LEVEL_NORM;
            scn->leveltransition = PR_SCN_LVLTRANS_6;
        }
    }
}

void EventSetHighTransitionEnd(register SCENE_INFO *scn) {
    eventInfo.hightransitioning = TRUE;
}

void EventBeginHighMode(register SCENE_INFO *scn) {
    SND_INFO *snd;

    scn->flags |= PR_SCN_FLAG_2000;
    scn->highcnt++;

    snd = (scn->highcnt < 2) ? D_800A0920 : D_800A0924;
    RapPlayKey(snd);
    EventSetHighTransitionStart(scn, snd->time);
}

void EventSetHighTransitionStart(register SCENE_INFO *scn, register s32 time) {
    eventInfo.highstarttime = scn->gametime + time;
    eventInfo.hightransitioning = TRUE;
}
