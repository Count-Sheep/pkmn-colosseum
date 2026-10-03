/**
 * @file gs_npc_event_candidate_8003037C_r40_80030A44_gc20.c
 * @brief fn_80030A44 carve, 0x80030A44 - 0x80030C14.
 *
 * NPC event callback: look the event id up in the nickname column of the
 * two-entry table lbl_80266F68, fetch the party (1) or storage (2)
 * Pokemon it names, print its nickname and, for a valid event Pokemon,
 * the gender symbol after it.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void fn_800FB680(s32, s32, u32, u16);
extern u32 GSmsgGetRect(u32 msg);
extern s32 GSmsgGetGSchar(u32 msg);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u8 menuCBRule_CheckPokemonEventFlag(void* pokemon);
extern u8 menuSubGetPokemonSexForDisp(void* pokemon);

extern u8 lbl_80266F68[];
extern u8 lbl_803A2688[];
extern u32 lbl_8047A420;
extern u32 lbl_8047A424;

void fn_80030A44(u8* r3, u8* r4)
{
    u32 combined;
    void* pokemon;
    u8* table;
    s32 kind;
    u32 msg;
    u16 width;
    s32 i;

    pokemon = NULL;
    combined = (*(u32*)(r4 + 0x64) & ~0xFF) | *(u8*)(r3 + 0x8B);
    kind = 0;
    table = lbl_80266F68;
    for (i = 0; i < 2; i++) {
        if (*(s16*)(r4 + 0x6) == *(u16*)(table + 0x4)) {
            kind = *(u16*)(table + 0x0);
        }
        table += 0x14;
    }

    switch (kind) {
    case 1:
        pokemon = (void*)heroGetStatus(NULL, 3, (u16)lbl_8047A424);
        break;
    case 2:
        pokemon = (void*)heroGetStatus(lbl_803A2688, 3, (u16)lbl_8047A420);
        break;
    }

    if (pokemon != NULL) {
        msgctrlSetValue(0x37, (s32)pokemonBiosGetNicknamePtr(pokemon));
        fn_800FB680(0, 0, combined, 0xE7);
        winSpriteSetDisp(r4, 1);
        if (pokemonCheckValid(pokemon) != 0 && menuCBRule_CheckPokemonEventFlag(pokemon) == 1) {
            switch (menuSubGetPokemonSexForDisp(pokemon)) {
            case 0:
                msg = 0xD67;
                break;
            case 1:
                msg = 0xD68;
                break;
            case 2:
            default:
                msg = 0;
                break;
            }
            if (msg != 0) {
                width = (s16)(GSmsgGetRect(0xE7) >> 16);
                msgctrlSetValue(0x37, GSmsgGetGSchar(msg));
                fn_800FB680(width + 2, 0, combined, 0xCF);
            }
        }
    } else {
        winSpriteSetDisp(r4, 0);
    }
}
