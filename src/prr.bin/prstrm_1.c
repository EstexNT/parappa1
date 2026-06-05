#include "prstrm.h"
#include <libcd.h>

STRM_DEC_CTRL_REC *strmDecCtrlRec;
void *strmSectorBuffer;
void *strmVlcBuffer;
void *strmImgBuffer;
u32 strmCurFrame;


// TODO: dbgInfo
extern s32 D_80068C2C;
extern s32 D_80068C30;


void *StrmNext(void) {
    void *addr;
    StHEADER *header;
    static s32 D_80082550 = 0;
    
    if (StGetNext(&addr, &header) != 0) {
        return NULL;
    }
    
    if ((D_80082550 + 1) != header->frameCount) {
        D_80068C30++;
    }
    D_80082550 = header->frameCount;
    D_80068C2C = header->frameCount;
    
    if ((strmDecCtrlRec->width != header->width) || (strmDecCtrlRec->height != header->height)) {
        strmDecCtrlRec->width = header->width;
        strmDecCtrlRec->height = header->height;
    }
    strmCurFrame++;
    return addr;
}
