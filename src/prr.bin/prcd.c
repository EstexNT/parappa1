#include "prcd.h"
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libsnd.h>
#include "prrap.h"
#include "prvdbg.h"
#include "prmemory.h"
#include "prscene.h"
#include "print.h"

static char rcsid[] = "@(#)prcd.c: version 01-00 95/10/10 00:00:00";

// Should be type CdlCB...
extern void (*cdStrmPrevCb)();
extern s32 cdStrmSectorCount;
extern BOOL cdFailed;

extern s32 D_800827F0;
extern s32 cdStrmFrameSect;
extern s32 D_80082814;

static CD_INFO cdInfoData = {0};
CD_INFO *cdInfo = &cdInfoData;


void CDReadStreamCallback(u_char status, u_char *result);
void CDSyncCallback(u_char status, u_char *result);
s32 CDReadSub(u8 *buf, s32 sec, s32 mode);
BOOL CDSeek(CdlLOC * loc, s32 ofs);
BOOL CDRead(CdlFILE *file, s32 mode, BOOL snd);

void VramBlockLoad(void *ptr);
void SoundBlockLoad(void *vh, void *vb, s32 wait);
const char D_800103BC[] = "%x: S_TransferToSB\nHEAD: %8x  BODY: %8x\n";


void CDInit(void) {
    CdlATV tmp = {64, 64, 64, 64};
    
    CdInit();
    CdMix(&tmp);
}

void CDReadStreamCallback(u_char status, u_char *result) {
    cdStrmPrevCb();
    if (status == CdlDataReady) {
        D_80082814++;
        cdStrmFrameSect += cdStrmSectorCount;
    }
}

void func_8001d590(void) {
    D_800827F0++;
}

void CDSyncCallback(u_char status, u_char *result) {
    if (status != CdlComplete) {
        return;
    }
    // CdlGetlocP: 
    // Byte 2 - Minute
    // Byte 3 - Second
    // Byte 4 - Frame
    cdInfo->curloc.minute = result[2];
    cdInfo->curloc.second = result[3];
    cdInfo->curloc.sector = result[4];
}

void CDSetSyncCallback(void) {
    CdSyncCallback(CDSyncCallback);
}

CdlFILE *CDSearch(CdlFILE *file, char *fname) {
    s32 i;
    
    if (fname == NULL) {
        return NULL;
    }

    for (i = 0; i < 4; i++) {
        if (CdSearchFile(file, fname) != NULL) {
            return file;
        }
    }
    return NULL;
}

s32 CDFileSearch(CD_FILE *cdf) {
    if (cdf->search == TRUE) {
        return 0;
    }
    if (cdf->fname == NULL) {
        return 0;
    }
    
    if (CDSearch(&cdf->cdl, cdf->fname) == NULL) {
        return -1;
    }
    cdf->pos = CdPosToInt(&cdf->cdl.pos);
    cdf->end = cdf->pos + (cdf->cdl.size / 2048);
    cdf->search = TRUE;
    return 0;
}

BOOL CDGetCurLoc(void) {
    u8 res[16];
    
    if (CdSync(1, res) != CdlComplete) {
        return FALSE;
    }

    // CdlGetlocP: 
    // Byte 2 - Minute
    // Byte 3 - Second
    // Byte 4 - Frame
    cdInfo->curloc.minute = res[2];
    cdInfo->curloc.second = res[3];
    cdInfo->curloc.sector = res[4];
    return TRUE;
}

void CDSetVol(s32 vol) {
    SsSetSerialVol(SS_SERIAL_A, vol, vol);
}

void CDToggleVol(BOOL disable) {
    CDSetVol((disable == TRUE) ? 0 : 127);
}

void CDStartStream(CD_FILE *cdf, BOOL fast, BOOL arg2) {
    u32 unused;
    s32 mode;
    
    if (cdf->fname == NULL) {
        return;
    }
    
    cdInfo->strmloc = cdf->cdl.pos;
    cdInfo->strmsec = CdPosToInt(&cdInfo->strmloc);
    CDSetVol(cdf->vol);
    do {
        do {
            while (CdControlB(CdlSetloc, (u_char *)&cdInfo->strmloc, &cdInfo->result) == 0);
        } while (CdSync(0, NULL) != CdlComplete);
        // Now at the location of the stream

        // Setup the filter
        cdInfo->strmfilter.file = 1;
        cdInfo->strmfilter.chan = cdf->chan;
        cdInfo->strmfilter.pad = 0;
        do {
            while (CdControlB(CdlSetfilter, (u_char *)&cdInfo->strmfilter, &cdInfo->result) == 0);
        } while (CdSync(0, NULL) != CdlComplete);
        
        if (arg2 == TRUE) {
            mode = CdlModeStream | CdlModeRT | CdlModeSF;
        } else {
            mode = CdlModeStream | CdlModeRT | CdlModeSF;
        }
        if (fast == TRUE) {
            mode |= CdlModeSpeed;
            cdStrmSectorCount = 16;
        } else {
            cdStrmSectorCount = 4;
        }

        while (CdRead2(mode) == 0);
    } while (CdSync(0, NULL) != CdlComplete);
    D_80082814 = 0;
    cdStrmFrameSect = 0 - cdStrmSectorCount;
    cdStrmPrevCb = CdReadyCallback(CDReadStreamCallback);
    cdInfo->unk28 = TRUE;
}

void CDSetFilter(s32 chan) {
    u8 res[16];
    
    cdInfo->strmfilter.chan = chan;
    do {
        CdControlB(CdlSetfilter, (u_char *)&cdInfo->strmfilter, res);
    } while (CdSync(0, NULL) != CdlComplete);
}

u8 CDGetFilterChan(void) {
    return cdInfo->strmfilter.chan;
}

void CDStop(void) {
    while (CdControlB(CdlStop, NULL, cdInfo->result) == 0);
    CdSyncCallback(NULL);
}

void CDPause(void) {
    do {
        while (CdControlB(CdlPause, NULL, cdInfo->result) == 0);
    } while (CdSync(0, NULL) != CdlComplete);
}

s32 CDGetCurrentSector(s32 pos) {
    // Absolute sector value (Skip the header lasting 2 seconds)
    cdInfo->cursec = CdPosToInt(&cdInfo->curloc) + 150;
    // Relative sector value
    cdInfo->relsec = cdInfo->cursec - pos;
    return cdInfo->relsec;
}


s32 CDGetCurrentStreamSector(s32 pos) {
    // Absolute sector value (Skip the header lasting 2 seconds)
    cdInfo->cursec = cdStrmFrameSect + 150;
    // Relative sector value
    cdInfo->relsec = cdInfo->cursec - pos;
    return cdStrmFrameSect;
}

BOOL CDFileEnded(CD_FILE *cdf) {
    return (cdInfo->cursec + 75) >= cdf->end;
}

s32 CDReadSub(u8 *buf, s32 sec, s32 mode) {
    long tmp; // Has to be long to match
    s32 readmode;
    
    readmode = (mode == 1) ? CdlModeSpeed : 0;
    while (CdRead(sec, buf, readmode) == 0);
    while ((tmp = CdReadSync(1, NULL)) > 0);
    
    return (tmp == 0) ? sec : 0;
}

BOOL CDSeek(CdlLOC *loc, s32 ofs) {
    s32 pos;
    
    pos = CdPosToInt(loc);
    pos += ofs;
    CdIntToPos(pos, loc);
    if (CdControlB(CdlSetloc, (u_char *)loc, NULL) == 0) {
        return FALSE;
    } else {
        return TRUE;
    }
}

void func_8001e0ec(UNK_DATA *ptr, char *name) {
    u8 *iptr;
    u32 unused;
    s32 i;
    s32 err;
    
    if (ptr == NULL) {
        return;
    }
    
    FntPrint("\n\n\n%s: %x\n", name, ptr);
    err = 0;
    FntPrint("seq:%d  size:%d  nelm:%d\n", ptr->seq, ptr->size, ptr->nelm);
    iptr = ptr->cp;
    FntPrint("cp: %8x\n", iptr);
    if (iptr == NULL) {
        return;
    }
    
    for (i = 0; i < ptr->nelm; i++, iptr++) {
        if (*iptr != (i % 256)) {
            FntPrint("Error: %dth element(= %d) wrong.\n", i, *iptr);
            err++;
            break;
        }
    }
    if (i == ptr->nelm) {
        FntPrint("Elem Check OK\n");
    }
    
    iptr = ptr->cp + ptr->nelm;
    for (i = 0; i < 4; i++, iptr++) {
        if (*iptr != 0x69) {
            FntPrint("Error: %dth magic(= %x) wrong.\n", i, *iptr);
            err++;
            break;
        }
    }
    if (i == 4) {
        FntPrint("MAGIC Check OK\n");
    }
}

const char D_80010480[] = "\n\nI:%3d\nptr:%x\n";
const char D_80010490[] = "\nMEM LEFT:%5d KB  USED:%5d KB\n";


s32 VramBlockRead(void *ptr, s32 mode);
const char D_800104B0[] = "Alloc VramBlockRead";

s32 SoundBlockRead(void *ptr, s32 mode);
const char D_800104C4[] = "Alloc Sound Head";
const char D_800104D8[] = "Alloc Sound Body";

s32 OnMemBlockRead(void *ptr, s32 mode);
const char D_800104EC[] = "Alloc OnMemBlockRead";


BOOL CDRead(CdlFILE *file, s32 mode, BOOL snd) {
    u8 *buf;
    INT_BLOCK_HEADER *hdr;
    long next; // Has to be long to match

    if (CDSeek(&file->pos, 0) == FALSE) {
        return FALSE;
    }

    while (TRUE) {
        // Read first 4 sectors of a file
        buf = MemoryEndAlloc(2048 * 4);
        if (buf == NULL) {
            DbgError("Block head malloc failed");
            exit(1);
        }

        if (CDReadSub(buf, 4, mode) == 0) {
            return FALSE;
        }
        hdr = (INT_BLOCK_HEADER *)buf;
        if (CDSeek(&file->pos, 4) == FALSE) {
            return FALSE;
        }

        if (hdr->type == INT_BLOCK_TYPE_VRAM) {
            if ((next = VramBlockRead(buf, mode)) == 0) {
                return FALSE;
            }
        } else if (hdr->type == INT_BLOCK_TYPE_SND) {
            if ((next = SoundBlockRead(buf, mode)) == 0) {
                return FALSE;
            }
            if (snd == TRUE) {
                RapStopSeq();
                RapPlayKey(sceneInitInfo.intermission);
                RapFlush();
            }
        } else if (hdr->type == INT_BLOCK_TYPE_MEM) {
            if ((next = OnMemBlockRead(buf, mode)) == 0) {
                return FALSE;
            }
        } else if (hdr->type == INT_BLOCK_TYPE_END) {
            break;
        } else {
            cdFailed = TRUE;
            return FALSE;
        }
        CDSeek(&file->pos, next);
    }
    return TRUE;
}

BOOL CDFileRead(CD_FILE *cdf, BOOL snd) {
    s32 i;
    long ret; // Has to be long to match
    s32 mode;
    s32 vs;
    CdlFILE file;

    mode = 1;
    ret = FALSE;
    for (i = 0; i < 4; i++) {
        if (i >= 1) {
            mode = 0;
        }
        vs = VSync(-1);
        CDInit();
        MemoryClear();
        if (CDFileSearch(cdf) < 0) {
            continue;
        }

        file = cdf->cdl;
        if ((ret = CDRead(&file, mode, snd)) == TRUE) {
            break;
        }
    }
    return ret;
}

#define CDROUNDUP(n, s) (((n) + ((s) - 1)) / (s))

BOOL CDFileReadBuf(CD_FILE *cdf, u8 *buf) {
    s32 i;
    s32 mode;
    s32 size;
    s32 ret;
    CdlFILE file;

    mode = 1;
    for (i = 0; i < 4; i++) {
        mode = 1;
        if (i >= 1) {
            mode = 0;
        }
        if (CDFileSearch(cdf) < 0) {
            continue;
        }

        size = CDROUNDUP(cdf->cdl.size, 2048);
        file = cdf->cdl;
        CDSeek(&file.pos, 0);
        ret = CDReadSub(buf, size, mode);
        if (ret == size) {
            break;
        }
    }
    return (ret == size);
}

s32 VramBlockRead(register void *ptr, register s32 mode) {
    void *block;
    u8 *iblock;
    INT_BLOCK_HEADER *hdr;
    INT_FILE_HEADER *filehdr;
    s32 i;

    hdr = (INT_BLOCK_HEADER *)ptr;
    filehdr = (INT_FILE_HEADER *)(((u8 *)ptr) + sizeof(INT_BLOCK_HEADER));
    block = MemoryAlloc(hdr->size * 2048);
    if (block == NULL) {
        DbgError(D_800104B0);
        exit(1);
    }

    if (CDReadSub(block, hdr->size, mode) == 0) {
        return 0;
    }
    iblock = block;
    for (i = 0; i < hdr->filenum; i++) {
        VramBlockLoad(iblock);
        iblock += filehdr[i].size;
    }
    MemoryFree();
    return hdr->size;
}

s32 SoundBlockRead(register void *ptr, register s32 mode) {
    INT_BLOCK_HEADER *hdr;
    INT_FILE_HEADER *filehdr;
    void *vb;
    void *vh;

    hdr = (INT_BLOCK_HEADER *)ptr;
    filehdr = (INT_FILE_HEADER *)(((u8 *)ptr) + sizeof(INT_BLOCK_HEADER));
    vh = MemoryAlloc(filehdr[0].size);
    if (vh == NULL) {
        DbgError(D_800104C4);
        exit(1);
    }

    vb = MemoryAlloc(filehdr[1].size);
    if (vb == NULL) {
        DbgError(D_800104D8);
        exit(1);
    }

    if (CDReadSub(vh, hdr->size, mode) == 0) {
        MemoryFree();
        MemoryFree();
        return 0;
    }

    SoundBlockLoad(vh, vb, TRUE);
    MemoryFree();
    return hdr->size;
}

s32 OnMemBlockRead(register void *ptr, register s32 mode) {
    void *block;
    u8 *iblock;
    INT_BLOCK_HEADER *hdr;
    INT_FILE_HEADER *filehdr;
    u32 mpos;
    s32 i;

    hdr = (INT_BLOCK_HEADER *)ptr;
    block = MemoryAlloc(hdr->size * 2048);
    if (block == NULL) {
        DbgError(D_800104EC);
        exit(1);
    }

    if (CDReadSub(block, hdr->size, mode) == 0) {
        return 0;
    }

    mpos = MemoryGetPos(block);
    filehdr = (INT_FILE_HEADER *)(((u8 *)ptr) + sizeof(INT_BLOCK_HEADER));
    iblock = block;
    if (mpos == NULL) {
        return hdr->size;
    }

    for (i = 0; i < hdr->filenum; i++, filehdr++) {
        if (MemorySetPos(iblock, mpos + i, filehdr->size) == FALSE) {
            return 0;
        }
        iblock += filehdr->size;
    }
    return hdr->size;
}

void VramBlockLoad(register void *ptr) {
    RECT r;
    GsIMAGE img;

    GsGetTimInfo(((u8 *)ptr) + 4, &img);
    r.x = img.px;
    r.y = img.py;
    r.w = img.pw;
    r.h = img.ph;
    LoadImage(&r, img.pixel);
    DrawSync(0);
    if ((img.pmode >> 3) & 1) {
        r.x = img.cx;
        r.y = img.cy;
        r.w = img.cw;
        r.h = img.ch;
        LoadImage(&r, img.clut);
        DrawSync(0);
    }
}

void SoundBlockLoad(register void *vh, register void *vb, register s32 wait) {
    char tmp[64];

    RapCloseHead();
    RapInit();

    if (RapOpenHead(vh) == 0) {
        sprintf(tmp, D_800103BC, dbgInfo.err, vh, vb);
        DbgError(tmp);
    } else {
        RapTransferAll(vb);
        RapTransferCompleted(wait);
    }
}
