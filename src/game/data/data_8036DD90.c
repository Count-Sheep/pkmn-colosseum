#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_8036DD90[];
extern void* jumptable_8036DDD4[];
extern void* jumptable_8036DE1C[];

extern u8 fn_801C8834[];
extern u8 fn_801C89F8[];

/* Auto-carved .data unit 0x8036DD90..0x8036DE6C (3 objects; the 0x8036DCB8 tables belong to gs_exact_801C84FC). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* jumptable_8036DD90[17] = {
    (void*)((u8*)fn_801C8834 + 0xCC),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xD4),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xDC),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xE4),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xEC),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xF4),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0xFC),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0x104),
    (void*)((u8*)fn_801C8834 + 0x114),
    (void*)((u8*)fn_801C8834 + 0x10C),
};

void* jumptable_8036DDD4[18] = {
    (void*)((u8*)fn_801C8834 + 0x48),
    (void*)((u8*)fn_801C8834 + 0x48),
    (void*)((u8*)fn_801C8834 + 0x50),
    (void*)((u8*)fn_801C8834 + 0x50),
    (void*)((u8*)fn_801C8834 + 0x58),
    (void*)((u8*)fn_801C8834 + 0x58),
    (void*)((u8*)fn_801C8834 + 0x60),
    (void*)((u8*)fn_801C8834 + 0x60),
    (void*)((u8*)fn_801C8834 + 0x68),
    (void*)((u8*)fn_801C8834 + 0x68),
    (void*)((u8*)fn_801C8834 + 0x70),
    (void*)((u8*)fn_801C8834 + 0x70),
    (void*)((u8*)fn_801C8834 + 0x78),
    (void*)((u8*)fn_801C8834 + 0x78),
    (void*)((u8*)fn_801C8834 + 0x80),
    (void*)((u8*)fn_801C8834 + 0x80),
    (void*)((u8*)fn_801C8834 + 0x88),
    (void*)((u8*)fn_801C8834 + 0x88),
};

void* jumptable_8036DE1C[20] = {
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0xEC),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x11C),
    (void*)((u8*)fn_801C89F8 + 0x14C),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x17C),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x1B8),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x1F4),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x23C),
    (void*)((u8*)fn_801C89F8 + 0x254),
    (void*)((u8*)fn_801C89F8 + 0x26C),
    (void*)((u8*)fn_801C89F8 + 0x2B0),
    (void*)((u8*)fn_801C89F8 + 0x358),
    (void*)((u8*)fn_801C89F8 + 0x320),
};

