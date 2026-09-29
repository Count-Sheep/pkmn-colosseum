/**
 * @file pcbox_exact_801347E8.c
 * @brief PC-box Pokemon slots, 0x801347E8 - 0x80134E10: the box empty/used
 *        slot counts, box name set/get, delPokemon and pcboxAddPokemon.
 *
 * Function-boundary carve of the PCBOX TU (see pcbox.c), built with
 * GC/1.3 -O4,p and no pragmas. The optimization_level/scheduling pragmas
 * pcbox.c carries for these functions only restate the defaults. Text only.
 * pcboxAddPokemon searches each box with the same record getter the other
 * functions expand, through a small "first empty record" inline.
 */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

/* One box contains its name/header followed by 30 Pokemon records. */
typedef struct PCBoxPokemonRecord {
    u8 data[0x138];
} PCBoxPokemonRecord;

typedef struct PCBoxData {
    u8 header[0x14];
    PCBoxPokemonRecord pokemon[30];
} PCBoxData;

static inline u8* pcboxGetPokemonRecord(PCBoxData* base, s8 box, s8 index)
{
    if (box < 0 || box >= 3) {
        return 0;
    }
    if (index < 0 || index >= 30) {
        return 0;
    }
    return base[box].pokemon[index].data;
}

/* 0x801347E8 | 0x104 */
s8 pcboxGetPokemonBoxNbEmptySlot(PCBoxData* base, s8 box) {
    s8 index;
    s8 count = 0;
    u8* pokemon;
    u8* record;

    if (base == 0) {
        base = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < 0 || box >= 3) {
        count = -1;
    } else {
        index = 0;
        pokemon = (u8*)base + (s32)box * sizeof(PCBoxData);
        while (index < 30) {
            if (box < 0 || box >= 3) {
                record = 0;
            } else if (index < 0 || index >= 30) {
                record = 0;
            } else {
                record = pokemon + 0x14;
            }
            if (record != 0 && pokemonCheckValid(record)) {
                count++;
            }
            pokemon += sizeof(PCBoxPokemonRecord);
            index++;
        }
    }

    if (count < 0) {
        return -1;
    }
    return (s8)(30 - count);
}


/* 0x801348EC | 0xF0 */
s32 getPokemonBoxNbUsedSlot__5PCBOXFSc(PCBoxData* base, s8 box) {
    u8* pokemon;
    s32 count = 0;
    s8 index;

    if (base == 0) {
        base = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < 0 || box >= 3) {
        return -1;
    }

    for (index = 0; index < 30; index++) {
        pokemon = pcboxGetPokemonRecord(base, box, index);
        if (pokemon != 0 && pokemonCheckValid(pokemon)) {
            count++;
        }
    }
    return count;
}


/* 0x801349DC | 0xBC */
s32 pcboxSetPokemonBoxName(void* base, s8 slot, u16* name) {
    extern void GScharCpy(void*, u16*);
    s32 len;
    u16* p;
    s8 s;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    s = slot;
    if (s < 0 || s >= 3) return 0;
    if (name == 0) return 0;
    p = name;
    len = 0;
    while (*p != 0) {
        p++;
        len++;
    }
    if (len > 8) return 0;
    GScharCpy((u8*)base + (s32)s * 0x24a4, name);
    return 1;
}

void* pcboxGetPokemonBoxName(void* base, s8 index) {
    s8 i;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    i = index;
    if (i < 0) goto _ret0;
    i = index;
    if (i < 3) goto _compute;
_ret0:
    return 0;
_compute:
    return (u8*)base + (s32)i * 0x24a4;
}


/* 0x80134AF8 | 0xC8 */
s32 delPokemon__5PCBOXFScSc(void* base, s8 slot, s8 idx) {
    extern u8 pokemonCheckValid(void*);
    extern void pokemonInit(void*);
    u8* entry;
    s8 s;
    s8 e;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    s = slot;
    if (s < 0 || s >= 3) {
        entry = 0;
    } else {
        e = idx;
        if (e < 0 || e >= 0x1e) {
            entry = 0;
        } else {
            entry = (u8*)base + (s32)s * 0x24a4 + (s32)e * 0x138 + 0x14;
        }
    }
    if (entry == 0) return 0;
    if (!pokemonCheckValid(entry)) return 0;
    pokemonInit(entry);
    return 1;
}


/* 0x80134BC0 | 0x250 */
static inline s8 pcboxSearchEmptyPokemon(PCBoxData* base, s8 box)
{
    s32 index;

    for (index = 0; index < 30; index++) {
        u8* record = pcboxGetPokemonRecord(base, box, index);
        if (record != NULL && !pokemonCheckValid(record)) {
            break;
        }
    }
    if (index < 30) {
        return index;
    }
    return -1;
}

s32 pcboxAddPokemon(PCBoxData* base, void* src, s8 box)
{
    extern void pokemonAllKaihuku(void*);
    s8 index;
    u8* record;
    PCBoxData* pc;

    pc = base; /* RULE-EXCEPTION(title-path): pure copy of the parameter, only for retail's register choice - see docs/RULE_EXCEPTIONS.md */
    if (pc == NULL) {
        pc = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < -1 || box >= 3) {
        return 0;
    }
    if (src == NULL) {
        return 0;
    }
    if (box == -1) {
        for (box = 0; box < 3; box++) {
            index = pcboxSearchEmptyPokemon(pc, box);
            if (index >= 0) {
                break;
            }
        }
        if (box >= 3) {
            return 0;
        }
    } else {
        index = pcboxSearchEmptyPokemon(pc, box);
        if (index < 0) {
            return 0;
        }
    }
    record = pcboxGetPokemonRecord(pc, box, index);
    if (record == NULL) {
        return 0;
    }
    *(PCBoxPokemonRecord*)record = *(PCBoxPokemonRecord*)src;
    pokemonAllKaihuku(record);
    return 1;
}
