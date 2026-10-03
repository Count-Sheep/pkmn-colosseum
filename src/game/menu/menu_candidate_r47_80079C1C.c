/*
 * Linked carve of the menu range bucket: fn_80079C1C .. fn_8007B114
 * (0x80079C1C-0x8007B350), the coupon/ex-disc and GBA link-prompt tail.
 * Owns its own .data (the pooled game-name string and fn_8007AB10's four
 * switch tables, 0x802EE508-0x802EE604) and its int->float biases
 * (.sdata2 0x8047C118-0x8047C128).
 */
#define MENU_R47_80079C1C_ONLY
#include "src/game/menu/menu_range_8007109C.c"
