/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
/**
 * @file pda_exact_8003F464.c
 * @brief PDA summary-page widget layout and visibility, 0x8003F464 - 0x80040018.
 *
 * fn_8003F464 is copied from pda_range_80037158.c. Its float literals are
 * redirected by the temporary constant_import build step to the shared
 * lbl_8047BCxx pool.
 */
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

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

/* Message tables of the summary page (lbl_802E52C8 + 0x24C...). */
typedef struct PdaSummaryTables {
    u8 pad000[0x24c];
    u32* pageMsgs[4];
    u32 sortMsgs[5];
    u32 titleMsgs[5];
} PdaSummaryTables;

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_802EF0A8[];
extern u32 GSmsgGetRect(s32 id);
extern void fn_800FB680(s32 arg0, s32 arg1, s32 arg2, void* data);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern void fn_800FE35C(void);

extern PdaSummaryTables lbl_802E52C8;
extern s32 fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(s32 env);
extern u32 GSmsgGetGSchar(u32 msg);
extern void msgctrlSetValue(s32 id, u32 value);

/* Anchor a PDA widget to its layout entry and hand the sprite to the drawer. */
static inline void pdaPlaceSprite(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    sprite->field_50 =
        (s16)((s32)*(f32*)((u8*)&lbl_803A6818 + 0x1dc) +
              *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2));
    spriteSetEnv(fn_800FE6D0(
        (s16)(*(s16*)((u8*)alphaSprite + 0x84) + sprite->field_50),
        (s16)(*(s16*)((u8*)alphaSprite + 0x86) + sprite->field_52)));
}

static inline s32 pdaTint(PdaSprite* alphaSprite, s32 tint)
{
    return alphaSprite->alphaByte | tint;
}

/* Per-widget layout and visibility for the PDA's summary page. */
void fn_8003F464(PdaSprite* alphaSprite, PdaSprite* sprite)
{
    extern f64 sin(f64 x);
    PdaSummaryTables* T = &lbl_802E52C8;
    u8* S = (u8*)&lbl_803A6818;
    f32 pos;
    f32 stride;
    s8 i;
    s32 off;
    s32 slot;
    s32 msg;

    alphaSprite->alphaByte = 255.0f * lbl_803A6818.alphaScale;
    switch (sprite->eventId) {
    case 0xd47:
    case 0xd48:
    case 0xd49:
    case 0xd4a:
    case 0xd4b:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd89:
        pdaPlaceSprite(alphaSprite, sprite);
        {
            s16* row = (s16*)(lbl_802EF0A8 + 0x17b00);
            slot = *(s8*)(S + 0x15f) - *(s8*)(S + 0x1c8);
            if (slot >= 0x1d) {
                slot = 0x1c;
            }
            sprite->field_52 = (s16)(*row + (s16)(slot * 0x1d));
        }

        break;
    case 0xd8a:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)((u8*)&lbl_803A6818 + 0x1c8) > 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(2.0f *
                      (f32)sin(3.1415927f *
                                   (2.0f *
                                    (2.0f * lbl_803A6818.angle)) +
                               3.1415927f) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b1c));
        break;
    case 0xd8b:
        pdaPlaceSprite(alphaSprite, sprite);
        if (*(s8*)(S + 0x1c9) >= 10) {
            if (*(s8*)(S + 0x160) != *(s8*)(S + 0x1c9)) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
                break;
            }
        } else {
            sprite->flags &= ~2;
            break;
        }
        sprite->field_52 =
            (s16)(2.0f *
                      (f32)sin(3.1415927f *
                               (2.0f *
                                (2.0f * lbl_803A6818.angle))) +
                  (f32)*(s16*)(lbl_802EF0A8 + 0x17b38));
        break;
    case 0xd66:
    case 0xd67:
    case 0xd68:
    case 0xd69:
    case 0xd6a:
    case 0xd6b:
    case 0xd6c:
    case 0xd6d:
    case 0xd6e:
        pdaPlaceSprite(alphaSprite, sprite);
        break;
    case 0xd6f:
        pdaPlaceSprite(alphaSprite, sprite);
        fn_800FE38C(
            *(s16*)(lbl_802EF0A8 + 0x17b52) -
                *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 2),
            *(s16*)(lbl_802EF0A8 + 0x17b54) -
                *(s16*)(lbl_802EF0A8 + sprite->eventId * 0x1c + 4),
            *(s16*)(lbl_802EF0A8 + 0x17b56),
            *(s16*)(lbl_802EF0A8 + 0x17b58));
        if (*(s8*)(S + 0x15d) != 4) {
            i = 0;
            pos = *(f32*)(S + 0x1d0);
            off = 0;
            stride = 29.0f;
            for (i = 0; i < *(s8*)(S + 0x160); i++) {
                if (i >= *(s8*)(S + 0x1c8) - 1 &&
                    i <= *(s8*)(S + 0x1c9) + 1) {
                    fn_800FB680(0, (s32)pos,
                                ((s64)alphaSprite->alphaByte | -0x100),
                                (void*)*(u32*)((u8*)T->pageMsgs[*(s8*)(S + 0x15d)] +
                                               off));
                }
                pos = pos + stride;
                off += 4;
            }
        }
        fn_800FE35C();
        break;
    case 0xd79:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ed) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ed);
        break;
    case 0xd7a:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ec) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ec);
        break;
    case 0xd7b:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36eb) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36eb);
        break;
    case 0xd7c:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36ea) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36ea);
        break;
    case 0xd7d:
        fn_800FB680((s16)((sprite->x - (s16)(GSmsgGetRect(0x36e9) >> 16)) / 2), -2,
                    ((s64)alphaSprite->alphaByte | -0x100), (void*)0x36e9);
        break;
    case 0xd74:
        fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(T->pageMsgs[3])[*(s8*)(S + 0x15c)]);
        break;
    case 0xd75:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((T->pageMsgs[2])[*(s8*)(S + 0x15b)]));
        fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)0xe7);
        break;
    case 0xd76:
        msgctrlSetValue(
            0x37,
            GSmsgGetGSchar((T->pageMsgs[2])[*(s8*)(S + 0x15a)]));
        fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)0xe7);
        break;
    case 0xd77:
        fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(T->pageMsgs[1])[*(s8*)(S + 0x159)]);
        break;
    case 0xd78:
        fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                    (void*)(T->pageMsgs[0])[*(s8*)(S + 0x158)]);
        break;
    case 0xd54:
        if (*(s8*)(S + 0x15d) == 0) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd55:
        if (*(s8*)(S + 0x15d) == 1) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd56:
        if (*(s8*)(S + 0x15d) == 2) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd57:
        if (*(s8*)(S + 0x15d) == 3) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd58:
        if (*(s8*)(S + 0x15d) == 4) {
            sprite->flags |= 2;
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd73:
        switch (*(s32*)(S + 0x154)) {
        case 0:
            if (*(s8*)(S + 0x15d) == 4) {
                slot = *(s32*)(S + 0x1e4);
                if (slot == 0) {
                    msg = T->titleMsgs[*(s8*)(S + 0x15d)];
                }
                if (slot == 2) {
                    msg = 0x3712;
                }
                if (slot == 1) {
                    msg = 0x3714;
                }
            } else {
                msg = T->titleMsgs[*(s8*)(S + 0x15d)];
            }
            fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100), (void*)msg);
            break;
        case 1:
            if (*(s8*)(S + 0x15d) == 3) {
                fn_800FB680(0, 0, ((s64)alphaSprite->alphaByte | -0x100),
                            (void*)T->sortMsgs[*(s8*)(S + 0x15f)]);
            }
            break;
        default:
            break;
        }
        break;
    case 0xd59:
    case 0xd5a:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5b:
    case 0xd5c:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    case 0xd5d:
    case 0xd5e:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 2) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            if (*(s8*)(S + 0x15e) == 0) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    case 0xd5f:
    case 0xd60:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 2) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            if (*(s8*)(S + 0x15e) == 1) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        }
        break;
    case 0xd61:
    case 0xd62:
        if (*(s8*)(S + 0x15d) != 2) {
            if (*(s8*)(S + 0x15d) == 3) {
                sprite->flags |= 2;
            } else {
                sprite->flags &= ~2;
            }
        } else {
            sprite->flags &= ~2;
        }
        break;
    default:
        break;
    }
}
