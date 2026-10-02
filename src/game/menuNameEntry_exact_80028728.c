/**
 * @file menuNameEntry_exact_80028728.c
 * @brief Pokemon model draw callback for the name-entry backdrop.
 */
#include "dolphin/types.h"

typedef struct NameEntryWindow {
    u8 pad00[0x54];
    s16 width;
    s16 height;
    u8 pad58[8];
    s32* mode;
} NameEntryWindow;

extern void* menuModelRender(void* work);
extern void fn_800D888C(s32 flags);
extern void fn_800D88DC(s32 flags);
extern void fn_800D7820(void* resource);
extern void fn_800D85D4(s32 slot, void* texture);
extern void fn_800D6A00(s32 mode);
extern void fn_800D67BC(s32 mode);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32 slot, f32 xScale, f32 yScale);
extern void fn_800D6728(void);
extern u8 lbl_803A2094[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;

s32 menuNameEntryBackDrawPokemonModel(NameEntryWindow* window,
                                      NameEntryWindow* output)
{
    void* texture;

    if (*window->mode != 2) {
        return 0;
    }
    texture = menuModelRender(lbl_803A2094);
    if (texture != NULL) {
        fn_800D888C(4);
        fn_800D88DC(3);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, texture);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B930, lbl_8047B930);
        fn_800D61E4(output->width, output->height);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B934, lbl_8047B934);
        fn_800D6728();
    }
    return 0;
}
