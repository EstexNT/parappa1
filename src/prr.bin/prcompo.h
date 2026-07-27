#ifndef _PRCOMPO_H_
#define _PRCOMPO_H_

#include "common.h"
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "prscene.h"
#include "prmime.h"
#include "prcmenu.h"


// LibGS object attribute bits

#define PARA_SPRITE_BRIGHT_OFF (1 << 6)

#define PARA_SPRITE_VFLIP (1 << 22)

#define PARA_SPRITE_HFLIP (1 << 23)

#define PARA_SPRITE_COLOR4BPP (0 << 24)
#define PARA_SPRITE_COLOR8BPP (1 << 24)
#define PARA_SPRITE_COLOR16BPP (2 << 24)

#define PARA_SPRITE_ROTATION (1 << 27)

#define PARA_SPRITE_TRANS_NORM (0 << 28)
#define PARA_SPRITE_TRANS_PADD (1 << 28)
#define PARA_SPRITE_TRANS_50ADD (2 << 28)
#define PARA_SPRITE_TRANS_PSUB (3 << 28)

#define PARA_SPRITE_TRANS_ON (1 << 30)
#define PARA_SPRITE_DISP_OFF (1 << 31)

#define COMPO_FRAME_TYPE_0 0
#define COMPO_FRAME_TYPE_1 1
#define COMPO_FRAME_TYPE_2 2
#define COMPO_FRAME_TYPE_3 3

#define COMPO_TURN_ICON_0 0
#define COMPO_TURN_ICON_1 1
#define COMPO_TURN_ICON_2 2
#define COMPO_TURN_ICON_3 3
#define COMPO_TURN_ICON_4 4
#define COMPO_TURN_ICON_5 5
#define COMPO_TURN_ICON_6 6
#define COMPO_TURN_ICON_7 7
#define COMPO_TURN_ICON_8 8

#define COMPO_FRAME_TEXT_TYPE_0 0
#define COMPO_FRAME_TEXT_TYPE_1 1
#define COMPO_FRAME_TEXT_TYPE_2 2
#define COMPO_FRAME_TEXT_TYPE_3 3
#define COMPO_FRAME_TEXT_TYPE_4 4

#define COMPO_FRAME_ANIM_TYPE_0 0
#define COMPO_FRAME_ANIM_TYPE_1 1
#define COMPO_FRAME_ANIM_TYPE_2 2
#define COMPO_FRAME_ANIM_TYPE_3 3
#define COMPO_FRAME_ANIM_TYPE_4 4
#define COMPO_FRAME_ANIM_TYPE_5 5
#define COMPO_FRAME_ANIM_TYPE_6 6
#define COMPO_FRAME_ANIM_TYPE_7 7
#define COMPO_FRAME_ANIM_TYPE_8 8
#define COMPO_FRAME_ANIM_TYPE_9 9
#define COMPO_FRAME_ANIM_TYPE_10 10

#define PR_FRAME_0 0
#define PR_FRAME_1 1
#define PR_FRAME_2 2
#define PR_FRAME_3 3
#define PR_FRAME_4 4
#define PR_FRAME_5 5
#define PR_FRAME_6 6
#define PR_FRAME_7 7
#define PR_FRAME_8 8
#define PR_FRAME_9 9

#define PR_MOVIE_0 0
#define PR_MOVIE_1 1
#define PR_MOVIE_2 2
#define PR_MOVIE_3 3
#define PR_MOVIE_4 4
#define PR_MOVIE_5 5
#define PR_MOVIE_6 6
#define PR_MOVIE_7 7
#define PR_MOVIE_8 8
#define PR_MOVIE_9 9
#define PR_MOVIE_10 10
#define PR_MOVIE_11 11
#define PR_MOVIE_12 12


typedef struct FRAME_TEXT_INFO {
    u32 color;
    u32 i;
} FRAME_TEXT_INFO;

typedef struct FONT_INFO {
    s16 tx;
    s16 ty;
    u8 w;
    u8 h;
} FONT_INFO;

typedef struct FRAME_POINT {
    s32 x;
    s32 y;
} FRAME_POINT;

typedef struct COMPO_SPRITE {
    u32 attr;
    s16 tx;
    s16 ty;
    s16 w;
    s16 h;
    s16 px;
    s16 py;
    s16 unk10;
    s16 tp;
} COMPO_SPRITE;

typedef struct LINE_BUTTON_DRAW_INFO {
    s16 x;
    s16 y;
    s16 scaleid;
    s16 keyid;
} LINE_BUTTON_DRAW_INFO;

// Scaling information for a button in the game bar
typedef struct LINE_BUTTON_SCALING { 
    s32 angle;
    s32 newscale;
    s32 linscale;
    s32 stage;
} LINE_BUTTON_SCALING;

// TODO: Move to prmenu
#define MENU_STAGE_ICON_0 0
#define MENU_STAGE_ICON_1 1
#define MENU_STAGE_ICON_2 2
#define MENU_STAGE_ICON_3 3
struct MENU_STAGE_DEFDATA {
    BOOL highlight;
    BOOL selected;
    s16 selcur;
    s16 selmax;
    s16 icons[9];
};
struct MENU_HIGHSCORE_DEFDATA {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s16 unkC;
    s16 unkE;
    s16 unk10;
    s16 unk12;
    char unk14[19][16];
};


extern s32 D_80082810;


void CompoUpdateWorkOfs(PACKET *packet);
void CompoLoadTimClut(u32 *timdata, BOOL clut);
void CompoInitGs(void);
s32 CompoInitTmd(u32 *tmd, GsDOBJ2 *obj, GsCOORDINATE2 *coord);
s32 CompoInitTmdAttr(u32 *tmd, u32 attr, GsDOBJ2 *obj, GsCOORDINATE2 *coord);
void CompoInitTod(u32 *tod, u32 **otod, s32 *oframes);
s32 CompoSetTod(s32 frame, s32 *framenum, u32 **todp, GsDOBJ2 *obj);
void CompoRegisterDObj(GsDOBJ2 *dobj, s32 len, GsOT *ot, s32 shift);
void CompoSetSpr(GsSPRITE *gspr, COMPO_SPRITE *spr, s32 shift, BOOL shiftpal);
void CompoSetSprScale(GsSPRITE *gspr, COMPO_SPRITE *spr, s32 sx, s32 sy);
void CompoSetSprSimple(GsSPRITE *gspr, COMPO_SPRITE *spr, BOOL adj);
void CompoSetSprSliding(GsSPRITE *gspr, COMPO_SPRITE *spr, s32 txoff, s32 w, s32 cyoff);
void CompoFastSpr(s16 x, s16 y, COMPO_SPRITE *spr, s32 shift, BOOL shiftpal, s32 pri, GsOT *ot);
void CompoSprScaled(s16 x, s16 y, s32 sx, s32 sy, COMPO_SPRITE *spr, s32 pri, GsOT *ot);
void CompoSimpleFastSpr(s16 x, s16 y, COMPO_SPRITE *spr, u32 arg3, BOOL adj, s32 pri, GsOT *ot);
void CompoSlidingFastSpr(s16 x, s16 y, COMPO_SPRITE *spr, s32 txoff, s32 w, s32 cyoff, s32 pri, GsOT *ot);
void CompoBox(s32 x, s32 y, s32 w, s32 h, u32 attr, s32 pri, GsOT *ot);
void CompoSetTextBox(s32 x, s32 y, s32 cx, s32 cy);
void CompoSetFontJp(s32 x, s32 y, s32 sx, s32 sy);
void CompoDrawTextJp(PARA_JP_TEXT *text, GsOT *ot);
void CompoSetScoreSprite(GsSPRITE *gspr, COMPO_SPRITE *spr, BOOL grayed);
void CompoDrawScoreSprite(s16 x, s16 y, COMPO_SPRITE *spr, BOOL grayed, GsOT *ot);
void CompoFrameSet(s32 b);
void CompoFrameResetActive(void);
void CompoFrameResetInactive(void);
void CompoFrameDrawText(s32 type, s32 pri, s32 idx);
BOOL CompoFrameFinished(void);
void CompoFrameDrawCounterClockwiseImmediate(s32 pri, s32 n, s32 idx);
BOOL CompoFrameSetAnimInactive(s32 type, s32 num);
s32 CompoFrameSetAnimActive(s32 num, s32 type);
void CompoFrameDrawAnim(s32 pri, s32 idx);
void CompoFrameDrawAnimMovie(s32 pri, s32 idx);
void CompoDrawFrameMovieGuiNoTextNoBorder(s32 prio, GsOT *ot);
void CompoFrameSetType(s32 type);
void CompoFrameMake(void *data, s32 type, s32 start, s32 end);
void CompoFrameMovieGuiMake(void *data, s32 type, s32 start, s32 end);
void CompoDrawFrameMovieGuiNoText(s32 pri, GsOT *ot);
void CompoDrawFrameMovieGui(s32 pri, GsOT *ot);
void CompoDrawFrameMenuGui(s32 pri, s32 idx);
void func_80022e88(void);
void CompoDrawMovieTextEn(PARA_EN_TEXT *text, GsOT *ot);
void CompoDrawMovieTextJp(PARA_JP_TEXT *text, GsOT *ot);
void CompoDrawStageTextJp(PARA_JP_TEXT *text, s32 i);
void CompoDrawStageTextEn(PARA_EN_TEXT *text, s32 i);
void CompoDrawLesson(s32 lesson, s32 idx);
void CompoDrawHand(s32 startover, s32 dir, s32 i);
void CompoDrawScore(BOOL flash, s32 level, s32 flashlvl, s32 score, s32 lesson, s32 idx);
void CompoClutModSetRect(s32 x, s32 y, s32 w, s32 h);
void CompoClutModDraw(s32 n, u16 *clut);
void func_80024264(s32 arg0, s32 arg1);
void func_80024310(s32 arg0, s32 arg1);
void func_800243bc(void);
void func_800244e8(s32 t);
void func_8002464c(void);
void CompoRemoveLevelGrayTextBorder(void);
void func_8002471c(LINE_BUTTON_DRAW_INFO *info, COMPO_SPRITE *spr, GsOT *ot);
void CompoLineButtonApplyScalingTeacher(s32 num);
void CompoLineButtonApplyScalingPara(s32 num);
void CompoLineButtonInitScalingTeacher(void);
void CompoLineButtonInitScalingPara(void);
void CompoResetLineButton(void);
void CompoDrawLineButton(LINE_BUTTON_DRAW_INFO *info, s32 idx);
void CompoDrawLineDots(s32 line, s32 dot, s32 i);
void CompoDrawTeacherIcon(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 i);
void CompoDrawParaIcon(s32 linei, s32 dot, s32 i);
void CompoSetTurnIcon(s32 turntype);
void CompoDrawLine(SCENE_INFO *scn, s32 i);
void CompoResetMime(void);
void CompoResetMimeSpecific(s32 i);
void CompoApplyDat(PARA_DAT_DATA *data, s32 frame, s32 idx);
void CompoApplyDatForVdf(PARA_DAT_DATA *data, s32 frame, s32 idx, s32 vdf);
void CompoSetModelAnim(s32 i, void *dat, void *vdf, PARA_DAT_DATA *data, PARA_DAT_OBJECT *obj, s32 *arg5);
void CompoSetMainModelAnim(void *dat, void *vdf, s32 frame, s32 framenum);
void CompoInitMainModelSet(void *dat, void *vdf, void *tmd, s32 wrap);
s32 CompoApplyMainDat(void);
void CompoResetFirstTmdVdf(void);
void CompoFlip(BOOL flip);
void CompoDrawTryAgainText(s32 sel);
void CompoDrawTitleScreen(s32 sel);
void CompoDrawStageSelect(void *data);
void CompoDrawSavingText(s32 arg0);
void CompoDrawNameEnter(void *data);
void CompoDrawSlotSelect(s32 draw, CARD_MENU_SLOTSELECT_INFO *data);
void CompoDrawHighScore(struct MENU_HIGHSCORE_DEFDATA *data);
void CompoDrawMainMenu(struct MENU_MAIN_DEFDATA *data);
void CompoDrawSaveDialog(s32 draw, CARD_MENU_DIALOG_INFO *data);
void CompoDrawPracticeBar(s32 arg0);
void CompoDrawPractice(SCENE_INFO *scn);
void CompoSetPacket(PACKET *p0, PACKET *p1);
void CompoResetPacket(void);
void CompoClear(s32 i);
void CompoDrawOt(s32 inidex);
void CompoDrawTitleScreenSel(s32 *sel);
void CompoSonyFade(s32 x, s32 y);
void CompoSonyDrawFade(s32 x, s32 y, s32 t);
void CompoSonyLoad(void);
void CompoSonyDraw(s32 i);
void CompoSonySwap(void);
void CompoInitOt(void);
void CompoDrawMenu(s32 draw, void *data);
void CompoDrawMenuOt(s32 menu, void *data);
void CompoFrameDraw(void *data, s32 type);
void CompoFrameSwap(void *data);
void CompoFrameMovieGuiDraw(SCENE_INFO *scn, s32 type);
void CompoFrameMovieGuiSwap(SCENE_INFO *scn);
void CompoInit(void);
void func_8002f83c(s32 r, s32 g, s32 b);
void CompoDrawThoughtBubble(SCENE_INFO *scn, BOOL draw);
void CompoDrawOtThoughtBubble(SCENE_INFO *scn);
void func_8002fa2c(void);
void CompoDrawTextSaveTitle(PARA_EN_TEXT *text, s32 pal, GsOT *ot);
void CompoDrawTextEn(s32 cy, PARA_EN_TEXT *text, GsOT *ot);
void CompoFrameSetAnim(s32 type, s32 b);
void CompoFrameMakeLast(void *data, s32 type, void (*pre)(void *data, s32 type), void (*post)(void *data));
void CompoFrameMovieGuiMakeLast(void *data, s32 type, void (*pre)(void *data, s32 type), void (*post)(void *data));
u16 CompoFadeColor(u16 color, s32 i);

#endif
