/**
 * @file pokemon_range_8012795C.c
 * @brief pokemonSetLevelBasisStatus, 0x8012795C - 0x8012805C.
 */
#include "game/pokemon_evolution.h"

/*
 * Nature-adjusted value of one stat: value * kake / waru of the nature's
 * rate entry for that stat (kind 0 is HP, which no nature changes).
 * nature is u32 as in XD's pokemonAdjustValueBySeikaku(u32, u16, u32)
 * (github.com/TeamOrre/xd-decomp @ 4989794e, pokemon.cpp); the
 * widened argument copy puts the /100 constant in r3 as retail does.
 * The separate result local is XD's too; it gives the value and rate
 * temps retail's registers.
 */
static inline s32 pokemonSeikakuAdjustStatus(s32 value, u32 nature, s32 kind)
{
    u8* data;
    u8* rate;
    s32 result;
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
    result = value * kake;
    if (waru != 0) {
        result /= waru;
    }
    return result;
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
    s32 value;
    u16 base;
    u16 rnd;
    u8 nature;
    u16 effort;

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
    /* A mask, not a (u8) cast: the cast's argument temp swaps obj and oldMaxHp. */
    pokemonSetStatus(obj, 0, 0x7A, 0, level & 0xFF);
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
        u16 newHp = 1;
        if (species != 0x12F) {
            newHp = hp + (maxHp - oldMaxHp);
        }
        pokemonSetStatus(obj, 0, 0x83, 0, newHp);
    }
}

