/**
 * @file menuCB_exact_80053A60.c
 * @brief Pokemon mark, held-item, sex, species, and level callbacks.
 */
#include "dolphin/types.h"

typedef struct MenuCBPane {
    u8 pad_00[0x4C];
    s32 textId;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
} MenuCBPane;

extern void* fn_80057270(MenuCBPane* pane);
extern u8 pokemonBiosGetPcboxMark(void* pokemon);
extern u16 pokemonGetSoubiItemDataId(void* pokemon);
extern void* itemDataBiosGetPtr(u16 itemId);
extern u32 itemDataBiosGetName(void* itemData);
extern u32 GSmsgGetGSchar(u32 msgId);
extern void msgctrlSetValue(s32 id, u32 value);
extern void fn_800FB680(s32 x, s32 y, s32 color, u32 msgId);
extern void winSpriteSetDisp(MenuCBPane* pane, u8 enable);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);
extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
extern void* pokemonDataBiosGetPtr(u16 dataId);
extern u32 pokemonDataBiosGetName(void* pokemonData);
extern u8 pokemonBiosGetLevel(void* pokemon);
extern u32 GSmsgGetRect(u32 msgId);

s32 fn_80053A60(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 8;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 fn_80053AC8(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 4;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 fn_80053B30(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 2;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 fn_80053B98(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        u32 mark = (u8)pokemonBiosGetPcboxMark(pokemon);
        s32 masked = mark & 1;

        if (masked != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 fn_80053C00(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    u16 itemId;
    void* itemData;
    s32 result;

    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        itemId = pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            itemData = itemDataBiosGetPtr(itemId);
            if (itemData != NULL) {
                msgctrlSetValue(0x37, GSmsgGetGSchar(itemDataBiosGetName(itemData)));
                fn_800FB680(0, 0, -1, 0xe7);
            }
        }
    }
    result = 0;
    sprite->textId = result;
    return result;
}

s32 fn_80053C84(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 visible;
    void* pokemon;
    u32 itemId;

    visible = FALSE;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        itemId = (u16)pokemonGetSoubiItemDataId(pokemon);
        if (itemId != 0) {
            visible = TRUE;
        }
    }
    winSpriteSetDisp(sprite, visible);
    return 0;
}

s32 fn_80053CE8(MenuCBPane* pane, MenuCBPane* sprite)
{
    s32 textId;
    void* pokemon;

    textId = 0;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        switch ((u8)menuSubGetPokemonSexForDisp(pokemon)) {
        case 0:
            textId = 0xd67;
            break;
        case 1:
            textId = 0xd68;
            break;
        case 2:
            break;
        }
    }
    sprite->textId = textId;
    return 0;
}

s32 fn_80053D64(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    s32 result;

    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        msgctrlSetValue(0x37, GSmsgGetGSchar(pokemonDataBiosGetName(
                                     pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))));
        fn_800FB680(0, 0, -1, 0xe7);
    }
    result = 0;
    sprite->textId = result;
    return result;
}

s32 fn_80053DD4(MenuCBPane* pane, MenuCBPane* sprite)
{
    void* pokemon;
    u32 level;
    s32 textId;
    u32 rect;

    textId = 0;
    pokemon = fn_80057270(pane);
    if (pokemon != NULL) {
        level = pokemonBiosGetLevel(pokemon);
        if ((s32)level < 100) {
            textId += 2;
        } else {
            textId += 3;
        }
        rect = GSmsgGetRect(0x1b82);
        fn_800FB680(sprite->width - (textId * 15) - (rect >> 16), 0, -1, 0x1b82);
        msgctrlSetValue(0x34, level);
        textId = 0xde;
    }
    sprite->textId = textId;
    return 0;
}
