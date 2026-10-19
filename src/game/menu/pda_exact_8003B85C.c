/**
 * @file pda_exact_8003B85C.c
 * @brief PDA party-list scroll and preview, 0x8003B85C - 0x8003BF54.
 *
 * Standalone copy of fn_8003B85C from pda_range_80037158.c with its own
 * copy of the preview loader. Unit flag -opt nopeephole replaces the shared
 * file's local "peephole off".
 */
/* RULE-EXCEPTION(user-approved): constant_import (temporary) - remove when this file is merged back into one unit - see docs/RULE_EXCEPTIONS.md */
#include "dolphin/types.h"

typedef struct PdaModelWindow {
    u8 pad00[0x8];
    s32 field_08;
    s32 field_0C;
    s32 field_10;
    u8 pad14[0x4];
    f32 field_18;
    u8 pad1C[0xC];
    f32 field_28;
    u8 pad2C[0x18];
    f32 alphaScale;
    u8 pad48[0x4C];
    s8 variant;
    u8 pad95[0xB];
} PdaModelWindow; /* lbl_803A6748, size 0xA0 */

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern PdaModelWindow lbl_803A6748;
extern u8 lbl_802EF0A8[];
extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern s32 lbl_8047A4DC;
extern f32 lbl_8047BAC4;
extern f32 lbl_8047BAC8;
extern f32 lbl_8047BAD4;
extern f32 lbl_8047BACC;
extern f32 lbl_8047BAD0;
extern f32 lbl_8047BAD8;
extern u16* windowGetKeyInfo();

extern u32 fn_80166AB8(u32, u32, u32);
extern u16 fn_801EE614();
extern u32 fn_801EE750();
extern u8 fn_801EE8F4();
extern u8 fn_801EEAD0();
extern u8 fn_801EEC74();
extern void fn_801EEDEC(u16 id, s32 flag);
extern u32 gamedataGetStatus();
extern void pokemonCreate(u32 work, u16 species, s32 level, u32 trainer);
extern void pokemonBiosSetRnd(u32 work, u32 rnd);
extern void pokemonBiosSetDarkpokemonDataId(u32 work, s32 id);
extern void pokemonBiosSetDp(u32 work, s32 dp);
extern void fn_80109C88(void* work, u32 pokemon);

/* Rebuild the preview Pokemon for the highlighted party entry; ids points
 * at the first entry's battle ID. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003B85C_LoadPreview; it reads the entry itself (its locals colour like retail's) and takes the battle-ID column from the caller (retail's table+2 base, coloured after the table) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_8003B85C_LoadPreview(u16* ids)
{
    u16 entry;
    u16 rndId;
    s32 mode;
    u16 species;
    u32 rnd;
    u32 work;
    u8 shadow;
    u8 snagged;
    s32 idx;

    work = lbl_8047A4D0;
    idx = *(s32*)&lbl_803A6748;
    species = lbl_8047A4D4[idx].field_00;
    entry = ids[(u16)idx * 2];
    rndId = ids[idx * 2];
    fn_801EE614(entry);
    fn_801EE8F4(entry);
    shadow = fn_801EEAD0(entry);
    snagged = fn_801EEC74(entry);
    if (shadow != 0) {
        if (snagged != 0) {
            mode = 1;
        } else {
            mode = 0;
        }
    } else {
        mode = 2;
    }
    pokemonCreate(work, species, 10, gamedataGetStatus(0, 1));
    rnd = *(u32*)((u8*)&lbl_803A6748 + 0x98);
    fn_801EE750(rndId);
    pokemonBiosSetRnd(work, rnd);
    switch (mode) {
    case 0:
    case 2:
        pokemonBiosSetDarkpokemonDataId(work, 1);
        pokemonBiosSetDp(work, 10);
        break;
    case 1:
        pokemonBiosSetDarkpokemonDataId(work, 0);
        pokemonBiosSetDp(work, 0);
        break;
    default:
        break;
    }
}

/* Magnitude of a scroll step. */
static inline f32 pda_8003B85C_Abs(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* Ease one scroll coordinate toward its target, snapping once the step would overshoot. */
static inline void pda_8003B85C_Ease(f32* cur, f32 t, f32 c)
{
    f32 rate;
    f32 moved;
    f32 rest;
    f32 diff;

    if (c != t) {
        rate = lbl_8047BACC * *(f32*)((u8*)&lbl_803A6748 + 0x24);
        if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
            rate = lbl_8047BAD0 * rate;
        }
        diff = t - c;
        moved = diff * rate;
        *cur = c + moved;
        rest = t - *cur;
        if (pda_8003B85C_Abs(rest) <= pda_8003B85C_Abs(moved)) {
            *cur = t;
        }
    }
}

/* Party-list scroll, cursor movement and the selected Pokemon preview. */
s32 fn_8003B85C(void* window, s32 enabled)
{
    extern u32 fn_800D3088(void);
    extern s32 fn_800D37CC(void);
    u16* keys;
    u16* ids;
    s32 held;
    s32 prevIndex;
    s32 idx;
    u16 code;
    s16 baseY;
    f32 step;
    f32 cur;
    f32 dt;

    keys = windowGetKeyInfo(window);
    held = 0;
    prevIndex = *(s32*)&lbl_803A6748;
    dt = (f32)fn_800D3088() / (f32)fn_800D37CC();
    *(f32*)((u8*)&lbl_803A6748 + 0x24) = dt;
    *(f32*)((u8*)&lbl_803A6748 + 0x28) = *(f32*)((u8*)&lbl_803A6748 + 0x28) + dt;
    if (*(f32*)((u8*)&lbl_803A6748 + 0x28) >= lbl_8047BAC8) {
        *(f32*)((u8*)&lbl_803A6748 + 0x28) = lbl_8047BAC4;
    }
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x14), *(f32*)((u8*)&lbl_803A6748 + 0x1c), *(f32*)((u8*)&lbl_803A6748 + 0x14));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x18), *(f32*)((u8*)&lbl_803A6748 + 0x20), *(f32*)((u8*)&lbl_803A6748 + 0x18));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x2c), *(f32*)((u8*)&lbl_803A6748 + 0x34), *(f32*)((u8*)&lbl_803A6748 + 0x2c));
    pda_8003B85C_Ease((f32*)((u8*)&lbl_803A6748 + 0x30), *(f32*)((u8*)&lbl_803A6748 + 0x38), *(f32*)((u8*)&lbl_803A6748 + 0x30));

    if (*(f32*)((u8*)&lbl_803A6748 + 0x48) != *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
        step = *(f32*)((u8*)&lbl_803A6748 + 0x24) / lbl_8047BAD4;
        if (*(f32*)((u8*)&lbl_803A6748 + 0x48) > *(f32*)((u8*)&lbl_803A6748 + 0x44)) {
            cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) + step;
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
            if (cur > lbl_8047BAC8) {
                *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC8;
            }
        } else {
            cur = *(f32*)((u8*)&lbl_803A6748 + 0x44) - step;
            *(f32*)((u8*)&lbl_803A6748 + 0x44) = cur;
            if (cur < lbl_8047BAC4) {
                *(f32*)((u8*)&lbl_803A6748 + 0x44) = lbl_8047BAC4;
            }
        }
    }

    /* RULE-EXCEPTION(user-approved): codegen-only cast; the (u16) on the key reads gives retail's lhz r0 / mr r3 copy - see docs/RULE_EXCEPTIONS.md */
    if (*(s8*)((u8*)&lbl_803A6748 + 0x40) != 0) {
        code = (u16)keys[0];
    } else {
        code = (u16)keys[3];
    }
    if ((keys[0] & 3) != 0) {
        held = 1;
    }
    if ((code & 2) != 0) {
        if (lbl_8047A4DC != 0) {
            if (*(s32*)&lbl_803A6748 < lbl_8047A4DC - 1) {
                if (enabled != 0) {
                    fn_801EEDEC(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
                }
                *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 + 1;
                fn_80166AB8(0x23, 0, 0);
            }
            if (*(s32*)&lbl_803A6748 >= *(s32*)((u8*)&lbl_803A6748 + 0xc)) {
                *(f32*)((u8*)&lbl_803A6748 + 0x20) = *(f32*)((u8*)&lbl_803A6748 + 0x20) - lbl_8047BAD8;
                *(s32*)((u8*)&lbl_803A6748 + 0xc) = *(s32*)((u8*)&lbl_803A6748 + 0xc) + 1;
                *(s32*)((u8*)&lbl_803A6748 + 0x8) = *(s32*)((u8*)&lbl_803A6748 + 0x8) + 1;
            }
        }
    }
    if ((code & 1) != 0) {
        if (*(s32*)&lbl_803A6748 > 0) {
            if (enabled != 0) {
                fn_801EEDEC(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId, 0);
            }
            *(s32*)&lbl_803A6748 = *(s32*)&lbl_803A6748 - 1;
            fn_80166AB8(0x23, 0, 0);
        }
        if (*(s32*)&lbl_803A6748 < *(s32*)((u8*)&lbl_803A6748 + 0x8)) {
            *(f32*)((u8*)&lbl_803A6748 + 0x20) = *(f32*)((u8*)&lbl_803A6748 + 0x20) + lbl_8047BAD8;
            *(s32*)((u8*)&lbl_803A6748 + 0x8) = *(s32*)((u8*)&lbl_803A6748 + 0x8) - 1;
            *(s32*)((u8*)&lbl_803A6748 + 0xc) = *(s32*)((u8*)&lbl_803A6748 + 0xc) - 1;
        }
    }

    if (held != 0) {
        if (*(f32*)((u8*)&lbl_803A6748 + 0x3c) < lbl_8047BAC8) {
            *(f32*)((u8*)&lbl_803A6748 + 0x3c) = *(f32*)((u8*)&lbl_803A6748 + 0x3c) + *(f32*)((u8*)&lbl_803A6748 + 0x24);
            if (*(f32*)((u8*)&lbl_803A6748 + 0x3c) >= lbl_8047BAC8) {
                *(s8*)((u8*)&lbl_803A6748 + 0x40) = 1;
            }
        }
    } else {
        *(f32*)((u8*)&lbl_803A6748 + 0x3c) = lbl_8047BAC4;
        *(s8*)((u8*)&lbl_803A6748 + 0x40) = 0;
    }

    if (enabled != 0) {
        if (prevIndex != *(s32*)&lbl_803A6748) {
            if (fn_801EE8F4(lbl_8047A4D4[*(s32*)&lbl_803A6748].battleId) != 0) {
                ids = &lbl_8047A4D4->battleId;
                pda_8003B85C_LoadPreview(ids);
                fn_80109C88((u8*)&lbl_803A6748 + 0x4c, lbl_8047A4D0);
            }
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x18) = *(f32*)((u8*)&lbl_803A6748 + 0x20);
        baseY = *(s16*)(lbl_802EF0A8 + 0x50a0);
        idx = *(s32*)&lbl_803A6748 - *(s32*)((u8*)&lbl_803A6748 + 0x8);
        if (idx >= 10) {
            idx = 9;
        }
        *(f32*)((u8*)&lbl_803A6748 + 0x30) = *(f32*)((u8*)&lbl_803A6748 + 0x38) =
            baseY + idx * 0x18;
    }
    return 0;
}
