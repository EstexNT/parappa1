#ifndef _PRCARD_H_
#define _PRCARD_H_

#include "common.h"
#include <kernel.h>
#include "prevent.h"


#define CARD_EVENT_IOE     1
#define CARD_EVENT_ERROR   2
#define CARD_EVENT_TIMEOUT 3
#define CARD_EVENT_NEWCARD 4


typedef struct CARD_HEADER {
    u8 magic[2];
    u8 type;
    u8 slotnum;
    char title[64];
    u8 pad[28];
    u8 clut[32];
    u8 icon1[128];
    u8 icon2[128];
    u8 icon3[128];
} CARD_HEADER;

typedef struct CARD_DATA {
    CARD_HEADER header;
    SAVE_DATA data;
    u8 pad[2804]; // Pads to make the size be 0x2000 (one card slot)
} CARD_DATA;


void CardReset(void);
void CardEnd(void);
s32 CardGetStatus(void);
s32 CardRead(char *fname, void *buf, s32 size);
s32 CardWrite(char *fname, void *buf, s32 size);
void CardDeleteFile(char *fname);
struct DIRENTRY *CardGetFiles(void);
BOOL CardGetInfo(s32 *firstfree, s32 *size);
s32 CardFormat(void);
void CardInitSaveDataHeader(CARD_DATA *data, char *title);


#endif
