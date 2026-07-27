#ifndef _COMMON_H_
#define _COMMON_H_

#include "include_asm.h"
#include <sys/types.h>

#ifndef NULL
#define NULL 0
#endif


typedef signed char 	   byte;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

#ifndef BOOL
#define BOOL int
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define NO_VALUE -1


#define NELEM(x) (sizeof(x) / sizeof(x[0]))


// Japanese text is a s16[50] array of indexes into the font atlas
typedef s16 PARA_JP_TEXT;
#define JP_TEXT_NUM 50
typedef PARA_JP_TEXT PARA_JP_T[JP_TEXT_NUM];

// English text is simply ASCII
typedef char PARA_EN_TEXT;
typedef PARA_EN_TEXT *PARA_EN_T;

// Describes a sound inside a vab
typedef struct SND_INFO {
    u8 prog;
    u8 tone;
    u8 note;
    u8 vol;
    s16 time; // Doesn't do anything usually
    // Only used to describe:
    // - When should good->cool rank transition end (in game time)
    // - In practice, for how long to stall when playing Kat's voice lines (in frames)
} SND_INFO;

typedef struct PARA_TIME {
    u16 min;
    u8 sec;
    u8 frame;
} PARA_TIME;


// Key index
#define PR_TAP_NONE 0
#define PR_TAP_TRIANGLE 1
#define PR_TAP_CIRCLE 2
#define PR_TAP_CROSS 3
#define PR_TAP_SQUARE 4
#define PR_TAP_L1 5
#define PR_TAP_L2 6
#define PR_TAP_R1 7
#define PR_TAP_R2 8
#define PR_TAP_NUM 9
#define PR_TAP_ICON_END 0xff

// Key codes (Pad)
#define PR_PAD_NONE 0
#define PR_PAD_L2 (1 << 0)
#define PR_PAD_R2 (1 << 1)
#define PR_PAD_L1 (1 << 2)
#define PR_PAD_R1 (1 << 3)
#define PR_PAD_TRIANGLE (1 << 4)
#define PR_PAD_CIRCLE (1 << 5)
#define PR_PAD_CROSS (1 << 6)
#define PR_PAD_SQUARE (1 << 7)
#define PR_PAD_SELECT (1 << 8)
#define PR_PAD_START (1 << 11)
#define PR_PAD_UP (1 << 12)
#define PR_PAD_RIGHT (1 << 13)
#define PR_PAD_DOWN (1 << 14)
#define PR_PAD_LEFT (1 << 15)


#define LEVEL_HIGH  0
#define LEVEL_NORM  1
#define LEVEL_LOW1  2
#define LEVEL_LOW2  3
#define LEVEL_XTIME 4


#define PARA_LANG_JP 0
#define PARA_LANG_EN 1


#define PR_MENU_0 0
#define PR_MENU_1 1
#define PR_MENU_STAGE 2
#define PR_MENU_MAIN 3
#define PR_MENU_START 4
#define PR_MENU_NAMEENTER 5
#define PR_MENU_HIGHSCORE 6
#define PR_MENU_SAVE 7
#define PR_MENU_LOAD 8
#define PR_MENU_REPLAY 9
#define PR_MENU_LOADING 10
#define PR_MENU_CARDSAVE 11
#define PR_MENU_CARDNOTINSERT 12
#define PR_MENU_CARDFORMAT 13
#define PR_MENU_CARDFULL 14
#define PR_MENU_CARDSAMENAME 15
#define PR_MENU_PRACTICE 16
#define PR_MENU_DEBUG 17


#endif
