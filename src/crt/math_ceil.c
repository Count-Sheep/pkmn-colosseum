/**
 * @file math_ceil.c
 * @brief fdlibm s_ceil.c, 0x800CDA74 - 0x800CDBB8.
 */
#include "dolphin/types.h"

typedef union DoubleShape {
    f64 value;
    struct {
        u32 hi;
        u32 lo;
    } parts;
} DoubleShape;

static const f64 huge = 1.0e300;

f64 ceil(f64 x) {
    DoubleShape shape;
    s32 i0;
    s32 i1;
    s32 j0;
    u32 i;
    u32 j;

    shape.value = x;
    i0 = shape.parts.hi;
    i1 = shape.parts.lo;
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;

    if (j0 < 20) {
        if (j0 < 0) {
            if (huge + x > 0.0) {
                if (i0 < 0) {
                    i0 = 0x80000000;
                    i1 = 0;
                } else if ((i0 | i1) != 0) {
                    i0 = 0x3ff00000;
                    i1 = 0;
                }
            }
        } else {
            i = 0x000fffff >> j0;
            if (((i0 & i) | i1) == 0) {
                return x;
            }
            if (huge + x > 0.0) {
                if (i0 > 0) {
                    i0 += 0x00100000 >> j0;
                }
                i0 &= ~i;
                i1 = 0;
            }
        }
    } else if (j0 > 51) {
        if (j0 == 0x400) {
            return x + x;
        }
        return x;
    } else {
        i = 0xffffffffU >> (j0 - 20);
        if ((i1 & i) == 0) {
            return x;
        }
        if (huge + x > 0.0) {
            if (i0 > 0) {
                if (j0 == 20) {
                    i0 += 1;
                } else {
                    j = i1 + (1 << (52 - j0));
                    if (j < (u32)i1) {
                        i0 += 1;
                    }
                    i1 = j;
                }
            }
            i1 &= ~i;
        }
    }

    shape.parts.hi = i0;
    shape.parts.lo = i1;
    return shape.value;
}
