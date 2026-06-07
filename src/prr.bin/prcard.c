#include "prcard.h"
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <libetc.h>

s32 CardTestEvent(void);
s32 CardTestEventOnce(void);
void CardClearEvent(void);
s32 CardTestEventX(void);
void CardClearEventX(void);
void CardInit(s32 val);
void CardClose(void);
s32 CardGetFreeIdAt(s32 slot, s32 ext);
s32 CardGetTotalSize(s32 num);
s32 CardReadAt(s32 slot, s32 ext, char *fname, void *buf, s32 size);
s32 CardWriteAt(s32 slot, s32 ext, char *fname, void *buf, s32 size, BOOL newf);
s32 CardGetFirstFreeId(void);
BOOL CardFileExists(char *fname);
s32 CardFormatAt(s32 slot, s32 ext);
void CardInitHeader(CARD_HEADER *p, char *title);


extern long D_800826DC;
extern long D_800826E0;
extern long D_800826E4;
extern long D_800826E8;
extern long D_800826EC;
extern long D_800826F0;
extern long D_800826F4;
extern long D_800826F8;
extern long D_80082700;

extern struct DIRENTRY D_8008994C[15];

static s32 cardExt = 0;
static s32 cardSlot = 0;

const u8 cardHdrClut[32] = { 
    0x00, 0x00, 0xdf, 0x00, 0xdd, 0x00, 0xbb, 0x00,
    0x99, 0x01, 0xd9, 0x00, 0xb9, 0x00, 0xb2, 0x00,
    0x92, 0x00, 0x8f, 0x00, 0x8d, 0x00, 0x8c, 0x00,
    0x6b, 0x00, 0x69, 0x00, 0x47, 0x00, 0x66, 0x1a,
};
const u8 cardHdrIcon1[128] = { 
    0x00, 0x00, 0x00, 0xce, 0xdb, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xd0, 0xbc, 0xaa, 0x0b, 0x00, 0x00,
    0x00, 0x00, 0xcd, 0x6b, 0xa6, 0xea, 0x00, 0x00,
    0x00, 0x00, 0xbc, 0x22, 0x61, 0xba, 0x00, 0x00,
    0x00, 0xd0, 0x6c, 0x11, 0x11, 0xa6, 0x0c, 0x00,
    0x00, 0xc0, 0x29, 0x11, 0x11, 0x81, 0x0b, 0x00,
    0x00, 0xcd, 0x16, 0x11, 0x11, 0x61, 0xda, 0x00,
    0x00, 0xbc, 0x11, 0x11, 0x11, 0x11, 0xaa, 0x00,
    0x00, 0x6b, 0x11, 0x11, 0x11, 0x11, 0xb6, 0x00,
    0x00, 0x2b, 0x11, 0x11, 0x11, 0x11, 0xb1, 0x00,
    0x00, 0x1b, 0x11, 0x11, 0x11, 0x11, 0xb1, 0x00,
    0x00, 0x8b, 0x88, 0x88, 0x88, 0x88, 0xb8, 0x00,
    0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00,
    0x00, 0x1b, 0x11, 0xf5, 0x5f, 0x11, 0xb1, 0x00,
    0x00, 0x1b, 0x11, 0xf4, 0x4f, 0x11, 0xb1, 0x00,
    0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00,
};
const u8 cardHdrIcon2[128] = { 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xd0, 0xbb, 0xbb, 0x0e, 0x00, 0x00,
    0x00, 0x00, 0xbb, 0x68, 0x63, 0xbb, 0x00, 0x00,
    0x00, 0xc0, 0x2a, 0x11, 0x11, 0xa3, 0x0b, 0x00,
    0x00, 0xab, 0x13, 0x11, 0x11, 0x31, 0xba, 0x00,
    0xb0, 0x6b, 0x11, 0x11, 0x11, 0x11, 0xb6, 0x0b,
    0xbe, 0x18, 0x11, 0x11, 0x11, 0x11, 0x71, 0x0b,
    0xbb, 0x12, 0x11, 0x11, 0x11, 0x11, 0x21, 0xba,
    0x8b, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0xb8,
    0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
    0x1b, 0x11, 0x11, 0xf5, 0x5f, 0x11, 0x11, 0xb1,
    0x1b, 0x11, 0x11, 0xf4, 0x4f, 0x11, 0x11, 0xb1,
    0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb,
};
const u8 cardHdrIcon3[128] = { 
    0x00, 0x00, 0x00, 0xbc, 0xbb, 0x0d, 0x00, 0x00,
    0x00, 0x00, 0xb0, 0x6a, 0x62, 0xea, 0x00, 0x00,
    0x00, 0x00, 0xab, 0x13, 0x11, 0xc3, 0x0e, 0x00,
    0x00, 0xd0, 0x1b, 0x11, 0x11, 0x81, 0x0c, 0x00,
    0x00, 0xb0, 0x16, 0x11, 0x11, 0x21, 0xeb, 0x00,
    0x00, 0xbe, 0x11, 0x11, 0x11, 0x11, 0xc8, 0x00,
    0x00, 0x8b, 0x11, 0x11, 0x11, 0x11, 0xc3, 0x00,
    0x00, 0x6b, 0x11, 0x11, 0x11, 0x11, 0xb1, 0x00,
    0x00, 0x3b, 0x11, 0x11, 0x11, 0x11, 0xb1, 0x00,
    0x00, 0x8b, 0x88, 0x88, 0x88, 0x88, 0xb8, 0x00,
    0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00,
    0x00, 0x1b, 0x11, 0xf4, 0x4f, 0x11, 0xb1, 0x00,
    0x00, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


s32 CardTestEvent(void) {
    u32 unused;

    while (TRUE) {
        if (TestEvent(D_800826DC) == TRUE) {
            return CARD_EVENT_IOE;
        }
        if (TestEvent(D_800826E0) == TRUE) {
            return CARD_EVENT_ERROR;
        }
        if (TestEvent(D_800826E4) == TRUE) {
            return CARD_EVENT_TIMEOUT;
        }
        if (TestEvent(D_800826E8) == TRUE) {
            return CARD_EVENT_NEWCARD;
        }
    }
}

s32 CardTestEventOnce(void) {
    if (TestEvent(D_800826DC) == TRUE) {
        return CARD_EVENT_IOE;
    }
    if (TestEvent(D_800826E0) == TRUE) {
        return CARD_EVENT_ERROR;
    }
    if (TestEvent(D_800826E4) == TRUE) {
        return CARD_EVENT_TIMEOUT;
    }
    if (TestEvent(D_800826E8) == TRUE) {
        return CARD_EVENT_NEWCARD;
    }
    return 0;
}

void CardClearEvent(void) {
    TestEvent(D_800826DC);
    TestEvent(D_800826E0);
    TestEvent(D_800826E4);
    TestEvent(D_800826E8);
}

s32 CardTestEventX(void) {
    while (TRUE) {
        if (TestEvent(D_800826EC) == TRUE) {
            return CARD_EVENT_IOE;
        }
        if (TestEvent(D_800826F0) == TRUE) {
            return CARD_EVENT_ERROR;
        }
        if (TestEvent(D_800826F4) == TRUE) {
            return CARD_EVENT_TIMEOUT;
        }
        if (TestEvent(D_800826F8) == TRUE) {
            return CARD_EVENT_NEWCARD;
        }
    }
}

void CardClearEventX(void) {
    TestEvent(D_800826EC);
    TestEvent(D_800826F0);
    TestEvent(D_800826F4);
    TestEvent(D_800826F8);
}

void CardInit(s32 val) {
    InitCARD(val);
    StartCARD();
    _bu_init();
    ChangeClearPAD(0);

    EnterCriticalSection();
    D_800826DC = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
    D_800826E0 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
    D_800826E4 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
    D_800826E8 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);
    D_800826EC = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);
    D_800826F0 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);
    D_800826F4 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
    D_800826F8 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);
    ExitCriticalSection();
    
    EnableEvent(D_800826DC);
    EnableEvent(D_800826E0);
    EnableEvent(D_800826E4);
    EnableEvent(D_800826E8);
    EnableEvent(D_800826EC);
    EnableEvent(D_800826F0);
    EnableEvent(D_800826F4);
    EnableEvent(D_800826F8);
}

void CardClose(void) {
    EnterCriticalSection();
    CloseEvent(D_800826DC);
    CloseEvent(D_800826E0);
    CloseEvent(D_800826E4);
    CloseEvent(D_800826E8);
    CloseEvent(D_800826EC);
    CloseEvent(D_800826F0);
    CloseEvent(D_800826F4);
    CloseEvent(D_800826F8);
    ExitCriticalSection();
}

s32 CardGetFreeIdAt(s32 slot, s32 ext) {
    s32 i;
    char tmp[64];
    struct DIRENTRY *d;
    
    i = 0;
    sprintf(tmp, "bu%1d%1d:*", slot, ext);
    d = &D_8008994C[0];
    if (firstfile(tmp, d) == d) {
        do {
            i++;
            d++;
        } while (nextfile(d) == d);
    }
    return i;
}

s32 CardGetTotalSize(s32 num) {
    u32 unused;
    s32 i;
    s32 size;
    struct DIRENTRY *ent;
    
    size = 0;
    ent = &D_8008994C[0];
    for (i = 0; i < num; i++) {
        size += ent[i].size;
    }
    size /= 0x2000;
    return 15 - size;
}

const char D_800101B0[] = "bu%1d%1d:%s";

s32 CardReadAt(s32 slot, s32 ext, char *fname, void *buf, s32 size) {
    s32 ret;
    long fd;
    char tmp[64];
    s32 sizeb;
    s32 unk;

    unk = 8;
    sprintf(tmp, D_800101B0, slot, ext, fname);
    
    sizeb = size * 0x2000;
    fd = open(tmp, O_RDONLY | O_NOWAIT);
    if (fd != -1) {
        D_80082700 = fd;
        CardClearEvent();
        read(fd, buf, sizeb);
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}

s32 CardWriteAt(s32 slot, s32 ext, char *fname, void *buf, s32 size, BOOL newf) {
    s32 ret;
    u32 unused;
    long fd;
    char tmp[64];
    s32 sizeb;
    
    sprintf(tmp, D_800101B0, slot, ext, fname);
    printf("new: %d key: [%s]\n", newf, tmp);
    
    if (newf == TRUE) {
        fd = open(tmp, (size << 0x10) | O_CREAT);
        if (fd != -1) {
            close(fd);
        } else {
            return (ret = -1);
        }
    }

    sizeb = size * 0x2000;
    fd = open(tmp, O_WRONLY | O_NOWAIT);
    if (fd != -1) {
        D_80082700 = fd;
        CardClearEvent();
        write(fd, buf, sizeb);
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}

const char D_800101D0[] = "bu%1d%1d:";

void CardReset(void) {
    ResetCallback();
    PadInit(0);
    CardInit(1);
}

void CardEnd(void) {
    CardClose();
}

s32 CardGetStatus(void) {
    s32 event;

    _card_info(0);
    
    // Check connection
    event = CardTestEvent();
    switch (event) {
        case CARD_EVENT_IOE: {
            break;
        }
        case CARD_EVENT_TIMEOUT: {
            return event;
        }
        case CARD_EVENT_NEWCARD: {
            // New card
            CardClearEventX();
            _card_clear(0);
            event = CardTestEventX();
            break;
        }
        case CARD_EVENT_ERROR:
        default: {
            break;
        }
    }

    // Prepare card
    CardClearEvent();
    _card_load(0);
    event = CardTestEvent();
    switch (event) {
        case CARD_EVENT_IOE: {
            return event;
        }
        case CARD_EVENT_TIMEOUT: {
            return event;
        }
        case CARD_EVENT_NEWCARD: {
            return event;
        }
        default: {
            break;
        }
    }
    return event;
}

s32 CardGetFirstFreeId(void) {
    bzero(D_8008994C, sizeof(D_8008994C));
    return CardGetFreeIdAt(cardSlot, cardExt);
}

BOOL CardFileExists(char *fname) {
    struct DIRENTRY *ent;
    s32 i;
    s32 j;
    char dpname[32];

    ent = &D_8008994C[0];

    for (i = 0; i < 15; i++) {
        for (j = 0; (j < 21) && (ent[i].name[j] != '\0'); j++) {
            dpname[j] = ent[i].name[j];
        }
        dpname[j] = '\0';
        printf("fname: [%s]  dpname: [%s]\n", fname, dpname);
        if (strcmp(fname, dpname) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}


s32 CardRead(char *fname, void *buf, s32 size) {
    s32 event;

    CardReadAt(cardSlot, cardExt, fname, buf, size);
    event = CardTestEvent();
    close(D_80082700);
    if (event == CARD_EVENT_IOE) {
        return 0;
    } else {
        return -1;
    }
}

s32 CardWrite(char *fname, void *buf, s32 size) {
    BOOL newf;
    s32 event;

    newf = CardFileExists(fname) != TRUE;
    CardWriteAt(cardSlot, cardExt, fname, buf, size, newf);
    event = CardTestEvent();
    close(D_80082700);
    if (event == CARD_EVENT_IOE) {
        return 0;
    } else {
        return -1;
    }
}


void CardDeleteFile(char *fname) {
    char tmp[64];

    sprintf(tmp, D_800101B0, cardSlot, cardExt, fname);
    delete(tmp);
}

struct DIRENTRY *CardGetFiles(void) {
    return D_8008994C;
}

BOOL CardGetInfo(s32 *firstfree, s32 *size) {
    *firstfree = CardGetFirstFreeId();
    *size = CardGetTotalSize(*firstfree);
    return TRUE;
}

s32 CardFormat(void) {
    return CardFormatAt(cardSlot, cardExt);
}

void CardInitSaveDataHeader(CARD_DATA *data, char *title) {
    CardInitHeader(&data->header, title);
}

s32 CardFormatAt(register s32 slot, register s32 ext) {
    s32 event;
    char tmp[64];

    sprintf(tmp, D_800101D0, slot, ext);
    CardClearEventX();
    format(tmp);

    event = CardTestEventX();
    switch (event) {
        case CARD_EVENT_IOE: {
            return CARD_EVENT_IOE;
        }
        case CARD_EVENT_TIMEOUT: {
            return CARD_EVENT_TIMEOUT;
        }
        default: {
            return CARD_EVENT_ERROR;
        }
    }
}

#define CARD_TYPE_MAGIC 0x10

void CardInitHeader(register CARD_HEADER *p, register char *title) {
    CARD_HEADER hdr;
    s32 i;
    u8 pad[20];

    hdr.magic[0] = 'S';
    hdr.magic[1] = 'C';
    hdr.type     = CARD_TYPE_MAGIC | 3;
    hdr.slotnum  = 1;
    for (i = 0; i < 64; i++) {
        hdr.title[i] = '\0';
    }
    strcpy(hdr.title, title);
    for (i = 0; i < 28; i++) {
        hdr.pad[i] = 0;
    }
    memcpy(hdr.clut, cardHdrClut, 32);
    memcpy(hdr.icon1, cardHdrIcon1, 128);
    if (hdr.type > (CARD_TYPE_MAGIC | 1)) {
        memcpy(hdr.icon2, cardHdrIcon2, 128);
    }
    if (hdr.type > (CARD_TYPE_MAGIC | 2)) {
        memcpy(hdr.icon3, cardHdrIcon3, 128);
    }
    memcpy(p, &hdr, sizeof(*p));
}
