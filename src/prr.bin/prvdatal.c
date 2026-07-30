#include "prvdatal.h"

static char rcsid[] = "@(#)prvdatal.c: version 01-00 95/10/10 00:00:00";

CD_FILE D_80067DE0 = {"\\S0\\COMMON.INT;1", 0, 0,};
CD_FILE D_80067E0C = {"\\SS\\MOVIE0.STR;1", 1, 127};
CD_FILE D_80067E38 = {"\\S0\\YCOMPO.INT;1", 0, 0,};
CD_FILE D_80067E64 = {"\\S6\\ZMOVIE6.STR;1", 1, 127};
VDATA_SCENE D_80067E90[9] = {
    {"SCENE0", 1, 0x2580, 0x60, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S0\\COMOD0.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S0\\COMPO00.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE0.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\SS\\MOVIE0T.STR;1", 1, 127},
            [VDATA_SCENE_FILE_XMOVIE] = {NULL, 0, 0},
            [VDATA_SCENE_FILE_YMOVIE] = {NULL, 0, 0},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S0\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE1", 1, 0x2af8, 0x60, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S1\\COMOD1.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S1\\COMPO01.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE1.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S1\\STAGE1.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S1\\XMOVIE1.STR;1", 1, 90},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S1\\XMOVIE1.STR;1", 1, 90},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S1\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE2", 2, 0x29cc, -0x30, 2, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S2\\COMOD2.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S2\\COMPO02.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE2.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S2\\STAGE2.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S2\\XMOVIE2.STR;1", 1, 127},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S2\\YMOVIE2.STR;1", 1, 127},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S2\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE3", 3, 0x1e78, -0x90, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S3\\COMOD3.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S3\\COMPO03.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE3.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S3\\STAGE3.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S3\\XMOVIE3.STR;1", 1, 127},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S3\\YMOVIE3.STR;1", 1, 127},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S3\\ZCOMPO.INT;1", 1, 90},
        }
    },
    // No scene4. Sorry
    {"SCENE5 PROTO", 5, 0x2710, -0x18, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S5\\COMOD5.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S5\\COMPO05.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE5.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S5\\STAGE5.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S5\\XMOVIE5.STR;1", 1, 90},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S5\\YMOVIE5.STR;1", 1, 90},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S5\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE6", 6, 0x2580, 0, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S6\\COMOD6.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S6\\COMPO06.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE6.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S6\\STAGE6.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S6\\XMOVIE6.STR;1", 1, 90},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S6\\YMOVIE6.STR;1", 1, 90},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S6\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE7", 7, 0x27d8, -0x60, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S7\\COMOD7.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S7\\COMPO07.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {"\\SS\\MOVIE7.STR;1", 1, 90},
            [VDATA_SCENE_FILE_XA]     = {"\\S7\\STAGE7.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {NULL, 0, 0},
            [VDATA_SCENE_FILE_YMOVIE] = {NULL, 0, 0},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S7\\ZCOMPO.INT;1", 0, 0},
        }
    },
    {"SCENE8", 1, 0x27d8, -0x60, 0, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S8\\COMOD8.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {NULL, 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {NULL, 0, 0},
            [VDATA_SCENE_FILE_XA]     = {NULL, 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {"\\S8\\XMOVIE8.STR;1", 1, 90},
            [VDATA_SCENE_FILE_YMOVIE] = {"\\S8\\XMOVIE8.STR;1", 1, 90},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S8\\ZCOMPO.INT;1", 1, 90},
        }
    },
    {"SCENE9", 1, 0x2c56, 0x60, -0x60, 
        {
            [VDATA_SCENE_FILE_COMOD]  = {"\\S9\\COMOD9.BIN;1", 0, 0},
            [VDATA_SCENE_FILE_COMPO]  = {"\\S9\\COMPO09.INT;1", 0, 0},
            [VDATA_SCENE_FILE_MOVIE]  = {NULL, 0, 0},
            [VDATA_SCENE_FILE_XA]     = {"\\S9\\STAGE9.XA1;1", 1, 90},
            [VDATA_SCENE_FILE_XMOVIE] = {NULL, 1, 90},
            [VDATA_SCENE_FILE_YMOVIE] = {NULL, 1, 90},
            [VDATA_SCENE_FILE_ZCOMPO] = {"\\S9\\ZCOMPO.INT;1", 1, 90},
        }
    },
};
int D_80082594 = 8;
LVL_INFO D_80068A3C[5] = {
    {"HIGH", 5, {1, 0}},
    {"NORMAL", 1, {2, 0}},
    {"LOWER1", 2, {3, 1}},
    {"LOWER2", 3, {3, 2}},
    {"XTIME", 6, {0, 4}},
};
