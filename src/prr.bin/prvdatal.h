#ifndef _PRVDATAL_H_
#define _PRVDATAL_H_

#include "common.h"
#include "prcd.h"


#define VDATA_SCENE_FILE_COMOD 0
#define VDATA_SCENE_FILE_COMPO 1
#define VDATA_SCENE_FILE_MOVIE 2
#define VDATA_SCENE_FILE_XA 3
#define VDATA_SCENE_FILE_XMOVIE 4
#define VDATA_SCENE_FILE_YMOVIE 5
#define VDATA_SCENE_FILE_ZCOMPO 6


typedef struct LVL_INFO {
    char *title;
    u32 channel;
    s16 neighbours[2];
} LVL_INFO;

typedef struct VDATA_SCENE {
    char *name;
    s16 unk4;
    s16 tracklen;
    s16 trackoffs;
    s16 tracksmalloffs;
    CD_FILE files[7];
    s32 tempo;
    s32 smalloffs;
    s32 timeoffs;
} VDATA_SCENE;

extern CD_FILE D_80067DE0;
extern CD_FILE D_80067E0C;
extern CD_FILE D_80067E38;
extern CD_FILE D_80067E64;
extern VDATA_SCENE D_80067E90[];
extern LVL_INFO D_80068A3C[];

#endif
