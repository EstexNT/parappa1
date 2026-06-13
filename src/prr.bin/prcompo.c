#include "prcompo.h"
#include <stdio.h>
#include <libetc.h>
#include "prvdbg.h"
#include "praction.h"
#include "prrap.h"
#include "prcompo_data.h"

static char rcsid[] = "@(#)prcompo.c: version 01-00 95/12/10 00:00:00";



extern PACKET *D_800827E0[2];
extern PACKET D_8008C528[2][13000];
extern GsOT D_80092AB8[2];
extern GsOT_TAG D_80092AE0[2][16];
extern s32 D_800827D4;


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
    box.g = (attr >> 0x08);
    box.b = (attr >> 0x00);
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


void CompoSetScoreSprite(register GsSPRITE *gspr, register COMPO_SPRITE *spr, register BOOL grayed) {
    s32 u;

    gspr->attribute = spr->attr;
    gspr->w = spr->w;
    gspr->h = spr->h;
    u = (spr->tx << 2);
    gspr->tpage = GetTPage(spr->tp, 1, (u & 0xff00) >> 2, (spr->ty & 0xff00) >> 0);
    gspr->u = u;
    gspr->v = spr->ty;
    gspr->cx = spr->px;
    if (grayed != FALSE) {
        gspr->cy = 491;
    } else {
        gspr->cy = spr->py;
    }
}

void CompoDrawScoreSprite(register s16 x, register s16 y, register COMPO_SPRITE *spr, register BOOL grayed, register GsOT *ot) {
    GsSPRITE gspr;

    gspr.x = x - (320 / 2);
    gspr.y = y - (240 / 2);

    CompoSetScoreSprite(&gspr, spr, grayed);
    GsSortFastSprite(&gspr, ot, 3);
}


s32 D_800823EC = 0;
s32 D_800823F0 = 0;
u32 D_800823F4 = 0;
s32 D_800823F8 = 0;

extern u8 D_80092B84[12][16];

void CompoFrameSet(register s32 b) {
    s32 i;
    s32 j;

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            D_80092B84[i][j] = b;
        }
    }
}

void CompoFrameResetActive(void) {
    D_800823F4 = 0;
    D_800823EC = 0;
    D_800823F0 = 0;
    CompoFrameSet(1);
}

void CompoFrameResetInactive(void) {
    D_800823F4 = 0;
    D_800823EC = 0;
    D_800823F0 = 0;
    CompoFrameSet(0);
}

void CompoFrameDrawText(register s32 type, register s32 pri, register s32 idx) {
    s32 i;
    s32 j;
    u32 sp28;
    s32 texttype;

    switch (type) {
        case COMPO_FRAME_TEXT_TYPE_4: {
            texttype = 3;
            break;
        }
        case COMPO_FRAME_TEXT_TYPE_3: {
            texttype = 2;
            break;
        }
        case COMPO_FRAME_TEXT_TYPE_2: {
            texttype = 1;
            break;
        }
        case COMPO_FRAME_TEXT_TYPE_1: 
        default: {
            texttype = 0;
            break;
        }
    }

    if ((D_800823F4 % 3) == 0) {
        if (D_800823EC > (32 - 1)) {
            D_800823EC = 0;
            D_800823F0++;
        }
        if (D_800823F0 == D_80067690[texttype].i) {
            D_800823F0 = 0;
            D_800823EC = 0;
        }

        sp28 = (32 - 1) - D_800823EC;
        for (j = 0; j < 12; j++) {
            for (i = 0; i < (16 - 1); i++) {
                D_80092B84[j][i] = D_80092B84[j][i + 1];
            }
            switch (texttype) {
                case 3: {
                    D_80092B84[j][16 - 1] = (D_8006761C[j][D_800823F0] & (1 << sp28)) > 0;
                    break;
                }
                case 2: {
                    D_80092B84[j][16 - 1] = (D_800675BC[j][D_800823F0] & (1 << sp28)) > 0;
                    break;
                }
                case 1: {
                    D_80092B84[j][16 - 1] = (D_8006755C[j][D_800823F0] & (1 << sp28)) > 0;
                    break;
                }
                case 0:
                default: {
                    D_80092B84[j][16 - 1] = (D_800674FC[j][D_800823F0] & (1 << sp28)) > 0;
                    break;
                }
            }
        }

        D_800823EC++;
    }
    D_800823F4++;
    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            if (D_80092B84[i][j] != 0) {
                CompoBox((s16)(j * 20), (s16)(i * 20), 20, 20, D_80067690[texttype].color, pri, &D_80092AB8[idx]);
            } else {
                CompoFastSpr(j * 20, i * 20, D_800674DC[D_8006741C[i][j]][D_80092B84[i][j]], 0, FALSE, pri, &D_80092AB8[idx]);
            }
        }
    }
}

BOOL CompoFrameFinished(void) {
    if (D_800823EC > 190) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void CompoFrameDrawCounterClockwiseImmediate(register s32 pri, register s32 n, register s32 idx) {
    s32 i;
    s32 j;

    if (D_800823EC >= 194) {
        return;
    }

    for (i = 0; i < n; i++) {
        if (D_800823EC < 192) {
            D_80092B84[D_800649DC[D_800823EC].x][D_800649DC[D_800823EC].y] = 0;
        }
        D_800823EC++;
    }

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            if (D_80092B84[i][j] != 0) {
                CompoFastSpr(j * 20, i * 20, D_800674DC[D_8006741C[i][j]][0], 0, FALSE, pri, &D_80092AB8[idx]);
            }
        }
    }
}

BOOL CompoFrameSetAnimInactive(register s32 type, register s32 num) {
    s32 i;

    for (i = 0; i < num; i++) {
        if (D_800823EC < 192) {
            CompoFrameSetAnim(type, 0);
        }
        D_800823EC++;
    }
    return TRUE;
}

BOOL CompoFrameSetAnimActive(register s32 num, register s32 type) {
    s32 i;

    for (i = 0; i < num; i++) {
        if (D_800823EC < 192) {
            CompoFrameSetAnim(type, 1);
        }
        D_800823EC++;
    }
    return TRUE;
}

void CompoFrameDrawAnim(register s32 pri, register s32 idx) {
    s32 i;
    s32 j;

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            if (D_80092B84[i][j] != 0) {
                CompoFastSpr(j * 20, i * 20, D_800674DC[D_8006741C[i][j]][0], 0, FALSE, pri, &D_80092AB8[idx]);
            }
        }
    }
}

void CompoFrameDrawAnimMovie(register s32 pri, register s32 idx) {
    s32 i;
    s32 j;

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            if (D_80092B84[i][j] != 0) {
                CompoFastSpr(j * 20, i * 20, D_800674DC[D_8006741C[i][j]][0], 0, FALSE, pri, &D_80092AB8[idx]);
            }
        }
    }
}

void CompoDrawFrameMovieGuiNoTextNoBorder(register s32 prio, register GsOT *ot) {
    s32 i;
    s32 j;

    for (i = 0; i < 8; i++) {
        for (j = 0; j < 6; j++) {
            CompoFastSpr(i * 40, j * 40, &D_80064204, 0, FALSE, prio, ot);
            CompoFastSpr(20 + i * 40, j * 40, &D_80064218, 0, FALSE, prio, ot);
            CompoFastSpr(i * 40, 20 + j * 40, &D_8006422C, 0, FALSE, prio, ot);
            CompoFastSpr(20 + i * 40, 20 + j * 40, &D_80064240, 0, FALSE, prio, ot);
        }
    }
}

void CompoFrameSetType(s32 type) {
    switch (type) {
        case COMPO_FRAME_TYPE_2: {
            D_800823EC = 52;
            CompoFrameSet(0);
            break;
        }
        case COMPO_FRAME_TYPE_1: {
            D_800823EC = 0;
            CompoFrameSet(0);
            break;
        }
        case COMPO_FRAME_TYPE_3: 
        default: {
            D_800823EC = 0;
            CompoFrameSet(1);
            break;
        }
    }
}

void CompoFrameMake(void *data, s32 type, s32 start, s32 end) {
    CompoFrameSetType(start);
    do {
        CompoFrameDraw(data, type);
        RapPlayFrameActive(30);
        VSync(2);
        CompoFrameSwap(data);
    } while (CompoFrameFinished() != FALSE);
    CompoFrameSetType(end);
    CompoFrameMakeLast(data, type, CompoFrameDraw, CompoFrameSwap);
}

void CompoFrameMovieGuiMake(void *data, s32 type, s32 start, s32 end) {
    CompoFrameSetType(start);
    do {
        CompoFrameDraw(data, type);
        VSync(2);
        CompoFrameSwap(data);
    } while (CompoFrameFinished() != FALSE);
    CompoFrameSetType(end);
    D_800823EC = 190;
    CompoFrameMovieGuiMakeLast(data, type, CompoFrameDraw, CompoFrameSwap);
}

void CompoDrawFrameMovieGuiNoText(register s32 pri, register GsOT *ot) {
    s32 i;

    CompoFastSpr(20, 20, &D_8006418C, 0, FALSE, pri, ot);
    CompoFastSpr(280, 20, &D_800641A0, 0, FALSE, pri, ot);
    CompoFastSpr(40, 20, &D_800641B4, 0, FALSE, pri, ot);
    CompoFastSpr(160, 20, &D_800641C8, 0, FALSE, pri, ot);
    CompoFastSpr(40, 160, &D_800641DC, 0, FALSE, pri, ot);
    CompoFastSpr(160, 160, &D_800641F0, 0, FALSE, pri, ot);
    for (i = 0; i < 7; i++) {
        CompoFastSpr(40 + i * 40, 0, &D_80064204, 0, FALSE, pri, ot);
        CompoFastSpr(20 + i * 40, 0, &D_80064218, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 220, &D_8006422C, 0, FALSE, pri, ot);
        CompoFastSpr(20 + i * 40, 220, &D_80064240, 0, FALSE, pri, ot);
    }
    for (i = 0; i < 6; i++) {
        CompoFastSpr(0, 20 + i * 40, &D_8006422C, 0, FALSE, pri, ot);
        CompoFastSpr(0, 0 + i * 40, &D_80064204, 0, FALSE, pri, ot);
        CompoFastSpr(300, 20 + i * 40, &D_80064240, 0, FALSE, pri, ot);
        CompoFastSpr(300, 0 + i * 40, &D_80064218, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 200, &D_80064204, 0, FALSE, pri, ot);
        CompoFastSpr(60 + i * 40, 200, &D_80064218, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 180, &D_8006422C, 0, FALSE, pri, ot);
        CompoFastSpr(60 + i * 40, 180, &D_80064240, 0, FALSE, pri, ot);
    }
    CompoFastSpr(280, 200, &D_80064204, 0, FALSE, pri, ot);
    CompoFastSpr(20, 200, &D_80064218, 0, FALSE, pri, ot);
    CompoFastSpr(280, 180, &D_8006422C, 0, FALSE, pri, ot);
    CompoFastSpr(20, 180, &D_80064240, 0, FALSE, pri, ot);
}

void CompoDrawFrameMovieGui(register s32 pri, register GsOT *ot) {
    s32 i;

    CompoFastSpr(280, 200, &D_800642A4, 0, FALSE, pri, ot);
    CompoFastSpr(20, 200, &D_800642B8, 0, FALSE, pri, ot);
    CompoFastSpr(280, 180, &D_800642CC, 0, FALSE, pri, ot);
    CompoFastSpr(20, 180, &D_800642E0, 0, FALSE, pri, ot);
    for (i = 0; i < 7; i++) {
        CompoFastSpr(40 + i * 40, 0, &D_80064204, 0, FALSE, pri, ot);
        CompoFastSpr(20 + i * 40, 0, &D_80064218, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 220, &D_8006422C, 0, FALSE, pri, ot);
        CompoFastSpr(20 + i * 40, 220, &D_80064240, 0, FALSE, pri, ot);
    }
    for (i = 0; i < 6; i++) {
        CompoFastSpr(0, 20 + i * 40, &D_8006422C, 0, FALSE, pri, ot);
        CompoFastSpr(0, 0 + i * 40, &D_80064204, 0, FALSE, pri, ot);
        CompoFastSpr(300, 20 + i * 40, &D_80064240, 0, FALSE, pri, ot);
        CompoFastSpr(300, 0 + i * 40, &D_80064218, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 200, &D_8006413C, 0, FALSE, pri, ot);
        CompoFastSpr(60 + i * 40, 200, &D_80064150, 0, FALSE, pri, ot);
        CompoFastSpr(40 + i * 40, 180, &D_80064164, 0, FALSE, pri, ot);
        CompoFastSpr(60 + i * 40, 180, &D_80064178, 0, FALSE, pri, ot);
    }
    CompoFastSpr(40, 160, &D_800641DC, 0, FALSE, pri, ot);
    CompoFastSpr(160, 160, &D_800641F0, 0, FALSE, pri, ot);
    CompoFastSpr(20, 20, &D_8006418C, 0, FALSE, pri, ot);
    CompoFastSpr(280, 20, &D_800641A0, 0, FALSE, pri, ot);
    CompoFastSpr(40, 20, &D_800641B4, 0, FALSE, pri, ot);
    CompoFastSpr(160, 20, &D_800641C8, 0, FALSE, pri, ot);
}

void CompoDrawFrameMenuGui(register s32 pri, register s32 idx) {
    s32 i;
    s32 j;

    CompoFastSpr(20, 20, &D_8006409C, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(20, 120, &D_800640B0, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(280, 20, &D_800640C4, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(280, 120, &D_800640D8, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(40, 20, &D_800640EC, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(160, 20, &D_80064100, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(40, 200, &D_80064114, 0, FALSE, pri, &D_80092AB8[idx]);
    CompoFastSpr(160, 200, &D_80064128, 0, FALSE, pri, &D_80092AB8[idx]);
    for (i = 0; i < 6; i++) {
        for (j = 0; j < 4; j++) {
            CompoFastSpr(40 + i * 40, 40 + j * 40, &D_80064088, 0, FALSE, pri, &D_80092AB8[idx]);
        }
    }
    for (i = 0; i < 7; i++) {
        CompoFastSpr(40 + i * 40, 0, &D_80064204, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(20 + i * 40, 0, &D_80064218, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(40 + i * 40, 220, &D_8006422C, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(20 + i * 40, 220, &D_80064240, 0, FALSE, pri, &D_80092AB8[idx]);
    }
    for (i = 0; i < 6; i++) {
        CompoFastSpr(0, 0 + i * 40, &D_80064204, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(300, 0 + i * 40, &D_80064218, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(0, 20 + i * 40, &D_8006422C, 0, FALSE, pri, &D_80092AB8[idx]);
        CompoFastSpr(300, 20 + i * 40, &D_80064240, 0, FALSE, pri, &D_80092AB8[idx]);
    }
}

void func_80022e88(void) {
    D_800823F8 = 0;
}


void CompoDrawMovieTextEn(register PARA_EN_TEXT *text, register GsOT *ot) {
    D_80082748 = 0;
    D_8008274C = 24;
    D_80082750 = 184;
    CompoDrawTextEn(482, text, ot);
    CompoDrawFrameMovieGui(5, ot);
    if (sceneInfo.dbgmsg != FALSE) {
        if (sceneInfo.clicksound != FALSE) {
            CompoBox(4, 15, 300, 17, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, ot);
        } else {
            CompoBox(4, 15, 300, 17, 0x0f0f0f, 2, ot);
        }
    }
}

void CompoDrawMovieTextJp(register PARA_JP_TEXT *text, register GsOT *ot) {
    u32 unused;

    dbgInfo.jptext = text;
    CompoDrawFrameMovieGui(5, ot);
    if (sceneInfo.dbgmsg != FALSE) {
        if (sceneInfo.clicksound != FALSE) {
            CompoBox(4, 15, 300, 17, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, ot);
        } else {
            CompoBox(4, 15, 300, 17, 0x0f0f0f, 2, ot);
        }
    }
    CompoDrawTextJp(text, ot);
}

void CompoDrawStageTextJp(register PARA_JP_TEXT *text, register s32 i) {
    dbgInfo.jptext = text;
    CompoBox(10, 203, 8, 14, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoBox(18, 195, 284, 30, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoBox(302, 203, 8, 14, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoFastSpr(10, 195, &D_80062B20, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(10, 217, &D_80062B0C, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(302, 195, &D_80062AF8, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(302, 217, &D_80062AE4, 0, FALSE, 2, &D_80092AB8[i]);
    CompoDrawTextJp(text, &D_80092AB8[i]);

    if (sceneInfo.dbgmsg != FALSE) {
        if (sceneInfo.clicksound != FALSE) {
            CompoBox(4, 71, 313, 120, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
        } else {
            CompoBox(4, 71, 313, 120, 0x0f0f0f, 2, &D_80092AB8[i]);
        }
    }
}

void CompoDrawStageTextEn(register PARA_EN_TEXT *text, register s32 i) {
    CompoBox(10, 203, 8, 14, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoBox(18, 195, 284, 30, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoBox(302, 203, 8, 14, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
    CompoFastSpr(10, 195, &D_80062B20, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(10, 217, &D_80062B0C, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(302, 195, &D_80062AF8, 0, FALSE, 2, &D_80092AB8[i]);
    CompoFastSpr(302, 217, &D_80062AE4, 0, FALSE, 2, &D_80092AB8[i]);

    D_80082748 = 0;
    D_8008274C = 28;
    D_80082750 = 196;
    CompoDrawTextEn(483, text, &D_80092AB8[i]);

    if (sceneInfo.dbgmsg != FALSE) {
        if (sceneInfo.clicksound != FALSE) {
            CompoBox(4, 71, 313, 120, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 2, &D_80092AB8[i]);
        } else {
            CompoBox(4, 71, 313, 120, 0x0f0f0f, 2, &D_80092AB8[i]);
        }
    }
}

void CompoDrawLesson(register s32 lesson, register s32 idx) {
    switch (lesson) {
        case 1: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062D78, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        case 2: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062D8C, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        case 3: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062DA0, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        case 4: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062DB4, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        case 5: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062DC8, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        case 6: {
            CompoFastSpr(101, 107, &D_80062D64, 0, FALSE, 2, &D_80092AB8[idx]);
            CompoFastSpr(207, 107, &D_80062DDC, 0, FALSE, 2, &D_80092AB8[idx]);
            break;
        }
        default: {
            break;
        }
    }
}

void CompoDrawHand(register BOOL startover, register s32 dir, register s32 i) {
    static s32 timer = 0;

    if (startover != FALSE) {
        timer = 20;
    }

    if (timer > 0) {
        if (dir != PR_DIR_DOWN) {
            if ((timer % 5) == 0) {
                CompoFastSpr(20, 148, &D_80062E04, 0, FALSE, 3, &D_80092AB8[i]);
            } else {
                CompoFastSpr(20, 154, &D_80062E04, 0, FALSE, 3, &D_80092AB8[i]);
            }
        } else {
            if ((timer % 5) == 0) {
                CompoFastSpr(20, 156, &D_80062DF0, 0, FALSE, 3, &D_80092AB8[i]);
            } else {
                CompoFastSpr(20, 150, &D_80062DF0, 0, FALSE, 3, &D_80092AB8[i]);
            }
        }
        timer--;
    }
}

void CompoDrawScore(register BOOL flash, register s32 level, register s32 flashlvl, register s32 score, register s32 lesson, register s32 idx) {
    s32 i;
    s32 ch;
    BOOL gray[4];
    char tmp[16];
    static s32 scorecount = 0;

    CompoDrawScoreSprite(20, 182, &D_80062D28, FALSE, &D_80092AB8[idx]);
    sprintf(tmp, "%d", score);

    for (i = 0; tmp[i] != '\0'; i++) {
        ch = tmp[i] - '0';
        CompoFastSpr(i * 9 + 58, 182, &D_80062D3C, ch, FALSE, 3, &D_80092AB8[idx]);
    }

    // U rappin'
    CompoDrawScoreSprite(192, 143 + level * 13, &D_80062C60, FALSE, &D_80092AB8[idx]);

    gray[LEVEL_HIGH] = TRUE;
    gray[LEVEL_NORM] = TRUE;
    gray[LEVEL_LOW1] = TRUE;
    gray[LEVEL_LOW2] = TRUE;
    gray[level] = FALSE;
    if (flash != FALSE) {
        if ((scorecount % 2) != 0) {
            gray[flashlvl] = FALSE;
        } else {
            gray[flashlvl] = TRUE;
        }
    }

    CompoDrawScoreSprite(256, 143, &D_80062C74, gray[LEVEL_HIGH], &D_80092AB8[idx]);
    CompoDrawScoreSprite(255, 156, &D_80062C9C, gray[LEVEL_NORM], &D_80092AB8[idx]);
    CompoDrawScoreSprite(260, 169, &D_80062CC4, gray[LEVEL_LOW1], &D_80092AB8[idx]);
    CompoDrawScoreSprite(251, 182, &D_80062CEC, gray[LEVEL_LOW2], &D_80092AB8[idx]);
    scorecount++;
}


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


extern s32 D_80092C44[36];
extern s32 D_80092CD4[36 * 3];
extern GsSPRITE D_80092E84;
extern LINE_BUTTON_SCALING D_80092EA8[36];
extern LINE_BUTTON_SCALING D_800930E8[36 + 64];

extern COMPO_SPRITE *D_8008277C;

void func_8002471c(LINE_BUTTON_DRAW_INFO *info, COMPO_SPRITE *spr, GsOT *ot) {
    D_80092E84.x = info->x - (320 / 2);
    D_80092E84.y = info->y - (240 / 2);
    CompoSetSprScale(&D_80092E84, spr, D_80092C44[info->scaleid], D_80092CD4[info->scaleid]);
    GsSortSprite(&D_80092E84, ot, 1);
}

void CompoLineButtonApplyScalingTeacher(s32 num) {
    s32 i;

    for (i = 0; i < num; i++) {
        if (D_80092EA8[i].stage >= 24) {
            continue;
        }

        if (D_80092EA8[i].stage < 6) {
            D_80092C44[i] = ONE + D_80092EA8[i].newscale;
            D_80092CD4[i] = ONE + D_80092EA8[i].newscale;
            D_80092EA8[i].newscale += D_80092EA8[i].linscale;
            if (D_80092EA8[i].newscale >= ONE) {
                D_80092EA8[i].linscale = -(ONE / 4);
            }
        } else if (D_80092EA8[i].stage < 22) {
            if (D_80092EA8[i].angle > (ONE * 2)) {
                D_80092EA8[i].angle = 0;
            }
            D_80092C44[i] = (rsin(D_80092EA8[i].angle) * ONE) >> 12;
            D_80092CD4[i] = ONE;
            D_80092EA8[i].angle += (ONE / 16);
        } else {
            D_80092C44[i] = ONE;
            D_80092CD4[i] = ONE;
        }
        D_80092EA8[i].stage++;
    }
}

void CompoLineButtonApplyScalingPara(s32 num) {
    s32 i;

    for (i = 0; i < num; i++) {
        if (D_800930E8[i].stage >= 24) {
            continue;
        }

        if (D_800930E8[i].stage < 5) {
            D_800930E8[i].newscale += D_800930E8[i].linscale;
            D_80092C44[i] = ONE + D_800930E8[i].newscale;
            D_80092CD4[i] = ONE + D_800930E8[i].newscale;
            if (D_800930E8[i].newscale >= ONE) {
                D_800930E8[i].linscale = -(ONE / 4);
            }
        } else if (D_800930E8[i].stage < 22) {
            if (D_800930E8[i].angle > (ONE * 2)) {
                D_800930E8[i].angle = 0;
            }
            D_80092C44[i] = (rsin(D_800930E8[i].angle) * ONE) >> 12;
            D_80092CD4[i] = ONE;
            D_800930E8[i].angle += (ONE / 16);
        } else {
            D_80092C44[i] = ONE;
            D_80092CD4[i] = ONE;
        }
        D_800930E8[i].stage++;
    }
}

void CompoLineButtonInitScalingTeacher(void) {
    s32 i;

    for (i = 0; i < 36; i++) {
        D_80092EA8[i].angle = 0;
        D_80092EA8[i].newscale = ONE / 2;
        D_80092EA8[i].linscale = ONE / 2;
        D_80092EA8[i].stage = 0;
        D_80092C44[i] = ONE;
        D_80092CD4[i] = ONE;
    }
}

void CompoLineButtonInitScalingPara(void) {
    s32 i;

    for (i = 0; i < 36; i++) {
        D_800930E8[i].angle = 0;
        D_800930E8[i].newscale = ONE / 2;
        D_800930E8[i].linscale = ONE / 2;
        D_800930E8[i].stage = 0;
        D_80092C44[i] = ONE;
        D_80092CD4[i] = ONE;
    }
}

void CompoResetLineButton(void) {
    D_80092E84.attribute = D_80062B84.attr;
    D_80092E84.rotate = 0;
}

void CompoDrawLineButton(register LINE_BUTTON_DRAW_INFO *info, register s32 idx) {
    D_80092E84.x = info->x - (320 / 2);
    D_80092E84.y = info->y - (240 / 2);
    CompoSetSprScale(&D_80092E84, D_800676B0[info->keyid], D_80092C44[info->scaleid], D_80092CD4[info->scaleid]);
    GsSortSprite(&D_80092E84, &D_80092AB8[idx], 1);
}

void CompoDrawLineDots(register s32 line, register s32 dot, register s32 idx) {
    s32 i;
    u32 sp24;
    s32 sp28;

    if (dot > 0) {
        sp28 = dot * 15 + 31;
    } else {
        sp28 = 0;
    }

    for (i = 0; i < 4; i++) {
        if (sp28 < D_800676DC[i]) {
            CompoFastSpr(D_800676DC[i], D_800676D4[line], &D_80062C24, 0, FALSE, 3, &D_80092AB8[idx]);
        } else {
            CompoFastSpr(D_800676DC[i], D_800676D4[line], &D_80062C38, 0, FALSE, 3, &D_80092AB8[idx]);
        }
    }
    if (line > 0) {
        i = 2;
    } else {
        i = 0;
    }
    for (i; i < 14; i++) {
        if (sp28 < D_800676F4[i]) {
            CompoFastSpr(D_800676F4[i], D_800676EC[line], &D_80062BFC, 0, FALSE, 3, &D_80092AB8[idx]);
        } else {
            CompoFastSpr(D_800676F4[i], D_800676EC[line], &D_80062C10, 0, FALSE, 3, &D_80092AB8[idx]);
        }
    }
}

void CompoDrawTeacherIcon(register s32 arg0, register s32 arg1, register s32 arg2, register s32 arg3, register s32 arg4, register s32 i) {
    static s32 D_80082408 = 0;
    static s32 D_8008240C = 0;

    if (D_8008240C != arg4) {
        D_8008240C = arg4;
        D_80082408 = 0;
    } else {
        D_80082408++;
    }

    if (D_80082408 > arg3) {
        D_80082408 = arg3;
    }

    if (arg4 > 0) {
        CompoFastSpr(arg1 + arg4 * 15 + D_80082408 * 4, arg2 + arg0 * 20, D_8008277C, 0, FALSE, 0, &D_80092AB8[i]);
    }
}

void CompoDrawParaIcon(register s32 linei, register s32 dot, register s32 i) {
    static s32 D_80082410 = 0;
    static s32 D_80082414 = 0;

    if (D_80082414 != dot) {
        D_80082414 = dot;
        D_80082410 = 1;
    } else {
        D_80082410++;
    }

    if (D_80082410 > 4) {
        D_80082410 = 4;
    }

    if (dot >= 0) {
        CompoFastSpr(26 + dot * 15 + D_80082410 * 4, 16 + linei * 20, &D_80062ABC, 0, FALSE, 0, &D_80092AB8[i]);
    }
}

void CompoSetTurnIcon(register s32 turntype) {
    switch (turntype) {
        case COMPO_TURN_ICON_1: {
            D_8008277C = &D_80062AA8;
            break;
        }
        case COMPO_TURN_ICON_2: {
            D_8008277C = &D_80062A94;
            break;
        }
        case COMPO_TURN_ICON_3: {
            D_8008277C = &D_80062A6C;
            break;
        }
        case COMPO_TURN_ICON_5: {
            D_8008277C = &D_80062A58;
            break;
        }
        case COMPO_TURN_ICON_7: {
            D_8008277C = &D_80062A80;
            break;
        }
        case COMPO_TURN_ICON_4: {
            D_8008277C = &D_80062AD0;
            break;
        }
        case COMPO_TURN_ICON_0:
        case COMPO_TURN_ICON_8:
        default: {
            D_8008277C = &D_80062ABC;
            break;
        }
    }
}

void CompoDrawLine(SCENE_INFO *scn, s32 idx) {
    s32 i;
    s32 j;
    u32 unused[2];
    LINE_BUTTON_DRAW_INFO info;

    for (i = 0; i < scn->teacherline.num; i++) {
        if (scn->teacherline.dot[i] >= 0) {
            CompoLineButtonApplyScalingTeacher(scn->teacherline.dot[i] + i * 18);
            if (scn->drawbuttons == TRUE) {
                CompoDrawTeacherIcon(i, 26, 18, 4, scn->teacherline.dot[i], idx);
            }
        }
        if (scn->parappaline.dot[i] >= 0) {
            CompoLineButtonApplyScalingPara(scn->parappaline.dot[i] + i * 18);
            if (scn->drawbuttons == TRUE) {
                CompoDrawParaIcon(i, scn->parappaline.dot[i], idx);
            }
        }
        if (scn->drawbuttons == TRUE) {
            CompoDrawLineDots(i, scn->teacherline.dot[i], idx);
        }

        for (j = 0; j < 18; j++) {
            info.x = 32 + j * 15;
            info.y = 24 + i * 20;
            info.scaleid = i * 17 + j;
            info.keyid = scn->teacherline.keyid[i][j];
            if ((scn->drawbuttons == TRUE) && (info.keyid > PR_TAP_NONE) && (info.keyid < PR_TAP_NUM)) {
                CompoDrawLineButton(&info, idx);
            }
        }
    }
}


static s32 D_80082418 = 0;
static s32 D_8008241C = 0;

extern SVECTOR D_80093728[1024];
extern PARA_DAT_DATA D_80095728;
extern PARA_DAT_OBJECT D_80095738[128];

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

void CompoApplyDat(register PARA_DAT_DATA *data, register s32 frame, register s32 idx) {
    s32 i;

    if (frame < 0) {
        frame += data->framenum;
    }
    MimeDatSetFrame(data, frame);

    for (i = 0; i < data->objnum; i++) {
        if (data->obj[i].num != 0) {
            actionMimeList[idx][i] = MimeDatGetKeyFrame(data, i);
        }
    }
    ActionVtxMime(idx);
}

void CompoApplyDatForVdf(register PARA_DAT_DATA *data, register s32 frame, register s32 idx, register s32 vdf) {
    s32 i;

    if (frame < 0) {
        frame += data->framenum;
    }
    MimeDatSetFrame(data, frame);

    for (i = 0; i < data->objnum; i++) {
        if (data->obj[i].num != 0) {
            actionMimeList[vdf][i] = MimeDatGetKeyFrame(data, i);
        }
    }
    ActionSetVdfForTmd(idx, vdf);
}

void CompoSetModelAnim(s32 i, void *dat, void *vdf, PARA_DAT_DATA *data, PARA_DAT_OBJECT *obj, s32 *arg5) {
    *arg5 = 0;
    ActionSetVdfData(i, vdf);
    MimeDatInit(data, obj, dat, FALSE);
    CompoResetMimeSpecific(i);
}

void CompoSetMainModelAnim(void *dat, void *vdf, s32 frame, s32 framenum) {
    D_80082418 = frame;
    D_8008241C = (frame + framenum) - 1;
    ActionSetVdfData(0, vdf);
    MimeDatInit(&D_80095728, D_80095738, dat, FALSE);
    CompoResetMimeSpecific(0);
}

void CompoInitMainModelSet(void *dat, void *vdf, void *tmd, s32 wrap) {
    D_80082418 = 0;
    D_8008241C = (0 + 1000) - 1;
    ActionInitModel(0, tmd, vdf, D_80093728);
    MimeDatInit(&D_80095728, D_80095738, dat, wrap);
}

s32 CompoApplyMainDat(void) {
    if (D_80082418 < D_8008241C) {
        CompoApplyDat(&D_80095728, D_80082418++, 0);
        return D_80082418;
    } else {
        return 0;
    }
}

void CompoResetFirstTmdVdf(void) {
    ActionResetMimeVdf(0);
}


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


static COMPO_SPRITE *D_80082420[2] = {
    &D_80062F58,
    &D_80062F44,
};
static COMPO_SPRITE *D_80082428[2] = {
    &D_80062F6C,
    &D_80062F80,
};
static COMPO_SPRITE *D_80082430[2] = {
    &D_80062EF4,
    &D_80062F08,
};
static COMPO_SPRITE *D_80082438[2] = {
    &D_80062F1C,
    &D_80062F30,
};
static COMPO_SPRITE *D_80082440[2] = {
    &D_80062EA4,
    &D_80062EB8,
};
static COMPO_SPRITE *D_80082448[2] = {
    &D_80062ECC,
    &D_80062EE0,
};

void CompoDrawTryAgainText(register s32 sel) {
    CompoFastSpr(56, 57, &D_80062E18, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    if (sel == 1) {
        CompoFastSpr(70, 149, &D_80062E54, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(178, 152, &D_80062E40, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else if (sel == 0) {
        CompoFastSpr(70, 149, &D_80062E68, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(178, 152, &D_80062E2C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(70, 149, &D_80062E54, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(178, 152, &D_80062E2C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawTitleScreen(register s32 sel) {
    u32 unused;
    s32 sp24;
    u32 unused1[10];
    static s32 D_80082450 = 0;

    if (D_80082450 > ONE) {
        D_80082450 = 0;
    }
    sp24 = (rsin(D_80082450) * ONE) >> 12;
    D_80082450 += (ONE / 32);

    CompoFastSpr(278, 78, &D_80064380, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(10, 214, &D_80064344, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(178, 214, &D_8006436C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    if (sel != 0) {
        CompoFastSpr(24, 139, &D_800643BC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(231, 139, &D_800643A8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(24, 139, &D_800643D0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(231, 139, &D_80064394, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

extern s32 D_80095B38[];
extern s32 D_80095B58[];

void CompoDrawStageSelect(register void *data) {
    struct MENU_STAGE_DEFDATA *p;
    s32 i;
    s32 sp28[6];
    s32 sp40[6];
    s32 sp58[6];

    p = (struct MENU_STAGE_DEFDATA *)data;
    for (i = 0; i < 6; i++) {
        D_80095B58[i] = p->icons[i + 1];
        sp28[i] = 0;
        sp58[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        D_80095B38[i] = 0;
    }

    CompoFastSpr(32, 33, &D_80063368, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    for (i = 0; i < 6; i++) {
        switch (D_80095B58[i]) {
            case MENU_STAGE_ICON_0: {
                D_80095B38[i] = 0;
                sp40[i] = 2;
                sp58[i] = 1;
                break;
            }
            case MENU_STAGE_ICON_1: {
                D_80095B38[i] = 1;
                sp40[i] = 0;
                sp58[i] = 0;
                break;
            }
            case MENU_STAGE_ICON_2: {
                D_80095B38[i] = 1;
                sp40[i] = 0;
                sp58[i] = 0;
                break;
            }
            case MENU_STAGE_ICON_3: {
                D_80095B38[i] = 1;
                sp40[i] = 0;
                sp58[i] = 0;
                break;
            }
        }
    }
    if (p->selcur == 7) {
        if (p->selected != FALSE) {
            CompoFastSpr(63, 100, &D_800634E4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
            CompoFastSpr(66, 103, &D_800634F8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(63, 98, &D_800634D0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
            CompoFastSpr(66, 103, &D_800634F8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
    } else {
        if (p->icons[7] != 0) {
            CompoFastSpr(63, 98, &D_8006350C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
            CompoFastSpr(66, 103, &D_800634F8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
        if (p->selcur < 7) {
            sp28[p->selcur + 5] = 1;
            D_80095B38[p->selcur - 1] = 2;
            sp28[p->selcur - 1] = 1;
        }
    }

    CompoFastSpr(43, 123, D_80067738[D_80095B38[0]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(89, 123, &D_800635C0, 46, 8, sp40[0], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(136, 106, D_80067738[D_80095B38[1]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(182, 106, &D_800635C0, 55, 8, sp40[1], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(224, 89, D_80067738[D_80095B38[2]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(270, 89, &D_800635C0, 64, 8, sp40[2], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(43, 200, D_80067738[D_80095B38[3]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(89, 200, &D_800635C0, 73, 8, sp40[3], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(136, 182, D_80067738[D_80095B38[4]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(182, 182, &D_800635C0, 82, 8, sp40[4], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(224, 165, D_80067738[D_80095B38[5]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(270, 165, &D_800635C0, 91, 8, sp40[5], 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 67, D_8006772C[D_80095B38[0]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(130, 50, D_8006772C[D_80095B38[1]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(218, 33, D_8006772C[D_80095B38[2]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 144, D_8006772C[D_80095B38[3]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(130, 126, D_8006772C[D_80095B38[4]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(218, 109, D_8006772C[D_80095B38[5]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(45, 76, D_80067770[sp58[0]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(138, 59, D_80067778[sp58[1]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(226, 42, D_80067780[sp58[2]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(45, 153, D_80067788[sp58[3]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(138, 135, D_80067790[sp58[4]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(226, 118, D_80067798[sp58[5]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(78, 65, D_80067750[D_80095B58[0]][sp28[0]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(171, 48, D_80067750[D_80095B58[1]][sp28[1]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(259, 31, D_80067750[D_80095B58[2]][sp28[2]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(78, 142, D_80067750[D_80095B58[3]][sp28[3]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(171, 124, D_80067750[D_80095B58[4]][sp28[4]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(259, 107, D_80067750[D_80095B58[5]][sp28[5]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(78, 65, D_80067744[D_80095B38[0]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(171, 48, D_80067744[D_80095B38[1]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(259, 31, D_80067744[D_80095B38[2]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(78, 142, D_80067744[D_80095B38[3]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(171, 124, D_80067744[D_80095B38[4]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(259, 107, D_80067744[D_80095B38[5]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    
    if (p->selcur == 8) {
        if (p->highlight != FALSE) {
            CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
        if (p->selected == FALSE) {
            CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
    } else {
        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 187, D_80082428[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawSavingText(s32 arg0) {
    s32 x;
    s32 y;

    x = (s16)D_80082754;
    y = (s16)D_80082758;

    CompoSetTextBox(90, 122, 272, 480);
    CompoSetFontJp(832, 256, 20, 21);
    CompoDrawTextJp(D_800677A0, &D_80092AB8[D_800827D4]);
    CompoSetTextBox(x, y, 272, 480);

    CompoBox(82, 123, 8, 9, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 0, &D_80092AB8[D_800827D4]);
    CompoBox(90, 115, 140, 25, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 0, &D_80092AB8[D_800827D4]);
    CompoBox(230, 123, 8, 9, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(82, 115, &D_80062B20, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(82, 132, &D_80062B0C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(230, 115, &D_80062AF8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(230, 132, &D_80062AE4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
}

static COMPO_SPRITE *D_80082454[2] = {
    &D_80063B9C,
    &D_80063BB0,
};
static COMPO_SPRITE *D_8008245C[2] = {
    &D_80062FA8,
    &D_80062FBC,
};
static COMPO_SPRITE *D_80082464[2] = {
    &D_800630C0,
    &D_800630D4,
};
static COMPO_SPRITE *D_8008246C[2] = {
    &D_8006314C,
    &D_80063160,
};
static COMPO_SPRITE *D_80082474[2] = {
    &D_800631EC,
    &D_80063200,
};
static COMPO_SPRITE *D_8008247C[2] = {
    &D_8006328C,
    &D_800632A0,
};


extern BOOL D_80095B70[57];

void CompoDrawNameEnter(register void *data) {
    struct CARD_MENU_NAMEENTER_INFO *p;
    s32 i;

    p = (struct CARD_MENU_NAMEENTER_INFO *)data;
    for (i = 0; i < 57; i++) {
        D_80095B70[i] = FALSE;
    }
    D_80095B70[p->letterselcur] = TRUE;
    if (p->base.saving != FALSE) {
        CompoDrawSavingText(0);
    }
    D_80082748 = 0;
    D_8008274C = 117;
    D_80082750 = 62;
    CompoDrawTextSaveTitle(p->name, 1, &D_80092AB8[D_800827D4]);
    CompoFastSpr(36, 28, &D_80063B88, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
#define NAMEENTERHELPER(n) 0, D_80095B70[n], 0, &D_80092AB8[D_800827D4]
    CompoSimpleFastSpr(31 + 18 * 0, 99, &D_800637DC, NAMEENTERHELPER(0));
    CompoSimpleFastSpr(31 + 18 * 1, 99, &D_80063854, NAMEENTERHELPER(1));
    CompoSimpleFastSpr(31 + 18 * 2, 99, &D_8006387C, NAMEENTERHELPER(2));
    CompoSimpleFastSpr(31 + 18 * 3, 99, &D_800638B8, NAMEENTERHELPER(3));
    CompoSimpleFastSpr(31 + 18 * 4, 99, &D_800638E0, NAMEENTERHELPER(4));
    CompoSimpleFastSpr(31 + 18 * 5, 99, &D_8006391C, NAMEENTERHELPER(5));
    CompoSimpleFastSpr(31 + 18 * 6, 99, &D_80063944, NAMEENTERHELPER(6));
    CompoSimpleFastSpr(31 + 18 * 7, 99, &D_80063958, NAMEENTERHELPER(7));
    CompoSimpleFastSpr(31 + 18 * 8, 99, &D_8006396C, NAMEENTERHELPER(8));
    CompoSimpleFastSpr(31 + 18 * 9, 99, &D_80063980, NAMEENTERHELPER(9));
    CompoSimpleFastSpr(31 + 18 * 10, 99, &D_80063994, NAMEENTERHELPER(10));
    CompoSimpleFastSpr(31 + 18 * 11, 99, &D_800639A8, NAMEENTERHELPER(11));
    CompoSimpleFastSpr(31 + 18 * 12, 99, &D_800639BC, NAMEENTERHELPER(12));
    CompoSimpleFastSpr(31 + 18 * 13, 99, &D_80063A0C, NAMEENTERHELPER(13));
    CompoSimpleFastSpr(31 + 18 * 0, 99 + 20 * 1, &D_80063A20, NAMEENTERHELPER(14));
    CompoSimpleFastSpr(31 + 18 * 1, 99 + 20 * 1, &D_80063A34, NAMEENTERHELPER(15));
    CompoSimpleFastSpr(31 + 18 * 2, 99 + 20 * 1, &D_80063A84, NAMEENTERHELPER(16));
    CompoSimpleFastSpr(31 + 18 * 3, 99 + 20 * 1, &D_80063AAC, NAMEENTERHELPER(17));
    CompoSimpleFastSpr(31 + 18 * 4, 99 + 20 * 1, &D_80063AC0, NAMEENTERHELPER(18));
    CompoSimpleFastSpr(31 + 18 * 5, 99 + 20 * 1, &D_80063AE8, NAMEENTERHELPER(19));
    CompoSimpleFastSpr(31 + 18 * 6, 99 + 20 * 1, &D_80063AFC, NAMEENTERHELPER(20));
    CompoSimpleFastSpr(31 + 18 * 7, 99 + 20 * 1, &D_80063B10, NAMEENTERHELPER(21));
    CompoSimpleFastSpr(31 + 18 * 8, 99 + 20 * 1, &D_80063B24, NAMEENTERHELPER(22));
    CompoSimpleFastSpr(31 + 18 * 9, 99 + 20 * 1, &D_80063B38, NAMEENTERHELPER(23));
    CompoSimpleFastSpr(31 + 18 * 10, 99 + 20 * 1, &D_80063B4C, NAMEENTERHELPER(24));
    CompoSimpleFastSpr(31 + 18 * 11, 99 + 20 * 1, &D_80063B74, NAMEENTERHELPER(25));
    CompoSimpleFastSpr(31 + 18 * 12, 99 + 20 * 1 - 1, &D_80063728, NAMEENTERHELPER(26));
    CompoSimpleFastSpr(31 + 18 * 13, 99 + 20 * 1 - 1, &D_8006373C, NAMEENTERHELPER(27));
    CompoSimpleFastSpr(31 + 18 * 0, 99 + 20 * 2, &D_80063750, NAMEENTERHELPER(28));
    CompoSimpleFastSpr(31 + 18 * 1, 99 + 20 * 2, &D_80063764, NAMEENTERHELPER(29));
    CompoSimpleFastSpr(31 + 18 * 2, 99 + 20 * 2, &D_80063778, NAMEENTERHELPER(30));
    CompoSimpleFastSpr(31 + 18 * 3, 99 + 20 * 2, &D_8006378C, NAMEENTERHELPER(31));
    CompoSimpleFastSpr(31 + 18 * 4, 99 + 20 * 2, &D_800637A0, NAMEENTERHELPER(32));
    CompoSimpleFastSpr(31 + 18 * 5, 99 + 20 * 2, &D_800637B4, NAMEENTERHELPER(33));
    CompoSimpleFastSpr(31 + 18 * 6, 99 + 20 * 2, &D_800637C8, NAMEENTERHELPER(34));
    CompoSimpleFastSpr(31 + 18 * 7, 99 + 20 * 2, &D_80063714, NAMEENTERHELPER(35));
    CompoSimpleFastSpr(31 + 18 * 8, 99 + 20 * 2, &D_800639E4, NAMEENTERHELPER(36));
    CompoSimpleFastSpr(31 + 18 * 9, 99 + 20 * 2, &D_800638F4, NAMEENTERHELPER(37));
    CompoSimpleFastSpr(31 + 18 * 10, 99 + 20 * 2, &D_80063908, NAMEENTERHELPER(38));
    CompoSimpleFastSpr(31 + 18 * 11, 99 + 20 * 2, &D_80063840, NAMEENTERHELPER(39));
    CompoSimpleFastSpr(31 + 18 * 12, 99 + 20 * 2, &D_80063AD4, NAMEENTERHELPER(40));
    CompoSimpleFastSpr(31 + 18 * 13, 99 + 20 * 2, &D_800638CC, NAMEENTERHELPER(41));
    CompoSimpleFastSpr(31 + 18 * 0, 99 + 20 * 3, &D_80063A70, NAMEENTERHELPER(42));
    CompoSimpleFastSpr(31 + 18 * 1, 99 + 20 * 3, &D_80063890, NAMEENTERHELPER(43));
    CompoSimpleFastSpr(31 + 18 * 2, 99 + 20 * 3, &D_80063818, NAMEENTERHELPER(44));
    CompoSimpleFastSpr(31 + 18 * 3, 99 + 20 * 3, &D_8006382C, NAMEENTERHELPER(45));
    CompoSimpleFastSpr(31 + 18 * 4, 99 + 20 * 3, &D_80063930, NAMEENTERHELPER(46));
    CompoSimpleFastSpr(31 + 18 * 5, 99 + 20 * 3, &D_80063868, NAMEENTERHELPER(47));
    CompoSimpleFastSpr(31 + 18 * 6, 99 + 20 * 3, &D_80063700, NAMEENTERHELPER(48));
    CompoSimpleFastSpr(31 + 18 * 0, 99 + 20 * 4, &D_80063A5C, NAMEENTERHELPER(49));
    CompoSimpleFastSpr(31 + 18 * 1, 99 + 20 * 4, &D_800638A4, NAMEENTERHELPER(50));
    CompoSimpleFastSpr(31 + 18 * 2, 99 + 20 * 4, &D_80063A48, NAMEENTERHELPER(51));
    CompoSimpleFastSpr(31 + 18 * 3, 99 + 20 * 4, &D_80063A98, NAMEENTERHELPER(52));
    CompoSimpleFastSpr(31 + 18 * 4, 99 + 20 * 4, &D_800639F8, NAMEENTERHELPER(53));
    CompoSimpleFastSpr(31 + 18 * 5, 99 + 20 * 4, &D_800639D0, NAMEENTERHELPER(54));
    CompoSimpleFastSpr(188, 183, &D_80063B60, NAMEENTERHELPER(55));

    if (p->endbutton == 1) {
        CompoFastSpr(225, 160, D_80082454[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 168, D_800677B8[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else if (p->endbutton == 2) {
        CompoFastSpr(225, 160, D_80082454[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 168, D_800677B8[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else if (D_80095B70[56] != FALSE) {
        if (p->base.highlight != FALSE) {
            CompoFastSpr(225, 160, D_80082454[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(225, 160, D_80082454[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
        CompoFastSpr(234, 168, D_800677B8[3], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(225, 160, D_80082454[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 168, D_800677B8[2], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawSlotSelect(register s32 draw, register struct CARD_MENU_SLOTSELECT_INFO *data) {
    s32 sp20[16];
    s32 i;
    s32 j;
    s32 fsel;
    s32 sp6C;
    char *sp70;

    if (data->base.saving != FALSE) {
        CompoDrawSavingText(0);
    }

    if (draw == PR_MENU_SAVE) {
        CompoFastSpr(37, 34, &D_80063624, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(138, 39, &D_800636B0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(114, 30, &D_8006369C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else if (draw == PR_MENU_LOAD) {
        CompoFastSpr(37, 34, &D_800635FC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(138, 39, &D_80063674, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(114, 30, &D_8006369C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(37, 34, &D_80063610, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(141, 39, &D_80063688, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(123, 30, &D_8006369C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
    for (i = 0, fsel = 0; ((i < data->height) && (fsel < data->availableslotnum)); i++) {
        for (j = 0; ((j < data->width) && (fsel < data->availableslotnum)); j++, fsel++) {
            if (fsel == data->selcur) {
                sp20[fsel] = 1;
            } else if (data->available[fsel] == FALSE) {
                sp20[fsel] = 2;
            } else {
                sp20[fsel] = 0;
            }

            sp70 = data->name[fsel];
            D_80082748 = 0;
            D_8008274C = 55 + j * 74;
            D_80082750 = 66 + i * 21;
            if (sp70 != NULL) {
                if (sp20[fsel] == 1) {
                    CompoDrawTextSaveTitle(sp70, 1, &D_80092AB8[D_800827D4]);
                } else {
                    CompoDrawTextSaveTitle(sp70, 0, &D_80092AB8[D_800827D4]);
                }
            }
            CompoFastSpr(51 + j * 74, 63 + i * 21, D_800677C8[sp20[fsel]], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
    }

    if (data->selcur == (data->height * data->width)) {
        if (data->base.highlight != FALSE) {
            CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
        if (data->base.sel == 0) {
            CompoFastSpr(234, 187, &D_80062F94, 0, 0, 1, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(234, 187, D_80082428[0], 0, 0, 1, &D_80092AB8[D_800827D4]);
        }
    } else {
        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 187, D_80082428[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawHighScore(register struct MENU_HIGHSCORE_DEFDATA *data) {
    s32 sel;
    s32 i;
    s32 j;
    s32 ij;
    char *text;

    ij = 0;
    for (i = 0; i < data->unkC; i++) {
        for (j = 0; j < data->unkE; j++, ij++) {
            if (data->unk14[ij][0] == '\0') {
                text = NULL;
                sel = 2;
            } else {
                text = data->unk14[ij];
                sel = 0;
            }
            D_80082748 = 0;
            D_8008274C = 57 + j * 79;
            D_80082750 = 70 + i * 18;
            if (text != NULL) {
                CompoDrawTextSaveTitle(text, 0, &D_80092AB8[D_800827D4]);
            }
            CompoFastSpr(55 + j * 79, 68 + i * 18, D_800677C8[3 + sel], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
        }
    }

    CompoFastSpr(37, 31, &D_80063D90, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(84, 56, &D_80063DA4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(163, 56, &D_80063DB8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(242, 56, &D_80063DCC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 69, &D_80063E1C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 87, &D_80063E30, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 105, &D_80063E44, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 123, &D_80063E58, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 141, &D_80063E6C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(37, 159, &D_80063E80, 0, FALSE, 0, &D_80092AB8[D_800827D4]);

    if (data->unk0 == 1) {
        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
    if (data->unk4 == 0) {
        CompoFastSpr(234, 187, &D_80062F94, 0, 0, 1, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawMainMenu(register struct MENU_MAIN_DEFDATA *data) {
    s16 *val;
    s32 i;

    if (data == NULL) {
        CompoFastSpr(32, 36, D_8008245C[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(40, 46, D_800677E0[2], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(119, 71, D_8008247C[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(127, 79, D_800677E0[14], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(211, 37, D_80082464[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(221, 47, D_800677E0[5], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(38, 110, D_8008246C[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(47, 119, D_800677E0[8], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(161, 110, D_80082474[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(160, 123, D_800677E0[17], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        CompoFastSpr(234, 187, D_80082428[1], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
        return;
    }

    for (i = 0; i < 5; i++) {
        val = data->selval[i];
        switch (i) {
            case 0: {
                if (data->selcur == i) {
                    if (val[0] == 0) {
                        CompoFastSpr(40, 46, D_800677E0[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(40, 46, D_800677E0[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                    if (data->highlight == TRUE) {
                        CompoFastSpr(32, 36, D_8008245C[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(32, 36, D_8008245C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                } else {
                    CompoFastSpr(32, 36, D_8008245C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(40, 46, D_800677E0[2], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                }
                break;
            }
            case 1: {
                if (data->selcur == i) {
                    if (val[0] == -1) {
                        CompoFastSpr(127, 79, D_800677E0[13], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(127, 79, D_800677E0[12], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                    if (data->highlight == TRUE) {
                        CompoFastSpr(119, 71, D_8008247C[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(119, 71, D_8008247C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                } else {
                    CompoFastSpr(119, 71, D_8008247C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(127, 79, D_800677E0[14], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                }
                break;
            }
            case 2: {
                if (data->selcur == i) {
                    if (val[0] == 0) {
                        CompoFastSpr(221, 47, D_800677E0[4], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(221, 47, D_800677E0[3], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                    if (data->highlight == TRUE) {
                        CompoFastSpr(211, 37, D_80082464[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(211, 37, D_80082464[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                } else {
                    CompoFastSpr(211, 37, D_80082464[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(221, 47, D_800677E0[5], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                }
                break;
            }
            case 3: {
                if (data->selcur == i) {
                    switch (val[0]) {
                        case 0: {
                            CompoFastSpr(47, 119, D_800677E0[10], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            CompoFastSpr(160, 123, D_800677E0[20], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            break;
                        }
                        case 1: {
                            CompoFastSpr(47, 119, D_800677E0[9], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            CompoFastSpr(160, 123, D_800677E0[20], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            break;
                        }
                        case 2: {
                            CompoFastSpr(47, 119, D_800677E0[11], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            CompoFastSpr(160, 123, D_800677E0[19], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            break;
                        }
                        case 3: {
                            CompoFastSpr(47, 119, D_800677E0[11], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            CompoFastSpr(160, 123, D_800677E0[18], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            break;
                        }
                        default: {
                            CompoFastSpr(47, 119, D_800677E0[11], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            CompoFastSpr(160, 123, D_800677E0[20], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                            break;
                        }
                    }
                    if (data->highlight == TRUE) {
                        CompoFastSpr(38, 110, D_8008246C[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(38, 110, D_8008246C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                    if (data->highlight == TRUE) {
                        CompoFastSpr(161, 110, D_80082474[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(161, 110, D_80082474[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                } else {
                    CompoFastSpr(38, 110, D_8008246C[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(47, 119, D_800677E0[8], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(161, 110, D_80082474[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(160, 123, D_800677E0[17], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                }
                break;
            }
            case 4: {
                if (data->selcur == i) {
                    if (data->highlight == TRUE) {
                        CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                    if (data->selexit == FALSE) {
                        CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    } else {
                        CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    }
                } else {
                    CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                    CompoFastSpr(234, 187, D_80082428[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                }
                break;
            }
            default: {
                break;
            }
        }
    }
}

void CompoDrawSaveDialog(register s32 draw, register struct CARD_MENU_DIALOG_INFO *data) {
    switch (draw) {
        case 1: {
            // "Memory card not inserted" (Card not inserted dialog)
            CompoFastSpr(36, 54, &D_80063C50, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            CompoFastSpr(157, 62, &D_80063C8C, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            if (data->base.highlight == TRUE) {
                CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            if (data->base.sel == 0) {
                CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            break;
        }
        case 2: {
            // "Memory card unformatted. OK to format?" (Format card dialog)
            CompoFastSpr(36, 54, &D_80063C64, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            CompoFastSpr(133, 55, &D_80063CA0, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            if (data->base.highlight == TRUE) {
                CompoFastSpr(224, 149, &D_80063CF0, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(224, 149, &D_80063D04, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            if (data->base.sel == 1) {
                CompoFastSpr(234, 159, &D_80063D68, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else if (data->base.sel == 2) {
                CompoFastSpr(234, 159, &D_80063D54, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(234, 159, &D_80063D7C, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            break;
        }
        case 3: {
            // "Memory card full" (Card full dialog)
            CompoFastSpr(36, 54, &D_80063C50, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            CompoFastSpr(157, 62, &D_80063CB4, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            if (data->base.highlight == TRUE) {
                CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            if (data->base.sel == 0) {
                CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            break;
        }
        case 4: {
            // "Save?" (Card save dialog)
            CompoFastSpr(28, 56, &D_80063C78, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            CompoFastSpr(170, 71, &D_80063CC8, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            if (data->base.highlight == TRUE) {
                CompoFastSpr(224, 149, &D_80063CF0, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(224, 149, &D_80063D04, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            if (data->base.sel == 1) {
                CompoFastSpr(234, 159, &D_80063D68, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else if (data->base.sel == 2) {
                CompoFastSpr(234, 159, &D_80063D54, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(234, 159, &D_80063D7C, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            break;
        }
        case 5: {
            // "Same name" (Card same name dialog)
            CompoFastSpr(36, 54, &D_80063C50, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            CompoFastSpr(145, 71, &D_80063CDC, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            if (data->base.highlight == TRUE) {
                CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            if (data->base.sel == 0) {
                CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            } else {
                CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 1, &D_80092AB8[D_800827D4]);
            }
            break;
        }
        default: {
            break;
        }
    }
    CompoFastSpr(121, 36, &D_80063C3C, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
}

void CompoDrawPracticeBar(register s32 arg0) {
    s32 i;
    s32 sp24;
    s32 sp28;

    if (arg0 > 0) {
        sp28 = arg0 * 15 + 31;
    } else {
        sp28 = 0;
    }

    for (i = 0; i < 4; i++) {
        if (sp28 < D_80067834[i]) {
            CompoFastSpr(D_80067834[i], 92, &D_80062C24, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(D_80067834[i], 92, &D_80062C38, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
        }
    }
    for (i = 0; i < 14; i++) {
        if (sp28 < D_80067844[i]) {
            CompoFastSpr(D_80067844[i], 96, &D_80062BFC, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
        } else {
            CompoFastSpr(D_80067844[i], 96, &D_80062C10, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
        }
    }
}

void CompoDrawPractice(register SCENE_INFO *scn) {
    u32 unused;
    s32 i;
    u32 unused1[2];
    LINE_BUTTON_DRAW_INFO info;

    static BOOL D_80082484 = TRUE;

    if (D_80082484 != FALSE) {
        CompoResetLineButton();
        D_80082484 = FALSE;
    }
    if ((scn->flags & PR_SCN_FLAG_400000) != 0) {
        switch (scn->practdraw) {
            case 1: {
                CompoFastSpr(121, 137, &D_80063EE4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FC0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(225, 134, &D_80063FE8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(266, 134, &D_80063FFC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_8006404C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(42, 180, &D_80064074, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 2: {
                CompoFastSpr(121, 137, &D_80063EF8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FC0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(225, 134, &D_80063FE8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(266, 134, &D_80063FFC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_8006404C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(42, 180, &D_80064074, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 3: {
                break;
            }
            case 0: {
                CompoFastSpr(123, 137, &D_80063F0C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_80064060, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 4: {
                CompoFastSpr(127, 146, &D_80063F20, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FD4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_80064038, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 5: {
                CompoFastSpr(115, 146, &D_80063F34, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FD4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_80064038, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 6: {
                CompoFastSpr(120, 146, &D_80063F48, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FD4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_80064038, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 7: {
                CompoFastSpr(107, 137, &D_80063F5C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            case 8: {
                CompoFastSpr(106, 137, &D_80063F70, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(240, 161, &D_80063FD4, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                CompoFastSpr(54, 196, &D_80064038, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
                break;
            }
            default: {
                break;
            }
        }
        CompoFastSpr(101, 122, &D_80063EBC, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }

    if (scn->teacherline.dot[0] >= 0) {
        CompoLineButtonApplyScalingTeacher(scn->teacherline.dot[0]);
        CompoSetTurnIcon(COMPO_TURN_ICON_4);
        CompoDrawTeacherIcon(0, 16, 92, 3, scn->teacherline.dot[0], D_800827D4);
    }
    if (scn->parappaline.dot[0] >= 0) {
        CompoLineButtonApplyScalingTeacher(scn->parappaline.dot[0]);
        CompoSetTurnIcon(COMPO_TURN_ICON_0);
        CompoDrawTeacherIcon(0, 16, 92, 3, scn->parappaline.dot[0], D_800827D4);
    }
    CompoDrawPracticeBar(-1);
    for (i = 0; i < 18; i++) {
        info.x = 41 + i * 14;
        info.y = 98;
        info.scaleid = i;
        info.keyid = scn->teacherline.keyid[0][i];
        if ((info.keyid > PR_TAP_NONE) && (info.keyid < PR_TAP_NUM)) {
            CompoDrawLineButton(&info, D_800827D4);
        }
    }

    CompoSlidingFastSpr(65 + 57 * 0, 80, &D_80063ED0, 7 * 0, 7, 0, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(65 + 57 * 1, 80, &D_80063ED0, 7 * 1, 7, 0, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(65 + 57 * 2 - 1, 80, &D_80063ED0, 7 * 2, 7, 0, 0, &D_80092AB8[D_800827D4]);
    CompoSlidingFastSpr(65 + 57 * 3 - 2, 80, &D_80063ED0, 7 * 3, 7, 0, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(32, 34, &D_80063E94, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(112, 31, &D_80063EA8, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(211, 113, &D_80063F84, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(26, 119, &D_80064010, 0, FALSE, 0, &D_80092AB8[D_800827D4]);

    if (scn->practhighlight != FALSE) {
        CompoFastSpr(227, 178, D_80082420[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(227, 178, D_80082420[1], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
    if (scn->practend == FALSE) {
        CompoFastSpr(234, 187, &D_80062F94, 0, FALSE, 1, &D_80092AB8[D_800827D4]);
    } else {
        CompoFastSpr(234, 187, D_80082428[0], 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    }
}


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


void CompoDrawTitleScreenSel(s32 *sel) {
    CompoDrawTitleScreen(*sel);
}


extern u16 D_80095C54[32][16];

void CompoSonyFade(s32 x, s32 y) {
    u32 unused[2];
    s32 i;
    s32 j;
    u16 color;

    for (i = 0; i < 16; i++) {
        color = D_8006787C[i];
        for (j = 0; j < (32 - 1); j++) {
            D_80095C54[j][i] = CompoFadeColor(color, j);
        }
    }
}

void CompoSonyDrawFade(s32 x, s32 y, s32 t) {
    LoadClut2((u_long *)&D_80095C54[t], x, y);
    DrawSync(0);
}

void CompoSonyLoad(void) {
    CompoLoadTimClut(&D_800655DC, TRUE);
    CompoSonyFade(0, 509);
}

void CompoSonyDraw(s32 i) {
    D_800827D4 = GsGetActiveBuff();
    GsSetWorkBase(D_800827E0[D_800827D4]);
    CompoClear(D_800827D4);

    if (i < 30) {
        CompoSonyDrawFade(0, 509, i);
    }
    if ((i >= 120) && (i < 150)) {
        CompoSonyDrawFade(0, 509, 150 - i - 1);
    }

    CompoFastSpr(0, 100, &D_8006789C, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
    CompoFastSpr(256, 100, &D_800678B0, 0, FALSE, 0, &D_80092AB8[D_800827D4]);
}

void CompoSonySwap(void) {
    GsSwapDispBuff();
    GsSortClear(0, 0, 0, &D_80092AB8[D_800827D4]);
    CompoDrawOt(D_800827D4);
}


void CompoInitOt(void) {
    s32 i;

    for (i = 0; i < 2; i++) {
        D_80092AB8[i].length = 4;
        D_80092AB8[i].org = D_80092AE0[i];
        D_80092AB8[i].offset = 0;

    }
    CompoSetPacket(D_8008C528[0], D_8008C528[1]);
}


// TODO: appInfo
extern s16 D_8009F834;

void CompoDrawMenu(s32 draw, void *data) {
    u32 unused;

    D_800827D4 = GsGetActiveBuff();
    GsSetWorkBase(D_800827E0[D_800827D4]);
    GsClearOt(0, 0, &D_80092AB8[D_800827D4]);

    switch (draw) {
        case PR_MENU_START: {
            if (D_80082810 != 0) {
                if (D_80082810 == 1) {
                    CompoBox(0, 0, 320, 240, 0x0f0f0f | PARA_SPRITE_TRANS_ON, 0, &D_80092AB8[D_800827D4]);
                    D_80082810++;
                } else {
                    CompoFlip(FALSE);
                    D_80082810 = 0;
                }
            } else {
                CompoDrawTryAgainText(*((s32 *)data));
            }
            break;
        }
        case PR_MENU_HIGHSCORE: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawHighScore((struct MENU_HIGHSCORE_DEFDATA *)data);
            break;
        }
        case PR_MENU_NAMEENTER: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawNameEnter(data);
            break;
        }
        case PR_MENU_STAGE: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawStageSelect(data);
            break;
        }
        case PR_MENU_DEBUG: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            // Possibly CompoDrawDebugMenu(data); here 
            break;
        }
        case PR_MENU_LOADING: {
            CompoFrameDrawText(D_8009F834, 0, D_800827D4);
            break;
        }
        case PR_MENU_MAIN: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawMainMenu((struct MENU_MAIN_DEFDATA *)data);
            break;
        }

        case PR_MENU_CARDNOTINSERT: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSaveDialog(1, (struct CARD_MENU_DIALOG_INFO *)data);
            break;
        }
        case PR_MENU_CARDFORMAT: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSaveDialog(2, (struct CARD_MENU_DIALOG_INFO *)data);
            break;
        }
        case PR_MENU_CARDFULL: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSaveDialog(3, (struct CARD_MENU_DIALOG_INFO *)data);
            break;
        }
        case PR_MENU_CARDSAVE: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSaveDialog(4, (struct CARD_MENU_DIALOG_INFO *)data);
            break;
        }
        case PR_MENU_CARDSAMENAME: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSaveDialog(5, (struct CARD_MENU_DIALOG_INFO *)data);
            break;
        }

        case PR_MENU_SAVE:
        case PR_MENU_LOAD:
        case PR_MENU_REPLAY: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawSlotSelect(draw, (struct CARD_MENU_SLOTSELECT_INFO *)data);
            break;
        }

        case PR_MENU_PRACTICE: {
            CompoDrawFrameMenuGui(3, D_800827D4);
            CompoDrawPractice((SCENE_INFO *)data);
            break;
        }

        default: {
            break;
        }
    }
}

void CompoDrawMenuOt(s32 menu, void *data) {
    GsSwapDispBuff();
    if ((menu != PR_MENU_START) && (menu != PR_MENU_1) && (menu != PR_MENU_LOADING)) {
        GsClearDispArea(0, 0, 0x46);
    }
    GsDrawOt(&D_80092AB8[D_800827D4]);
    FntFlush(-1);
}

extern BOOL D_80082780;

void CompoFrameDraw(void *data, s32 type) {
    D_800827D4 = GsGetActiveBuff();
    D_80082780 = TRUE;
    GsSetWorkBase(D_800827E0[D_800827D4]);
    GsClearOt(0, 0, &D_80092AB8[D_800827D4]);

    switch (type) {
        case PR_FRAME_1: {
            CompoDrawFrameMovieGui(5, &D_80092AB8[D_800827D4]);
            CompoFrameDrawCounterClockwiseImmediate(0, 8, D_800827D4);
            break;
        }
        case PR_FRAME_2: {
            D_80082780 = FALSE;
            CompoFrameDrawCounterClockwiseImmediate(0, 8, D_800827D4);
            break;
        }
        case PR_FRAME_3: {
            CompoFrameDrawCounterClockwiseImmediate(0, 8, D_800827D4);
            break;
        }

        case PR_FRAME_6: {
            CompoDrawSaveDialog(4, (struct CARD_MENU_DIALOG_INFO *)data);
            CompoDrawFrameMenuGui(5, D_800827D4);
            CompoFrameSetAnimInactive(COMPO_FRAME_ANIM_TYPE_4, 8);
            CompoFrameDrawAnim(0, D_800827D4);
            break;
        }
        case PR_FRAME_7: {
            CompoDrawMainMenu(NULL);
            CompoDrawFrameMenuGui(5, D_800827D4);
            CompoFrameSetAnimInactive(COMPO_FRAME_ANIM_TYPE_4, 8);
            CompoFrameDrawAnim(0, D_800827D4);
            break;
        }

        case PR_FRAME_4: {
            D_80082780 = FALSE;
            CompoFrameSetAnimActive(8, COMPO_FRAME_ANIM_TYPE_4);
            CompoFrameDrawAnim(0, D_800827D4);
            break;
        }

        case PR_FRAME_9: {
            if (D_800823EC == 0) {
                RapPlayFrameMovieGuiActive(1);
            }
            if (D_800823EC == 15) {
                RapPlayFrameMovieGuiActive(0);
            }

            if (D_800823EC < 15) {
                CompoDrawFrameMovieGuiNoText(5, &D_80092AB8[D_800827D4]);
            } else {
                CompoDrawFrameMovieGui(5, &D_80092AB8[D_800827D4]);
            }
            D_800823EC++;
            if (D_800823EC > 30) {
                D_800823EC = 192;
            }
            break;
        }
        case PR_FRAME_8: {
            if (D_800823EC == 0) {
                RapPlayFrameMovieGuiActive(0);
            }
            if (D_800823EC == 15) {
                RapPlayFrameMovieGuiActive(1);
            }

            if (D_800823EC < 15) {
                D_80082780 = FALSE;
                CompoDrawFrameMovieGuiNoText(5, &D_80092AB8[D_800827D4]);
            } else {
                CompoDrawFrameMovieGuiNoTextNoBorder(0, &D_80092AB8[D_800827D4]);
            }
            D_800823EC++;
            if (D_800823EC > 30) {
                D_800823EC = 192;
            }
            break;
        }

        case PR_FRAME_5: 
        default: {
            if (data != NULL) {
                CompoFrameDrawText(D_8009F834, 0, D_800827D4);
            }
            break;
        }
        
    }
}

void CompoFrameSwap(void *data) {
    GsSwapDispBuff();
    if (D_80082780 != FALSE) {
        GsClearDispArea(0xff, 0xff, 0xff);
    }
    GsDrawOt(&D_80092AB8[D_800827D4]);
    FntFlush(-1);
}

void CompoFrameMovieGuiDraw(SCENE_INFO *scn, s32 type) {
    RECT r;
    u32 unused;

    D_800827D4 = GsGetActiveBuff();
    GsSetWorkBase(D_800827E0[D_800827D4]);
    GsClearOt(0, 0, &D_80092AB8[D_800827D4]);
    GsSwapDispBuff();

    switch (type) {
        case PR_MOVIE_1: {
            CompoDrawFrameMovieGui(5, &D_80092AB8[D_800827D4]);
            CompoFrameDrawCounterClockwiseImmediate(0, 8, D_800827D4);
            break;
        }
        case PR_MOVIE_5: {
            CompoFrameSetAnimActive(8, COMPO_FRAME_ANIM_TYPE_4);
            CompoFrameDrawAnim(0, D_800827D4);
            break;
        }
        case PR_MOVIE_4: {
            CompoFrameSetAnimActive(8, COMPO_FRAME_ANIM_TYPE_0);
            CompoFrameDrawAnim(0, D_800827D4);
            break;
        }
        case PR_MOVIE_10: {
            CompoFrameDrawAnimMovie(0, D_800827D4);
            break;
        }
        case PR_MOVIE_11: {
            r.x = 0;
            r.y = (D_800827D4 == 0) ? 0 : 240;
            r.w = 320;
            r.h = 240;
            MoveImage(&r, 0, (D_800827D4 == 0) ? 0 : 240);
            CompoDrawFrameMovieGui(5, &D_80092AB8[D_800827D4]);
            break;
        }

        case PR_MOVIE_12:
        default: {
            if (scn->drawtextlang != PARA_LANG_JP) {
                CompoDrawMovieTextEn(scn->entext, &D_80092AB8[D_800827D4]);
            } else {
                CompoDrawMovieTextJp(scn->jptext, &D_80092AB8[D_800827D4]);
            }
            break;
        }
    }
}

void CompoFrameMovieGuiSwap(SCENE_INFO *scn) {
    CompoUpdateWorkOfs(D_800827E0[D_800827D4]);
    GsDrawOt(&D_80092AB8[D_800827D4]);
    FntFlush(-1);
}
