/**
 * @file gs_npc_event_candidate_8003037C_r40_80030C14.c
 * @brief fn_80030C14 carve, 0x80030C14 - 0x80030D34.
 *
 * NPC event callback: draw trainer model A (event 0x10CC) or B (0x10CD)
 * over the sprite, like fn_800301B0 does for events 0x10CE/0x10CF.
 */
#include "dolphin/types.h"

extern void fn_800D88DC(s32 layer);
extern void fn_800D888C(s32 layer);
extern void fn_800D6A00(s32 mode);
extern void fn_800D7820(void* resource);
extern void fn_800D85D4(s32 slot, void* model);
extern void fn_800D67BC(s32 blendMode);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D5CB8(s32 slot, s32 r, s32 g, s32 b, s32 a);
extern void fn_800D59B8(s32 slot, f32 scaleX, f32 scaleY);
extern void fn_800D6728(void);
extern void* menuModelRender(void* data);

extern u8 lbl_803A3230[];
extern u8 lbl_803A31E8[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B9D4;
extern f32 lbl_8047B9F0;

void fn_80030C14(void* r3, u8* r4)
{
    s32 evtype = *(s16*)(r4 + 0x6);
    void* model = NULL;

    switch (evtype) {
    case 0x10CC:
        model = menuModelRender(lbl_803A3230);
        break;
    case 0x10CD:
        model = menuModelRender(lbl_803A31E8);
        break;
    }
    if (model != NULL) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, model);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9D4, lbl_8047B9D4);
        fn_800D61E4(*(s16*)(r4 + 0x54), *(s16*)(r4 + 0x56));
        fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
        fn_800D59B8(0, lbl_8047B9F0, lbl_8047B9F0);
        fn_800D6728();
    }
}
