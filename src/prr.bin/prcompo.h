#ifndef _PRCOMPO_H_
#define _PRCOMPO_H_

#include "common.h"
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "prscene.h"
#include "prmime.h"


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
void CompoDrawTextJp(s32 *text, GsOT *ot);
void CompoSetScoreSprite(GsSPRITE *gspr, COMPO_SPRITE *spr, s32 grayed);
void CompoDrawScoreSprite(s32 x, s32 y, COMPO_SPRITE *spr, s32 grayed, GsOT *ot);
void CompoFrameSet(s32 b);
void CompoFrameResetActive(void);
void CompoFrameResetInactive(void);
void CompoFrameDrawText(s32 type, s32 pri, s32 idx);
s32 CompoFrameFinished(void);
void CompoFrameDrawCounterClockwiseImmediate(s32 pri, s32 n, s32 idx);
s32 CompoFrameSetAnimInactive(s32 type, s32 num);
s32 CompoFrameSetAnimActive(s32 num, s32 type);
void CompoFrameDrawAnim(s32 pri, s32 idx);
void CompoFrameDrawAnimMovie(s32 pri, s32 idx);
void CompoDrawFrameMovieGuiNoTextNoBorder (s32 prio, GsOT *ot);
void CompoFrameSetType(s32 type);
void CompoFrameMake(void *data, s32 type, s32 start, s32 end);
void CompoFrameMovieGuiMake(void *data, s32 type, s32 start, s32 end);
void CompoDrawFrameMovieGuiNoText(s32 pri, GsOT *ot);
void CompoDrawFrameMovieGui(s32 pri, GsOT *ot);
void CompoDrawFrameMenuGui(s32 pri, s32 i);
void func_80022e88(void);
void CompoDrawMovieTextEn(char *text, GsOT *ot);
void CompoDrawMovieTextJp(s32 *text, GsOT *ot);
void CompoDrawStageTextJp(s32 *text, s32 i);
void CompoDrawStageTextEn(char *text, s32 i);
void CompoDrawLesson(s32 lesson, s32 idx);
void CompoDrawHand(s32 startover, s32 downorup, s32 i);
void CompoDrawScore(s32 flash, s32 level, s32 flashlvl, s32 score, s32 lesson, s32 idx);
void CompoClutModSetRect(s32 x, s32 y, s32 w, s32 h);
void CompoClutModDraw(s32 n, u16 *clut);
void func_80024264(void);
void func_80024310(void);
void func_800243bc(void);
void func_800244e8(s32 t);
void func_8002464c(void);
void CompoRemoveLevelGrayTextBorder(void);
void func_8002471c(void);
void CompoLineButtonApplyScalingTeacher(s32 num);
void CompoLineButtonApplyScalingPara(s32 num);
void CompoLineButtonInitScalingTeacher(void);
void CompoLineButtonInitScalingPara(void);
void CompoResetLineButton(void);
void CompoDrawLineButton(struct LINE_BUTTON_DRAW_INFO *info, s32 idx);
void CompoDrawLineDots(s32 line, s32 dot, s32 i);
void CompoDrawTeacherIcon(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 i);
void CompoDrawParaIcon(void);
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
void CompoDrawSavingText(void);
void CompoDrawNameEnter(struct CARD_MENU_NAMEENTER_INFO *data);
void CompoDrawSlotSelect(s32 draw, struct CARD_MENU_SLOTSELECT_INFO *data);
void CompoDrawHighScore(void);
void CompoDrawMainMenu(struct MENU_MAIN_DEFDATA *data);
void CompoDrawSaveDialog(s32 draw, struct CARD_MENU_DIALOG_INFO *data);
void CompoDrawPracticeBar(s32 arg0);
void CompoDrawPractice(SCENE_INFO *scn);
void CompoSetPacket(PACKET *p0, PACKET *p1);
void CompoResetPacket(void);
void CompoClear(s32 i);
void CompoDrawOt(s32 inidex);
void CompoDrawTitleScreenSel(s32 *sel);
void CompoSonyFade(void);
void CompoSonyDrawFade(s32 x, s32 y, s32 t);
void CompoSonyLoad(void);
void CompoSonyDraw(s32 i);
void CompoSonySwap(void);
void CompoInitOt(void);
void CompoDrawMenu(s32 draw, void *data);
void CompoDrawMenuOt(s32 menu);
void CompoFrameDraw(void *data, s32 type);
void CompoFrameSwap(void);
void CompoFrameMovieGuiDraw(SCENE_INFO *scn, s32 type);
void CompoFrameMovieGuiSwap(void);
void CompoInit(void);
void func_8002f83c(void);
void CompoDrawThoughtBubble(void);
void CompoDrawOtThoughtBubble(void);
void func_8002fa2c(void);
void CompoDrawTextSaveTitle(char *text, s32 pal, GsOT *ot);
void CompoDrawTextEn(s32 cy, char *text, GsOT *ot);
void CompoFrameSetAnim(s32 type, s32 b);
void CompoFrameMakeLast(void *data, s32 type, void *pre, void *post);
void CompoFrameMovieGuiMakeLast(void *data, s32 type, void *pre, void *post);
void CompoFadeColor(s32 color, s32 i);

#endif
