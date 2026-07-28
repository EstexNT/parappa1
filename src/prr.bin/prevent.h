#ifndef _PREVENT_H_
#define _PREVENT_H_

#include "common.h"
#include "prscene.h"
#include "prinfo.h"


// Not in cool
#define PR_COOLSTATE_NOTCOOL 0
// In cool
#define PR_COOLSTATE_COOL 1
// Player lost cool after not upholding the score quota
#define PR_COOLSTATE_LOSTCOOL 4
// Cool becoming normal
#define PR_COOLSTATE_TRANSCOOL 5
// Set right after losing cool rank
#define PR_COOLSTATE_IMMSTART 6
#define PR_COOLSTATE_IMMLOSTCOOL PR_COOLSTATE_IMMSTART
// Set right after achieving cool rank
#define PR_COOLSTATE_IMMGOTCOOL 7
// Cool became normal
#define PR_COOLSTATE_IMMNOTCOOL 8


// Model texture animation
// Determines when to apply a new texture for i.e. faces and hands on character models
typedef struct FACE_SET_INFO {
    s32 time;
    s16 ids[4];
} FACE_SET_INFO;

// List of model texture animations
typedef struct FACE_SET {
    FACE_SET_INFO *info;
    s32 max;
    s32 cur;
 } FACE_SET;

typedef struct FACE_SET_USE {
    s32 num;
    s32 starttime;
    FACE_SET *set;
} FACE_SET_USE;

typedef struct EVENT_RECORD_INFO {
    RECORD_DATA data[550];
    u32 unk;
    s32 num;
    s32 idx;
} EVENT_RECORD_INFO;

typedef struct EVENT_INFO {
    PARA_JP_TEXT *jptext;
    PARA_EN_TEXT *entext;
    s32 linetime;
    u32 unkC;
    s32 preveighth;
    s32 eighth; // 1/8th of a beat in a bar (Half a dot)
    s16 jptexttimer;
    s16 entexttimer;
    s16 paraidletimer;
    s16 teacheridletimer;
    s32 coolstate;
    u32 unk24;
    u32 inpflags;
    s32 highstarttime;
    u32 unk30;
    BOOL hightransitioning;
    u32 unk38;
    s32 finished;
    s32 gameover;
    s32 unk44;
    s32 transitionhigh;
    s16 controldelay;
    s16 controlstage;
    u32 unk50;
    s16 flashhigh;
    s16 lvlestimate;
    s16 normaleval;
    u8 unk5A[0x64 - 0x5a];
    FACE_SET_USE faceset[8];
} EVENT_INFO;


extern EVENT_RECORD_INFO eventRecordInfo;
extern EVENT_INFO eventInfo;

extern s32 eventCurMovieSub;


s32 EventGetKeyIndexFromPad(u32 pad);
u32 EventGetPadFromKeyIndex(s32 keyid);
void EventResetEstimate(SCENE_INFO *scn);
BOOL EventInputsInactive(SCENE_INFO *scn);
void EventGetDispScore(SCENE_INFO *scn);
SCENE_INFO *EventUpdate(SCENE_INFO *scn);
void EventSetMovieSubtitles(MOVIE_SUBTITLES *sub);
void EventSetFlashHigh(BOOL flashhigh);
void EventCheckTextOverride(SCENE_INFO *scn);
SCENE_INFO *EventUpdateText(SCENE_INFO *scn);
void EventRecordInit(BOOL init);
void EventReset(void);
void EventUpdateHigh(SCENE_INFO *scn);
void EventUpdateImmHigh(SCENE_INFO *scn);
void EventUpdatePerfomance(SCENE_INFO *scn);
void EventUpdateScore(SCENE_INFO *scn);
void EventCheckGameOver(SCENE_INFO *scn);
MOVIE_SUBTITLES_INFO *EventGetMovieSubInfo(PARA_TIME *t);
void EventCheckHighEnding(SCENE_INFO *scn);
BOOL EventCheckHighToNormal(SCENE_INFO *scn);
BOOL EventUpdateLevel(SCENE_INFO *scn, s32 dir);
s32 EventCheckPerformance(SCENE_INFO *scn);
void EventPlayScoreChange(s32 dir);
s32 EventCalcLvlEstimate(SCENE_INFO *scn);
void EventTransitionLevel(SCENE_INFO *scn, s32 dir);
void EventSetHighTransitionEnd(SCENE_INFO *scn);
void EventBeginHighMode(SCENE_INFO *scn);
void EventSetHighTransitionStart(SCENE_INFO *scn, s32 time);

#endif
