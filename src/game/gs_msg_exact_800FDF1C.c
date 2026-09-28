/**
 * @file gs_msg_exact_800FDF1C.c
 * @brief _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO,
 *        0x800FDF1C - 0x800FDFE4.
 *
 * Function-boundary carve of the GSmsg TU (see gs_msg.c): find the glyph
 * record for a character code in the task's font. The task's font id picks
 * the font slot; each of the slot's glyph banks is binary-searched by code,
 * and the bank that holds it is returned through outBank. No jump table, no
 * pooled constant; its only data is the .sdata message-system pointer
 * (lbl_80478B08), kept extern. GC/1.3 -O4,p with the TU's unit-wide
 * -opt nopeephole, no pragmas.
 *
 * The mangled name is the retail symbol (a C++ function taking
 * MSG_TASK_WORK*, unsigned short, tagFONT_INFO**); the TU is built as C and
 * keeps it literally, as the rest of gs_msg.c does.
 */
#include "dolphin/types.h"

struct GlyphEntry {
    u16 code;   /* 0x00 */
    u8 width;   /* 0x02 */
    u8 height;  /* 0x03 */
    u32 offset; /* 0x04 */
};

struct FontBank {
    u16 count;                   /* 0x00: glyphs, sorted by code */
    u8 reserved_02[2];           /* 0x02 */
    u32 dataOffset;              /* 0x04 */
    struct FontBank* next;       /* 0x08 */
    struct FontBank* previous;   /* 0x0C */
    struct GlyphEntry glyphs[1]; /* 0x10 */
};

struct FontSlot {
    u16 id;                /* 0x00 */
    u8 width;              /* 0x02 */
    u8 height;             /* 0x03 */
    struct FontBank* bank; /* 0x04 */
};

/* The message-system record (layout as in gs_msg.c; only the fields used
 * here are named). */
struct MessageSystem {
    u16 taskCount;          /* 0x00 */
    u16 taskHandle;         /* 0x02 */
    u16 fontCount;          /* 0x04 */
    u16 fontHandle;         /* 0x06 */
    u8 reserved_08[0x1C];   /* 0x08 */
    struct FontSlot* fonts; /* 0x24: fontCount slots */
};

extern struct MessageSystem* lbl_80478B08;

u16* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(u8* work, u16 code, void** outBank) {
    struct MessageSystem* head;
    s32 count;
    struct FontSlot* slot;
    s32 index;
    struct FontBank* bank;
    struct GlyphEntry* entries;
    struct GlyphEntry* entry;
    u32 low;
    u32 high;
    u32 mid;

    head = lbl_80478B08;
    count = head->fontCount;
    for (index = 0; index < count; index++) {
        slot = head->fonts + index;
        if (slot->id == *(u16*)(work + 0x20)) break;
    }
    if (index == count) return NULL;

    bank = slot->bank;
    while (bank != NULL) {
        high = bank->count;
        entries = bank->glyphs;
        low = 0;
        while (low < high) {
            mid = (low + high) / 2;
            entry = &entries[mid];
            /* RULE-EXCEPTION(title-path): cast whose only effect is register allocation (the widened code stays in r4) - see docs/RULE_EXCEPTIONS.md */
            if (entry->code == (u32)code) {
                if (outBank != NULL) *outBank = bank;
                return (u16*)entry;
            }
            if (entry->code < code) low = mid + 1;
            else high = mid;
        }
        /* Retail re-tests the search bounds here (a second blt on the
         * loop's CR) and leaves the bank walk if they are still open. */
        if (low < high) break;
        bank = bank->next;
    }
    return NULL;
}
