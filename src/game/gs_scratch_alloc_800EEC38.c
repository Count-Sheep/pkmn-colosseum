/**
 * GSscratchAlloc (0x800EEC38-0x800EEDF8), part of the GSscratch TU
 * (0x800EE928-0x800EEF48, -O4,p). Kept as its own object so the exact
 * functions before it can link.
 *
 * Status: 99.06%. The only difference left is a register swap: retail keeps
 * the free-record pointer in r7 and the marking loop's shift counter in r6,
 * this source gets them the other way round. A search over all 5040
 * declaration orders of the locals and the natural variants of the record
 * search/marking code did not move it; the one source known to swap them
 * back is a single-use wrapper around GSscratchFindAllocation(0xFF), which
 * is register shaping and is not used.
 *
 * GSscratchFindAllocation is also expanded in GSscratchFree, and
 * GSscratchMarkBlocks in GSscratchInit.
 */
#include "dolphin/types.h"

typedef struct GSscratchAllocation {
    u8 firstBlock; /* 0xFF: record unused */
    u8 blockCount;
    u8 _pad[2];
    void (*callback)(BOOL valid, void *ptr, u8 blockCount);
} GSscratchAllocation;

extern GSscratchAllocation lbl_804018F0[32]; /* allocation records */
extern u8 *lbl_8047ABE0;                     /* scratch base (0xE0000000) */
extern u8 lbl_8047ABE8;                      /* 1: scratch contents invalid */
extern u32 lbl_8047ABEC;                     /* used-block bitmap */

static inline GSscratchAllocation *GSscratchFindAllocation(u8 firstBlock)
{
    GSscratchAllocation *allocation;
    u32 i;

    allocation = lbl_804018F0;
    for (i = 0; i < 32; allocation++, i++) {
        if (allocation->firstBlock == firstBlock) {
            return allocation;
        }
    }
    return NULL;
}

static inline void GSscratchMarkBlocks(u8 firstBlock, u8 blockCount)
{
    u32 mask;
    u32 used;

    mask = 0x80000000;
    while (firstBlock-- != 0) {
        mask >>= 1;
    }
    used = lbl_8047ABEC;
    while (blockCount-- != 0) {
        used |= mask;
        mask >>= 1;
    }
    lbl_8047ABEC = used;
}

void *GSscratchAlloc(u8 blockCount,
                     void (*callback)(BOOL valid, void *ptr, u8 blockCount))
{
    u32 scanMask;
    u32 occupied;
    u8 shiftCount;
    u8 remaining;
    u32 usedMask;
    u8 firstBlock;
    GSscratchAllocation *allocation;

    if (lbl_8047ABE8 == 1) {
        return NULL;
    }
    if (blockCount == 0 || blockCount > 32) {
        return NULL;
    }

    /* First fit: blocks past the end count as occupied. */
    usedMask = lbl_8047ABEC;
    for (firstBlock = 0; firstBlock < 32; firstBlock++) {
        remaining = blockCount;
        shiftCount = firstBlock;
        occupied = 0;
        scanMask = 0x80000000;
        while (shiftCount-- != 0) {
            scanMask >>= 1;
        }
        while (remaining-- != 0) {
            occupied <<= 1;
            if (scanMask != 0) {
                if ((usedMask & scanMask) != 0) {
                    occupied |= 1;
                }
                scanMask >>= 1;
            } else {
                occupied |= 1;
            }
        }
        if (occupied == 0) {
            allocation = GSscratchFindAllocation(0xFF);
            if (allocation == NULL) {
                return NULL;
            }
            allocation->firstBlock = firstBlock;
            allocation->blockCount = blockCount;
            allocation->callback = callback;
            GSscratchMarkBlocks(firstBlock, blockCount);
            return lbl_8047ABE0 + allocation->firstBlock * 0x200;
        }
    }
    return NULL;
}
