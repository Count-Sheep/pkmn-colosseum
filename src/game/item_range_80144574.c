/**
 * @file item_range_80144574.c
 * @brief item-use game code, 0x80144574 - 0x8014635C (1 fn).
 *
 * Split out of the misnamed people_field.c unit (2026-07-02). fn_80144574
 * (size 0x1DE8) is item-use-on-Pokemon logic (calls hpRecover__FP20...);
 * it ends exactly at seqGetPrivateId (0x8014635C), the first MusyX seq.c
 * function. Asm-only until matched; the range name stays honest until the
 * function is decompiled.
 */
#include "dolphin/types.h"

typedef struct ItemUsePokemonLog {
    s32 type;
    u16 value;
    u16 extra;
} ItemUsePokemonLog;

extern u8* itemDataBiosGetPtr(u16 index);
extern u8 itemDataBiosGetItemEffectParam(u8* item);
extern void itemParamConvertOrigFormat(u8* dst, u8* src);
extern u8* itemParamGetPtr(u8 idx);
extern u8 itemParamGetMeromeroFlag(u8* p);
extern u8 itemParamGetCriticalFlag(u8* p);
extern u8 itemParamGetAttackUp(u8* p);
extern u8 itemParamGetDefenceUp(u8* p);
extern u8 itemParamGetQuickUp(u8* p);
extern u8 itemParamGetHitUp(u8* p);
extern u8 itemParamGetSpAttackUp(u8* p);
extern u8 itemParamGetGuardFlag(u8* p);
extern u8 itemParamGetLevelUpFlag(u8* p);
extern u8 itemParamGetSleepFlag(u8* p);
extern u8 itemParamGetPoisonFlag(u8* p);
extern u8 itemParamGetBurnFlag(u8* p);
extern u8 itemParamGetFreezeFlag(u8* p);
extern u8 itemParamGetParalyzeFlag(u8* p);
extern u8 itemParamGetConfuseFlag(u8* p);
extern u8 itemParamGetPPMaxUpFlag(u8* p);
extern u8 itemParamGetPPMaxFullFlag(u8* p);
extern u8 itemParamGetHPEffortUp(u8* p);
extern u8 itemParamGetAttackEffortUp(u8* p);
extern u8 itemParamGetDefenceEffortUp(u8* p);
extern u8 itemParamGetQuickEffortUp(u8* p);
extern u8 itemParamGetSpDefenceEffortUp(u8* p);
extern u8 itemParamGetSpAttackEffortUp(u8* p);
extern u8 itemParamGetHPUp(u8* p);
extern u8 itemParamGetReviveFlag(u8* p);
extern u8 itemParamGetPPUp(u8* p);
extern u8 itemParamGetPPSelectFlag(u8* p);
extern u8 itemParamGetEvolutionFlag(u8* p);
extern s8 itemParamGetFriend1Up(u8* p);
extern s8 itemParamGetFriend2Up(u8* p);
extern s8 itemParamGetFriend3Up(u8* p);
extern void* fightFloorGetFightPokemonPtrToFightTrainerPtr(s32, void*);
extern void* fightTrainerCheckFightPokemonFightOut(void*, void*);
extern void fn_802331A4(void* fightOutPokemon, s32 id);
extern void fn_8020248C(void*, s32, u32);
extern u8 fn_802025B8(void*, s32);
extern u8 fightOutPokemonCheckWriteJoutaiDataId(void* fightPokemon, s32 statusId);
extern void fightOutPokemonWriteJoutaiDataId(void* fightOutPokemon, s32 statusId);
extern u8 fn_802026E4(void* fightOutPokemon, s32 statusId);
extern u8 fn_80121ADC(void* pokemon, s32 statusId);
extern void fn_80121B4C(void* pokemon, s32 statusId);
extern void* fightTargetGetPtrAsNowFightType(s32 type, void* fightPokemon);
extern u8 fightSideCheckWriteJoutaiDataId(void* side, s32 statusId);
extern void fightSideWriteJoutaiDataId(void* side, s32 statusId, u32 value);
extern void* fightPokemonGetPokemonPtr(void* fightPokemon);
extern void* fightPokemonBiosGetMotoPokemonPtr(void* fightPokemon);
extern void hpRecover__FP20ITEMUSE2POKEMON_LOG1PsP7PokemonUcbUsP12FightPokemon(
    ItemUsePokemonLog* log, s16* logCount, void* pokemon, u8 recovery,
    u8 revive, u16 amount, void* fightPokemon);
extern s32 pokemonGetStatus(void* obj, u32 id, u32 selector, u32 subindex);
extern void pokemonSetStatus(
    void* obj, u32 id, u32 selector, u32 subindex, u32 value);
extern void pokemonResetBasisStatus(void* pokemon);
extern u8 pokemonIsDarkPokemon(void* pokemon);
extern u8 pokemonWazaCheckValid(void* pokemon, u8 wazaNum);
extern u8 pokemonWazaGetMaxPP(void* pokemon, u8 wazaNum);
extern s32 pokemonBiosGetPokemonWazaPp(void* pokemon, u8 wazaNum);
extern void pokemonBiosSetPokemonWazaPp(void* pokemon, u8 wazaNum, u8 value);
extern u8 pokemonBiosGetPokemonWazaPpCount(void* pokemon, u8 wazaNum);
extern void pokemonBiosSetPokemonWazaPpCount(void* pokemon, u8 wazaNum, u8 value);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u8 wazaNum);
extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
extern u8 pokemonBiosGetLevel(void* pokemon);
extern u16 pokemonBiosGetMaxHp(void* pokemon);
extern u16 pokemonBiosGetMaxHpEffort(void* pokemon);
extern u16 pokemonBiosGetPhyAtkEffort(void* pokemon);
extern u16 pokemonBiosGetPhyDefEffort(void* pokemon);
extern u16 pokemonBiosGetSpeAtkEffort(void* pokemon);
extern u16 pokemonBiosGetSpeDefEffort(void* pokemon);
extern u16 pokemonBiosGetNimblenessEffort(void* pokemon);
extern void pokemonBiosSetExp(void* pokemon, u32 exp);
extern u8 pokemonDataBiosGetGrowDataId(u8* data);
extern u8* pokemonDataBiosGetPtr(u16 species);
extern u32 pokemonGrowDataBiosGetExp(void* growData, u8 level);
extern void* pokemonGrowDataBiosGetPtr(u8 growDataId);
extern u16 pokemonEvolutionCheck(
    void* pokemon, s32 mode, u16 trigger, u16* outSpecies, u8* outMode);
extern s16 friendXUp__FP7PokemonP12FightPokemonScUsUs(
    void* pokemon, void* fightPokemon, s8 delta, u16 minFriend, u16 maxFriend);

extern u8 lbl_80478BE8[8];

static inline u8* itemUseGetParam(u16 itemDataId) {
    u8* item;
    u8 effect;

    item = itemDataBiosGetPtr(itemDataId);
    if (item == NULL) {
        return NULL;
    }
    effect = itemDataBiosGetItemEffectParam(item);
    if (effect == 0) {
        return NULL;
    }
    return itemParamGetPtr(effect);
}

static inline void* itemUseGetPokemon(void* fightPokemon) {
    if (fightPokemon == NULL) {
        return NULL;
    }
    return fightPokemonGetPokemonPtr(fightPokemon);
}

static inline void* itemUseGetFightOut(void* fightPokemon) {
    if (fightPokemon == NULL) {
        return NULL;
    }
    return fightTrainerCheckFightPokemonFightOut(
        fightFloorGetFightPokemonPtrToFightTrainerPtr(0, fightPokemon), fightPokemon);
}

static inline void itemUseNotify(void* fightPokemon, s32 id) {
    void* fightOut;

    fightOut = itemUseGetFightOut(fightPokemon);
    if (fightOut != NULL) {
        fn_802331A4(fightOut, id);
    }
}

static inline void itemUseAddLog(
    ItemUsePokemonLog* log, s16* count, s32 type, s16 value, s16 extra)
{
    s16 n;

    n = *count;
    if (n < 0x20) {
        log[n].type = type;
        log[n].value = value;
        log[n].extra = extra;
        *count = n + 1;
    }
}

/* Raise a battle stat stage (capped at +12); returns the stages gained. */
static inline u8 itemUseStatUp(void* fightPokemon, u32 statusId, u8 boost) {
    u8 oldStage;
    u8 newStage;
    void* fightOut;

    fightOut = itemUseGetFightOut(fightPokemon);
    if (fightOut == NULL) {
        return 0;
    }
    oldStage = pokemonGetStatus(fightOut, 0, statusId, 0);
    if (oldStage >= 0xC) {
        return 0;
    }
    newStage = oldStage + boost;
    if (newStage > 0xC) {
        newStage = 0xC;
    }
    pokemonSetStatus(fightOut, 0, statusId, 0, newStage);
    itemUseNotify(fightPokemon, 0x99);
    return newStage - oldStage;
}

/* Raise one effort value (cap 100 each, 510 total); -1 when the total is
 * already full, otherwise the points gained. */
static inline s16 itemUseEffortUp(
    void* pokemon, void* fightPokemon, u32 statusId, u8 boost)
{
    u16 total;
    u16 oldEffort;
    u16 newEffort;
    u16 newTotal;

    total = pokemonBiosGetMaxHpEffort(pokemon);
    total += pokemonBiosGetPhyAtkEffort(pokemon);
    total += pokemonBiosGetPhyDefEffort(pokemon);
    total += pokemonBiosGetSpeAtkEffort(pokemon);
    total += pokemonBiosGetSpeDefEffort(pokemon);
    total += pokemonBiosGetNimblenessEffort(pokemon);
    if (total >= 0x1FE) {
        return -1;
    }
    oldEffort = pokemonGetStatus(pokemon, 0, statusId, 0);
    if (oldEffort >= 0x64) {
        return 0;
    }
    newEffort = oldEffort + boost;
    if (newEffort > 0x64) {
        newEffort = 0x64;
    }
    newTotal = total - oldEffort;
    newTotal += newEffort;
    if (newTotal > 0x1FE) {
        newEffort -= newTotal - 0x1FE;
    }
    pokemonSetStatus(pokemon, 0, statusId, 0, newEffort);
    pokemonResetBasisStatus(pokemon);
    itemUseNotify(fightPokemon, statusId);
    return (u16)(newEffort - oldEffort);
}

static inline u8 itemUseEffortUpLog(
    ItemUsePokemonLog* log, s16* count, void* pokemon, void* fightPokemon,
    u32 statusId, u8 boost, s32 type)
{
    s16 gained;

    gained = itemUseEffortUp(pokemon, fightPokemon, statusId, boost);
    if (gained < 0) {
        return FALSE;
    }
    if (gained == 0) {
        return TRUE;
    }
    itemUseAddLog(log, count, type, gained, 0);
    return TRUE;
}

/* Restore PP of one move; returns the PP gained. */
static inline u8 itemUsePPUp(void* pokemon, s32 slot, u8 boost) {
    u16 wazaDataId;
    u8 maxPp;
    u8 oldPp;
    u8 newPp;

    wazaDataId = pokemonBiosGetPokemonWazaDataId(pokemon, slot);
    if (wazaDataId == 0x165) {
        return 0;
    }
    if (wazaDataId == 0x164) {
        return 0;
    }
    maxPp = pokemonWazaGetMaxPP(pokemon, slot);
    oldPp = pokemonBiosGetPokemonWazaPp(pokemon, slot);
    if (oldPp >= maxPp) {
        return 0;
    }
    newPp = oldPp + boost;
    if (newPp > maxPp) {
        newPp = maxPp;
    }
    pokemonBiosSetPokemonWazaPp(pokemon, slot, newPp);
    return newPp - oldPp;
}

static inline void itemUsePPMaxUp(ItemUsePokemonLog* log, s16* count, void* pokemon, s32 slot) {
    u16 wazaDataId;
    u8 ppCount;
    u8 maxPp;
    u8 delta;

    wazaDataId = pokemonBiosGetPokemonWazaDataId(pokemon, slot);
    if ((u16)(wazaDataId - 0x164) > 1 && pokemonWazaCheckValid(pokemon, slot)) {
        ppCount = pokemonBiosGetPokemonWazaPpCount(pokemon, slot);
        if (ppCount < 3) {
            maxPp = pokemonWazaGetMaxPP(pokemon, slot);
            pokemonBiosSetPokemonWazaPpCount(pokemon, slot, ppCount + 1);
            delta = pokemonWazaGetMaxPP(pokemon, slot) - maxPp;
            pokemonBiosSetPokemonWazaPp(
                pokemon, slot, pokemonBiosGetPokemonWazaPp(pokemon, slot) + delta);
            itemUseAddLog(log, count, 0x12, delta, slot);
        }
    }
}

static inline u16 itemUseLevelUp(ItemUsePokemonLog* log, s16* count, void* pokemon) {
    u8 level;
    u16 oldMaxHp;
    u16 newMaxHp;

    if (pokemonIsDarkPokemon(pokemon)) {
        return 0;
    }
    level = pokemonBiosGetLevel(pokemon);
    if (level >= 100) {
        return 0;
    }
    oldMaxHp = pokemonBiosGetMaxHp(pokemon);
    pokemonBiosSetExp(pokemon,
        pokemonGrowDataBiosGetExp(
            pokemonGrowDataBiosGetPtr(pokemonDataBiosGetGrowDataId(
                pokemonDataBiosGetPtr(pokemonBiosGetPokemonDataId(pokemon)))),
            level + 1));
    pokemonResetBasisStatus(pokemon);
    newMaxHp = pokemonBiosGetMaxHp(pokemon);
    itemUseAddLog(log, count, 8, 0, 0);
    return newMaxHp - oldMaxHp;
}

static inline s16 itemUseFriendUp(
    ItemUsePokemonLog* log, s16* count, void* pokemon, void* fightPokemon,
    s8 delta, u16 minFriend, u16 maxFriend)
{
    s16 gained;

    if (*count <= 0) {
        return 0;
    }
    gained = friendXUp__FP7PokemonP12FightPokemonScUsUs(
        pokemon, fightPokemon, delta, minFriend, maxFriend);
    if (gained == 0) {
        return 0;
    }
    itemUseAddLog(log, count, 0x1E, gained, 0);
    return gained;
}

s16 fn_80144574(
    ItemUsePokemonLog* log, void* pokemon, void* fightPokemon,
    u16 itemDataId, u8 moveSlot)
{
    u16 evolutionSpecies;
    s16 logCount;
    u8 evolutionMode;
    u8 convertedParam[0x1C];
    u8* itemParam;
    void* fightOut;
    void* side;
    void* target;
    u16 hpAmount;
    u16 wazaDataId;
    u16 evolution;
    u8 boost;
    u8 delta;
    u8 ppCount;
    u8 maxPp;
    u8 slot;
    u8 i;
    u8 sleepCleared;
    u8 poisonCleared;
    u8 badPoisonCleared;
    s8 friendBoost;
    s16 friendDelta;
    u8 ok;

    hpAmount = 0;
    logCount = 0;

    if (itemDataId != 0xAF) {
        itemParam = itemUseGetParam(itemDataId);
    } else {
        itemParamConvertOrigFormat(convertedParam, lbl_80478BE8);
        itemParam = convertedParam;
    }
    if (itemParam == NULL) {
        return 0;
    }

    if (fightPokemon != NULL) {
        pokemon = itemUseGetPokemon(fightPokemon);
    }

    if (itemParamGetMeromeroFlag(itemParam)) {
        fightOut = itemUseGetFightOut(fightPokemon);
        if (fightOut != NULL && fn_802026E4(fightOut, 0xA)) {
            fightOutPokemonWriteJoutaiDataId(fightOut, 0xA);
            itemUseNotify(fightPokemon, 0x7C);
            itemUseAddLog(log, &logCount, 0, 0, 0);
        }
    }

    if (itemParamGetCriticalFlag(itemParam)) {
        fightOut = itemUseGetFightOut(fightPokemon);
        if (fightOut != NULL && fn_802025B8(fightOut, 0xF) == 2) {
            fn_8020248C(fightOut, 0xF, 0);
            itemUseNotify(fightPokemon, 0x7C);
            itemUseNotify(fightPokemon, 0x99);
            itemUseAddLog(log, &logCount, 1, 0, 0);
        }
    }

    if ((boost = itemParamGetAttackUp(itemParam)) != 0) {
        if ((delta = itemUseStatUp(fightPokemon, 0xE6, boost)) != 0) {
            itemUseAddLog(log, &logCount, 2, delta, 0);
        }
    }
    if ((boost = itemParamGetDefenceUp(itemParam)) != 0) {
        if ((delta = itemUseStatUp(fightPokemon, 0xE7, boost)) != 0) {
            itemUseAddLog(log, &logCount, 3, delta, 0);
        }
    }
    if ((boost = itemParamGetQuickUp(itemParam)) != 0) {
        if ((delta = itemUseStatUp(fightPokemon, 0xEA, boost)) != 0) {
            itemUseAddLog(log, &logCount, 4, delta, 0);
        }
    }
    if ((boost = itemParamGetHitUp(itemParam)) != 0) {
        if ((delta = itemUseStatUp(fightPokemon, 0xEB, boost)) != 0) {
            itemUseAddLog(log, &logCount, 5, delta, 0);
        }
    }
    if ((boost = itemParamGetSpAttackUp(itemParam)) != 0) {
        if ((delta = itemUseStatUp(fightPokemon, 0xE8, boost)) != 0) {
            itemUseAddLog(log, &logCount, 6, delta, 0);
        }
    }

    if (itemParamGetGuardFlag(itemParam)) {
        fightOut = itemUseGetFightOut(fightPokemon);
        if (fightOut != NULL) {
            side = fightTargetGetPtrAsNowFightType(2, fightOut);
            if (fightSideCheckWriteJoutaiDataId(side, 0x4C) == 2) {
                fightSideWriteJoutaiDataId(side, 0x4C, 0);
                itemUseNotify(fightPokemon, 0x99);
                itemUseAddLog(log, &logCount, 7, 5, 0);
            }
        }
    }

    if (itemParamGetLevelUpFlag(itemParam)) {
        hpAmount = itemUseLevelUp(log, &logCount, pokemon);
    }

    if (itemParamGetSleepFlag(itemParam)) {
        sleepCleared = 0;
        if (fn_80121ADC(pokemon, 8)) {
            fn_80121B4C(pokemon, 8);
            fightOut = itemUseGetFightOut(fightPokemon);
            if (fightOut != NULL) {
                fightOutPokemonWriteJoutaiDataId(fightOut, 0x17);
                sleepCleared = 1;
            }
            itemUseNotify(fightPokemon, 0x7C);
            itemUseAddLog(log, &logCount, 9, 0, 0);
            if (sleepCleared) {
                itemUseAddLog(log, &logCount, 0xA, 0, 0);
            }
        }
    }

    if (itemParamGetPoisonFlag(itemParam)) {
        badPoisonCleared = poisonCleared = 0;
        if (fn_80121ADC(pokemon, 3)) {
            fn_80121B4C(pokemon, 3);
            poisonCleared = 1;
        }
        if (fn_80121ADC(pokemon, 4)) {
            fn_80121B4C(pokemon, 4);
            badPoisonCleared = 1;
        }
        itemUseNotify(fightPokemon, 0x7C);
        if (poisonCleared) {
            itemUseAddLog(log, &logCount, 0xB, 0, 0);
        }
        if (badPoisonCleared) {
            itemUseAddLog(log, &logCount, 0xC, 0, 0);
        }
    }

    if (itemParamGetBurnFlag(itemParam) && fn_80121ADC(pokemon, 6)) {
        fn_80121B4C(pokemon, 6);
        itemUseNotify(fightPokemon, 0x7C);
        itemUseAddLog(log, &logCount, 0xE, 0, 0);
    }
    if (itemParamGetFreezeFlag(itemParam) && fn_80121ADC(pokemon, 7)) {
        fn_80121B4C(pokemon, 7);
        itemUseNotify(fightPokemon, 0x7C);
        itemUseAddLog(log, &logCount, 0xF, 0, 0);
    }
    if (itemParamGetParalyzeFlag(itemParam) && fn_80121ADC(pokemon, 5)) {
        fn_80121B4C(pokemon, 5);
        itemUseNotify(fightPokemon, 0x7C);
        itemUseAddLog(log, &logCount, 0x10, 0, 0);
    }

    if (itemParamGetConfuseFlag(itemParam)) {
        fightOut = itemUseGetFightOut(fightPokemon);
        if (fightOut != NULL && fn_802026E4(fightOut, 9)) {
            fightOutPokemonWriteJoutaiDataId(fightOut, 9);
            itemUseNotify(fightPokemon, 0x7C);
            itemUseAddLog(log, &logCount, 0x11, 0, 0);
        }
    }

    if (itemParamGetPPMaxUpFlag(itemParam)) {
        itemUsePPMaxUp(log, &logCount, pokemon, moveSlot);
    }

    if ((boost = itemParamGetHPEffortUp(itemParam)) != 0) {
        if (pokemonBiosGetPokemonDataId(pokemon) == 0x12F) {
            ok = TRUE;
        } else {
            ok = itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x8D, boost, 0x13);
        }
        if (!ok) {
            return 0;
        }
    }
    if ((boost = itemParamGetAttackEffortUp(itemParam)) != 0) {
        if (!itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x8E, boost, 0x14)) {
            return 0;
        }
    }

    if ((boost = itemParamGetHPUp(itemParam)) != 0) {
        hpRecover__FP20ITEMUSE2POKEMON_LOG1PsP7PokemonUcbUsP12FightPokemon(
            log, &logCount, pokemon, boost, itemParamGetReviveFlag(itemParam),
            hpAmount, fightPokemon);
    }

    if ((boost = itemParamGetPPUp(itemParam)) != 0) {
        slot = 0xFF;
        if (itemParamGetPPSelectFlag(itemParam)) {
            slot = moveSlot;
        }
        target = pokemon;
        if (fn_80121ADC(pokemon, 0x10) || fn_80121ADC(pokemon, 0x31)) {
            target = fightPokemonBiosGetMotoPokemonPtr(fightPokemon);
        }
        if (slot != 0xFF) {
            if ((delta = itemUsePPUp(target, slot, boost)) != 0) {
                itemUseAddLog(log, &logCount, 0x17, delta, slot);
            }
        } else {
            for (i = 0; i < 4; i++) {
                if ((delta = itemUsePPUp(target, i, boost)) != 0) {
                    itemUseAddLog(log, &logCount, 0x17, delta, i);
                }
            }
        }
    }

    if (itemParamGetEvolutionFlag(itemParam) && !pokemonIsDarkPokemon(pokemon)) {
        evolution = pokemonEvolutionCheck(
            pokemon, 1, itemDataId, &evolutionSpecies, &evolutionMode);
        if (evolution != 0 || evolutionSpecies != 0) {
            itemUseAddLog(log, &logCount, 0x18, evolution, evolutionSpecies);
        }
    }

    if ((boost = itemParamGetDefenceEffortUp(itemParam)) != 0) {
        if (!itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x8F, boost, 0x19)) {
            return 0;
        }
    }
    if ((boost = itemParamGetQuickEffortUp(itemParam)) != 0) {
        if (!itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x92, boost, 0x1A)) {
            return 0;
        }
    }
    if ((boost = itemParamGetSpDefenceEffortUp(itemParam)) != 0) {
        if (!itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x91, boost, 0x1B)) {
            return 0;
        }
    }
    if ((boost = itemParamGetSpAttackEffortUp(itemParam)) != 0) {
        if (!itemUseEffortUpLog(log, &logCount, pokemon, fightPokemon, 0x90, boost, 0x1C)) {
            return 0;
        }
    }

    if (itemParamGetPPMaxFullFlag(itemParam)) {
        slot = moveSlot;
        wazaDataId = pokemonBiosGetPokemonWazaDataId(pokemon, slot);
        if ((u16)(wazaDataId - 0x164) > 1 &&
            pokemonBiosGetPokemonWazaPpCount(pokemon, slot) < 3) {
            maxPp = pokemonWazaGetMaxPP(pokemon, slot);
            pokemonBiosSetPokemonWazaPpCount(pokemon, slot, 3);
            delta = pokemonWazaGetMaxPP(pokemon, slot) - maxPp;
            if (delta != 0) {
                pokemonBiosSetPokemonWazaPp(
                    pokemon, slot, pokemonBiosGetPokemonWazaPp(pokemon, slot) + delta);
            }
            itemUseAddLog(log, &logCount, 0x1D, delta, moveSlot);
        }
    }

    friendDelta = 0;
    if ((friendBoost = itemParamGetFriend1Up(itemParam)) != 0) {
        friendDelta = itemUseFriendUp(log, &logCount, pokemon, fightPokemon, friendBoost, 0, 100);
    }
    if (friendDelta == 0) {
        if ((friendBoost = itemParamGetFriend2Up(itemParam)) != 0) {
            friendDelta = itemUseFriendUp(log, &logCount, pokemon, fightPokemon, friendBoost, 100, 200);
        }
    }
    if (friendDelta == 0) {
        if ((friendBoost = itemParamGetFriend3Up(itemParam)) != 0) {
            itemUseFriendUp(log, &logCount, pokemon, fightPokemon, friendBoost, 200, 0xFFFF);
        }
    }

    return logCount;
}
