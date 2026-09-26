#ifndef CRT_MATH_PPC_H
#define CRT_MATH_PPC_H

/*
 * The MSL inline float square root as Colosseum's libraries expand it: three
 * Newton steps from frsqrte for positive input, NaN for negative input and
 * NaN, the input itself otherwise (zero, infinity).
 */

#include "crt/math.h"

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline int __fpclassifyf(f32 x)
{
    switch ((*(s32*) &x) & 0x7F800000) {
    case 0x7F800000:
        if ((*(s32*) &x) & 0x007FFFFF) {
            return FP_NAN;
        } else {
            return FP_INFINITE;
        }
        break;
    case 0:
        if ((*(s32*) &x) & 0x007FFFFF) {
            return FP_SUBNORMAL;
        } else {
            return FP_ZERO;
        }
        break;
    }
    return FP_NORMAL;
}

static inline f32 sqrtf(f32 x)
{
    if (x > 0.0F) {
        f64 xd = x;
        f64 guess = __frsqrte(xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        return (f32) (xd * guess);
    } else if ((f64) x < 0.0) {
        return lbl_80478AC0[0];
    } else if (__fpclassifyf(x) == FP_NAN) {
        return lbl_80478AC0[0];
    }
    return x;
}

#endif /* CRT_MATH_PPC_H */
