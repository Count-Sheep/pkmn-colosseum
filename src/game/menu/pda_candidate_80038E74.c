/** PDA menu button callback, 0x80038E74 - 0x80039004. */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x4];
    s8 flags;
    u8 pad05;
    s16 eventId;
    u8 pad08[0x44];
    s32 messageId;
    s16 field_50;
    s16 field_52;
    s16 x;
    s16 y;
    u8 pad58[0xc];
    u8 colorR;
    u8 colorG;
    u8 colorB;
    u8 alpha;
    u8 pad68[0x8];
    f32 value;
    u8 pad74[0x17];
    u8 alphaByte;
    u8 pad8c[9];
    s8 selectedIndex;
} PdaSprite;
extern u32 menuGetCursorItemID(s32 menu);
extern f32 lbl_803A65B0[8][3];
typedef struct PdaMenuRecord {
    s32 eventId;
    u8 pad04[0x10];
    u8 enabled;
    u8 pad15[3];
} PdaMenuRecord;
extern const PdaMenuRecord lbl_80267060[8];

/* Position and tint one of the PDA's eight main menu buttons. */
/* RULE-EXCEPTION(user-approved): peephole control; see docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
s32 fn_80038E74(void* work, PdaSprite* sprite)
{
    s32 i;
    u32 enabled;

    (void)work;
    for (i = 0; i < 8; i++) {
        if (sprite->eventId == lbl_80267060[i].eventId) {
            break;
        }
    }
    if (i >= 8) {
        return 0;
    }
    {
        /* RULE-EXCEPTION(user-approved): volatile load shapes conversion/allocation;
         * see docs/RULE_EXCEPTIONS.md. */
        s16 x = *(volatile f32*)((u8*)lbl_803A65B0 + i * 0xc);
        f32* pos = lbl_803A65B0[i];
        enabled = lbl_80267060[i].enabled;
        sprite->field_50 = x;
        sprite->field_52 = pos[1];
        sprite->alpha = pos[2];
    }
    if (enabled != 0) {
        if (sprite->eventId == (s32)menuGetCursorItemID(0x24)) {
            sprite->colorR = 0xff;
            sprite->colorG = 0xff;
            sprite->colorB = 0xff;
        } else {
            sprite->colorR = 0x46;
            sprite->colorG = 0x8f;
            sprite->colorB = 0xb4;
        }
    }
    return 0;
}
/* RULE-EXCEPTION(user-approved): restore peephole control; see docs/RULE_EXCEPTIONS.md. */
#pragma peephole reset
