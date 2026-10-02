/** Exact menuCB state accessors, 0x800566B4 - 0x80056704. */
#include "dolphin/types.h"

extern u32 lbl_8047A56C;
extern f32 lbl_8047A570;
extern f32 lbl_8047A578;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BEC4;

u32 fn_800566B4(void)
{
    return !(lbl_8047A570 >= lbl_8047BEC4);
}

void fn_800566D8(u32 a)
{
    lbl_8047A56C = a;
    lbl_8047A570 = lbl_8047BEC0;
}

u32 fn_800566E8(void)
{
    return lbl_8047BEC0 != lbl_8047A578;
}
