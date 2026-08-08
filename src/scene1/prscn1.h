#ifndef _SCENE1_PRSCN1_H_
#define _SCENE1_PRSCN1_H_

#include "common.h"
#include "prscene.h"
#include "prrap.h"
#include "prcompo.h"
#include "prcd.h"
#include "prvdbg.h"
#include "prstrm.h"
#include "prvdatal.h"
#include "prmemory.h"
#include "prapp.h"
#include "praction.h"


typedef struct SCENE1_CONTROL_INFO {
    s32 starttime;
    u32 flags; // TODO: Defines (Maybe same as in SCENE_INFO?)
    s16 bezhigh;
    s16 beznormal;
    s16 bezlow1;
    s16 bezlow2;
    u8 teacheranimhigh;
    u8 teacheranimnormal;
    u8 teacheranimlow1;
    u8 teacheranimlow2;
    u8 paraanimhigh;
    u8 paraanimnormal;
    u8 tamasel;
    u8 paraface;
    u8 teacherfacehigh;
    u8 teacherfacenormal;
    u8 teacherfacelow1;
    u8 teacherfacelow2;
    u8 mcface;
    u8 visfirst;
    u8 vissecond;
    u8 jpsel;
    u8 ensel;
    u8 unk21;
    u8 unk22;
    u8 unk23;
} SCENE1_CONTROL_INFO;

typedef struct SCENE1_CONTROL_LIST {
    SCENE1_CONTROL_INFO *info;
    s32 max;
    s32 cur;
} SCENE1_CONTROL_LIST;

extern PARA_JP_T scn1StageTextJp[];
extern s32 scn1StageTextJpLen;
extern PARA_EN_T scn1StageTextEn[];
extern s32 scn1StageTextEnLen;

extern VISUAL_LINE_INFO scn1VisualLineInfo[];
extern s32 scn1VisualLineInfoLen;
extern SCENE1_CONTROL_LIST scn1SceneControlList[9];

extern DEMO_INPUT scn1DemoInput[];
extern s32 scn1DemoInputLen;

#endif
