#include "common.h"

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryInit);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryClear);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryAlloc);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryFree);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryEndAlloc);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryGetInfo);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemorySetPos);

INCLUDE_ASM("asm/prr.bin/nonmatchings/prmemory", MemoryGetPos);

void MemoryZero(u8 *ptr, s32 size) {
    for (size; size > 0; size--) {
        *ptr++ = 0;
    }
}
