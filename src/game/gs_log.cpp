/**
 * GSlog: in-memory text log, its snprintf wrapper and the formatter
 * GSlogWrite uses (.text 0x800DD270-0x800DE680).
 *
 * The retail translation unit continues with logVsnprintf_float (0x800DE680-0x800DEFC8), GSlogWritef's formatter with
 * %f support. That function is still a candidate (game/gs_log_800DE680.cpp,
 * see its header), so this object is the TU up to it and owns the data only
 * its own functions reference:
 *   .rodata 0x802704A0-0x80270528  "0123456789ABCDEF" (lbl_80478AE8's
 *                                  target, the digit table),
 *                                  the timestamp format, GSlogInit messages
 *   .data   0x80315388-0x8031540C  fn_800DE128's switch table
 *   .sdata  0x80478AE8-0x80478AF0  lbl_80478AE8, the digit table pointer
 *   .sbss   0x8047AAF8-0x8047AB18  the log state
 *   .sdata2 0x8047CAA0-0x8047CAB0  "(float)", "(null)"
 * The .bss buffers (0x80400F30-0x804011B8) stay extern: logVsnprintf_float
 * addresses its own three buffers from the TU's .bss base (0x80400F30, the
 * first GSlogWritef buffer), so the block can only be compiled as a whole
 * together with it.
 *
 * Source attribution: the XD path GSAPI/GSlogM/GSlog.cpp and the XD helper
 * names (logFloat2Str, logHex2Str, logInt2Str, logStr2Int, logStrRev) are
 * inherited from commit e0e2b44b, which read TeamOrre/xd-decomp's
 * splits.txt; they are not verified here (no XD symbol data in this repo).
 * That the TU is C++ rests on the build evidence below, not on that path.
 *
 * The TU is C++ (MWCC lays its statics out in declaration order; see
 * gs_log_800DE680.cpp for the .bss evidence). Built as one C++ file
 * (-lang=c++) the whole TU reproduces every retail section offset: GSlogWritef's and GSlogWrite's
 * timestamp/message buffers, then fraction/field/spec for each formatter,
 * including fn_800DE128's fraction buffer at 0x80401158, which it declares
 * but never uses.
 *
 * Log layout: a text buffer of lbl_8047AB04 bytes and a table of u16 line
 * lengths (one entry per 128 buffer bytes). Lines are stored back to back
 * with their trailing NUL; when a new line does not fit, the oldest lines are
 * dropped from the front.
 */
#include "dolphin/types.h"
#include "game/gs_log_format.h"

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

extern "C" {
u64 OSGetTime(void);
void OSTicksToCalendarTime(u64 ticks, GSCalendarTime* td);
void* memcpy(void* dst, const void* src, u32 n);

void fn_800E209C(u16 handle);  /* GSmem: free handle */
void fn_800E24B0(u16 handle);  /* GSmem: unlock handle */
void* fn_800E27B0(u16 handle); /* GSmem: lock handle, return pointer */

u32 GSlogGetLine(u32 count);
u32 GSlogGetLineCount(void);
void GSlogWritef(const char* fmt, ...);
void GSlogWrite(const char* fmt, ...);
u32 GSlogInit(u32 size, u8 timestamps);
void fn_800DE09C(char* dst, u32 size, const char* format, ...);
void fn_800DE128(char* output, u32 capacity, const char* format, va_list args);
void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args);

/* The TU's .bss block (see above). */
extern char lbl_80400F30[0x14];  /* GSlogWritef: timestamp */
extern char lbl_80400F44[0x100]; /* GSlogWritef: message */
extern char lbl_80401044[0x14];  /* GSlogWrite: timestamp */
extern char lbl_80401058[0x100]; /* GSlogWrite: message */
extern char lbl_80401168[16];    /* fn_800DE128: converted field */
extern char lbl_80401178[16];    /* fn_800DE128: conversion specifier */
}

u16 _toolentryAlloc(u32 size); /* GSmem: allocate a handle */

char* lbl_80478AE8 = "0123456789ABCDEF";

static u16 lbl_8047AAF8;   /* text buffer handle */
static u16 lbl_8047AAFA;   /* line length table handle */
static u8* lbl_8047AAFC;   /* text buffer (locked) */
static u16* lbl_8047AB00;  /* line length table (locked) */
static u32 lbl_8047AB04;   /* text buffer size */
static u32 lbl_8047AB08;   /* line count */
static u32 lbl_8047AB0C;   /* line capacity */
static u8 lbl_8047AB10;    /* cleared by GSlogInit, never read */
static u8 lbl_8047AB11;    /* prefix lines with a timestamp */

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

    entries = (u16*)fn_800E27B0(lbl_8047AAFA);
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

    GSlogCopyForward(lbl_8047AAFC, lbl_8047AAFC + entries[0], lbl_8047AB04 - entries[0]);
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
    va_list args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    va_start(args, fmt);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C(lbl_80400F30, 0x14, "[%d/%02d %d:%02d:%02d] ", time.mon + 1,
                    time.mday, time.hour, time.min, time.sec);
    }

    logVsnprintf_float(lbl_80400F44, 0xFF, fmt, args);
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
    va_list args;
    GSCalendarTime time;
    u16 lineLength;
    u8 fits;
    u8* buffer;

    va_start(args, fmt);
    if (lbl_8047AB11 != 0) {
        OSTicksToCalendarTime(OSGetTime(), &time);
        fn_800DE09C(lbl_80401044, 0x14, "[%d/%02d %d:%02d:%02d] ", time.mon + 1,
                    time.mday, time.hour, time.min, time.sec);
    }

    fn_800DE128(lbl_80401058, 0xFF, fmt, args);
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
    lbl_8047AB10 = 0;
    lbl_8047AB04 = size;
    lbl_8047AB11 = timestamps;
    if (size == 0) {
        GSlogWrite("GSlog: Init OK, no buffer\n");
        return 1;
    }

    lbl_8047AAF8 = _toolentryAlloc(size);
    if (lbl_8047AAF8 == 0) {
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AB0C = size >> 7;
    lbl_8047AAFA = _toolentryAlloc((size >> 6) & ~1);
    if (lbl_8047AAFA == 0) {
        fn_800E209C(lbl_8047AAF8);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AAFC = (u8*)fn_800E27B0(lbl_8047AAF8);
    if (lbl_8047AAFC == NULL) {
        fn_800E209C(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAFA);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    lbl_8047AB00 = (u16*)fn_800E27B0(lbl_8047AAFA);
    if (lbl_8047AB00 == NULL) {
        fn_800E24B0(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAF8);
        fn_800E209C(lbl_8047AAFA);
        GSlogWrite("GSlog: Init FAILED\n");
        return 0;
    }

    GSlogWrite("GSlog: Init OK, log buffer size %d bytes\n", lbl_8047AB04);
    return 1;
}

void fn_800DE09C(char* dst, u32 size, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    fn_800DE128(dst, size, format, args);
}

void fn_800DE128(char* output, u32 capacity, const char* format, va_list args)
{
    const char* src = format;
    char* dst = output;
    char* token;
    u8 done = 0;
    u8 parsing = 0;
    u8 leftJustify = 0;
    u8 zeroPad = 0;
    u8 ready = 0;
    char* text;
    s32 width;
    u32 hex;
    s32 number;

    while (!done) {
        if (!parsing) {
            if (*src == '%') {
                if (src[1] == '%') {
                    *dst++ = '%';
                    src++;
                } else {
                    token = lbl_80401178;
                    parsing = 1;
                }
            } else {
                *dst++ = *src;
            }
        } else {
            switch (*src) {
            case 'c':
                text = lbl_80401168;
                lbl_80401168[0] = va_arg(args, int);
                lbl_80401168[1] = 0;
                ready = 1;
                break;
            case 'd':
                number = va_arg(args, int);
                logInt2Str(number, lbl_80401168);
                text = lbl_80401168;
                ready = 1;
                break;
            case 'f':
                va_arg(args, double);
                text = "(float)";
                ready = 1;
                break;
            case 's':
                text = va_arg(args, char*);
                if (text == NULL) {
                    text = "(null)";
                }
                ready = 1;
                break;
            case 'X':
            case 'x':
                hex = va_arg(args, u32);
                if (*src == 'X') {
                    logHex2Str(hex, lbl_80401168, TRUE);
                } else {
                    logHex2Str(hex, lbl_80401168, FALSE);
                }
                text = lbl_80401168;
                ready = 1;
                break;
            default:
                *token++ = *src;
                break;
            }

            if (ready == 1) {
                s32 length;

                *token = 0;
                token = lbl_80401178;
                if (*token == '-') {
                    leftJustify = 1;
                    token++;
                }
                if (*token == '0') {
                    zeroPad = 1;
                    token++;
                }
                width = logStr2Int(token);
                length = strlen(text);
                if (width > length) {
                    width -= length;
                    if (!leftJustify) {
                        while (width-- != 0 && (u32)(dst - output) < capacity) {
                            if (zeroPad) {
                                *dst++ = '0';
                            } else {
                                *dst++ = ' ';
                            }
                        }
                    }
                }
                while (*text != 0 && (u32)(dst - output) < capacity) {
                    *dst++ = *text++;
                }
                if (leftJustify == 1) {
                    while (width-- != 0 && (u32)(dst - output) < capacity) {
                        *dst++ = ' ';
                    }
                }
                leftJustify = 0;
                zeroPad = 0;
                parsing = 0;
                ready = 0;
            }
        }

        if (*src == 0 || (u32)(dst - output) >= capacity) {
            done = 1;
        }
        src++;
    }
    *dst = 0;
}
