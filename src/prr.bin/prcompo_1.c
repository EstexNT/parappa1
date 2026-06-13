#include "prcompo.h"
#include <string.h>
#include "prcompo_data.h"


extern PACKET *D_800827E0[2];
extern PACKET D_8008C528[2][13000];
extern GsOT D_80092AB8[2];
extern GsOT_TAG D_80092AE0[2][16];
extern s32 D_800827D4;

extern u8 D_80096054[32];
static u8 *D_80082488 = D_80096054;


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

void CompoDrawThoughtBubble(SCENE_INFO *scn, BOOL draw) {
    D_800827D4 = GsGetActiveBuff();
    GsSetWorkBase(D_800827E0[D_800827D4]);
    GsClearOt(0, 0, &D_80092AB8[D_800827D4]);
    GsSwapDispBuff();
    if (draw != FALSE) {
        CompoFastSpr(32, 48, &D_800642F4, 0, FALSE, 2, &D_80092AB8[D_800827D4]);
    }
}

void CompoDrawOtThoughtBubble(SCENE_INFO *scn) {
    GsDrawOt(&D_80092AB8[D_800827D4]);
    FntFlush(-1);
}

void func_8002fa2c(void) {
    u32 i;

    for (i = 0; i < 2; i++) {
        ClearImage(&D_800678C4[i], 0, 0, 0);
        DrawSync(0);
    }
}


s32 D_80082748;
u16 D_8008274C;
u16 D_80082750;
u16 D_80082754;
u16 D_80082758;
u16 D_8008275C;
u16 D_80082760;
s32 D_80082764;
s32 D_80082768;
s32 D_8008276C;
s32 D_80082770;
RECT D_80082774;
COMPO_SPRITE *D_8008277C;
BOOL D_80082780;

void CompoDrawTextSaveTitle(register PARA_EN_TEXT *text, register s32 pal, register GsOT *ot) {
    s32 i;
    s32 len;
    u32 unused[6];
    GsSPRITE gspr;
    s16 u;
    s16 v;
    s32 ch;
    s32 x;

    len = strlen(text);
    gspr.attribute = D_80062A44.attr;
    x = 0;

    for (i = 0; i < len; i++) {
        ch = text[i];
        if (ch >= 0x80) {
            continue;
        }

        gspr.w = D_800646E0[ch].w;
        gspr.h = D_800646E0[ch].h;
        gspr.x = D_8008274C + x - (320 / 2);
        x += gspr.w;
        gspr.y = D_80082750 + D_80082748 - (240 / 2);
        if (D_800646E0[ch].ty >= 0) {
            gspr.cx = D_80062A44.px;
            gspr.cy = D_80062A44.py + pal;
            u = (D_80062A44.tx << 2) + D_800646E0[ch].tx;
            v = (D_80062A44.ty << 0) + D_800646E0[ch].ty;
            gspr.tpage = GetTPage(0, 1, (u & 0xff00) >> 2, (v & 0xff00));
            gspr.u = u;
            gspr.v = v;
            GsSortFastSprite(&gspr, ot, 0);
        }
    }
}

void CompoDrawTextEn(register s32 cy, register PARA_EN_TEXT *text, register GsOT *ot) {
    s32 i;
    s32 len;
    u32 unused[6];
    GsSPRITE gspr;
    s16 u;
    s16 v;
    s32 ch;
    s32 x;
    s32 xbase;
    s32 xbasenl;
    BOOL nl;
    register s32 pal;
    
    pal = cy;
    nl = FALSE;
    len = strlen(text);
    gspr.attribute = D_80062A30.attr;
    x = 0;
    xbasenl = 0;
    xbase = 0;

    for (i = 0; i < len; i++) {
        if (text[i] == '\n') {
            nl = TRUE;
            continue;
        }
        ch = text[i];
        if (ch >= 0x80) {
            continue;
        }

        if (nl != FALSE) {
            xbasenl += D_800643E4[ch].w;
        } else {
            xbase += D_800643E4[ch].w;
        }
    }

    xbase = (264 - xbase) / 2;
    if (nl != FALSE) {
        xbasenl = (264 - xbasenl) / 2;
    }

    for (i = 0; i < len; i++) {
        if (text[i] == '\n') {
            D_80082748 += 15;
            x = 0;
            xbase = xbasenl;
            continue;
        }

        ch = text[i];
        if (ch >= 0x80) {
            continue;
        }

        gspr.w = D_800643E4[ch].w;
        gspr.h = D_800643E4[ch].h;
        gspr.x = xbase + D_8008274C + x - (320 / 2);
        x += gspr.w;
        gspr.y = D_80082750 + D_80082748 - (240 / 2);
        if (D_800643E4[ch].ty >= 0) {
            gspr.cx = D_80062A30.px;
            gspr.cy = pal;
            u = (D_80062A30.tx << 2) + D_800643E4[ch].tx;
            v = (D_80062A30.ty << 0) + D_800643E4[ch].ty;
            gspr.tpage = GetTPage(0, 1, (u & 0xff00) >> 2, (v & 0xff00));
            gspr.u = u;
            gspr.v = v;
            GsSortFastSprite(&gspr, ot, 0);
        }
    }
}


// NOT MATCHING:
// D_800823EC is always accessed via $gp (same-TU declaration)
// Meanwhile (i.e.) D_80082748 is always accessed via direct addressing (extern)
// However, the two functions above access (i.e.) D_80082748 via $gp
// while CompoFrameSetAnim accesses D_800823EC via $gp which should not be possible 
#ifndef NON_MATCHING
INCLUDE_ASM("asm/prr.bin/nonmatchings/prcompo_1", CompoFrameSetAnim);
#else
extern s32 D_800823EC;
extern u8 D_80092B84[12][16];

void CompoFrameSetAnim(register s32 type, register s32 b) {
    s32 x;
    s32 y;

    switch (type) {
        case COMPO_FRAME_ANIM_TYPE_9: {
            x = D_800649DC[D_800823EC].x;
            y = D_800649DC[D_800823EC].y;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_1: {
            if (D_800823EC < 48) {
                x = D_800823EC % 12;
            } else {
                if ((D_800823EC >= 48) && (D_800823EC < 96)) {
                    x = (D_800823EC + 1) % 12;
                } else if ((D_800823EC >= 96) && (D_800823EC < 144)) {
                    x = (D_800823EC + 2) % 12;
                } else {
                    x = (D_800823EC + 3) % 12;
                }
            }

            y = D_800823EC % 16;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_2: {
            y = D_800823EC % 16;
            x = D_800823EC / 16;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_3: {
            x = D_800823EC % 12;
            y = (D_800823EC / 12) % 16;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_10: {
            x = D_800649DC[191 - D_800823EC].x;
            y = D_800649DC[191 - D_800823EC].y;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_5: {
            x = D_800823EC % 12;
            y = (15 - (D_800823EC / 12)) % 16;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_6: {
            if (D_800823EC < 48) {
                x = D_800823EC % 12;
            } else {
                if ((D_800823EC >= 48) && (D_800823EC < 96)) {
                    x = (D_800823EC + 1) % 12;
                } else if ((D_800823EC >= 96) && (D_800823EC < 144)) {
                    x = (D_800823EC + 2) % 12;
                } else {
                    x = (D_800823EC + 3) % 12;
                }
            }

            y = 15 - (D_800823EC % 16);
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_7: {
            x = 11 - (D_800823EC / 16);
            y = 15 - (D_800823EC % 16);
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_8: {
            x = 11 - (D_800823EC % 12);
            y = 15 - ((15 - (D_800823EC / 12)) % 16);
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_0: {
            x = D_80064FDC[D_800823EC].x;
            y = D_80064FDC[D_800823EC].y;
            break;
        }
        case COMPO_FRAME_ANIM_TYPE_4: {
            x = D_80064FDC[191 - D_800823EC].x;
            y = D_80064FDC[191 - D_800823EC].y;
            break;
        }
        default: {
            break;
        }
    }
    D_80092B84[x][y] = b;
}
#endif

void CompoFrameMakeLast(register void *data, register s32 type, register void (*pre)(void *data, s32 type), register void (*post)(void *data)) {
    s32 i;

    for (i = 0; i < 4; i++) {
        pre(data, type);
        RapPlayFrameActive(30);
        VSync(2);
        post(data);
    }
}

void CompoFrameMovieGuiMakeLast(register void *data, register s32 type, register void (*pre)(void *data, s32 type), register void (*post)(void *data)) {
    s32 i;

    for (i = 0; i < 4; i++) {
        pre(data, type);
        VSync(2);
        post(data);
    }
}


u16 CompoFadeColor(register u16 color, register s32 i) {
    u32 r;
    u32 g;
    u32 b;
    u32 stp;
    u32 fader;
    u32 fadeg;
    u32 fadeb;
    u16 fadecolor;

    if ((color & 0xffff) == 0) {
        return color;
    }

    r = ((color >> 0) & 0x1f);
    g = ((color >> 5) & 0x1f);
    b = ((color >> 10) & 0x1f);

    stp = 0x8000;
    fader = (r * i) / 30;
    fadeg = (g * i) / 30;
    fadeb = (b * i) / 30;

    fadecolor = stp | (fadeb << 10) | (fadeg << 5) | (fader << 0);
    return fadecolor;
}
