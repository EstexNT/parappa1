#ifndef _PRMDL_H_
#define _PRMDL_H_

#include "common.h"
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>

// .tod
// Model animation (Movement/Rotation)
// Packet types
#define TOD_ATTR    0
#define TOD_COORD   1
#define TOD_TMDID   2
#define TOD_PARENT  3
#define TOD_MATRIX  4
#define TOD_TMDDATA 5
#define TOD_LIGHT   6
#define TOD_CAMERA  7
#define TOD_OBJCTL  8
#define TOD_USER0   9
#define TOD_CLIP    TOD_USER0
#define TOD_USER1   10
#define TOD_USER2   11
#define TOD_USER3   12
#define TOD_USER4   13
#define TOD_SYSFUNC 15
// Modes
#define TOD_CREATE    0
#define TOD_NOCREATE  1
#define TOD_COORDONLY 2

// .vdf
// Vertex animation
typedef struct PARA_VDF_OBJECT {
    SVECTOR *top;
    u32 offset; // Offset of the vector
    u32 total; // Number of vectors
    u32 object; // Number of the object
} PARA_VDF_OBJECT;

typedef struct PARA_VDF_DATA {
    s32 objnum;
    PARA_VDF_OBJECT *obj;
    u32 *mime;
} PARA_VDF_DATA;

// .dat
// Animation timing and smoothing
typedef struct PARA_DAT_OBJECT {
    u16 num;
    s16 *out;
} PARA_DAT_OBJECT;

typedef struct PARA_DAT_DATA {
    u16 objnum;
    PARA_DAT_OBJECT *obj;
    s16 wrap;
    u16 framenum;
    s16 frame;
} PARA_DAT_DATA;


u32 *MimeTodSetFrame(s32 frame, u32 *tod, GsDOBJ2 *obj, s32 mode);
s32 MimeDatInit(PARA_DAT_DATA *data, PARA_DAT_OBJECT *obj, u8 *dat, s32 wrap);
s16 MimeDatGetKeyFrame(PARA_DAT_DATA *data, s16 key);
void MimeDatSetFrame(PARA_DAT_DATA *data, u16 frame);

#endif
