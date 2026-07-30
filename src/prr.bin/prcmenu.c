#include "prcmenu.h"
#include <string.h>
#include <stdio.h>
#include <libetc.h>
// #include "prcompo.h"
#include "prapp.h"


static char rcsid[] = "@(#)prcmenu.c: version 01-00 96/08/01 00:00:00";

extern CARD_MENU_SLOT_INFO D_80089BC4;
extern char D_8008C21C[32];
extern struct MENU_MAIN_DEFDATA D_8008C284;
extern struct DIRENTRY D_8008C2A4[15];

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

char D_8006269C[58] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
    'Y', 'Z', '1', '2', '3', '4', '5', '6', 
    '7', '8', '9', '0', '-', '=', '!', '@', 
    '#', '$', '%', '^', '&', '*', '(', ')', 
    '_', '+', ',', '.', '?', '[', ']', '\x08', 
    '\n', '\x00',
};
CARD_MENU_NAMEENTER_INFO D_800626D8 = {
    {FALSE, 0, FALSE}, 
    D_8006269C,
    4, 14, 57, 0,
    0, 0, {'\x00'}
};
CARD_MENU_SLOTSELECT_INFO D_80062700 = {
    {FALSE, 0, FALSE},
    5, 3, 16, 0, 0,
    {FALSE}, {NULL}, {0}
};
u16 D_80062798[96] = {
    0x8140 /* 　 */, 0x8149 /* ！ */, 0x8168 /* ” */, 0x8194 /* ＃ */, 0x8190 /* ＄ */, 0x8193 /* ％ */,
    0x8195 /* ＆ */, 0x8166 /* ’ */, 0x8169 /* （ */, 0x816a /* ） */, 0x8196 /* ＊ */, 0x817b /* ＋ */,
    0x8143 /* ， */, 0x817c /* − */, 0x8144 /* ． */, 0x815e /* ／ */, 0x824f /* ０ */, 0x8250 /* １ */,
    0x8251 /* ２ */, 0x8252 /* ３ */, 0x8253 /* ４ */, 0x8254 /* ５ */, 0x8255 /* ６ */, 0x8256 /* ７ */,
    0x8257 /* ８ */, 0x8258 /* ９ */, 0x8146 /* ： */, 0x8147 /* ； */, 0x8183 /* ＜ */, 0x8181 /* ＝ */,
    0x8184 /* ＞ */, 0x8148 /* ？ */, 0x8197 /* ＠ */, 0x8260 /* Ａ */, 0x8261 /* Ｂ */, 0x8262 /* Ｃ */,
    0x8263 /* Ｄ */, 0x8264 /* Ｅ */, 0x8265 /* Ｆ */, 0x8266 /* Ｇ */, 0x8267 /* Ｈ */, 0x8268 /* Ｉ */,
    0x8269 /* Ｊ */, 0x826a /* Ｋ */, 0x826b /* Ｌ */, 0x826c /* Ｍ */, 0x826d /* Ｎ */, 0x826e /* Ｏ */,
    0x826f /* Ｐ */, 0x8270 /* Ｑ */, 0x8271 /* Ｒ */, 0x8272 /* Ｓ */, 0x8273 /* Ｔ */, 0x8274 /* Ｕ */,
    0x8275 /* Ｖ */, 0x8276 /* Ｗ */, 0x8277 /* Ｘ */, 0x8278 /* Ｙ */, 0x8279 /* Ｚ */, 0x816d /* ［ */,
    0x818f /* ￥ */, 0x816e /* ］ */, 0x814f /* ＾ */, 0x8151 /* ＿ */, 0x8165 /* ‘ */, 0x8281 /* ａ */,
    0x8282 /* ｂ */, 0x8283 /* ｃ */, 0x8284 /* ｄ */, 0x8285 /* ｅ */, 0x8286 /* ｆ */, 0x8287 /* ｇ */,
    0x8288 /* ｈ */, 0x8289 /* ｉ */, 0x828a /* ｊ */, 0x828b /* ｋ */, 0x828c /* ｌ */, 0x828d /* ｍ */,
    0x828e /* ｎ */, 0x828f /* ｏ */, 0x8290 /* ｐ */, 0x8291 /* ｑ */, 0x8292 /* ｒ */, 0x8293 /* ｓ */,
    0x8294 /* ｔ */, 0x8295 /* ｕ */, 0x8296 /* ｖ */, 0x8297 /* ｗ */, 0x8298 /* ｘ */, 0x8299 /* ｙ */,
    0x829a /* ｚ */, 0x816f /* ｛ */, 0x8162 /* ｜ */, 0x8170 /* ｝ */, 0x8150 /* ￣ */, 0x8140 /* 　 */,
};
HIGH_SCORE_VISUAL_LIST D_80062858 = {
    {
        {0, {'\x00'}, 6, 3},
        {19, {'\x00'}, 0, 0},
    },
};

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
        if (appInfo.testmode == TRUE) {
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
    
    visual = &D_80062858;
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
