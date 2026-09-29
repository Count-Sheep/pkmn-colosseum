/**
 * @file field_range_801DF790.c
 * @brief field/hero, 0x801DF790 - 0x801E09E0.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 *
 * fn_801DF790 and fn_801DFC30 below previously lived, misattributed, in
 * game/battle/battle_waza.c (whose splits.txt range ends at 0x801DE698);
 * relocated here so this unit's real C source is scored where it belongs.
 * The remaining 2 functions in this TU's declared range are still asm-only.
 */
#include "dolphin/types.h"

extern void winMsgOpenFieldWithSE(s32 messageID, s32 windowID, s32 arg2, s32 arg3);
extern s32 fn_8001E184(void);
extern s32 menuPokemonOpen(s32 mode, s32 arg1, s32 arg2);
extern void* savedataGetStatus(s32 side, s32 slotType);
extern void* heroBiosGetPokemonPtr(void* status, u16 slot);
extern u8 heroIsMinePokemon(void* status, void* pokemon);
extern u8 pokemonCheckValid(void* pokemon);
extern u16 pokemonBiosGetDarkpokemonDataId(void* pokemon);
extern u8 fn_801EEC74(u16 id);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern u8 pokemonWazaCheckValid(void* pokemon, u16 slot);
extern s32 fn_80097BBC(u8 chan);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 slot);
extern void fn_80166AB8(u32 sndId, u32 fadeTime, u32 volume);
extern void pokemonWazaInit(void* pokemon, u32 slot);
extern u32* pokemonBiosGetPokemonWazaPtr(void* pokemon, u16 slot, u8 mode);
extern void pokemonWazaBiosCopy(u32* dst, u32* src);
extern void msgctrlSetValue(s32 id, void* value);
extern s32 menuNameEntryOpen(s32 mode, s32 slot);
extern void winMsgClose(s32 windowID);

static inline u8 fieldWazaCanForget(u16 selection)
{
    void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), selection);
    u16 darkId;

    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    darkId = pokemonBiosGetDarkpokemonDataId(pokemon);
    if (darkId != 0) {
        if (fn_801EEC74(darkId) != 0) {
            return 1;
        }
        return 0;
    }
    return 1;
}

static inline s32 fieldWazaCountValid(u16 selection)
{
    void* pokemon;
    s32 i;
    s32 count;

    count = 0;
    pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), selection);

    if (pokemonCheckValid(pokemon) == 0) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        if (pokemonWazaCheckValid(pokemon, (u16)i) != 0) {
            count++;
        }
    }
    return count;
}

/**
 * fn_801DF790 - Waza item effect handler.
 * Address: 0x801DF790 | Size: 0x4A0
 */
void fn_801DF790(s32 slot, s32 itemID) {
    s32 selection;
    s32 moveSlot;
    u32 wazaId;
    s32 running;
    s32 state;

    running = 1;
    moveSlot = 0;
    wazaId = 0;
    state = 0;

    (void)slot;
    (void)itemID;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B28, 1, 0, 1);
            state = 1;
            break;
        case 1:
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 2:
            winMsgOpenFieldWithSE(0x3B29, 1, 0, 1);
            state = 3;
            break;
        case 3:
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 4;
            } else {
                state = 12;
            }
            break;
        case 4:
            if (fieldWazaCanForget((u16)selection) != 0) {
                state = 5;
            } else {
                state = 13;
            }
            break;
        case 5:
            if (fieldWazaCountValid((u16)selection) == 1) {
                state = 14;
            } else {
                state = 6;
            }
            break;
        case 6:
            winMsgOpenFieldWithSE(0x3B2A, 1, 0, 1);
            state = 7;
            break;
        case 7:
            if (fieldWazaCountValid((u16)selection) == 1) {
                state = 14;
            } else {
                moveSlot = fn_80097BBC((u8)selection);
                if (moveSlot >= 0) {
                    state = 8;
                } else {
                    state = 2;
                }
            }
            break;
        case 8:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            wazaId = pokemonBiosGetPokemonWazaDataId(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection), (u16)moveSlot);
            msgctrlSetValue(0x39, (void*)(u32)wazaId);
            winMsgOpenFieldWithSE(0x3B2B, 1, 0, 1);
            state = 9;
            break;
        case 9:
            if ((s8)fn_8001E184() == 0) {
                state = 10;
            } else {
                state = 6;
            }
            break;
        case 10:
            fn_80166AB8(0x48, 0, 0);
            state = 11;
            break;
        case 11: {
            u16 slotIndex;
            void* pokemon;
            u32* dstWaza;
            s32 nextSlot;

            pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            pokemonWazaInit(pokemon, (u16)moveSlot);
            for (slotIndex = (u16)moveSlot; slotIndex < 3; slotIndex++) {
                nextSlot = slotIndex + 1;
                if (pokemonWazaCheckValid(pokemon, nextSlot) == 0) {
                    break;
                }
                if (pokemon != NULL) {
                    dstWaza = pokemonBiosGetPokemonWazaPtr(pokemon, slotIndex, 0);
                    pokemonWazaBiosCopy(dstWaza, pokemonBiosGetPokemonWazaPtr(pokemon, nextSlot, 0));
                }
            }
            pokemonWazaInit(pokemon, slotIndex);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            msgctrlSetValue(0x39, (void*)(u32)wazaId);
            winMsgOpenFieldWithSE(0x3B2C, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 7;
            } else {
                state = 12;
            }
            break;
        }
        case 12:
            winMsgOpenFieldWithSE(0x3B2D, 1, 0, 1);
            state = 15;
            break;
        case 14:
            winMsgOpenFieldWithSE(0x3B2E, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 13:
            msgctrlSetValue(
                0x32,
                pokemonBiosGetNicknamePtr(
                    heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x44AA, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 12;
            }
            break;
        case 15:
            running = 0;
            break;
        }
    } while (running != 0);
}

typedef struct FieldPokemonData {
    u8 data[0x138];
} FieldPokemonData;

static inline u16 fieldCountValidPokemon(void)
{
    u16 i;
    u16 count;
    void* party;

    count = 0;
    party = savedataGetStatus(0, 2);
    for (i = 0; i < 6; i++) {
        if (pokemonCheckValid(heroBiosGetPokemonPtr(party, i)) != 0) {
            count++;
        }
    }
    return count;
}

extern void* sodateyaGetPokemonPtr(s32);
extern u32 pokemonBiosGetDp(void*);
extern u32 fn_801ED24C(s32);

static inline s32 sodateyaGetPokemonDPValue(void)
{
    void* pokemon = sodateyaGetPokemonPtr(0);

    return (s32)(fn_801ED24C(0) - pokemonBiosGetDp(sodateyaGetPokemonPtr(0))) / 100;
}

static inline s32 sodateyaCalcPrice(s32 levels)
{
    s32 price;
    s32 value;

    if (fn_801ED24C(0) != 0) {
        value = sodateyaGetPokemonDPValue();
        if (value != 0) {
            price = value * 100 + 100;
        } else {
            price = 0;
        }
    } else {
        price = 0;
    }
    price += levels * 100;
    return price + 100;
}

/**
 * fn_801DFC30 - Waza/scene master controller.
 * Address: 0x801DFC30 | Size: 0x7A4
 * Very large function (~2KB) that serves as the master controller
 * coordinating all waza visual effects, scene state, and transitions.
 * This is likely the top-level function called from the battle state machine
 * to drive a complete move execution's visual presentation.
 */
void fn_801DFC30(void) {
    extern u8 fn_801ED218(s32);
    extern u8 pokemonGetStatus(void*, u16, s32, s32);
    extern u8 fn_801ED0CC(s32, void*);
    extern s32 fn_801E075C(s32);
    extern s32 fn_801ED294(s32);
    extern u8 pokemonBiosGetLevel(void*);
    extern u32 heroGetStatus(s32, s32, s32);
    extern void heroDecPokedoru(void*, u32);
    extern void winMsgOpenField(u32, s32, s32);
    extern void fn_80183350(u32, u32);
    extern void fn_8018C69C(u32, u32, u32);
    extern void fn_8018B76C(u32, u32, u32, u32, u32);
    extern void fn_80183018(u32, u32);
    extern void fn_801ECFE0(s32, void*);
    extern void fn_801E09E0(s32);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void _threadSwitch(void);

    s32 i;
    s32 selection;
    s32 levels;
    s32 running;
    s32 state;
    s32 validCount;
    s32 price;
    f32 timer;

    running = 1;
    state = 0;

    do {
        switch (state) {
        case 0:
            if ((u8)fn_801ED218(0) != 0) {
                state = 9;
            } else {
                state = 1;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x3B11, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 8;
            }
            break;
        case 2:
            validCount = 0;
            for (i = 0; i < 6; i++) {
                void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);
                if ((u8)pokemonCheckValid(pokemon) != 0 &&
                    pokemonGetStatus(pokemon, 0, 0x7B, 0) == 0) {
                    validCount++;
                }
            }
            if (validCount == 1) {
                state = 3;
            } else {
                state = 5;
            }
            break;
        case 3:
            winMsgOpenFieldWithSE(0x3B13, 1, 0, 2);
            state = 21;
            break;
        case 4:
            winMsgOpenFieldWithSE(0x3B15, 1, 0, 2);
            state = 21;
            break;
        case 5:
            winMsgOpenFieldWithSE(0x3B17, 1, 0, 2);
            state = 6;
            break;
        case 6:
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 7;
            } else {
                state = 8;
            }
            break;
        case 7: {
            s32 lastValid;
            void* dst;
            void* party;
            void* pokemon;

            fn_801E075C(selection);
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B19, 1, 0, 2);
            state = 21;

            party = savedataGetStatus(0, 2);
            pokemon = heroBiosGetPokemonPtr(party, (u16)selection);
            lastValid = fieldCountValidPokemon();
            if ((u8)fn_801ED0CC(0, pokemon) != 0 && selection < 6) {
                for (i = selection; i < lastValid - 1; i++) {
                    dst = heroBiosGetPokemonPtr(party, (u16)i);
                    pokemon = heroBiosGetPokemonPtr(party, (u16)(i + 1));
                    if ((u8)pokemonCheckValid(pokemon) == 0) {
                        break;
                    }
                    *(FieldPokemonData*)dst = *(FieldPokemonData*)pokemon;
                }
                pokemonInit(pokemon);
            }
            break;
        }
        case 8:
            winMsgOpenFieldWithSE(0x3B1B, 1, 0, 2);
            state = 21;
            break;
        case 9:
            winMsgOpenFieldWithSE(0x3B1C, 1, 0, 2);
            state = 10;
            break;
        case 10:
            {
                s32 grown = fn_801ED294(0);

                levels = (u8)pokemonBiosGetLevel(sodateyaGetPokemonPtr(0)) - (u8)grown;
            }
            if (levels != 0) {
                state = 11;
            } else {
                state = 12;
            }
            break;
        case 11: {
            s32 grown = fn_801ED294(0);
            s32 level = pokemonBiosGetLevel(sodateyaGetPokemonPtr(0));
            msgctrlSetValue(0x2F, (void*)(u32)((u8)level - (u8)grown));
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenFieldWithSE(0x3B1D, 1, 0, 2);
            state = 12;
            break;
        }
        case 12:
            winMsgOpenFieldWithSE(0x3B1E, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 13;
            } else {
                state = 4;
            }
            break;
        case 13:
            validCount = 0;
            for (i = 0; i < 6; i++) {
                if ((u8)pokemonCheckValid(heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i)) != 0) {
                    validCount++;
                }
            }
            if (validCount == 6) {
                state = 14;
            } else {
                state = 15;
            }
            break;
        case 14:
            winMsgOpenFieldWithSE(0x3B12, 1, 0, 2);
            state = 4;
            break;
        case 15:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            msgctrlSetValue(0x4B, (void*)sodateyaCalcPrice(levels));
            winMsgOpenFieldWithSE(0x3B14, 1, 0, 2);
            if ((s8)fn_8001E184() == 0) {
                state = 16;
            } else {
                state = 4;
            }
            break;
        case 16:
            price = sodateyaCalcPrice(levels);
            if ((s32)heroGetStatus(0, 0xC, 0) >= price) {
                heroDecPokedoru(savedataGetStatus(0, 2), price);
                fn_80166AB8(0x3CB, 0, 0);
                timer = 0.0f;
                while (timer < 0.7f) {
                    timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
                    _threadSwitch();
                }
                state = 17;
            } else {
                state = 18;
            }
            break;
        case 17:
            winMsgClose(1);
            fn_801E09E0(selection);
            state = 19;
            break;
        case 18:
            winMsgOpenFieldWithSE(0x3B16, 1, 0, 2);
            state = 4;
            break;
        case 19:
            winMsgOpenFieldWithSE(0x3B18, 1, 0, 2);
            state = 20;
            break;
        case 20:
            msgctrlSetValue(0x32, pokemonBiosGetNicknamePtr(sodateyaGetPokemonPtr(0)));
            winMsgOpenField(0x3B1A, 1, 0);
            state = 8;
            for (i = 0; i < 6; i++) {
                void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i);

                if ((u8)pokemonCheckValid(pokemon) == 0) {
                    /* RULE-EXCEPTION(title-path): goto used only to give retail's
                     * loop exit (i = -1 on the fall-through edge only) - see
                     * docs/RULE_EXCEPTIONS.md (pending: function not yet exact). */
                    goto found;
                }
                pokemonBiosGetDp(pokemon);
            }
            i = -1;
        found:
            fn_801ECFE0(0, heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)i));
            fn_80183350(0x4D, 1);
            fn_8018C69C(0x4D, 1, 8);
            fn_8018B76C(0x4D, 1, 5, 0, 1);
            fn_80183018(0x4D, 1);
            break;
        case 21:
            winMsgClose(1);
            running = 0;
            break;
        }
    } while (running != 0);
}

/**
 * fn_801E03D4 - Party Pokemon nickname flow.
 * Address: 0x801E03D4 | Size: 0x388
 */
void fn_801E03D4(void) {
    s32 running = 1;
    s32 selection;
    s32 state = 0;

    do {
        switch (state) {
        case 0:
            winMsgOpenFieldWithSE(0x3B21, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 2;
            } else {
                state = 1;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x3B22, 1, 0, 1);
            state = 12;
            break;
        case 2:
            winMsgOpenFieldWithSE(0x3B23, 1, 0, 1);
            selection = menuPokemonOpen(6, 0, 0);
            if (selection >= 0) {
                state = 3;
            } else {
                state = 1;
            }
            break;
        case 3: {
            void* status = savedataGetStatus(0, 2);

            if (heroIsMinePokemon(status, heroBiosGetPokemonPtr(status, (u16)selection)) != 0) {
                state = 4;
            } else {
                state = 5;
            }
            break;
        }
        case 4: {
            void* pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection);
            u8 canRename;

            if (pokemonCheckValid(pokemon) == 0) {
                canRename = 0;
            } else {
                u16 darkID = pokemonBiosGetDarkpokemonDataId(pokemon);

                if (darkID != 0) {
                    if (fn_801EEC74(darkID) != 0) {
                        canRename = 1;
                    } else {
                        canRename = 0;
                    }
                } else {
                    canRename = 1;
                }
            }

            if (canRename != 0) {
                state = 6;
            } else {
                state = 11;
            }
            break;
        }
        case 5:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B24, 1, 0, 1);
            state = 12;
            break;
        case 6:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B25, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 7;
            } else {
                state = 1;
            }
            break;
        case 7:
            winMsgOpenFieldWithSE(0x3B26, 1, 0, 1);
            if (menuNameEntryOpen(2, selection) == 0) {
                state = 9;
            } else {
                state = 8;
            }
            break;
        case 8:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B27, 1, 0, 1);
            state = 12;
            break;
        case 9:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B1F, 1, 0, 1);
            if ((s8)fn_8001E184() == 0) {
                state = 10;
            } else {
                state = 7;
            }
            break;
        case 10:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B47, 1, 0, 1);
            state = 12;
            break;
        case 11:
            msgctrlSetValue(
                0x32, pokemonBiosGetNicknamePtr(
                          heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)selection)));
            winMsgOpenFieldWithSE(0x3B20, 1, 0, 1);
            state = 12;
            break;
        case 12:
            winMsgClose(1);
            running = 0;
            break;
        }
    } while (running != 0);
}

typedef struct EtcToolVec {
    f32 x;
    f32 y;
    f32 z;
} EtcToolVec;

typedef struct EtcToolSequenceData {
    u32 objectIds[13];
    EtcToolVec objectPosition;
    EtcToolVec objectScale;
    EtcToolVec partyPosition;
    EtcToolVec partyScale;
} EtcToolSequenceData;

extern const EtcToolSequenceData lbl_80279A00;
extern const EtcToolVec lbl_803750C8[3];

/**
 * fn_801E075C - Show the selected party Pokemon's ball model.
 * Address: 0x801E075C | Size: 0x284
 */
void fn_801E075C(s32 partyIndex)
{
    extern u8 pokemonBiosGetCatchBallId(void* pokemon);
    extern u16 pokemonBiosGetPokemonDataId(void* pokemon);
    extern void* pokemonDataBiosGetPtr(u16 id);
    extern u16 pokemonDataBiosGetVoice(void* data);
    extern void* floorOpenObject(u32 resource);
    extern void GSvecCopy(void* dst, const void* src);
    extern void GSmodelSetPosition(void* model, void* position);
    extern void GSmodelSetScale(void* model, void* scale);
    extern void GSmodelSetVisibility(void* model, s32 visible);
    extern void GSmodelFree(void* model);
    extern s32 fn_800D37CC(void);
    extern u32 fn_800D3088(void);
    extern void _threadSwitch(void);
    EtcToolVec position;
    EtcToolVec scale;
    s32 running;
    void* model;
    s32 state;
    const EtcToolSequenceData* data = &lbl_80279A00;

    position = data->partyPosition;
    scale = data->partyScale;
    model = NULL;
    state = 0;
    running = 1;

    do {
        switch (state) {
        case 0: {
            u8 ball;
            void* pokemon;
            u16 species;
            u32 objectIds[13];

            ball = pokemonBiosGetCatchBallId(
                heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)partyIndex));
            objectIds[0] = data->objectIds[0];
            objectIds[1] = data->objectIds[1];
            objectIds[2] = data->objectIds[2];
            objectIds[3] = data->objectIds[3];
            objectIds[4] = data->objectIds[4];
            objectIds[5] = data->objectIds[5];
            objectIds[6] = data->objectIds[6];
            objectIds[7] = data->objectIds[7];
            objectIds[8] = data->objectIds[8];
            objectIds[9] = data->objectIds[9];
            objectIds[10] = data->objectIds[10];
            objectIds[11] = data->objectIds[11];
            objectIds[12] = data->objectIds[12];
            model = floorOpenObject(objectIds[ball]);
            GSvecCopy(&position, &lbl_803750C8[0]);
            pokemon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)partyIndex);
            species = pokemonBiosGetPokemonDataId(pokemon);
            if (pokemonCheckValid(pokemon) != 0) {
                void* pokemonData = pokemonDataBiosGetPtr(species);

                if (pokemonData != NULL) {
                    fn_80166AB8(pokemonDataBiosGetVoice(pokemonData), 0, 0);
                }
            }
            GSmodelSetPosition(model, &position);
            GSmodelSetScale(model, &scale);
            state = 1;
            break;
        }
        case 1: {
            f32 timer = 0.0f;
            f32 endTime = 1.5f;

            while (timer < endTime) {
                timer += (f32)fn_800D3088() / (f32)fn_800D37CC();
                _threadSwitch();
            }
            state = 100;
            break;
        }
        case 100:
            running = 0;
            GSmodelSetVisibility(model, 0);
            GSmodelFree(model);
            model = NULL;
            break;
        }
    } while (running != 0);
}
