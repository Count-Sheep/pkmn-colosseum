/**
 * @file msgctrl_candidate_80132834.c
 * @brief Number formatting helper at 0x80132834 - 0x80132A38
 *        (_msgctrlMakeDigit).
 */
#include "dolphin/types.h"

/*
 * Formats value backwards into a capacity-character u16 buffer, terminates
 * it at capacity - 1, and returns a pointer to the first emitted glyph.
 * Types 0/5 are signed, 2 is ten-digit decimal, 3 is eight-digit hexadecimal,
 * 4 inserts thousands separators, and 5 selects the alternate full-width
 * glyph table. Mode 5 still uses the ordinary minus sign. As in retail, the
 * caller must provide enough capacity; there is no clipping check.
 *
 * This helper retains C++ linkage. Its optimizer settings are translation-unit
 * flags in configure.py: -opt nopeephole -schedule on.
 */
#pragma push
#pragma cplusplus on
u16* _msgctrlMakeDigit(u16* output, s32 length, u32 value, s32 type)
{
    extern u16 lbl_803635F0[];
    extern u16 lbl_80363610[];
    u16* table = output;
    u8 negative;
    s32 radix;
    s32 digit_count;
    s32 width;
    u16* digits;
    s32 index;
    u32 current;

    index = length - 1;
    table[index] = 0;
    negative = 0;
    radix = 10;
    digit_count = 0;
    width = 1;

    switch (type) {
    case 0:
    case 5:
        current = value;
        /* The signed view keeps the retail stack-backed parameter access;
         * subtraction remains unsigned, including for 0x80000000. */
        if (*(s32*)&value < 0) {
            current = 0U - current;
            value = current;
            negative = 1;
        }
        break;
    case 2:
        width = 10;
        break;
    case 3:
        radix = 16;
        width = 8;
        break;
    }

    switch (type) {
    case 5:
        digits = lbl_80363610;
        break;
    default:
        digits = lbl_803635F0;
        break;
    }
    while (value != 0) {
        if (type == 4 && digit_count != 0 && digit_count % 3 == 0) {
            table[--index] = ',';
        }
        table[--index] = digits[value % (u32)radix];
        value /= (u32)radix;
        digit_count++;
    }

    while (digit_count < width) {
        table[--index] = digits[0];
        digit_count++;
    }

    if (negative) {
        table[--index] = '-';
    }
    return &table[index];
}
#pragma pop
