#include "prmenu.h"
#include <string.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include "prrap.h"
#include "prapp.h"
#include "prcompo.h"

static char rcsid[] = "@(#)prmenu.c: version 01-00 95/10/10 00:00:00";


typedef struct MENU_RUNNER_DEF {
    void (*init)(struct MENU_RUNNER_DEF *def, s32 menu, void *data);
    s32 (*input)(u32 pad, void *defdata, void *data);
    BOOL (*needmain)(void *defdata);
    void (*update)(void);
    void *defdata;
} MENU_RUNNER_DEF;


extern s32 D_80082790;
extern s32 D_80082794;

s32 MenuRunnerHighScoreInput(u32 pad, void *defdata, void *data);
void MenuRunnerStartUpdate(void);
s32 MenuRunnerStartInput(u32 pad, void *defdata, void *data);
s32 MenuRunnerStageInput(u32 pad, void *defdata, void *data);
s32 MenuRunnerDebugInput(u32 pad, void *defdata, void *data);
void MenuRunnerDrawDebugMenu(MENU_DEBUG_DEFDATA *defdata);
void func_80033df4(void *defdata);
void func_80033f70(void *defdata);
s32 MenuRunnerMainInput(u32 pad, void *defdata, void *data);
s32 func_8003480c(u32 pad, void *defdata, void *data);
void MenuUpdateHighlight(BOOL *highlight);
void MenuRunnerMainInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
void MenuRunnerStartInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
void MenuRunnerHighScoreInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
void MenuRunnerLoadingInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
void MenuRunnerDebugInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
void MenuRunnerStageInit(MENU_RUNNER_DEF *def, s32 menu, void *data);
s32 MenuRunnerRun(MENU_RUNNER_DEF *def, s32 menu, void *data);
MENU_RUNNER_DEF *MenuGetRunnerDef(s32 menu, void *data);
void MenuRunnerVSyncCallback(void);
void MenuRunnerDrawDebug(s32 menu, void *defdata);


MENU_HIGHSCORE_DEFDATA menuRunnerHighScoreDefData = {
    0, 0, 0, 
    6, 3, 19, 18,
    {0}
};
MENU_DEBUG_ENTRY menuDebugEntries[] = {
    {2, "Threshhold",
        {"Strict", "About"},
        {1,        0}},
    {2, "ClickSound",
        {"NO",  "YES"},
        {FALSE, TRUE}},
    {2, "HIGH CLEAR",
        {"False", "True"},
        {FALSE,   TRUE}},
    {3, "PLAY MODE ",
        {"Normal",    "Demo",      "Replay"},
        {PLAY_MODE_0, PLAY_MODE_1, PLAY_MODE_2}},
    {2, "PENALTY   ",
        {"YES", "No"},
        {TRUE,  FALSE}},
    {2, "DBGMSG    ",
        {"NO",  "YES"},
        {FALSE, TRUE}},
    {2, "TEST MODE ",
        {"NO",  "YES"},
        {FALSE, TRUE}},
    {3, "MINUS PNT ",
        {"-3", "-5", "-6"},
        {-3,   -5,   -6}},
    {3, "PRINT MODE",
        {"DRAW", "EVAL", "APP"},
        {0,      1,      2}},
    {2, "ERASE GUI ",
        {"NO",  "YES"},
        {FALSE, TRUE}},
};
s32 menuDebugEntriesNum = NELEM(menuDebugEntries);
MENU_MAIN_DEFDATA menuRunnerMainDefData = {
    0, 0, 0, 5,
    {{0, 2}, {0, 1}, {0, 2}, {0, 4}, {0, 1}}
};

extern MENU_STAGE_DEFDATA menuRunnerStageDefData;
extern MENU_START_DEFDATA menuRunnerStartDefData;
extern MENU_DEBUG_DEFDATA menuRunnerDebugDefData;

MENU_RUNNER_DEF menuRunnerDefStage = {
    MenuRunnerStageInit, MenuRunnerStageInput, NULL, NULL, &menuRunnerStageDefData
};
MENU_RUNNER_DEF menuRunnerDefMain = {
    MenuRunnerMainInit, MenuRunnerMainInput, NULL, NULL, &menuRunnerMainDefData
};
MENU_RUNNER_DEF menuRunnerDefStart = {
    MenuRunnerStartInit, MenuRunnerStartInput, NULL, MenuRunnerStartUpdate, &menuRunnerStartDefData
};
MENU_RUNNER_DEF menuRunnerDefHighScore = {
    MenuRunnerHighScoreInit, MenuRunnerHighScoreInput, NULL, NULL, &menuRunnerHighScoreDefData
};
MENU_RUNNER_DEF menuRunnerDefLoading = {
    MenuRunnerLoadingInit, NULL, NULL, NULL, NULL
};
MENU_RUNNER_DEF menuRunnerDefDebug = {
    MenuRunnerDebugInit, MenuRunnerDebugInput, NULL, NULL, &menuRunnerDebugDefData
};



BOOL func_800330a8(u32 arg0) {
    return (--D_80082790 < 0) ? TRUE : FALSE;
}

void MenuPlayButtonSound(u32 pad) {
    SND_INFO *snd;

    if (pad == PR_PAD_NONE) {
        return;
    }

    switch (pad) {
        case PR_PAD_CIRCLE: {
            snd = sceneInitInfo.menucircle;
            break;
        }
        case PR_PAD_CROSS: {
            snd = sceneInitInfo.menucross;
            break;
        }
        case PR_PAD_SELECT: {
            snd = sceneInitInfo.menuselect;
            break;
        }
        case PR_PAD_UP:
        case PR_PAD_RIGHT:
        case PR_PAD_DOWN:
        case PR_PAD_LEFT: {
            snd = sceneInitInfo.menudpad;
            break;
        }
        default: {
            return;
        }
    }
    RapPlayKey(snd);
    RapFlush();
}

s32 MenuRunnerHighScoreInput(u32 pad, void *defdata, void *data) {
    MENU_HIGHSCORE_DEFDATA *d;

    d = (MENU_HIGHSCORE_DEFDATA *)defdata;

    if (pad == PR_PAD_CROSS) {
        d->unk4 = 1;
        MenuPlayButtonSound(PR_PAD_CIRCLE);
        return 1;
    } else {
        return 0;
    }
}

void MenuRunnerStartUpdate(void) {
    if (D_80082794 == 0) {
        RapPlayKey(&sceneInitInfo.tryagain[0]);
        RapFlush();
    } else if (D_80082794 == 36) {
        RapPlayKey(&sceneInitInfo.tryagain[1]);
        RapFlush();
    }
    if (D_80082794 == 72) {
        D_80082794 = 0;
    } else {
        D_80082794++;
    }
}

s32 MenuRunnerStartInput(u32 pad, void *defdata, void *data) {
    if (pad == PR_PAD_CIRCLE) {
        MenuPlayButtonSound(pad);
        ((MENU_START_DEFDATA *)(defdata))->unk0 = 0;
        return 1;
    } else if (pad == PR_PAD_CROSS) {
        MenuPlayButtonSound(pad);
        ((MENU_START_DEFDATA *)(defdata))->unk0 = 1;
        return 2;
    }
    return 0;
}

s32 MenuRunnerStageInput(u32 pad, void *defdata, void *data) {
    u32 unused;
    MENU_STAGE_DEFDATA *d;
    s32 sel;

    d = (MENU_STAGE_DEFDATA *)defdata;
    if ((pad == PR_PAD_RIGHT) || (pad == PR_PAD_DOWN)) {
        MenuPlayButtonSound(pad);
        sel = (d->selcur % d->selmax) + 1;
        while (d->icons[sel] == STAGECLEAR_0) {
            sel = (sel % d->selmax) + 1;
        }
        d->selcur = sel;
    } else if ((pad == PR_PAD_LEFT) || (pad == PR_PAD_UP)) {
        MenuPlayButtonSound(pad);
        sel = d->selcur;
        sel = (--sel > 0) ? sel : d->selmax;
        while (d->icons[sel] == STAGECLEAR_0) {
            sel = (--sel > 0) ? sel : d->selmax;
        }
        d->selcur = sel;
    } else if ((pad == PR_PAD_CIRCLE) && (d->selcur > 0) && (d->selcur < 8)) {
        MenuPlayButtonSound(PR_PAD_CIRCLE);
        if ((d->selcur >= 1) && (d->selcur <= 6)) {
            *((s32 *)(data)) = d->selcur;
        } else {
            d->selected = TRUE;
            *((s32 *)(data)) = 8;
        }
        return 1;
    } else if ((pad == PR_PAD_CROSS) && (d->selcur == 8)) {
        d->selected = TRUE;
        MenuPlayButtonSound(PR_PAD_CIRCLE);
        return 2;
    }
    return 0;
}

s32 MenuRunnerDebugInput(u32 pad, void *defdata, void *data) {
    s32 sel;
    s32 entrysel;
    MENU_DEBUG_DEFDATA *d;
    MENU_DEBUG_ENTRY *entries;

    d = (MENU_DEBUG_DEFDATA *)defdata;
    entries = d->entries;
    MenuPlayButtonSound(pad);
    sel = d->selcur;
    entrysel = entries[sel].selcur;
    if (pad == PR_PAD_UP) {
        sel = (--sel >= 0) ? sel : (d->entriesnum - 1);
        d->selcur = sel;
    } else if (pad == PR_PAD_DOWN) {
        sel = ++sel % d->entriesnum;
        d->selcur = sel;
    } else if (pad == PR_PAD_LEFT) {
        entrysel = (--entrysel >= 0) ? entrysel : (entries[sel].selmax - 1);
        entries[sel].selcur = entrysel;
    } else if (pad == PR_PAD_RIGHT) {
        entrysel = ++entrysel % entries[sel].selmax;
        entries[sel].selcur = entrysel;
    } else if (pad == PR_PAD_CROSS) {
        return 2;
    } else if (pad == PR_PAD_CIRCLE) {
        return 1;
    }
    return 0;
}

void MenuRunnerDrawDebugMenu(MENU_DEBUG_DEFDATA *defdata) {
    s32 i;
    s32 j;
    long optlen;
    u32 unused;
    char opttemp[64];
    u32 unused1;
    u32 unused2;
    MENU_DEBUG_ENTRY *entry;

    entry = &defdata->entries[0];
    FntPrint("\n\n\n");
    for (i = 0; i < defdata->entriesnum; i++, entry++) {
        if (defdata->selcur == i) {
            FntPrint("~c888 >>>");
        } else {
            FntPrint("~c000    ");
        }
        FntPrint("%s:", entry->title);
        for (j = 0; j < entry->selmax; j++) {
            strcpy(opttemp, (j == entry->selcur) ? "~c888*" : "~c000 ");
            strcat(opttemp, entry->opt[j]);
            if ((optlen = strlen(entry->opt[j])) < 7) {
                for (optlen = 7 - optlen; optlen > 0; optlen--) {
                    strcat(opttemp, " ");
                }
            }
            FntPrint(opttemp);
        }
        FntPrint("\n");
    }
    FntPrint("\n\n~c222      O: OK   X: CANCEL~c888\n");
    FntFlush(-1);
}

void func_80033df4(void *defdata) {
    MENU_HIGHSCORE_DEFDATA *d;
    s32 i;
    s32 j;
    s32 n;

    d = (MENU_HIGHSCORE_DEFDATA *)defdata;
    FntPrint("\n\n\n\n\n\n\n~c222");
    for (i = 0, n = 0; i < d->unkC; i++) {
        FntPrint("  ");
        for (j = 0; j < d->unkE; j++, n++) {
            FntPrint("[%s]", d->unk14[n]);
        }
        if (i == 2) {
            FntPrint("\n\n\n");
        } else {
            FntPrint("\n\n");
        }
    }
    FntPrint("\n\n\n\n\n\n            HIT O KEY TO CONTINUE...\n");
}

void func_80033f70(void *defdata) {

}

void MenuUpdateDebugOptions(SCENE_INFO *scn) {
    MENU_DEBUG_ENTRY *entry;

    entry = &menuDebugEntries[0];
    scn->threshold = entry->val[entry->selcur]; entry++;
    scn->clicksound = entry->val[entry->selcur]; entry++;
    entry++;
    if (appInfo.playmode == PLAY_MODE_0) {
        appInfo.playmode = entry->val[entry->selcur]; 
    } entry++;
    scn->penalty = entry->val[entry->selcur]; entry++;
    appInfo.dbgmsg = entry->val[entry->selcur]; entry++;
    entry++;
    appInfo.minuspnt = entry->val[entry->selcur]; entry++;
    appInfo.printmode = entry->val[entry->selcur]; entry++;
    appInfo.erasegui = entry->val[entry->selcur]; entry++;

}

s32 MenuRunnerMainInput(u32 pad, void *defdata, void *data) {
    MENU_MAIN_DEFDATA *d;
    APP_INFO *info;
    s16 *sp18;

    d = (MENU_MAIN_DEFDATA *)defdata;
    info = (APP_INFO *)data;
    if (d->selcur == 0) {
        if (pad == PR_PAD_CIRCLE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 0;
            info->lang = 0;
        } else if (pad == PR_PAD_CROSS) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 1;
            info->lang = 1;
        }
    } else if (d->selcur == 2) {
        if (pad == PR_PAD_CIRCLE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 0;
            info->difficulty = 0;
        } else if (pad == PR_PAD_CROSS) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 1;
            info->difficulty = 1;
        }
    } else if (d->selcur == 1) {
        if (pad == PR_PAD_CIRCLE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 0;
            return 1;
        }
    } else if (d->selcur == 3) {
        if (pad == PR_PAD_SQUARE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 0;
            return 3;
        } else if (pad == PR_PAD_CROSS) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 1;
            return 4;
        } else if (pad == PR_PAD_CIRCLE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 2;
            return 2;
        } else if (pad == PR_PAD_TRIANGLE) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 3;
            return 6;
        }
    } else if (d->selcur == 4) {
        if (pad == PR_PAD_CROSS) {
            MenuPlayButtonSound(PR_PAD_CIRCLE);
            d->selval[d->selcur][0] = 0;
            d->selexit = TRUE;
            return 7;
        }
    }
    sp18 = d->selval[d->selcur];
    if ((pad == PR_PAD_RIGHT) || (pad == PR_PAD_DOWN)) {
        MenuPlayButtonSound(PR_PAD_RIGHT);
        d->selcur = (d->selcur + 1) % d->selmax;
    } else if ((pad == PR_PAD_LEFT) || (pad == PR_PAD_UP)) {
        MenuPlayButtonSound(PR_PAD_RIGHT);
        d->selcur = (((d->selcur - 1) >= 0) ? d->selcur : d->selmax) - 1;
    }
    return 0;
}

s32 func_8003480c(u32 pad, void *defdata, void *data) {
    MenuPlayButtonSound(pad);
    if (pad == PR_PAD_CIRCLE) {
        return 1;
    } else if (pad == PR_PAD_CROSS) {
        return 2;
    } else {
        return 0;
    }
}

s32 D_8008278C;

void MenuUpdateHighlight(BOOL *highlight) {
    BOOL *temph;

    temph = highlight;
    if (D_8008278C == 0) {
        *temph = (*temph != TRUE) ? TRUE : FALSE;
        D_8008278C++;
    } else if (D_8008278C == 19) {
        D_8008278C = 0;
    } else {
        D_8008278C++;
    }
}

void MenuRunnerMainInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    MENU_MAIN_DEFDATA *d;
    APP_INFO *info;

    d = (MENU_MAIN_DEFDATA *)def->defdata;
    info = (APP_INFO *)data;
    d->selcur = 0;
    d->selval[1][0] = -1;
    d->selval[3][0] = -1;
    d->selexit = FALSE;
    info->lang = d->selval[0][0];
    info->difficulty = (d->selval[2][0] != 0) ? TRUE : FALSE;
}

void MenuRunnerStartInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    MENU_START_DEFDATA *d;

    d = (MENU_START_DEFDATA *)def->defdata;
    D_80082794 = 0;
    d->unk0 = -1;
}

void MenuRunnerHighScoreInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    MENU_HIGHSCORE_DEFDATA *d;

    d = (MENU_HIGHSCORE_DEFDATA *)(def->defdata = data);
    d->unk4 = 0;
}

void MenuRunnerLoadingInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    void *defdata;

    defdata = def->defdata;
}

void MenuRunnerDebugInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    MENU_DEBUG_DEFDATA *d;

    d = (MENU_DEBUG_DEFDATA *)def->defdata;
    d->entries = menuDebugEntries;
    d->selcur = 0;
    d->entriesnum = menuDebugEntriesNum;
}

void MenuRunnerStageInit(MENU_RUNNER_DEF *def, s32 menu, void *data) {
    s32 stageclear;
    s32 highcount;
    MENU_STAGE_DEFDATA *d;
    s32 i;

    d = (MENU_STAGE_DEFDATA *)def->defdata;
    d->selcur = 1;
    d->selmax = 8;
    d->highlight = FALSE;
    d->selected = FALSE;
    if (appInfo.nosave == TRUE) {
        for (i = 0; i < 9; i++) {
            d->icons[i] = STAGECLEAR_1;
        }
        d->icons[5] = STAGECLEAR_2;
        d->icons[6] = STAGECLEAR_3;
    } else if (appInfo.difficulty == TRUE) {
        d->icons[1] = STAGECLEAR_1;
        for (i = 2; i < 8; i++) {
            d->icons[i] = STAGECLEAR_0;
        }
        d->icons[8] = STAGECLEAR_1;
    } else {
        highcount = 0;
        for (i = 1; i < 8; i++) {
            stageclear = D_800A2174.save.stageclear[i - 1];
            d->icons[i] = stageclear;
            if (stageclear == STAGECLEAR_3) {
                highcount++;
            }
        }
        d->icons[7] = (highcount >= 6) ? STAGECLEAR_1 : STAGECLEAR_0;
        d->icons[8] = STAGECLEAR_1;
    }
}

s32 MenuRunnerProcess(s32 menu, void *data) {
    s32 ret;
    MENU_RUNNER_DEF *def;

    def = MenuGetRunnerDef(menu, data);
    if (def == NULL) {
        return 0;
    }
    def->init(def, menu, data);
    ret = MenuRunnerRun(def, menu, data);
    VSyncCallback(NULL);
    return ret;
}

s32 MenuRunnerRun(register MENU_RUNNER_DEF *def, register s32 menu, register void *data) {
    u32 padprev;
    u32 padcurr;
    s32 ret;
    s32 i;
    BOOL runnerended;

    if (menu == PR_MENU_MAIN) {
        CompoFrameMake(NULL, PR_FRAME_7, COMPO_FRAME_TYPE_3, COMPO_FRAME_TYPE_1);
    }
    VSyncCallback(MenuRunnerVSyncCallback);
    while (PadRead(1) != 0);

    runnerended = FALSE;
    padprev = 0;
    for (i = 60; i > 0; i) {
        if (runnerended == FALSE) {
            padcurr = PadRead(1);
            if (padprev != padcurr) {
                ret = def->input(padcurr, def->defdata, data);
                if (ret != 0) {
                    runnerended = TRUE;
                }
            }
        }
        if (def->needmain != NULL) {
            if (def->needmain(def->defdata) == TRUE) {
                ret = 3;
                break;
            }
        }
        if ((menu == PR_MENU_MAIN) || (menu == PR_MENU_HIGHSCORE) || (menu == PR_MENU_STAGE)) {
            MenuUpdateHighlight((BOOL *)def->defdata);
        }
        if (def->update != NULL) {
            def->update();
        }
        if (runnerended == TRUE) {
            i--;
        }

        padprev = padcurr;
        CompoDrawMenu(menu, def->defdata);
        MenuRunnerDrawDebug(menu, def->defdata);
        VSync(0);
        CompoDrawMenuOt(menu, def->defdata);
    }
    VSyncCallback(NULL);
    return ret;
}

const char D_80010770[] = "\n\n\n~c000StageClear: ";

MENU_RUNNER_DEF *MenuGetRunnerDef(register s32 menu, register void *data) {
    switch (menu) {
        case PR_MENU_STAGE: {
            return &menuRunnerDefStage;
        }
        case PR_MENU_MAIN: {
            return &menuRunnerDefMain;
        }
        case PR_MENU_START: {
            return &menuRunnerDefStart;
        }
        case PR_MENU_HIGHSCORE: {
            return &menuRunnerDefHighScore;
        }
        case PR_MENU_LOADING: {
            return &menuRunnerDefLoading;
        }
        case PR_MENU_DEBUG: {
            return &menuRunnerDefDebug;
        }
        default: {
            return NULL;
        }
    }
}

void MenuRunnerVSyncCallback(void) {
    RapFlush();
}

void MenuRunnerDrawDebug(register s32 menu, register void *defdata) {
    s32 i;

    switch (menu) {
        case PR_MENU_MAIN: {
            break;
        }
        case PR_MENU_STAGE: {
            if (appInfo.dbgmsg != FALSE) {
                FntPrint(D_80010770);
                for (i = 0; i < 6; i++) {
                    FntPrint("%d", D_800A2174.save.stageclear[i]);
                }
            }
            break;
        }
        case PR_MENU_DEBUG: {
            MenuRunnerDrawDebugMenu((MENU_DEBUG_DEFDATA *)defdata);
            break;
        }
        default: {
            break;
        }
    }
}
