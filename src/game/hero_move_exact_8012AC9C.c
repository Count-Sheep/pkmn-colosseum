/**
 * @file hero_move_exact_8012AC9C.c
 * @brief heroMove TU start: cbTsureFriend, .text 0x8012AC9C-0x8012AD50.
 *
 * Text-only unit carved from the hero_move range at a function boundary.
 * The hero-move state (.bss lbl_80426BD0) stays extern. Built with the
 * hero_move TU's GC/1.3 -O4,p flags; no pragma is active.
 */
#include "game/hero_move.h"

extern HeroMoveWork lbl_80426BD0;

extern void* heroGetStatus(void* hero, u32 selector, u16 index);
extern u8 pokemonCheckValid(void* pokemon);
extern u16 pokemonBiosGetItemDataId(void* pokemon);
extern void* itemDataBiosGetPtr(u16 itemId);
extern u32 itemDataBiosGetItemSoubiDataId(void* item);
extern void pokemonGetFriendFormPokemonFriendFilterId(void* pokemon, u32 soubiId, u32 filter);

/*
 * Step callback registered by fn_8013024C: every 256 steps, apply the
 * walking friendship filter to each valid party Pokemon, passing its held
 * item's equip ID (0 when it holds nothing).
 */
void cbTsureFriend__Fl15FootStepCounterl(s32 arg)
{
    s32 i;
    void* pokemon;
    void* item;
    u32 soubiId;

    lbl_80426BD0.friendSteps++;
    if (lbl_80426BD0.friendSteps < 0x100) {
        return;
    }
    lbl_80426BD0.friendSteps = 0;

    for (i = 0; i < 6; i++) {
        pokemon = heroGetStatus(NULL, 3, i);
        if (pokemon != NULL && pokemonCheckValid(pokemon)) {
            item = itemDataBiosGetPtr(pokemonBiosGetItemDataId(pokemon));
            if (item == NULL) {
                soubiId = 0;
            } else {
                soubiId = itemDataBiosGetItemSoubiDataId(item);
            }
            pokemonGetFriendFormPokemonFriendFilterId(pokemon, soubiId, 5);
        }
    }
}
