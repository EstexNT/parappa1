#ifndef _COMMON_H_
#define _COMMON_H_

#include "include_asm.h"
#include "psyq/sys/types.h"

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

#define NO_VALUE -1


#define NELEM(x) (sizeof(x) / sizeof(x[0]))

#endif
