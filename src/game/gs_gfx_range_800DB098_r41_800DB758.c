/**
 * @file gs_gfx_range_800DB098_r41_800DB758.c
 * @brief Candidate for fn_800DB758 (display-list capture primitive header),
 * 0x800DB758 - 0x800DB890.
 *
 * Text-only candidate. fn_800DB758's switch uses jumptable_80315364, which
 * sits in the middle of the GSgfx dl TU's .data (after the
 * _dlParseSurface/_dlParseVertex tables), so it can only be linked with
 * that whole TU (0x800DA578 - 0x800DB890).
 */

#include "dolphin/types.h"

extern u32 lbl_8047AA80;
extern void fn_800D6A80(u16, s32, u32*, u32*);

void fn_800DB758(u16 vertCount)
{
    u32 state;
    u32 obj;
    u8* p;
    u16* p16;

    state = lbl_8047AA80;
    if (*(s32*)(state + 0x488) == 7) {
        vertCount = (vertCount & 0x7FFF) << 1;
    }

    obj = *(u32*)(state + 0x480);
    fn_800D6A80(vertCount, *(u32*)(state + 0x488),
                (u32*)(obj + 0x10), (u32*)(obj + 0x14));

    state = lbl_8047AA80;
    switch (*(u32*)(state + 0x488)) {
        case 0:
            *(u8*)*(u32*)(state + 0x484) = 0xB8;
            break;
        case 1:
            *(u8*)*(u32*)(state + 0x484) = 0xA8;
            break;
        case 2:
            *(u8*)*(u32*)(state + 0x484) = 0xB0;
            break;
        case 3:
            *(u8*)*(u32*)(state + 0x484) = 0x90;
            break;
        case 4:
            *(u8*)*(u32*)(state + 0x484) = 0x98;
            break;
        case 5:
            *(u8*)*(u32*)(state + 0x484) = 0xA0;
            break;
        case 6:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
        case 7:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
    }

    state = lbl_8047AA80;
    p = *(u8**)(state + 0x484);
    *(u32*)(state + 0x484) = (u32)(p + 1);
    obj = *(u32*)(state + 0x480);
    *p = (u8)(*p | *(u32*)(*(u32*)(obj + 0xC) + 4));

    state = lbl_8047AA80;
    p16 = *(u16**)(state + 0x484);
    *p16++ = vertCount;
    state = lbl_8047AA80;
    *(u32*)(state + 0x484) = (u32)p16;
}
