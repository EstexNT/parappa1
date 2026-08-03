#ifndef _PRMENU_H_
#define _PRMENU_H_

#include "common.h"
#include "prscene.h"


#define MENU_STAGE_ICON_0 0
#define MENU_STAGE_ICON_1 1
#define MENU_STAGE_ICON_2 2
#define MENU_STAGE_ICON_3 3


typedef struct MENU_STAGE_DEFDATA {
    BOOL highlight;
    BOOL selected;
    s16 selcur;
    s16 selmax;
    s16 icons[9];
} MENU_STAGE_DEFDATA;

typedef struct MENU_MAIN_DEFDATA {
    BOOL highlight;
    BOOL selexit;
    s16 selcur;
    s16 selmax;
    s16 selval[5][2];
} MENU_MAIN_DEFDATA;

typedef struct MENU_START_DEFDATA {
    s32 unk0;
    s32 unk4;
} MENU_START_DEFDATA;

typedef struct MENU_HIGHSCORE_DEFDATA {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    char unk14[19][16];
} MENU_HIGHSCORE_DEFDATA;

typedef struct MENU_DEBUG_ENTRY {
    s32 selmax;
    char *title;
    char *opt[3];
    s32 val[3];
    s32 selcur;
} MENU_DEBUG_ENTRY;

typedef struct MENU_DEBUG_DEFDATA {
    MENU_DEBUG_ENTRY *entries;
    s32 selcur;
    s32 entriesnum;
} MENU_DEBUG_DEFDATA;


BOOL func_800330a8(u32 arg0);
void MenuPlayButtonSound(u32 pad);
void MenuUpdateDebugOptions(SCENE_INFO *scn);
s32 MenuRunnerProcess(s32 menu, void *data);

#endif
