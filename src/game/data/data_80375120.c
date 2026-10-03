#include "dolphin/types.h"

#pragma section ".data"

extern u8 fn_801DF790[];
extern u8 fn_801DFC30[];
extern u8 fn_801E03D4[];

/* Switch tables 0x80375120..0x803751EC, split from data_80372700.c after
 * game/gs_range_candidate_801DF474.c's jumptable_80375100. */

void* jumptable_80375120[16] = {
    (void*)((u8*)fn_801DF790 + 0x40),
    (void*)((u8*)fn_801DF790 + 0x5C),
    (void*)((u8*)fn_801DF790 + 0x78),
    (void*)((u8*)fn_801DF790 + 0x94),
    (void*)((u8*)fn_801DF790 + 0xBC),
    (void*)((u8*)fn_801DF790 + 0x130),
    (void*)((u8*)fn_801DF790 + 0x1A0),
    (void*)((u8*)fn_801DF790 + 0x1BC),
    (void*)((u8*)fn_801DF790 + 0x244),
    (void*)((u8*)fn_801DF790 + 0x2B0),
    (void*)((u8*)fn_801DF790 + 0x2CC),
    (void*)((u8*)fn_801DF790 + 0x2E4),
    (void*)((u8*)fn_801DF790 + 0x3E0),
    (void*)((u8*)fn_801DF790 + 0x3FC),
    (void*)((u8*)fn_801DF790 + 0x42C),
    (void*)((u8*)fn_801DF790 + 0x480),
};

void* jumptable_80375160[22] = {
    (void*)((u8*)fn_801DFC30 + 0x60),
    (void*)((u8*)fn_801DFC30 + 0x80),
    (void*)((u8*)fn_801DFC30 + 0xB0),
    (void*)((u8*)fn_801DFC30 + 0x120),
    (void*)((u8*)fn_801DFC30 + 0x13C),
    (void*)((u8*)fn_801DFC30 + 0x158),
    (void*)((u8*)fn_801DFC30 + 0x174),
    (void*)((u8*)fn_801DFC30 + 0x19C),
    (void*)((u8*)fn_801DFC30 + 0x2E4),
    (void*)((u8*)fn_801DFC30 + 0x300),
    (void*)((u8*)fn_801DFC30 + 0x31C),
    (void*)((u8*)fn_801DFC30 + 0x358),
    (void*)((u8*)fn_801DFC30 + 0x3B8),
    (void*)((u8*)fn_801DFC30 + 0x3E8),
    (void*)((u8*)fn_801DFC30 + 0x438),
    (void*)((u8*)fn_801DFC30 + 0x454),
    (void*)((u8*)fn_801DFC30 + 0x518),
    (void*)((u8*)fn_801DFC30 + 0x630),
    (void*)((u8*)fn_801DFC30 + 0x648),
    (void*)((u8*)fn_801DFC30 + 0x664),
    (void*)((u8*)fn_801DFC30 + 0x680),
    (void*)((u8*)fn_801DFC30 + 0x754),
};

void* jumptable_803751B8[13] = {
    (void*)((u8*)fn_801E03D4 + 0x40),
    (void*)((u8*)fn_801E03D4 + 0x70),
    (void*)((u8*)fn_801E03D4 + 0x8C),
    (void*)((u8*)fn_801E03D4 + 0xC8),
    (void*)((u8*)fn_801E03D4 + 0x104),
    (void*)((u8*)fn_801E03D4 + 0x178),
    (void*)((u8*)fn_801E03D4 + 0x1B8),
    (void*)((u8*)fn_801E03D4 + 0x20C),
    (void*)((u8*)fn_801E03D4 + 0x244),
    (void*)((u8*)fn_801E03D4 + 0x284),
    (void*)((u8*)fn_801E03D4 + 0x2D8),
    (void*)((u8*)fn_801E03D4 + 0x318),
    (void*)((u8*)fn_801E03D4 + 0x358),
};
