#include "prcmenu.h"
#include <string.h>
#include <stdio.h>
#include <libetc.h>
// #include "prcompo.h"


// static char rcsid[] = "@(#)prcmenu.c: version 01-00 96/08/01 00:00:00";

extern char D_80089BA4[32];
extern CARD_MENU_SLOT_INFO D_80089BC4;
extern CARD_DATA D_8008A21C;
extern char D_8008C21C[32];
extern char D_8008C23C[64 + 8];
extern struct MENU_MAIN_DEFDATA D_8008C284;
extern struct DIRENTRY D_8008C2A4[15];
extern CARD_MENU_DIALOG_INFO D_8008C4FC;

// extern u32 D_80082708;
extern BOOL D_8008270C;
extern s32 D_80082710;
extern BOOL D_80082714;
extern BOOL D_80082718;
extern BOOL D_8008271C;
extern s32 D_80082720;
extern void *D_80082724;
extern BOOL D_80082728;
extern s32 D_8008272C;
extern s32 D_80082730;

extern char D_8006269C[58];
extern CARD_MENU_NAMEENTER_INFO D_800626D8;
extern CARD_MENU_SLOTSELECT_INFO D_80062700;
extern u16 D_80062798[96];
extern HIGH_SCORE_VISUAL D_80062858[20];

extern const char D_80010208[];
extern const char D_80010218[];
extern const char D_80010238[];
extern const char D_80010248[];
extern const char D_8001025C[];
extern const char D_80010278[];
extern const char D_8001028C[];


void CardMenuDrawTimed(s32 menu, void *data, s32 time) {
    for (time; time > 0; time--) {
        CompoDrawMenu(menu, data);
        VSync(0);
        CompoDrawMenuOt(0);
    }
}

void CardMenuDrawSlow(s32 menu, void *data) {
    if (D_80082728 == FALSE) {
        CompoDrawMenu(menu, data);
        D_80082728++;
    } else {
        CompoDrawMenuOt(0);
        D_80082728 = FALSE;
    }
}

void CardMenuHighScoreVSyncCallback(void) {
    if (D_8008272C == 0) {
        D_8008C284.highlight = (D_8008C284.highlight != TRUE);
        CardMenuDrawSlow(PR_MENU_MAIN, &D_8008C284);
        D_8008272C++;
    } else if (D_8008272C == 1) {
        CardMenuDrawSlow(PR_MENU_MAIN, &D_8008C284);
        D_8008272C++;
    } else if (D_8008272C == 19) {
        D_8008272C = 0;
    } else {
        D_8008272C++;
    }
}

void CardMenuResetBaseInfo(CARD_MENU_BASE_INFO *base) {
    base->highlight = TRUE;
    base->sel = 0;
    base->saving = FALSE;
}

void CardMenuUpdateSaving(s32 state) {
    if (state == CARD_STATE_F_NAMEENTER) {
        D_800626D8.base.saving = TRUE;
        CardMenuDrawTimed(PR_MENU_NAMEENTER, &D_800626D8, 20);
        CardMenuResetBaseInfo(&D_800626D8.base);
    } else if (state == CARD_STATE_F_SLOTSELECTSAVE) {
        D_80062700.base.saving = TRUE;
        D_80062700.base.sel = 1;
        CardMenuDrawTimed(PR_MENU_SAVE, &D_80062700, 20);
        CardMenuResetBaseInfo(&D_80062700.base);
    }
}

void CardMenuUpdateHighlight(CARD_MENU_BASE_INFO *base) {
    if (D_80082730 == 0) {
        base->highlight = (base->highlight != TRUE);
        D_80082730++;
    } else if (D_80082730 >= 19) {
        D_80082730 = 0;
    } else {
        D_80082730++;
    }
}

// INCLUDE_ASM("asm/prr.bin/nonmatchings/prcmenu", CardMenuSlotSelectUpdate);
s32 CardMenuSlotSelectUpdate(u32 pad, CARD_INFO *info) {
    CARD_MENU_SLOTSELECT_INFO *slotselectinfo;
    s32 sel;
    s32 i;
    SLOT_INFO *slotinfo;

    slotselectinfo = &D_80062700;
    slotinfo = &D_80089BC4.info[0];
    sel = slotselectinfo->selcur;

    if ((pad == PR_PAD_CROSS) && (sel == (slotselectinfo->selmax - 1))) {
        func_800330F8(PR_PAD_CIRCLE);
        return 2;
    }

    if ((pad == PR_PAD_CIRCLE) && (sel < (slotselectinfo->selmax - 1))) {
        func_800330F8(PR_PAD_CIRCLE);
        if (slotinfo[sel].fname[0] != '\0') {
            D_80082714 = TRUE;
            strcpy(D_8008C21C, slotinfo[sel].fname);
        }
        return 1;
    }

    if (pad == PR_PAD_UP) {
        func_800330F8(pad);
        if (sel == (slotselectinfo->selmax - 1)) {
            for (i = sel - 1; (i > 0) && (slotselectinfo->available[i] == FALSE); i--) {

            }
            if (slotselectinfo->available[i] == TRUE) {
                sel = i;
            }
        } else if ((sel - slotselectinfo->width) < 0) {
            sel = slotselectinfo->selmax - 1;
        } else {
            sel -= slotselectinfo->width;
        }
        while (slotselectinfo->available[sel] != TRUE) {
            sel = (--sel >= 0) ? sel : (slotselectinfo->selmax - 1);
        }
    } else if (pad == PR_PAD_DOWN) {
        func_800330F8(pad);
        if (sel == (slotselectinfo->selmax - 1)) {
            for (i = 0; (i < (slotselectinfo->selmax - 1)) && (slotselectinfo->available[i] == FALSE); i++) {
                
            }
            if (slotselectinfo->available[i] == TRUE) {
                sel = i;
            }
        } else {
            sel += slotselectinfo->width;
            if (sel >= (slotselectinfo->selmax - 1)) {
                sel = slotselectinfo->selmax - 1;
            } else if (slotselectinfo->available[sel] != TRUE) {
                sel = slotselectinfo->selmax - 1;
            }
        }
    } else if (pad == PR_PAD_RIGHT) {
        func_800330F8(pad);
        sel = (sel + 1) % slotselectinfo->selmax;
        while (slotselectinfo->available[sel] != TRUE) {
            sel = (sel + 1) % slotselectinfo->selmax;
        }
    } else if (pad == PR_PAD_LEFT) {
        func_800330F8(pad);
        sel = (--sel >= 0) ? sel : (slotselectinfo->selmax - 1);
        while (slotselectinfo->available[sel] != TRUE) {
            sel = (--sel >= 0) ? sel : (slotselectinfo->selmax - 1);
        }
    }
    slotselectinfo->selcur = sel;
    return 0;
}

void CardMenuCacheFiles(void) {
    struct DIRENTRY *files;
    s32 firstfree;
    s32 size;

    CardGetInfo(&firstfree, &size);
    files = CardGetFiles();
    bcopy(files, D_8008C2A4, sizeof(D_8008C2A4));
}

// TODO: appInfo
extern s16 D_8009F828;

void CardMenuRun(CARD_INFO *info, s32 (*update)(s32 state, CARD_INFO *info, void **data), s32 (*front)(s32 state, CARD_INFO *info), s32 (*back)(s32 state, CARD_INFO *info), s32 initstate) {
    s32 state;
    s32 draw;
    s32 time;
    void *data;

    time = 0;
    state = initstate;
    while (state != CARD_STATE_END) {
        state = front(state, info);
        state = back(state, info);
        draw = update(state, info, &data);
        if (draw == PR_MENU_0) {
            draw = PR_MENU_MAIN;
        }
        CompoDrawMenu(draw, data);
        if (D_8009F828 == TRUE) {
            CardMenuRunDebug(state, draw, &time, info);
        }
        VSync(0);
        CompoDrawMenuOt(0);
    }
}

BOOL CardMenuSave(CARD_INFO *info) {
    D_80082714 = FALSE;
    D_8008C284.highlight = TRUE;
    D_8008C284.selcur = 3;
    // TODO: Defines
    CompoFrameMake(NULL, 6, 3, 1);
    CardReset();
    CardMenuRun(info, CardMenuUpdate, CardMenuSaveFront, CardMenuSaveBack, CARD_STATE_F_SAVE);
    CardMenuCacheFiles();
    CardEnd();
    return D_80082714;
}

s32 CardMenuMisc(CARD_INFO *info, s32 type) {
    D_80082714 = FALSE;
    D_80082710 = type;
    CardReset();
    CardMenuRun(info, CardMenuUpdate, CardMenuMiscFront, CardMenuMiscBack, CARD_STATE_F_STALL);
    CardEnd();
    if (D_80082714 == TRUE) {
        return info->save.stagecur;
    } else {
        return -1;
    }
}

s32 CardMenuLoad(CARD_INFO *info) {
    s32 stage;

    D_8008C284.highlight = TRUE;
    D_8008C284.selcur = 3;
    D_80082720 = PR_MENU_MAIN;
    D_80082724 = &D_8008C284;

    stage = CardMenuMisc(info, 0);
    if (stage >= 0) {
        D_8008270C = TRUE;
        CardMenuCacheFiles();
    }
    return stage;
}

s32 CardMenuReplay(CARD_INFO *info) {
    D_8008C284.highlight = TRUE;
    D_8008C284.selcur = 3;
    D_80082720 = PR_MENU_MAIN;
    D_80082724 = &D_8008C284;
    return CardMenuMisc(info, 1);
}

#ifndef NON_MATCHING
INCLUDE_ASM("asm/prr.bin/nonmatchings/prcmenu", CardMenuHighScoreMakeVisual);
#else
HIGH_SCORE_VISUAL *CardMenuHighScoreMakeVisual(CARD_INFO *info) {
    HIGH_SCORE_VISUAL_LIST *visual;
    char tmp[32];
    s32 i;
    s32 j;
    s32 n;
    s32 sp44;

    visual = D_80062858;
    sp44 = 0;
    for (i = 0, n = 0; i < visual->vis[0].height; i++) {
        for (j = 0; j < visual->vis[0].width; j++, n++) {
            if (info->highscore[i][j].score <= 0) {
                // TODO: ??
                visual->vis[n].text[16] = '\0';
            } else {
                sp44++;
                sprintf(tmp, D_8001028C, info->highscore[i][j].score, info->highscore[i][j].name);
                // NOT MATCHING: Weird addressing here
                strcpy(visual->vis[1 + n].text, tmp);
            }
        }
    }
    return visual;
}
#endif

HIGH_SCORE_VISUAL *CardMenuHighScore(CARD_INFO *info) {
    D_80082718 = FALSE;
    D_8008271C = FALSE;
    D_8008C284.highlight = TRUE;
    D_8008C284.selcur = 1;
    D_80082720 = PR_MENU_MAIN;
    D_80082724 = &D_8008C284;
    CardMenuMisc(info, 2);
    if (D_80082718 == TRUE) {
        return CardMenuHighScoreMakeVisual(info);
    } else {
        return NULL;
    }
}
