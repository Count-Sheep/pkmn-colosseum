/**
 * Residual source island 0x80055E38-0x80056A80 from the shared menuCB unit.
 */
#define MENUCB_PREFIX_RESIDUAL_80055E38_ONLY
#include "src/game/menu/menuCB_range_80055E38.c"

extern f32 lbl_8047A57C;
extern f32 lbl_8047A574;
extern u32 lbl_8047A580;
extern u8 lbl_8026768C[];
extern f32 lbl_8047BED0;
extern f32 lbl_8047BED4;
extern f32 lbl_8047BED8;
extern f32 lbl_8047BEB4;
extern void* fn_80104704(u32);
extern f32 lbl_8047A54C;
extern void* lbl_8047A548;
extern f32 lbl_8047A550;
extern f32 lbl_8047A558;
extern f32 lbl_8047BE60;
extern f32 lbl_8047BE64;
extern f32 lbl_8047BE68;
extern f32 lbl_8047BE6C;
extern f32 lbl_8047BE80;
extern f32 lbl_8047BE84;
extern s32 lbl_80267320[6];
extern u8 lbl_80314F98[];
extern void* heroGetStatus(void* hero, s32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern s32 fn_80057DE8(void* pokemon);
extern void* fn_80057F94(void* pokemon);
extern void fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void fn_800D88DC(u32 flags);
extern void fn_800DC0D4(s32, s32, s32, s32, s32);
extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
extern void fn_800DBFD4(s32, s32, s32, s32, s32);
extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
extern u16 GStextureGetXsize(void* texture);
extern u16 GStextureGetYsize(void* texture);
extern void fn_800D7820(void* resource);
extern void fn_800D85D4(s32 slot, void* texture);
extern void fn_800D6A00(s32 mode);
extern void fn_800D67BC(s32 mode);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32 slot, f32 xScale, f32 yScale);
extern void fn_800D6728(void);
extern void fn_800D888C(u32 flags);
extern void* menuSpriteBiosGetPtr(s32 id);
extern void windowDrawSprite2(s32, s32, s16, s16, s32, void*, s32, s32);

typedef struct MenuCBState {
    s32 markKind;
    s32 cursorKind;
} MenuCBState;

typedef struct MenuCBPane {
    u8 pad_00[0x6];
    s16 itemId;
    u8 pad_08[0x44];
    s32 textId;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    u8 pad_58[0x8];
    MenuCBState* state;
    u8 pad_64[0x3];
    u8 alpha;
    u8 pad_68[0x1c];
    s16 originX;
    s16 originY;
    u8 pad_88[0xd];
    s8 boxIndex;
    u8 pad_96;
    s8 previousBoxIndex;
    u8 flag98;
} MenuCBPane;

typedef struct MenuCBLayoutEntry {
    s32 itemId;
    s16 y;
    s16 pad_06;
} MenuCBLayoutEntry;

extern MenuCBLayoutEntry lbl_802E61E8[17];

extern const s32 lbl_80267518[3][30];
extern f32 lbl_8047A570;
extern f32 lbl_8047A574;
extern void* getPokemon__5PCBOXFScSc(void* pcbox, s8 box, s8 slot);

#pragma push
#pragma peephole off
static inline s32 menuCBFindPcboxSprite(MenuCBPane* sprite, s32* slot)
{
    const s32* entry;
    const s32* row;
    s32 box;
    s32 col;
    s32 found;
    s16 id;
    found = 0;
    id = sprite->itemId;
    for (row = lbl_80267518[0], box = 0; box < 3; row += 30, box++) {
        entry = row;
        for (col = 0; col < 30; entry++, col++) {
            if (id == *entry) {
                found = 1;
                break;
            }
        }
        if (found) {
            break;
        }
    }
    if (!found) {
        return -1;
    }
    *slot = col;
    return box;
}

s32 fn_80056084(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 box;
    s16 y;
    s32 slot;
    void* texture;
    void* pokemon;
    s32 alpha;
    s32 drawFallback;
    s16 insetX;
    s16 insetY;
    s16 width;
    s16 height;
    s16 x;
    f32 t;
    f32 scaleS0;
    f32 scaleT1;
    f32 scaleT0;
    f32 scaleS1;

    drawFallback = TRUE;
    texture = NULL;
    box = menuCBFindPcboxSprite(sprite, &slot);

    if (box >= 0) {
        pokemon = getPokemon__5PCBOXFScSc(NULL, box, slot);
        if (pokemon != NULL) {
            if (pokemonCheckValid(pokemon) != 0) {
                texture = fn_80057F94(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (drawFallback == FALSE) {
        return 0;
    }

    if (texture != NULL) {
        if (fn_80057DE8(pokemon) != 0) {
            if (lbl_8047A574 < 0.5f) {
                t = 2.0f * lbl_8047A574;
            } else {
                t = 1.0f - 2.0f * (lbl_8047A574 - 0.5f);
            }
            alpha = (s32)(255.0f * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A570 < 1.0f && (s32)lbl_8047A56C == slot) {
            t = 1.0f - lbl_8047A570;
            width = (s16)(t * sprite->width);
            height = (s16)(t * sprite->height);
            insetX = (s16)((sprite->width - width) / 2);
            insetY = (s16)((sprite->height - height) / 2);
        } else {
            width = sprite->width;
            insetX = 0;
            height = sprite->height;
            insetY = 0;
        }

        y = sprite->height;
        x = sprite->width;
        fn_800D88DC(0x80000002);
        fn_800DC0D4(0, 0xf, 0xb, 0xa, 8);
        fn_800DC14C(0, 0, 0, 0, 1, 0);
        fn_800DBFD4(0, 7, 7, 7, 4);
        fn_800DC04C(0, 0, 0, 0, 1, 0);

        scaleS0 = 0.0f / (f32)GStextureGetXsize(texture);
        scaleS1 = (f32)x / (f32)GStextureGetXsize(texture);
        scaleT0 = 0.0f / (f32)GStextureGetYsize(texture);
        scaleT1 = (f32)y / (f32)GStextureGetYsize(texture);

        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, texture);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(insetX, insetY);
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS0, scaleT0);
        fn_800D61E4((s16)(insetX + width), (s16)(insetY + height));
        fn_800D5CB8(0, 0x3c, 0xc, 0xff, alpha);
        fn_800D59B8(0, scaleS1, scaleT1);
        fn_800D6728();
        fn_800D888C(0x80000000);
    } else {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);

        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane,
                          0x232, 0);
    }

    return 0;
}
#pragma pop
