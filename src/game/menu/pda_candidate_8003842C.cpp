/**
 * PC/People menu driver, 0x8003842C - 0x800388C4.
 *
 * RULE-EXCEPTION(user-approved): GC/1.3.2 C++ compilation, pooled table
 * ownership, helper-local allocation and scoped compiler controls reproduce
 * retail code. See docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern "C" {

extern void* lbl_8047A480;
extern s8 lbl_8047A490;
extern s8 lbl_8047A47C;
extern s8 lbl_8047A47D;
extern f32 lbl_8047A488;
extern f32 lbl_8047A484;

u8 lbl_802E51C8[96] = {
    0x00, 0x00, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x28, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x50, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x78, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x90, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
u8 lbl_802E5228[96] = {
    0x00, 0x00, 0x00, 0x00, 0x42, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x68, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x88, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0x9C, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x42, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x42, 0xB0, 0x00, 0x00, 0xC3, 0x96, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
u8 lbl_802E5288[32] = {
    0x42, 0x28, 0x00, 0x00, 0x43, 0x33, 0x00, 0x00, 0x42, 0x50, 0x00, 0x00,
    0x43, 0x6A, 0x00, 0x00, 0x42, 0x78, 0x00, 0x00, 0x43, 0x90, 0x80, 0x00,
    0x42, 0x90, 0x00, 0x00, 0x43, 0xAC, 0x00, 0x00,
};
u8 lbl_802E52A8[16] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0xC9, 0x0F, 0xDB, 0x40, 0x49, 0x0F, 0xDB,
    0x40, 0x96, 0xCB, 0xE4,
};
u8 lbl_803A6498[0x54];
u8 lbl_803A64EC[0x60];
u8 lbl_803A654C[0x64];
/* The same four-row reset is expanded at each menu-choice transition. */
static inline void pdaResetPeopleTables(void)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    s32 i;
    u8* src;
    u8* dst;
    lbl_8047A480 = (void*)0x1b5a;
    src = lbl_802E51C8;
    dst = lbl_803A654C;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    src = lbl_802E5228;
    dst = lbl_803A64EC;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)lbl_802E52A8;
    lbl_8047A484 = *(f32*)lbl_802E52A8;
    lbl_8047A47D = 0;
}
/* Separate second-loop locals retain the initial expansion's allocation. */
static inline void pdaResetPeopleTablesFirst(void)
{
    extern void* memcpy(void* dst, const void* src, u32 size);
    u8* dst;
    u8* rowDest;
    s32 rowIndex;
    u8* rowSource;
    s32 i;
    u8* src;
    lbl_8047A480 = (void*)0x1b5a;
    src = lbl_802E51C8;
    dst = lbl_803A654C;
    for (i = 0; i < 4; src += 0x18, dst += 0x18, i++) {
        memcpy(dst, src, 0x18);
    }
    rowSource = lbl_802E5228;
    rowDest = lbl_803A64EC;
    for (rowIndex = 0; rowIndex < 4; rowSource += 0x18, rowDest += 0x18, rowIndex++) {
        memcpy(rowDest, rowSource, 0x18);
    }
    lbl_8047A47C = 0;
    lbl_8047A488 = *(f32*)lbl_802E52A8;
    lbl_8047A484 = *(f32*)lbl_802E52A8;
    lbl_8047A47D = 0;
}

#pragma push
/* RULE-EXCEPTION(user-approved): retain retail copies and local allocation;
 * both settings are recorded in docs/RULE_EXCEPTIONS.md. */
#pragma peephole off
#pragma opt_lifetimes off
extern void menuCloseCustom(s32, s32, s32);
void fn_8003842C(void)
{
    extern u8 lbl_802EF0A8[];
    extern s32 menuOpen(s32 menu, s32 mode);
    extern void fn_801661D0(s32 a, s32 b, s32 c, s32 d);
    extern void fn_801660D8(s32 a, s32 b, s32 c);
    extern void fn_800FF660(void);
    extern void fn_80058150(void);
    extern void fn_8003A520(void);
    extern s32 fn_80038A0C(void);
    extern void menuModelInit(u8* model, s16 a, s16 b);
    extern void menuModelSetMotion(u8* model, s32 motion);
    extern void fn_8010A010(u8* model, s32 id);
    extern void fn_8010A420(u8* model);
    extern void* peopleInfoBiosGetPtr(s32 id);
    extern void fn_8018F4C8(void* info, s32 kind, s32* out0, s32* out1);
    s32 state;
    s32 running;
    s32 choice;
    s32 motion;
    s32 scratch;

    state = 0;
    running = 1;
    fn_801661D0(0x55, 0x1f4, 1, 1);
    do {
        switch (state) {
        case 0:
            pdaResetPeopleTablesFirst();
            menuOpen(0x18, 0);
            menuOpen(0x1e, 0);
            menuOpen(0x1f, 0);
            menuOpen(0x1a, 0);
            menuOpen(0x1b, 0);
            menuOpen(0x19, 0);
            state = 1;
            break;
        case 1:
            lbl_8047A490 = -1;
            menuOpen(0x20, 0);
            menuOpen(0x21, 0);
            menuOpen(0x22, 0);
            menuOpen(0x23, 0);
            choice = menuOpen(0x1d, 1);
            if (choice >= 0) {
                lbl_8047A490 = (s8)choice;
                switch (choice) {
                case 0:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0xa;
                    break;
                case 1:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0x14;
                    break;
                case 2:
                    menuCloseCustom(0x20, 0, 1);
                    pdaResetPeopleTables();
                    state = 0x1e;
                    break;
                case 3:
                    state = 0x3e8;
                    break;
                }
            } else {
                state = 0x3e8;
            }
            break;
        case 0xa:
            fn_80058150();
            state = 1;
            break;
        case 0x14:
            lbl_8047A480 = (void*)0x1b61;
            fn_8003A520();
            lbl_8047A480 = (void*)0x1b5a;
            state = 1;
            break;
        case 0x1e:
            menuModelInit(lbl_803A6498, (s16)(*(s16*)(lbl_802EF0A8 + 0x5fd6) + 0xa),
            *(s16*)(lbl_802EF0A8 + 0x5fd8));
            fn_8010A010(lbl_803A6498, 0xf70400);
            fn_8018F4C8(peopleInfoBiosGetPtr(0xf70400), 1, &motion, &scratch);
            menuModelSetMotion(lbl_803A6498, motion);
            choice = fn_80038A0C();
            fn_8010A420(lbl_803A6498);
            state = choice;
            break;
        case 0x28:
            state = 0x3e8;
            break;
        case 0x3e8:
            menuCloseCustom(0x1d, 0, 1);
            running = 0;
            break;
        }
    } while (running);
    fn_801660D8(0x3e8, 1, 1);
    fn_800FF660();
}
#pragma pop

}
