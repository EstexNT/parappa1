#include "prcompo.h"
#include <libetc.h>
#include "prvdbg.h"
#include "prmime.h"
#include "praction.h"

// static char rcsid[] = "@(#)prcompo.c: version 01-00 95/12/10 00:00:00";

// TODO: Perhaps put all of data into a separate file
extern COMPO_SPRITE D_80062A30;

extern RECT D_800678C4[];



void CompoUpdateWorkOfs(PACKET *packet) {
    s32 diff;
    static s32 tmpoffs = 0;

    diff = GsGetWorkBase() - packet;
    if (diff > tmpoffs) {
        tmpoffs = diff;
    }
    dbgInfo.workoffs = diff;
    dbgInfo.workoffsmax = tmpoffs;
}

void CompoLoadTimClut(register u32 *timdata, register BOOL clut) {
    RECT r;
    GsIMAGE img;

    timdata++;
    GsGetTimInfo(timdata, &img);
    r.x = img.px;
    r.y = img.py;
    r.w = img.pw;
    r.h = img.ph;
    LoadImage(&r, img.pixel);

    if ((clut != FALSE) && ((img.pmode >> 3) & 1)) {
        LoadClut2(img.clut, img.cx, img.cy);
    }
}

void CompoInitGs(void) {
    RECT r;

    ResetGraph(1);
    PadInit(0);
    GsInitGraph(320, 240, 4, 0, 0);
    GsDefDispBuff(0, 0, 0, 240);
    GsInit3D();
    GsSetProjection(440);
    r.x = r.y = 0;
    r.w = 320;
    r.h = 480;
    ClearImage(&r, 0, 0, 0);
}

s32 CompoInitTmd(u32 *tmd, GsDOBJ2 *obj, GsCOORDINATE2 *coord) {
    GsDOBJ2 *iobj;
    u32 *tmdp;
    u32 unused;
    s32 i;
    s32 num;
    
    tmdp = ++tmd;
    GsMapModelingData(tmdp);
    
    tmdp++;
    num = *tmdp;

    tmdp++;
    for (i = 0; i < num; i++) {
        GsLinkObject4(tmdp, &obj[i], i);
    }

    for (i = 0, iobj = obj; i < num; iobj++, i++) {
        iobj->coord2 = coord;
        iobj->attribute = 0;
    }

    return num;
}

s32 CompoInitTmdAttr(u32 *tmd, u32 attr, GsDOBJ2 *obj, GsCOORDINATE2 *coord) {
    GsDOBJ2 *iobj;
    u32 *tmdp;
    u32 unused;
    s32 i;
    s32 num;
    
    tmdp = ++tmd;
    GsMapModelingData(tmdp);

    // BUG: Loading flag as number of objects
    // tmdp++;
    
    num = *tmdp++;

    tmdp++;
    for (i = 0; i < num; i++) {
        GsLinkObject4(tmdp, &obj[i], i);
    }

    for (i = 0, iobj = obj; i < num; iobj++, i++) {
        iobj->coord2 = coord;
        iobj->attribute = attr;
    }

    return num;
}

void CompoInitTod(register u32 *tod, register u32 **otod, register s32 *oframes) {
    *otod = ++tod;
    *oframes = **otod;
    (*otod)++;
}

s32 CompoSetTod(register s32 frame, register s32 *framenum, register u32 **todp, register GsDOBJ2 *obj) {
    u32 *prevtod;
    
    if (*todp == NULL) {
        return -1;
    }
    if (*framenum < 1) {
        return -1;
    }

    prevtod = *todp;
    *todp = MimeTodSetFrame(frame, *todp, obj, 0);
    if (prevtod != *todp) {
        (*framenum)--;
    }
    return *framenum;
}

void CompoRegisterDObj(GsDOBJ2 *dobj, s32 len, GsOT *ot, s32 shift) {
    MATRIX mtx;
    GsDOBJ2 *iobj;
    s32 i;

    for (i = 0, iobj = dobj; i < len; iobj++, i++) {
        GsGetLs(iobj->coord2, &mtx);
        GsSetLsMatrix(&mtx);
        GsSortObject4(iobj, ot, 14 - shift, getScratchAddr(0));
    }
}


void CompoSetSpr(register GsSPRITE *gspr, register COMPO_SPRITE *spr, register s32 shift, register BOOL shiftpal) {
    s32 u;

    gspr->attribute = spr->attr;
    gspr->w = spr->w;
    gspr->h = spr->h;
    if (spr->tp == 1) {
        u = (spr->tx << 1) + shift * spr->w;
        gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 1, (spr->ty & 0xff00) >> 0);
        gspr->u = u;
    } else {
        u = (spr->tx << 2) + shift * spr->w;
        gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 2, (spr->ty & 0xff00) >> 0);
        gspr->u = u;
    }
    gspr->v = spr->ty;
    gspr->cx = spr->px;
    if (shiftpal != FALSE) {
        gspr->cy = spr->py + shift;
    } else {
        gspr->cy = spr->py;
    }
}

void CompoSetSprScale(register GsSPRITE *gspr, register COMPO_SPRITE *spr, register s32 sx, register s32 sy) {
    s32 u;

    gspr->w = spr->w;
    gspr->h = spr->h;
    u = (spr->tx << 2);
    gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 2, (spr->ty & 0xff00) >> 0);
    gspr->u = u;
    gspr->v = spr->ty;
    gspr->cx = spr->px;
    gspr->cy = spr->py;
    gspr->scalex = sx;
    gspr->scaley = sy;
    gspr->mx = spr->w / 2;
    gspr->my = spr->h / 2;
}

void CompoSetSprSimple(register GsSPRITE *gspr, register COMPO_SPRITE *spr, register BOOL adj) {
    s32 u;

    gspr->attribute = spr->attr;
    gspr->w = spr->w;
    gspr->h = spr->h;
    u = (spr->tx << 2);
    gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 2, (spr->ty & 0xff00) >> 0);
    gspr->u = u;
    gspr->v = spr->ty;
    gspr->cx = spr->px;
    if (adj != FALSE) {
        gspr->cy = spr->py;
    } else {
        gspr->cy = spr->py + 1;
    }
}

void CompoSetSprSliding(register GsSPRITE *gspr, register COMPO_SPRITE *spr, register s32 txoff, register s32 w, register s32 cyoff) {
    s32 u;

    gspr->attribute = spr->attr;
    gspr->w = w;
    gspr->h = spr->h;
    u = (spr->tx << 2) + txoff;
    gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 2, (spr->ty & 0xff00) >> 0);
    gspr->u = u;
    gspr->v = spr->ty;
    gspr->cx = spr->px;
    gspr->cy = spr->py + cyoff;
}

void CompoFastSpr(register s16 x, register s16 y, register COMPO_SPRITE *spr, register s32 shift, register BOOL shiftpal, register s32 pri, register GsOT *ot) {
    GsSPRITE gspr;

    gspr.x = x - (320 / 2);
    gspr.y = y - (240 / 2);

    CompoSetSpr(&gspr, spr, shift, shiftpal);
    GsSortFastSprite(&gspr, ot, pri);
}

void CompoSprScaled(register s16 x, register s16 y, register s32 sx, register s32 sy, register COMPO_SPRITE *spr, register s32 pri, register GsOT *ot) {
    GsSPRITE gspr;

    gspr.x = x - (320 / 2);
    gspr.y = y - (240 / 2);
    gspr.attribute = spr->attr;
    gspr.rotate = 0;

    CompoSetSprScale(&gspr, spr, sx, sy);
    GsSortSprite(&gspr, ot, pri);
}

void CompoSimpleFastSpr(register s16 x, register s16 y, register COMPO_SPRITE *spr, register u32 arg3, register BOOL adj, register s32 pri, register GsOT *ot) {
    GsSPRITE gspr;

    gspr.x = x - (320 / 2);
    gspr.y = y - (240 / 2);

    CompoSetSprSimple(&gspr, spr, adj);
    GsSortFastSprite(&gspr, ot, pri);
}

void CompoSlidingFastSpr(register s16 x, register s16 y, register COMPO_SPRITE *spr, register s32 txoff, register s32 w, register s32 cyoff, register s32 pri, register GsOT *ot) {
    GsSPRITE gspr;

    gspr.x = x - (320 / 2);
    gspr.y = y - (240 / 2);

    CompoSetSprSliding(&gspr, spr, txoff, w, cyoff);
    GsSortFastSprite(&gspr, ot, pri);
}

void CompoBox(register s32 x, register s32 y, register s32 w, register s32 h, u32 attr, s32 pri, GsOT *ot) {
    RECT r;
    GsBOXF box;

    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;

    box.attribute = attr & 0xff000000;
    box.x = r.x - (320 / 2);
    box.y = r.y - (240 / 2);
    box.w = r.w;
    box.h = r.h;
    box.r = (attr >> 0x10);
    box.g = (attr >> 8);
    box.b = (attr >> 0);
    GsSortBoxFill(&box, ot, pri);
}


extern s32 D_80082748;
extern u16 D_8008274C;
extern u16 D_80082750;
extern u16 D_80082754;
extern u16 D_80082758;
extern u16 D_8008275C;
extern u16 D_80082760;
extern s32 D_80082764;
extern s32 D_80082768;
extern s32 D_8008276C;
extern s32 D_80082770;
extern GsSPRITE D_80092B60;

void CompoSetTextBox(s32 x, s32 y, s32 cx, s32 cy) {
    D_80082754 = x;
    D_80082758 = y;
    D_80092B60.cx = D_8008275C = cx;
    D_80092B60.cy = D_80082760 = cy;
    D_80092B60.attribute = D_80062A30.attr;
}

void CompoSetFontJp(s32 x, s32 y, s32 sx, s32 sy) {
    D_80082764 = x;
    D_80082768 = y;
    D_8008276C = sx;
    D_80082770 = sy;
    D_80092B60.w = 12;
    D_80092B60.h = 12;
}

void CompoDrawTextJp(register PARA_JP_TEXT *text, register GsOT *ot) {
    s32 i;
    s32 ch;
    s16 u;
    s16 v;
    s32 dx;
    s32 dy;

    dx = 0;
    dy = 0;
    if ((text == NULL) || (*text == 0)) {
        return;
    }

    for (i = 0; text[i] != 0; i++) {
        if (text[i] == -1) {
            dy++;
            dx = 0;
            i++;
            if (text[i] == 0) {
                return;
            }
        } else if (text[i] == 1) {
            dx++;
            continue;
        }
        ch = text[i] - 1;
        D_80092B60.x = D_80082754 + dx * 12 - (320 / 2);
        dx++;
        D_80092B60.y = D_80082758 + dy * 12 - (240 / 2);

        v = D_80082768 + (ch / D_8008276C) * 12;
        u = (D_80082764 << 2) + (ch % D_8008276C) * D_80092B60.w;

        D_80092B60.tpage = GetTPage(0, 1, (u & 0xff00) >> 2, (v & 0xff00));
        D_80092B60.u = u;
        D_80092B60.v = v;
        GsSortFastSprite(&D_80092B60, ot, 0);
    }
}


INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSetScoreSprite);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawScoreSprite);


// TODO: .sdata
static s32 D_800823EC = 0;
static s32 D_800823F0 = 0;
static s32 D_800823F4 = 0;
static s32 D_800823F8 = 0;

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSet);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameResetActive);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameResetInactive);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameDrawText);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameFinished);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameDrawCounterClockwiseImmediate);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSetAnimInactive);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSetAnimActive);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameDrawAnim);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameDrawAnimMovie);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawFrameMovieGuiNoTextNoBorder);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSetType);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMake);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMovieGuiMake);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawFrameMovieGuiNoText);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawFrameMovieGui);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawFrameMenuGui);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", func_80022e88);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawMovieTextEn);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawMovieTextJp);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawStageTextJp);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawStageTextEn);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawLesson);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawHand);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawScore);


extern RECT D_80082774;

void CompoClutModSetRect(s32 x, s32 y, s32 w, s32 h) {
    D_80082774.x = x;
    D_80082774.y = y;
    D_80082774.w = w;
    D_80082774.h = h;
}

void CompoClutModDraw(s32 n, u16 *clut) {
    u16 tmp[256];
    s32 i;

    for (i = 0; i < n; i++) {
        tmp[i] = *clut++;
    }
    LoadImage(&D_80082774, (u_long *)tmp);
    DrawSync(0);
}

void func_80024264(register s32 arg0, s32 arg1) {
    u16 c;
    u16 tmp[16];
    u32 unused[6];

    c = arg0;
    CompoClutModSetRect(80, 492, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    tmp[arg1] = c;
    CompoClutModDraw(16, tmp);
}

void func_80024310(register s32 arg0, s32 arg1) {
    u16 c;
    u16 tmp[16];
    u32 unused[6];

    c = arg0;
    CompoClutModSetRect(272, 480, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    tmp[arg1] = c;
    CompoClutModDraw(16, tmp);
}

void func_800243bc(void) {
    u16 tmp[16];
    s32 i;

    CompoClutModSetRect(48, 493, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    DrawSync(0);
    for (i = 0; i < 16; i++) {
        if (tmp[i] != 0) {
            tmp[i] |= 0x8000;
        }
    }
    CompoClutModDraw(16, tmp);
    DrawSync(0);
}

void func_800244e8(s32 t) {
    u16 tmp[16];
    s32 i;

    for (i = 480; i < 486; i++) {
        CompoClutModSetRect(32, i, 16, 1);
        StoreImage(&D_80082774, (u_long *)tmp);
        DrawSync(0);
        tmp[15] = (t == FALSE) ? 0x8000 : 0x0000;
        LoadClut2((u_long *)tmp, 32, i);
        DrawSync(0);
    }
    CompoClutModSetRect(32, 490, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    DrawSync(0);
    tmp[15] = (t == FALSE) ? 0x8000 : 0x0000;
    LoadClut2((u_long *)tmp, 32, 490);
    DrawSync(0);
}

void func_8002464c(void) {
    u16 tmp[16];

    CompoClutModSetRect(128, 492, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    tmp[14] = 0;
    CompoClutModDraw(16, tmp);
}

void CompoRemoveLevelGrayTextBorder(void) {
    u16 tmp[16];

    CompoClutModSetRect(304, 491, 16, 1);
    StoreImage(&D_80082774, (u_long *)tmp);
    tmp[3] = 0;
    CompoClutModDraw(16, tmp);
}


INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", func_8002471c);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoLineButtonApplyScalingTeacher);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoLineButtonApplyScalingPara);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoLineButtonInitScalingTeacher);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoLineButtonInitScalingPara);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoResetLineButton);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawLineButton);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawLineDots);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTeacherIcon);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawParaIcon);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSetTurnIcon);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawLine);


void CompoResetMime(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 128; j++) {
            actionMimeList[i][j] = 0;
        }
    }
}

void CompoResetMimeSpecific(register s32 i) {
    s32 j;

    for (j = 0; j < 128; j++) {
        actionMimeList[i][j] = 0;
    }
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoApplyDat);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoApplyDatForVdf);

void CompoSetModelAnim(s32 i, void *dat, void *vdf, PARA_DAT_DATA *data, PARA_DAT_OBJECT *obj, s32 *arg5) {
    *arg5 = 0;
    ActionSetVdfData(i, vdf);
    MimeDatInit(data, obj, dat, FALSE);
    CompoResetMimeSpecific(i);
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSetMainModelAnim);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoInitMainModelSet);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoApplyMainDat);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoResetFirstTmdVdf);


void CompoFlip(BOOL flip) {
    RECT r;
    s32 i;

    i = GsGetActiveBuff();
    r.x = 0;
    r.w = 320;
    r.h = 240;
    if (flip != FALSE) {
        r.y = (i == 0) ? 240 : 0;
        MoveImage(&r, 0, (i == 0) ? 0 : 240);
    } else {
        r.y = (i == 0) ? 0 : 240;
        MoveImage(&r, 0, (i == 0) ? 240 : 0);
    }
}


INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTryAgainText);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTitleScreen);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawStageSelect);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawSavingText);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawNameEnter);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawSlotSelect);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawHighScore);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawMainMenu);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawSaveDialog);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawPracticeBar);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawPractice);


extern PACKET *D_800827E0[2];
extern PACKET D_8008C528[2][13000];
extern GsOT D_80092AB8[2];
extern GsOT_TAG D_80092AE0[2][16];

void CompoSetPacket(PACKET *p0, PACKET *p1) {
    D_800827E0[0] = p0;
    D_800827E0[1] = p1;
}

void CompoResetPacket(void) {
    CompoSetPacket(D_8008C528[0], D_8008C528[1]);
}

void CompoClear(s32 i) {
    GsClearOt(0, 0, &D_80092AB8[i]);
}

void CompoDrawOt(s32 i) {
    GsDrawOt(&D_80092AB8[i]);
}


INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTitleScreenSel);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSonyFade);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSonyDrawFade);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSonyLoad);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSonyDraw);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoSonySwap);


void CompoInitOt(void) {
    s32 i;

    for (i = 0; i < 2; i++) {
        D_80092AB8[i].length = 4;
        D_80092AB8[i].org = D_80092AE0[i];
        D_80092AB8[i].offset = 0;

    }
    CompoSetPacket(D_8008C528[0], D_8008C528[1]);
}


INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawMenu);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawMenuOt);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameDraw);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSwap);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMovieGuiDraw);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMovieGuiSwap);


void CompoInit(void) {
    CompoInitGs();
    CompoInitOt();
}

void func_8002f83c(s32 r, s32 g, s32 b) {
    RECT rect;

    rect.x = rect.y = 0;
    rect.w = 320;
    rect.h = 480;
    ClearImage(&rect, r, g, b);
}

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawThoughtBubble);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawOtThoughtBubble);

void func_8002fa2c(void) {
    u32 i;

    for (i = 0; i < 2; i++) {
        ClearImage(&D_800678C4[i], 0, 0, 0);
        DrawSync(0);
    }
}

// TODO: Uses gp. Split

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTextSaveTitle);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoDrawTextEn);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameSetAnim);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMakeLast);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo", CompoFrameMovieGuiMakeLast);

u16 CompoFadeColor(register u16 color, register s32 i) {
    u32 sp0;
    u32 sp4;
    u32 sp8;
    u32 spC;
    u32 sp10;
    u32 sp14;
    u32 sp18;
    u16 sp1C;

    if ((color & 0xffff) == 0) {
        return color;
    }

    sp0 = ((color >> 0) & 0x1f);
    sp4 = ((color >> 5) & 0x1f);
    sp8 = ((color >> 10) & 0x1f);

    spC = 0x8000;
    sp10 = (sp0 * i) / 30;
    sp14 = (sp4 * i) / 30;
    sp18 = (sp8 * i) / 30;

    sp1C = spC | (sp18 << 10) | (sp14 << 5) | (sp10 << 0);
    return sp1C;
}
