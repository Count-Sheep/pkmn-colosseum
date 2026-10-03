/**
 * @file math_range_800CB2B4.c
 * @brief MSL fdlibm __ieee754_exp (e_exp.c), 0x800CB2B4 - 0x800CB4D8.
 */
#include "dolphin/types.h"

#define __HI(x) *(s32*)&x
#define __LO(x) *(1 + (s32*)&x)

static const f64 one = 1.0;
static const f64 halF[2] = { 0.5, -0.5 };
static const f64 huge = 1.0e+300;
static const f64 twom1000 = 9.33263618503218878990e-302;
static const f64 o_threshold = 7.09782712893383973096e+02;
static const f64 u_threshold = -7.45133219101941108420e+02;
static const f64 ln2HI[2] = { 6.93147180369123816490e-01, -6.93147180369123816490e-01 };
static const f64 ln2LO[2] = { 1.90821492927058770002e-10, -1.90821492927058770002e-10 };
static const f64 invln2 = 1.44269504088896338700e+00;
static const f64 P1 = 1.66666666666666019037e-01;
static const f64 P2 = -2.77777777770155933842e-03;
static const f64 P3 = 6.61375632143793436117e-05;
static const f64 P4 = -1.65339022054652515390e-06;
static const f64 P5 = 4.13813679705723846039e-08;

f64 __ieee754_exp(f64 x)
{
    f64 y;
    f64 hi;
    f64 lo;
    f64 c;
    f64 t;
    s32 k;
    s32 xsb;
    u32 hx;

    hx = __HI(x);
    xsb = (hx >> 31) & 1;
    hx &= 0x7FFFFFFF;

    if (hx >= 0x40862E42) {
        if (hx >= 0x7FF00000) {
            if (((hx & 0xFFFFF) | __LO(x)) != 0) {
                return x + x;
            } else {
                return (xsb == 0) ? x : 0.0;
            }
        }
        if (x > o_threshold) {
            return huge * huge;
        }
        if (x < u_threshold) {
            return twom1000 * twom1000;
        }
    }

    if (hx > 0x3FD62E42) {
        if (hx < 0x3FF0A2B2) {
            hi = x - ln2HI[xsb];
            lo = ln2LO[xsb];
            k = 1 - xsb - xsb;
        } else {
            k = (s32)(invln2 * x + halF[xsb]);
            t = k;
            hi = x - t * ln2HI[0];
            lo = t * ln2LO[0];
        }
        x = hi - lo;
    } else if (hx < 0x3E300000) {
        if (huge + x > one) {
            return one + x;
        }
    } else {
        k = 0;
    }

    t = x * x;
    c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    if (k == 0) {
        return one - ((x * c) / (c - 2.0) - x);
    } else {
        y = one - ((lo - (x * c) / (2.0 - c)) - hi);
    }
    if (k >= -1021) {
        __HI(y) += (k << 20);
        return y;
    } else {
        __HI(y) += ((k + 1000) << 20);
        return y * twom1000;
    }
}
