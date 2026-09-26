#include "dolphin/types.h"

#pragma section ".data"

extern void* jumptable_8036BF80[];

extern u8 psSetGeneratorAngleRadiusScale[];

/* Auto-carved .data unit 0x8036BF80..0x8036BFA4 (1 object): the switch jump table of psSetGeneratorAngleRadiusScale. Carved out of data_80369D20.c when 0x8036BF20..0x8036BF80 moved to gs_dvd_candidate_80167040.c. */

void* jumptable_8036BF80[9] = {
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x4C),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x88),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x1CC),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x4C),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x4C),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x130),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0xE0),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0xE0),
    (void*)((u8*)psSetGeneratorAngleRadiusScale + 0x194),
};
