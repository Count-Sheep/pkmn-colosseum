/**
 * GSscratchAlloc (0x800EEC38-0x800EEDF8), part of the GSscratch TU
 * (0x800EE928-0x800EEF48, -O4,p). Kept as its own object so the exact
 * functions before it can link.
 *
 * Status: 99.06%, permanent under the strict policy. The only difference is
 * a register swap: retail keeps the free-record pointer in r7 and the
 * marking loop's shift counter in r6; this source gets them the other way
 * round. Everything else (instructions, order, constants) is identical.
 *
 * The one source that reproduces the swap is a single-use wrapper
 * GSscratchFindFreeAllocation(void) { return GSscratchFindAllocation(0xFF); }
 * (an extra inline level whose only effect is the colouring). Nothing in the
 * target admits it: the free-record search occurs once in the binary (the
 * records at lbl_804018F0 are only touched by this TU; Free searches by
 * block, Init only clears), and the expansion carries no inline fingerprint
 * (no stale-CR branch, no routed return temp; the not-found path is a plain
 * li r7,0 / cmplwi r7,0). Ruled out, all at or below 99.06% with the other
 * six TU functions kept exact:
 * - compilers GC/1.2.5n, 1.3, 1.3.2, 2.0, 2.5, 2.6, 2.7 x -O4,p / -O4,s /
 *   -opt nopeephole / -inline deferred (only 1.3-2.7 at -O4,p, with or
 *   without deferred, keep the six exact; all give the same 99.06%), plus
 *   -inline smart, -O3,p/s, -O2,p and -opt noschedule on 1.3/2.0/2.7;
 *   also as C++ (extern "C", GSscratchNotify enum callback like GSgfx's
 *   _gfxScratchNotify__F15GSscratchNotifyPvUc) on 1.3/2.0/2.6;
 * - all 5040 declaration orders of the locals, with the marking loop both
 *   as GSscratchMarkBlocks and written out;
 * - types: bitmap/masks s32 vs u32, loop counter s32/u32, blockCount int,
 *   helper parameters int/u32, sentinel as 0xFF/-1/255/(u8)/enum/#define/
 *   const, u8* return, C++ declarations at first use;
 * - structure: record fill order, marking before the fill, marking via
 *   allocation->firstBlock, return from firstBlock, merged guards, while
 *   instead of for, usedMask reloaded per block, record search inlined or
 *   written with break/goto/index/countdown, a GSscratchBlockMask helper
 *   (expanded in Free, Alloc and Init) in any combination;
 * - TU order: Alloc before Free, first, after Init, fully reversed, helpers
 *   defined last with deferred inlining.
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
