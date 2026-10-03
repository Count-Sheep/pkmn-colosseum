/* Name-entry keyboard cursor, 0x80027D58-0x800280FC.
 * RULE-EXCEPTION(user-approved): unit-scope -opt nopeephole and
 * -opt nostrength (frontend strength reduction off); see
 * docs/RULE_EXCEPTIONS.md. Source mirrors src/game/menuNameEntry.c.
 */
#include "dolphin/types.h"

extern s32 GSmsgGetLength(void*);
extern void* GSmsgGetGSchar(u32);
extern u8 lbl_80266DD8[];
extern const u32 lbl_8047B920[2];

/* Name-entry session block; the menu windows reach it through +0x60. */
typedef struct NAME_ENTRY_ARG {
    u16 buffer[12];
    u16* name;
    s32 kind;
    s32 index;
    s32* row;
    s32* letter;
    s32* column;
    f32* fade;
    s32* state;
    u32* work0;
    u32* work1;
    u32* work2;
    u32* work3;
    u32* work4;
} NAME_ENTRY_ARG;

/* Removes the last letter of the name; FALSE when the name is already empty. */
static inline u8 menuNameEntryDeleteLetter(NAME_ENTRY_ARG* arg)
{
    s32 pos;
    u8 deleted;

    pos = *arg->state;
    if (pos <= 0) {
        deleted = 0;
    } else {
        u16* name = arg->name;
        pos--;
        name += pos;
        *name = 0;
        deleted = 1;
        *arg->state = pos;
    }
    return deleted;
}

u16 exchangeDakuon__FUs11DAKUON_MODE(u16 letter, s32 mode);
s32 selectLetter__FP14NAME_ENTRY_ARG(NAME_ENTRY_ARG* arg);
extern u16* windowGetKeyInfo(void);

static inline s32 menuNameEntryGetDakuonMode(u16 letter)
{
    u32* tables;
    s32 kind;
    s32 count;
    s32 i;
    u16* chars;

    kind = 0;
    do {
        tables = (u32*)lbl_8047B920;
        if (tables[kind] != 0) {
            count = GSmsgGetLength((void*)tables[kind]);
            chars = (u16*)GSmsgGetGSchar(tables[kind]) + 1;
            for (i = 1; i < count; i += 2, chars += 2) {
                if (*chars == letter) {
                    break;
                }
            }
            if (i < count) {
                break;
            }
        }
    } while (++kind < 2);
    if (kind >= 2) {
        kind = 0;
    }
    return kind;
}
s32 menuNameEntryCursor(void* window)
{
    extern void fn_80166A28(u32 se);
    u16* keys;
    u16* letters;
    s32 row;
    u16 converted;
    s32 value;
    u16* name;
    s32* rowp;
    s32 mode;
    s32 length;
    NAME_ENTRY_ARG* arg;
    s32 pos;
    s32 offset;
    s32 kind;

    keys = windowGetKeyInfo();
    arg = *(NAME_ENTRY_ARG**)((u8*)window + 0x60);
    if (keys[2] & 0x40) {
        rowp = arg->row;
        row = *rowp;
        row++;
        if (row >= 2) {
            row = 0;
        }
        *rowp = row;
        fn_80166A28(0x27);
        return 0;
    }
    if (keys[2] & 0x10) {
        if (selectLetter__FP14NAME_ENTRY_ARG(arg)) {
            ((u8*)window)[0x98] = 1;
            return 0;
        }
        letters = arg->name;
        length = 0;
        while (*letters != 0) {
            letters++;
            length++;
        }
        if (length >= ((s32*)(lbl_80266DD8 + 4))[arg->kind * 4]) {
            *arg->letter = 0xf;
            *arg->column = 3;
        }
        return 0;
    }
    if (keys[2] & 0x20) {
        if (menuNameEntryDeleteLetter(arg)) {
            fn_80166A28(0x25);
        }
        return 0;
    }
    if (keys[2] & 0x400) {
        pos = *arg->state - 1;
        if (pos >= 0) {
            name = arg->name;
            offset = pos;
            kind = menuNameEntryGetDakuonMode(name[offset]);
            mode = kind;
            do {
                mode++;
                if (mode >= 2) {
                    mode = 0;
                }
                converted = exchangeDakuon__FUs11DAKUON_MODE(name[offset], mode);
            } while (converted == 0);
            name[offset] = converted;
        }
        fn_80166A28(0x24);
        return 0;
    }
    if (keys[2] & 0x800) {
        ((u8*)window)[0x98] = 1;
        return 0;
    }
    if (keys[3] & 8) {
        value = *arg->letter + 1;
        if (value >= 0x10) {
            value = 0xf;
        } else {
            fn_80166A28(0x23);
        }
        *arg->letter = value;
    }
    if (keys[3] & 4) {
        value = *arg->letter - 1;
        if (value < 0) {
            value = 0;
        } else {
            fn_80166A28(0x23);
        }
        *arg->letter = value;
    }
    if (keys[3] & 2) {
        value = *arg->column;
        if (*arg->letter >= 0xf) {
            if (value == 1) {
                value += 2;
            } else {
                value += 1;
            }
        } else {
            value += 1;
        }
        if (value >= 4) {
            value = 3;
        } else {
            fn_80166A28(0x23);
        }
        *arg->column = value;
    }
    if (keys[3] & 1) {
        value = *arg->column;
        if (*arg->letter >= 0xf) {
            if (value == 2) {
                value -= 2;
            } else {
                value -= 1;
            }
        } else {
            value -= 1;
        }
        if (value < 0) {
            value = 0;
        } else {
            fn_80166A28(0x23);
        }
        *arg->column = value;
    }
    return 0;
}
