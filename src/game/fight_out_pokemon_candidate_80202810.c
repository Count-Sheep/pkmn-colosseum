/**
 * @file fight_out_pokemon_candidate_80202810.c
 * @brief fightOutPokemon + fightPokemon candidate prefix, address range
 *        0x80202810-0x8020355C, 9 functions.
 *
 * OutPokemon/Pokemon field accessors, sequence/status writers, and
 * damage-calc support the seq/waza layers call into (statusGetStatus,
 * fadeEffectGetRandom callers, etc). Corresponds to XD's
 * fight.cpp fightOutPokemon+fightPokemon cluster (0x80200644-0x80208288).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct StatusIdTable7 {
    u16 id[7];
} StatusIdTable7;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

/* 0x80202810 | size: 0x188 | medium */
/* 0x80202810 | size: 0x188 */
void fightOutPokemonWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern void fn_8011B788();
    extern void fn_80121B4C();
    extern void fn_801DA36C();
    void* eeData;

    eeData = pokemonGetStatus(ctx, 0, 0xEE, 0);
    if ((u16)(u32)typeObj == 0) {
        if (eeData != NULL) {
            fn_801DA36C(eeData, 1);
            fn_801DA36C(eeData, 2);
        }
    } else {
        if (eeData != NULL) {
            if ((u16)(u32)typeObj == 8) {
                fn_801DA36C(eeData, 1);
            }
            if ((u16)(u32)typeObj == 7) {
                fn_801DA36C(eeData, 2);
            }
        }
    }
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8 || fn_80119ED0(typeObj) == 0xCD) {
        eeData = pokemonGetStatus(ctx, 0, 0xD6, 0);
        if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
            if (eeData == NULL) {
                eeData = NULL;
            } else {
                eeData = pokemonGetStatus(eeData, 0, 0xCC, 0);
            }
            fn_80121B4C(eeData, typeObj);
        } else if (fn_80119ED0(typeObj) == 0xCD) {
            fn_8011B788(eeData, typeObj);
        }
    } else if (fn_80119ED0(typeObj) == 0xD8) {
        fn_8011B788(ctx, typeObj);
    }
}

/* 0x80202998 | size: 0x94 */
void fightOutPokemonResetSeqStatus(void* ctx, u16 mode) {
    extern void fn_801DA36C();
    void* obj;
    u16 modeVal;
    obj = pokemonGetStatus(ctx, 0, 0xEE, 0);
    modeVal = mode;
    if (modeVal == 0) {
        if (obj != NULL) {
            fn_801DA36C(obj, 1);
            fn_801DA36C(obj, 2);
        }
    } else {
        if (obj != NULL) {
            if (modeVal == 8) {
                fn_801DA36C(obj, 1);
            }
            modeVal = mode;
            if (modeVal == 7) {
                fn_801DA36C(obj, 2);
            }
        }
    }
}

/* 0x80202A2C | size: 0xB0 */
void fightPokemonWriteJoutaiDataId(void* ctx, void* typeObj, u32 param) {
    extern u16 fn_80119ED0();
    extern void fn_8011AFCC();
    extern void fn_8012190C();
    void* resolved;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        fn_8012190C(resolved, typeObj, param);
    } else if (fn_80119ED0(typeObj) == 0xCD) {
        fn_8011AFCC(ctx, typeObj, param);
    }
}

/* 0x80202ADC | size: 0xAC */
u32 fightPokemonCheckWriteJoutaiDataId(void* ctx, void* typeObj) {
    extern u16 fn_80119ED0();
    extern u32 fn_8011B67C();
    extern u32 fn_80121ADC();
    void* resolved;
    u32 result;
    if (fn_80119ED0(typeObj) == 0x7C || fn_80119ED0(typeObj) == 0xC8) {
        if (ctx == NULL) {
            resolved = NULL;
        } else {
            resolved = pokemonGetStatus(ctx, 0, 0xCC, 0);
        }
        result = fn_80121ADC(resolved, typeObj);
    } else if (fn_80119ED0(typeObj) != 0xCD) {
        result = 0;
    } else {
        result = fn_8011B67C(ctx, typeObj);
    }
    return result;
}

/* 0x80202B88 | size: 0x94 */
u32 fightOutPokemonIsAlly(void* obj1, void* obj2) {
    extern u32 fightTargetGetPtr();
    extern u16 fightFloorGetStatus();
    u16 tableId;
    u32 val1;
    u32 val2;
    tableId = 0xFFFF & fightFloorGetStatus(NULL, 0, 0x14, 0);
    if (obj1 == NULL) {
        return 0;
    }
    if (obj2 == NULL) {
        return 0;
    }
    val1 = fightTargetGetPtr(2, obj1, tableId);
    val2 = fightTargetGetPtr(2, obj2, tableId);
    return (u8)(val1 == val2);
}

/* Address: 0x80202C1C | Size: 0x57c | Ghidra import */

extern u8 pokemonCheckFightOut();
extern u8 pokemonCheckValid();
extern u16 fn_801EF634();
extern u32 fightFloorGetStatus();
extern u32 fightSideGetStatus();
extern u8 fightTrainerCheckValid();
extern void* fightTrainerGetStatus();
extern void* fightOutPokemonEnemySearchAry();
extern u8 fightOutPokemonEnemyCheckValid();
extern void fightOutPokemonEnemyCreate();

static inline void* fn_80202C1C_getCC(void* p) {
    if (p == NULL) {
        return NULL;
    }
    return pokemonGetStatus(p, 0, 0xCC, 0);
}

static inline u8 fn_80202C1C_checkBody(void* p) {
    void* sub;

    if (p == NULL) {
        return 0;
    }
    if (fn_801EF634() == 1) {
        return 0;
    }
    sub = pokemonGetStatus(p, 0, 0xCB, 0);
    if (sub == NULL) {
        return 0;
    }
    if (pokemonCheckValid(sub) == 0) {
        return 0;
    }
    sub = fn_80202C1C_getCC(p);
    if (sub == NULL) {
        return 0;
    }
    if (pokemonCheckValid(sub) == 0) {
        return 0;
    }
    if ((s32)pokemonGetStatus(p, 0, 0xCE, 0) < 0) {
        return 0;
    }
    return 1;
}

static inline u8 fn_80202C1C_checkHolder(void* p) {
    void* body;

    if (p == NULL) {
        return 0;
    }
    if (fn_801EF634() == 1) {
        return 0;
    }
    body = pokemonGetStatus(p, 0, 0xD6, 0);
    if (body == NULL) {
        return 0;
    }
    if (!fn_80202C1C_checkBody(body)) {
        return 0;
    } else {
        return 1;
    }
}

static inline u8 fn_80202C1C_checkBodyOut(void* p) {
    void* sub;

    if (p == NULL) {
        return 0;
    }
    if (!fn_80202C1C_checkBody(p)) {
        return 0;
    }
    if ((s32)pokemonGetStatus(p, 0, 0xD2, 0) == 1) {
        return 0;
    }
    sub = fn_80202C1C_getCC(p);
    if (sub == NULL) {
        return 0;
    }
    if (pokemonCheckFightOut(sub) == 0) {
        return 0;
    }
    return 1;
}

static inline u8 fn_80202C1C_checkEntry(void* p) {
    if (p == NULL) {
        return 0;
    }
    if (!fn_80202C1C_checkHolder(p)) {
        return 0;
    }
    if ((s32)pokemonGetStatus(p, 0, 0x120, 0) == 1) {
        return 0;
    }
    if (!fn_80202C1C_checkBodyOut(pokemonGetStatus(p, 0, 0xD6, 0))) {
        return 0;
    } else {
        return 1;
    }
}

static inline void fn_80202C1C_addEnemy(void* ctx, void* entry) {
    void* arr;
    void* slot;
    u16 k;

    if (ctx == NULL || !fn_80202C1C_checkHolder(entry)) {
        return;
    }
    arr = pokemonGetStatus(ctx, 0, 0x122, 0);
    if (fightOutPokemonEnemySearchAry(arr, 4, entry) == NULL) {
        for (k = 0; k < 4; k++) {
            slot = (void*)((u32)arr + k * 0xC);
            if (fightOutPokemonEnemyCheckValid(slot) == 0) {
                fightOutPokemonEnemyCreate(slot, entry);
                break;
            }
        }
    }
}

void fn_80202C1C(u32 r3, u32 r4) {
    u16 sideCount;
    u16 memberCount;
    void* trainer;
    u32 i;
    void* entry;
    u32 j;

    fightFloorGetStatus(0, 0, 0x14, 0);
    sideCount = fightFloorGetStatus(0, 0, 0x16, 0);
    memberCount = fightFloorGetStatus(0, 0, 0x18, 0);
    for (i = 0; (i & 0xFFFF) < (sideCount & 0xFFFF); i++) {
        trainer = (void*)fightSideGetStatus(r4, 0, 7, i);
        if (fightTrainerCheckValid(trainer) != 0) {
            for (j = 0; (j & 0xFFFF) < (memberCount & 0xFFFF); j++) {
                entry = fightTrainerGetStatus(trainer, 0, 0x46, j);
                if (fn_80202C1C_checkEntry(entry)) {
                    fn_80202C1C_addEnemy((void*)r3, entry);
                }
            }
        }
    }
}

/* 0x80203198 | size: 0x14C | medium */
/* 0x80203198 | size: 0x14C */
extern u16 fightOutPokemonEnemyBiosGetOumuWazaDataId();
extern void* fightOutPokemonEnemySearchAry();
extern u8 fightOutPokemonEnemyCheckValid();
extern void fightOutPokemonEnemyInit();

static inline u8 fn_80203198_countOumu(void* ctx) {
    void* entryPtr;
    void* entry;
    u8 count;
    u8 i;

    if (ctx == NULL) {
        return 0;
    }
    entryPtr = pokemonGetStatus(ctx, 0, 0x122, 0);
    for (i = 0; i < 4; i++) {}
    count = 0;
    for (i = 0; i < 4; i++) {
        entry = (void*)((u32)entryPtr + i * 0xC);
        if (fightOutPokemonEnemyCheckValid(entry) != 0) {
            u16 oumu = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
            if (oumu != 0 && oumu != 0x165) {
                count++;
            }
        }
    }
    return count;
}

void fn_80203198(void* ctx, u32 param) {
    void* entry;
    u16 species;

    if (ctx == NULL) {
        return;
    }
    entry = fightOutPokemonEnemySearchAry(pokemonGetStatus(ctx, 0, 0x122, 0), 4, param);
    if (entry == NULL) {
        return;
    }
    species = fightOutPokemonEnemyBiosGetOumuWazaDataId(entry);
    fightOutPokemonEnemyInit(entry);
    if (species == 0) {
        return;
    }
    if (species == 0x165) {
        return;
    }
    if (species == 0xFFFF) {
        return;
    }
    if ((s32)(u32)pokemonGetStatus(ctx, 0, 0xF7, 0) != 0) {
        return;
    }
    if (fn_80203198_countOumu(ctx) == 0) {
        pokemonSetStatus(ctx, 0, 0xF7, 0, (u32)species);
    }
}

/* 0x802032E4 | size: 0x138 */
#pragma push
#pragma scheduling on
static inline void* fn_802032E4_getCC(void* ctx) {
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetFriendFormPokemonFriendFilterId(void* ctx, u32 param) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void pokemonGetFriendFormPokemonFriendFilterId();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* resolvedPtr;
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_802032E4_getCC(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_802032E4_getCC(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { resolvedPtr = pokemonGetStatus(ctx, 0, 0xCC, 0); tmp = resolvedPtr; }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetFriendFormPokemonFriendFilterId(ccData, value, param);
}
#pragma pop

/* 0x8020341C | size: 0x140 */
#pragma push
#pragma scheduling off
static inline void* fn_8020341C_resolveCcData(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

static inline void* fn_8020341C_resolveCcCtx(void* ctx)
{
    return pokemonGetStatus(ctx, 0, 0xCC, 0);
}

void fightPokemonGetEffortFromPokemon(void* ctx, u32 param1, u32 param2) {
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
    extern u8 fn_80121ADC();
    extern void pokemonGetEffortFromPokemon();
    extern u32 pokemonGetSoubiItemSoubiDataId();
    void* ccData;
    void* ccCtx;
    u8 result;
    u32 value;

    if (ctx == 0) { ccData = 0; } else { ccData = fn_8020341C_resolveCcData(ctx); }
    if (ccData == NULL) { return; }
    if (ctx == 0) { ccCtx = 0; } else { ccCtx = fn_8020341C_resolveCcCtx(ctx); }
    if (ccCtx == NULL) {
        value = 0;
    } else {
        if (fn_80119ED0(0x3D) == 0x7C || fn_80119ED0(0x3D) == 0xC8) {
            void* tmp;
            if (ctx == 0) { tmp = 0; } else { tmp = pokemonGetStatus(ctx, 0, 0xCC, 0); }
            result = fn_80121ADC(tmp, 0x3D);
        } else {
            if (fn_80119ED0(0x3D) != 0xCD) {
                result = 0;
            } else {
                result = fn_8011B67C(ctx, 0x3D);
            }
        }
        if (result == 1) {
            value = 0;
        } else {
            value = pokemonGetSoubiItemSoubiDataId(ccCtx);
        }
    }
    pokemonGetEffortFromPokemon(ccData, value, param1, param2);
}
#pragma pop
