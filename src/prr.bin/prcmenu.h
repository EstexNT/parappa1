#ifndef _PRCMENU_H_
#define _PRCMENU_H_

#include "common.h"
#include "prcard.h"

// F - Frontend (Graphical), B - Backend (I/O interaction)
#define CARD_STATE_BAD 0
#define CARD_STATE_1 1
#define CARD_STATE_F_SAVE 2
#define CARD_STATE_F_STALL 3
#define CARD_STATE_B_CHECKSTATUS 4
#define CARD_STATE_F_NOTINSERTED 5
#define CARD_STATE_B_CHECKSLOTS 6
#define CARD_STATE_F_FULL 7
#define CARD_STATE_F_FORMAT 8
#define CARD_STATE_B_GETSLOTINFO 9
#define CARD_STATE_F_NAMEENTER 10
#define CARD_STATE_F_SLOTSELECTSAVE 11
#define CARD_STATE_F_SLOTSELECTLOAD 12
#define CARD_STATE_F_SLOTSELECTREPLAY 13
#define CARD_STATE_B_FORMAT 14
#define CARD_STATE_B_WRITE 15
#define CARD_STATE_B_READ 16
#define CARD_STATE_B_READHIGHSCORE 17
#define CARD_STATE_F_SAMENAME 18
#define CARD_STATE_END 19


// Base information for a card menu
typedef struct CARD_MENU_BASE_INFO {
    BOOL highlight; // TRUE if the selected thing needs to have a yellow highlight, FALSE for purple highlight
    s32 sel; // The selection
    BOOL saving; // TRUE if need to draw the "Saving..." text
} CARD_MENU_BASE_INFO;

typedef struct CARD_MENU_NAMEENTER_INFO {
    CARD_MENU_BASE_INFO base;
    char *map;
    s16 unk10;
    s16 unk12;
    s16 letterselmax;
    s16 letterselcur;
    s16 namelen;
    s16 endbutton;
    char name[8];
} CARD_MENU_NAMEENTER_INFO;

typedef struct CARD_MENU_SLOTSELECT_INFO {
    CARD_MENU_BASE_INFO base;
    s16 height;
    s16 width;
    s16 selmax;
    s16 availableslotnum;
    s16 selcur;
    s16 available[16];
    u8 unk36[2];
    char *name[16];
    s16 stagecur[16];
} CARD_MENU_SLOTSELECT_INFO;



typedef struct CARD_MENU_DIALOG_INFO {
    CARD_MENU_BASE_INFO base;
    u8 unk0C[0x24 - 0xc];
} CARD_MENU_DIALOG_INFO;

typedef struct HIGHSCORE_INFO {
    char name[12];
    s32 score;
} HIGHSCORE_INFO;

typedef struct HIGH_SCORE_VISUAL {
    s32 unk0;
    char text[8];
    s16 height;
    s16 width;
} HIGH_SCORE_VISUAL;

typedef struct HIGH_SCORE_VISUAL_LIST {
    HIGH_SCORE_VISUAL vis[20];
} HIGH_SCORE_VISUAL_LIST;

typedef struct CARD_INFO {
    SAVE_DATA save;
    HIGHSCORE_INFO highscore[6][4];
} CARD_INFO;

typedef struct SLOT_INFO {
    char fname[92];
    char name[12];
    s16 available;
    s16 stagecur;
} SLOT_INFO;

typedef struct {
    SLOT_INFO info[15];
    s16 availablenum;
    s16 freenum;
} CARD_MENU_SLOT_INFO;

// TODO: Move to prmenu
struct MENU_MAIN_DEFDATA {
    BOOL highlight;
    s32 selexit;
    s16 selcur;
    s16 selmax;
    s16 selval[5][2];
};


void CardMenuDrawTimed(s32 menu, void *data, s32 time);
void CardMenuDrawSlow(s32 menu, void *data);
void CardMenuHighScoreVSyncCallback(void);
void CardMenuResetBaseInfo(CARD_MENU_BASE_INFO *base);
void CardMenuUpdateSaving(s32 state);
void CardMenuUpdateHighlight(CARD_MENU_BASE_INFO *base);
s32 CardMenuSlotSelectUpdate(u32 pad, CARD_INFO *info);
void CardMenuCacheFiles(void);
void CardMenuRun(CARD_INFO *info, s32 (*update)(s32 state, CARD_INFO *info, void **data), s32 (*front)(s32 state, CARD_INFO *info), s32 (*back)(s32 state, CARD_INFO *info), s32 initstate);
BOOL CardMenuSave(CARD_INFO *info);
s32 CardMenuMisc(CARD_INFO *info, s32 type);
s32 CardMenuLoad(CARD_INFO *info);
s32 CardMenuReplay(CARD_INFO *info);
HIGH_SCORE_VISUAL *CardMenuHighScoreMakeVisual(CARD_INFO *info);
HIGH_SCORE_VISUAL *CardMenuHighScore(CARD_INFO *info);
s32 CardMenuUpdate(s32 state, CARD_INFO *info, void **data);
void CardMenuRunDebug(s32 state, s32 draw, s32 *time, CARD_INFO *info);
s32 CardMenuSaveFront(s32 state, CARD_INFO *info);
s32 CardMenuMiscFront(s32 state, CARD_INFO *info);
s32 CardMenuSaveBack(s32 state, CARD_INFO *info);
s32 CardMenuMiscBack(s32 state, CARD_INFO *info);
BOOL CardMenuCheckCachedFiles(void);
char *CardMenuGetUserFileName(char *filename);
s32 CardMenuCheckSlots(CARD_INFO *info, BOOL *samename);
void CardMenuCacheData(CARD_INFO *info, CARD_DATA *data);
void CardMenuCheckSlotsAvailable(CARD_INFO *info);
void CardMenuSetupSaveData(CARD_INFO *info);
void CardMenuReadHighScore(CARD_INFO *info);
void CardMenuResetNameEnterInfo(CARD_INFO *info);
void CardMenuResetSlotSelect(CARD_INFO *info);
void CardMenuLoadSlotSelectInfo(CARD_INFO *info, BOOL arg1);
void CardMenuRunDebugSlot(CARD_INFO *info, CARD_MENU_SLOTSELECT_INFO *data);
u32 CardMenuReadPadTrigger(void);
s32 CardMenuNameEnterUpdate(u32 pad, CARD_INFO *info);
s32 CardMenuConvertStatus(s32 status);
void CardMenuInitName(CARD_INFO *info, char *filename, char *title);
BOOL CardMenuCheckFileName(char *name);
s32 CardMenuGetAvailableSlotInfoIndex(CARD_INFO *info);


#endif
