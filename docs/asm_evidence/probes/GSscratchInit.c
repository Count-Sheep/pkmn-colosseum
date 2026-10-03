/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_scratch.md).
 * C cannot express the final step, which moves the stack into the locked cache
 * and returns to the caller's saved LR, so the candidate ends at the
 * bookkeeping that precedes it. */
#include "dolphin/types.h"

typedef struct GSscratchAllocation {
    u8 firstBlock;
    u8 blockCount;
    u8 _pad[2];
    void (*callback)(BOOL valid, void *ptr, u8 blockCount);
} GSscratchAllocation;

extern GSscratchAllocation lbl_804018F0[32];
extern u8 *lbl_8047ABE0;
extern u8 lbl_8047ABE8;
extern u32 lbl_8047ABEC;
extern u32 lbl_8047ABD8;
extern u32 lbl_8047ABE4;

extern void LCEnable(void);

BOOL GSscratchInit(u8 reservedBlocks)
{
    u32 mask;
    u32 i;
    u8 shift;
    u8 count;

    lbl_8047ABEC = 0;
    for (i = 0; i < 32; i++) {
        lbl_804018F0[i].firstBlock = 0xFF;
    }

    LCEnable();
    lbl_8047ABD8 = reservedBlocks << 9;
    lbl_8047ABE0 = (u8*)0xE0000000;
    if (reservedBlocks != 0) {
        shift = 0;
        mask = 0x80000000;
        while (shift-- != 0) {
            mask >>= 1;
        }
        count = reservedBlocks;
        while (count-- != 0) {
            lbl_8047ABEC |= mask;
            mask >>= 1;
        }
        lbl_804018F0[0].firstBlock = 0;
        lbl_804018F0[0].blockCount = reservedBlocks;
    }
    lbl_8047ABE8 = 0;
    return TRUE;
}
