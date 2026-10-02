#include "dolphin/types.h"

extern f32 lbl_8047CACC;

/* RULE-EXCEPTION(user-approved): local scheduling control -- see docs/RULE_EXCEPTIONS.md. */
#pragma scheduling off
void GSmaterialStoreAlpha(u8* obj)
{
    f32 scale = lbl_8047CACC;

    obj[0x1] =
        (u8)(s32)(scale * *(f32*)(*(u32*)(*(u32*)(obj + 0x8) + 0xc) + 0xc));
}
#pragma scheduling on
