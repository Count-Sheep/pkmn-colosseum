/**
 * @file menuNameEntry_exact_80028588.c
 * @brief Name-entry selection phase callback.
 */
#include "dolphin/types.h"

extern void winSeqSetMenu(void* sequence, s32 menuId);

s32 menuNameEntrySelectCtrl(void* window)
{
    u8* work;
    s8 phase;

    work = window;
    phase = (s8)work[1];
    switch (phase) {
    case 0:
        if ((s8)work[2] == 0) {
            winSeqSetMenu(*(void**)(work + 4), 0x56);
            work[2] = 1;
        }
        break;
    case 3:
        if ((s8)work[2] == 0) {
            winSeqSetMenu(*(void**)(work + 4), 0x5A);
            work[2] = 1;
        }
        break;
    }
    return 0;
}
