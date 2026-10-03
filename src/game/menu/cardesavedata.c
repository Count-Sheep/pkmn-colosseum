/**
 * @file gs_range_8007FD64.c
 * @brief gs-engine code, 0x8007FD64 - 0x80088428 (28 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

#if !defined(CARDESAVEDATA_EXACT_8008102C_ONLY)
#define CARDESAVEDATA_ALL
#endif

/* ===== External function declarations (fn_80084A8C only) ===== */
extern void fn_8005CF2C();
extern void fn_800776E4();
extern void fn_8008ABA0();
extern void fn_80092E38();
extern void fn_80092FC8();
extern void fn_80093160();
extern void fn_800932F0();
extern void fn_800934E4();
extern void fn_80093610();
extern void fn_80093698();
extern void fn_800D3088();
extern void fn_800D37CC();
extern void menuCloseCustom();
extern s32 menuIsCheck();
extern void menuGetEnablePort();
extern s32 menuSetEnablePort();
extern void windowGetFreeWork();
extern void windowGetKeyInfo();
extern void winMsgOpen();
extern void winMsgClose();
extern void menuOpen();
extern void windowSearchID();
extern void _threadSwitch();

/* ===== SDA globals (fn_80084A8C only) ===== */
extern u8 lbl_80478950[4];
extern s8 lbl_80478954[4];
extern char lbl_8047C1A0[] __attribute__((section(".sdata2")));

/* ===== Rodata / data labels ===== */
extern u8 jumptable_802EEB78[];
extern u8 lbl_8026F2E8[];
extern u8 lbl_8026F488[];
extern f32 lbl_8047C1C8;
extern f32 lbl_8047C1CC;

typedef struct CardEPadState {
    /* 0x00 */ u8 pad00[4];
    /* 0x04 */ u16 trigger;
    /* 0x06 */ u16 repeat;
} CardEPadState;

typedef struct CardEModelAnim {
    /* 0x00 */ u32 modelId;
    /* 0x04 */ s16 anim;
    /* 0x06 */ s16 animAlt;
} CardEModelAnim;

typedef struct CardEGridTable {
    /* 0x00 */ u32 selectedIconModel[4];
    /* 0x10 */ u16 selectedIconAnim;
    /* 0x12 */ u8 pad12[2];
    /* 0x14 */ CardEModelAnim cell[3][3];
    /* 0x5C */ u32 cursorModel[4];
    /* 0x6C */ u16 cursorAnim;
} CardEGridTable;

typedef struct CardESelection {
    /* 0x00 */ u16 id[3];
} CardESelection;

extern void* fn_801054B8();
extern void* fn_800F92D4(u32);
extern void fn_800ECCA8(void*, s16);
extern void fn_800ECA78(void*, f32);
extern void fn_800EC9DC(void*, f32);
extern void fn_800EC990(void*);
extern void fn_800ECB74(void*, u32);
extern u8 fn_800EC960(void*);
extern void fn_80166A28();
extern s32 fn_801666BC();

extern void GScharLenCpy(void*, const void*, u32);
extern u16 fn_800E2C04(u32, u32);
extern void* fn_800E27B0(u16);
extern u16 fn_800E202C(void*);
extern void fn_800E24B0(u16);
extern void fn_800E209C(u16);
extern s32 fn_80083BF8(void*);
extern void* fn_80083AF4(void*, s32);
extern void* windowSearchItemID(void*, s32);
extern void qsort(void*, u32, u32, s32 (*)(u32, u32));
extern void __assert();
extern char lbl_80268B88[];
extern char lbl_8047C140[7];
extern char lbl_8047C178[] __attribute__((section(".sdata2")));
extern const u16 lbl_8047C190[4];

typedef struct MenuCardEItem {
    u8 pad0[0x50];
    s16 x;
    s16 y;
    s16 h;
    s16 w;
} MenuCardEItem;

#define CARDE_CTX_U32(ctx, off) (*(u32*)((u8*)(ctx) + (off)))
#define CARDE_CTX_S16(ctx, off) (*(s16*)((u8*)(ctx) + (off)))

#if defined(CARDESAVEDATA_ALL)
static void menuCardE_SetItem(void* ctx, u32 off, void* window, s32 id) {
    CARDE_CTX_U32(ctx, off) = (u32)windowSearchItemID(window, id);
}

static void menuCardE_CopyRect(void* ctx, u32 dst, u32 itemOff) {
    MenuCardEItem* item = (MenuCardEItem*)CARDE_CTX_U32(ctx, itemOff);

    CARDE_CTX_S16(ctx, dst + 0) = item->x;
    CARDE_CTX_S16(ctx, dst + 2) = item->y;
    CARDE_CTX_S16(ctx, dst + 6) = item->h;
    CARDE_CTX_S16(ctx, dst + 4) = item->w;
}
#endif

/* 0x8007FD64 | size: 0x58
 * menuCardE_CompareEntryPtrs: qsort-style comparator for MenuCardEEntry*
 * elements.
 */
#if defined(CARDESAVEDATA_ALL)
s32 menuCardE_CompareEntryPtrs(u32 r3, u32 r4) {
    u32 r0;
    u32 r5;

    r5 = *(u32*)((u8*)r3 + 0x0);
    r4 = *(u32*)((u8*)r4 + 0x0);
    r3 = *(u8*)((u8*)r5 + 0x1C);
    r0 = *(u8*)((u8*)r4 + 0x1C);
    r3 = (s8)r3;
    r0 = (s8)r0;
    if ((s32)r3 < (s32)r0) {
        return 0x1;
    }
    if ((s32)r3 > (s32)r0) {
        return -0x1;
    }
    r3 = *(u8*)((u8*)r5 + 0x1A);
    r0 = *(u8*)((u8*)r4 + 0x1A);
    if (r3 < r0) {
        return -0x1;
    }
    r0 = r0 - r3;
    r3 = (u32)r0 >> 31;
    return r3;
}
#endif

extern void GScharCpy(void* dst, const void* src);
extern const u8 lbl_80268DC0[];
extern const u8 lbl_80478948[8];
extern u8 fn_8008102C(void** object_ref, const u32* descriptor, s32 index,
                      s32 value, const char* text, s32 subindex);

/* One packed field: its decoder id, its bit width and its element count. */
typedef struct CardEFieldDesc {
    /* 0x0 */ u32 field;
    /* 0x4 */ s32 width;
    /* 0x8 */ s32 count;
} CardEFieldDesc;

/* The descriptor tables at the head of the Card-e rodata. */
typedef struct CardEFieldTable {
    /* 0x000 */ CardEFieldDesc header[40];
    /* 0x1E0 */ CardEFieldDesc trainer[8];
    /* 0x240 */ CardEFieldDesc pokemon[24];
    /* 0x360 */ CardEFieldDesc extra[3];
} CardEFieldTable;

/* Bit reader handed to fn_8008102C; the decoded object comes first. */
typedef struct CardEReader {
    /* 0x0 */ void* object;
    /* 0x4 */ const u8* packed;
    /* 0x8 */ u32 size;
    /* 0xC */ s32 bitPosition;
} CardEReader;

static inline u16 CardEPeekBits(const u8* packed, s32 start, s32 count)
{
    s32 end = start + count;
    u16 value = 0;
    s32 cursor;

    for (cursor = start; cursor < end; cursor++) {
        value = (value << 1) |
                ((packed[cursor / 8] & lbl_80478948[cursor & 7]) ? 1 : 0);
    }
    return value;
}

static inline u16 CardEReadBits(CardEReader* reader, s32 count)
{
    u16 value = CardEPeekBits(reader->packed, reader->bitPosition, count);

    reader->bitPosition += count;
    return value;
}

static inline void CardEReadText(CardEReader* reader,
                                 const CardEFieldDesc* desc, u16* text)
{
    s32 cursor = reader->bitPosition;
    const u8* packed = reader->packed;
    s32 remaining = desc->width;

    while (remaining > 16) {
        *text++ = CardEPeekBits(packed, cursor, 16);
        cursor += 16;
        remaining -= 16;
    }
    if (remaining != 0) {
        *text++ = CardEPeekBits(packed, cursor, remaining);
    }
    *text = 0;
    reader->bitPosition += desc->width;
}

static inline u8 CardEReadField(CardEReader* reader,
                                const CardEFieldDesc* descs, u32 n,
                                s32 index)
{
    u16 text[256];
    s32 i;
    u8 ok = 1;

    if (descs[n].width < 16) {
        for (i = 0; i < descs[n].count; i++) {
            if (!fn_8008102C((void**)reader, (const u32*)&descs[n], index,
                             CardEReadBits(reader, descs[n].width), NULL,
                             i)) {
                ok = 0;
            }
        }
    } else {
        CardEReadText(reader, &descs[n], text);
        if (!fn_8008102C((void**)reader, (const u32*)&descs[n], index, 0,
                         (const char*)text, -1)) {
            ok = 0;
        }
    }
    return ok;
}

/* Decode and validate a packed card-e record. */
#pragma push
#if defined(CARDESAVEDATA_ALL)
u32 fn_80080310(void* output, const u8* packed, u32 size)
{
    CardEReader reader;
    u32 i;
    s32 record;
    u32 group;
    u8 valid = 1;
    const CardEFieldTable* table = (const CardEFieldTable*)lbl_80268DC0;

    memset(output, 0, 0xB20);
    reader.object = output;
    reader.packed = packed;
    reader.size = size;
    reader.bitPosition = 0;
    CardEReadField(&reader, table->header, 0, -1);

    reader.bitPosition = 0;
    switch (*(s32*)output) {
    case 0:
        for (i = 0; i < 40; i++) {
            if (!CardEReadField(&reader, table->header, i, -1)) {
                valid = 0;
            }
        }
        for (record = 0; record < 9; record++) {
            for (group = 0; group < 8; group++) {
                if (!CardEReadField(&reader, table->trainer, group, record)) {
                    valid = 0;
                }
            }
        }
        for (record = 0; record < 36; record++) {
            for (group = 0; group < 24; group++) {
                if (!CardEReadField(&reader, table->pokemon, group, record)) {
                    valid = 0;
                }
            }
        }
        break;
    case 1:
        for (i = 0; i < 3; i++) {
            if (!CardEReadField(&reader, table->extra, i, -1)) {
                valid = 0;
            }
        }
        break;
    default:
        valid = 0;
        break;
    }

    if (!valid) {
        return 0;
    }
    return (u32)reader.bitPosition <= reader.size * 8;
}
#endif
#pragma pop


/* Range checks inlined into fn_8008102C; the result is materialised. */
static inline s32 CardEValueInRange(s32 value, s32 max)
{
    return value >= 0 && value <= max;
}

/* Table lookups inlined into fn_8008102C; each result is materialised. */
static inline u8 CardEFindHalf(const u16* entry, s32 count, u16 value)
{
    s32 i;

    for (i = 0; i < count; entry++, i++) {
        if (value == *entry) {
            return 1;
        }
    }
    return 0;
}

static inline u8 CardEFindByte(const u8* entry, s32 count, u8 value)
{
    s32 i;

    for (i = 0; i < count; entry++, i++) {
        if (*entry == value) {
            return 1;
        }
    }
    return 0;
}

static inline u8 CardEFindSByte(const s8* entry, s32 count, s8 value)
{
    s32 i;

    for (i = 0; i < count; entry++, i++) {
        if (*entry == value) {
            return 1;
        }
    }
    return 0;
}

static inline u8 CardEFindWord(const u32* entry, s32 count, u32 value)
{
    s32 i;

    for (i = 0; i < count; entry++, i++) {
        if (value == *entry) {
            return 1;
        }
    }
    return 0;
}

static inline u8 CardEIsLeaderSlot(const u8* object, s32 index)
{
    if ((s8)object[0x5B] == index) {
        return 1;
    }
    if ((s8)object[0x5C] == index) {
        return 1;
    }
    if ((s8)object[0x5D] == index) {
        return 1;
    }
    return 0;
}

typedef struct CardETrainerEntry {
    /* 0x00 */ u16 name[6];
    /* 0x0C */ u8 enabled;
    /* 0x0D */ s8 item[4];
    /* 0x11 */ u8 pad11;
    /* 0x12 */ u16 move[4];
    /* 0x1A */ u8 pad1A[2];
    /* 0x1C */ u32 field1C;
    /* 0x20 */ u16 field20;
    /* 0x22 */ u16 field22;
    /* 0x24 */ u8 field24;
    /* 0x25 */ u8 pad25[3];
} CardETrainerEntry;

typedef struct CardEPokemonEntry {
    /* 0x00 */ u16 species;
    /* 0x02 */ u8 field02;
    /* 0x03 */ u8 field03;
    /* 0x04 */ u16 field04[4];
    /* 0x0C */ u16 field0C;
    /* 0x0E */ s8 field0E;
    /* 0x0F */ s8 field0F[6];
    /* 0x15 */ u8 pad15;
    /* 0x16 */ s16 field16[7];
    /* 0x24 */ u8 field24;
    /* 0x25 */ s8 field25;
    /* 0x26 */ u8 field26;
    /* 0x27 */ u8 field27;
    /* 0x28 */ u8 field28;
    /* 0x29 */ u8 pad29;
} CardEPokemonEntry;

typedef struct CardERecordData {
    /* 0x000 */ u8 header[0x3AC];
    /* 0x3AC */ CardETrainerEntry trainer[9];
    /* 0x514 */ CardEPokemonEntry pokemon[36];
    /* 0xAFC */ u32 fieldAFC;
} CardERecordData;

static inline u8 CardEIsValidType(u8 type)
{
    switch (type) {
    case 0:
    case 13:
    case 18:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 32:
    case 36:
        return 1;
    }
    return 0;
}

/* Apply one decoded card-e field and reject values outside its domain. */
#pragma push
#pragma optimization_level 3
u8 fn_8008102C(void** object_ref, const u32* descriptor, s32 index,
               s32 value, const char* text, s32 subindex)
{
#define object (*(u8**)object_ref)
#define card (*(CardERecordData**)object_ref)
    u32 field = descriptor[0];
    const u8* table = lbl_80268DC0;
    s32 i;
    s32 j;
    const u16* tableEntry;
    u8* record;
    u16 half;

    switch (field) {
    case 0:
    case 72: /* retail's jump table sends field 72 to the field-0 store */
        *(s32*)object = value;
        switch (*(s32*)object) {
        case 0:
        case 1:
            break;
        default:
            return 0;
        }
        break;
    case 1:
        object[4] = (u8)value;
        switch (object[4]) {
        default:
        case 0:
            return 0;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
        }
        break;
    case 2:
        object[5] = (u8)value;
        switch (object[5]) {
        default:
        case 0:
            return 0;
        case 1:
        case 2:
        case 3:
            break;
        }
        break;
    case 3:
        object[6] = (u8)value;
        switch (object[6]) {
        default:
        case 0:
            return 0;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        }
        break;
    case 4:
        object[7] = (u8)value;
        switch (object[7]) {
        case 0:
            return 0;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
            break;
        }
        break;
    case 5:
        object[8] = (u8)value;
        break;
    case 6:
        GScharCpy(object + 0x0A, text);
        break;
    case 7:
        ((s8*)object)[0x24] = (value - 1);
        if (value < 0 || value >= 6) {
            return 0;
        }
        break;
    case 8:
        object[0x25] = (u8)value;
        break;
    case 9:
        ((s8*)object)[0x26] = (value - 1);
        if ((s8)object[0x26] < 0 || (s8)object[0x26] >= 5) {
            return 0;
        }
        break;
    case 10:
        GScharCpy(object + 0x28, text);
        break;
    case 11:
        GScharCpy(object + 0x38, text);
        break;
    case 12:
        GScharCpy(object + 0x48, text);
        break;
    case 13:
        ((s8*)object)[0x58] = value;
        if (value < 1 || value > 3) {
            return 0;
        }
        break;
    case 14:
        ((s8*)object)[0x59] = value;
        if (value < 1 || value > 6) {
            return 0;
        }
        break;
    case 15:
        ((s8*)object)[0x5A] = value;
        if (value < 1 || value > 5) {
            return 0;
        }
        break;
    case 16:
        ((s8*)object)[0x5B] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 17:
        ((s8*)object)[0x5C] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 18:
        ((s8*)object)[0x5D] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 19:
        ((s8*)object)[0x5E] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 20:
        ((s8*)object)[0x5F] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 21:
        ((s8*)object)[0x60] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 22:
        ((s8*)object)[0x61] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 23:
        ((s8*)object)[0x62] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 24:
        ((s8*)object)[0x63] = (value - 1);
        if (value < 0 || value > 9) {
            return 0;
        }
        break;
    case 25:
        *(u16*)(object + 0x64) = (u16)value;
        if (!CardEFindHalf((const u16*)(table + 0x384), 0x2F * 7,
                           *(u16*)(object + 0x64))) {
            return 0;
        }
        break;
    case 26:
        *(u16*)(object + 0x66) = (u16)value;
        if (!CardEFindHalf((const u16*)(table + 0x384), 0x2F * 7,
                           *(u16*)(object + 0x66))) {
            return 0;
        }
        break;
    case 27:
        *(u16*)(object + 0x68) = (u16)value;
        if (!CardEFindHalf((const u16*)(table + 0x384), 0x2F * 7,
                           *(u16*)(object + 0x68))) {
            return 0;
        }
        break;
    case 28:
        object[0x6A] = (u8)value;
        if (!CardEIsValidType(object[0x6A])) {
            return 0;
        }
        break;
    case 29:
        object[0x6B] = (u8)value;
        if (!CardEIsValidType(object[0x6B])) {
            return 0;
        }
        break;
    case 30:
        object[0x6C] = (u8)value;
        if (!CardEIsValidType(object[0x6C])) {
            return 0;
        }
        break;
    case 31:
        GScharCpy(object + 0x6E, text);
        break;
    case 32:
        GScharCpy(object + 0x182, text);
        break;
    case 33:
        GScharCpy(object + 0x296, text);
        break;
    case 34:
        GScharCpy(object + 0xCA, text);
        break;
    case 35:
        GScharCpy(object + 0x1DE, text);
        break;
    case 36:
        GScharCpy(object + 0x2F2, text);
        break;
    case 37:
        GScharCpy(object + 0x126, text);
        break;
    case 38:
        GScharCpy(object + 0x23A, text);
        break;
    case 39:
        GScharCpy(object + 0x34E, text);
        break;
    case 40:
        GScharCpy(object + 0x3AC + index * 0x28, text);
        break;
    case 41:
        switch (value) {
        case 0:
            card->trainer[index].enabled = 1;
            break;
        case 1:
            card->trainer[index].enabled = 0;
            break;
        default:
            card->trainer[index].enabled = 0;
            return 0;
        }
        break;
    case 42:
        card->trainer[index].item[subindex] = value - 1;
        if (value < 0 || value > 0x24) {
            return 0;
        }
        break;
    case 43:
        card->trainer[index].move[subindex] = value;
        if (!CardEFindHalf((const u16*)(table + 0x384), 0x2F * 7, value)) {
            return 0;
        }
        break;
    case 44:
        card->trainer[index].field1C = value;
        break;
    case 45:
        card->trainer[index].field20 = value;
        if (!CardEFindHalf((const u16*)(table + 0x618), 0x13 * 4, value)) {
            return 0;
        }
        break;
    case 46:
        card->trainer[index].field22 = value;
        if (CardEIsLeaderSlot(object, index) && (u16)value > 999) {
            return 0;
        }
        break;
    case 47:
        card->trainer[index].field24 = value;
        if (!CardEFindHalf((const u16*)(table + 0x6B0), 4 * 8, (u8)value)) {
            return 0;
        }
        break;
    case 48:
        card->pokemon[index].species = value;
        if (!CardEFindHalf((const u16*)(table + 0x6F0), 0x2B * 9, (u8)value)) {
            return 0;
        }
        break;
    case 49:
        card->pokemon[index].field02 = value;
        if (!CardEFindByte(table + 0x9F8, 3 * 10, value)) {
            return 0;
        }
        break;
    case 50:
        card->pokemon[index].field03 = value;
        break;
    case 51:
        card->pokemon[index].field04[subindex] = value;
        if (!CardEFindHalf((const u16*)(table + 0xA18), 0x47 * 5, (u8)value)) {
            return 0;
        }
        break;
    case 52:
        card->pokemon[index].field0C = value;
        if (!CardEFindHalf((const u16*)(table + 0x384), 0x2F * 7, value)) {
            return 0;
        }
        break;
    case 53:
        switch (value) {
        case 0:
        case 1:
            card->pokemon[index].field0E = value;
            break;
        default:
            card->pokemon[index].field0E = -1;
            break;
        }
        break;
    case 54:
        card->pokemon[index].field0F[0] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 55:
        card->pokemon[index].field0F[1] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 56:
        card->pokemon[index].field0F[2] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 57:
        card->pokemon[index].field0F[3] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 58:
        card->pokemon[index].field0F[4] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 59:
        card->pokemon[index].field0F[5] = CardEValueInRange(value, 0x1F) ? (s8)value : (s8)-1;
        break;
    case 60:
        card->pokemon[index].field16[0] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 61:
        card->pokemon[index].field16[1] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 62:
        card->pokemon[index].field16[2] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 63:
        card->pokemon[index].field16[3] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 64:
        card->pokemon[index].field16[4] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 65:
        card->pokemon[index].field16[5] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 66:
        card->pokemon[index].field16[6] = CardEValueInRange(value, 0xFF) ? (s16)value : (s16)-1;
        break;
    case 67:
        switch (value) {
        default:
        case 0:
            card->pokemon[index].field24 = 0;
            break;
        case 1:
            card->pokemon[index].field24 = 0;
            break;
        case 2:
            card->pokemon[index].field24 = 1;
            break;
        case 3:
            card->pokemon[index].field24 = 2;
            break;
        }
        break;
    case 68:
        if (value & 0x20) {
            card->pokemon[index].field25 = -1;
        } else {
            card->pokemon[index].field25 = value - 1;
        }
        if (!CardEFindSByte((const s8*)(table + 0xCE0), 0x0D * 2,
                            card->pokemon[index].field25)) {
            return 0;
        }
        break;
    case 69:
        card->pokemon[index].field26 = value;
        switch (card->pokemon[index].field26) {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
        default:
            return 0;
        }
        break;
    case 70:
        card->pokemon[index].field27 = value;
        switch (card->pokemon[index].field27) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            break;
        default:
            card->pokemon[index].field27 = 0;
            return 0;
        }
        break;
    case 71:
        card->pokemon[index].field28 = value;
        break;
    case 73:
        card->fieldAFC = value;
        if (!CardEFindWord((const u32*)(table + 0xCFC), 0x2B, value)) {
            return 0;
        }
        break;
    case 74:
        GScharCpy(object + 0xB00, text);
        break;
    default:
        __assert(table + 0x63E8, 0x8C5, lbl_8047C178);
        return 0;
    }
#undef object
#undef card
    return 1;
}
#pragma pop

#if defined(CARDESAVEDATA_ALL)
typedef struct CardEGridEntry {
    u16 id;
    u8 pad02[0x18];
    u8 key;
    s8 layers;
    s8 rows;
    s8 columns;
    u8 pad1E[6];
    u8 data[1];
} CardEGridEntry;

typedef struct CardEGridMatrixCell {
    u8 pad00[0xC];
    u8 valid;
    u8 pad0D[3];
} CardEGridMatrixCell;

typedef struct CardEGridLayer {
    u8 pad00[0x76];
    CardEGridMatrixCell cells[1];
} CardEGridLayer;

static inline u32 CardEGridEntrySize(CardEGridEntry* entry)
{
    return 0x24 + entry->layers *
           (0x76 + ((entry->rows * entry->columns) << 4));
}

static inline void CardEGridValidate(CardEGridEntry* entry)
{
    if (entry->layers <= 3 && entry->rows <= 6 && entry->columns <= 5) {
        return;
    }
    entry->id = 0;
}

static inline s32 CardEGridLayerIsValid(CardEGridEntry* entry, s8 layer)
{
    return layer >= 0 && layer < entry->layers;
}

static inline void CardEGridSetEntry(CardEGridEntry** entryOut,
                                     CardEGridEntry* entry)
{
    if (entryOut != NULL) {
        *entryOut = entry;
    }
}

/* Return one well-formed record, or the terminating slot for a negative index. */
static inline CardEGridEntry* CardEGridGetEntry(void* arena, s32 index)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    CardEGridEntry* result;
    u8* end;
    s32 currentIndex;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    CardEGridSetEntry(&result, NULL);
    currentIndex = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        if (currentIndex == index) {
            result = entry;
        }
        currentIndex++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    if (index < 0) {
        result = entry;
    }
    return result;
}

/* Clear an empty decoded card-e grid entry. */
#pragma push
void fn_80082650(CardEGridEntry* entry)
{
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    extern char lbl_8047C188[] __attribute__((section(".sdata2")));
    CardEGridLayer* layer;
    s32 i;
    u8 valid;

    if (entry == NULL) {
        __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
    }
    if (!CardEGridLayerIsValid(entry, 0)) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    layer = (CardEGridLayer*)entry->data;
    if (layer == NULL) {
        __assert(lbl_8026F1C8, 0x1F1, lbl_8047C188);
    }

    for (i = entry->rows * entry->columns; i > 0; i--) {
        if (layer->cells[0].valid != 0) {
            valid = 1;
            goto scan_done;
        }
        layer = (CardEGridLayer*)((u8*)layer + sizeof(CardEGridMatrixCell));
    }
    valid = 0;
scan_done:
    if (!valid) {
        entry->id = 0;
    }
}
#pragma pop

typedef struct CardEPageLayout {
    u8 field_00[0x10];
    u8 summary[0x66];
    u8 cells[1][0x10];
} CardEPageLayout;

extern char lbl_8047C180[] __attribute__((section(".sdata2")));
extern char lbl_8047C188[] __attribute__((section(".sdata2")));

/* Asserts on source lines 0x17F/0x180, shared by every grid accessor. */
static inline CardEPageLayout* CardEGetLevel(CardEGridEntry* series, s8 level)
{
    if (series == NULL) {
        __assert("cardesavedata.c", 0x17F, lbl_8047C180);
    }
    if (!CardEGridLayerIsValid(series, level)) {
        __assert("cardesavedata.c", 0x180, "0 <= level && level < series->level_max");
    }
    return (CardEPageLayout*)((u8*)series->data +
        level * (0x76 + ((series->rows * series->columns) << 4)));
}

static inline void CardEGridSetCountOut(s32* countOut, s32 count)
{
    if (countOut != NULL) {
        *countOut = count;
    }
}

/* fn_80083BF8's body: count the well-formed records. */
static inline s32 CardEGridCountEntries(void* arena)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    s32 currentCount;
    s32 count;
    u8* end;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    currentCount = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        currentCount++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    CardEGridSetCountOut(&count, currentCount);
    return count;
}

/* Append an empty record after the last one, or NULL when it won't fit. */
static inline CardEGridEntry* CardEGridAppend(void* arena, s8 layers, s8 rows,
                                              s8 columns)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    CardEGridEntry* tail;
    u8* base;
    s32 currentIndex;
    u8* end;
    u8* next;

    if (arena != NULL) {
        base = arena;
    } else {
        base = savedataGetStatus(0, 0xD);
    }
    entry = (CardEGridEntry*)base;
    end = base + 0x4000;
    CardEGridSetEntry(&tail, NULL);
    currentIndex = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        if (currentIndex == -1) {
            tail = entry;
        }
        currentIndex++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    CardEGridSetEntry(&tail, entry);
    next = (u8*)tail + (layers * (0x76 + ((rows * columns) << 4)) + 0x24);
    if (base + 0x4000 < next) {
        return NULL;
    }
    memset(tail, 0, next - (u8*)tail);
    return tail;
}

void* fn_800836AC(u8* arena, u8* descriptor, u8 create)
{
    extern void fn_800CAA3C(void*, const void*);
    CardEGridEntry* series;
    int count;
    s32 i;

    if (*(s32*)descriptor != 0) {
        return NULL;
    }
    count = CardEGridCountEntries(arena);
    series = NULL;
    for (i = 0; i < count; i++) {
        series = CardEGridGetEntry(arena, i);
        if (series->key == descriptor[8]) {
            break;
        }
    }
    if (i == count) {
        if (!create) {
            return NULL;
        }
        series = CardEGridAppend(arena, ((s8*)descriptor)[0x58],
                                 ((s8*)descriptor)[0x59], ((s8*)descriptor)[0x5A]);
        if (series != NULL) {
            fn_800CAA3C(series, descriptor + 0x0A);
            series->key = descriptor[8];
            series->layers = ((s8*)descriptor)[0x58];
            series->rows = ((s8*)descriptor)[0x59];
            series->columns = ((s8*)descriptor)[0x5A];
            for (i = 0; i < series->layers; i++) {
                fn_800CAA3C(CardEGetLevel(series, i), descriptor + 0x28 + i * 0x10);
            }
        }
    }
    return series;
}

static inline s32 cardEPageSize(const u8* card)
{
    return (s8)card[0x1C] * (s8)card[0x1D] * 0x10 + 0x76;
}

static inline u8* cardEGetPage(u8* card, s8 pageIndex)
{
    return card + 0x24 + pageIndex * cardEPageSize(card);
}

static inline u8* cardEGetCell(u8* card, s8 pageIndex, s8 row, s8 column)
{
    u8* page = cardEGetPage(card, pageIndex);
    s32 index = row * (s8)card[0x1D] + column;

    return page + 0x76 + index * 0x10;
}

/* fn_80080ED8 (0x80080ED8) is its own unit:
 * cardesavedata_candidate_80080ED8_gc125.c. */


void fn_80082960(u8* card, const u8* window, s8 pageIndex)
{
    CardEGridEntry* series = (CardEGridEntry*)card;
    CardEPageLayout* lv;
    u8* entry;

    if (card[0x1A] != window[8]) {
        __assert("cardesavedata.c", 0x209, "series->series_number == pCardE->series_number");
    }
    lv = CardEGetLevel(series, pageIndex);
    if (lv == NULL) {
        __assert("cardesavedata.c", 0x20C, lbl_8047C188);
    }
    entry = lv->summary + (s8)window[0x24] * 0x0E;
    *(u16*)entry = 0;
    entry[0x0C] = 0;
}

u32 fn_80082A88(u8* card, s8 pageIndex)
{
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    extern char lbl_8047C188[] __attribute__((section(".sdata2")));
    CardEGridEntry* grid = (CardEGridEntry*)card;
    u8* page;
    s32 count;
    s32 i;
    s32 valid;

    if (card == NULL) {
        __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
    }
    valid = 0;
    if (pageIndex >= 0 && pageIndex < grid->layers) {
        valid = 1;
    }
    if (!valid) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    page = card + pageIndex * (0x76 + ((grid->rows * grid->columns) << 4));
    page += 0x24;
    if (page == NULL) {
        __assert(lbl_8026F1C8, 0x1F1, lbl_8047C188);
    }
    count = grid->rows * grid->columns;
    for (i = 0; i < count; i++) {
        if (page[0x82] != 0) {
            return 1;
        }
        page += 0x10;
    }
    return 0;
}

u8* fn_80082BA4(u8* card, const u8* window, s8 pageIndex)
{
    extern void fn_800CAA3C(void*, const void*);
    CardEGridEntry* series = (CardEGridEntry*)card;
    CardEPageLayout* lv;
    u8* entry;

    if (card[0x1A] != window[8]) {
        __assert("cardesavedata.c", 0x1D1, "series->series_number == pCardE->series_number");
    }
    lv = CardEGetLevel(series, pageIndex);
    if (lv == NULL) {
        __assert("cardesavedata.c", 0x1D4, lbl_8047C188);
    }
    entry = lv->summary + (s8)window[0x24] * 0x0E;
    fn_800CAA3C(entry, window + 0x3AC + (s8)(window + 0x5E)[pageIndex] * 0x28);
    entry[0x0C] = 1;
    return (u8*)lv;
}

u8* fn_80082EA4(u8* card, s8 pageIndex, s8 row, s8 column)
{
    CardEGridEntry* series = (CardEGridEntry*)card;
    CardEPageLayout* lv = CardEGetLevel(series, pageIndex);

    if (lv == NULL) {
        __assert("cardesavedata.c", 0x198, lbl_8047C188);
    }
    if (row >= series->rows) {
        __assert("cardesavedata.c", 0x199, "pack < series->pack_max");
    }
    if (column >= series->columns) {
        __assert("cardesavedata.c", 0x19A, "card < series->trainer_card_max");
    }
    return lv->cells[row * series->columns + column];
}

/* fn_80082A88's body, inlined into fn_80082738 with the pooled strings. */
static inline u8 CardELevelInUse(CardEGridEntry* series, s8 level)
{
    CardEPageLayout* lv = CardEGetLevel(series, level);
    s32 count;
    s32 i;

    if (lv == NULL) {
        __assert("cardesavedata.c", 0x1F1, lbl_8047C188);
    }
    count = series->rows * series->columns;
    for (i = 0; i < count; i++) {
        if (lv->cells[i][0x0C] != 0) {
            return 1;
        }
    }
    return 0;
}

u32 fn_80082738(u8* card, const u8* window, s8 pageIndex)
{
    u8* cell;

    if (card[0x1A] != window[8]) {
        __assert("cardesavedata.c", 0x225, "series->series_number == pCardE->series_number");
    }
    cell = fn_80082EA4(card, pageIndex, ((s8*)window)[0x24], ((s8*)window)[0x26]);
    cell[0x0C] = 0;
    *(u16*)cell = 0;
    if (pageIndex == 0 && !CardELevelInUse((CardEGridEntry*)card, 0)) {
        return 1;
    }
    return 0;
}

u8* fn_80082CF0(u8* card, const u8* window, s8 pageIndex)
{
    extern void fn_800CAA3C(void*, const void*);
    u8* cell;
    const u8* descriptor;

    if (card[0x1A] != window[8]) {
        __assert("cardesavedata.c", 0x1B0, "series->series_number == pCardE->series_number");
    }
    cell = fn_80082EA4(card, pageIndex, ((s8*)window)[0x24], ((s8*)window)[0x26]);
    descriptor = window + 0x3AC + (s8)(window + 0x5B)[pageIndex] * 0x28;
    fn_800CAA3C(cell, descriptor);
    cell[0x0C] = 1;
    *(u16*)(cell + 0x0E) = *(const u16*)(descriptor + 0x22);
    card[0x1E + (s8)window[0x24]] = window[0x25];
    return cell;
}

/* Return the start of one layer in a decoded card-e grid entry. */
#pragma push
void* fn_80082FE4(CardEGridEntry* entry, s8 layer)
{
    extern char lbl_8026F1C8[];
    extern char lbl_8026F1D8[];
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    u8* layerEntry;

    if (entry == NULL) {
        __assert(lbl_8026F1C8, 0x17F, lbl_8047C180);
    }
    if (!CardEGridLayerIsValid(entry, layer)) {
        __assert(lbl_8026F1C8, 0x180, lbl_8026F1D8);
    }
    layerEntry = (u8*)entry;
    layerEntry += layer *
                  (0x76 + ((entry->rows * entry->columns) << 4));
    return layerEntry + 0x24;
}
#pragma pop

void fn_800830A4(u8* arena)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* series;
    s32 index;
    u8 validState;

    if (arena == NULL) {
        arena = savedataGetStatus(0, 0xD);
    }
    validState = arena[0x4000] == 1 || arena[0x4000] == 2;
    if (!validState) {
        __assert("cardesavedata.c", 0x161, "aex->state == CARDE_EX_TRAINER_STATE_APPEARING || aex->state == CARDE_EX_TRAINER_STATE_ALREADY_BATTLED_WITH");
    }
    arena[0x4000] = 0;

    index = 0;
    for (;;) {
        series = CardEGridGetEntry(arena, index);
        if (series == NULL) {
            __assert("cardesavedata.c", 0x169, lbl_8047C180);
        }
        if (series->key == arena[0x4001]) {
            break;
        }
        index++;
    }

    CardEGetLevel(series, ((s8*)arena)[0x4002])->summary[0x61] = 1;
}

void fn_800832C8(u8* arena, u8* cardData, s8 layer)
{
    typedef struct CardEObjectRecord {
        u16 id;
        u8 pad02[0xA];
        u8 enabled;
        s8 item[4];
        u8 pad11;
        u16 field12;
        u16 field14;
        u16 field16;
        u16 field18;
        u8 pad1A[2];
        u32 field1C;
        u16 field20;
        u8 pad22[2];
        u8 field24;
        u8 pad25[3];
    } CardEObjectRecord;
    typedef struct CardEObjectData {
        u8 bytes[0x2A];
    } CardEObjectData;
    extern void* savedataGetStatus(u32, u32);
    extern void fn_800CAA3C(void*, const void*);
    extern void fn_801EE1E0(u8 type, u16 id);
    extern void fn_801EE2B4(u8 type, void* data);
    extern void fn_801EE10C(u8 type, u8 key);
    extern char lbl_8047C180[] __attribute__((section(".sdata2")));
    u8* layerData;
    CardEObjectData* object;
    CardEGridEntry* found;
    s32 wanted;
    s32 i;
    s32 objectIndex;
    CardEObjectRecord* record;
    u8* pending;

    arena = arena != NULL ? arena : savedataGetStatus(0, 0xD);
    pending = arena + 0x4000;
    record = (CardEObjectRecord*)(cardData + 0x3AC +
                                  (s8)cardData[0x61 + layer] * 0x28);
    if (record->id == 0) {
        __assert("cardesavedata.c", 0x108, "trainer->name[0]");
    }

    pending[0] = 1;
    pending[1] = cardData[8];
    ((s8*)pending)[2] = layer;
    fn_800CAA3C(pending + 4, cardData + layer * 0x5C + 0x6E);
    fn_800CAA3C(pending + 0x60, cardData + layer * 0x5C + 0x182);
    fn_800CAA3C(pending + 0xBC, cardData + layer * 0x5C + 0x296);
    fn_800CAA3C(pending + 0x118, record);
    pending[0x124] = record->enabled;
    pending[0x125] = cardData[0x6A + layer];
    *(u16*)(pending + 0x126) = record->field12;
    *(u16*)(pending + 0x128) = record->field14;
    *(u16*)(pending + 0x12A) = record->field16;
    *(u16*)(pending + 0x12C) = record->field18;
    *(u32*)(pending + 0x130) = record->field1C;
    *(u16*)(pending + 0x134) = record->field20;
    pending[0x136] = record->field24;
    ((s8*)pending)[0x1E0] = -1;
    pending[0x1E1] = 0;
    pending[0x1E2] = 0;

    wanted = 0;
    for (;;) {
        found = CardEGridGetEntry(arena, wanted);
        if (found == NULL) {
            __assert("cardesavedata.c", 0x121, lbl_8047C180);
        }
        if (found->key == pending[1]) {
            break;
        }
        wanted++;
    }

    layerData = (u8*)CardEGetLevel(found, ((s8*)pending)[2]);

    for (i = 0; i < 4; i++) {
        objectIndex = record->item[i];
        if (objectIndex < 0) {
            *(u16*)(pending + 0x138 + i * 0x2A) = 0;
            continue;
        }

        object = (CardEObjectData*)(cardData + 0x514 +
                                    objectIndex * 0x2A);
        *(CardEObjectData*)(pending + 0x138 + i * 0x2A) = *object;
        if (object->bytes[2] != 0) {
            ((s8*)pending)[0x1E0] = i;
            pending[0x1E2] = object->bytes[0x28];
            pending[0x1E1] = object->bytes[2];
            *(u16*)(layerData + 0x74) = *(u16*)object;
            fn_801EE1E0(pending[0x1E1], *(u16*)object);
        }
    }

    fn_800CAA3C(layerData + 0x64, pending + 0x118);
    layerData[0x70] = pending[0x125];
    layerData[0x71] = 0;
    layerData[0x72] = pending[0x1E1];
    fn_801EE2B4(pending[0x1E1], pending + 0x118);
    fn_801EE10C(pending[0x1E1], pending[0x125]);
}



/* Return one well-formed record, or the terminating slot for a negative index. */
#pragma push
void* fn_80083AF4(void* arena, s32 index)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    CardEGridEntry* result;
    u8* end;
    s32 currentIndex;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    CardEGridSetEntry(&result, NULL);
    currentIndex = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        if (currentIndex == index) {
            result = entry;
        }
        currentIndex++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    if (index < 0) {
        result = entry;
    }
    return result;
}

static inline void CardEGridSetCount(s32* countOut, s32 count)
{
    if (countOut != NULL) {
        *countOut = count;
    }
}

/* Count well-formed records in the Card-e save-data arena. */
s32 fn_80083BF8(void* arena)
{
    extern void* savedataGetStatus(u32, u32);
    CardEGridEntry* entry;
    u8* end;
    s32 count;
    s32 currentCount;

    if (arena != NULL) {
        entry = arena;
    } else {
        entry = savedataGetStatus(0, 0xD);
    }
    end = (u8*)entry + 0x4000;
    currentCount = 0;
    while (1) {
        if (end < (u8*)entry + 0x24 || entry->id == 0) {
            break;
        }
        if (entry->layers > 3 || entry->rows > 6 || entry->columns > 5) {
            entry->id = 0;
            break;
        }
        currentCount++;
        entry = (CardEGridEntry*)((u8*)entry + CardEGridEntrySize(entry));
    }
    CardEGridSetCount(&count, currentCount);
    return count;
}
#pragma pop


/* 0x8007FDBC | size: 0x554 */
#pragma push
#pragma optimization_level 3
/* Both allocations assert on the same source line, so they share a helper. */
static inline void* menuCardE_Alloc(u32 size, char* pool) {
    void* buf;
    u16 handle;

    handle = fn_800E2C04((size + 0x1F) & ~0x1F, 0x20);
    if (handle == 0) {
        __assert(pool + 0x1F0, 0x1A2, lbl_8047C140);
    }
    buf = fn_800E27B0(handle);
    memset(buf, 0, size);
    return buf;
}

static inline void menuCardE_Free(void* buf, char* pool) {
    u16 handle;

    handle = fn_800E202C(buf);
    if (handle == 0) {
        __assert(pool + 0x1F0, 0x1AB, lbl_8047C140);
    }
    fn_800E24B0(handle);
    fn_800E209C(handle);
}

void* fn_8007FDBC(void* window, const void* title) {
    char* table;
    u8* ctx;
    s32 k;
    u8* rowItems;
    u16* rowIds1;
    u16* rowIds2;
    u16* rowIds0;
    u16* ids0;
    u8* rowCtx;
    u16* ids1;
    u16* ids2;
    s32 count;
    s32 i;
    s32 j;

    table = lbl_80268B88;

    ctx = menuCardE_Alloc(0x4E8, table);

    if (title != 0) {
        GScharLenCpy(ctx, title, 0x50);
        *(u16*)(ctx + 0x9E) = 0;
    } else {
        *(u16*)ctx = 0;
    }

    if (CARDE_CTX_U32(ctx, 0xB0) != 0) {
        menuCardE_Free((void*)CARDE_CTX_U32(ctx, 0xB0), table);
        CARDE_CTX_U32(ctx, 0xB0) = 0;
    }

    count = CARDE_CTX_U32(ctx, 0xAC) = fn_80083BF8(0);
    if (count != 0) {
        CARDE_CTX_U32(ctx, 0xB0) = (u32)menuCardE_Alloc(count * 4, table);

        for (k = 0; k < count; k++) {
            ((u32*)CARDE_CTX_U32(ctx, 0xB0))[k] =
                (u32)fn_80083AF4(0, k);
        }
        qsort((void*)CARDE_CTX_U32(ctx, 0xB0), count, 4,
              menuCardE_CompareEntryPtrs);
    }

    CARDE_CTX_U32(ctx, 0xA4) = (s32)CARDE_CTX_U32(ctx, 0xAC) != 0 ? 0 : -1;

    menuCardE_SetItem(ctx, 0x118, window, 0x79B);
    menuCardE_SetItem(ctx, 0x11C, window, 0x79C);
    menuCardE_SetItem(ctx, 0x120, window, 0x79D);
    menuCardE_SetItem(ctx, 0x124, window, 0x780);
    menuCardE_SetItem(ctx, 0x128, window, 0x781);
    menuCardE_SetItem(ctx, 0x12C, window, 0x782);
    menuCardE_SetItem(ctx, 0x130, window, 0x1193);
    menuCardE_SetItem(ctx, 0x134, window, 0x1195);
    menuCardE_SetItem(ctx, 0x138, window, 0x1194);
    menuCardE_SetItem(ctx, 0x13C, window, 0x796);
    menuCardE_SetItem(ctx, 0x140, window, 0x793);
    menuCardE_SetItem(ctx, 0x144, window, 0x797);
    menuCardE_SetItem(ctx, 0x148, window, 0x1196);
    menuCardE_SetItem(ctx, 0x14C, window, 0x792);
    menuCardE_SetItem(ctx, 0x150, window, 0x1126);
    menuCardE_SetItem(ctx, 0x154, window, 0x795);
    menuCardE_SetItem(ctx, 0x158, window, 0x791);
    menuCardE_SetItem(ctx, 0x15C, window, 0x1125);
    menuCardE_SetItem(ctx, 0x160, window, 0x799);
    menuCardE_SetItem(ctx, 0x164, window, 0x79A);
    menuCardE_SetItem(ctx, 0x168, window, 0x825);
    menuCardE_SetItem(ctx, 0x16C, window, 0x826);

    rowCtx = ctx;
    ids0 = (u16*)(table + 0);
    ids1 = (u16*)(table + 0x90);
    ids2 = (u16*)(table + 0x120);
    for (i = 0; i < 0x24; i++) {
        rowIds0 = ids0;
        rowItems = rowCtx;
        rowIds1 = ids1;
        rowIds2 = ids2;

        for (j = 0; j < 2; j++) {
            CARDE_CTX_U32(rowItems, 0x170) =
                (u32)windowSearchItemID(window, *rowIds0);
            CARDE_CTX_U32(rowItems, 0x3B0) =
                (u32)windowSearchItemID(window, *rowIds1);
            CARDE_CTX_U32(rowItems, 0x290) =
                (u32)windowSearchItemID(window, *rowIds2);
            rowIds0 += 0x24;
            rowItems += 0x90;
            rowIds1 += 0x24;
            rowIds2 += 0x24;
        }
        ids0++;
        rowCtx += 4;
        ids1++;
        ids2++;
    }

    menuCardE_SetItem(ctx, 0x4D0, window, 0x119A);
    menuCardE_SetItem(ctx, 0x4D4, window, 0x11C2);
    menuCardE_SetItem(ctx, 0x4D8, window, 0x790);
    menuCardE_SetItem(ctx, 0x4DC, window, 0x798);
    menuCardE_SetItem(ctx, 0x4E0, window, 0x78F);
    menuCardE_SetItem(ctx, 0x4E4, window, 0x794);

    menuCardE_CopyRect(ctx, 0xCE, 0x200);
    menuCardE_CopyRect(ctx, 0xD6, 0x440);
    menuCardE_CopyRect(ctx, 0xDE, 0x320);
    menuCardE_CopyRect(ctx, 0xE6, 0x118);
    menuCardE_CopyRect(ctx, 0xEE, 0x11C);
    menuCardE_CopyRect(ctx, 0xF6, 0x120);
    menuCardE_CopyRect(ctx, 0xFE, 0x15C);
    menuCardE_CopyRect(ctx, 0x106, 0x154);
    menuCardE_CopyRect(ctx, 0x10E, 0x14C);

    return ctx;
}
#pragma pop


/* 0x80084034 | size: 0x4 */
void fn_80084034(void) {
}

/* 0x80083CBC | size: 0x40 */
void fn_80083CBC(void* ptr) {
    memset(ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD), 0, 0x49CC);
}

/* 0x80083CFC | size: 0x34 */
void* fn_80083CFC(void* ptr) {
    return ptr != 0 ? ptr : (void*)savedataGetStatus(0, 0xD);
}

/* Update the four-controller Card-e connection/status display. */
typedef struct CardEStatusWork {
    s32 state[4];
    s32 previousState[4];
    u8 refreshMessages;
    s8 port;
    u8 pad22[2];
    u32 command;
    u32 field28;
    s32 mode;
    void* headerSprite[2];
    void* statusSprite[5];
    void* optionSprite[8][4];
} CardEStatusWork;

typedef struct CardEMessageEntry {
    u16 itemId;
    u16 initialMessage;
    u16 updatedMessage;
} CardEMessageEntry;

extern char lbl_8047C198[] __attribute__((section(".sdata2")));
extern void fn_801081F8(void* window, u16 itemId, u16 messageId);
extern void winSpriteSetDisp(void* sprite, u32 enable);

/* The status display's rodata: per-state sprite masks, then the item
 * messages, then the source file name used by its asserts. */
typedef struct CardEStatusData {
    /* 0x000 */ u32 stateFlags[28];
    /* 0x070 */ CardEMessageEntry messages[46];
    /* 0x184 */ char file[1];
} CardEStatusData;

static inline CardEStatusWork* CardEGetStatusWork(u8* window)
{
    if (window == NULL) {
        window = ((u8* (*)(u32))windowSearchID)(0xA6);
    }
    return *(CardEStatusWork**)((void* (*)(u8*))windowGetFreeWork)(window);
}

static inline void CardEShowStatusSprite(void* sprite, u32 bit)
{
    if (sprite != NULL) {
        winSpriteSetDisp(sprite, bit != 0);
    }
}

static inline void CardESetStatusMessage(u8* window, void* sprite, s32 active)
{
    if (sprite != NULL) {
        if (active) {
            fn_801081F8(window, *(s16*)((u8*)sprite + 6), 0x1BA);
        } else {
            fn_801081F8(window, *(s16*)((u8*)sprite + 6), 0);
            *(s32*)((u8*)sprite + 0x64) = -1;
        }
    }
}

#pragma push
void fn_80084038(u8* window)
{
    const u8* data = lbl_8026F2E8;
    CardEStatusWork* work;
    CardEStatusWork* status;
    void* sprite;
    u16 handle;
    s32 flags;
    u32 n;
    s32 i;
    u8 changed;
    u8 selected;

    status = CardEGetStatusWork(window);
    work = status;
    switch ((s8)window[1]) {
    case 0:
        if ((s8)window[2] != 0) {
            break;
        }
        handle = ((u16 (*)(u32, u32))fn_800E2C04)(0xE0, 0x20);
        if (handle == 0) {
            __assert((const char*)data + 0x184, 0xEA, lbl_8047C198);
        }
        status = ((CardEStatusWork* (*)(u16))fn_800E27B0)(handle);
        memset(status, 0, sizeof(CardEStatusWork));
        work = status;
        *(CardEStatusWork**)(((void* (*)(u8*))windowGetFreeWork)(window)) = status;
        for (i = 0; i < 4; i++) {
            status->previousState[i] = 0;
            status->state[i] = 0;
        }
        status->refreshMessages = 1;
        status->port = 1;
        status->command = 0;
        status->field28 = 0;

#define FIND_STATUS_SPRITE(member, item) \
        status->member = ((void* (*)(u8*, u32))windowSearchItemID)(window, item)
        FIND_STATUS_SPRITE(headerSprite[0], 0x10F6);
        FIND_STATUS_SPRITE(headerSprite[1], 0x10F7);
        FIND_STATUS_SPRITE(statusSprite[0], 0x10D5);
        FIND_STATUS_SPRITE(statusSprite[1], 0x10DA);
        FIND_STATUS_SPRITE(statusSprite[2], 0x10E3);
        FIND_STATUS_SPRITE(statusSprite[3], 0x10F0);
        FIND_STATUS_SPRITE(statusSprite[4], 0x10F5);
        FIND_STATUS_SPRITE(optionSprite[0][0], 0x10D1);
        FIND_STATUS_SPRITE(optionSprite[1][0], 0x10D6);
        FIND_STATUS_SPRITE(optionSprite[2][0], 0x10DB);
        FIND_STATUS_SPRITE(optionSprite[3][0], 0x10DF);
        FIND_STATUS_SPRITE(optionSprite[4][0], 0x10E4);
        FIND_STATUS_SPRITE(optionSprite[5][0], 0x10E8);
        FIND_STATUS_SPRITE(optionSprite[6][0], 0x10EC);
        FIND_STATUS_SPRITE(optionSprite[7][0], 0x10F1);
        FIND_STATUS_SPRITE(optionSprite[0][1], 0x10D2);
        FIND_STATUS_SPRITE(optionSprite[1][1], 0x10D7);
        FIND_STATUS_SPRITE(optionSprite[2][1], 0x10DC);
        FIND_STATUS_SPRITE(optionSprite[3][1], 0x10E0);
        FIND_STATUS_SPRITE(optionSprite[4][1], 0x10E5);
        FIND_STATUS_SPRITE(optionSprite[5][1], 0x10E9);
        FIND_STATUS_SPRITE(optionSprite[6][1], 0x10ED);
        FIND_STATUS_SPRITE(optionSprite[7][1], 0x10F2);
        FIND_STATUS_SPRITE(optionSprite[0][2], 0x10D3);
        FIND_STATUS_SPRITE(optionSprite[1][2], 0x10D8);
        FIND_STATUS_SPRITE(optionSprite[2][2], 0x10DD);
        FIND_STATUS_SPRITE(optionSprite[3][2], 0x10E1);
        FIND_STATUS_SPRITE(optionSprite[4][2], 0x10E6);
        FIND_STATUS_SPRITE(optionSprite[5][2], 0x10EA);
        FIND_STATUS_SPRITE(optionSprite[6][2], 0x10EE);
        FIND_STATUS_SPRITE(optionSprite[7][2], 0x10F3);
        FIND_STATUS_SPRITE(optionSprite[0][3], 0x10D4);
        FIND_STATUS_SPRITE(optionSprite[1][3], 0x10D9);
        FIND_STATUS_SPRITE(optionSprite[2][3], 0x10DE);
        FIND_STATUS_SPRITE(optionSprite[3][3], 0x10E2);
        FIND_STATUS_SPRITE(optionSprite[4][3], 0x10E7);
        FIND_STATUS_SPRITE(optionSprite[5][3], 0x10EB);
        FIND_STATUS_SPRITE(optionSprite[6][3], 0x10EF);
        FIND_STATUS_SPRITE(optionSprite[7][3], 0x10F4);
#undef FIND_STATUS_SPRITE

        for (n = 0; n < 46; n++) {
            fn_801081F8(window, ((const CardEMessageEntry*)(data + 0x70))[n].itemId,
                        ((const CardEMessageEntry*)(data + 0x70))[n].initialMessage);
        }
        break;
    case 3:
        if ((s8)window[2] != 0) {
            break;
        }
        for (n = 0; n < 46; n++) {
            fn_801081F8(window, ((const CardEMessageEntry*)(data + 0x70))[n].itemId,
                        ((const CardEMessageEntry*)(data + 0x70))[n].updatedMessage);
        }
        window[2] = 1;
        status->refreshMessages = 1;
        break;
    case 5:
        handle = ((u16 (*)(void*))fn_800E202C)(status);
        if (handle == 0) {
            __assert((const char*)data + 0x184, 0xF3, lbl_8047C198);
        }
        fn_800E24B0(handle);
        fn_800E209C(handle);
        return;
    }

    changed = 0;
    if ((s8)window[1] == 2 && work->refreshMessages != 0) {
        work->refreshMessages = 0;
        changed = 1;
    }

    for (i = 0; i <= 3; i++) {
        if ((work->state[i] == 5 || work->state[i] == 4) &&
            !((u8 (*)(s32))fn_8008ABA0)(i + 1)) {
            ((void (*)(u8))menuSetEnablePort)(
                ((u32 (*)(void))menuGetEnablePort)() & ~lbl_80478950[i]);
            work->state[i] = 7;
            work->field28 = 8;
        }
    }

    for (i = 0; i < 4; i++) {
        if (work->previousState[i] != work->state[i]) {
            changed = 1;
        }
        work->previousState[i] = work->state[i];
    }
    if (!changed) {
        return;
    }

    if (work->headerSprite[0] != NULL) {
        winSpriteSetDisp(work->headerSprite[0], 1);
    }
    if (work->headerSprite[1] != NULL) {
        winSpriteSetDisp(work->headerSprite[1], 1);
    }

    flags = ((const u32*)(data + 0))[work->state[0]];
    CardEShowStatusSprite(work->statusSprite[0], flags & 0x100);
    CardEShowStatusSprite(work->statusSprite[1], flags & 0x200);
    CardEShowStatusSprite(work->statusSprite[2], flags & 0x400);
    CardEShowStatusSprite(work->statusSprite[3], flags & 0x800);
    CardEShowStatusSprite(work->statusSprite[4], flags & 0x1000);

    if (work->refreshMessages == 0) {
        /* i is left at 4 by the loop above. */
        selected = work->state[i] == 9;
        CardESetStatusMessage(window, work->statusSprite[0], selected);
        CardESetStatusMessage(window, work->statusSprite[2], selected);
    }

    for (i = 0; i <= 3; i++) {
        flags = ((const u32*)(data + 0))[work->state[i]];
        CardEShowStatusSprite(work->optionSprite[0][i], flags & 0x01);
        CardEShowStatusSprite(work->optionSprite[1][i], flags & 0x02);
        CardEShowStatusSprite(work->optionSprite[2][i], flags & 0x04);
        CardEShowStatusSprite(work->optionSprite[3][i], flags & 0x08);
        CardEShowStatusSprite(work->optionSprite[4][i], flags & 0x10);
        CardEShowStatusSprite(work->optionSprite[5][i], flags & 0x20);
        CardEShowStatusSprite(work->optionSprite[6][i], flags & 0x40);
        CardEShowStatusSprite(work->optionSprite[7][i], flags & 0x80);

        switch (work->state[i]) {
        case 6:
        case 7:
        case 11:
            if (work->headerSprite[0] != NULL) {
                winSpriteSetDisp(work->headerSprite[0], 0);
            }
            if (work->headerSprite[1] != NULL) {
                winSpriteSetDisp(work->headerSprite[1], 0);
            }
            break;
        }

        if (work->refreshMessages == 0) {
            selected = work->state[i] == 2;
            CardESetStatusMessage(window, work->optionSprite[0][i], selected);
            CardESetStatusMessage(window, work->optionSprite[3][i], selected);
            CardESetStatusMessage(window, work->optionSprite[4][i], selected);

            if ((flags & 4) != 0) {
                sprite = work->optionSprite[2][i];
                if (*(u32*)((u8*)sprite + 0x0C) == 0) {
                    fn_801081F8(window, *(s16*)((u8*)sprite + 6),
                                lbl_8047C190[i]);
                }
            }
        }
    }
}

#pragma pop

#pragma push
/* Run the Card-e transfer UI while temporarily reserving controller port 1. */
s32 fn_800849B4(s32 mode, s32 command, void* input, void* output)
{
    extern void fn_80093698(s32);
    extern u8 fn_80084A8C(s32, u32, void*, void*);
    extern void menuCloseCustom(s32, s32, s32);
    extern u8 menuIsCheck(s32);
    extern u8 menuSetEnablePort(u8);
    extern void winMsgClose(s32);
    u8 previousPort;
    u8 succeeded;
    s32 port;

    previousPort = menuSetEnablePort(1);
    succeeded = fn_80084A8C(mode, command, input, output);
    winMsgClose(0);
    if (menuIsCheck(0xE4) != 0) {
        menuCloseCustom(0xE4, 0, 1);
    }
    menuSetEnablePort(previousPort);
    for (port = 0; port < 3; port++) {
        fn_80093698(port);
    }

    if (succeeded != 0) {
        return 0;
    }
    return -1;
}
#pragma pop

/* Typed views of the unprototyped externs used by the transfer flow. */
#define CardE_threadSwitch() ((void (*)(void))_threadSwitch)()
#define CardE_menuIsCheck(id) ((u8 (*)(s32))menuIsCheck)(id)
#define CardE_menuGetEnablePort() ((u32 (*)(void))menuGetEnablePort)()
#define CardE_menuSetEnablePort(mask) ((u8 (*)(u8))menuSetEnablePort)(mask)
#define CardE_portConnected(port) ((u8 (*)(s32))fn_8008ABA0)(port)
#define CardE_portBusy(port) ((s32 (*)(s32))fn_800934E4)(port)
#define CardE_portResult(port) ((s32 (*)(s32))fn_80093610)(port)
#define CardE_portClose(port) ((void (*)(s32))fn_80093698)(port)
#define CardE_msgOpen(a, id, b, c) ((void (*)(s32, s32, s32, s32))winMsgOpen)(a, id, b, c)

extern void msgctrlSetValue(s32 id, s32 value);
extern void fn_80166A28(s32 se);
extern s32 fn_80087AE8();
#define CardE_step(work, flags) \
    ((u8 (*)(CardEStatusWork*, s32))fn_80087AE8)(work, flags)
extern void* fn_80128E04(void);
extern u32 fn_80128E24(void);
extern u8 gamedataAttestBiosGetLangareaId(void*);
extern u32 gamedataBiosGetGamedataAtttestPtr(void*);

/* Scan the four ports and drop any that lost their GBA; true if one is lost. */
static inline u8 CardEScanPorts(CardEStatusWork* work)
{
    s32 i;
    u8 lost = 0;

    for (i = 0; i <= 3; i++) {
        if (work->state[i] == 5 || work->state[i] == 4) {
            if (!CardE_portConnected(i + 1)) {
                CardE_menuSetEnablePort(CardE_menuGetEnablePort() &
                                        ~lbl_80478950[i]);
                work->state[i] = 7;
                work->field28 = 8;
            }
        }
        if (work->state[i] == 7) {
            lost = 1;
        }
    }
    return lost;
}

/* Wait briefly for a disconnect and report it; false if none was found. */
static inline u8 CardEReportDisconnect(CardEStatusWork* work)
{
    s32 n;

    for (n = 0; n < 15; n++) {
        if (CardEScanPorts(work)) {
            break;
        }
        CardE_threadSwitch();
    }
    fn_80166A28(0x26);
    for (n = 0; n < 4; n++) {
        if (work->state[n] == 7) {
            break;
        }
    }
    if (n > 3 && (work->field28 & 8) == 0) {
        return 0;
    }
    CardE_menuSetEnablePort(1);
    msgctrlSetValue(0x2F, n + 1);
    if (n == 0) {
        CardE_msgOpen(7, 0x44C0, 1, 0);
    } else {
        CardE_msgOpen(7, 0x44B8, 1, 0);
    }
    work->field28 = 8;
    return 1;
}

/* Give up on the transfer. */
static inline void CardEAbort(CardEStatusWork* work)
{
    CardE_menuSetEnablePort(1);
    if (!CardEReportDisconnect(work)) {
        work->state[work->port] = 6;
    }
    if (work->mode == 3) {
        CardE_msgOpen(7, 0x44E7, 1, 0);
    } else {
        CardE_msgOpen(7, 0x44E6, 1, 0);
    }
    CardE_portClose(work->port);
}

/* Show an error for the current port; true if the transfer should retry. */
static inline u8 CardERetry(CardEStatusWork* work, s32 message)
{
    if (CardEReportDisconnect(work)) {
        if (work->command & 8) {
            return 1;
        }
        return 0;
    }
    work->state[work->port] = 6;
    msgctrlSetValue(0x2F, work->port + 1);
    CardE_msgOpen(7, message, 0, 0);
    if ((work->command & 8) == 0) {
        CardE_step(work, 1);
    } else if (CardE_step(work, 7)) {
        return 1;
    }
    CardE_portClose(work->port);
    return 0;
}

/* Wait for the current port's transfer step; 0xE means cancelled. */
static inline s32 CardEWaitResult(CardEStatusWork* work)
{
    work->field28 = 0;
    while (!CardE_portBusy(work->port)) {
        if (CardE_menuIsCheck(0x10C)) {
            CardE_threadSwitch();
        } else if (((CardEPadState* (*)(void))windowGetKeyInfo)()->trigger &
                   0x20) {
            work->field28 = 2;
            return 0xE;
        } else if (work->field28 == 8) {
            return 0xE;
        } else {
            CardE_threadSwitch();
        }
    }
    work->field28 = 0;
    return CardE_portResult(work->port);
}

/* Spin for one second of game time. */
static inline void CardEWaitSecond(void)
{
    f32 t = 0.0f;

    while (t < 1.0f) {
        CardE_threadSwitch();
        t += (f32)((u32 (*)(void))fn_800D3088)() /
             (f32)((s32 (*)(void))fn_800D37CC)();
    }
}

static inline u8 CardEIsLangArea(u8 lang)
{
    void* bios;
    u32 attest;

    if (fn_80128E24() != 0 && (bios = fn_80128E04()) != NULL &&
        (attest = gamedataBiosGetGamedataAtttestPtr(bios)) != 0 &&
        gamedataAttestBiosGetLangareaId((void*)attest) == lang) {
        return 1;
    }
    return 0;
}

static inline u8 CardECheckRegion(u32 region)
{
    u8 lang;

    switch (region) {
    case 1:
        lang = 1;
        break;
    case 2:
        lang = 2;
        break;
    case 3:
        lang = 4;
        break;
    case 4:
        lang = 5;
        break;
    case 5:
        lang = 3;
        break;
    case 7:
        lang = 6;
        break;
    default:
        return 0;
    }
    return CardEIsLangArea(lang);
}

/* Open the transfer status window and seed each port's state. */
static inline CardEStatusWork* CardEOpenStatus(const u8* data, s32 mode,
                                               s32 command, s32 done)
{
    void* window;
    CardEStatusWork* work;
    const s32* initial;
    s32 i;
    s8 port;

    ((void (*)(s32, s32))menuOpen)(0xE4, 0);
    window = ((void* (*)(s32))windowSearchID)(0xE4);
    if (window == NULL) {
        __assert(data + 0x184, 0x1F4, lbl_8047C1A0);
    }
    work = CardEGetStatusWork(window);
    work->command = command;
    initial = (const s32*)(data + 0x30) + mode * 4;
    work->mode = mode;
    work->port = lbl_80478954[done];
    for (i = 0; i < 4; i++) {
        port = lbl_80478954[i];
        work->state[port] = initial[port];
        if (i < done && work->state[port] == 1) {
            work->state[port] = 5;
        }
    }
    return work;
}

/* 0x80084A8C | size: 0x305C
 * Run the Card-e GBA transfer for each port in turn. */
u8 fn_80084A8C(s32 mode, u32 command, void* input, void* output)
{
    extern void* savedataGetStatus(u32, u32);
    extern void heroInit(void* hero);
    extern void heroBiosCopy(void* hero, void* status);
    u8 outBuffer[0xD8];
    u8 heroBuffer[0xB1C];
    u32 flags;
    const u8* data = lbl_8026F2E8;
    void* status;
    void* hero;
    CardEStatusWork* work;
    u32 k;
    s32 result;
    s32 n;
    s8 port;
    u8 ok;
    u8 linked = 0;
    void* out;

    CardE_portClose(1);
    while (!((s32 (*)(s32, const u8*, void*))fn_800932F0)(1, data + 0x190,
                                                          NULL)) {
        CardE_threadSwitch();
    }
    work = CardEOpenStatus(data, mode, command, 0);

    if ((command & 0x10) && (mode == 0 || mode == 2)) {
        status = savedataGetStatus(0, 2);
        work->port = 0;
        work->state[0] = 8;
        if (command & 2) {
            msgctrlSetValue(0x2F, 1);
            CardE_msgOpen(7, 0x3D88, 0, 0);
            CardEWaitSecond();
            if (!((u8 (*)(void*))fn_800776E4)(status)) {
                ((void (*)(s32, s32, s32))menuCloseCustom)(0xE4, 0, 1);
                ((void (*)(void*, s32))fn_8005CF2C)(status, 0);
                return 0;
            }
        }
        if (input != NULL && ((void**)input)[0] != NULL) {
            heroBiosCopy(((void**)input)[0], status);
        }
        work->state[0] = 10;
    }

    for (k = 0; k < 4; k++) {
        port = lbl_80478954[k];
        work->port = port;
        if (input != NULL && ((void**)input)[(s8)port] != NULL) {
            hero = ((void**)input)[(s8)port];
        } else {
            hero = heroBuffer;
        }
        if (output != NULL && (s8)port == 1) {
            out = output;
        } else {
            out = outBuffer;
        }

    retry:
        CardE_menuSetEnablePort(CardE_menuGetEnablePort() &
                                ~lbl_80478950[(s8)port]);
        if (work->field28 != 4) {
            work->state[(s8)port] = 2;
            msgctrlSetValue(0x2F, (s8)port + 1);
            CardE_msgOpen(7, 0x3C42, 0, 0);
            if (!CardE_step(work, 6)) {
                CardEAbort(work);
                return 0;
            }
        }
        msgctrlSetValue(0x2F, (s8)port + 1);
        CardE_msgOpen(7, 0x3C43, 0, 0);
        work->state[(s8)port] = 3;
        if (!linked) {
            if (CardEWaitResult(work) == 0xE) {
                CardEAbort(work);
                return 0;
            }
            linked = 1;
        }

        ((void (*)(s32, s32))fn_80093160)((s8)port, 0);
        switch (CardEWaitResult(work)) {
        case 2:
            break;
        case 0xE:
            CardEAbort(work);
            return 0;
        case 0x20002:
        default:
            if (!CardERetry(work, 0x3C47)) {
                return 0;
            }
            goto retry;
        }

        work->state[(s8)port] = 4;
        for (n = 0; n < 300; n++) {
            if (CardE_portConnected((s8)port + 1)) {
                CardE_menuSetEnablePort(CardE_menuGetEnablePort() |
                                        lbl_80478950[(s8)port]);
                break;
            }
            CardE_threadSwitch();
        }
        if ((command & 0x40) == 0) {
            msgctrlSetValue(0x2F, (s8)port + 1);
            CardE_msgOpen(7, 0x3C4D, 0, 0);
            CardEWaitSecond();
        }
        heroInit(hero);
        flags = 0;
        ((void (*)(s32, void*, u32*))fn_80092FC8)((s8)port, hero, &flags);
        result = CardEWaitResult(work);
        switch (result) {
        case 0xE:
            CardEAbort(work);
            return 0;
        }
        if ((flags >> 8) & 3) {
            if (!CardERetry(work, 0x3C49)) {
                return 0;
            }
            goto retry;
        }
        if (!CardECheckRegion((flags >> 4) & 0xF)) {
            if (!CardERetry(work, 0x44F0)) {
                return 0;
            }
            goto retry;
        }
        switch (result) {
        case 4:
            if (flags & 2) {
                break;
            }
        default:
            if (!CardERetry(work, 0x3C49)) {
                return 0;
            }
            goto retry;
        }
        if (command & 1) {
            ok = 1;
            if (flags & 4) {
                if ((flags & 8) == 0) {
                    ok = 0;
                }
            } else if ((flags & 1) == 0) {
                ok = 0;
            }
            if (!ok) {
                if (!CardERetry(work, 0x44C3)) {
                    return 0;
                }
                goto retry;
            }
        }
        if (command & 2) {
            if ((flags & 1) == 0) {
                if (!CardERetry(work, 0x44C3)) {
                    return 0;
                }
                goto retry;
            }
            if (!((u8 (*)(void*))fn_800776E4)(hero)) {
                ((void (*)(s32, s32, s32))menuCloseCustom)(0xE4, 0, 1);
                ((void (*)(void*, s32))fn_8005CF2C)(hero, 1);
                if ((command & 8) == 0) {
                    return 0;
                }
                work = CardEOpenStatus(data, mode, command, k);
                work->state[(s8)port] = 6;
                goto retry;
            }
        }
        if ((output != NULL && (s8)port == 1) || (command & 0x20)) {
            ((void (*)(s32, void*))fn_80092E38)((s8)port, out);
            switch (CardEWaitResult(work)) {
            case 0xB:
                break;
            case 0xE:
                CardEAbort(work);
                return 0;
            default:
                if (!CardERetry(work, 0x3C47)) {
                    return 0;
                }
                goto retry;
            }
        }
        if ((command & 0x20) && (((s32*)out)[2] & 0x10) == 0) {
            if (!CardERetry(work, 0x4417)) {
                msgctrlSetValue(0x2F, 0);
                CardE_msgOpen(7, 0x44CF, 0, 0);
                CardE_step(work, 1);
                return 0;
            }
            goto retry;
        }

        CardE_portClose((s8)port);
        fn_80166A28(0x3CC);
        work->state[(s8)port] = 5;
        CardE_menuSetEnablePort(CardE_menuGetEnablePort() |
                                lbl_80478950[(s8)port]);
        msgctrlSetValue(0x2F, (s8)port + 1);
        CardE_msgOpen(7, 0x3C4B, 0, 0);
        switch (mode) {
        case 0:
            if (CardE_step(work, 3)) {
                return 1;
            }
            CardEAbort(work);
            return 0;
        case 1:
            if ((s8)port == 2) {
                if (CardE_step(work, 3)) {
                    return 1;
                }
                CardEAbort(work);
                return 0;
            }
            break;
        case 2:
            if ((s8)port == 3) {
                if (CardE_step(work, 3)) {
                    return 1;
                }
                CardEAbort(work);
                return 0;
            }
            break;
        case 3:
            if ((s8)port == 0) {
                if (CardE_step(work, 3)) {
                    return 1;
                }
                CardEAbort(work);
                return 0;
            }
            break;
        }
        work->port = lbl_80478954[k + 1];
        if (!CardE_step(work, 7)) {
            CardEAbort(work);
            return 0;
        }
    }
}

#define CARDE_GRID_TABLE ((CardEGridTable*)lbl_8026F488)
#define CARDE_SHOW_MODEL(model_id, anim_id)                                      \
    do {                                                                         \
        void* model_;                                                            \
        model_ = fn_800F92D4((model_id));                                        \
        if (model_ != 0) {                                                       \
            fn_800ECCA8(model_, (anim_id));                                      \
            fn_800ECA78(model_, lbl_8047C1CC);                                   \
            fn_800EC9DC(model_, lbl_8047C1C8);                                   \
            fn_800EC990(model_);                                                 \
        }                                                                        \
    } while (0)

typedef struct CardGridKeyInfo {
    u8 _00[4];
    u16 buttons;
    u16 repeat;
} CardGridKeyInfo;

typedef struct CardGridCell {
    u32 modelId;
    s16 selectAnim;
    s16 resetAnim;
} CardGridCell;

typedef struct CardGridTiles {
    u16 id[3][3];
} CardGridTiles;

typedef struct CardGridCells {
    CardGridCell cell[3][3];
} CardGridCells;

typedef struct CardGridCursor {
    s16 anim[3][3];
} CardGridCursor;

typedef struct CardGridData {
    CardGridTiles tiles;
    u16 _12;
    CardGridCells cells;
    CardGridCursor cursor;
    u16 _6E;
} CardGridData;

typedef struct CardGridChoice {
    s32 x;
    s32 y;
} CardGridChoice;

extern const u32 lbl_8047C1C0;
extern const u32 lbl_8047C1C4;
extern CardGridKeyInfo* windowGetPortKeyInfo(u32 port);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelStartAnimation(void* model);
extern void GSmodelSetAnimType(void* model, u32 type);
extern u8 GSmodelIsAnimating(void* model);

static inline void cardGridAnimate(void* model, s16 anim)
{
    if (model != 0) {
        GSmodelSetAnimIndex(model, anim);
        GSmodelSetAnimFrame(model, lbl_8047C1CC);
        GSmodelSetAnimRate(model, lbl_8047C1C8);
        GSmodelStartAnimation(model);
    }
}

static inline void cardGridSelectCell(CardGridCells cells, s32 x, s32 y)
{
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
        s32 anim = cells.cell[y][x].selectAnim;
        void* model = fn_800F92D4(cells.cell[y][x].modelId);
        if (model != 0) {
            cardGridAnimate(model, anim);
        }
    }
}

static inline void cardGridResetCell(CardGridCells cells, s32 x, s32 y)
{
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
        s32 anim = cells.cell[y][x].resetAnim;
        void* model = fn_800F92D4(cells.cell[y][x].modelId);
        if (model != 0) {
            cardGridAnimate(model, anim);
        }
    }
}

static inline void cardGridResetAll(const CardGridData* data,
                                    s32 occupied[3][3])
{
    s32 y;
    s32 x;

    for (y = 0; y < 3; y++) {
        for (x = 0; x < 3; x++) {
            if (occupied[y][x] != 0) {
                cardGridResetCell(data->cells, x, y);
                occupied[y][x] = 0;
            }
        }
    }
}

static inline void cardGridShowCount(s32 count)
{
    s16 anims[4];

    ((u32*)anims)[0] = lbl_8047C1C0;
    ((u32*)anims)[1] = lbl_8047C1C4;
    if (count >= 0 && count < 4) {
        s32 anim = anims[count];
        void* model = fn_800F92D4(0x107E100B);
        if (model != 0) {
            cardGridAnimate(model, anim);
        }
    }
}

static inline u16 cardGridTileAt(CardGridTiles tiles, s32 x, s32 y)
{
    if (x < 0 || x >= 3 || y < 0 || y >= 3) {
        return 0;
    }
    return tiles.id[y][x];
}

static inline u16 cardGridTile(CardGridTiles tiles, s32 x, s32 y)
{
    return cardGridTileAt(tiles, x, y);
}

static inline void cardGridCountRow(CardGridTiles tiles, s32* occupied,
                                    s32 y, const u16* expected, s32* matches)
{
    s32 x;

    for (x = 0; x < 3; x++) {
        if (occupied[x] != 0) {
            u16 tile = cardGridTile(tiles, x, y);
            if (tile == expected[0]) {
                (*matches)++;
            } else if (tile == expected[1]) {
                (*matches)++;
            } else if (tile == expected[2]) {
                (*matches)++;
            }
        }
    }
}

static inline s32 cardGridCountMatches(CardGridTiles tiles,
                                       s32 occupied[3][3],
                                       const u16* expected)
{
    s32 matches = 0;
    s32 y;

    for (y = 0; y < 3; y++) {
        cardGridCountRow(tiles, occupied[y], y, expected, &matches);
    }
    return matches;
}

static inline void cardGridShowCursor(CardGridCursor cursor, s32 x, s32 y)
{
    if (x >= 0 && x < 3 && y >= 0 && y < 3) {
        s32 anim = cursor.anim[y][x];
        void* model = fn_800F92D4(0x107E1009);
        if (model != 0) {
            cardGridAnimate(model, anim);
            GSmodelSetAnimType(model, 0);
            while (GSmodelIsAnimating(model) != 0) {
                _threadSwitch();
            }
        }
    }
}

static inline BOOL cardGridSelect(const CardGridData* data, s32 occupied[3][3],
                                  s32 x, s32 y)
{
    if (x < 0 || x >= 3 || y < 0 || y >= 3) {
        return FALSE;
    }
    if (occupied[y][x] != 0) {
        return FALSE;
    }
    cardGridSelectCell(data->cells, x, y);
    occupied[y][x] = 1;
    return TRUE;
}

/* Run the card-e three-tile grid prompt.  A zero return means that three
 * cells were accepted; one means the player backed out before completing it. */
u32 fn_80087C64(const u16* expected)
{
    s32 occupied[3][3];
    CardGridChoice choices[3];
    const CardGridData* data = (const CardGridData*)lbl_8026F488;
    s32 x;
    s32 y;
    s32 count;
    s32 i;
    s32 j;

    windowGetPortKeyInfo(1);
    x = 1;
    y = 1;
    count = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            occupied[i][j] = 0;
        }
    }

    while (count < 3) {
        CardGridKeyInfo* key = windowGetPortKeyInfo(1);
        BOOL handled = FALSE;

        if (key->repeat & 0x10) {
            if (cardGridSelect(data, occupied, x, y)) {
                choices[count].x = x;
                choices[count].y = y;
                count++;
                cardGridShowCount(count);

                if (count >= 3) {
                    if (cardGridCountMatches(data->tiles, occupied, expected) < 3) {
                        fn_80166A28(0x26);
                        while (fn_801666BC(0x26) == 2) {
                            _threadSwitch();
                        }
                        cardGridResetAll(data, occupied);
                        count = 0;
                        cardGridShowCount(count);
                    } else {
                        fn_80166A28(0x4A1);
                        while (fn_801666BC(0x4A1) == 2) {
                            _threadSwitch();
                        }
                    }
                } else {
                    fn_80166A28(0x3C6);
                }
            }
            handled = TRUE;
        }

        if ((key->buttons & 0x20) && !handled) {
            CardGridChoice* choice;

            count--;
            if (count < 0) {
                break;
            }
            fn_80166A28(0x3C7);
            choice = &choices[count];
            cardGridResetCell(data->cells, choice->x, choice->y);
            cardGridShowCount(count);
            occupied[choice->y][choice->x] = 0;
            handled = TRUE;
        }

        if (!handled) {
            u16 repeat = key->repeat;
            s32 nextX = x;
            s32 nextY = y;
            BOOL moved = FALSE;

            if ((repeat & 1) && y > 0) {
                nextY = y - 1;
                moved = TRUE;
            }
            if ((repeat & 2) && nextY < 2) {
                nextY++;
                moved = TRUE;
            }
            if ((repeat & 4) && x > 0) {
                nextX = x - 1;
                moved = TRUE;
            }
            if ((repeat & 8) && nextX < 2) {
                nextX++;
                moved = TRUE;
            }

            if (moved) {
                cardGridShowCursor(data->cursor, nextX, nextY);
                x = nextX;
                y = nextY;
            }
        }
        _threadSwitch();
    }

    return count < 0 ? 1 : 0;
}

#undef CARDE_SHOW_MODEL
#undef CARDE_GRID_TABLE

#endif
