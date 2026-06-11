#ifndef _PRVDATAL_H_
#define _PRVDATAL_H_

#include "common.h"

typedef struct LVL_INFO {
    char *title;
    u32 channel;
    s16 neighbours[2];
} LVL_INFO;


extern LVL_INFO D_80068A3C[];

#endif
