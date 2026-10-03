/**
 * @file menuCB_range_8005344C.c
 * @brief colosseum-battle Pokemon-select prefix, 0x8005344C - 0x80054914.
 *
 * Split out of the former game/menu/menuCB_Battle.c bucket (2026-07-07) into
 * true XD source-unit segments. PCBOX-centric (getPokemon/setPokemon/delPokemon,
 * pcboxGetNbPokemonBox), menuModelCheck/Render, itemDataBiosGetName,
 * winSpriteSetDisp disp-subs + windowGetKeyInfo ctrl fns. Identity SPECULATIVE
 * (0 XD anchors; structural-family evidence only).
 */
#include "dolphin/types.h"

typedef struct MenuCBPane MenuCBPane;
typedef struct MenuCBSlotInfo MenuCBSlotInfo;
typedef struct MenuCBState MenuCBState;
typedef struct MenuKeyInfo MenuKeyInfo;
typedef struct MenuModelWork MenuModelWork;
typedef struct MenuCBLayoutEntry MenuCBLayoutEntry;
typedef struct MenuCBStatusWork MenuCBStatusWork;
typedef struct MenuCursorItem MenuCursorItem;
typedef struct MenuCBResourceLayout MenuCBResourceLayout;
typedef struct MenuItemBios MenuItemBios;

typedef struct MenuCBPokemonBlob {
    u8 bytes[0x138];
} MenuCBPokemonBlob;

struct MenuCBState {
    s32 markKind;
    s32 cursorKind;
};

struct MenuCBPane {
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
};

struct MenuCBSlotInfo {
    s32 itemId;
    s32 kind;
    s32 slot;
};

struct MenuKeyInfo {
    u16 buttons;
    u16 buttonsPrev;
    u16 buttonsDown;
    u16 buttonsRepeat;
};

struct MenuModelWork {
    u8 pad_00[0x48];
};

struct MenuCBLayoutEntry {
    s32 itemId;
    s16 y;
    s16 pad_06;
};

struct MenuCBStatusWork {
    u8 pad_00;
    s8 state;
    s8 initialized;
    u8 pad_03;
    s32 windowId;
};

struct MenuCursorItem {
    u8 pad_00[2];
    s16 x;
    s16 y;
};

struct MenuCBResourceLayout {
    s32 itemId;
    s16 y;
    u8 pad_06[0x16];
};

struct MenuItemBios {
    u8 pad_00[6];
    s16 x;
    s16 y;
};

extern f32 lbl_8047A54C;
extern void* lbl_8047A548;
extern f32 lbl_8047A550;
extern f32 lbl_8047A554;
extern f32 lbl_8047A558;
extern f32 lbl_8047A540;
extern u32 lbl_8047A544;
extern const f32 lbl_8047BE60;
extern const f32 lbl_8047BE64;
extern const f32 lbl_8047BE6C;
extern const f32 lbl_8047BE68;
extern const f32 lbl_8047BE70;
extern const f32 lbl_8047BE84;
extern const f32 lbl_8047BE8C;
extern const f32 lbl_8047BE90;
extern const f32 lbl_8047BE94;
extern const f32 lbl_8047BE80;
extern const f32 lbl_8047BE98;
extern const f32 lbl_8047BE88;
extern MenuCBLayoutEntry lbl_802E61E8[17];
extern const u32 lbl_80267350[18];
extern s32 lbl_80267320[6];
extern u8 lbl_80314F98[];
extern u8 lbl_80314E08[];
extern u8 lbl_802EF0A8[];
extern MenuCBPokemonBlob lbl_803A95E8;
extern MenuCBSlotInfo lbl_80267398[0x20];
extern MenuModelWork lbl_803A9720;
extern u32 lbl_8047A560;
extern s32 lbl_804788F8;

extern void* fn_80057270();
extern s32 fn_800573C0(void);
extern s32 fn_800566E8(void);
extern s32 fn_80057E40(MenuCBPane* pane);
extern void fn_800FB680(s32 x, s32 y, s32 color, u32 msgId);
extern u32 GSmsgGetGSchar(u32 msgId);
extern u32 GSmsgGetRect(u32 msgId);
extern void* itemDataBiosGetPtr(u16 itemId);
extern u32 itemDataBiosGetName(void* itemData);
extern s32 menuCloseCustom(s32 menuId, s32 mode, s32 wait);
extern s32 menuCloseSync(s32 menuId, s32 wait);
extern void menuClose(s32 menuId);
extern void menuOpen(s32 menuId, s32 wait);
extern s32 menuOpenCustom(s32 menuId, s32 owner, s32 arg2, s32 arg3, s32 arg4, s32 arg5, ...);
extern void menuButtonNormal(MenuCBPane* pane);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);
extern void msgctrlSetValue(s32 id, u32 value);
extern void _threadSwitch(void);
extern u8 pokemonBiosGetLevel(void* pokemon);
extern u8 pokemonBiosGetPcboxMark(void* pokemon);
extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern u32 pokemonDataBiosGetName(void* pokemonData);
extern void* pokemonDataBiosGetPtr(u16 dataId);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern void* pcboxGetPokemonBoxName(void* pcbox, s8 box);
extern s8 pcboxGetNbPokemonBox(void);
extern s8 getPokemonBoxNbUsedSlot__5PCBOXFSc(void* pcbox, s8 box);
extern void winSpriteSetDisp(MenuCBPane* pane, u8 enable);
extern MenuKeyInfo* windowGetKeyInfo(void);
extern void fn_8010A420(MenuModelWork* work);
extern s32 menuModelCheck(MenuModelWork* work, u8 wait);
extern s32 fn_8010A210(MenuModelWork* work, void* pokemon);
extern void* menuModelRender(MenuModelWork* work);
extern void fn_80109C88(MenuModelWork* work, void* pokemon);
s32 fn_80057694(void);
void fn_800576A4(u32 value);
u32 fn_800567AC(void);
u32 fn_80056704(void);
void menuCursorNormal(MenuCBPane* pane);
MenuCursorItem* windowGetCursorToItem();
void fn_80057830(s16 x, s16 y, s32 selected);
extern void fn_80054760(s32 forward, s32 wait);
extern MenuItemBios* menuItemBiosGetPtr(s32 itemId);
extern void menuModelInit(MenuModelWork* work, s16 x, s16 y);
extern void pokemonInit(MenuCBPokemonBlob* pokemon);
extern void fn_800558B8(void);
extern void fn_80056A80(void);
extern void fn_80056B74(MenuCBPane* pane, s32 enabled);
extern void fn_80057A38(void);
extern void fn_80057A64(void* pokemon, s32 arg1);
extern s8 fn_801347D8(void);
extern s32 fn_80055E38(MenuCBPane* pane);
extern void fn_800FE6D0(s16 x, s16 y);
extern void spriteSetEnv(void);
extern void* heroGetStatus(void* hero, s32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern s32 fn_80057DE8(void* pokemon);
extern void* fn_80057F94(void* pokemon);
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
s32 fn_800576B4(void);
s32 fn_80057538(void);
s32 fn_80056A78(void);
void* getPokemon__5PCBOXFScSc(void*, s8, s8);
void delPokemon__5PCBOXFScSc(void*, s8, s8);
void setPokemon__5PCBOXFP7PokemonScSc(void*, void*, s8, s8);
void fn_800574FC(void* pokemon);
void* fn_800574E0(void);
void fn_800574A8(void);
void fn_80057400(void);
void fn_800576C4(s32 state);

#if !defined(MENUCB_SUFFIX_ONLY)

#pragma push
#pragma peephole off
s32 fn_8005344C(MenuCBPane* pane, MenuCBPane* sprite) {
    s16 position;
    s16 x;
    s16 y;
    s32 visible;
    MenuCBPane* window;

    extern s32 fn_80057A08(MenuCBPane* pane);
    extern void* windowSearchID(s32 id);
    extern s32 fn_80058F08(s16* position, s32 box);
    extern void fn_80057094(s16* x, s16* y);

    visible = FALSE;
    if (fn_80057A08(pane) != 0) {
        window = windowSearchID(0x94);
        if (window != NULL && fn_80058F08(&position, window->boxIndex) == 0) {
            visible = TRUE;
            fn_80057094(&x, &y);
            sprite->x = x + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 2);
            sprite->y = y + *(s16*)(lbl_802EF0A8 + (*(s16*)((u8*)sprite + 6) * 0x1c) + 4);
        }
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

typedef struct MenuCBTweenFrame {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    s32 alpha;
} MenuCBTweenFrame;

extern const MenuCBTweenFrame lbl_80267338[2];

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_8005351C(MenuCBPane* pane, MenuCBPane* sprite)
{
    MenuCBTweenFrame frames[2];
    f32 amount;
    s32 visible;
    f32 fx, fy, fw, fh, fa;
    extern s32 fn_80057E40();

    frames[0] = lbl_80267338[0];
    frames[1] = lbl_80267338[1];
    if (fn_80057E40() != 2) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, visible);
    if (visible) {
        amount = lbl_8047A540;
        fx = frames[0].x + amount * (frames[1].x - frames[0].x);
        fy = frames[0].y + amount * (frames[1].y - frames[0].y);
        fw = frames[0].width + amount * (frames[1].width - frames[0].width);
        fh = frames[0].height + amount * (frames[1].height - frames[0].height);
        fa = frames[0].alpha + amount * (frames[1].alpha - frames[0].alpha);
        sprite->x = fx;
        sprite->y = fy;
        sprite->width = fw;
        sprite->height = fh;
        sprite->alpha = fa;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma peephole off
s32 fn_80053728(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 visible;

    if (fn_80057E40(pane) != 2) {
        visible = TRUE;
    } else {
        visible = FALSE;
    }
    winSpriteSetDisp(sprite, (u8)visible);
    return 0;
}
#pragma pop

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
u32 fn_80053778(u32 unused, u8* pane) {
    u32 texture;
    s32 alpha;
    s32 height;
    MenuCBPokemonBlob* pokemon;
    f32 heightScale;
    f32 fade;
    extern void fn_800D61E4();
    extern s32 fn_80057E40();

    texture = 0;
    if (fn_80057E40() != 2) {
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D6A00(6);
        fn_800D7820(lbl_80314E08);

        heightScale = 2.0f * lbl_8047A540;
        if (heightScale > 1.0f) {
            heightScale = 1.0f;
        }
        height = (s32)((f32)(s32)*(s16*)(pane + 0x56) * heightScale);

        fade = 2.0f * (lbl_8047A540 - 0.5f);
        if (fade < 0.0f) {
            fade = 0.0f;
        }
        alpha = (s32)(102.0f * (1.0f - fade));

        fn_800D67BC(4);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0);
        fn_800D61E4(*(s16*)(pane + 0x54), 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0);
        fn_800D61E4(*(s16*)(pane + 0x54), height);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, alpha);
        fn_800D61E4(0, height);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, alpha);
        fn_800D6728();
        return 0;
    }

    pokemon = fn_80057270();
    if (pokemon == NULL) {
        return 0;
    }

    if ((u8)menuModelCheck(&lbl_803A9720, 0) == 0) {
        if ((u8)fn_8010A210(&lbl_803A9720, pokemon) != 0) {
            texture = (u32)menuModelRender(&lbl_803A9720);
        } else {
            fn_80109C88(&lbl_803A9720, pokemon);
            lbl_803A95E8 = *pokemon;
        }
    }

    if (texture != 0) {
        fn_800D88DC(3);
        fn_800D888C(4);
        fn_800D6A00(7);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, (void*)texture);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047BE68, lbl_8047BE68);
        fn_800D61E4(*(s16*)(pane + 0x54), *(s16*)(pane + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047BE60, lbl_8047BE60);
        fn_800D6728();
    }

    return 0;
}
#pragma pop

#endif

#if !defined(MENUCB_PREFIX_ONLY)

#pragma push
#pragma peephole off
s32 fn_80054420(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 i;
    s32 scaled;
    s16 offset;

    for (i = 0; i < 17; i++) {
        if (*(s16*)((u8*)sprite + 6) == lbl_802E61E8[i].itemId) {
            break;
        }
    }
    if (i >= 17) {
        return 0;
    }
    offset = lbl_802E61E8[i].y;
    scaled = (s32)(lbl_8047BE80 * lbl_8047A558);
    sprite->y = scaled + offset;
    return 0;
}
#pragma pop

/* The party slot whose item id a sprite shows, or -1. */
static inline s32 menuCBGetPartySlot(s32 itemId) {
    s32 slot;

    for (slot = 0; slot < 6; slot++) {
        if (itemId == lbl_80267320[slot]) {
            break;
        }
    }
    if (slot >= 6) {
        slot = -1;
    }
    return slot;
}

#pragma push
#pragma peephole off
s32 fn_80053ED8(MenuCBPane* pane, MenuCBPane* sprite) {
    s32 index;
    s32 partySlot;
    s32 drawFallback;
    void* texture;
    void* pokemon;
    s32 alpha;
    s16 x;
    s16 y;
    s16 insetX;
    s16 insetY;
    s32 width;
    s32 height;
    f32 t;
    f32 scaleS0;
    f32 scaleS1;
    f32 scaleT0;
    f32 scaleT1;

    drawFallback = TRUE;

    for (index = 0; index < 17; index++) {
        if (sprite->itemId == lbl_802E61E8[index].itemId) {
            break;
        }
    }

    if (index >= 17) {
        return 0;
    }

    sprite->y = (s16)((s32)(-383.0f * lbl_8047A558) + lbl_802E61E8[index].y);
    fn_800FE6D0((s16)(pane->originX + sprite->x), (s16)(pane->originY + sprite->y));
    spriteSetEnv();

    texture = NULL;
    partySlot = menuCBGetPartySlot(sprite->itemId);

    if (partySlot >= 0) {
        pokemon = heroGetStatus(0, 3, (u16)partySlot);
        if (pokemon != NULL) {
            if ((u8)pokemonCheckValid(pokemon) != 0) {
                texture = fn_80057F94(pokemon);
            } else {
                drawFallback = FALSE;
            }
        }
    }

    if (texture != NULL) {
        if (fn_80057DE8(pokemon) != 0) {
            if (lbl_8047A550 < 0.5f) {
                t = 2.0f * lbl_8047A550;
            } else {
                t = 1.0f - (2.0f * (lbl_8047A550 - 0.5f));
            }
            alpha = (s32)(255.0f * t);
        } else {
            alpha = 0;
        }

        if (lbl_8047A54C < 1.0f && (s32)lbl_8047A548 == partySlot) {
            t = 1.0f - lbl_8047A54C;
            width = (s32)(t * sprite->width);
            height = (s32)(t * sprite->height);
            insetX = (s16)((sprite->width - (s16)width) / 2);
            insetY = (s16)((sprite->height - (s16)height) / 2);
        } else {
            width = sprite->width;
            height = sprite->height;
            insetX = 0;
            insetY = 0;
        }

        x = sprite->width;
        y = sprite->height;
        fn_800D88DC(-0x7ffffffe);
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
    } else if (drawFallback != FALSE) {
        void* bios = menuSpriteBiosGetPtr(0x232);
        s16 drawW = *(s16*)((u8*)bios + 0xc);
        bios = menuSpriteBiosGetPtr(0x232);
        windowDrawSprite2(0, 0, drawW, *(s16*)((u8*)bios + 0xe), -1, pane, 0x232, 0);
    }

    return 0;
}
#pragma pop


s32 fn_8005464C(void) {
    return !(lbl_8047A54C >= lbl_8047BE8C);
}

void fn_80054670(void* ptr) {
    lbl_8047A548 = ptr;
    lbl_8047A54C = lbl_8047BE68;
}

s32 fn_80054680(void) {
    if (lbl_8047A554 > lbl_8047BE68) {
        return 3;
    }
    if (lbl_8047A554 < lbl_8047BE68) {
        return 2;
    }
    return lbl_8047A558 >= lbl_8047BE60;
}

void fn_800546C0(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = lbl_8047BE60;
        lbl_8047A554 = lbl_8047BE68;
        return;
    }

    lbl_8047A558 = lbl_8047BE68;
    lbl_8047A554 = lbl_8047BE90;
}

void fn_800546F0(s32 forward) {
    if (forward != 0) {
        lbl_8047A558 = lbl_8047BE68;
        lbl_8047A554 = lbl_8047BE68;
        return;
    }

    lbl_8047A558 = lbl_8047BE60;
    lbl_8047A554 = lbl_8047BE94;
}

#pragma push
#pragma scheduling off
#pragma dont_inline on
void fn_8005471C(void) {
    menuCloseCustom(0x8f, 2, 0);
    menuCloseSync(0x8f, 1);
    fn_8010A420(&lbl_803A9720);
}
#pragma dont_inline reset
#pragma pop

/* Refresh the 17 layout rows' y from the resource layout table. */
static inline void menuCBLoadLayoutY(void) {
    int i;

    for (i = 0; i < 17; i++) {
        lbl_802E61E8[i].y =
            ((MenuCBResourceLayout*)lbl_802EF0A8)[lbl_802E61E8[i].itemId].y;
    }
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
void fn_80054760(s32 forward, s32 wait) {
    MenuItemBios* item;

    lbl_8047A544 = wait;
    if (lbl_804788F8 != 0) {
        menuCBLoadLayoutY();
        lbl_804788F8 = 0;
    }

    if (forward == 0) {
        lbl_8047A558 = lbl_8047BE60;
        lbl_8047A554 = lbl_8047BE68;
    } else {
        lbl_8047A558 = lbl_8047BE68;
        lbl_8047A554 = lbl_8047BE68;
    }

    item = menuItemBiosGetPtr(0xD34);
    menuModelInit(&lbl_803A9720, item->x, item->y);
    pokemonInit(&lbl_803A95E8);
    menuOpenCustom(0x8F, 0x1F, 0, 0, 0, 0);
}
#pragma pop

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma peephole off
s32 fn_800544A8(u8* ctx) {
    extern void winSeqSetMenu(s32, s32);

    switch ((s8)ctx[1]) {
    case 0:
        if ((s8)ctx[2] != 0) { break; }
        lbl_8047A550 = 0.0f;
        lbl_8047A54C = 1.0f;
        lbl_8047A540 = 0.0f;
        if ((s32)lbl_8047A544 != 0) {
            winSeqSetMenu(0x8f, 0xff);
        }
        ctx[2] = 1;
        break;
    case 2:
        if (lbl_8047A554 > 0.0f) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 >= 1.0f) {
                lbl_8047A558 = 1.0f;
                lbl_8047A554 = 0.0f;
            }
        }
        if (lbl_8047A554 < 0.0f) {
            lbl_8047A558 = lbl_8047A558 + lbl_8047A554;
            if (lbl_8047A558 <= 0.0f) {
                lbl_8047A558 = 0.0f;
                lbl_8047A554 = 0.0f;
            }
        }
        lbl_8047A550 += 0.016666668f;
        if (lbl_8047A550 >= 1.0f) {
            lbl_8047A550 -= 1.0f;
        }
        if (lbl_8047A54C < 1.0f) {
            lbl_8047A54C += 0.016666668f;
            if (lbl_8047A54C > 1.0f) {
                lbl_8047A54C = 1.0f;
            }
        }
        lbl_8047A540 += 0.016666668f;
        if (lbl_8047A540 >= 1.0f) {
            lbl_8047A540 -= 1.0f;
        }
        break;
    case 3:
        if ((s8)ctx[2] != 0) { break; }
        winSeqSetMenu(0x8f, 0x103);
        ctx[2] = 1;
        break;
    }
    return 0;
}
#pragma pop

#endif
