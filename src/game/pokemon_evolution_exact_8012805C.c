/**
 * @file pokemon_evolution_exact_8012805C.c
 * @brief pokemonEvolutionAll and pokemonEvolutionCreateAddPokemon,
 *        0x8012805C - 0x80128524, with the data they own.
 *
 * Function-boundary carve of the evolution code. It owns:
 *  - .sdata2 0x8047D020 (0.5f, the fade time in pokemonEvolutionAll). It is
 *    the only float the evolution functions use, and it sits in its own
 *    8-aligned piece between the pokemon TU's pool (0x8047CFF0 - 0x8047D018
 *    + pad, used from 0x8011F77C on) and hero.c's 0x8047D028, so it is this
 *    code's whole pool.
 *  - .rodata 0x80272998 / 0x802729A4, the initialisers of the two status
 *    lists pokemonEvolutionCreateAddPokemon clears; nothing else in the
 *    evolution code has .rodata.
 */
#include "game/pokemon_evolution.h"

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

