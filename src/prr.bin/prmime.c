#include "prmime.h"

u32 *MimeTodSetPacket(u32 *packet, GsDOBJ2 *obj, s32 mode) {
    u32 *p;
    u32 hdr;
    u32 id;
    u32 flag;
    u32 type;
    u32 len;

    GsCOORD2PARAM *pparam;
    MATRIX *pcoord;
    u8 unused[0x3c];

    GsDOBJ2 dummyObj;
    MATRIX dummyObjCoord;
    GsCOORD2PARAM dummyObjParam;

    static u16 unkclip = 700;
    static s32 nearclip = 1;
    static s32 farclip = 10000;


    p = packet;
    hdr  = *p++;
    id   = (hdr >> 0x00) & 0xffff;
    type = (hdr >> 0x10) & 0x000f;
    flag = (hdr >> 0x14) & 0x000f;
    len  = (hdr >> 0x18) & 0x00ff;

    if (obj == NULL) {
        obj    = &dummyObj;
        pcoord = &dummyObjCoord;
        pparam = &dummyObjParam;
    } else {
        pcoord = &obj->coord2->coord;
        pparam = obj->coord2->param;
        obj->coord2->flg = 0;
    }

    switch (type) {
        case TOD_ATTR: {
            obj->attribute = (obj->attribute & *p) | *(p + 1);
            p += 2;
            break;
        }
        case TOD_COORD: {
            if (flag & 0x01) {
                pcoord->m[0][0] = ONE;
                pcoord->m[1][1] = ONE;
                pcoord->m[2][2] = ONE;
                pcoord->m[0][1] = pcoord->m[0][2] = 0;
                pcoord->m[1][0] = pcoord->m[1][2] = 0;
                pcoord->m[2][0] = pcoord->m[2][1] = 0;
                
                if (flag & 0x02) {
                    pparam->rotate.vx += (*(((long *)p) + 0)) / 360;
                    pparam->rotate.vy += (*(((long *)p) + 1)) / 360;
                    pparam->rotate.vz += (*(((long *)p) + 2)) / 360;
                    p += 3;
                }
                if (flag & 0x04) {
                    pparam->scale.vx = (pparam->scale.vx * *(((short *)p) + 0)) / 4096;
                    pparam->scale.vy = (pparam->scale.vy * *(((short *)p) + 1)) / 4096;
                    pparam->scale.vz = (pparam->scale.vz * *(((short *)p) + 2)) / 4096;
                    p += 2;
                }
                if (flag & 0x08) {
                    pparam->trans.vx += *(((long *)p) + 0);
                    pparam->trans.vy += *(((long *)p) + 1);
                    pparam->trans.vz += *(((long *)p) + 2);
                    p += 3;
                }
                RotMatrixY(pparam->rotate.vy, pcoord);
                RotMatrixX(pparam->rotate.vx, pcoord);
                RotMatrixZ(pparam->rotate.vz, pcoord);
                ScaleMatrix(pcoord, &pparam->scale);
                TransMatrix(pcoord, &pparam->trans);
            } else {
                pcoord->m[0][0] = ONE;
                pcoord->m[1][1] = ONE;
                pcoord->m[2][2] = ONE;
                pcoord->m[0][1] = pcoord->m[0][2] = 0;
                pcoord->m[1][0] = pcoord->m[1][2] = 0;
                pcoord->m[2][0] = pcoord->m[2][1] = 0;

                if (flag & 0x02) {
                    pparam->rotate.vx = (*(((long *)p) + 0)) / 360;
                    pparam->rotate.vy = (*(((long *)p) + 1)) / 360;
                    pparam->rotate.vz = (*(((long *)p) + 2)) / 360;
                    p += 3;
                    RotMatrixY(pparam->rotate.vy, pcoord);
                    RotMatrixX(pparam->rotate.vx, pcoord);
                    RotMatrixZ(pparam->rotate.vz, pcoord);
                }
                if (flag & 0x04) {
                    pparam->scale.vx = *(((short *)p) + 0);
                    pparam->scale.vy = *(((short *)p) + 1);
                    pparam->scale.vz = *(((short *)p) + 2);
                    p += 2;
                    if(!(flag & 0x02)) {
                        RotMatrixY(pparam->rotate.vy, pcoord);
                        RotMatrixX(pparam->rotate.vx, pcoord);
                        RotMatrixZ(pparam->rotate.vz, pcoord);
                    }
                    ScaleMatrix(pcoord, &pparam->scale);
                }
                if (flag & 0x08) {
                    pparam->trans.vx = *(((long *)p) + 0);
                    pparam->trans.vy = *(((long *)p) + 1);
                    pparam->trans.vz = *(((long *)p) + 2);
                    p += 3;
                    TransMatrix(pcoord, &pparam->trans);
                }
            }
            break;
        }
        case TOD_MATRIX: {
            s16 *mp;

            mp = (s16 *)p;
            pcoord->m[0][0] = *(mp + 0);
            pcoord->m[1][0] = *(mp + 1);
            pcoord->m[2][0] = *(mp + 2);
            pcoord->m[0][1] = *(mp + 3);
            pcoord->m[1][1] = *(mp + 4);
            pcoord->m[2][1] = *(mp + 5);
            pcoord->m[0][2] = *(mp + 6);
            pcoord->m[1][2] = *(mp + 7);
            pcoord->m[2][2] = *(mp + 8);
            p += 5;
            pcoord->t[0] = *(p + 0);
            pcoord->t[1] = *(p + 1);
            pcoord->t[2] = *(p + 2);
            p += 3;
            break; 
        }
        case TOD_TMDID: {
            break;
        }
        case TOD_PARENT: {
            if (mode != TOD_COORDONLY) {
                p++;
            }
            break;
        }
        case TOD_OBJCTL: {
            break;
        }
        case TOD_LIGHT: {
            if (flag & 0x02) {
                p += 3;
            }
            if (flag & 0x04) {
                p++;
            }
            break;
        }
        case TOD_CAMERA: {
            if (flag & 0x01) {
                
            } else {
                if (flag & 0x04) {
                    p += 6;
                }
                if (flag & 0x08) {
                    p++;
                }
            }
            break;
        }
        case TOD_CLIP: {
            s32 cliptmp;

            cliptmp = *(s32 *)(p + 0);
            if ((cliptmp > 0) && (cliptmp != unkclip)) {

            }

            cliptmp = *(s32 *)(p + 1);
            if ((cliptmp > 0) && (cliptmp != nearclip)) {
                nearclip = cliptmp;
                GsSetNearClip(nearclip);
            }
            cliptmp = *(s32 *)(p + 2);
            if ((cliptmp > 0) && (cliptmp != farclip)) {
                farclip = cliptmp;
                GsSetFarClip(farclip);
            }
            p += len - 1;
            break;
        }
    }
    return packet + len;
}

// Returns a pointer to the frame data of the TOD
u32 *MimeTodSetFrame(s32 frame, u32 *tod, GsDOBJ2 *obj, s32 mode) {
    u32 hdr;
    u32 packetnum;
    u32 todframe;
    s32 i;

    hdr = *tod;
    packetnum = (hdr >> 0x10) & 0xffff;
    todframe = *(tod + 1);

    if (todframe > frame) {
        return tod;
    }

    tod += 2;
    for (i = 0; i < packetnum; i++) {
        tod = MimeTodSetPacket(tod, obj, mode);
    }
    return tod;
}

s32 MimeDatInit(PARA_DAT_DATA *data, PARA_DAT_OBJECT *obj, u8 *dat, s32 wrap) {
    s32 i;
    u16 fr;
    
    data->obj = obj;

    data->objnum = *((s16 *)dat)++;
    data->wrap = wrap;

    fr = 0;
    for (i = 0; i < data->objnum; i++) {
        data->obj[i].num = *((s16 *)dat)++;
        if (fr < data->obj[i].num) {
            fr = data->obj[i].num;
        }
        data->obj[i].out = ((s16 *)dat);
        dat = (u8 *)(((s16 *)dat) + data->obj[i].num);
    }
    data->framenum = fr;

    if (fr == 0) {
        return -1;
    }
    return data->objnum;
}

s16 MimeDatGetKeyFrame(PARA_DAT_DATA *data, s16 key) {
    s16 n;

    n = data->frame;
    if (n >= data->obj[key].num) {
        n = data->obj[key].num + -1;
    }
    return data->obj[key].out[n];
}

void MimeDatSetFrame(PARA_DAT_DATA *data, u16 frame) {
    if (data->wrap == TRUE) {
        frame %= data->framenum;
    }
    data->frame = frame;
}
