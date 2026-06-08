#ifndef _PRSCENE_H_
#define _PRSCENE_H_

#include "common.h"

#define PR_DIR_DOWN 0
#define PR_DIR_UP   1
#define PR_DIR_IDLE 2

#define PR_INP_FLAG_1  (1 << 0)
#define PR_INP_FLAG_2  (1 << 1)
#define PR_INP_FLAG_4  (1 << 2)
#define PR_INP_FLAG_8  (1 << 3)
#define PR_INP_FLAG_10 (1 << 4)
#define PR_INP_FLAG_20 (1 << 5)
#define PR_INP_FLAG_40 (1 << 6)
#define PR_INP_FLAG_80 (1 << 7)

#define PR_SCN_FLAG_1    (1 << 0)
#define PR_SCN_FLAG_2    (1 << 1)
#define PR_SCN_FLAG_4    (1 << 2)
#define PR_SCN_FLAG_8    (1 << 3)
#define PR_SCN_FLAG_10   (1 << 4)
#define PR_SCN_FLAG_20   (1 << 5)
#define PR_SCN_FLAG_40   (1 << 6)
#define PR_SCN_FLAG_80   (1 << 7)
#define PR_SCN_FLAG_100  (1 << 8)
#define PR_SCN_FLAG_200  (1 << 9)
#define PR_SCN_FLAG_2000 (1 << 13)
#define PR_SCN_FLAG_4000 (1 << 14)

#define PR_SCN_LVLTRANS_1 1
#define PR_SCN_LVLTRANS_2 2
#define PR_SCN_LVLTRANS_3 3
#define PR_SCN_LVLTRANS_4 4
#define PR_SCN_LVLTRANS_5 5
#define PR_SCN_LVLTRANS_6 6


// Describes at which point which subtitle should play and for how long 
typedef struct MOVIE_SUBTITLES_INFO {
    PARA_TIME start;
    s16 seljp;
    s16 selen;
    s16 texttime;
} MOVIE_SUBTITLES_INFO;

// Describes lists of things needed for subtitles
typedef struct MOVIE_SUBTITLES {
    PARA_JP_TEXT **jp;
    PARA_EN_TEXT **en;
    MOVIE_SUBTITLES_INFO *info;
    u32 num; // How many subtitles we have to display
} MOVIE_SUBTITLES;

// 1/8th of a beat in a bar times at which to do following checks
typedef struct EIGHTH_TIMES {
    s32 checkfail; // Game over check
    s32 checkperf; // Perfomance check
    s32 checkhigh; // Cool check
} EIGHTH_TIMES;

// Describes the visual information of a bar (keys to display)
typedef struct VISUAL_LINE_INFO {
    s16 linenum;
    s16 start;
    // Key indexes
    s8 first[20]; 
    s8 second[20]; 
} VISUAL_LINE_INFO;

// Describes the current bar display
typedef struct LINE_INFO {
    s16 num; // Number of lines to display
    s16 dot[2]; // An icon's current dot
    s16 enabled;
    s8 *keyid[2];
} LINE_INFO;

// Describes an input for demo mode
typedef struct DEMO_INPUT {
    s16 time; // In 1/4ths of a beat
    u16 pad;
} DEMO_INPUT;

// Describes which input key and key control to use (In addition to some other stuff)
typedef struct INPUT_INFO_SET_SUB {
    s8 infoid;
    s8 ctrlid;
    s16 type;
    u16 flags;
} INPUT_INFO_SET_SUB;

// Describes input information for the bar (First key, activated keys etc)
typedef struct INPUT_INFO_SET {
    u8 lesson;
    s8 unk1;
    s8 firstinput;
    s8 teacherinputnum;
    s16 highthreshold;
    s32 validpad;
    INPUT_INFO_SET_SUB sub[2];
} INPUT_INFO_SET;

// Assigns a sound to an input
typedef struct INPUT_KEY_SND_MAP {
    SND_INFO snd;
    s16 vcount;
    s32 keyid;
} INPUT_KEY_SND_MAP;

// Describes a mapping of inputs to sounds (mapmax > 1 in case you want one type of input to make different sounds, i.e. duck/jump in stage 1 lesson 2)
typedef struct INPUT_KEY_INFO {
    INPUT_KEY_SND_MAP *keymap;
    s16 mapmax;
    s16 mapnum;
    u32 unk8;
} INPUT_KEY_INFO;

typedef INPUT_KEY_INFO *INPUT_KEY_INFO_LIST[9];
typedef u8 KEY_CTRL[16];

typedef struct SCENE_INIT_INFO {
    EIGHTH_TIMES *times;
    INPUT_INFO_SET *inpinfo;
    s32 inpinfolen;
    MOVIE_SUBTITLES *moviesub;
    VISUAL_LINE_INFO *visline;
    s32 vislinelen;
    INPUT_KEY_INFO_LIST *keyinfo;
    s32 keyinfolen;
    KEY_CTRL *keyctrl;
    DEMO_INPUT *demoinp;
    s32 demoinplen;
    SND_INFO *endlow;
    SND_INFO *hightonorm;
    SND_INFO *tohighfirst;
    SND_INFO *tohigh;
    SND_INFO *lvlchange;
    SND_INFO *scorechange;
    SND_INFO *click;
    SND_INFO *dbgstackcheck;
    SND_INFO *field19_0x4c;
    SND_INFO *intermission;
    SND_INFO *field21_0x54;
    SND_INFO *field22_0x58;
    SND_INFO *tryagain;
    SND_INFO *menutriangle;
    SND_INFO *menucross;
    SND_INFO *menucircle;
    SND_INFO *menuselect;
    void *keypressed;
    void *onkeypressed;
    void (*perfup)(struct SCENE_INFO *scn);
    void *anykeypressed;
    void *demoinit;
} SCENE_INIT_INFO;

typedef struct SCENE_INFO {
    u32 flags;
    PARA_TIME gametimet;
    PARA_TIME prs;
    s32 gametime; // 1/96th of a beat
    s32 taptime; // Game time at which an input was done
    u32 unk14;
    u32 pad;
    s32 practdraw;
    s32 keyid;
    u8 unk24[0x30 - 0x24];
    s32 dispscore;
    s32 leniency;
    s32 curline;
    u32 unk3C;
    INPUT_INFO_SET *teacherinp;
    INPUT_INFO_SET *parappainp;
    s32 practend;
    s16 practhighlight;
    s16 level;
    s16 lvlhigh;
    s16 notplayer;
    s16 drawtext;
    s16 unk56;
    s16 leveltoflash;
    s16 flashlevel;
    s16 drawlesson;
    s16 lesson;
    s16 unk60;
    s16 unk62;
    s16 ingame;
    s16 drawtextlang;
    s16 penalty;
    s16 clicksound;
    s16 dbgmsg;
    s16 threshold;
    s16 highcnt;
    s16 lownum;
    s16 unk74;
    s16 unk76;
    s16 drawbuttons;
    u8 pad7A[0x88 - 0x7a];
    LINE_INFO teacherline;
    LINE_INFO parappaline;
    u8 padA8[0xf8 - 0xa8];
    PARA_JP_TEXT *jptext;
    PARA_EN_TEXT *entext;
    u8 unk100[0x17e - 0x100];
    s16 leveltransition;
    u8 pad180[0x1d8 - 0x180];
} SCENE_INFO;

extern SCENE_INIT_INFO sceneInitInfo;
extern SCENE_INFO sceneInfo;

#endif
