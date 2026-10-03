#include "dolphin/types.h"

#pragma section ".data"

extern u8 pad_05_802EDE54_data[];
extern u8 lbl_802EDE58[];
extern void* jumptable_802EDE78[];
extern void* jumptable_802EDEFC[];
extern void* jumptable_802EDF20[];
extern void* jumptable_802EE0F0[];
extern void* jumptable_802EE20C[];
extern void* jumptable_802EE31C[];

extern u8 fn_8006C164[];
extern u8 fn_8006C7D4[];
extern u8 fn_800704AC[];
extern u8 fn_800706C4[];
extern u8 fn_80070A9C[];

/* Auto-carved .data unit 0x802EDE54..0x802EDFB0 (5 objects). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

u8 pad_05_802EDE54_data[4] = {
    0x00, 0x00, 0x00, 0x00,
};

u8 lbl_802EDE58[32] = {
    0x00, 0x00, 0x3B, 0xEB, 0x00, 0x00, 0x3B, 0xF0, 0x00, 0x00, 0x3B, 0xF3,
    0x00, 0x00, 0x3B, 0xF5, 0x00, 0x00, 0x3B, 0xF7, 0x00, 0x00, 0x3B, 0xF8,
    0x00, 0x00, 0x41, 0xFD, 0x00, 0x00, 0x3B, 0xF9,
};

void* jumptable_802EDE78[33] = {
    (void*)((u8*)fn_8006C164 + 0x2BC),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x32C),
    (void*)((u8*)fn_8006C164 + 0x350),
    (void*)((u8*)fn_8006C164 + 0x3C8),
    (void*)((u8*)fn_8006C164 + 0x3EC),
    (void*)((u8*)fn_8006C164 + 0x410),
    (void*)((u8*)fn_8006C164 + 0x434),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x458),
    (void*)((u8*)fn_8006C164 + 0x2D8),
    (void*)((u8*)fn_8006C164 + 0x2E4),
    (void*)((u8*)fn_8006C164 + 0x2E4),
    (void*)((u8*)fn_8006C164 + 0x2E4),
    (void*)((u8*)fn_8006C164 + 0x2E4),
    (void*)((u8*)fn_8006C164 + 0x2CC),
    (void*)((u8*)fn_8006C164 + 0x2D8),
    (void*)((u8*)fn_8006C164 + 0x2E4),
    (void*)((u8*)fn_8006C164 + 0x2CC),
};

void* jumptable_802EDEFC[9] = {
    (void*)((u8*)fn_8006C164 + 0x17C),
    (void*)((u8*)fn_8006C164 + 0x180),
    (void*)((u8*)fn_8006C164 + 0x184),
    (void*)((u8*)fn_8006C164 + 0x1AC),
    (void*)((u8*)fn_8006C164 + 0x1B0),
    (void*)((u8*)fn_8006C164 + 0x1B4),
    (void*)((u8*)fn_8006C164 + 0x1DC),
    (void*)((u8*)fn_8006C164 + 0x1E0),
    (void*)((u8*)fn_8006C164 + 0x1E4),
};

void* jumptable_802EDF20[36] = {
    (void*)((u8*)fn_8006C7D4 + 0x48),
    (void*)((u8*)fn_8006C7D4 + 0x54),
    (void*)((u8*)fn_8006C7D4 + 0x60),
    (void*)((u8*)fn_8006C7D4 + 0x6C),
    (void*)((u8*)fn_8006C7D4 + 0x78),
    (void*)((u8*)fn_8006C7D4 + 0x84),
    (void*)((u8*)fn_8006C7D4 + 0x90),
    (void*)((u8*)fn_8006C7D4 + 0x9C),
    (void*)((u8*)fn_8006C7D4 + 0xA8),
    (void*)((u8*)fn_8006C7D4 + 0xB4),
    (void*)((u8*)fn_8006C7D4 + 0xC0),
    (void*)((u8*)fn_8006C7D4 + 0xCC),
    (void*)((u8*)fn_8006C7D4 + 0xD8),
    (void*)((u8*)fn_8006C7D4 + 0xE4),
    (void*)((u8*)fn_8006C7D4 + 0xF0),
    (void*)((u8*)fn_8006C7D4 + 0xFC),
    (void*)((u8*)fn_8006C7D4 + 0x108),
    (void*)((u8*)fn_8006C7D4 + 0x114),
    (void*)((u8*)fn_8006C7D4 + 0x120),
    (void*)((u8*)fn_8006C7D4 + 0x12C),
    (void*)((u8*)fn_8006C7D4 + 0x138),
    (void*)((u8*)fn_8006C7D4 + 0x144),
    (void*)((u8*)fn_8006C7D4 + 0x150),
    (void*)((u8*)fn_8006C7D4 + 0x15C),
    (void*)((u8*)fn_8006C7D4 + 0x168),
    (void*)((u8*)fn_8006C7D4 + 0x174),
    (void*)((u8*)fn_8006C7D4 + 0x180),
    (void*)((u8*)fn_8006C7D4 + 0x18C),
    (void*)((u8*)fn_8006C7D4 + 0x198),
    (void*)((u8*)fn_8006C7D4 + 0x1A4),
    (void*)((u8*)fn_8006C7D4 + 0x1B0),
    (void*)((u8*)fn_8006C7D4 + 0x1BC),
    (void*)((u8*)fn_8006C7D4 + 0x1C8),
    (void*)((u8*)fn_8006C7D4 + 0x1D4),
    (void*)((u8*)fn_8006C7D4 + 0x1E0),
    (void*)((u8*)fn_8006C7D4 + 0x1EC),
};

/* 0x802EE0F0..0x802EE454 (jumptable_802EE0F0, jumptable_802EE20C and
 * jumptable_802EE31C) is game/menu/menu_middle_r47_prefix.c's own .data. */
