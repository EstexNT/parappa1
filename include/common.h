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

// English text is simply ASCII
typedef char PARA_EN_TEXT;

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

// Initial information shared between scenes. TODO: Turn into a struct and move into an approriate scenecmn.h header
extern SND_INFO *D_800A0934;

#endif
