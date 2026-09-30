/**
 * @file math_range_800CE378.c
 * @brief libm __ieee754_sqrt (0x800CE378 - 0x800CE59C) with its literal.
 *
 * Function-boundary carve of the MSL/fdlibm math TU, built at the TU's
 * GC/1.3 -O4,p. fdlibm's `one - tiny` / `one + tiny` rounding tests fold at
 * compile time, leaving the single 1.0 literal, which this unit owns
 * (.sdata2 0x8047C968-0x8047C970). The same body is in math_range_800CAA58.c.
 */
#include "dolphin/types.h"

extern s32 lbl_8047AA10;          /* errno */
extern const f32 lbl_80478AC0[];  /* __float_nan */

/*
 * fdlibm e_sqrt.c, bit-by-bit integer square root.  The MSL variant sets
 * errno to EDOM (33) for non-finite and negative cases.
 */
f64 __ieee754_sqrt(f64 x) {
#define SQRT_ONE  1.00000000000000000000e+00
#define SQRT_TINY 1.00000000000000002506e-300
    f64 z;
    u32 sign = 0x80000000;
    u32 r;
    u32 t1;
    u32 s1;
    u32 ix1;
    u32 q1;
    s32 ix0;
    s32 s0;
    s32 q;
    s32 m;
    s32 t;
    s32 i;

    ix0 = *(s32*)&x;
    ix1 = *((u32*)&x + 1);

    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        lbl_8047AA10 = 33;
        return x * x + x;
    }

    if (ix0 <= 0) {
        if (((ix0 & ~sign) | ix1) == 0) {
            return x;
        }
        if (ix0 < 0) {
            lbl_8047AA10 = 33;
            return lbl_80478AC0[0];
        }
    }

    m = ix0 >> 20;
    if (m == 0) {
        while (ix0 == 0) {
            m -= 21;
            ix0 |= ix1 >> 11;
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++) {
            ix0 <<= 1;
        }
        m -= i - 1;
        ix0 |= ix1 >> (32 - i);
        ix1 <<= i;
    }

    m -= 1023;
    ix0 = (ix0 & 0x000fffff) | 0x00100000;
    if (m & 1) {
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1;

    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0;
    r = 0x00200000;

    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;
    while (r != 0) {
        t1 = s1 + r;
        t = s0;
        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1 = t1 + r;
            if (((t1 & sign) == sign) && (s1 & sign) == 0) {
                s0 += 1;
            }
            ix0 -= t;
            if (ix1 < t1) {
                ix0 -= 1;
            }
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    if ((ix0 | ix1) != 0) {
        z = SQRT_ONE - SQRT_TINY;
        if (z >= SQRT_ONE) {
            z = SQRT_ONE + SQRT_TINY;
            if (q1 == 0xffffffff) {
                q1 = 0;
                q += 1;
            } else if (z > SQRT_ONE) {
                if (q1 == 0xfffffffe) {
                    q += 1;
                }
                q1 += 2;
            } else {
                q1 += (q1 & 1);
            }
        }
    }

    ix0 = (q >> 1) + 0x3fe00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1) {
        ix1 |= sign;
    }
    ix0 += (m << 20);
    *(s32*)&z = ix0;
    *((u32*)&z + 1) = ix1;
    return z;
#undef SQRT_ONE
#undef SQRT_TINY
}
