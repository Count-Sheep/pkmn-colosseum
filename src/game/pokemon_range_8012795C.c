/**
 * @file pokemon_range_8012795C.c
 * @brief Pokemon level-stat and evolution code, 0x8012795C - 0x80128CC0.
 *
 * Standalone candidate for pokemonSetLevelBasisStatus and the evolution
 * functions after it. pokemonGetStatus (0x8012640C) is owned by
 * pokemon_get_status_exact_8012640C.c.
 */
#include "dolphin/types.h"

typedef struct PokemonBiosBuf {
    u32 data[0x4E];
} PokemonBiosBuf;

extern u32 pokemonGetStatus(u8* obj, u32 id, u32 selector, u32 d);
extern void pokemonSetStatus(u8* obj, u32 id, u32 selector, u32 subindex, u32 value);
extern u8 pokemonCheckValid(u8* pokemon);
extern u8 pokemonDataCheckValid(u32 a, u16 species);
extern void pokemonBiosCopy(u32* dst, u32* src);
extern u16 pokemonBiosGetPokemonDataId(u8* pokemon);
extern void pokemonBiosSetPokemonDataId(u8* pokemon, u16 species);
extern void* pokemonBiosGetNicknamePtr(u8* pokemon);
extern void pokemonBiosSetNicknamePtr(u8* pokemon, void* name);
extern void pokemonBiosSetItemDataId(u8* pokemon, u16 item);
extern u8 pokemonBiosGetLevel(u8* pokemon);
extern u16 pokemonBiosGetFriend(u8* pokemon);
extern u16 pokemonBiosGetPhyAtk(u8* pokemon);
extern u16 pokemonBiosGetPhyDef(u8* pokemon);
extern u32 pokemonBiosGetRnd(u8* pokemon);
extern u8 pokemonBiosGetBeautiful(u8* pokemon);
extern u16 pokemonBiosGetItemDataId(u8* pokemon);
extern u8* pokemonDataBiosGetPtr(u16 species);
extern u32 pokemonDataBiosGetName(u8* data);
extern u8 pokemonDataBiosGetSinkaKind(u8* data, u16 index);
extern u16 pokemonDataBiosGetSinkaBuff(u8* data, u16 index);
extern u16 pokemonDataBiosGetSinkaPokemonDataId(u8* data, u16 index);
extern u8* pokemonSeikakuDataBiosGetPtr(u8 nature);
extern u8 pokemonSeikakuDataBiosGetPhyAtkRateDataId(u8* data);
extern u8 pokemonSeikakuDataBiosGetPhyDefRateDataId(u8* data);
extern u8 pokemonSeikakuDataBiosGetNimblenessRateDataId(u8* data);
extern u8 pokemonSeikakuDataBiosGetSpeAtkRateDataId(u8* data);
extern u8 pokemonSeikakuDataBiosGetSpeDefRateDataId(u8* data);
extern u8* pokemonSeikakuRateDataBiosGetPtr(u8 rate);
extern u8 pokemonSeikakuRateDataBiosGetKake(u8* rate);
extern u8 pokemonSeikakuRateDataBiosGetWaru(u8* rate);
extern void pokemonResetBasisStatus(u8* pokemon);
extern void pokemonInitJoutai(u8* pokemon);
extern u32 pokemonDoItemSoubi(u8* pokemon, u32 item, u8 flag);
extern s8 pokemonSearchWazaDataId(u8* pokemon, u16 waza);
extern u16 pokemonGetOboeWazaDataId(u8* pokemon, u8 level, u8* index);
extern void pokemonWazaCreate(u8* pokemon, u32 slot, u32 waza);
extern u16 pokemonGetSoubiItemDataId(u8* pokemon);
extern u8* itemDataBiosGetPtr(u16 item);
extern u16 itemDataBiosGetItemSoubiDataId(u8* item);
extern u32 GSmsgGetGSchar(u32 msg);
extern s32 GScharCmp(void* a, void* b);
extern u8* heroGetStatus(void* hero, u32 selector, u16 index);
extern u32 heroAddPokemon(void* hero, void* pokemon);
extern void memoDataSet(u32 mode, void* pokemon);
extern void fadeSet(f32 duration, u32 mode);
extern void fadeCheck(u32 wait);
extern s32 evolutionOpen(u8* pokemon, void* evolved, s32 mode, u16* waza,
                         s32 count, u8* slots);

/*
 * The species data of a Pokemon, or NULL when it has no species. Expanded
 * six times in this range (twice each in pokemonEvolutionCreateAddPokemon
 * and pokemonEvolutionCheck, once each in pokemonEvolution and
 * getEvoPokemonLevelUp) with the same NULL-on-zero-id path.
 */
static inline u8* pokemonGetDataPtr(u8* pokemon)
{
    u16 species = pokemonBiosGetPokemonDataId(pokemon);

    if (species == 0) {
        return NULL;
    }
    return pokemonDataBiosGetPtr(species);
}

/*
 * First free party slot of hero (0-5), or -1 when the party is full.
 * Expanded in pokemonEvolutionAll and getEvoPokemonLevelUp.
 */
static inline s8 heroGetFreePartySlot(void* hero)
{
    int i;

    for (i = 0; i < 6; i++) {
        if (!pokemonCheckValid(heroGetStatus(hero, 3, i))) {
            break;
        }
    }
    if (i < 6) {
        return i;
    }
    return -1;
}

s32 pokemonEvolution(u8* dst, u8* src, u16 species, u8* evolution, u16* waza);
s32 pokemonEvolutionCreateAddPokemon(u8* dst, u8* src, u16 species);

/*
 * Give hero the extra Pokemon an evolution leaves behind (the Shedinja
 * case): a copy of the evolved Pokemon as species, when the party has room.
 * Its species guard is tested again after the caller's own test (retail
 * branches twice on the same condition register).
 */
static inline void pokemonEvolutionAddPokemon(void* hero, u8* pokemon, u16 species, s32 setMemo)
{
    PokemonBiosBuf added;

    if (species == 0) {
        return;
    }
    if (heroGetFreePartySlot(hero) < 0) {
        return;
    }
    pokemonEvolutionCreateAddPokemon((u8*)&added, pokemon, species);
    heroAddPokemon(hero, &added);
    if (setMemo != 0) {
        memoDataSet(0, &added);
    }
}

/*
 * Nature-adjusted value of one stat: value * kake / waru of the nature's
 * rate entry for that stat (kind 0 is HP, which no nature changes).
 */
static inline s32 pokemonSeikakuAdjustStatus(s32 value, u8 nature, s32 kind)
{
    u8* data;
    u8* rate;
    u8 kake;
    u8 waru;

    data = pokemonSeikakuDataBiosGetPtr(nature);
    if (data == NULL) {
        return value;
    }
    switch (kind) {
    case 0:
        return value;
    case 1:
        rate = pokemonSeikakuRateDataBiosGetPtr(pokemonSeikakuDataBiosGetPhyAtkRateDataId(data));
        break;
    case 2:
        rate = pokemonSeikakuRateDataBiosGetPtr(pokemonSeikakuDataBiosGetPhyDefRateDataId(data));
        break;
    case 3:
        rate = pokemonSeikakuRateDataBiosGetPtr(pokemonSeikakuDataBiosGetNimblenessRateDataId(data));
        break;
    case 4:
        rate = pokemonSeikakuRateDataBiosGetPtr(pokemonSeikakuDataBiosGetSpeAtkRateDataId(data));
        break;
    default:
        rate = pokemonSeikakuRateDataBiosGetPtr(pokemonSeikakuDataBiosGetSpeDefRateDataId(data));
        break;
    }
    if (rate == NULL) {
        return 0;
    }
    kake = pokemonSeikakuRateDataBiosGetKake(rate);
    waru = pokemonSeikakuRateDataBiosGetWaru(rate);
    value *= kake;
    if (waru != 0) {
        value /= waru;
    }
    return value;
}

/*
 * Stat of obj at level from the species base stat (baseSel), the random
 * value (rndSel) and the effort value (effortSel); expanded six times in
 * pokemonSetLevelBasisStatus, HP first.
 */
static inline s32 pokemonCalcLevelStatus(u8* obj, u8 level, u32 baseSel, u32 rndSel,
                                         u32 effortSel, s32 kind)
{
    u16 species;
    u8 nature;
    u16 base;
    u16 rnd;
    u16 effort;
    s32 value;

    species = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    nature = (u8)pokemonGetStatus(obj, 0, 0xBF, 0);
    base = (u16)pokemonGetStatus(NULL, species, baseSel, 0);
    rnd = (u16)pokemonGetStatus(obj, 0, rndSel, 0);
    effort = (pokemonGetStatus(obj, 0, effortSel, 0) >> 2) & 0x3FFF;
    if (kind == 0) {
        value = level + 10 + level * (rnd + base * 2 + effort) / 100;
    } else {
        value = level * (rnd + base * 2 + effort) / 100 + 5;
    }
    return pokemonSeikakuAdjustStatus(value, nature, kind);
}

void pokemonSetLevelBasisStatus(u8* obj, u32 level)
{
    u16 species;
    u16 oldMaxHp;
    u16 maxHp;
    u16 hp;

    species = (u16)pokemonGetStatus(obj, 0, 0x6E, 0);
    oldMaxHp = (u16)pokemonGetStatus(obj, 0, 0x87, 0);
    pokemonSetStatus(obj, 0, 0x7A, 0, (u8)level);
    if (species == 0x12F) {
        maxHp = 1;
        pokemonSetStatus(obj, 0, 0x87, 0, 1);
    } else {
        s32 value = pokemonCalcLevelStatus(obj, level, 3, 0x93, 0x8D, 0);
        pokemonSetStatus(obj, 0, 0x87, 0, value);
        maxHp = value;
    }
    pokemonSetStatus(obj, 0, 0x88, 0, pokemonCalcLevelStatus(obj, level, 4, 0x94, 0x8E, 1));
    pokemonSetStatus(obj, 0, 0x89, 0, pokemonCalcLevelStatus(obj, level, 5, 0x95, 0x8F, 2));
    pokemonSetStatus(obj, 0, 0x8C, 0, pokemonCalcLevelStatus(obj, level, 8, 0x98, 0x92, 3));
    pokemonSetStatus(obj, 0, 0x8A, 0, pokemonCalcLevelStatus(obj, level, 6, 0x96, 0x90, 4));
    pokemonSetStatus(obj, 0, 0x8B, 0, pokemonCalcLevelStatus(obj, level, 7, 0x97, 0x91, 5));
    hp = (u16)pokemonGetStatus(obj, 0, 0x83, 0);
    if (hp != 0 || oldMaxHp == 0) {
        if (species == 0x12F) {
            hp = 1;
        } else {
            hp = hp + (maxHp - oldMaxHp);
        }
        pokemonSetStatus(obj, 0, 0x83, 0, hp);
    }
}

s32 pokemonEvolutionAll(u8* pokemon, u16 species, u16 addSpecies,
                        u8* evolution, void* hero, s32 setMemo, s32 mode,
                        s32 useFade)
{
    u8 slots[20];
    u16 waza[20];
    u16 selected[20];
    PokemonBiosBuf evolved;
    s32 count;
    s32 result;
    int i;

    if (!pokemonCheckValid(pokemon)) {
        return 2;
    }
    if (species == 0) {
        return 2;
    }
    count = pokemonEvolution((u8*)&evolved, pokemon, species, evolution, waza);
    if (count < 0) {
        return 2;
    }
    if (useFade != 0) {
        fadeSet(0.5f, 3);
        fadeCheck(1);
    }
    for (i = 0; i < count; i++) {
        selected[i] = waza[i];
    }
    result = evolutionOpen(pokemon, &evolved, mode, selected, count, slots);
    if (useFade != 0) {
        fadeSet(0.5f, 2);
        fadeCheck(1);
    }
    if (result == 2) {
        return 2;
    }
    if (result == 1) {
        return 1;
    }
    if (setMemo != 0) {
        memoDataSet(0, &evolved);
    }
    pokemonBiosCopy((u32*)pokemon, evolved.data);
    for (i = 0; i < count; i++) {
        if (slots[i] != 0xFF) {
            pokemonWazaCreate(pokemon, slots[i], waza[i]);
        }
    }
    if (addSpecies != 0) {
        pokemonEvolutionAddPokemon(hero, pokemon, addSpecies, setMemo);
    }
    return 0;
}

s32 pokemonEvolutionCreateAddPokemon(u8* dst, u8* src, u16 species)
{
    u16 clearA[] = { 0xB0, 0xB1, 0xB2, 0xB3, 0xB4 };
    u16 clearB[] = { 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
                     0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE };
    u8* data;
    void* name;
    int i;

    if (!pokemonDataCheckValid(0, species)) {
        return 0;
    }
    if (pokemonGetDataPtr(src) == NULL) {
        return 0;
    }
    pokemonBiosCopy((u32*)dst, (u32*)src);
    pokemonBiosSetPokemonDataId(dst, species);
    data = pokemonGetDataPtr(dst);
    if (data == NULL) {
        return 0;
    }
    name = (void*)GSmsgGetGSchar(pokemonDataBiosGetName(data));
    pokemonBiosSetNicknamePtr(dst, name);
    pokemonBiosSetItemDataId(dst, 0);
    pokemonSetStatus(dst, 0, 0xBB, 0, 0);
    pokemonSetStatus(dst, 0, 0xBE, 0, 0);
    for (i = 0; i < 5; i++) {
        pokemonSetStatus(dst, 0, clearA[i], 0, 0);
    }
    for (i = 0; i < 12; i++) {
        pokemonSetStatus(dst, 0, clearB[i], 0, 0);
    }
    pokemonSetStatus(dst, 0, 0xAF, 0, 0);
    pokemonInitJoutai(dst);
    pokemonSetStatus(dst, 0, 0xBC, 0, 0);
    pokemonResetBasisStatus(dst);
    return 1;
}

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
        name = (void*)GSmsgGetGSchar(pokemonDataBiosGetName(pokemonDataBiosGetPtr(species)));
        pokemonBiosSetNicknamePtr(dst, name);
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
            waza[count] = move;
            count++;
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
