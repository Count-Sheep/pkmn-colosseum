/**
 * @file cardesavedata_exact_80083CBC.c
 * @brief fn_80083CBC / fn_80083CFC, 0x80083CBC - 0x80083D30.
 *
 * Function-boundary carve of the Card-e save-data TU (see cardesavedata.c):
 * the arena clear and the arena lookup, both defaulting to save-data status
 * block 0xD. No jump table, no pooled constant, no data. GC/1.3 -O4,p with
 * the TU's unit-wide -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);
extern void* savedataGetStatus(u32, u32);

/* Clear the Card-e arena (ptr, or the save-data block). */
void fn_80083CBC(void* ptr) {
    memset(ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD), 0, 0x49CC);
}

/* The Card-e arena: ptr, or the save-data block. */
void* fn_80083CFC(void* ptr) {
    return ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD);
}
