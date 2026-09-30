/**
 * @file memo_r57b_8025FD34_suffix.c
 * @brief memo.c carve, 0x8025FD34 - 0x8025FEE4: memoDataGetPokemonTrainerRndFromID,
 *        memoDataGetPokemonRndFromID and memoDataGetPokemonID.
 *
 * Text only, built -O4,s like the rest of memo.c. The memo block (save-data
 * status 0xC) is a u16 count followed by 12-byte records whose +4 halfword
 * holds the Pokemon ID in its low 14 bits and +8/+0xC the trainer and
 * Pokemon random values. The two lookups repeat the NULL-block fallback
 * before reading the count, as memoDataGetCount (0x8025FEE4) does. The same
 * bodies are in memo.c.
 */
#include "dolphin/types.h"

extern void* savedataGetStatus(s32 side, s32 slotType);

u32 memoDataGetPokemonTrainerRndFromID(u16* block, u16 id)
{
    u16* memo;
    u16* countMemo;
    u16 count;
    u32 offset;
    u32 i;
    s32 entryID;

    memo = block;
    if (memo == NULL) {
        memo = (u16*)savedataGetStatus(0, 0xC);
    }
    countMemo = memo;
    if (memo == NULL) {
        countMemo = (u16*)savedataGetStatus(0, 0xC);
    }
    count = (u32)*countMemo;
    for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
        offset = (i & 0xFFFF) * 12;
        entryID = offset + 4;
        entryID = (u32)*(u16*)((u8*)memo + entryID) & 0x3FFF;
        if (entryID == (u16)id) {
            memo = (u16*)((u8*)memo + offset);
            return *(u32*)((u8*)memo + 8);
        }
    }
    return 0;
}

u32 memoDataGetPokemonRndFromID(u16* block, u16 id)
{
    u16* memo;
    u16* countMemo;
    u32 offset;
    u32 i;
    u16 count;
    s32 entryID;

    memo = block;
    if (memo == NULL) {
        memo = (u16*)savedataGetStatus(0, 0xC);
    }
    countMemo = memo;
    if (memo == NULL) {
        countMemo = (u16*)savedataGetStatus(0, 0xC);
    }
    count = (u32)*countMemo;
    for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
        offset = (i & 0xFFFF) * 12;
        entryID = offset + 4;
        entryID = (u32)*(u16*)((u8*)memo + entryID) & 0x3FFF;
        if (entryID == (u16)id) {
            memo = (u16*)((u8*)memo + offset);
            return *(u32*)((u8*)memo + 0xC);
        }
    }
    return 0;
}

u16 memoDataGetPokemonID(u16* memo, u32 index)
{
    u16 id;

    if (memo == NULL) {
        memo = (u16*)savedataGetStatus(0, 0xC);
    }
    if (*memo != 0) {
        memo = (u16*)((u8*)memo + (index & 0xFFFF) * 12);
        id = *(u16*)((u8*)memo + 4);
    } else {
        id = 0;
    }
    return id;
}
