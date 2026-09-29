/**
 * @file gs_msg.c
 * @brief GSmsg -- GSAPI message/text-box subsystem (font, VM opcodes,
 *        message task scheduling).
 *
 * Address range: 0x800F96E4 - 0x800FE35C (37 functions).
 * XD class: game/pxdvs/GSAPI/GSmsg/GSmsg.cpp
 *
 * Split out of the former monolithic game/gs_thread_hi.c
 * (0x800F8268-0x800FF0A0 per config/GC6E01/splits.txt).
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "game/gs_thread.h"
#include "game/gs_texture.h"

/*
 * The standalone linked units compile only their own text range from this
 * file: GS_MSG_CHARCPY_ONLY (GScharMakeFromSJIS through GScharCpy,
 * 0x800F9D04-0x800F9EE4), GS_MSG_CHARCMP_ONLY (0x800F9EE4),
 * GS_MSG_GETGSCHAR_ONLY (0x800FA280),
 * GS_MSG_GETLENGTH_ONLY (0x800FA314), GS_MSG_OPENCLOSE_ONLY (GSmsgClose
 * through GSmsgSetCtrlFunc, 0x800FC1D0-0x800FC528) and GS_MSG_INIT_ONLY
 * (0x800FC528).
 */
#if defined(GS_MSG_CHARCPY_ONLY) || \
    defined(GS_MSG_CHARCMP_ONLY) || defined(GS_MSG_GETGSCHAR_ONLY) || \
    defined(GS_MSG_GETLENGTH_ONLY) || defined(GS_MSG_OPENCLOSE_ONLY) || \
    defined(GS_MSG_INIT_ONLY)
#define GS_MSG_PARTIAL
#endif

typedef u8 M2C_UNK;
#define M2C_FIELD(base, type, offset) (*(type)((u8*)(base) + (offset)))


/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const void* fmt, ...);          /* OSReport */
extern u16   GSmemAllocRaw(u32 size);                    /* _toolentryAlloc__FUl */
extern void* GSmemGetPtr(u16 handle);                    /* fn_800E27B0 */
extern void* GSmemLock(u16 handle);                      /* fn_800E24B0 */
extern void  GSmemFree(u16 handle);                      /* fn_800E209C */
extern u16   GSmemAlloc(u32 alignment, u32 size);        /* fn_800E2C04 */
extern void  OSSetIdleFunction(void* func, void* arg,
                          void* stackTop, u32 stackSize); /* OSCreateFiber-like */
extern void  OSDisableInterrupts(void);
extern void  OSRestoreInterrupts(void);
extern void  fn_800D30A0(void* callback);                 /* GSgfx register swap callback */
extern void  threadSaveGPRRegisters(void);                /* GSthread context init */
extern void  threadSaveFPRRegisters(void);                           /* GSthread FPU context init */
/* renamed symbols referenced by asm incs (symbolmap port) */
extern void GSscratchFree(void*);
extern f64 cos(f64 angle);

/* ===== String constants (rodata references) ===== */
extern const char lbl_80271008[]; /* "GSthreadCreate. Warning: 'usesFPU==FALE' OK?\n" */

/* ===== Forward declarations for internal functions ===== */
extern void gappVSyncCallback(void);            /* GStaskSwapCallback */
extern void fn_800F0F4C(u32 arg);          /* GSthread trampoline / entry wrapper */
extern void fn_800AB150(void* buf);
extern u32 fn_800D0F44(u32 buttonIdx);
extern void fn_800AB4FC(void*);
extern void fn_800E209C(u16 handle);
extern void* fn_800E24B0(u16 handle);
extern void* fn_800E27B0(u16 handle);
extern u16 fn_800E2C04(u32 alignment, u32 size);
extern u16 _toolentryAlloc__FUl(u32 size);
extern u32 fn_80080ED8(u16* destination, const u8* source);
extern void fn_800DBEB4(u32 index, GXColor color);
extern void fn_800D5CB8(s32 a, s32 b, s32 c, s32 d, s32 e);
extern void fn_800D61E4(s16 x, s16 y);
extern void fn_800D6728(void);
extern void fn_800D67BC(s32 a);
extern void fn_800D6A00(s32 a);
extern void fn_800D7820(void* ptr);
extern void fn_800D85D4(void);
extern void fn_800D888C(u32 mask);
extern void fn_800D88DC(u32 mask);
extern void fn_800D9ED8(void);
extern void fn_800DC1D4(s32 a);
extern void logVsnprintf_float(char* output, u32 capacity, const char* format, void* arguments);
extern void fn_801669BC(u32 type);
extern void fn_800CDBE0(void);
extern u32 fn_800D3088(void);
extern void fn_800DBF78(void);
extern void fn_800DBFD4(void);
extern void fn_800DC04C(void);
extern void fn_800DC0D4(void);
extern void fn_800DC14C(void);
extern void fn_800DC224(void);
extern void windowDrawSprite(s32 x, s32 y, s32 context, u32 key, u32 flags);
extern void fn_80166A28(void);
extern void fn_800D59B8(s32 slot, f32 xScale, f32 yScale);
extern void fn_800D5BA0(s32 slot, u32 color);
extern void fn_800D9D68(u16 a, u16 b, u16 c, u16 d);
extern f64 tan(void);
extern void fn_800D7FE4(void* mtx);
extern void fn_800D834C(void);
extern void fn_800D9BD0(f32 a, f32 b, f32 c, f32 d);
extern void fn_800DA028(s32 a);
extern void fn_800DA100(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void fn_800DA1E8(s32 a, s32 b, s32 c);
extern void fn_800DA2BC(s32 a, s32 b, s32 c);
extern void fn_800DA4C4(s32 a, s32 b, s32 c);
extern void set__5GSvecFfff(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E0218(void* dst, void* a, void* b, void* c);
extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dst, const void* src, u32 n);

/* ===== BSS/SDA symbol externs (for asm{} blocks) ===== */
/* BSS/data/rodata symbols accessed via lis/@ha + addi/@l pairs */
extern u32 lbl_80401C10;
/* .bss symbols */
extern u8  lbl_80401DE0[];
extern u8  lbl_80401E48[];
extern u8  lbl_80402418[];
extern u8  lbl_80402480[];
/* .data symbols */
extern u8  lbl_80314E08[];
extern u8  lbl_80314F98[];
extern u8  lbl_80315678[];
/* .rodata symbols */
extern u8  lbl_80271300[];
extern u8  lbl_80271500[];
extern u8  lbl_80271700[];
extern u8  lbl_80271730[];
extern u8  lbl_80271754[];
extern u8  lbl_8027177C[];
extern u8  lbl_802717B4[];
extern u8  lbl_802717D4[];
/* .sdata symbol */
extern float lbl_80478AC0;
/* sdata2 (r2) float/double constants used in asm blocks */
extern f64 lbl_8047CCC8;  /* f64 */
extern f32 lbl_8047CCD0;  /* f32 */
extern f32 lbl_8047CCD4;  /* f32 */
extern f32 lbl_8047CCD8;  /* f32 */
extern f32 lbl_8047CCDC;  /* f32 */
extern f64 lbl_8047CCE0;  /* f64 */
extern f64 lbl_8047CCE8;  /* f64 */
extern f64 lbl_8047CCF0;  /* f64 */
extern f64 lbl_8047CCF8;  /* f64 */
extern const f32 lbl_8047CD08; /* 1.0f */
extern f64 lbl_8047CD10;  /* f64 */
extern f64 lbl_8047CD18;  /* f64 */
extern f64 lbl_8047CD20;  /* f64 */
extern f64 lbl_8047CD28;  /* f64 */
extern f32 lbl_8047CD30;  /* f32 */
extern f32 lbl_8047CD34;  /* f32 */
extern f32 lbl_8047CD38;  /* f32 */
extern f32 lbl_8047CD3C;  /* f32 */
extern f32 lbl_8047CD40;  /* f32 */
extern f32 lbl_8047CD44;  /* f32 */
extern f32 lbl_8047CD48;  /* f32 */
extern f32 lbl_8047CD4C;  /* f32 */
extern f64 lbl_8047CD50;  /* f64 */
extern f32 lbl_8047CD58;  /* f32 */
extern f32 lbl_8047CD5C;  /* f32 */
extern f32 lbl_8047CD60;  /* f32 */
extern f32 lbl_8047CD64;  /* f32 */
extern f32 lbl_8047CD68;  /* f32 */
extern f32 lbl_8047CD6C;  /* f32 */
extern f32 lbl_8047CD70;  /* f32 */
extern f32 lbl_8047CD74;  /* f32 */
extern f32 lbl_8047CD78;  /* f32 */
/*
 * Message system state. lbl_80478B08 (.sdata) points at the 0x2C-byte
 * record GSmsgInit clears in .bss (lbl_804024E8). Offsets are evidenced by
 * GSmsgInit, the font/task scans, GSmsgDaemon and the renderer.
 */
struct MessageSystem {
    u16 taskCount;                   /* 0x00 */
    u16 taskHandle;                  /* 0x02 */
    u16 fontCount;                   /* 0x04 */
    u16 fontHandle;                  /* 0x06 */
    struct MessageGroup* groups;     /* 0x08 */
    GStextureHandle* textures[2];    /* 0x0C: double-buffered glyph atlas */
    void* image;                     /* 0x14: locked atlas image */
    s16 atlasX;                      /* 0x18: glyph atlas cursor */
    s16 atlasY;                      /* 0x1A */
    u8 unk1C;                        /* 0x1C */
    s8 textureIndex;                 /* 0x1D */
    u8 reserved_1E[2];               /* 0x1E */
    u8* tasks;                       /* 0x20: taskCount 0x68-byte task records */
    struct FontSlot* fonts;          /* 0x24: fontCount slots */
    struct MessageControl* controls; /* 0x28: GSmsgSetCtrlFunc table */
};
/* sbss (r13) symbols -- task and thread system */
extern struct MessageSystem lbl_804024E8;
extern struct MessageSystem* lbl_80478B08; /* = &lbl_804024E8 */
extern u32 lbl_80478B10;
extern u32 lbl_80478B14;
extern u32 lbl_8047AC00;
extern u32 lbl_8047AC04;
extern u32 lbl_8047AC08;
extern u32 lbl_8047AC0C;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC14;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC1C;
extern u32 lbl_8047AC20;
extern u32 lbl_8047AC24;
extern u32 lbl_8047AC28;
extern u32 lbl_8047AC2C;
extern u32 lbl_8047AC30;
extern u32 lbl_8047AC34;
extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;
extern u32 lbl_8047AC44;
extern u32 lbl_8047AC48;
extern u32 lbl_8047AC4C;
extern u32 lbl_8047AC50;
extern u32 lbl_8047AC54;
extern u16 lbl_8047AC58;
extern u32 lbl_8047AC5C;
extern u32 lbl_8047AC60;
extern u32 lbl_8047AC64;
extern s32 lbl_8047AC68;
extern u32 lbl_8047AC6C;
extern u32 lbl_8047AC70;
extern u32 lbl_8047AC72;
extern u32 lbl_8047AC74;
extern u32 lbl_8047AC78;
extern u32 lbl_8047AC7C;
extern u32 lbl_8047AC80;
extern u32 lbl_8047AC84;
extern u32 lbl_8047AC88;
extern u32 lbl_8047AC8C;
extern u32 lbl_8047AC90;
extern u32 lbl_8047AC94;
extern u32 lbl_8047AC98;
extern u32 lbl_8047AC9C;

/* Forward declarations for all asm-wrapped functions in this block */
extern void fn_800F8268(void);
extern void fn_800F8428();
extern void fn_800F8654();
extern void fn_800F8A54();
extern u32 fn_800F92D4(u32 key);
extern void GSresInit(u32 count);
extern u8* fn_800F96E4(u8* destination, s32 capacity, u32 key);
extern u32 fn_800F9AEC(void* outbuf, const u16* src, s32 mode);
extern void GScharMakeFromSJIS(u16* destination, const u8* source);
extern u8* GScharCpy(u8* dst, const u8* src);
extern void GSmsgSetColor(void* obj);
extern s32 GSmsgGetRect();
extern void GSmsgInitRuby();
extern s32 fn_800FAEF8(s32, s32, u32, const char*, ...);
extern s32 fn_800FB43C();
extern s32 fn_800FB680();
extern s32 fn_800FB8C8(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key);
extern s32 fn_800FBB34(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key);
extern void GSmsgDaemon(void);
extern s32 GSmsgExec();
extern u32 fn_800FC2A8(void* ptr);
extern void* GSmsgFontOpen();
extern s32 GSmsgInit(u16 taskCount, u16 fontCount);
extern s32 fn_800FC7E0();
extern void fn_800FD348(u8* work);
extern void fn_800FD69C(u8* work, const u8* pixels, s16 width, s16 height, s16 yOffset);
extern u16* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(u8* work, u16 code, void** outBank);
extern s32 _msgGetLength__FPCUs(const void* str);
extern s32 _msgGetSize__FPCUs(const u16* text);
extern void fn_800FE35C(void);
extern void fn_800FE38C(s32 x1, s32 y1, s32 x2, s32 y2);
extern void spriteSetEnv(void);
extern void fn_800FE6A0(f32 a, f32 b);
extern void fn_800FE6AC(s16* outA, s16* outB);
extern void fn_800FE6D0(s32 a, s32 b);
extern void GSgappUnblock(u32 taskId);
extern void GSgappBlock(u32 taskId);
extern void GSgappTerminate(u32 taskId);
extern void GSgappUpdate(void);
extern u32 GSgappCreate(s32 state, u8 priority, void* param, void* func);
extern void GSgappInit();
extern void gappBackgroundCallback(void);

/* Retail uses eight-byte entries after the 0x10-byte group header. */
struct MessageEntry {
    u32 key;
    u32 offset;
};
struct MessageGroup {
    u16 id;
    u8 reserved_02;
    u8 fontId;
    u16 count;
    u8 reserved_06[2];
    struct MessageGroup* next;
    struct MessageGroup* previous;
    struct MessageEntry entries[1];
};

/* Font files are chains of eight-byte headers, each followed by a glyph bank. */
struct GlyphEntry {
    u16 code;
    u8 width;
    u8 height;
    u32 offset;
};
struct FontBank {
    u16 count;
    u8 reserved_02[2];
    u32 dataOffset;
    struct FontBank* next;
    struct FontBank* previous;
    struct GlyphEntry glyphs[1];
};
struct FontSlot {
    u16 id;
    u8 width;
    u8 height;
    struct FontBank* bank;
};
struct FontFileHeader {
    u16 id;
    u8 width;
    u8 height;
    u32 nextOffset;
};

/* Retail repeats this lookup at 0x800FA280/0x800FA314/0x800FBB34/0x800FBF74.
 * The last two expansions retain the optional group-output pointer check. */
static inline void* GSmsgFindMessage(u32 key, struct MessageGroup** outGroup) {
    struct MessageGroup* node;
    struct MessageEntry* entries;
    u32 val;
    u32 lo;
    u32 hi;
    u32 mid;
    u32 index;
    u32 group;

    if (key == 0) return NULL;

    node = lbl_80478B08->groups;
    group = key >> 20;
    index = key & 0xFFFFF;

    while (node != NULL) {
        if (node->id == group) {
            hi = node->count;
            entries = node->entries;
            lo = 0;
            while (lo < hi) {
                mid = (lo + hi) / 2;
                val = entries[mid].key;
                if (val == index) {
                    if (outGroup != NULL) *outGroup = node;
                    return (u8*)node + entries[mid].offset;
                }
                if (val < index) lo = mid + 1;
                else hi = mid;
            }
        }
        node = node->next;
    }
    return NULL;
}

#if !defined(GS_MSG_PARTIAL)

/* Eight-byte records in msgctrlcode: five high flag bits, then a callback. */
struct MessageControl {
    unsigned int mode : 2;
    unsigned int stop : 1;
    unsigned int execute : 1;
    unsigned int measure : 1;
    unsigned int reserved : 27;
    u32 (*callback)(u8*);
};

/* Both renderer passes expand this dispatch; only the first uses its stop bit.
 * XD keeps it as the dead-stripped _msgCallCtrlFunc__FP13MSG_TASK_WORKUc
 * (NXXJ01.map, GSmsg.o, UNUSED 0x1D8). Mode 2 resolves a message key through
 * the same lookup as GSmsgGetGSchar (retail repeats its zero-key test), and
 * the switch has no default: for mode 3 retail pushes the call stack with
 * whatever `next` last held (see the RULE-EXCEPTION note below). */
static inline u8 GSmsgDispatchControl(u8* work, u32 control) {
    struct MessageControl* table;
    struct MessageControl* entry;
    u8* next;
    u32 enabled;
    u32 result;
    u32 mode;

    table = *(struct MessageControl**)((u8*)lbl_80478B08 + 0x28);
    if (table == NULL) return 0;
    if (work[1] == 0) {
        enabled = table[control].execute;
    } else {
        enabled = table[control].measure;
    }
    if (enabled == 0) return 0;
    entry = &table[control];
    if (entry->callback != NULL) {
        result = entry->callback(work);
        mode = entry->mode;
        if (mode != 0 && result != 0) {
            /* RULE-EXCEPTION(title-path): uninitialized read (mode 3 has no
             * case, so `next` keeps its previous value, as in retail) — see
             * docs/RULE_EXCEPTIONS.md */
            switch (mode) {
            case 1:
                next = (u8*)result;
                break;
            case 2:
                next = GSmsgFindMessage(result, NULL);
                break;
            }
            if (*(s8*)(work + 0x40) >= 3) {
                GSlogWrite((const char*)lbl_80271700, lbl_80315678);
            } else {
                *(u8**)(work + 0x34 + (*(s8*)(work + 0x40))++ * 4) = *(u8**)(work + 0x30);
                *(u8**)(work + 0x30) = next;
            }
        }
    }
    return entry->stop;
}

/* Retail expands this reader in fn_800FC7E0 and _msgGetSize__FPCUs,
 * including a separate zero-code test on the returned halfword. */
static inline u16 GSmsgReadCode(u8* work) {
    u16* cursor;
    u16 code;
    s8 depth;

    for (;;) {
        cursor = *(u16**)(work + 0x30);
        code = *cursor;
        if (code == 0) {
            depth = *(s8*)(work + 0x40);
            if (depth == 0) return code;
            depth--;
            *(s8*)(work + 0x40) = depth;
            *(u16**)(work + 0x30) = *(u16**)(work + 0x34 + depth * 4);
        } else {
            *(u16**)(work + 0x30) = cursor + 1;
            return code;
        }
    }
}

/* 0x800F96E4 | 0x408 */
u8* fn_800F96E4(u8* destination, s32 capacity, u32 key) {
    struct MessageGroup* bank;
    u8* text;
    u8* work;
    u8* dst;
    u8* savedDst;
    u8* parameters;
    u16 code;
    u32 control;
    u32 count = 0;
    u32 savedCount;
    u32 maxBytes;
    s8 savedDepth;

    if (key == 0) return NULL;
    if (destination == NULL || capacity <= 0) return NULL;

    dst = destination;
    maxBytes = ((u32)capacity - 1) * 2;
    text = GSmsgFindMessage(key, &bank);
    if (text == NULL) return NULL;

    work = (u8*)&lbl_80401DE0;
    memset(work, 0, 0x68);
    work[0] = 1;
    *(f32*)(work + 0x60) = 1.0f;
    *(f32*)(work + 0x64) = 1.0f;
    *(s32*)(work + 0x24) = -1;
    *(u8**)(work + 0x28) = text;
    *(u8**)(work + 0x2C) = text;
    *(u8**)(work + 0x30) = text;
    *(u16*)(work + 0x20) = bank->fontId;
    *(u32*)(work + 0x1C) = key;
    work[1] = 1;

    for (;;) {
        savedDst = dst;
        savedCount = count;
        code = GSmsgReadCode(work);
        if (code == 0) break;

        count += 2;
        if (count > maxBytes) break;
        *(u16*)dst = code;
        dst += 2;
        if (code != 0xFFFF) continue;

        parameters = *(u8**)(work + 0x30);
        control = *parameters;
        *(u8**)(work + 0x30) = parameters + 1;
        count++;
        if (count > maxBytes) {
            dst = savedDst;
            break;
        }
        *dst++ = (u8)control;

        parameters = *(u8**)(work + 0x30);
        savedDepth = *(s8*)(work + 0x40);
        GSmsgDispatchControl(work, control);
        if (savedDepth == *(s8*)(work + 0x40)) {
            count += (u32)(*(u8**)(work + 0x30) - parameters);
            if (count > maxBytes) {
                dst = savedDst;
                break;
            }
            while (parameters < *(u8**)(work + 0x30)) {
                *dst++ = *parameters++;
            }
        } else {
            dst = savedDst;
            count = savedCount;
        }
    }

    *(u16*)dst = 0;
    return destination;
}

/* 0x800F9AEC | 0x118 */
u32 fn_800F9AEC(void* outbuf, const u16* src, s32 mode) {
    u8* out;
    const u16* table;
    s32 idx;
    u32 count;
    const u16* p;
    u32 ch;

    out = (u8*)outbuf;

    switch (mode) {
    case 1:
        count = 0;
        if (src != NULL) {
            table = (const u16*)lbl_80271300;
            while ((ch = *src) != 0) {
                p = table;
                idx = 0;
                while (ch != *p) {
                    idx++;
                    p++;
                    if (idx >= 0x100) {
                        idx = 0xB7;
                        break;
                    }
                }
                if (out != NULL) {
                    *out++ = (u8)idx;
                }
                count++;
                src++;
            }
        }
        return count;
    case 7:
    case 9:
    default:
        count = 0;
        if (src != NULL) {
            table = (const u16*)lbl_80271500;
            while ((ch = *src) != 0) {
                p = table;
                idx = 0;
                while (ch != *p) {
                    idx++;
                    p++;
                    if (idx >= 0x100) {
                        idx = 0xB7;
                        break;
                    }
                }
                if (out != NULL) {
                    *out++ = (u8)idx;
                }
                count++;
                src++;
            }
        }
        return count;
    }
}

/* 0x800F9C04 | 0x100 */
u32 fn_800F9C04(void* outbuf, const u8* src, u32 count, s32 mode) {
    u16* out;
    u16* table;
    u32 total;

    out = (u16*)outbuf;

    switch (mode) {
    case 1:
        total = 0;
        if (src != NULL) {
            table = (u16*)lbl_80271300;
            while (count != 0 && *src != 0xFF) {
                if (out != NULL) {
                    *out = table[*src++];
                    out++;
                }
                total++;
                count--;
            }
            if (out != NULL) *out = 0;
        }
        return total;
    case 7:
    case 9:
    default:
        total = 0;
        if (src != NULL) {
            table = (u16*)lbl_80271500;
            while (count != 0 && *src != 0xFF) {
                if (out != NULL) {
                    *out = table[*src++];
                    out++;
                }
                total++;
                count--;
            }
            if (out != NULL) *out = 0;
        }
        return total;
    }
}

#endif /* !GS_MSG_PARTIAL */

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_CHARCPY_ONLY)

/* 0x800F9D04 | 0x20 */
void GScharMakeFromSJIS(u16* destination, const u8* source) {
    fn_80080ED8(destination, source);
}

/* 0x800F9D24 | 0x14C */
void* GScharLenCpy(u16* dst, const u16* src, s32 maxlen) {
    s32 length;
    s32 i;

    if (maxlen <= 0) return dst;
    length = ((u32)_msgGetSize__FPCUs(src) + 1) >> 1;
    if (length >= maxlen) length = maxlen - 1;
    memcpy(dst, src, length * sizeof(*dst));
    i = length;
    while (i < maxlen) {
        dst[i++] = 0;
    }
    return dst;
}

/* 0x800F9E70 | 0x74 */
u8* GScharCpy(u8* dst, const u8* src) {
    if (dst == NULL) return NULL;
    if (src == NULL) {
        *(u16*)dst = 0;
    } else {
        memcpy(dst, src, _msgGetSize__FPCUs((const u16*)src));
    }
    return dst;
}

#endif /* !GS_MSG_PARTIAL || GS_MSG_CHARCPY_ONLY */

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_CHARCMP_ONLY)
/* 0x800F9EE4 | 0x180 */
s32 GScharCmp(const u16* str1, const u16* str2) {
    u32 len1;
    u32 len2;
    u32 i;

    len1 = _msgGetLength__FPCUs(str1);
    len2 = _msgGetLength__FPCUs(str2);

    if (len1 == len2) {
        for (i = 0; i < len2; i++) {
            if (str1[i] != str2[i]) {
                if (str1[i] > str2[i]) return 1;
                return -1;
            }
        }
        return 0;
    } else if (len1 > len2) {
        for (i = 0; i < len2; i++) {
            if (str1[i] != str2[i]) {
                if (str1[i] > str2[i]) return 1;
                return -1;
            }
        }
        return 1;
    } else {
        for (i = 0; i < len1; i++) {
            if (str1[i] != str2[i]) {
                if (str1[i] > str2[i]) return 1;
                return -1;
            }
        }
        return -1;
    }
}

#endif

#if !defined(GS_MSG_PARTIAL)
/* 0x800FA064 | 0xFC */
void GSmsgAdjustAlign(u8* o) {
    s16 r5;

    if (*(s16*)(o + 0x18) == 0) return;
    r5 = (s16)((u32)GSmsgGetRect(*(u32*)(o + 0x1C)) >> 16);

    switch (o[0x4A]) {
    case 0:
        *(f32*)(o + 0xC) = *(f32*)(o + 0x4);
        break;
    case 1:
        *(f32*)(o + 0xC) = *(f32*)(o + 0x4) + (f32)((*(s16*)(o + 0x18) - r5) / 2);
        break;
    case 2:
        *(f32*)(o + 0xC) = *(f32*)(o + 0x4) + (f32)*(s16*)(o + 0x18) - (f32)r5;
        break;
    }
}

/* 0x800FA160 | 0x5C */
void GSmsgSetColor(void* obj) {
    GXColor clr = { 255, 255, 255, 255 };
    u32 color;

    color = *(u32*)((u8*)obj + 0x24);
    clr.r = (u8)(color >> 24);
    clr.g = (u8)(color >> 16);
    clr.b = (u8)(color >> 8);
    clr.a = (u8)color;
    fn_800DBEB4(0, clr);
}

/* XD keeps this task-record reset as the dead-stripped
 * _msgInitTask__FP13MSG_TASK_WORKPUc (NXXJ01.map GSmsg.o, UNUSED 0x6C). */
static inline void msgInitTask(u8* work, u8* text) {
    memset(work, 0, 0x68);
    work[0] = 1;
    *(f32*)(work + 0x60) = 1.0f;
    *(f32*)(work + 0x64) = 1.0f;
    *(s32*)(work + 0x24) = -1;
    *(u8**)(work + 0x28) = text;
    *(u8**)(work + 0x2C) = text;
    *(u8**)(work + 0x30) = text;
}

/* Font-slot lookup: copies the slot's glyph width/height into the task work
 * and derives the line height. Retail expands this body in GSmsgGetRect (and
 * the other task initializers) after storing the font id through the global
 * work record, so the id is forwarded from that store rather than reloaded.
 * Pokemon XD's GSmsgSetFontInfo (GXXE01 0x80107200, trevor403/xd-asm
 * b1087f18 code/func_FUN_80107200.s) tests `id == 1 || id == 3` for the
 * six-pixel line height; Colosseum emits the same two-test disjunction
 * (`cmplwi 1; beq; bne`) with both font ids equal to 1. */
static inline void msgSetFontInfo(u8* o) {
    s32 index;
    struct FontSlot* entry;
    u32 val;

    for (index = 0; index < lbl_80478B08->fontCount; index++) {
        entry = &lbl_80478B08->fonts[index];
        if (entry->id == *(u16*)(o + 0x20)) {
            o[0x22] = entry->width;
            o[0x23] = entry->height;
            val = *(u16*)(o + 0x20);
            if (val == 0) {
                *(u8*)(o + 0x42) = 0xB;
            } else if (val == 1 || val == 1) {
                *(u8*)(o + 0x42) = 6;
            } else {
                *(s8*)(o + 0x42) = (s8)(s32)(lbl_8047CD20 * (f64)(u32)o[0x23] + lbl_8047CD18);
            }
            break;
        }
    }
}

/* 0x800FA1BC | 0xC4 */
void GSmsgSetFontInfo(void* obj) {
    u8* o;
    u8* head;
    s32 count;
    s32 index;
    struct FontSlot* entry;
    u32 val;

    o = (u8*)obj;
    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 0x4);
    for (index = 0; index < count; index++) {
        entry = *(struct FontSlot**)(head + 0x24) + index;
        if (entry->id == *(u16*)(o + 0x20)) {
            o[0x22] = entry->width;
            o[0x23] = entry->height;
            val = *(u16*)(o + 0x20);
            if (val == 0) {
                *(u8*)(o + 0x42) = 0xB;
            } else if (val == 1 || val == 1) {
                *(u8*)(o + 0x42) = 6;
            } else {
                *(s8*)(o + 0x42) = (s8)(s32)(lbl_8047CD20 * (f64)(u32)o[0x23] + lbl_8047CD18);
            }
            break;
        }
    }
}

#endif /* !GS_MSG_PARTIAL */

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_GETGSCHAR_ONLY)
/* 0x800FA280 | 0x94 */
void* GSmsgGetGSchar(u32 key) {
    return GSmsgFindMessage(key, NULL);
}
#endif

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_GETLENGTH_ONLY)
/* 0x800FA314 | 0xBC */
s32 GSmsgGetLength(u32 key) {
    if (key == 0) return 0;
    return _msgGetLength__FPCUs(GSmsgFindMessage(key, NULL));
}
#endif

#if !defined(GS_MSG_PARTIAL)
/* 0x800FA3D0 | 0x74 */
static inline u8* GSmsgFindCheck(u8* head, u32 key)
{
    s32 count;
    u8* entry;
    u8 i;

    count = *(u16*)head;
    for (i = 0; i < count;) {
        entry = *(u8**)(head + 0x20) + i++ * 0x68;
        if (entry[0] != 0 && *(u32*)(entry + 0x1C) == key) {
            return entry;
        }
    }
    return NULL;
}

s32 GSmsgIsCheck(u32 key) {
    u8* head;
    u8* entry;

    head = (u8*)lbl_80478B08;
    entry = GSmsgFindCheck(head, key);
    if (entry != NULL && entry[0] == 1) {
        return 1;
    }
    return 0;
}

/* 0x800FA444 | 0x654
 * Measures a message: runs a scratch task over the text in measure mode and
 * returns (width - 1) << 16 | height. Instruction-exact in the canonical
 * report; its object stays unlinked because the int-to-float conversion
 * constants are the TU's shared .sdata2 pool (lbl_8047CD08-lbl_8047CD28),
 * which other, unlinked GSmsg functions reference by name. */
s32 GSmsgGetRect(arg0)
    u32 arg0;
{
    struct MessageGroup *bank;
    u8 *text;
    u8 *work;
    u8 *ip;
    u16 code;
    u32 control;
    s16 maxX = 0;
    s16 maxY = 0;
    u8 lineStart = 0;
    void *fontInfo;

    if (arg0 == 0) {
        return 0;
    }

    text = GSmsgFindMessage(arg0, &bank);
    if (text == NULL) {
        return 0;
    }

    work = (u8 *)&lbl_80401E48;
    memset(work, 0, 0x68);
    lbl_80401E48[0] = 1;
    *(f32 *)(lbl_80401E48 + 0x60) = 1.0f;
    *(f32 *)(lbl_80401E48 + 0x64) = 1.0f;
    *(s32 *)(lbl_80401E48 + 0x24) = -1;
    *(u32 *)(lbl_80401E48 + 0x28) = (u32)text;
    *(u32 *)(lbl_80401E48 + 0x2C) = (u32)text;
    *(u32 *)(lbl_80401E48 + 0x30) = (u32)text;
    *(u16 *)(lbl_80401E48 + 0x20) = bank->fontId;
    *(u32 *)(lbl_80401E48 + 0x1C) = arg0;
    lbl_80401E48[1] = 1;
    msgSetFontInfo(work);

    for (;;) {
        code = GSmsgReadCode(work);
        if (code == 0) break;
        if (code == 0xFFFF) {
            ip = *(u8 **)(work + 0x30);
            *(u8 **)(work + 0x30) = ip + 1;
            control = *ip;

            if (control == 3) {
                *(f32 *)(work + 0x0C) += (f32)work[0x22];
                if ((f32)maxX < *(f32 *)(work + 0x0C)) {
                    maxX = (s16)*(f32 *)(work + 0x0C);
                }
            }

            GSmsgDispatchControl(work, control);

            if (lineStart != 0 && *(f32 *)(work + 0x0C) == *(f32 *)(work + 0x04)) {
                *(f32 *)(work + 0x0C) += (f32)(s32)work[0x22];
            }
        } else {
            if (work[0x4B] == 2) continue;
            if (code == 0x20) {
                *(f32 *)(work + 0x14) = (f32)((work[0x22] / 2) * *(f32 *)(work + 0x60));
            } else {
                fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(work, code, NULL);
                if (fontInfo == NULL) {
                    *(f32 *)(work + 0x14) = (f32)work[0x22] * *(f32 *)(work + 0x60);
                } else {
                    *(f32 *)(work + 0x14) = (f32)((u8 *)fontInfo)[2] * *(f32 *)(work + 0x60);
                }
            }

            if ((s8)work[0x41] == 0) {
                if (code == 0x300C) {
                    lineStart = 1;
                }
                if (code == 0x300D) {
                    lineStart = 0;
                }
            }

            *(f32 *)(work + 0x0C) += *(f32 *)(work + 0x14);
        }
        if ((f32)maxX < *(f32 *)(work + 0x0C)) {
            maxX = (s16)*(f32 *)(work + 0x0C);
        }
        if ((f32)maxY < *(f32 *)(work + 0x10)) {
            maxY = (s16)*(f32 *)(work + 0x10);
        }
    }

    maxY += (f32)work[0x23] * *(f32 *)(work + 0x64) + 1.0f;
    return ((u32)(maxX - 1) << 0x10) | maxY;
}

/* 0x800FAA98 | 0x460 */
void GSmsgInitRuby(arg0)
    u8 *arg0;
{
    u16 *savedIp;
    u8 *ip;
    u16 *resume[3];
    u16 code;
    u32 control;
    u8 savedDepth;
    s8 savedMode;
    u8 savedFlag;
    u8 pass;
    s16 glyphWidth;
    u8 count0;
    u8 count1;
    s16 width0;
    s16 width1;
    s32 i;
    void *fontInfo;

    savedIp = *(u16 **)(arg0 + 0x30);
    savedDepth = arg0[0x40];
    savedMode = *(s8*)(arg0 + 0x45);
    savedFlag = arg0[1];
    for (i = 0; i < 3; i++) {
        resume[i] = *(u16**)(arg0 + 0x34 + i * 4);
    }

    *(u32 *)(arg0 + 0x54) = 0;
    arg0[0x58] = 0;
    arg0[0x59] = 0;
    arg0[1] = 0;
    arg0[0x45] = 1;
    arg0[0x4B] = 1;

    count0 = 0;
    count1 = 0;
    width0 = 0;
    width1 = 0;

    for (pass = 0; pass < 2; pass++) {
        for (;;) {
            code = GSmsgReadCode(arg0);
            if (code == 0) break;
            if (code == 0xFFFF) {
                ip = *(u8 **)(arg0 + 0x30);
                control = *ip;
                *(u8 **)(arg0 + 0x30) = ip + 1;
                GSmsgDispatchControl(arg0, control);

                if (pass == 0) {
                    if (arg0[0x4B] == 2) {
                        *(u32 *)(arg0 + 0x54) = *(u32 *)(arg0 + 0x30);
                        break;
                    }
                } else if (arg0[0x4B] == 0) {
                    break;
                }
                continue;
            }

            fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, code, NULL);
            if (fontInfo != NULL) {
                glyphWidth = ((u8 *)fontInfo)[2];
            } else {
                glyphWidth = arg0[0x22];
            }

            if (pass == 0) {
                count0++;
                width0 = (s16)((f32)width0 + ((f32)glyphWidth * *(f32 *)(arg0 + 0x60) + lbl_8047CD30));
            } else {
                count1++;
                width1 = (s16)((f32)width1 + (lbl_8047CD34 * ((f32)glyphWidth * *(f32 *)(arg0 + 0x60)) + lbl_8047CD30));
            }
        }
    }

    arg0[0x5A] = count0;
    arg0[0x5B] = count1;
    *(f32 *)(arg0 + 0x4C) = *(f32 *)(arg0 + 0x0C) + (f32)((width0 - width1) / 2);
    *(f32 *)(arg0 + 0x5C) = lbl_8047CD34 * ((f32)arg0[0x22] * *(f32 *)(arg0 + 0x60));
    *(f32 *)(arg0 + 0x50) = -((lbl_8047CD38 * (f32)arg0[0x23]) - *(f32 *)(arg0 + 0x10));

    *(u32 *)(arg0 + 0x30) = (u32)savedIp;
    arg0[0x40] = savedDepth;
    for (i = 0; i < 3; i++) {
        *(u16**)(arg0 + 0x34 + i * 4) = resume[i];
    }
    arg0[0x45] = savedMode;
    arg0[1] = savedFlag;
}

/* 0x800FAEF8 | 0x544 */
s32 fn_800FAEF8(s32 x, s32 y, u32 color, const char* fmt, ...) {
    typedef struct GSVaList {
        u32 flags;
        void* inputArgArea;
        void* regSaveArea;
    } GSVaList;

    extern u8 lbl_80401DE0[];
    extern u8 lbl_80314E08[];
    extern u8 lbl_80314F98[];
    extern void fn_800D9ED8(s32 arg);
    extern void fn_800D88DC(s32 arg);
    extern void fn_800D888C(u32 mask);
    extern void fn_800D7820(void* tex);
    extern void fn_800D85D4(s32 index, void* texture);
    extern void fn_800D6A00(s32 mode);
    extern void fn_800D67BC(s32 mode);
    extern void fn_800D61E4(s16 x, s16 y);
    extern void fn_800D5CB8(s32 a, s32 b, s32 c, s32 d, u32 color);
    extern void fn_800D6728(void);
    extern void fn_800DC1D4(s32 arg);

    u8* base;
    u8* work;
    u8* messageHead;
    u8* codeEntry;
    u8* fontInfo;
    u8* glyph;
    u16 code;
    u16 fontId;
    s32 count;
    s32 index;
    u32 glyphInfo;
    u8 glyphWidth;
    void* image;
    void* outNode;
    s16 drawX0;
    s16 drawY0;
    s16 drawX1;
    s16 drawY1;
    f32 advance;
    GSVaList args;

    __builtin_va_info(&args);
    base = lbl_80401DE0;
    logVsnprintf_float((char*)base + 0x4D0, 0xFF, fmt, &args);
    *(u8*)(base + 0x5CF) = 0;
    fn_80080ED8((u16*)(base + 0x0D0), base + 0x4D0);

    work = base + 0x5D0;
    memset(work, 0, 0x68);
    *(u8*)(work + 0x00) = 1;
    *(f32*)(work + 0x60) = 1.0f;
    *(f32*)(work + 0x64) = 1.0f;
    *(u32*)(work + 0x24) = 0xFFFFFFFF;
    *(u32*)(work + 0x28) = (u32)(base + 0x0D0);
    *(u32*)(work + 0x2C) = (u32)(base + 0x0D0);
    *(u32*)(work + 0x30) = (u32)(base + 0x0D0);
    *(f32*)(work + 0x04) = (f32)x;
    *(f32*)(work + 0x08) = (f32)y;
    *(u32*)(work + 0x24) = color;
    *(u8*)(work + 0x02) = 1;
    *(u16*)(work + 0x20) = 2;

    messageHead = (u8*)lbl_80478B08;
    count = *(u16*)(messageHead + 4);
    fontId = *(u16*)(work + 0x20);
    for (index = 0; index < count; index++) {
        codeEntry = *(u8**)(messageHead + 0x24) + index * 8;
        if (*(u16*)codeEntry == fontId) {
            *(u8*)(work + 0x22) = codeEntry[2];
            *(u8*)(work + 0x23) = codeEntry[3];
            if (fontId == 0) {
                *(u8*)(work + 0x42) = 0xB;
            } else if (fontId == 1 || fontId == 1) {
                *(u8*)(work + 0x42) = 6;
            } else {
                *(s8*)(work + 0x42) = (s8)(s32)(lbl_8047CD20 * (f64)(u32)codeEntry[3] + lbl_8047CD18);
            }
            break;
        }
    }

    spriteSetEnv();
    fn_800D9ED8(1);
    fn_800D88DC(3);
    fn_800D888C(4);
    fn_800D7820(lbl_80314F98);
    messageHead = (u8*)lbl_80478B08;
    fn_800D85D4(0, *(void**)(messageHead + 0x0C + ((s8)messageHead[0x1D] * 4)));
    messageHead = (u8*)lbl_80478B08;
    image = GStextureLockImage(*(void**)(messageHead + 0x0C + ((s8)messageHead[0x1D] * 4)), 0);
    messageHead = (u8*)lbl_80478B08;
    *(void**)(messageHead + 0x14) = image;
    *(f32*)(work + 0x0C) = *(f32*)(work + 0x04);
    *(f32*)(work + 0x10) = *(f32*)(work + 0x08);

    for (;;) {
        code = GSmsgReadCode(work);
        if (code == 0) {
            break;
        }

        if (code == 0x0A || code == 0x0D) {
            continue;
        }
        if (code == 0x20) {
            advance = (f32)(((u8)*(u8*)(work + 0x22)) >> 1) * *(f32*)(work + 0x60);
            *(f32*)(work + 0x14) = advance;
        } else {
            glyph = (u8*)_msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(work, code, &outNode);
            if (glyph == NULL) {
                drawX0 = (s16)*(f32*)(work + 0x0C);
                drawY0 = (s16)((s32)*(f32*)(work + 0x10) + 2);
                drawX1 = (s16)((f32)*(u8*)(work + 0x22) * *(f32*)(work + 0x60) + (f32)drawX0);
                drawY1 = (s16)((f32)*(u8*)(work + 0x23) * *(f32*)(work + 0x64) + (f32)drawY0);
                fn_800D888C(0x80000002u);
                fn_800D6A00(7);
                fn_800D7820(lbl_80314E08);
                fn_800D67BC(2);
                fn_800D61E4(drawX0, drawY0);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D61E4(drawX1, drawY1);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D6728();
                fn_800D88DC(0x80000002u);
                fn_800D7820(lbl_80314F98);
                fn_800DC1D4(1);
                advance = (f32)work[0x22] * *(f32*)(work + 0x60);
                *(f32*)(work + 0x14) = 2.0f + advance;
            } else {
                fontInfo = (u8*)outNode;
                glyphInfo = *(u32*)(glyph + 4);
                glyphWidth = glyph[2];
                fn_800FD69C(work, fontInfo + (*(u32*)(fontInfo + 4) + (glyphInfo & 0xFFFFFF)),
                            glyphWidth, glyph[3], (s8)(glyphInfo >> 24));
                *(f32*)(work + 0x14) = (f32)(s16)glyphWidth * *(f32*)(work + 0x60);
            }
        }

        *(f32*)(work + 0x0C) += *(f32*)(work + 0x14);
    }

    messageHead = (u8*)lbl_80478B08;
    GStextureUnlockImage(*(void**)(messageHead + 0x0C + ((s8)messageHead[0x1D] * 4)));
    return 0;
}

/* 0x800FB43C | 0x244 */
s32 fn_800FB43C(s32 x, s32 y, u32 key) {
    struct MessageGroup* group;
    void* text;
    u8* work;
    u16 fontId;

    work = (u8*)&lbl_80402418;
    text = GSmsgFindMessage(key, &group);
    if (text == NULL) return -1;

    msgInitTask(work, text);
    fontId = group->fontId;
    *(u16*)(work + 0x20) = fontId;
    *(u32*)(work + 0x1C) = key;
    *(f32*)(work + 0x04) = (f32)x;
    *(f32*)(work + 0x08) = (f32)y;
    *(s16*)(work + 0x18) = 0;
    *(s16*)(work + 0x1A) = 0;
    work[0x44] = 3;
    *(s32*)(work + 0x24) = -1;
    work[2] = 1;

    msgSetFontInfo(work);
    return fn_800FC7E0(work, work[0x44], 0, 0);
}

/* 0x800FB680 | 0x248 */
s32 fn_800FB680(s32 x, s32 y, s32 color, u32 key) {
    struct MessageGroup* group;
    void* text;
    u8* work;
    u16 fontId;

    work = (u8*)&lbl_80402418;
    text = GSmsgFindMessage(key, &group);
    if (text == NULL) return -1;

    msgInitTask(work, text);
    fontId = group->fontId;
    *(u16*)(work + 0x20) = fontId;
    *(u32*)(work + 0x1C) = key;
    *(f32*)(work + 0x04) = (f32)x;
    *(f32*)(work + 0x08) = (f32)y;
    *(s16*)(work + 0x18) = 0;
    *(s16*)(work + 0x1A) = 0;
    work[0x44] = 3;
    *(s32*)(work + 0x24) = color;
    work[2] = 1;

    msgSetFontInfo(work);
    return fn_800FC7E0(work, work[0x44], 0, 0);
}

/* 0x800FB8C8 | 0x26C */
s32 fn_800FB8C8(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key) {
    struct MessageGroup* group;
    void* text;
    u8* work;
    u16 fontId;

    x += width - (s16)((u32)GSmsgGetRect(key) >> 16);
    work = (u8*)&lbl_80402418;
    text = GSmsgFindMessage(key, &group);
    if (text == NULL) return -1;

    msgInitTask(work, text);
    fontId = group->fontId;
    *(u16*)(work + 0x20) = fontId;
    *(u32*)(work + 0x1C) = key;
    *(f32*)(work + 0x04) = (f32)x;
    *(f32*)(work + 0x08) = (f32)y;
    *(s16*)(work + 0x18) = width;
    *(s16*)(work + 0x1A) = height;
    work[0x44] = 3;
    *(s32*)(work + 0x24) = color;
    work[2] = 1;

    msgSetFontInfo(work);
    return fn_800FC7E0(work, work[0x44], 0, 0);
}

/* 0x800FBB34 | 0x254 */
s32 fn_800FBB34(s32 x, s32 y, s16 width, s16 height, s32 color, u32 key) {
    struct MessageGroup* group;
    void* text;
    u8* work;
    u16 fontId;

    work = (u8*)&lbl_80402418;
    text = GSmsgFindMessage(key, &group);
    if (text == NULL) return -1;

    msgInitTask(work, text);
    fontId = group->fontId;
    *(u16*)(work + 0x20) = fontId;
    *(u32*)(work + 0x1C) = key;
    *(f32*)(work + 0x04) = (f32)x;
    *(f32*)(work + 0x08) = (f32)y;
    *(s16*)(work + 0x18) = width;
    *(s16*)(work + 0x1A) = height;
    work[0x44] = 3;
    *(s32*)(work + 0x24) = color;
    work[2] = 1;

    msgSetFontInfo(work);
    return fn_800FC7E0(work, work[0x44], 0, 0);
}

/* 0x800FBD88 | 0xF4 */
void fn_800FBD88(u32 key) {
    u8* head;
    u8* entry;
    u8 type;
    u32 r3;

    head = (u8*)lbl_80478B08;
    entry = GSmsgFindCheck(head, key);
    if (entry == NULL) return;
    type = *(u8*)(entry + 0x3);
    r3 = 0;
    switch (type) {
    case 0:
        break;
    case 1:
        r3 = 0x57;
        break;
    case 2:
        r3 = 0x58;
        break;
    case 3:
        r3 = 0x59;
        break;
    case 4:
        r3 = 0x497;
        break;
    case 5:
        r3 = 0x498;
        break;
    default:
        r3 = 0;
        break;
    }
    if (r3 != 0) fn_801669BC(r3);
    *(u8*)(entry + 0x0) = 0;
}

/* 0x800FBE7C | 0x94 */
s32 fn_800FBE7C(u32 key, u32 state, u32 flag) {
    u8* head;
    u8* entry;

    head = (u8*)lbl_80478B08;
    entry = GSmsgFindCheck(head, key);
    if (entry == NULL) return -1;
    return fn_800FC7E0(entry, *(u8*)(entry + 0x44), state, flag);
}

/* 0x800FBF10 | 0x64 */
void GSmsgDaemon(void) {
    GStextureUnlockImage(lbl_80478B08->textures[lbl_80478B08->textureIndex]);
    lbl_80478B08->atlasX = 2;
    lbl_80478B08->atlasY = 1;
    lbl_80478B08->textureIndex ^= 1;
}

/* 0x800FBF74 | 0x25C */
s32 GSmsgExec(key, mode, type)
    u32 key;
    s8 mode;
    s8 type;
{
    u8* mgr;
    u8* work;
    struct MessageGroup* group;
    void* text;
    s32 count;
    s32 i;

    mgr = (u8*)lbl_80478B08;
    count = *(u16*)mgr;
    for (i = 0; i < count; i++) {
        work = *(u8**)(mgr + 0x20) + i * 0x68;
        if (work[0] == 0) {
            break;
        }
    }
    if (i == count) {
        GSlogWrite(lbl_80271730, key);
        return -1;
    }

    text = GSmsgFindMessage(key, &group);
    if (text == NULL) {
        GSlogWrite(lbl_80271754, key);
        return -1;
    }

    msgInitTask(work, text);
    *(u16*)(work + 0x20) = group->fontId;
    *(u32*)(work + 0x1C) = key;
    *(s8*)(work + 0x44) = mode;
    *(s8*)(work + 0x03) = type;

    msgSetFontInfo(work);
    return 0;
}

#endif /* !GS_MSG_PARTIAL */

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_OPENCLOSE_ONLY)
/* 0x800FC1D0 | 0x74 */
s32 GSmsgClose(struct MessageGroup* group) {
    struct MessageSystem* system;
    struct MessageGroup* node;

    system = lbl_80478B08;
    if (system->groups == NULL) return -1;
    node = system->groups;
    while (node != NULL) {
        if (node == group) {
            if (node->previous != NULL) node->previous->next = node->next;
            else system->groups = node->next;
            if (node->next != NULL) node->next->previous = node->previous;
            break;
        }
        node = node->next;
    }
    return 0;
}

/* 0x800FC244 | 0x64. Includes the compiler's trailing return at 0x800FC2A4. */
struct MessageGroup* GSmsgOpen(struct MessageGroup* group) {
    struct MessageSystem* system;
    struct MessageGroup* node;

    system = lbl_80478B08;
    if (system->groups == NULL) {
        system->groups = group;
        group->next = NULL;
        group->previous = NULL;
        return group;
    }
    node = system->groups;
    while (1) {
        if (node == group) return NULL;
        if (node->next == NULL) {
            node->next = group;
            group->next = NULL;
            group->previous = node;
            return group;
        }
        node = node->next;
    }
}

/* 0x800FC2A8 | 0xF4 */
s32 GSmsgFontClose(void* ptr) {
    struct FontFileHeader* file;
    struct MessageSystem* system;
    s32 count;
    struct FontSlot* entry;
    struct FontBank* bank;
    struct FontBank* previous;
    s32 idx;

    file = (struct FontFileHeader*)ptr;
    while (1) {
        system = lbl_80478B08;
        count = system->fontCount;
        for (idx = 0; idx < count; idx++) {
            entry = &system->fonts[idx];
            if (entry->bank != NULL) {
                if (entry->id == file->id) break;
            }
        }
        if (idx != count) {
            bank = entry->bank;
            while (bank != NULL) {
                if (bank == (struct FontBank*)(file + 1)) {
                    previous = bank->previous;
                    if (previous == NULL && bank->next == NULL) {
                        entry->id = 0xFFFF;
                        entry->bank = NULL;
                    } else {
                        if (previous != NULL) {
                            previous->next = bank->next;
                        } else {
                            entry->bank = bank->next;
                        }
                        if (bank->next != NULL) {
                            bank->next->previous = bank->previous;
                        }
                    }
                    break;
                }
                bank = bank->next;
            }
        }
        if (file->nextOffset == 0) break;
        file = (struct FontFileHeader*)((u8*)file + file->nextOffset);
    }
    return 0;
}

/* 0x800FC39C | 0x17C */
void* GSmsgFontOpen(void* ptr) {
    struct FontFileHeader* file;
    struct MessageSystem* system;
    struct FontSlot* entry;
    struct FontBank* bank;
    struct FontBank* node;
    u16 key;
    s32 count;
    s32 idx;
    s32 slot;

    file = (struct FontFileHeader*)ptr;
    while (1) {
        key = file->id;
        if (key == 0xFFFF) return NULL;

        system = lbl_80478B08;
        count = system->fontCount;
        for (idx = 0; idx < count; idx++) {
            entry = &system->fonts[idx];
            if (entry->bank != NULL) {
                if (entry->id == key) break;
            }
        }
        if (idx == count) {
            for (slot = 0; slot < count; slot++) {
                entry = &system->fonts[slot];
                if (entry->bank == NULL) {
                    *(struct FontFileHeader*)entry = *file;
                    bank = (struct FontBank*)(file + 1);
                    entry->bank = bank;
                    bank->next = NULL;
                    bank->previous = NULL;
                    break;
                }
            }
            if (slot == lbl_80478B08->fontCount) {
                GSlogWrite((const char*)lbl_8027177C, file->id);
            }
        } else {
            bank = (struct FontBank*)(file + 1);
            node = entry->bank;
            while (1) {
                if (node == bank) return NULL;
                if (node->next == NULL) {
                    node->next = bank;
                    bank->next = NULL;
                    bank->previous = node;
                    break;
                }
                node = node->next;
            }
        }
        if (file->nextOffset == 0) break;
        file = (struct FontFileHeader*)((u8*)file + file->nextOffset);
    }
    return ptr;
}

/* 0x800FC518 | 0x10 */
s32 GSmsgSetCtrlFunc(struct MessageControl* controls) {
    lbl_80478B08->controls = controls;
    return 0;
}

#endif

#if !defined(GS_MSG_PARTIAL) || defined(GS_MSG_INIT_ONLY)
/* 0x800FC528 | 0x2B8 */
s32 GSmsgInit(u16 taskCount, u16 fontCount) {
    u8* work;
    struct FontSlot* slot;
    s32 index;

    memset(&lbl_804024E8, 0, sizeof(struct MessageSystem));
    lbl_80478B08->taskHandle = _toolentryAlloc__FUl(taskCount * 0x68);
    if (lbl_80478B08->taskHandle == 0) {
        GSlogWrite((const char*)lbl_802717B4);
        return -1;
    }
    lbl_80478B08->tasks = fn_800E27B0(lbl_80478B08->taskHandle);

    lbl_80478B08->fontHandle = _toolentryAlloc__FUl(fontCount * sizeof(struct FontSlot));
    if (lbl_80478B08->fontHandle == 0) {
        GSlogWrite((const char*)lbl_802717B4);
        return -1;
    }
    lbl_80478B08->fonts = fn_800E27B0(lbl_80478B08->fontHandle);

    for (index = 0; index < taskCount; index++) {
        work = lbl_80478B08->tasks + index * 0x68;
        memset(work, 0, 0x68);
        *(f32*)(work + 0x60) = lbl_8047CD08; /* 1.0f scale */
        *(f32*)(work + 0x64) = lbl_8047CD08;
    }
    lbl_80478B08->taskCount = taskCount;
    for (index = 0; index < fontCount; index++) {
        slot = &lbl_80478B08->fonts[index];
        slot->id = 0xFFFF;
        slot->bank = NULL;
    }
    lbl_80478B08->fontCount = fontCount;

    lbl_80478B08->textures[0] = GStextureCreate(0x200, 0x200, 0x40, 0, 0);
    lbl_80478B08->textures[1] = GStextureCreate(0x200, 0x200, 0x40, 0, 0);
    return 0;
}

#endif

#if !defined(GS_MSG_PARTIAL)
/* 0x800FC7E0 | 0xB68 */
s32 fn_800FC7E0(arg0, arg1, arg2, arg3)
    u8 *arg0;
    u8 arg1;
    s32 arg2;
    u8 arg3;
{
    extern void fn_800D85D4(s32, u32);
    extern void fn_800DC224(s32, s32, s32, s32, s32);
    extern void fn_800DBF78(s32, s32);
    extern void fn_800DC0D4(s32, s32, s32, s32, s32);
    extern void fn_800DC14C(s32, s32, s32, s32, s32, s32);
    extern void fn_800DBFD4(s32, s32, s32, s32, s32);
    extern void fn_800DC04C(s32, s32, s32, s32, s32, s32);
    u8 *mgr;
    u16 *cursor;
    u16 *savedCursor;
    u16 code;
    u8 control;
    u8 stackDepth;
    u8 continueFlag;
    u8 quoteFlag;
    u8 glyphWidth;
    u8 normalFlag;
    u8 savedDepth;
    s32 loopCount;
    s32 soundId;
    s32 drawX;
    s32 drawY;
    u32 texHandle;
    f32 angle;
    f32 amp;
    void *fontNode;
    void *fontInfo;
    u32 savedStack[3];
    GXColor color = { 255, 255, 255, 255 };

    quoteFlag = 0;
    if (arg0 == NULL) {
        return -1;
    }
    if (arg0[0] == 0) {
        return -1;
    }

    spriteSetEnv();
    fn_800D88DC(0x80000003);
    fn_800D888C(4);
    fn_800D7820(lbl_80314F98);

    mgr = (u8*)lbl_80478B08;
    fn_800D85D4(0, *(u32*)(mgr + ((s8)mgr[0x1D] * 4) + 0x0C));
    fn_800DC1D4(1);
    fn_800DC224(0, 0, 0, 0, 0);
    fn_800DBEB4(0, color);
    fn_800DBF78(0, 0x0C);
    fn_800DC0D4(0, 0x0F, 0x0E, 0x0A, 0x0F);
    fn_800DC14C(0, 0, 0, 0, 1, 0);
    fn_800DBFD4(0, 7, 4, 5, 7);
    fn_800DC04C(0, 0, 0, 0, 1, 0);

    mgr = (u8*)lbl_80478B08;
    texHandle = (u32)GStextureLockImage(
        *(void**)(mgr + ((s8)mgr[0x1D] * 4) + 0x0C), 0);
    *(u32*)(mgr + 0x14) = texHandle;

    arg0[0x45] = ((arg2 & 0x30) != 0);
    arg0[1] = 0;
    arg0[0x4B] = 0;
    arg0[0x46] = 0;
    normalFlag = 0;
    continueFlag = 0;

    if ((u8)arg3 == 0) {
        for (loopCount = 0; (u32)loopCount < fn_800D3088(); loopCount++) {
            for (;;) {
                code = GSmsgReadCode(arg0);
                if (code == 0) {
                    arg0[0] = 2;
                    break;
                }

                if (code == 0xFFFF) {
                    cursor = *(u16**)(arg0 + 0x30);
                    control = *(u8*)cursor;
                    *(u16**)(arg0 + 0x30) = (u16*)((u8*)cursor + 1);
                    continueFlag = GSmsgDispatchControl(arg0, control);
                } else if (arg0[0x4B] != 2) {
                    continueFlag = 1;
                    if ((u8)arg1 != 0) {
                        continueFlag = 0;
                    }
                    normalFlag = 1;
                }

                if (continueFlag != 0) {
                    break;
                }
            }
        }
    }

    savedStack[0] = *(u32*)(arg0 + 0x34);
    savedStack[1] = *(u32*)(arg0 + 0x38);
    savedStack[2] = *(u32*)(arg0 + 0x3C);
    savedCursor = *(u16**)(arg0 + 0x30);
    savedDepth = arg0[0x40];

    arg0[1] = 1;
    *(f32*)(arg0 + 0x0C) = *(f32*)(arg0 + 0x04);
    *(f32*)(arg0 + 0x10) = *(f32*)(arg0 + 0x08);
    *(u32*)(arg0 + 0x30) = *(u32*)(arg0 + 0x2C);
    arg0[0x40] = 0;
    arg0[0x4B] = 0;

    for (;;) {
        if (*(u16**)(arg0 + 0x30) == savedCursor) {
            s32 i;
            stackDepth = arg0[0x40];
            for (i = 0; i < (s8)stackDepth; i++) {
                if (*(u32*)(arg0 + 0x34 + i * 4) != savedStack[i]) {
                    break;
                }
            }
            if (i == (s8)stackDepth) {
                break;
            }
        }

        code = GSmsgReadCode(arg0);
        if (code == 0) {
            break;
        }

        if (code == 0xFFFF) {
            cursor = *(u16**)(arg0 + 0x30);
            control = *(u8*)cursor;
            *(u16**)(arg0 + 0x30) = (u16*)((u8*)cursor + 1);
            GSmsgDispatchControl(arg0, control);
            if (quoteFlag != 0 && *(f32*)(arg0 + 0x0C) == *(f32*)(arg0 + 0x04)) {
                *(f32*)(arg0 + 0x0C) += (f32)arg0[0x22];
            }
        } else if (arg0[0x4B] != 2) {
            if (code == 0x20) {
                *(f32*)(arg0 + 0x14) = (f32)((arg0[0x22] >> 1) * *(f32*)(arg0 + 0x60));
            } else {
                fontNode = NULL;
                fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, code, &fontNode);
                if (fontInfo == NULL) {
                    s16 x0 = (s16)*(f32*)(arg0 + 0x0C);
                    s16 y0 = (s16)((s32)*(f32*)(arg0 + 0x10) + 2);
                    s16 x1 = (s16)(((f32)arg0[0x22] * *(f32*)(arg0 + 0x60)) + (f32)x0);
                    s16 y1 = (s16)(((f32)arg0[0x23] * *(f32*)(arg0 + 0x64)) + (f32)y0);

                    fn_800D888C(0x80000002);
                    fn_800D6A00(7);
                    fn_800D7820(lbl_80314E08);
                    fn_800D67BC(2);
                    fn_800D61E4(x0, y0);
                    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                    fn_800D61E4(x1, y1);
                    fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                    fn_800D6728();
                    fn_800D88DC(0x80000002);
                    fn_800D7820(lbl_80314F98);
                    fn_800DC1D4(1);
                    *(f32*)(arg0 + 0x14) = lbl_8047CD30 + ((f32)arg0[0x22] * *(f32*)(arg0 + 0x60));
                } else {
                    glyphWidth = ((struct GlyphEntry*)fontInfo)->width;
                    fn_800FD69C(arg0,
                                (u8*)fontNode + ((struct FontBank*)fontNode)->dataOffset +
                                    (((struct GlyphEntry*)fontInfo)->offset & 0xFFFFFF),
                                glyphWidth, ((struct GlyphEntry*)fontInfo)->height,
                                (s8)(((struct GlyphEntry*)fontInfo)->offset >> 24));
                    *(f32*)(arg0 + 0x14) = (f32)((s16)glyphWidth * *(f32*)(arg0 + 0x60));
                }
            }
            *(f32*)(arg0 + 0x0C) += *(f32*)(arg0 + 0x14);
            if ((s8)arg0[0x41] == 0) {
                if (code == 0x300C) {
                    quoteFlag = 1;
                }
                if (code == 0x300D) {
                    quoteFlag = 0;
                }
            }
            if (arg0[0x4B] == 1) {
                fn_800FD348(arg0);
            }
        }
    }

    if ((u8)arg1 != 0) {
        normalFlag = 0;
    }
    if (normalFlag != arg0[0x47]) {
        soundId = 0;
        switch ((s8)arg0[3]) {
        case 1: soundId = 0x57; break;
        case 2: soundId = 0x58; break;
        case 3: soundId = 0x59; break;
        case 4: soundId = 0x497; break;
        case 5: soundId = 0x498; break;
        }
        if (soundId != 0) {
            if (normalFlag != 0) {
                fn_80166A28();
            } else {
                fn_801669BC(soundId);
            }
        }
        arg0[0x47] = normalFlag;
    }

    if (arg0[0x46] != 0) {
        angle = ((lbl_8047CD40 * (f32)lbl_8047AC68) / lbl_8047CD44);
        drawX = (s32)(lbl_8047CD30 + *(f32*)(arg0 + 0x0C));
        drawY = (s32)(lbl_8047CD3C + *(f32*)(arg0 + 0x10));
        amp = (f32)cos(angle);
        drawY += (s32)(lbl_8047CD48 * amp);
        lbl_8047AC68 += fn_800D3088();
        lbl_8047AC68 %= 0x32;
        fn_800D888C(0x80000000);
        fn_800D7820(lbl_80314E08);
        windowDrawSprite(drawX, drawY, 0, 0xB9, 0);
        fn_800D88DC(0x80000000);
        fn_800D7820(lbl_80314F98);
        fn_800DC1D4(1);
    }

    *(u32*)(arg0 + 0x30) = (u32)savedCursor;
    arg0[0x40] = savedDepth;
    *(u32*)(arg0 + 0x34) = savedStack[0];
    *(u32*)(arg0 + 0x38) = savedStack[1];
    *(u32*)(arg0 + 0x3C) = savedStack[2];

    mgr = (u8*)lbl_80478B08;
    GStextureUnlockImage(*(void**)(mgr + ((s8)mgr[0x1D] * 4) + 0x0C));
    fn_800D888C(0x80000000);
    return 0;
}

/* 0x800FD348 | 0x354 */
void fn_800FD348(u8* arg0)
{
    void *fontNode;
    u8 *scan;
    u16 code;
    f32 savedX;
    f32 savedY;
    f32 scaleX;
    f32 scaleY;
    u8 targetCount;
    u8 drawCount;
    u8 glyphWidth;
    u8 glyphHeight;
    s16 x0;
    s16 y0;
    s16 x1;
    s16 y1;
    void *fontInfo;

    arg0[0x58]++;
    targetCount = (s32)(arg0[0x5B] * arg0[0x58]) / (s32)arg0[0x5A];
    drawCount = targetCount - arg0[0x59];
    savedX = *(f32 *)(arg0 + 0x0C);
    savedY = *(f32 *)(arg0 + 0x10);
    *(f32 *)(arg0 + 0x0C) = *(f32 *)(arg0 + 0x4C);
    *(f32 *)(arg0 + 0x10) = *(f32 *)(arg0 + 0x50);
    *(f32 *)(arg0 + 0x60) *= 0.5f;
    *(f32 *)(arg0 + 0x64) *= 0.5f;

    scan = *(u8 **)(arg0 + 0x54);

    while (drawCount != 0) {
        code = *(u16 *)scan;
        scan += 2;
        if (code == 0xFFFF) {
            scan++;
            continue;
        }

        if (code == 0x20) {
            *(f32 *)(arg0 + 0x14) = (f32)((arg0[0x22] / 2) * *(f32 *)(arg0 + 0x60));
        } else {
            fontInfo = _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(arg0, code, &fontNode);
            if (fontInfo == NULL) {
                glyphHeight = arg0[0x23];
                x0 = (s16)*(f32 *)(arg0 + 0x0C);
                y0 = (s16)((s32)*(f32 *)(arg0 + 0x10) + 2);
                scaleX = *(f32 *)(arg0 + 0x60);
                scaleY = *(f32 *)(arg0 + 0x64);
                x1 = (s16)((arg0[0x22] * scaleX) + (f32)x0);
                y1 = (s16)(((f32)glyphHeight * scaleY) + (f32)y0);

                fn_800D888C(0x80000002);
                fn_800D6A00(7);
                fn_800D7820(lbl_80314E08);
                fn_800D67BC(2);
                fn_800D61E4(x0, y0);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D61E4(x1, y1);
                fn_800D5CB8(0, 0xFF, 0xFF, 0xFF, 0xFF);
                fn_800D6728();
                fn_800D88DC(0x80000002);
                fn_800D7820(lbl_80314F98);
                fn_800DC1D4(1);

                *(f32 *)(arg0 + 0x14) = 2.0f + (arg0[0x22] * *(f32 *)(arg0 + 0x60));
            } else {
                glyphWidth = ((struct GlyphEntry *)fontInfo)->width;
                fn_800FD69C(arg0,
                            (u8 *)fontNode + ((struct FontBank *)fontNode)->dataOffset +
                                (((struct GlyphEntry *)fontInfo)->offset & 0xFFFFFF),
                            glyphWidth, ((struct GlyphEntry *)fontInfo)->height,
                            (s8)(((struct GlyphEntry *)fontInfo)->offset >> 0x18));
                *(f32 *)(arg0 + 0x14) = (f32)((s16)glyphWidth * *(f32 *)(arg0 + 0x60));
            }
        }

        drawCount--;
        *(f32 *)(arg0 + 0x0C) += *(f32 *)(arg0 + 0x14);
    }

    *(f32 *)(arg0 + 0x4C) = *(f32 *)(arg0 + 0x0C);
    *(f32 *)(arg0 + 0x50) = *(f32 *)(arg0 + 0x10);
    *(u32 *)(arg0 + 0x54) = (u32)scan;
    arg0[0x59] = (u8)targetCount;
    *(f32 *)(arg0 + 0x0C) = savedX;
    *(f32 *)(arg0 + 0x10) = savedY;
    *(f32 *)(arg0 + 0x60) *= 2.0f;
    *(f32 *)(arg0 + 0x64) *= 2.0f;
}

/* 0x800FD69C | 0x880 */
void fn_800FD69C(u8* arg0, const u8* arg1, s16 arg2, s16 arg3, s16 arg4)
{
    u8 *mgr;
    u8 *buffer;
    const u8 *srcRow;
    s16 curX;
    s16 curY;
    s32 row;
    s32 rowPos;
    s32 xPos;
    s16 drawX0;
    s16 drawY0;
    s16 drawX1;
    s16 drawY1;
    s32 widthRounded;
    s32 srcOffset;
    s32 destOffset;
    s32 tileOffset;
    s32 texelOffset;
    s32 rowIndex;
    u8 outlineAlpha;
    u32 color;
    f32 scaleX;
    f32 scaleY;
    f32 baseX;
    f32 baseY;
    f32 u0;
    f32 v0;
    f32 u1;
    f32 v1;

    mgr = (u8 *)lbl_80478B08;
    if (*(s16 *)(mgr + 0x18) + (arg2 + 2) >= 0x200) {
        *(s16 *)(mgr + 0x1A) = *(s16 *)(mgr + 0x1A) + *(u8 *)(mgr + 0x1C) + 2;
        mgr = (u8*)lbl_80478B08;
        *(s16 *)(mgr + 0x18) = 2;
        mgr = (u8*)lbl_80478B08;
        *(u8 *)(mgr + 0x1C) = arg0[0x23] + 2;
        mgr = (u8*)lbl_80478B08;
        if (*(s16 *)(mgr + 0x1A) + *(u8 *)(mgr + 0x1C) >= 0x200) {
            *(s16 *)(mgr + 0x1A) = 1;
        }
    }

    mgr = (u8*)lbl_80478B08;
    if (*(u8 *)(mgr + 0x1C) < arg3) {
        *(u8 *)(mgr + 0x1C) = arg0[0x23];
    }

    buffer = (u8*)lbl_80478B08->image;
    widthRounded = ((arg2 + 1) & ~1) / 2;

    for (row = -1; row < arg3 + 1; row++) {
        rowPos = lbl_80478B08->atlasY + row;
        tileOffset = (rowPos >> 3) * 64;
        texelOffset = (rowPos & 7) << 3;
        for (xPos = -2; xPos < arg2 + 2; xPos += 2) {
            srcOffset = lbl_80478B08->atlasX + xPos;
            buffer[(tileOffset + (srcOffset >> 3)) * 32 +
                   (((srcOffset & 7) + texelOffset) >> 1)] = 0;
        }
    }

    srcOffset = 0;
    for (rowIndex = 0; rowIndex < arg3; rowIndex++) {
        rowPos = lbl_80478B08->atlasY + rowIndex;
        tileOffset = (rowPos >> 3) * 64;
        texelOffset = (rowPos & 7) << 3;
        srcRow = arg1 + srcOffset;
        for (xPos = 0; xPos < arg2; xPos += 2) {
            destOffset = lbl_80478B08->atlasX + xPos;
            buffer[(tileOffset + (destOffset >> 3)) * 32 +
                   (((destOffset & 7) + texelOffset) >> 1)] = *srcRow++;
        }
        srcOffset = srcOffset + widthRounded;
    }

    curX = lbl_80478B08->atlasX;
    curY = lbl_80478B08->atlasY;
    scaleX = *(f32 *)(arg0 + 0x60);
    scaleY = *(f32 *)(arg0 + 0x64);
    baseX = *(f32 *)(arg0 + 0x0C);
    baseY = ((f32)(s16)arg4 * scaleY) + *(f32 *)(arg0 + 0x10);
    baseY += (f32)(s8)arg0[0x43];
    drawX0 = (s16)baseX;
    drawY0 = (s16)baseY;
    drawX1 = (s16)(((f32)(s16)arg2 * scaleX) + (f32)drawX0);
    drawY1 = (s16)(((f32)(s16)arg3 * scaleY) + (f32)drawY0);

    u0 = (f32)curX * lbl_8047CD4C;
    v0 = (f32)curY * lbl_8047CD4C;
    u1 = (f32)(curX + arg2) * lbl_8047CD4C;
    v1 = (f32)(curY + arg3) * lbl_8047CD4C;

    color = *(u32 *)(arg0 + 0x24);
    outlineAlpha = (u8)(((color & 0xFF) * 0xC0) / 0xFF);

    fn_800D6A00(7);

    switch (arg0[2]) {
    case 0:
    default:
        fn_800D67BC(2);
        break;

    case 1:
        fn_800D67BC(4);
        fn_800D61E4(drawX0 + 1, drawY0 + 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u0, v0);
        fn_800D61E4(drawX1 + 1, drawY1 + 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u1, v1);
        break;

    case 2:
        fn_800D67BC(0xA);
        fn_800D61E4(drawX0 - 1, drawY0);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u0, v0);
        fn_800D61E4(drawX1 - 1, drawY1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u1, v1);
        fn_800D61E4(drawX0 + 1, drawY0);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u0, v0);
        fn_800D61E4(drawX1 + 1, drawY1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u1, v1);
        fn_800D61E4(drawX0, drawY0 - 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u0, v0);
        fn_800D61E4(drawX1, drawY1 - 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u1, v1);
        fn_800D61E4(drawX0, drawY0 + 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u0, v0);
        fn_800D61E4(drawX1, drawY1 + 1);
        fn_800D5CB8(0, 0, 0, 0, outlineAlpha);
        fn_800D59B8(0, u1, v1);
        break;

    }

    fn_800D61E4(drawX0, drawY0);
    fn_800D5BA0(0, *(u32*)(arg0 + 0x24));
    fn_800D59B8(0, u0, v0);
    fn_800D61E4(drawX1, drawY1);
    fn_800D5BA0(0, *(u32*)(arg0 + 0x24));
    fn_800D59B8(0, u1, v1);
    fn_800D6728();

    mgr = (u8*)lbl_80478B08;
    *(s16 *)(mgr + 0x18) = *(s16*)(mgr + 0x18) + widthRounded * 2 + 2;
}

/* 0x800FDF1C | 0xC8 */
u16* _msgGetCodeInfo__FP13MSG_TASK_WORKUsPP12tagFONT_INFO(u8* work, u16 code, void** outBank) {
    u8* head;
    s32 count;
    struct FontSlot* slot;
    s32 index;
    struct FontBank* bank;
    struct GlyphEntry* entries;
    struct GlyphEntry* entry;
    u32 low;
    u32 high;
    u32 mid;

    head = (u8*)lbl_80478B08;
    count = *(u16*)(head + 4);
    for (index = 0; index < count; index++) {
        slot = *(struct FontSlot**)(head + 0x24) + index;
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

/* 0x800FDFE4 | 0x2C */
s32 _msgGetLength__FPCUs(const void* str) {
    s32 r;
    r = _msgGetSize__FPCUs(str);
    return (s32)(((u32)r + 1) >> 1) - 1;
}

/* 0x800FE010 | 0x34C */
s32 _msgGetSize__FPCUs(const u16* arg0)
{
    u8 *work;
    u16 code;
    u32 control;
    u8 *mgr;
    struct MessageControl *table;
    struct MessageControl *controlEntry;
    u8 *entry;
    u8 *next;
    u32 flags;
    u32 result;
    u32 mode;
    s8 depth;
    s32 i;

    if (arg0 == NULL) {
        GSlogWrite((const char *)lbl_802717D4);
        return 0;
    }

    /* Retail rematerialises &lbl_80402480 after the memset for these header
     * stores (fn_800F96E4 does the same), so they name the buffer directly. */
    work = (u8 *)&lbl_80402480;
    memset(work, 0, 0x68);
    lbl_80402480[0] = 1;
    *(f32 *)(lbl_80402480 + 0x60) = 1.0f;
    *(f32 *)(lbl_80402480 + 0x64) = 1.0f;
    *(s32 *)(lbl_80402480 + 0x24) = -1;
    *(u32 *)(lbl_80402480 + 0x28) = (u32)arg0;
    *(u32 *)(lbl_80402480 + 0x2C) = (u32)arg0;
    *(u32 *)(lbl_80402480 + 0x30) = (u32)arg0;
    lbl_80402480[1] = 1;

    mgr = (u8 *)lbl_80478B08;
    for (i = 0; i < *(u16 *)(mgr + 0x04); i++) {
        entry = (u8 *)*(u32 *)(mgr + 0x24) + i * 8;
        if (*(u16 *)entry == *(u16 *)(work + 0x20)) {
            work[0x22] = entry[2];
            work[0x23] = entry[3];
            if (*(u16 *)(work + 0x20) == 0) {
                *(s8 *)(work + 0x42) = 0xB;
            } else if (*(u16 *)(work + 0x20) == 1 || *(u16 *)(work + 0x20) == 1) {
                *(s8 *)(work + 0x42) = 6;
            } else {
                *(s8 *)(work + 0x42) = (s8)(s32)((lbl_8047CD20 * (f64)entry[3]) + lbl_8047CD18);
            }
            break;
        }
    }

    for (;;) {
        code = GSmsgReadCode(work);
        if (code == 0) break;
        if (code != 0xFFFF) {
            continue;
        }

        control = *(*(u8 **)(work + 0x30))++;
        table = *(struct MessageControl**)((u8*)lbl_80478B08 + 0x28);
        if (table == NULL) {
            continue;
        }

        if (work[1] == 0) {
            flags = table[control].execute;
        } else {
            flags = table[control].measure;
        }
        if (flags == 0) {
            continue;
        }
        controlEntry = &table[control];
        if (controlEntry->callback == NULL) {
            continue;
        }

        result = controlEntry->callback(work);
        mode = controlEntry->mode;
        if (mode == 0 || result == 0) {
            continue;
        }

        /* Retail has no default: for mode 3 "next" keeps whatever it held
         * (no initialising instruction on that path in the target). */
        switch (mode) {
        case 1:
            next = (u8 *)result;
            break;
        case 2:
            next = GSmsgFindMessage(result, NULL);
            break;
        }

        depth = *(s8*)(work + 0x40);
        if (depth >= 3) {
            GSlogWrite((const char *)lbl_80271700, lbl_80315678);
        } else {
            (*(s8*)(work + 0x40))++;
            *(u32 *)(work + 0x34 + depth * 4) =
                *(u32 *)(work + 0x30);
            *(u32 *)(work + 0x30) = (u32)next;
        }
    }

    return ((u8 *)*(u32 *)(work + 0x30) - (u8 *)arg0) + 2;
}
#endif /* !GS_MSG_PARTIAL */
