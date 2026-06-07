#ifndef _PRINT_H_
#define _PRINT_H_

#include "common.h"

#define INT_BLOCK_TYPE_VRAM 1
#define INT_BLOCK_TYPE_SND  2
#define INT_BLOCK_TYPE_MEM  3
#define INT_BLOCK_TYPE_END  -1

typedef struct INT_BLOCK_HEADER {
    s32 type;
    s32 filenum;
    s32 size; // In sectors
    s32 bytesize; // In bytes
} INT_BLOCK_HEADER;

typedef struct INT_FILE_HEADER {
    s32 size;
    char name[16];
} INT_FILE_HEADER;


#endif
