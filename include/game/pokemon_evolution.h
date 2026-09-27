/**
 * @file pokemon_evolution.h
 * @brief Shared declarations and inline helpers of the pokemon evolution
 *        code (0x8012795C - 0x80128CC0).
 *
 * The helpers below are expanded, never called: each appears two or more
 * times in retail (see the comments) and has no out-of-line copy.
 */
#ifndef GAME_POKEMON_EVOLUTION_H
#define GAME_POKEMON_EVOLUTION_H

#include "dolphin/types.h"

/* A whole PokemonBios record (0x138 bytes), as the evolution code copies it. */
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


void pokemonSetLevelBasisStatus(u8* obj, u32 level);
s32 pokemonEvolutionAll(u8* pokemon, u16 species, u16 addSpecies,
                        u8* evolution, void* hero, s32 setMemo, s32 mode,
                        s32 useFade);
u16 getEvoPokemonLevelUp(u8* pokemon, u16* extra, u8* evolution);
u16 pokemonEvolutionCheck(u8* pokemon, s32 mode, u16 trigger, u16* extra, u8* evolution);

#endif /* GAME_POKEMON_EVOLUTION_H */
