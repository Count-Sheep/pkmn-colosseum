/* gs_range_801C766C split, 0x801C8834-0x801C8DD0: fn_801C8834 (with the itemToPokemonId inline) and fn_801C89F8.
   One unit, because their three switch tables are one .data run 0x8036DD90-0x8036DE6C: fn_801C89F8's table starts
   4-aligned at 0x8036DE1C, right after fn_801C8834's two, and a compiled .data section is 8-aligned. */
#define GS_801C766C_SPLIT
#define GS_801C8834_ONLY
#define GS_801C89F8_ONLY
#include "src/game/gs_range_801C766C.c"
