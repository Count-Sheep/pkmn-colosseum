/**
 * @file menu_middle_range_8006AF44.c
 * @brief menu-middle unit, 0x8006AF44 - 0x8006AFC4 (fn_8006AF44).
 *
 * Part of menuCB_Bios.c (0x8006A65C - 0x8006B6B4: the functions that use
 * its rodata, lbl_80267DD8 and the "menuCB_Bios.c" assert block at
 * 0x80267DE8). Its units build with one flag set, GC/1.3, -O4,p,
 * "-opt nopeephole", and the local peephole/scheduling pragmas that part of
 * menu_middle.c carried are gone. Evidence:
 * - every exact function in the range stays exact with no pragma:
 *   fn_8006A65C - fn_8006A81C, fn_8006A824, menuCBBios_InitTrainer,
 *   fn_8006AC6C, fn_8006ADB4, fn_8006ADEC, fn_8006AEEC, fn_8006AFC4,
 *   fn_8006B09C, fn_8006B0F8, fn_8006B1C0 - fn_8006B1F4, fn_8006B354,
 *   fn_8006B3C8, fn_8006B4AC - fn_8006B5A8 (ten of them used to need
 *   "peephole off" and six "scheduling off");
 * - fn_8006AF44 (93.4) and fn_8006AE18 (97.7) become exact, and every
 *   candidate gains or holds: fn_8006A990 79.3 -> 83.6, fn_8006AABC
 *   67.2 -> 71.7, fn_8006ACCC 90.5 -> 94.0, fn_8006B420 94.4 -> 94.7,
 *   fn_8006B5D0 85.3 (GC/2.0) -> 93.9.
 */
#define MENU_MIDDLE_RESIDUAL_8006AF44_ONLY
#include "menu_middle.c"
