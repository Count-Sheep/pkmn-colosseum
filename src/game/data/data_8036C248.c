#include "dolphin/types.h"

#pragma section ".data"

extern u8 lbl_8036C248[];
extern void* jumptable_8036C254[];
extern void* jumptable_8036C278[];

extern u8 cameraUpdate[];

/* Auto-carved .data unit 0x8036C248..0x8036C29C (3 objects), split from data_8036BFC0.c around generator.c's jump tables. Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

u8 lbl_8036C248[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void* jumptable_8036C254[9] = {
    (void*)((u8*)cameraUpdate + 0x7C4),
    (void*)((u8*)cameraUpdate + 0x89C),
    (void*)((u8*)cameraUpdate + 0x89C),
    (void*)((u8*)cameraUpdate + 0xA6C),
    (void*)((u8*)cameraUpdate + 0xBC8),
    (void*)((u8*)cameraUpdate + 0xA60),
    (void*)((u8*)cameraUpdate + 0xA54),
    (void*)((u8*)cameraUpdate + 0xAF4),
    (void*)((u8*)cameraUpdate + 0xAE4),
};

void* jumptable_8036C278[9] = {
    (void*)((u8*)cameraUpdate + 0x6A0),
    (void*)((u8*)cameraUpdate + 0x6A0),
    (void*)((u8*)cameraUpdate + 0x79C),
    (void*)((u8*)cameraUpdate + 0x79C),
    (void*)((u8*)cameraUpdate + 0x714),
    (void*)((u8*)cameraUpdate + 0x6A0),
    (void*)((u8*)cameraUpdate + 0x79C),
    (void*)((u8*)cameraUpdate + 0x6A0),
    (void*)((u8*)cameraUpdate + 0x79C),
};

