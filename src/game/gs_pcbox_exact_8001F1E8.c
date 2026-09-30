/**
 * @file gs_pcbox_exact_8001F1E8.c
 * @brief Title menu button callback, 0x8001F1E8 - 0x8001F304.
 *
 * One-function unit of the title TU, next to fn_8001EF78 (menu 0x15's
 * tick, gs_pcbox_exact_8001EF78.c) and fn_8001F304 (the item draw
 * callback, gs_pcbox_exact_8001F304.c). Text only, data-free: the title state word lbl_8047A31C and the flag
 * lbl_8047A328 are .sbss owned elsewhere. Built with -opt nopeephole, which
 * keeps retail's `andi.` + `cmplwi` pairs.
 *
 * Before the title state reaches 4 the callback disables the debug menu and
 * waits for A/START (0x1100) or B (0x200): either one advances the state to
 * 4, plays sound 0x46E and raises lbl_8047A328. From state 4 on it enables
 * the debug menu and marks the item (offset 0x98) when the window key info
 * reports 0x810.
 */
#include "dolphin/types.h"

extern u32 lbl_8047A31C;
extern u32 lbl_8047A328;

extern u32 fn_800F7AF0(s32 pad);
extern u32 fn_800F7BC4(s32 pad);
extern u8* windowGetKeyInfo(void);
extern void dbgMenuSetEnable(s32 enable);
extern void fn_801669E4(s32 id, s32 a, s32 b);
extern void fn_80166AB8(s32 id, s32 a, s32 b);
extern void* menuDataBiosGetPtr(u32 id);

void fn_8001F1E8(u8* arg)
{
    u32 a;
    u32 b;
    u8* obj;

    if ((s32)lbl_8047A31C < 4) {
        dbgMenuSetEnable(0);
        if (arg == 0) return;
        menuDataBiosGetPtr(*(u32*)(arg + 0x4));
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if ((b & a) & 0x1100) {
            lbl_8047A31C = 4;
            fn_801669E4(0x46e, 0, 0);
            lbl_8047A328 = 1;
        }
        a = fn_800F7AF0(1);
        b = fn_800F7BC4(1);
        if (((b & a) & 0x200) == 0) return;
        lbl_8047A31C = 4;
        fn_801669E4(0x46e, 0, 0);
        lbl_8047A328 = 1;
        return;
    }
    dbgMenuSetEnable(1);
    if (arg == 0) return;
    menuDataBiosGetPtr(*(u32*)(arg + 0x4));
    obj = windowGetKeyInfo();
    if ((*(u16*)(obj + 0x4) & 0x810) == 0) return;
    *(u8*)(arg + 0x98) = 1;
    fn_80166AB8(0x4c2, 0, 0);
}
