/**
 * GSlog: in-memory text log (0x800DD270-0x800DE09C).
 *
 * The retail GSlog translation unit also contains the snprintf wrapper
 * fn_800DE09C and the two formatters fn_800DE128 / logVsnprintf_float that
 * follow it: its pooled string table at 0x802704A0 starts with the formatter's
 * "0123456789ABCDEF" digit table and continues with the GSlog messages used
 * below. This object covers the exact prefix range only, so the string pool
 * and the .bss/.sbss state stay extern.
 *
 * Layout: a text buffer of lbl_8047AB04 bytes and a table of u16 line lengths
 * (one entry per 128 buffer bytes). Lines are stored back to back without a
 * separator other than their trailing NUL; when a new line does not fit, the
 * oldest lines are dropped from the front.
 */
#include "dolphin/types.h"

extern void GSlogWrite(const char*, ...);
extern void fn_800DE09C(char*, u32, const char*, ...);
extern void fn_800DE128(char*, u32, const char*, void*);
extern void logVsnprintf_float(char*, u32, const char*, void*);
extern void fn_800E209C(u16 handle);   /* GSmem: free handle */
extern void fn_800E24B0(u16 handle);   /* GSmem: unlock handle */
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u16 handle);  /* GSmem: lock handle, return pointer */
extern u64 OSGetTime(void);
extern void OSTicksToCalendarTime(u64 ticks, void* td);
extern u32 strlen(const char*);
extern void* memcpy(void* dst, const void* src, u32 n);

extern u16 lbl_8047AAF8;  /* text buffer handle */
extern u16 lbl_8047AAFA;  /* line length table handle */
extern u8* lbl_8047AAFC;  /* text buffer (locked) */
extern u16* lbl_8047AB00; /* line length table (locked) */
extern u32 lbl_8047AB04;  /* text buffer size */
extern u32 lbl_8047AB08;  /* line count */
extern u32 lbl_8047AB0C;  /* line capacity */
extern u8 lbl_8047AB10;
extern u8 lbl_8047AB11;   /* prefix lines with a timestamp */

extern char lbl_802704A0[]; /* TU string pool */
extern char lbl_802704B4[]; /* "[%d/%02d %d:%02d:%02d] " */
extern char lbl_80400F30[]; /* GSlogWritef timestamp, 0x14 bytes */
extern char lbl_80400F44[]; /* GSlogWritef message, 0x100 bytes */
extern char lbl_80401044[]; /* GSlogWrite timestamp, 0x14 bytes */
extern char lbl_80401058[]; /* GSlogWrite message, 0x100 bytes */

typedef struct GSLogVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} GSLogVaList;

typedef struct GSCalendarTime {
    s32 sec;
    s32 min;
    s32 hour;
    s32 mday;
    s32 mon;
    s32 year;
    s32 wday;
    s32 yday;
    s32 msec;
    s32 usec;
} GSCalendarTime;

u32 GSlogGetLine(u32 count) {
    u16* entries;
    u32 sum;
    u32 i;
    u32 ret;

    if (lbl_8047AAF8 == 0) {
        return 0;
    }
    if (count > lbl_8047AB08) {
        return 0;
    }

    entries = fn_800E27B0(lbl_8047AAFA);
    sum = 0;
    for (i = 0; i < count; i++) {
        sum += entries[i];
    }
    ret = (u32)lbl_8047AAFC + sum;
    fn_800E24B0(lbl_8047AAFA);
    return ret;
}

u32 GSlogGetLineCount(void) { return lbl_8047AB08; }

/* Forward copy that moves whole words when source and destination are a word
 * or more apart and bytes otherwise. The distance test is transcribed from
 * the retail code, which recomputes the difference and selects it when it is
 * non-zero (an ABS-style macro expansion); both loggers expand it the same
 * way, and so does the GSmem compactor at 0x800E1544. */
static inline void GSlogCopyForward(void* dst, const void* src, u32 size) {
    if ((u32)(((s32)src - (s32)dst) != 0 ? ((s32)src - (s32)dst)
                                         : -((s32)src - (s32)dst)) >= 4) {
        u32* wordDst = (u32*)dst;
        const u32* wordSrc = (const u32*)src;
        u32 words = size >> 2;
        u32 rest = size & 3;
        u8* byteDst;
        const u8* byteSrc;

        while (words-- != 0) {
            *wordDst++ = *wordSrc++;
        }
        byteDst = (u8*)wordDst;
        byteSrc = (const u8*)wordSrc;
        while (rest-- != 0) {
            *byteDst++ = *byteSrc++;
        }
    } else {
        u8* byteDst = (u8*)dst;
        const u8* byteSrc = (const u8*)src;

        while (size-- != 0) {
            *byteDst++ = *byteSrc++;
        }
    }
}

/* Bytes currently held by the stored lines. */
static inline u32 GSlogUsedBytes(void) {
    u16* entries = lbl_8047AB00;
    u32 used = 0;
    u32 i;

    for (i = 0; i < lbl_8047AB08; i++) {
        used += entries[i];
    }
    return used;
}

/* Remove the oldest line: slide the text and the length table down. */
static inline void GSlogDropOldestLine(void) {
    u16* entries = lbl_8047AB00;
    u32 i;
    u16* dst;
    u16* src;

    GSlogCopyForward(lbl_8047AAFC, lbl_8047AAFC + entries[0],
                     lbl_8047AB04 - entries[0]);
    dst = lbl_8047AB00;
    src = dst + 1;
    for (i = lbl_8047AB08 - 1; i != 0; i--) {
        *dst++ = *src++;
    }
    lbl_8047AB08--;
}

static inline u8 GSlogHasRoom(u16 lineLength) {
    u8 fits = TRUE;

    if (lbl_8047AB08 >= lbl_8047AB0C) {
        fits = FALSE;
    }
    if (lineLength + GSlogUsedBytes() >= lbl_8047AB04) {
        fits = FALSE;
    }
    return fits;
}

void GSlogWritef(const char* fmt, ...) {
    GSLogVaList args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    __builtin_va_info(&args);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C(lbl_80400F30, 0x14, lbl_802704B4, time.mon + 1, time.mday,
                    time.hour, time.min, time.sec);
    }

    logVsnprintf_float(lbl_80400F44, 0xFF, fmt, &args);
    lbl_80400F44[0xFF] = 0;

    if (lbl_8047AAFC != NULL) {
        if (lbl_8047AB11 != 0) {
            lineLength = strlen(lbl_80400F30) + strlen(lbl_80400F44) + 1;
        } else {
            lineLength = strlen(lbl_80400F44) + 1;
        }

        do {
            fits = GSlogHasRoom(lineLength);
            if (!fits) {
                GSlogDropOldestLine();
            }
        } while (!fits);

        buffer = lbl_8047AAFC + GSlogUsedBytes();
        if (lbl_8047AB11 != 0) {
            memcpy(buffer, lbl_80400F30, strlen(lbl_80400F30));
            buffer += strlen(lbl_80400F30);
        }
        memcpy(buffer, lbl_80400F44, strlen(lbl_80400F44) + 1);
        lbl_8047AB00[lbl_8047AB08++] = lineLength;
    }

    /* Retail evaluates both lengths again and discards them (the remains of
     * a compiled-out echo, most likely). */
    if (lbl_8047AB11 != 0) {
        strlen(lbl_80400F30);
    }
    strlen(lbl_80400F44);
}

void GSlogWrite(const char* fmt, ...) {
    GSLogVaList args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    __builtin_va_info(&args);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C(lbl_80401044, 0x14, lbl_802704B4, time.mon + 1, time.mday,
                    time.hour, time.min, time.sec);
    }

    fn_800DE128(lbl_80401058, 0xFF, fmt, &args);
    lbl_80401058[0xFF] = 0;

    if (lbl_8047AAFC != NULL) {
        if (lbl_8047AB11 != 0) {
            lineLength = strlen(lbl_80401044) + strlen(lbl_80401058) + 1;
        } else {
            lineLength = strlen(lbl_80401058) + 1;
        }

        do {
            fits = GSlogHasRoom(lineLength);
            if (!fits) {
                GSlogDropOldestLine();
            }
        } while (!fits);

        buffer = lbl_8047AAFC + GSlogUsedBytes();
        if (lbl_8047AB11 != 0) {
            memcpy(buffer, lbl_80401044, strlen(lbl_80401044));
            buffer += strlen(lbl_80401044);
        }
        memcpy(buffer, lbl_80401058, strlen(lbl_80401058) + 1);
        lbl_8047AB00[lbl_8047AB08++] = lineLength;
    }

    if (lbl_8047AB11 != 0) {
        strlen(lbl_80401044);
    }
    strlen(lbl_80401058);
}

/* Allocates a text buffer of `size` bytes plus one u16 length slot per 128
 * bytes. Returns 1 on success (or when logging is disabled with size 0). */
u32 GSlogInit(u32 size, u8 timestamps) {
    const char* strings = lbl_802704A0;

    lbl_8047AB10 = 0;
    lbl_8047AB04 = size;
    lbl_8047AB11 = timestamps;
    if (size == 0) {
        GSlogWrite(strings + 0x2c); /* "GSlog: Init OK, no buffer\n" */
        return 1;
    }

    lbl_8047AAF8 = _toolentryAlloc__FUl(size);
    if (lbl_8047AAF8 == 0) {
        GSlogWrite(strings + 0x48); /* "GSlog: Init FAILED\n" */
        return 0;
    }

    lbl_8047AB0C = size >> 7;
    lbl_8047AAFA = _toolentryAlloc__FUl((size >> 6) & ~1);
    if (lbl_8047AAFA == 0) {
        fn_800E209C(lbl_8047AAF8);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    lbl_8047AAFC = fn_800E27B0(lbl_8047AAF8);
    if (lbl_8047AAFC == NULL) {
        fn_800E209C(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAFA);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    lbl_8047AB00 = fn_800E27B0(lbl_8047AAFA);
    if (lbl_8047AB00 == NULL) {
        fn_800E24B0(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAFA);
        GSlogWrite(strings + 0x48);
        return 0;
    }

    /* "GSlog: Init OK, log buffer size %d bytes\n" */
    GSlogWrite(strings + 0x5c, lbl_8047AB04);
    return 1;
}
