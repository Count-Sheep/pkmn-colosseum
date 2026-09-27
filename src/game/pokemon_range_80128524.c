/**
 * @file pokemon_range_80128524.c
 * @brief pokemonEvolution, getEvoPokemonLevelUp and pokemonEvolutionCheck,
 *        0x80128524 - 0x80128CC0 (candidate).
 */
#include "game/pokemon_evolution.h"

s32 pokemonEvolution(u8* dst, u8* src, u16 species, u8* evolution, u16* waza)
{
    s32 count;
    u8* data;
    void* nickname;
    void* name;
    u8 level;
    u8 index;
    u16 move;

    count = 0;
    if (!pokemonCheckValid(src)) {
        return -1;
    }
    if (!pokemonDataCheckValid(0, species)) {
        return -1;
    }
    data = pokemonGetDataPtr(src);
    if (data == NULL) {
        return -1;
    }
    pokemonBiosCopy((u32*)dst, (u32*)src);
    nickname = pokemonBiosGetNicknamePtr(dst);
    name = (void*)GSmsgGetGSchar(pokemonDataBiosGetName(data));
    if (GScharCmp(nickname, name) == 0) {
        void* newName = (void*)GSmsgGetGSchar(pokemonDataBiosGetName(pokemonDataBiosGetPtr(species)));

        pokemonBiosSetNicknamePtr(dst, newName);
    }
    pokemonBiosSetPokemonDataId(dst, species);
    pokemonResetBasisStatus(dst);
    if (evolution[0] == 6) {
        pokemonDoItemSoubi(dst, 0, 0);
    }
    level = pokemonBiosGetLevel(dst);
    index = 0;
    while ((move = pokemonGetOboeWazaDataId(dst, level, &index)) != 0) {
        if (pokemonSearchWazaDataId(dst, move) == -1) {
            waza[count++] = move;
            if (count >= 20) {
                break;
            }
        }
        index++;
    }
    return count;
}

u16 getEvoPokemonLevelUp(u8* pokemon, u16* extra, u8* evolution)
{
    int i;
    u16 buff;
    u16 result;
    u8* data;
    u16 deferred;
    u8 kind;

    result = 0;
    deferred = 0;
    data = pokemonGetDataPtr(pokemon);
    if (data == NULL) {
        return 0xFFFF;
    }
    for (i = 0; i < 5; i++) {
        kind = pokemonDataBiosGetSinkaKind(data, i);
        buff = pokemonDataBiosGetSinkaBuff(data, i);
        switch (kind) {
        case 1:
            if (pokemonBiosGetFriend(pokemon) >= 220) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 2:
            pokemonBiosGetFriend(pokemon);
            break;
        case 3:
            pokemonBiosGetFriend(pokemon);
            break;
        case 4:
            if (pokemonBiosGetLevel(pokemon) >= buff) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 8:
            if (pokemonBiosGetLevel(pokemon) >= buff &&
                pokemonBiosGetPhyDef(pokemon) < pokemonBiosGetPhyAtk(pokemon)) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 9:
            if (pokemonBiosGetLevel(pokemon) >= buff &&
                pokemonBiosGetPhyDef(pokemon) == pokemonBiosGetPhyAtk(pokemon)) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 10:
            if (pokemonBiosGetLevel(pokemon) >= buff &&
                pokemonBiosGetPhyDef(pokemon) > pokemonBiosGetPhyAtk(pokemon)) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 11:
            if (pokemonBiosGetLevel(pokemon) >= buff &&
                (pokemonBiosGetRnd(pokemon) >> 16) % 10 < 5) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 12:
            if (pokemonBiosGetLevel(pokemon) >= buff &&
                (pokemonBiosGetRnd(pokemon) >> 16) % 10 >= 5) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        case 14:
            if (pokemonBiosGetLevel(pokemon) >= buff && heroGetFreePartySlot(NULL) >= 0) {
                deferred = pokemonDataBiosGetSinkaPokemonDataId(data, i);
            }
            break;
        case 15:
            if (pokemonBiosGetBeautiful(pokemon) >= buff) {
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
            }
            break;
        }
    }
    *extra = deferred;
    return result;
}

/*
 * Evolution by an item used on the Pokemon (sinka kind 7, buff = item).
 * pokemonEvolutionCheck's mode-1 body; its result reaches the caller
 * through a temp copied into r3 at the end of the case.
 */
static inline u16 getEvoPokemonItem(u8* pokemon, u16 item, u8* evolution)
{
    u8* data;
    int i;
    u16 result;
    u8 kind;
    u16 buff;

    result = 0;
    data = pokemonGetDataPtr(pokemon);
    if (data == NULL) {
        result = 0xFFFF;
    } else {
        for (i = 0; i < 5; i++) {
            kind = pokemonDataBiosGetSinkaKind(data, i);
            buff = pokemonDataBiosGetSinkaBuff(data, i);
            switch (kind) {
            case 7:
                if (buff == item) {
                    result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                    evolution[0] = kind;
                    *(u16*)(evolution + 2) = buff;
                }
                break;
            }
        }
    }
    return result;
}

/*
 * Evolution by trade (sinka kind 5, or kind 6 while holding the buff
 * item). pokemonEvolutionCheck's mode-2 body, returned the same way.
 */
static inline u16 getEvoPokemonTrade(u8* pokemon, u8* evolution)
{
    u8* data;
    int i;
    u16 result;
    u8 kind;
    u16 buff;

    result = 0;
    data = pokemonGetDataPtr(pokemon);
    if (data == NULL) {
        result = 0xFFFF;
    } else {
        for (i = 0; i < 5; i++) {
            kind = pokemonDataBiosGetSinkaKind(data, i);
            buff = pokemonDataBiosGetSinkaBuff(data, i);
            switch (kind) {
            case 5:
                result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                evolution[0] = kind;
                *(u16*)(evolution + 2) = buff;
                break;
            case 6:
                if (buff == pokemonBiosGetItemDataId(pokemon)) {
                    result = pokemonDataBiosGetSinkaPokemonDataId(data, i);
                    evolution[0] = kind;
                    *(u16*)(evolution + 2) = buff;
                }
                break;
            }
        }
    }
    return result;
}

u16 pokemonEvolutionCheck(u8* pokemon, s32 mode, u16 trigger, u16* extra, u8* evolution)
{
    u16 value;
    u16 item;
    u16 result;

    value = 0;
    if (!pokemonCheckValid(pokemon)) {
        return 0xFFFF;
    }
    item = pokemonGetSoubiItemDataId(pokemon);
    if (item == 0) {
        item = 0;
    } else {
        item = itemDataBiosGetItemSoubiDataId(itemDataBiosGetPtr(item));
    }
    if (item == 0x26) {
        *extra = value;
        return 0;
    }
    switch (mode) {
    case 0:
        result = getEvoPokemonLevelUp(pokemon, &value, evolution);
        break;
    case 1:
        result = getEvoPokemonItem(pokemon, trigger, evolution);
        break;
    case 2:
        result = getEvoPokemonTrade(pokemon, evolution);
        break;
    default:
        result = 0xFFFF;
        break;
    }
    *extra = value;
    return result;
}

