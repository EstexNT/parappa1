#include "prpract.h"
#include <libetc.h>
#include "prrap.h"
// #include "prcompo.h"
#include "prapp.h"

static char rcsid[] = "@(#)prpract.c: version 01-00 95/10/10 00:00:00";

void PracticePlaySound(SCENE_INFO *scn, SND_INFO *snd, s32 n, s32 draw);
void PracticeRunMain(SCENE_INFO *scn);
BOOL PracticeRunMainLoop(SCENE_INFO *scn);
BOOL PracticeRunMainTryAgain(SCENE_INFO *scn);
void PracticeResetLine(SCENE_INFO *scn);
u32 PracticeReadPadTrigger(void);
s32 PracticeRunMainLoopStage(SCENE_INFO *scn, s32 stage);
s32 PracticeEvalTap(s32 time, s32 target);
void PracticeSetVisualLineInfo(SCENE_INFO *scn, s32 i, s32 turn, s32 stage);


SND_INFO practSndStageStart = {
    1, 0, 24 + 0, 90,
    150
};
SND_INFO practSndStageMusic = {
    1, 1, 24 + 1, 90,
    75
};
SND_INFO practSndTryAgain = {
    3, 0, 24 + 0, 110,
    95
};
SND_INFO practSndFinished = {
    3, 4, 24 + 4, 110,
    88
};

s32 practInputTime[] = {0, 19, 37, 56};
SND_INFO practSndInput[] = {
    {2, 0, 24 + 0, 90, 0},
    {2, 1, 24 + 1, 90, 0},
    {2, 2, 24 + 2, 90, 0},
    {2, 3, 24 + 3, 90, 0},
};
SND_INFO practSndTeacherStageStart[] = {
    {3, 3, 24 + 3, 110, 80},
    {3, 6, 24 + 6, 110, 46},
    {3, 2, 24 + 2, 110, 42},
    {3, 5, 24 + 5, 110, 49},
};
SND_INFO practSndTeacherResponse[] = {
    {3, 1, 24 + 1, 110, 0},
    {3, 7, 24 + 7, 110, 0},
    {3, 8, 24 + 8, 110, 0},
};

VISUAL_LINE_INFO practVisualLineInfo[4] = {
    {1, 0, 
    {PR_TAP_NONE, PR_TAP_NONE, PR_TAP_TRIANGLE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_ICON_END, PR_TAP_NONE},
    },
    {1, 0, 
    {PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_TRIANGLE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_ICON_END, PR_TAP_NONE},
    },
    {1, 0, 
    {PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_TRIANGLE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_ICON_END, PR_TAP_NONE},
    },
    {1, 0, 
    {PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, PR_TAP_NONE, 
     PR_TAP_NONE, PR_TAP_NONE, PR_TAP_TRIANGLE, PR_TAP_NONE, 
     PR_TAP_ICON_END, PR_TAP_NONE},
    }
};


void PracticeMainVSyncCallback(void) {
    RapFlush(); 
}

void func_80036468(s32 n) {
    for (n; n > 0; n--) {
        VSync(2);
    }
}

void func_800364d4(u32 arg0) {

}

void PracticePlaySound(SCENE_INFO *scn, SND_INFO *snd, s32 n, s32 draw) {
    s32 i;

    if (draw != 3) {
        scn->flags = PR_SCN_FLAG_400000;
        scn->practdraw = draw;
    }
    RapPlayKey(snd);
    for (i = n; i > 0; i--) {
        CompoDrawMenu(PR_MENU_PRACTICE, scn);
        VSync(2);
        CompoDrawMenuOt(0);
    }
}

void PracticeRun(SCENE_INFO *scn, s32 i) {
    AppReadS0YCompo(i);
    RapStopSeq();
    PracticeRunMain(scn);
    AppReadZCompo(i);
}

void PracticeRunMain(register SCENE_INFO *scn) {
    BOOL loopret;
    VSyncCallback(PracticeMainVSyncCallback);
    while (TRUE) {
        EventReset();
        loopret = PracticeRunMainLoop(scn);
        if (loopret == TRUE) {
            PracticePlaySound(scn, &practSndFinished, practSndFinished.time, 7);
        }
        if (PracticeRunMainTryAgain(scn) == FALSE) {
            break;
        }
    }
    VSyncCallback(NULL);
}

BOOL PracticeRunMainLoop(register SCENE_INFO *scn) {
    s32 stagedone;
    s32 j;
    s32 i;
    s32 draw;
    s32 startdraw;
    SND_INFO *snd;

    for (i = 0; i < 4; i++) {
        CompoDrawMenu(PR_MENU_PRACTICE, scn);
        VSync(2);
        CompoDrawMenuOt(0);
    }
    stagedone = 0;
    for (j = 0; j < 4; j++) {
        snd = &practSndTeacherStageStart[j];
        startdraw = ((s16[]){3, 4, 5, 6})[j];

        PracticePlaySound(scn, snd, snd->time, startdraw);
        do {
            draw = PracticeRunMainLoopStage(scn, j);
            if (draw >= 0) {
                snd = &practSndTeacherResponse[draw];
                PracticeResetLine(scn);
                PracticePlaySound(scn, snd, 75, draw);
            }
        } while ((draw > 0) && (draw != -1));
        if (draw == -1) {
            break;
        } else if (draw == 0) {
            stagedone++;
        }
    }
    return (stagedone == 4) ? TRUE : FALSE;
}

BOOL PracticeRunMainTryAgain(register SCENE_INFO *scn) {
    s32 highlighttime;
    BOOL choicemade;
    s32 delay;
    BOOL ret;
    u32 pad;

    PracticePlaySound(scn, &practSndTryAgain, practSndTryAgain.time, 8);
    RapFlush();

    choicemade = 0;
    delay = 15;
    scn->practend = FALSE;
    highlighttime = 0;
    while (delay > 0) {
        CompoDrawMenu(PR_MENU_PRACTICE, scn);
        VSync(2);
        CompoDrawMenuOt(0);
        pad = PracticeReadPadTrigger();
        if (pad == PR_PAD_CIRCLE) {
            choicemade = TRUE;
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            ret = TRUE;
        } else if (pad == PR_PAD_CROSS) {
            scn->practend = TRUE;
            scn->practhighlight = TRUE;
            choicemade = TRUE;
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            ret = FALSE;
        }
        if (choicemade == TRUE) {
            delay--;
        }
        if (highlighttime == 0) {
            scn->practhighlight = (scn->practhighlight != TRUE) ? TRUE : FALSE;
            highlighttime++;
        } else if (highlighttime == 19) {
            highlighttime = 0;
        } else {
            highlighttime++;
        }
    }
    return ret;
}

void PracticeResetLine(register SCENE_INFO *scn) {
    scn->teacherline.num = 0;
    scn->teacherline.dot[0] = -1;
    scn->teacherline.enabled = 0;
    scn->parappaline.dot[0] = -1;
    scn->parappaline.enabled = 0;
    CompoLineButtonInitScalingTeacher();
}

u32 D_800827BC;

u32 PracticeReadPadTrigger(void) {
    u32 pad;

    pad = PadRead(1);
    if (pad != D_800827BC) {
        return (D_800827BC = pad);
    } else {
        return 0;
    }
}

s32 PracticeRunMainLoopStage(register SCENE_INFO *scn, register s32 stage) {
    u32 pad;
    s32 i;
    s32 time;
    s32 timetarget;
    s32 turn;
    s32 response;
    s32 evali;
    BOOL pressed;
    BOOL dontupdate;

    PracticeResetLine(scn);
    RapPlayKey(&practSndStageStart);
    for (i = 112; i < 150; i++) {
        PracticeSetVisualLineInfo(scn, i, 2, stage);
        CompoDrawMenu(PR_MENU_PRACTICE, scn);
        VSync(2);
        CompoDrawMenuOt(0);
    }

    time = practInputTime[stage];
    timetarget = time + 75;
    response = -1;
    evali = 0;
    dontupdate = FALSE;
    pressed = FALSE;
    for (i = 0; i < 150; i++) {
        if ((pressed == FALSE) && (dontupdate == FALSE) && (i >= 149)) {
            i = 0;
        }
        if ((dontupdate == FALSE) && ((i % 75) == 0)) {
            RapPlayKey(&practSndStageMusic);
        }
        turn = (i < 73) ? 1 : 2;
        pad = PracticeReadPadTrigger();
        if (pad != 0) {
            if ((dontupdate == FALSE) && (pad == PR_PAD_CROSS)) {
                RapStopSeq();
                MenuPlayButtonSound(PR_PAD_CIRCLE);
                i = 135;
                pressed = TRUE;
                dontupdate = TRUE;
            }
            if ((pad == PR_PAD_TRIANGLE) && (turn == 2) && (dontupdate == FALSE) && (pressed == FALSE)) {
                RapPlayKey(&practSndInput[stage]);
                response = PracticeEvalTap(i, timetarget);
                pressed = TRUE;
                evali = i;
            }
        }

        if ((turn == 1) && (i == time)) {
            RapPlayKey(&practSndInput[stage]);
        }
        if (dontupdate == FALSE) {
            PracticeSetVisualLineInfo(scn, i, turn, stage);
        }
        CompoDrawMenu(PR_MENU_PRACTICE, scn);
        VSync(2);
        CompoDrawMenuOt(0);
    }
    return response;
}

s32 PracticeEvalTap(register s32 time, register s32 target) {
    if ((time >= (target - 2)) && (time <= (target + 2))) {
        return 0; // On time
    }
    if (time < (target - 2)) {
        return 1; // Early
    } else {
        return 2; // Late
    }
}

void PracticeSetVisualLineInfo(register SCENE_INFO *scn, register s32 i, register s32 turn, register s32 stage) {
    scn->flags = 0;
    if (i == 135) {
        scn->flags |= PR_SCN_FLAG_800;
        scn->teacherline.num = practVisualLineInfo[stage].linenum;
        scn->teacherline.dot[0] = practVisualLineInfo[stage].start;
        scn->teacherline.enabled = 1;
        scn->teacherline.keyid[0] = practVisualLineInfo[stage].first;
    } else if (i == 60) {
        scn->flags |= PR_SCN_FLAG_200000;
        scn->parappaline.enabled = 1;
        scn->parappaline.dot[0] = 0;
        scn->parappaline.keyid[0] = practVisualLineInfo[stage].first;
    }

    if ((i % 5) != 0) {
        return;
    }
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
    if (scn->parappaline.enabled == 1) {
        if ((scn->parappaline.dot[0] <= 18) && 
            (scn->parappaline.keyid[0][scn->parappaline.dot[0]] != PR_TAP_ICON_END)) {
            scn->parappaline.dot[0]++;
        } else {
            scn->parappaline.enabled = 0;
            scn->parappaline.dot[0] = -1;
            CompoLineButtonInitScalingTeacher();
        }
    }
}
