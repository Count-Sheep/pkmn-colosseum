/**
 * @file gs_range_8018FE30.c
 * @brief gs-engine, 0x8018FE30 - 0x801902E0, fn_801903B0/fn_80190528
 *        (0x801903B0 - 0x801906A0; see gs_range_8018FE30_suffix_801903B0.c)
 *        and _flagSet (0x8019075C - 0x801908D4; see
 *        gs_range_8018FE30_suffix_8019075C.c).
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

typedef struct FlagStateEntry {
    u32 unused;
    u32* buffer;
} FlagStateEntry;

typedef struct FlagDefinition {
    u8 typeAndWidth;
    u8 initialValue;
    u8 itemSwap;
    u8 event;
    u16 bitPosition;
    s16 next;
} FlagDefinition;

typedef struct FlagTypeAndWidth {
    u8 type : 2;
    u8 width : 6;
} FlagTypeAndWidth;

typedef struct FlagConfig {
    u32 count;
    s16 head;
} FlagConfig;

typedef struct FlagSceneEntry {
    u8 memberFlags;
    u8 pad_01;
    u16 floorId;
    u16 pokedoru;
    u16 pad_06;
    f32 posX;
    f32 posY;
    f32 posZ;
    void* data;
} FlagSceneEntry;

extern const char lbl_802741F8[];
extern const char lbl_802742B8[];
extern void GSlogWrite(const char* fmt, ...);
void GSflagInitBitPos(FlagDefinition* definitions, u32 count, u32 capacity1,
                      u32 capacity2, u32 capacity3);

typedef struct FlagInitState {
    u32 _pad00[2];
    u32 count1;
    u32* buffer1;
    u32 count2;
    u32* buffer2;
    u32 count3;
    u32* buffer3;
} FlagInitState;

void fn_801903B0(s32 flagId);
void _flagSet(s32 flagId, u32 value);
extern void* fn_800FF56C(void);
extern void* savedataGetStatus(u8* data, u16 index);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, s32 arg4);
extern void fn_8012F1FC(s32 slot);
extern void fn_8012F40C(s32 slot);
extern s32 heroMoveDismissMember(s32 idx);
extern void heroBiosSetPokedoru(u16 value);
extern void floorChangePos(u32 arg0, void* data, f32 posX, f32 posY, f32 posZ);

#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY) && !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
void fn_8018FE30(s32 flagId)
{
    extern u8* lbl_80478F9C;
    extern FlagConfig* lbl_80478F98;
    extern FlagStateEntry* lbl_80478EEC;
    extern u32** lbl_80478ED4;
    extern u8* lbl_80478EE4;
    extern u16* lbl_80478EF4;
    extern u8* lbl_80478EFC;
    extern u32 lbl_8036C568[];
    u8* definitions = lbl_80478F9C;
    s32 current;
    u8* definition;
    u8* scene;
    u8* memberFlags;
    FlagStateEntry* state;
    u32 definitionOffset;
    u32 typeAndWidth;
    u32* buffer;
    u32 bitWidth;
    u32 bitOffset;
    u32 wordIndex;
    u32 bitPosition;
    u32 mask;
    u32 remaining;
    u32 value;
    u16* itemSwapTable;
    s32 i;

    if (flagId < 0) {
        return;
    }

    for (current = flagId; current != -1;
         current = *(s16*)(definitions + (current << 3) + 6)) {
        definitionOffset = current << 3;
        definition = definitions + definitionOffset;
        if (definition[1] != 0) {
            typeAndWidth = definition[0];
            state = lbl_80478EEC + ((typeAndWidth & 0xC0) >> 6);
            buffer = state->buffer;
            value = 0;
            if (buffer == NULL) {
                GSlogWrite(lbl_802741F8);
            } else {
                bitWidth = typeAndWidth & 0x3F;
                bitOffset = *(u16*)(definition + 4);
                if (32 - __cntlzw(value) > bitWidth) {
                    GSlogWrite(lbl_802741F8 + 0x34, current, value, value,
                               32 - __cntlzw(value), bitWidth);
                    value &= lbl_8036C568[bitWidth];
                }
                wordIndex = bitOffset >> 5;
                bitPosition = bitOffset & 0x1F;
                if (bitWidth > 1) {
                    mask = lbl_8036C568[bitWidth];
                    buffer[wordIndex] =
                        (buffer[wordIndex] & ~(mask << bitPosition)) |
                        (value << bitPosition);
                    if (bitWidth + bitPosition >= 32) {
                        remaining = bitWidth + bitPosition - 32;
                        mask = lbl_8036C568[remaining];
                        buffer[wordIndex + 1] =
                            (buffer[wordIndex + 1] & ~mask) |
                            (value >> (bitWidth - remaining));
                    }
                } else if (value == 0) {
                    buffer[wordIndex] &= ~(1u << bitPosition);
                } else {
                    buffer[wordIndex] |= 1u << bitPosition;
                }
            }
        }
    }

    current = lbl_80478F98->head;
    while (current != flagId && current != -1) {
        definitionOffset = current << 3;
        definition = definitions + definitionOffset;

        if (definition[1] != 0) {
            value = lbl_80478ED4[definition[1]][0];
            typeAndWidth = definition[0];
            state = lbl_80478EEC + ((typeAndWidth & 0xC0) >> 6);
            buffer = state->buffer;
            if (buffer == NULL) {
                GSlogWrite(lbl_802741F8);
            } else {
                bitWidth = typeAndWidth & 0x3F;
                bitOffset = *(u16*)(definition + 4);
                if (32 - __cntlzw(value) > bitWidth) {
                    GSlogWrite(lbl_802741F8 + 0x34, current, value, value,
                               32 - __cntlzw(value), bitWidth);
                    value &= lbl_8036C568[bitWidth];
                }
                wordIndex = bitOffset >> 5;
                bitPosition = bitOffset & 0x1F;
                if (bitWidth > 1) {
                    mask = lbl_8036C568[bitWidth];
                    buffer[wordIndex] =
                        (buffer[wordIndex] & ~(mask << bitPosition)) |
                        (value << bitPosition);
                    if (bitWidth + bitPosition >= 32) {
                        remaining = bitWidth + bitPosition - 32;
                        mask = lbl_8036C568[remaining];
                        buffer[wordIndex + 1] =
                            (buffer[wordIndex + 1] & ~mask) |
                            (value >> (bitWidth - remaining));
                    }
                } else if (value == 0) {
                    buffer[wordIndex] &= ~(1u << bitPosition);
                } else {
                    buffer[wordIndex] |= 1u << bitPosition;
                }
            }
        }

        if (definition[2] != 0) {
            itemSwapTable = &lbl_80478EF4[definition[2] * 2];
            heroItemDecItemDataId(0, itemSwapTable[1], 1, -1);
            heroItemAddItemDataId(0, itemSwapTable[0], 1, -1);
        }
        current = *(s16*)(definition + 6);
    }

    definition = definitions + (flagId << 3);
    if (definition[2] != 0) {
        itemSwapTable = &lbl_80478EF4[definition[2] * 2];
        heroItemDecItemDataId(0, itemSwapTable[1], 1, -1);
        heroItemAddItemDataId(0, itemSwapTable[0], 1, -1);
    }

    if (definition[3] != 0) {
        scene = lbl_80478EFC + (definition[3] * 0x18);
        if (scene[0] != 0) {
            fn_8012F1FC(0);
            fn_8012F40C(0);
            heroMoveDismissMember(1);

            memberFlags = lbl_80478EE4 + (scene[0] * 2);
            for (i = 0; i < 2; i++) {
                if (memberFlags[i] != 0) {
                    fn_8012F1FC(i);
                }
            }
        }

        savedataGetStatus(0, 2);
        heroBiosSetPokedoru(*(u16*)(scene + 4));
        if (*(u16*)(scene + 2) == 0) {
            fn_800FF56C();
        }
        floorChangePos(0, scene, *(f32*)(scene + 8), *(f32*)(scene + 0xC),
                       *(f32*)(scene + 0x10));
    }
}
#endif

/* GSflagClear, fn_801909A8 (XD GSflagInit) and GSflagInitBitPos, 0x801908D4 -
 * 0x80190E34, are linked from gs_flag_exact_801908D4.c. */

/* fn_801902E0 (XD GSflagTest) is linked from gs_flag_test_exact_801902E0.c. */

/* XD's _flagSet(buffer, defs, flagId, value) (GSflag.o, trevor403/xd-asm
 * @ b1087f18 FUN_801a0490), inlined here into each setter the way XD's
 * GSflagSet (FUN_801a03a4) calls it with the level's buffer.
 *
 * The multi-bit branch reads each word into `bits`, clears the field in
 * place and ORs in a separately named `shifted` value: that keeps the loaded
 * word in the register the andc writes back, puts the shifted value first in
 * the or, and declaring `word` before `bitPosition` gives retail's r6/r7
 * colouring of the bit position and word pointer. */
static inline u32 flagGetBitLength(u32 value)
{
    return 32 - __cntlzw(value);
}

static inline void flagSetValue(u32* buffer, FlagDefinition* defs, s32 flagId,
                                u32 value)
{
    extern u32 lbl_8036C568[];
    extern const char lbl_8027422C[];
    u32 bitWidth;
    u32 bitOffset;
    u32 length;
    u32 wordIndex;
    u32 end;
    u32* word;
    u32 bitPosition;

    if (buffer == NULL) {
        GSlogWrite(lbl_802741F8);
        return;
    }
    bitWidth = defs[flagId].typeAndWidth & 0x3F;
    bitOffset = defs[flagId].bitPosition;
    length = flagGetBitLength(value);
    if (length > bitWidth) {
        GSlogWrite(lbl_8027422C, flagId, value, value, length, bitWidth);
        value &= lbl_8036C568[bitWidth];
    }
    wordIndex = bitOffset >> 5;
    bitPosition = bitOffset & 0x1F;
    if (bitWidth > 1) {
        u32 bits;
        u32 shifted;
        word = &buffer[wordIndex];
        end = bitWidth + bitPosition;
        bits = word[0];
        bits &= ~(lbl_8036C568[bitWidth] << bitPosition);
        shifted = value << bitPosition;
        word[0] = shifted | bits;
        if (end >= 32) {
            u32 spill = end - 32;
            bits = word[1];
            bits &= ~lbl_8036C568[spill];
            shifted = value >> (bitWidth - spill);
            word[1] = shifted | bits;
        }
    } else if (value == 0) {
        buffer[wordIndex] &= ~(1 << bitPosition);
    } else {
        buffer[wordIndex] |= 1 << bitPosition;
    }
}

#define DEFINE_FLAG_SET(name, args, valueExpr)                                \
    void name args                                                             \
    {                                                                          \
        extern FlagDefinition* lbl_80478F9C;                                   \
        extern FlagStateEntry* lbl_80478EEC;                                   \
        flagSetValue(                                                          \
            lbl_80478EEC[(lbl_80478F9C[flagId].typeAndWidth & 0xC0) >> 6].buffer, \
            lbl_80478F9C, flagId, valueExpr);                                  \
    }

#if !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
DEFINE_FLAG_SET(fn_801903B0, (s32 flagId), 0)
DEFINE_FLAG_SET(fn_80190528, (s32 flagId), 1)
#endif
#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY)
DEFINE_FLAG_SET(_flagSet, (s32 flagId, u32 value), value)
#endif

#undef DEFINE_FLAG_SET

/* fn_801906A0 (0x801906A0, XD GSflagGet with _flagGet inlined) is linked
 * from gs_flag_get_exact_801906A0.c. _flagSet (0x8019075C) is scored from
 * this file through gs_range_8018FE30_suffix_8019075C.c. */

#if !defined(GS_RANGE_8018FE30_SUFFIX_801903B0_ONLY) && !defined(GS_RANGE_8018FE30_SUFFIX_8019075C_ONLY)
void GSflagClear(s32 level)
{
    extern FlagStateEntry* lbl_80478EEC;
    extern const char lbl_802742B8[];
    extern void GSlogWrite(const char* fmt, ...);
    FlagStateEntry* states;
    u32* buffer;
    u32 wordCount;
    u32 i;

    states = lbl_80478EEC;
    buffer = states[level].buffer;
    if (buffer == 0) {
        GSlogWrite(lbl_802742B8);
    } else {
        wordCount = states[level].unused;
        for (i = 0; i < wordCount; i++) {
            buffer[i] = 0;
        }
    }
}
#endif
