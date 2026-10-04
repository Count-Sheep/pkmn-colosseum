/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when this file is merged back into one unit — see docs/RULE_EXCEPTIONS.md */
/** GSmaterialResetAlpha, 0x800DF140 - 0x800DF188. */
#include "dolphin/types.h"

void GSmaterialResetAlpha(u8* obj) {
    extern void HSD_MObjSetAlpha(u32, f32);
    HSD_MObjSetAlpha(*(u32*)(obj + 0x8), (f32)obj[0x1] / 255.0f);
}
