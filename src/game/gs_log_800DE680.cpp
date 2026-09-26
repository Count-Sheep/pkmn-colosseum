/**
 * logVsnprintf_float (0x800DE680-0x800DEFC8): GSlogWritef's formatter, the
 * last function of the GSlog translation unit (see gs_log.cpp). Candidate,
 * not linked.
 *
 * Supported conversions: %c %d %s %x %X and %f, %%, with the '-' and '0'
 * flags, a decimal width and, for %f, a '.precision' that truncates the
 * fraction digits. %f goes through logFloat2Str: sign
 * and integer part into the field buffer, then up to ten leading fraction
 * zeros and (s32)(fraction * 1e9) into the fraction buffer, padded with '0'
 * to ten digits.
 *
 * Source attribution: the XD path GSAPI/GSlogM/GSlog.cpp and the XD helper
 * names (logFloat2Str, logHex2Str, logInt2Str, logStr2Int, logStrRev) are
 * inherited from commit e0e2b44b, which read TeamOrre/xd-decomp's
 * splits.txt; they are not verified here (no XD symbol data in this repo).
 * That the TU is C++ rests on the build evidence below, not on that path.
 *
 * TU evidence (C++, statics in declaration order): the function addresses its
 * three buffers from the TU's .bss base, lbl_80400F30 + 0x258/0x268/0x278 =
 * fraction, field, spec in declaration order. Compiled as one C++ file with
 * gs_log.cpp's functions (all eight in address order, every static a
 * function-local static, no extra flags), MWCC reproduces the whole retail
 * .bss block, .rodata pool, .sdata, .sbss, .sdata2 and both switch tables at
 * their retail offsets, and the other seven functions exactly. The C front
 * end instead orders statics by first reference (the unused fraction buffer
 * of fn_800DE128 goes last) and the .sbss state in reverse, and only
 * reproduces the layout with -inline deferred and the file reversed.
 *
 * What is left (GC/1.3.2 through GC/2.7 give the same code): one register
 * choice. The fraction cursor `p` in logFloat2Str (retail r18, shared with
 * `whole`, the width and the logStrRev start pointers) is coloured r27 (the
 * text/field pointer's register) here; the second logInt2Str expansion that
 * consumes it then takes its other register pattern (cursor r4 instead of
 * r3, value r5 instead of r9, and so on). In the whole-TU build 48 of 594
 * instructions differ, all in 0x800DEA40-0x800DEB74, and only in register
 * numbers. (Built alone, as here, the statics sit at this object's own .bss
 * base, so the buffer offsets differ as well.) Variants
 * tried without success: the cursor as a helper local, the helper's `buf`
 * or `fraction` parameter, a local shared with the integer part, index
 * loops, the padding and zero-count loop forms, declaration orders of every
 * local, helper parameter orders and definition orders, static vs inline
 * helpers, the fraction buffer as a file-scope static, the 'f' case written
 * out without a helper, and case orders. Making the cursor the integer
 * part's cursor too (or reusing `buf`) does give r18 here, but then the
 * three parameters are coloured after the loop state instead of first.
 *
 * Once it matches, this file merges back into gs_log.cpp with the buffers as
 * function-local statics and gs_log.cpp takes the whole .bss block.
 */
#include "dolphin/types.h"
#include "game/gs_log_format.h"

extern "C" {
char* strchr(const char* s, int c);
void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args);
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

void logVsnprintf_float(char* output, u32 capacity, const char* format, va_list args)
{
    static char fraction[16];
    static char field[16];
    static char spec[16];
    const char* src = format;
    char* dst = output;
    char* token;
    u8 done = 0;
    u8 parsing = 0;
    u8 ready = 0;
    u8 leftJustify = 0;
    u8 zeroPad = 0;
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
                    token = spec;
                    parsing = 1;
                }
            } else {
                *dst++ = *src;
            }
        } else {
            switch (*src) {
            case 'c':
                text = field;
                field[0] = va_arg(args, int);
                field[1] = 0;
                ready = 1;
                break;
            case 'd':
                number = va_arg(args, int);
                logInt2Str(number, field);
                text = field;
                ready = 1;
                break;
            case 'f':
                value = va_arg(args, double);
                logFloat2Str(value, field, fraction);
                text = field;
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
                    logHex2Str(hex, field, TRUE);
                } else {
                    logHex2Str(hex, field, FALSE);
                }
                text = field;
                ready = 1;
                break;
            default:
                *token++ = *src;
                break;
            }

            if (ready == 1) {
                s32 length;

                *token = 0;
                token = spec;
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
                    const char* dot;

                    text = fraction;
                    length = strlen(fraction);
                    dot = strchr(token, '.');
                    if (dot != NULL) {
                        width = logStr2Int(dot + 1);
                        if (width > 0 && length > width) {
                            text[width] = 0;
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
