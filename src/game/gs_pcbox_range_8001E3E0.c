/**
 * @file gs_pcbox_range_8001E3E0.c
 * @brief Unidentified scene state machine (low-confidence split segment).
 *
 * Address range: 0x8001E3E0 - 0x8001FD48 (9 functions; fn_8001FD48, the
 * title-floor coroutine, belongs to the GStitle TU and has its own unit)
 *
 * Split from game/gs_pcbox.c. This segment has zero name anchors; it is
 * structurally similar to XD's game/menuPcBoxPokemon.cpp scene state
 * machine (CMenuPokemonLeave::init/loop/main) but this cannot be verified
 * via anchor monotonicity, only via internal self-consistency (single
 * shared .data symbol, disjoint call signature from its neighbors).
 * Kept under an address-based filename per low-confidence handling rules.
 */

#include "dolphin/types.h"

/* =========================================================================
 * External declarations (shared)
 * ========================================================================= */

/* Pokemon data */
extern void  heroItemGetItemKindToItemAryPtr(void* pokeData, u8 fieldId, u16* outCount,
                          s32 p4, s32 p5, s32 p6);
extern void  heroHizukiItemGetItemAryPtr(void* pokeData, u16* outCount, s32 p3, s32 p4, s32 p5);
extern u8    fn_801429E8(void* fieldData);
extern u16   itemBiosGetNum(void* fieldData);
extern u16   itemDataBiosGetPtr(u16 speciesId);
extern u16   itemDataBiosGetPrice(void);

/* Text formatting */
extern void  fn_8002A0B8(void* outBuf, void* fmt, s32 p3, s32 p4,
                          u16 p5, s32 p6, ...);
extern s32   heroGetStatus(void* partyData, s32 slot, s32 p3);

/* Dialog/rendering */
extern void  winMsgOpenWithSE(s32 p1, void* text, s32 p3, s32 p4, u8 p5);
extern void  winMsgClose(s32 slot);
extern void  winSeqSetMenu(void* ctx, s32 state);
extern void  menuDataBiosGetXY(s16 npcId, u16* outX, u16* outY);
extern void  menuDataBiosSetXY(s16 x, s16 y, s16 z);
extern void* menuDataBiosGetPtr(void* data);

/* 0x8001E3E0 - 0x8001EF78 (fn_8001E3E0, fn_8001E4B4, fn_8001E58C, fn_8001E644,
 * fn_8001EA98, fn_8001EC08) belong to the menuSub TU and live in
 * src/game/menuSub.c. */


/* 0x8001EF78 | 0x270 */
extern void fn_801666BC(void);
extern f64 lbl_8047B828;
extern f64 lbl_8047B830;
extern f32 lbl_8047B810;
extern f32 lbl_8047A338;
extern f32 lbl_8047B818;
extern f32 lbl_8047A344;
extern f32 lbl_8047B814;
extern u32 lbl_8047A31C;
extern f32 lbl_8047A334;
extern u8 lbl_803A1F88[];
extern f32 lbl_8047B81C;
extern f32 lbl_8047B820;
extern f32 lbl_8047B824;
#if 0
asm void fn_8001EF78(void) {
#include "src/game/gs_pcbox_fn_8001EF78.inc"
}
#else
void fn_8001EF78(void) {
    extern u8 lbl_803A1F88[];
    extern u32 lbl_8047A31C;
    extern f32 lbl_8047A334;
    extern f32 lbl_8047A338;
    extern f32 lbl_8047A344;
    extern f32 lbl_8047B810;
    extern f32 lbl_8047B814;
    extern f32 lbl_8047B818;
    extern f32 lbl_8047B81C;
    extern f32 lbl_8047B820;
    extern f32 lbl_8047B824;
    extern f64 lbl_8047B828;
    extern f64 lbl_8047B830;
    extern void fn_800D3088();
    extern void fn_800D37CC();
    extern void fn_801666BC();
    u8 sp[0x30];
    u32 tmp = 0;
    u32 r3 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f2 = 0.0f;
    f32 f3 = 0.0f;
    f32 f4 = 0.0f;
    f32 f31 = 0.0f;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    fn_800D37CC();
    tmp = 0x43300000;
    f1 = lbl_8047B828;
    *(u32*)(sp + 0x8) = tmp;
    f31 = f0 - f1;
    fn_800D3088();
    tmp = 0x43300000;
    f3 = lbl_8047B830;
    *(u32*)(sp + 0x10) = tmp;
    f2 = lbl_8047B810;
    f1 = lbl_8047A338;
    f3 = f0 - f3;
    f0 = lbl_8047B818;
    f3 = f3 / f31;
    f1 = f3 * f2 + f1;
    lbl_8047A344 = f3;
    lbl_8047A338 = f1;
    /* cror eq, gt, eq */;
    if (f1 == f0) {
        f0 = lbl_8047B814;
        lbl_8047A338 = f0;
    }
    tmp = lbl_8047A31C;
    if ((s32)tmp < 0x1e) return;
    if ((s32)tmp == 0xc8) return;
    f0 = lbl_8047A334;
    r3 = 0x46a;
    f0 = f0 + f3;
    lbl_8047A334 = f0;
    fn_801666BC();
    if ((s32)r3 == 0) {
        tmp = lbl_8047A31C;
        f0 = lbl_8047B814;
        lbl_8047A334 = f0;
        if ((s32)tmp != 0x3e8) {
            tmp = 0x28;
            lbl_8047A31C = tmp;
    }
    }
    r3 = (u32)lbl_803A1F88;
    f0 = lbl_8047A344;
    r3 = (u32)lbl_803A1F88;
    tmp = 0x3;
    ctr_fn = (void(*)(void))tmp;
    do {
        f1 = *(f32*)((u8*)r3 + 0x10);
        f2 = *(f32*)((u8*)r3 + 0x28);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x10);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x10) = f2;
            f3 = *(f32*)((u8*)r3 + 0x28);
            f1 = *(f32*)((u8*)r3 + 0x10);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x10) = f3;
        } while (0);
        f1 = *(f32*)((u8*)r3 + 0x1C);
        f2 = *(f32*)((u8*)r3 + 0x34);
        do {
            if (f1 == f2) break;
            f3 = f2 - f1;
            f2 = lbl_8047B81C;
            f1 = lbl_8047B820;
            f2 = f2 * f3;
            f4 = f2 * f0;
            if (f4 > f1) {
                f4 = f1;
            }
            f1 = lbl_8047B824;
            /* cror eq, lt, eq */;
            if (f4 == f1) {
                f4 = f1;
            }
            f2 = *(f32*)((u8*)r3 + 0x1C);
            f1 = lbl_8047B814;
            f2 = f2 + f4;
            *(f32*)((u8*)r3 + 0x1C) = f2;
            f3 = *(f32*)((u8*)r3 + 0x34);
            f1 = *(f32*)((u8*)r3 + 0x1C);
            f2 = f3 - f1;
            if (f4 > f1) {
            } else {

                f4 = -f4;
            }
            f1 = lbl_8047B814;
            if (f2 > f1) {
                f1 = f2;
            } else {

                f1 = -f2;
            }
            /* cror eq, lt, eq */;
            if (f1 != f4) {
                f1 = lbl_8047B814;
                if (f2 > f1) {
                } else {

                    f2 = -f2;
                }
                f1 = lbl_8047B818;
                if (f2 >= f1) break;
            }
            *(f32*)((u8*)r3 + 0x1C) = f3;
        } while (0);
        r3 = r3 + 0x4;
    } while (--ctr != 0);

    return;
}
#endif

/* 0x8001F1E8 | 0x11C: fn_8001F1E8 lives in gs_pcbox_exact_8001F1E8.c. */

/* 0x8001F304 | 0xA44: fn_8001F304 lives in gs_pcbox_exact_8001F304.c. */

/* fn_8001FD48 (0x8001FD48, 0x5E0) is the title-floor coroutine and lives
 * in its own unit, src/game/gs_pcbox_range_8001E3E0_r40_8001FD48.c. */
