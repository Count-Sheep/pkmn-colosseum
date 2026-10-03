#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_802EE06C[];

extern u8 fn_8006F720[];

/* Carved .data unit 0x802EE06C..0x802EE0F0: fn_8006F720's switch table. It
 * starts 4-aligned, so the compiled table cannot link from its own unit. */
void* jumptable_802EE06C[33] = {
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x88),
    (void*)((u8*)fn_8006F720 + 0x90),
    (void*)((u8*)fn_8006F720 + 0x98),
    (void*)((u8*)fn_8006F720 + 0xA0),
    (void*)((u8*)fn_8006F720 + 0xA8),
    (void*)((u8*)fn_8006F720 + 0xB0),
    (void*)((u8*)fn_8006F720 + 0xB8),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0xC0),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x80),
    (void*)((u8*)fn_8006F720 + 0x88),
    (void*)((u8*)fn_8006F720 + 0x88),
    (void*)((u8*)fn_8006F720 + 0x88),
};
