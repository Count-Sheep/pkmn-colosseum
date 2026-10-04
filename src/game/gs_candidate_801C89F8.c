/* gs_range_801C766C split, 0x801C89F8-0x801C8DD0: fn_801C89F8 (exact). Its switch table (in data_8036DD90.c) starts
   4-aligned at 0x8036DE1C and compiled .data is 8-aligned, so this unit links from the extracted object until
   it can share a unit with fn_801C8834 (0x801C8834-0x801C89F8), whose tables precede it. */
#define GS_801C766C_SPLIT
#define GS_801C89F8_ONLY
#include "src/game/gs_range_801C766C.c"
