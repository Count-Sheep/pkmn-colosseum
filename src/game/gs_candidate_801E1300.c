/** Exact GSvtr texture-display setup and draw callback, 0x801E1300 - 0x801E16D0. */
#include "dolphin/types.h"

#define GS_SDATA2 __declspec(section ".sdata2")
extern GS_SDATA2 const char lbl_8047E458[];
extern GS_SDATA2 const char lbl_8047E45C[];
extern GS_SDATA2 const char lbl_8047E464[];
extern GS_SDATA2 const char lbl_8047E46C[];
extern GS_SDATA2 const char lbl_8047E474[];

void _vtrTexDispFunc__Fv(void);

void fn_801E1300(void)
{
    extern u32 lbl_80467CF8[];
    extern u8 lbl_8047B420;
    extern u32 lbl_8047B424;
    extern u32 lbl_8047B42C;
    extern u8 lbl_8047B434;
    extern u32 lbl_8047B438;
    extern u8 lbl_8047B43C;
    extern void *memset(void *dst, int value, u32 size);
    extern u32 GSgappCreate(s32 state, u8 priority, u32 param, void *func);

    lbl_8047B420 = 0;
    lbl_8047B424 = 0;
    lbl_8047B42C = 0;
    lbl_8047B438 = 0;
    lbl_8047B434 = 1;
    lbl_8047B43C = 0;
    memset(lbl_80467CF8, 0, 0x10);
    GSgappCreate(1, 0xFD, 10, _vtrTexDispFunc__Fv);
}

void _vtrTexDispFunc__Fv(void)
{
    extern u8 lbl_8047B420;
    extern u32 lbl_8047B424;
    extern u32 lbl_8047B428;
    extern u8 lbl_8047B434;
    extern u8 lbl_8047B435;
    extern void* lbl_8047B438;
    extern f32 lbl_8047E428;
    extern f32 lbl_8047E42C;
    extern f32 lbl_8047E430;
    extern f32 lbl_8047E434;
    extern f32 lbl_8047E438;
    extern f32 lbl_8047E43C;
    extern f32 lbl_8047E440;
    extern f32 lbl_8047E444;
    extern f32 lbl_8047E448;
    extern f32 lbl_8047E44C;
    extern f32 lbl_8047E450;
    extern f32 lbl_8047E454;
    extern u8 lbl_80314958[];
    extern u8 lbl_80314C78[];
    extern void fn_800D88DC(s32 mode);
    extern void fn_800D888C(s32 mode);
    extern void fn_800D9B58(f32, f32, f32, f32);
    extern void fn_800DA4C4(s32, s32, s32);
    extern void fn_800DA2BC(s32, s32, s32);
    extern void fn_800DA1E8(s32, s32, s32);
    extern void fn_800DA028(s32);
    extern void fn_800D9ED8(s32);
    extern void fn_800D6A00(s32 primitive);
    extern void fn_800D7820(void* format);
    extern void fn_800D67BC(s32 count);
    extern void fn_800D6680(f32 x, f32 y, f32 z);
    extern void fn_800D5C18(s32 index, s32 red, s32 green, s32 blue);
    extern void fn_800D85D4(s32 index, void* texture);
    extern void fn_800D59B8(s32 index, f32 u, f32 v);
    extern void fn_800D6728(void);
    extern void fn_800FAEF8(s32 x, s32 y, s32 color, const char* text,
                            ...);

    if (!lbl_8047B420 || !lbl_8047B434) {
        return;
    }
    if (lbl_8047B438 == NULL) {
        return;
    }

    fn_800D88DC(1);
    fn_800D888C(4);
    fn_800D9B58(lbl_8047E428, lbl_8047E428, lbl_8047E42C,
                lbl_8047E430);
    fn_800DA4C4(0, 1, 1);
    fn_800DA2BC(1, 1, 0);
    fn_800DA1E8(0, 1, 1);
    fn_800DA028(0);
    fn_800D9ED8(1);
    fn_800D6A00(4);

    if (lbl_8047B435) {
        fn_800D888C(2);
        fn_800D7820(lbl_80314958);
        fn_800D67BC(4);
        fn_800D6680(lbl_8047E434, lbl_8047E438, lbl_8047E428);
        fn_800D5C18(0, 0xFF, 0, 0);
        fn_800D6680(lbl_8047E43C, lbl_8047E438, lbl_8047E428);
        fn_800D5C18(0, 0xFF, 0, 0);
        fn_800D6680(lbl_8047E434, lbl_8047E440, lbl_8047E428);
        fn_800D5C18(0, 0xFF, 0, 0);
        fn_800D6680(lbl_8047E43C, lbl_8047E440, lbl_8047E428);
        fn_800D5C18(0, 0xFF, 0, 0);
        fn_800D6728();
    }

    fn_800D88DC(2);
    fn_800D7820(lbl_80314C78);
    fn_800D85D4(0, lbl_8047B438);
    fn_800D67BC(4);
    fn_800D6680(lbl_8047E444, lbl_8047E448, lbl_8047E428);
    fn_800D5C18(0, 0xFF, 0xFF, 0xFF);
    fn_800D59B8(0, lbl_8047E428, lbl_8047E428);
    fn_800D6680(lbl_8047E44C, lbl_8047E448, lbl_8047E428);
    fn_800D5C18(0, 0xFF, 0xFF, 0xFF);
    fn_800D59B8(0, lbl_8047E450, lbl_8047E428);
    fn_800D6680(lbl_8047E444, lbl_8047E454, lbl_8047E428);
    fn_800D5C18(0, 0xFF, 0xFF, 0xFF);
    fn_800D59B8(0, lbl_8047E428, lbl_8047E450);
    fn_800D6680(lbl_8047E44C, lbl_8047E454, lbl_8047E428);
    fn_800D5C18(0, 0xFF, 0xFF, 0xFF);
    fn_800D59B8(0, lbl_8047E450, lbl_8047E450);
    fn_800D6728();

    switch (lbl_8047B424) {
    case 0:
        fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E458);
        break;
    case 1:
        fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E45C);
        break;
    case 2:
        fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E464);
        break;
    case 3:
        switch (lbl_8047B428) {
        case 0:
        case 1:
            fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E46C);
            break;
        case 2:
        case 3:
            fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E45C);
            break;
        }
        break;
    case 4:
        fn_800FAEF8(0x230, 0x2C, -1, lbl_8047E474);
        break;
    }
    fn_800D9ED8(0);
}
