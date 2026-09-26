/**
 * GSlog formatters, candidate range 0x800DE128-0x800DEFC8 (not linked).
 *
 * fn_800DE128 (0x800DE128) is the vsnprintf-style formatter GSlogWrite uses,
 * without float support; fn_800DE09C is the snprintf wrapper over it and
 * logVsnprintf_float (0x800DE680) is the variant GSlogWritef uses, with a
 * real %f. All three belong to the retail GSlog translation unit: its pooled
 * strings at 0x802704A0 start with the formatter's "0123456789ABCDEF" digit
 * table, and its .data (0x80315388-0x80315490) is the two formatters' switch
 * tables.
 *
 * Why this range is not linked yet:
 * - fn_800DE128 is instruction-exact (GC/2.0 -O4,p), but it cannot be linked
 *   on its own: its jump table ends at 0x8031540C, inside the TU's .data, and
 *   a compiled data object starting there is aligned to 8 (0x80315410), which
 *   shifts everything after it. The table can only be owned together with
 *   logVsnprintf_float's (0x8031540C-0x80315490).
 * - logVsnprintf_float addresses its three 16-byte buffers from the TU's
 *   .bss base (lbl_80400F30 + 0x258/0x268/0x278), i.e. they are pooled
 *   statics of the whole GSlog TU. With function-local statics it reaches
 *   97.5%; the rest is register choice in the fraction digits and the
 *   precision parse, and GC/2.0 lays pooled statics out in first-reference
 *   order, which puts the fraction buffer last instead of first. Here the
 *   buffers are extern, which costs the pooled base (88%).
 *
 * Supported conversions: %c %d %s %x %X, %f, %%, with the '-' (left justify)
 * and '0' (zero pad) flags and a decimal width (and a precision for %f in the
 * float variant). Output is truncated at `capacity` characters and always NUL
 * terminated after it. fn_800DE128 prints "(float)" for %f.
 *
 * The helpers are the XD GSlog helpers (logInt2Str, logHex2Str, logStr2Int,
 * logStrRev, logFloat2Str). The target expands each of them repeatedly:
 * logStrRev in every integer conversion, logHex2Str once per case letter,
 * logInt2Str for %d and twice in %f, logStr2Int for the width and the %f
 * precision.
 */
#include "dolphin/types.h"

typedef struct {
    char gpr;
    char fpr;
    char reserved[2];
    char* input_arg_area;
    char* reg_save_area;
} __va_list[1];
typedef __va_list va_list;

extern void* __va_arg(void* ap, int type);
#define va_arg(ap, t) (*((t*)__va_arg(ap, _var_arg_typeof(t))))

extern u32 strlen(const char* s);

extern char* lbl_80478AE8;    /* digit table: "0123456789ABCDEF" */
extern char lbl_80401168[16]; /* converted field */
extern char lbl_80401178[16]; /* conversion specifier being parsed */
extern char lbl_8047CAA0[8];  /* "(float)" */
extern char lbl_8047CAA8[7];  /* "(null)" */
extern char lbl_80401188[16]; /* logVsnprintf_float: fraction digits */
extern char lbl_80401198[16]; /* logVsnprintf_float: converted field */
extern char lbl_804011A8[16]; /* logVsnprintf_float: conversion specifier */
extern char* strchr(const char* s, int c);

static inline void logStrRev(char* start)
{
    char* end = &start[strlen(start) - 1];

    while (start < end) {
        *start ^= *end;
        *end ^= *start;
        *start++ ^= *end--;
    }
}

static inline void logInt2Str(s32 value, char* buf)
{
    char* p = buf;
    u8 negative = 0;

    if (value < 0) {
        negative = 1;
        value = -value;
    }
    do {
        *p++ = lbl_80478AE8[value % 10];
        value /= 10;
    } while (value != 0);
    if (negative == 1) {
        *p++ = '-';
    }
    *p = 0;
    logStrRev(buf);
}

static inline void logHex2Str(u32 value, char* buf, BOOL upper)
{
    char* p = buf;

    if (upper) {
        do {
            *p++ = lbl_80478AE8[value & 0xF];
            value >>= 4;
        } while (value != 0);
    } else {
        do {
            *p = lbl_80478AE8[value & 0xF];
            if (*p >= 'A') {
                *p += 'a' - 'A';
            }
            value >>= 4;
            p++;
        } while (value != 0);
    }
    *p = 0;
    logStrRev(buf);
}

/*
 * Writes the sign and integer part to buf, and to fraction the fractional
 * part as up to ten leading zeros followed by (s32)(fraction * 1e9), padded
 * with '0' to at least ten digits.
 */
static inline void logFloat2Str(f32 value, char* buf, char* fraction)
{
    s32 whole;
    s32 zeros;
    f32 scale;
    char* p;
    s32 length;

    if (value < 0.0f) {
        *buf++ = '-';
        value = -value;
    }
    whole = value;
    logInt2Str(whole, buf);
    value -= whole;
    scale = 0.1f;
    for (zeros = 0; zeros < 10; zeros++) {
        if (scale < value) {
            break;
        }
        scale /= 10.0f;
    }
    p = fraction;
    while (zeros-- != 0) {
        *p++ = '0';
    }
    value *= 1000000000.0f;
    logInt2Str((s32)value, p);
    length = strlen(fraction);
    p = fraction + length;
    while (length < 10) {
        *p++ = '0';
        length++;
    }
    *p = 0;
}

static inline s32 logStr2Int(char* s)
{
    s32 value = 0;

    while (*s != 0) {
        if (*s < '0' || *s > '9') {
            break;
        }
        value *= 10;
        value += *s++ - '0';
    }
    return value;
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
                /*
                 * Fetched into a local first: the target keeps the fetched
                 * value and copies it into the helper's (modified) parameter
                 * (lwz r0 / mr r9,r0 / neg r9,r0), as logVsnprintf_float does.
                 */
                number = va_arg(args, int);
                logInt2Str(number, lbl_80401168);
                text = lbl_80401168;
                ready = 1;
                break;
            case 'f':
                va_arg(args, double);
                text = lbl_8047CAA0;
                ready = 1;
                break;
            case 's':
                text = va_arg(args, char*);
                if (text == NULL) {
                    text = lbl_8047CAA8;
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
                /*
                 * Retail keeps `width` untouched when the text is at least
                 * as wide, so a left-justified field then pads `width`
                 * spaces after the text; preserved as-is.
                 */
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

void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args)
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
    f32 value;

    while (!done) {
        if (!parsing) {
            if (*src == '%') {
                if (src[1] == '%') {
                    *dst++ = '%';
                    src++;
                } else {
                    token = lbl_804011A8;
                    parsing = 1;
                }
            } else {
                *dst++ = *src;
            }
        } else {
            switch (*src) {
            case 'c':
                text = lbl_80401198;
                lbl_80401198[0] = va_arg(args, int);
                lbl_80401198[1] = 0;
                ready = 1;
                break;
            case 'd':
                number = va_arg(args, int);
                logInt2Str(number, lbl_80401198);
                text = lbl_80401198;
                ready = 1;
                break;
            case 'f':
                value = va_arg(args, double);
                logFloat2Str(value, lbl_80401198, lbl_80401188);
                text = lbl_80401198;
                ready = 1;
                break;
            case 's':
                text = va_arg(args, char*);
                ready = 1;
                break;
            case 'X':
            case 'x':
                hex = va_arg(args, u32);
                if (*src == 'X') {
                    logHex2Str(hex, lbl_80401198, TRUE);
                } else {
                    logHex2Str(hex, lbl_80401198, FALSE);
                }
                text = lbl_80401198;
                ready = 1;
                break;
            default:
                *token++ = *src;
                break;
            }

            if (ready == 1) {
                s32 length;

                *token = 0;
                token = lbl_804011A8;
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
                if (*src == 'f') {
                    char* dot;

                    text = lbl_80401188;
                    length = strlen(lbl_80401188);
                    dot = strchr(token, '.');
                    if (dot != NULL) {
                        width = logStr2Int(dot + 1);
                        if (width > 0 && length > width) {
                            lbl_80401188[width] = 0;
                        }
                    }
                    *dst++ = '.';
                    while (*text != 0 && (u32)(dst - output) < capacity) {
                        *dst++ = *text++;
                    }
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

