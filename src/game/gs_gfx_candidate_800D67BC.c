/**
 * @file gs_gfx_candidate_800D67BC.c
 * @brief GS graphics vertex-batch draw, 0x800D67BC - 0x800D6A00.
 *
 * Function-boundary carve of fn_800D67BC. The range needs nothing its TU
 * would own: no jump table, no pooled constant, and the primitive table
 * (lbl_80314350) and draw counters (lbl_804001F0) stay extern.
 *
 * Retail expands fn_800D6A80 (the vertex/primitive counter, out of line at
 * 0x800D6A80 and owned by gs_gfx_exact_800D6A00.c) at the end of this
 * function, with the two counter addresses constant-folded. The callee
 * sits later in the TU than this caller, so the original build inlined it
 * with deferred inlining. The carve reproduces the same expansion by
 * defining the same fn_800D6A80 source `inline` ahead of the caller, which
 * emits no second fn_800D6A80 symbol.
 */
#include "dolphin/types.h"

extern void fn_800DB758(u16 vertCount);
extern void fn_800D4F98(u32, u32, ...);
extern void fn_800D7650(u8*);
extern void fn_800D7868(u8*, u32, u32, u32, u32, u8, u32, u8);
extern void fn_800D7A70(u32);
extern void fn_800D892C(u32);
extern void fn_800B928C(u32, u32, u16);

extern u32 lbl_8047AA80;
extern u32 lbl_80314350[];
extern u8 lbl_804001F0[];

inline void fn_800D6A80(u16 vertCount, s32 type, u32* totalVerts, u32* totalPrims) {
    *totalVerts += vertCount;
    switch (type) {
        case 3: *totalPrims += vertCount / 3; break;
        case 4:
        case 5: *totalPrims += vertCount - 2; break;
        case 6:
        case 7: *totalPrims += (vertCount >> 1); break;
    }
}

void fn_800D67BC(u16 vertCount) {
    if (*(u8*)(lbl_8047AA80 + 0x47e) == 1) {
        fn_800DB758(vertCount);
        return;
    }
    if (*(s32*)lbl_8047AA80 == 1) {
        fn_800D4F98(2, 1, vertCount);
        return;
    }
    if (*(u8*)(lbl_8047AA80 + 0x1b) != *(u8*)(lbl_8047AA80 + 0x1a)) {
        return;
    }
    if ((*(u32*)(lbl_8047AA80 + 0x4) & *(u32*)(lbl_8047AA80 + 0x8)) == 0) {
        return;
    }

    if (*(u32*)(lbl_8047AA80 + 0x24) == 0) {
        fn_800D7650((u8*)*(u32*)(lbl_8047AA80 + 0x20));
        fn_800D7868((u8*)*(u32*)(lbl_8047AA80 + 0x20), 1, 0, 1, 4, 0, 0, 0);
        if (*(u32*)(lbl_8047AA80 + 0x10) & 4) {
            fn_800D7868((u8*)*(u32*)(lbl_8047AA80 + 0x20), 2, 0, 2, 4, 0, 0, 0);
        }
        if (*(u32*)(lbl_8047AA80 + 0x10) & 1) {
            fn_800D7868((u8*)*(u32*)(lbl_8047AA80 + 0x20), 4, 0, 6, 10, 0, 0, 0);
        }
        if (*(u32*)(lbl_8047AA80 + 0x10) & 2) {
            fn_800D7868((u8*)*(u32*)(lbl_8047AA80 + 0x20), 6, 0, 8, 4, 0, 0, 0);
        }
        *(u32*)(lbl_8047AA80 + 0x24) = *(u32*)(lbl_8047AA80 + 0x20);
    }

    if (*(s32*)(lbl_8047AA80 + 0x14) == 7) {
        vertCount = (vertCount & 0x7fff) << 1;
    }

    fn_800D7A70(*(u32*)(lbl_8047AA80 + 0x24));
    fn_800D892C(*(u32*)(lbl_8047AA80 + 0x24));
    fn_800B928C(lbl_80314350[*(u32*)(lbl_8047AA80 + 0x14)],
                *(u32*)(*(u32*)(lbl_8047AA80 + 0x24) + 0x4), vertCount);
    fn_800D6A80(vertCount, *(s32*)(lbl_8047AA80 + 0x14),
                (u32*)(lbl_804001F0 + 0xc), (u32*)(lbl_804001F0 + 0x4));
}
