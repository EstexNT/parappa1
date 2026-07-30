#include "prcmenu.h"
#include <string.h>
#include <stdio.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <ctype.h>
// #include "prcompo.h"
#include "prapp.h"

extern char D_80089BA4[32];
extern CARD_MENU_SLOT_INFO D_80089BC4;
extern CARD_DATA D_8008A21C;
extern char D_8008C21C[32];
extern char D_8008C23C[64 + 8];
extern struct MENU_MAIN_DEFDATA D_8008C284;
extern struct DIRENTRY D_8008C2A4[15];
extern CARD_MENU_DIALOG_INFO D_8008C4FC;

extern u32 D_80082708;
extern BOOL D_8008270C;
extern s32 D_80082710;
extern BOOL D_80082714;
extern BOOL D_80082718;
extern BOOL D_8008271C;
extern s32 D_80082720;
extern void *D_80082724;
extern BOOL D_80082728;
extern s32 D_8008272C;

extern CARD_MENU_NAMEENTER_INFO D_800626D8;
extern CARD_MENU_SLOTSELECT_INFO D_80062700;
extern u16 D_80062798[96];


static char *D_800823B0 = "BISCPS-18002";
static char *D_800823BC = "PARAPP";
static char *D_800823C8 = "PARAPPA";

static char D_800823CC[] = "%s%s";
static char D_800823D4[] = "%s";

const char D_80010208[] = "%s \"%s\" ST%d";
const char D_80010218[] = "convName:[%s] docNameTemp:[%s]\n";
const char D_80010238[] = "CardName:[%s]\n";
const char D_80010248[] = "Read RC: [%s], %d\n";
const char D_8001025C[] = "\n%4d:  MENU: %d   DRAW: %d\n";
const char D_80010278[] = "CardName2: [%s]\n";
const char D_8001028C[] = "%4d %-3.3s";


s32 CardMenuUpdate(register s32 state, register CARD_INFO *info, register void **data) {
    s32 type;

    *data = NULL;
    switch (state) {
        case CARD_STATE_F_SAVE: {
            type = PR_MENU_CARDSAVE;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        case CARD_STATE_F_STALL: {
            type = 0;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        case CARD_STATE_F_NOTINSERTED: {
            type = PR_MENU_CARDNOTINSERT;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        case CARD_STATE_F_FORMAT: {
            type = PR_MENU_CARDFORMAT;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        case CARD_STATE_F_FULL: {
            type = PR_MENU_CARDFULL;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        case CARD_STATE_F_NAMEENTER: {
            type = PR_MENU_NAMEENTER;
            CardMenuUpdateHighlight(&D_800626D8.base);
            *data = &D_800626D8;
            break;
        }
        case CARD_STATE_F_SLOTSELECTSAVE: {
            type = PR_MENU_SAVE;
            CardMenuUpdateHighlight(&D_80062700.base);
            *data = &D_80062700;
            break;
        }
        case CARD_STATE_F_SLOTSELECTLOAD: {
            type = PR_MENU_LOAD;
            CardMenuUpdateHighlight(&D_80062700.base);
            *data = &D_80062700;
            break;
        }
        case CARD_STATE_F_SLOTSELECTREPLAY: {
            type = PR_MENU_REPLAY;
            CardMenuUpdateHighlight(&D_80062700.base);
            *data = &D_80062700;
            break;
        }
        case CARD_STATE_F_SAMENAME: {
            type = PR_MENU_CARDSAMENAME;
            CardMenuUpdateHighlight(&D_8008C4FC.base);
            *data = &D_8008C4FC;
            break;
        }
        default: {
            type = D_80082720;
            *data = D_80082724;
            break;
        }
    }
    D_80082720 = type;
    D_80082724 = *data;
    return type;
}

void CardMenuRunDebug(register s32 state, register s32 draw, register s32 *time, register CARD_INFO *info) {
    u32 unused;
    struct DIRENTRY *files;

    files = CardGetFiles();
    FntPrint("\n\n~c888");
    FntPrint(D_8001025C, (*time)++ / 60, state, draw);
    switch (draw) {
        case PR_MENU_SAVE:
        case PR_MENU_LOAD:
        case PR_MENU_REPLAY: {
            CardMenuRunDebugSlot(info, &D_80062700);
            break;
        }
        default: {
            break;
        }
    }
    FntFlush(-1);
}

s32 CardMenuSaveFront(register s32 state, register CARD_INFO *info) {
    s32 updateret;
    BOOL samename;
    u32 pad;
    s32 firstfree;
    s32 size;
    char *userfilename;

    pad = CardMenuReadPadTrigger();
    if (pad == 0) {
        return state;
    }

    switch (state) {
        case CARD_STATE_F_SAVE: {
            if (pad == PR_PAD_CIRCLE) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_B_CHECKSTATUS;
                D_8008C4FC.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_CARDSAVE, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            } else if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_END;
                D_8008C4FC.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_CARDSAVE, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            }
            break;
        }
        case CARD_STATE_F_NOTINSERTED: {
            if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_F_SAVE;
                D_8008C4FC.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_CARDNOTINSERT, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            }
            break;
        }
        case CARD_STATE_F_FULL: {
            if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_F_SAVE;
                D_8008C4FC.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_CARDFULL, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            }
            break;
        }
        case CARD_STATE_F_FORMAT: {
            if (pad == PR_PAD_CIRCLE) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_B_FORMAT;
                D_8008C4FC.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_CARDFORMAT, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            } else if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_F_SAVE;
                D_8008C4FC.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_CARDFORMAT, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
            }
            break;
        }
        case CARD_STATE_F_NAMEENTER: {
            updateret = CardMenuNameEnterUpdate(pad, info);
            if (updateret == 1) {
                D_800626D8.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_NAMEENTER, &D_800626D8, 20);
                CardMenuResetBaseInfo(&D_800626D8.base);
                if (CardGetStatus() != CARD_EVENT_IOE) {
                    return CardMenuConvertStatus(CardGetStatus());
                }
                CardGetInfo(&firstfree, &size);
                CardMenuCheckSlots(info, &samename);
                if (samename == TRUE) {
                    return CARD_STATE_F_SAMENAME;
                }
                if (D_80089BC4.freenum >= 1) {
                    D_8008270C = TRUE;
                    CardMenuUpdateSaving(CARD_STATE_F_NAMEENTER);
                    state = CARD_STATE_B_WRITE;
                } else {
                    return CARD_STATE_F_FULL;
                }
            } else if (updateret == 2) {
                D_800626D8.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_NAMEENTER, &D_800626D8, 20);
                CardMenuResetBaseInfo(&D_800626D8.base);
                state = CARD_STATE_F_SAVE;
            }
            break;
        }
        case CARD_STATE_F_SLOTSELECTSAVE: {
            updateret = CardMenuSlotSelectUpdate(pad, info);
            if (updateret == 1) {
                if (D_8008C21C[0] == '\0') {
                    state = CARD_STATE_F_NAMEENTER;
                    CardMenuResetNameEnterInfo(info);
                } else {
                    state = CARD_STATE_B_WRITE;
                    CardMenuUpdateSaving(CARD_STATE_F_SLOTSELECTSAVE);
                    userfilename = CardMenuGetUserFileName(D_8008C21C);
                    strcpy(info->save.name, userfilename);
                }
            } else if (updateret == 2) {
                state = CARD_STATE_F_SAVE;
                D_800626D8.base.sel = 1;
                D_80062700.base.saving = FALSE;
                D_80062700.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_SAVE, &D_80062700, 20);
                CardMenuResetBaseInfo(&D_80062700.base);
            }
            break;
        }
        case CARD_STATE_F_SAMENAME: {
            if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                D_8008C4FC.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_CARDSAMENAME, &D_8008C4FC, 20);
                CardMenuResetBaseInfo(&D_8008C4FC.base);
                state = CARD_STATE_F_NAMEENTER;
                CardMenuResetNameEnterInfo(info);
            }
            break;
        }
        default: {
            break;
        }
    }
    return state;
}

s32 CardMenuMiscFront(register s32 state, register CARD_INFO *info) {
    s32 updateret;
    u32 pad;

    pad = CardMenuReadPadTrigger();
    if (pad == 0) {
        return state;
    }

    switch (state) {
        case CARD_STATE_F_NOTINSERTED: {
            if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_END;
            }
            break;
        }
        case CARD_STATE_F_FULL: {
            if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_END;
            }
            break;
        }
        case CARD_STATE_F_FORMAT: {
            if (pad == PR_PAD_CIRCLE) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_B_FORMAT;
            } else if (pad == PR_PAD_CROSS) {
                func_800330F8(PR_PAD_CIRCLE);
                state = CARD_STATE_END;
            }
            break;
        }
        case CARD_STATE_F_SLOTSELECTLOAD: {
            updateret = CardMenuSlotSelectUpdate(pad, info);
            if (updateret == 1) {
                state = CARD_STATE_B_READ;
                D_80062700.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_LOAD, &D_80062700, 20);
                CardMenuResetBaseInfo(&D_80062700.base);
            } else if (updateret == 2) {
                state = CARD_STATE_END;
                D_80062700.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_LOAD, &D_80062700, 20);
                CardMenuResetBaseInfo(&D_80062700.base);
            }
            break;
        }
        case CARD_STATE_F_SLOTSELECTREPLAY: {
            updateret = CardMenuSlotSelectUpdate(pad, info);
            if (updateret == 1) {
                state = CARD_STATE_B_READ;
                D_80062700.base.sel = 1;
                CardMenuDrawTimed(PR_MENU_REPLAY, &D_80062700, 20);
                CardMenuResetBaseInfo(&D_80062700.base);
            } else if (updateret == 2) {
                state = CARD_STATE_END;
                D_80062700.base.sel = 2;
                CardMenuDrawTimed(PR_MENU_REPLAY, &D_80062700, 20);
                CardMenuResetBaseInfo(&D_80062700.base);
            }
            break;
        }
        default: {
            break;
        }
    }
    return state;
}

s32 CardMenuSaveBack(register s32 state, register CARD_INFO *info) {
    s32 firstfree;
    s32 size;
    BOOL samename;
    BOOL sp1C;

    switch (state) {
        case CARD_STATE_B_CHECKSTATUS: {
            return CardMenuConvertStatus(CardGetStatus());
        }
        case CARD_STATE_B_CHECKSLOTS: {
            CardGetInfo(&firstfree, &size);
            if (CardMenuCheckSlots(info, &samename) >= 1) {
                return CARD_STATE_B_GETSLOTINFO;
            } else {
                return CARD_STATE_F_FULL;
            }
        }
        case CARD_STATE_B_GETSLOTINFO: {
            sp1C = ((D_8008270C == FALSE) || (CardMenuCheckCachedFiles() == TRUE)) ? TRUE : FALSE;
            CardGetInfo(&firstfree, &size);
            CardMenuCheckSlots(info, &samename);
            CardMenuLoadSlotSelectInfo(info, sp1C);
            return CARD_STATE_F_SLOTSELECTSAVE;
        }
        case CARD_STATE_B_FORMAT: {
            CardFormat();
            return CARD_STATE_B_CHECKSTATUS;
        }
        case CARD_STATE_B_WRITE: {
            CardMenuSetupSaveData(info);
            printf(D_80010278, D_8008C21C);
            VSyncCallback(NULL);
            if (CardWrite(D_8008C21C, &D_8008A21C, 1) == 0) {
                D_80082714 = TRUE;
                return CARD_STATE_END;
            } else {
                return CARD_STATE_F_SAVE;
            }
        }
        default: {
            return state;
        }
    }
}

s32 CardMenuMiscBack(register s32 state, register CARD_INFO *info) {
    s32 firstfree;
    s32 size;

    switch (state) {
        case CARD_STATE_F_STALL: {
            return CARD_STATE_B_CHECKSTATUS;
        }
        case CARD_STATE_B_CHECKSTATUS: {
            return CardMenuConvertStatus(CardGetStatus());
        }
        case CARD_STATE_B_CHECKSLOTS: {
            CardMenuDrawTimed(PR_MENU_MAIN, &D_8008C284, 1);
            CardGetInfo(&firstfree, &size);
            CardMenuCheckSlotsAvailable(info);
            CardMenuResetSlotSelect(info);
            if (D_80082710 == 0) {
                return CARD_STATE_F_SLOTSELECTLOAD;
            } else if (D_80082710 == 1) {
                return CARD_STATE_F_SLOTSELECTREPLAY;
            } else {
                return CARD_STATE_B_READHIGHSCORE;
            }
        }
        case CARD_STATE_B_FORMAT: {
            CardFormat();
            return CARD_STATE_B_CHECKSTATUS;
        }
        case CARD_STATE_B_READ: {
            CardRead(D_8008C21C, &D_8008A21C, 1);
            CardMenuCacheData(info, &D_8008A21C);
            return CARD_STATE_END;
        }
        case CARD_STATE_B_READHIGHSCORE: {
            D_80082718 = TRUE;
            CardMenuDrawTimed(PR_MENU_MAIN, &D_8008C284, 1);
            CardMenuReadHighScore(info);
            return CARD_STATE_END;
        }
        default: {
            return state;
        }
    }
}

BOOL CardMenuCheckCachedFiles(void) {
    struct DIRENTRY *files;

    files = CardGetFiles();
    if (bcmp(files, D_8008C2A4, sizeof(D_8008C2A4)) != 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}

char *CardMenuGetUserFileName(register char *filename) {
    s32 i;
    u32 basenamelen;

    basenamelen = strlen(D_800823B0);
    for (i = 0; i < 20; i++) {
        D_80089BA4[i] = filename[i];
    }
    D_80089BA4[i] = '\0';
    // BUG: Checking an array for nullptr
    if ((D_80089BA4 == NULL) || (strlen(D_80089BA4) < basenamelen)) {
        return NULL;
    } else {
        return &D_80089BA4[basenamelen];
    }
}

s32 CardMenuCheckSlots(register CARD_INFO *info, register BOOL *samename) {
    u32 unused;
    u32 unused1;
    s32 i;
    s32 availableslots;
    s32 usedslots;
    char *name;
    char *userfilename;
    SLOT_INFO *slotinfo;
    struct DIRENTRY *files;

    name = info->save.name;
    slotinfo = &D_80089BC4.info[0];
    files = CardGetFiles();
    *samename = FALSE;
    bzero(&D_80089BC4, sizeof(D_80089BC4));
    usedslots = 0;
    availableslots = 0;
    for (i = 0; i < 15; i++) {
        slotinfo[availableslots].available = FALSE;
        slotinfo[availableslots].fname[0] = slotinfo[availableslots].name[0] = '\0';
        if (files[i].name[0] == '\0') {
            continue;
        }
        usedslots += (files[i].size + (0x2000 - 1)) / 0x2000;
        if ((files[i].name[0] != '\0') && (CardMenuCheckFileName(files[i].name) == FALSE)) {
            continue;
        }
        userfilename = CardMenuGetUserFileName(files[i].name);
        if (strcmp(userfilename, name) == 0) {
            *samename = TRUE;
        }
        slotinfo[availableslots].available = TRUE;
        strcpy(slotinfo[availableslots].fname, files[i].name);
        strcpy(slotinfo[availableslots].name, userfilename);
        availableslots++;
    }

    for (i = 0; i < (15 - usedslots); i++, availableslots++) {
        slotinfo[availableslots].available = TRUE;
        
    }
    D_80089BC4.availablenum = availableslots;
    D_80089BC4.freenum = 15 - usedslots;
    return availableslots;
}

void CardMenuCacheData(register CARD_INFO *info, register CARD_DATA *data) {
    CARD_DATA *infodata;

    infodata = &data->data;
    AppCacheSaveData(infodata);
}

void CardMenuCheckSlotsAvailable(register CARD_INFO *info) {
    char *userfilename;
    s32 i;
    s32 availableslots;
    SLOT_INFO *slotinfo;
    struct DIRENTRY *files;

    slotinfo = &D_80089BC4.info[0];
    files = CardGetFiles();
    availableslots = 0;
    for (i = 0; i < 15; i++) {
        slotinfo[i].available = FALSE;
        slotinfo[i].fname[0] = slotinfo[i].name[0] = '\0';
        if ((files[i].name[0] == '\0') || (CardMenuCheckFileName(files[i].name) == FALSE)) {
            continue;
        }
        userfilename = CardMenuGetUserFileName(files[i].name);
        strcpy(slotinfo[availableslots].name, userfilename);
        strcpy(slotinfo[availableslots].fname, files[i].name);
        slotinfo[availableslots].available = TRUE;
        availableslots++;
    }
    D_80089BC4.availablenum = availableslots;
}

void CardMenuSetupSaveData(register CARD_INFO *info) {
    SAVE_DATA *data;

    CardMenuInitName(info, D_8008C21C, D_8008C23C);
    bzero(&D_8008A21C, sizeof(D_8008A21C));
    CardInitSaveDataHeader(&D_8008A21C, D_8008C23C);
    data = &D_8008A21C.data;
    bcopy(&info->save, data, sizeof(*data));
}

void CardMenuReadHighScore(register CARD_INFO *info) {
    s32 i;
    s32 readret;
    SLOT_INFO *slotinfo;
    SAVE_DATA *data;
    SAVE_DATA *data1;

    data = &D_8008A21C.data;
    data1 = data;
    AppResetHighScore();
    if (D_80089BC4.availablenum == 0) {
        return;
    }

    D_8008271C = TRUE;
    D_8008272C = 0;
    VSyncCallback(CardMenuHighScoreVSyncCallback);
    slotinfo = &D_80089BC4.info[0];
    for (i = 0; i < 15; i++) {
        bzero(&D_8008A21C, sizeof(D_8008A21C));
        if (slotinfo[i].available == FALSE) {
            continue;
        }

        strcpy(D_8008C21C, slotinfo[i].fname);
        readret = CardRead(D_8008C21C, &D_8008A21C, 1);
        printf(D_80010248, D_8008C21C, readret);
        if (readret < 0) {
            continue;
        }
        slotinfo[i].stagecur = data1->stagecur;
        AppUpdateHighScore(data1);
    }
    VSyncCallback(NULL);
    if (D_80082728 != FALSE) {
        CompoDrawMenuOt(0);
        D_80082728 = FALSE;
    }
}

void CardMenuResetNameEnterInfo(register CARD_INFO *info) {
    s32 i;
    char *name;
    CARD_MENU_NAMEENTER_INFO *nameinfo;

    name = info->save.name;
    nameinfo = &D_800626D8;
    nameinfo->letterselcur = 0;
    nameinfo->endbutton = 0;
    nameinfo->base.saving = FALSE;
    nameinfo->base.highlight = FALSE;
    for (i = 0; i < 6; i++) {
        nameinfo->name[i] = '\0';
    }
    if ((name == NULL) || (*name == '\0')) {
        nameinfo->namelen = 0;
    } else {
        nameinfo->namelen = strlen(name);
        strcpy(nameinfo->name, name);
    }
}

void CardMenuResetSlotSelect(register CARD_INFO *info) {
    s32 i;
    CARD_MENU_SLOTSELECT_INFO *slotselectinfo;
    SLOT_INFO *slotinfo;

    slotselectinfo = &D_80062700;
    slotinfo = &D_80089BC4.info[0];
    slotselectinfo->base.saving = FALSE;
    D_8008C21C[0] = '\0';
    for (i = 0; i < 15; i++) {
        if (i < D_80089BC4.availablenum) {
            slotselectinfo->available[i] = slotinfo[i].available;
            slotselectinfo->name[i] = (slotinfo[i].available == TRUE) ? slotinfo[i].name : NULL;
            slotselectinfo->stagecur[i] = slotinfo[i].stagecur;
        } else {
            slotselectinfo->available[i] = FALSE;
            slotselectinfo->name[i] = NULL;
        }
    }
    slotselectinfo->availableslotnum = D_80089BC4.availablenum;
    slotselectinfo->available[15] = TRUE;
    slotselectinfo->selcur = slotselectinfo->height * slotselectinfo->width;
}

void CardMenuLoadSlotSelectInfo(register CARD_INFO *info, register BOOL arg1) {
    s32 i;
    CARD_MENU_SLOTSELECT_INFO *slotselectinfo;
    SLOT_INFO *slotinfo;
    char *name;
    s32 newsel;

    slotselectinfo = &D_80062700;
    slotinfo = &D_80089BC4.info[0];
    name = info->save.name;
    slotselectinfo->base.saving = FALSE;
    D_8008C21C[0] = '\0';
    for (i = 0; i < 15; i++) {
        if (i < D_80089BC4.availablenum) {
            slotselectinfo->available[i] = slotinfo[i].available;
            slotselectinfo->name[i] = (slotinfo[i].available == TRUE) ? slotinfo[i].name : NULL;
            slotselectinfo->stagecur[i] = slotinfo[i].stagecur;
        } else {
            slotselectinfo->available[i] = FALSE;
            slotselectinfo->name[i] = NULL;
        }
    }

    if (name[0] == '\0') {
        arg1 = TRUE;
    }
    newsel = -1;
    for (i = 0; i < D_80089BC4.availablenum; i++) {
        if (slotselectinfo->available[i] == FALSE) {
            continue;
        }
        if ((arg1 == TRUE) && (slotselectinfo->name[i][0] == '\0')) {
            newsel = i;
            break;
        } else if ((arg1 == FALSE) && (slotselectinfo->name[i] != NULL) && (strcmp(slotselectinfo->name[i], name) == 0)) {
            newsel = i;
            break;
        }
    }
    slotselectinfo->availableslotnum = D_80089BC4.availablenum;
    if (newsel >= 0) {
        slotselectinfo->selcur = newsel;
    } else {
        slotselectinfo->selcur = slotselectinfo->height * slotselectinfo->width;
    }
    slotselectinfo->available[15] = TRUE;
}

void CardMenuRunDebugSlot(register CARD_INFO *info, register CARD_MENU_SLOTSELECT_INFO *data) {

}

u32 CardMenuReadPadTrigger(void) {
    u32 pad;

    pad = PadRead(1);
    if (pad != D_80082708) {
        return (D_80082708 = pad);
    }
    return 0;
}

s32 CardMenuNameEnterUpdate(register u32 pad, register CARD_INFO *info) {
    CARD_MENU_NAMEENTER_INFO *nameinfo;
    char *name;
    s32 sp18;
    s32 letterselcur;
    s32 sel;
    s32 sp24;
    s32 namemapped;

    nameinfo = &D_800626D8;
    name = info->save.name;
    sel = letterselcur = nameinfo->letterselcur;
    nameinfo->endbutton = 0;
    if (pad == PR_PAD_UP) {
        func_800330F8(pad);
        if (letterselcur == 0) {
            sel = 56;
        } else {
            if ((letterselcur >= 1) && (letterselcur < 14)) {
                sel = 0;
            } else if ((letterselcur >= 14) && (letterselcur < 49)) {
                sel = letterselcur - 14;
            } else if ((letterselcur >= 49) && (letterselcur < 55)) {
                sel = letterselcur - 7;
            } else {
                sel = letterselcur - 1;
            }
        }
    }
    if (pad == PR_PAD_DOWN) {
        func_800330F8(pad);
        if ((letterselcur >= 0) && (letterselcur < 35)) {
            sel = letterselcur + 14;
        } else if ((letterselcur >= 35) && (letterselcur < 42)) {
            sel = 48;
        } else if ((letterselcur >= 42) && (letterselcur < 48)) {
            sel = letterselcur + 7;
        } else if ((letterselcur >= 48) && (letterselcur < 55)) {
            sel = 55;
        } else if (letterselcur == 55) {
            sel = 56;
        } else {
            sel = 0;
        }
    }
    if (pad == PR_PAD_RIGHT) {
        func_800330F8(pad);
        sel = (letterselcur + 1) % nameinfo->letterselmax;
    }
    if (pad == PR_PAD_LEFT) {
        func_800330F8(pad);
        sel = (((letterselcur - 1) < 0) ? nameinfo->letterselmax : letterselcur) - 1;
    }
    nameinfo->letterselcur = sel;

    if ((pad & (PR_PAD_TRIANGLE | PR_PAD_CIRCLE | PR_PAD_CROSS | PR_PAD_SQUARE)) != 0) {
        namemapped = nameinfo->map[sel];
        if (namemapped == 8) {
            // Backspace
            func_800330F8(PR_PAD_CROSS);
            if (nameinfo->namelen > 0) {
                nameinfo->namelen--;
                nameinfo->name[nameinfo->namelen] = '\0';
            }
        } else {
            if ((namemapped == 10) && (pad == PR_PAD_CIRCLE)) {
                func_800330F8(PR_PAD_CIRCLE);
                if (nameinfo->name[0] == '\0') {
                    strcpy(name, D_800823BC);
                } else {
                    strcpy(name, nameinfo->name);
                }
                nameinfo->endbutton = 1;
                nameinfo->base.sel = 1;
                return 1;
            } else if ((namemapped == 10) && (pad == PR_PAD_CROSS)) {
                func_800330F8(PR_PAD_CIRCLE);
                nameinfo->endbutton = 2;
                nameinfo->base.sel = 2;
                return 2;
            } else if ((namemapped >= ' ') && (nameinfo->namelen < 6)) {
                func_800330F8(PR_PAD_SELECT);
                nameinfo->name[nameinfo->namelen] = namemapped;
                nameinfo->namelen++;
                nameinfo->name[nameinfo->namelen] = '\0';
            }
        }
    }
    return 0;
}

s32 CardMenuConvertStatus(register s32 status) {
    switch (status) {
        case CARD_EVENT_IOE: {
            return CARD_STATE_B_CHECKSLOTS;
        }
        case CARD_EVENT_TIMEOUT: {
            return CARD_STATE_F_NOTINSERTED;
        }
        case CARD_EVENT_NEWCARD: {
            return CARD_STATE_F_FORMAT;
        }
        case CARD_EVENT_ERROR:
        default: {
            return CARD_STATE_1;
        }
    }
}

void CardMenuInitName(register CARD_INFO *info, register char *filename, register char *title) {
    char *name;
    char titleasc[128];
    char nameconv[32];
    char *titletemp;
    u16 sjischar;
    s32 i;
    u32 unused;
    s32 freeslot;
    s32 titleasclen;
    s32 curstage;
    u32 titleascindex;

    name = info->save.name;
    freeslot = CardMenuGetAvailableSlotInfoIndex(info);
    if (freeslot == 69) {
        exit(1);
    }

    sprintf(filename, D_800823CC, D_800823B0, name);
    curstage = AppGetCurrentStageNum();
    for (i = 0; (name[i] != '\0') && (i < 12); i++) {
        nameconv[i] = (isalnum(name[i]) == FALSE) ? '?' : name[i];
    }
    nameconv[i] = '\0';
    if (nameconv[0] != '\0') {
        sprintf(titleasc, D_80010208, D_800823C8, nameconv, curstage);
    } else {
        sprintf(titleasc, D_800823D4, D_800823C8);
    }

    titletemp = title;
    titleasclen = strlen(titleasc);
    printf(D_80010218, nameconv, titleasc);
    printf(D_80010238, filename);
    for (i = 0; i < 32; i++) {
        titleascindex = titleasc[i] - 0x20;
        sjischar = (i < titleasclen) ? D_80062798[titleascindex] : 0;
        *titletemp++ = ((char *)&sjischar)[1];
        *titletemp++ = ((char *)&sjischar)[0];
    }
}

BOOL CardMenuCheckFileName(register char *name) {
    return strncmp(name, D_800823B0, strlen(D_800823B0)) == 0;
}

s32 CardMenuGetAvailableSlotInfoIndex(register CARD_INFO *info) {
    s32 i;

    for (i = 0; i < 15; i++) {
        if (D_80089BC4.info[i].available == TRUE) {
            return i;
        }
    }
    return 69;
}

u32 D_80082708;
BOOL D_8008270C;
s32 D_80082710;
BOOL D_80082714;
BOOL D_80082718;
BOOL D_8008271C;
s32 D_80082720;
void *D_80082724;
BOOL D_80082728;
s32 D_8008272C;
s32 D_80082730;
