/**
 * @file menuCB_range_800676EC.c
 * @brief Residual menuCB candidate range, 0x800676EC - 0x80068738.
 */
#define MENUCB_RANGE_RESIDUAL_EMPTY_ONLY
#include "menuCB_range_80062948.c"

typedef struct MenuCBContext676EC {
    u8 pad_00[0x95];
    u8 state95;
    u8 pad_96[2];
    u8 state98;
    u8 state99;
} MenuCBContext676EC;

typedef struct MenuCBAfe4 {
    u8 pad[4];
    s32 type;
} MenuCBAfe4;

extern u8 lbl_803A9F08[];
extern u32 lbl_802EDB64[];
extern f32 lbl_8047BFE8;
extern f32 lbl_8047BFEC;
extern MenuCBAfe4* fn_8006AFE4(s32);
extern s32 fn_8006B1D4(void);
extern u8 fn_8008ABA0(s32);
extern u32 fn_800F7AF0(void);
extern u32 fn_800F7BC4(s32);
extern void fn_80166AB8(u32, u32, u32);
extern s32 fn_8025D9CC(void);
extern s32 toolentryTaisenDeleteEtnryPokemonOrder(s32);
extern u16 toolentryTaisenGetBattlePlayerID(s32);
extern s32 toolentryTaisenGetBattleType(void);
extern s32 toolentryTaisenGetControlerType(s32);
extern s32 toolentryTaisenGetEntryPlayerNum(void);
extern s32 toolentryTaisenGetHomePlace(s32);
extern s32 toolentryTaisenGetPokemonNum(s32);
extern s32 toolentryTaisenSetEtnryPokemonOrder(s32, s32);
extern s32 toolentryTaisengetEtnryPokemonOrderNum(s32);

static inline void menuCBSetAnimTrack(s32 player, s32 slot)
{
    f32* track = (f32*)(lbl_803A9F08 + player * 0x30 + 0xCD8C);

    track[slot] = (f32)((5 - slot) * 0x18);
    track[slot + 6] = lbl_8047BFE8;
}

static inline void menuCBEntryCheckLinkLost(int player)
{
    s32 type;
    s32 controller;

    if (fn_8025D9CC() == 4 && lbl_803A9F08[(s32)player + 4] != 0) {
        type = fn_8006AFE4(toolentryTaisenGetControlerType(player))->type;
        if (type == 1 || type == 2) {
            controller = toolentryTaisenGetControlerType(player);
            if (fn_8008ABA0(controller) == 0) {
                *(volatile u8*)&lbl_803A9F08[0xCE58] = 0;
                if (*(volatile s32*)&lbl_803A9F08[0xCE5C] < 0) {
                    *(s32*)&lbl_803A9F08[0xCE5C] = controller;
                }
            }
        }
    }
}

static inline void menuCBEntryPadInput(int player)
{
    s32 controller;
    s32 selection;
    u32 buttons;
    s32 count;
    s32 slot;
    u16 maxPokemon;

    controller = toolentryTaisenGetControlerType(player);
    buttons = fn_800F7BC4(controller) & fn_800F7AF0();

    toolentryTaisenGetBattlePlayerID(player);
    if ((buttons & 0x40) != 0) {
        count = toolentryTaisengetEtnryPokemonOrderNum(player);
        if (count != toolentryTaisenDeleteEtnryPokemonOrder(player)) {
            fn_80166AB8(0x25, 0, 0);
        }
    } else if ((buttons & 0xC0F) != 0) {
        selection = -1;
        maxPokemon = toolentryTaisenGetPokemonNum(player);

        if (buttons & 1) {
            selection = 0;
        }
        if (buttons & 8) {
            selection = 1;
        }
        if (buttons & 0x800) {
            selection = 2;
        }
        if (buttons & 4) {
            selection = 3;
        }
        if (buttons & 2) {
            selection = 4;
        }
        if (buttons & 0x400) {
            selection = 5;
        }
        if (maxPokemon <= selection) {
            selection = -1;
        }
        if (selection >= 0) {
            slot = toolentryTaisenSetEtnryPokemonOrder(player, selection);
            if (slot >= 0) {
                fn_80166AB8(0x3C3, 0, 0);
                menuCBSetAnimTrack(player, slot);
            }
        }
    }
}

static inline void menuCBEntryCpuInput(int player)
{
    s32 selection;
    u32 buttons;
    s32 slot;
    u16 maxPokemon;

    toolentryTaisenGetControlerType(player);
    toolentryTaisenGetBattlePlayerID(player);
    *(f32*)&lbl_803A9F08[0xCE4C] =
        *(f32*)&lbl_803A9F08[0xCE4C] + *(f32*)&lbl_803A9F08[0xCD88];
    if (*(volatile f32*)&lbl_803A9F08[0xCE4C] >= lbl_8047BFEC) {
        *(volatile f32*)&lbl_803A9F08[0xCE4C] = lbl_8047BFE8;
        {
            s32 idx = *(volatile s32*)&lbl_803A9F08[0xCE50];
            buttons = ((volatile u32*)lbl_802EDB64)[idx];
            *(volatile s32*)&lbl_803A9F08[0xCE50] = idx + 1;
        }
    } else {
        buttons = 0;
    }
    if ((buttons & 0xC0F) != 0) {
        selection = -1;
        maxPokemon = toolentryTaisenGetPokemonNum(player);
        if (buttons & 1) {
            selection = 0;
        }
        if (buttons & 8) {
            selection = 1;
        }
        if (buttons & 0x800) {
            selection = 2;
        }
        if (buttons & 4) {
            selection = 3;
        }
        if (buttons & 2) {
            selection = 4;
        }
        if (buttons & 0x400) {
            selection = 5;
        }
        slot = toolentryTaisenSetEtnryPokemonOrder(
            player, (maxPokemon <= selection) ? -1 : selection);
        if (slot >= 0) {
            fn_80166AB8(0x3C3, 0, 0);
            menuCBSetAnimTrack(player, slot);
        }
    }
}

void fn_800679C0(MenuCBContext676EC* context, s32 startPlayer)
{
    s32 player;
    s32 entryPlayers;
    s32 type;

    entryPlayers = toolentryTaisenGetEntryPlayerNum();
    toolentryTaisenGetBattleType();
    toolentryTaisenGetHomePlace(0);
    toolentryTaisenGetHomePlace(1);
    toolentryTaisenGetHomePlace(2);
    toolentryTaisenGetHomePlace(3);

    player = (startPlayer != 0) ? 1 : 0;

    while (player < entryPlayers) {
        menuCBEntryCheckLinkLost(player);

        if (lbl_803A9F08[(u32)player + 4] == 0) {
            if (fn_8025D9CC() == 4) {
                type = fn_8006AFE4(toolentryTaisenGetControlerType(player))->type;
                if (type == 1 || type == 2) {
                    _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                        toolentryTaisenGetControlerType(player), player);
                } else {
                    menuCBEntryPadInput(player);
                }
            } else if (player == 1) {
                menuCBEntryCpuInput(player);
            } else {
                switch ((u16)toolentryTaisenGetHomePlace(player)) {
                case 0:
                    menuCBEntryPadInput(player);
                    break;
                case 1:
                case 2:
                    _menuCBPokemonEntryEntCheckGBA__F13GSinputDevicel(
                        toolentryTaisenGetControlerType(player), player);
                    break;
                default:
                    menuCBEntryPadInput(player);
                    break;
                }
            }

            if ((toolentryTaisenGetControlerType(player) == 1) &&
                ((u16)toolentryTaisenGetHomePlace(player) == 0)) {
                s32 orderCount = toolentryTaisengetEtnryPokemonOrderNum(player);
                u16 pokemonCount = fn_8006B1D4();
                u16 limit = toolentryTaisenGetPokemonNum(player);

                if (orderCount == (u16)((limit < pokemonCount) ? limit : pokemonCount)) {
                    s32 prev = orderCount - 1;
                    if (prev < 0) {
                        prev = 0;
                    }
                    if (lbl_8047BFE8 ==
                        *(f32*)(lbl_803A9F08 + (u32)player * 0x30 + prev * 4 + 0xCD8C)) {
                        context->state95 = 1;
                        context->state98 = 1;
                    }
                }
            }
        }

        player += 1;
    }
}
