/**
 * @file math_ppc.h
 * @brief MSL <math.h> inlines as Colosseum's library code expands them:
 *        fpclassify/isnan and the PPC sqrtf (frsqrte plus three
 *        Newton-Raphson steps, NaN for negative or NaN input).
 *
 * Shape read from the retail expansions (for example robj.c
 * resolveCnsOrientation at 0x801AF3E4): x > 0.0f takes the estimate path,
 * x < 0.0 (a double compare) returns NAN, and otherwise the float's bits
 * are classified through memory and a NaN returns NAN. NAN is the MSL
 * __float_nan word, which the linker places at 0x80478AC0 in .sdata; code
 * addresses it absolutely because it is an incomplete array.
 */
#ifndef CRT_MATH_PPC_H
#define CRT_MATH_PPC_H

#include "crt/math.h"
#include "dolphin/types.h"

#define FP_NAN       1
#define FP_INFINITE  2
#define FP_ZERO      3
#define FP_NORMAL    4
#define FP_SUBNORMAL 5

#define NAN (*(f32*) lbl_80478AC0)

f64 __fabs(f64 x);
f64 acos(f64 x);
f64 cos(f64 x);
f64 sin(f64 x);

/*
 * MSL's float wrappers over the double routines. Callers expand them
 * inline: the double result is rounded (frsp) into a temporary and then
 * copied to the variable's register, the inline return-value fingerprint
 * (e.g. quatlib.c EulerToQuat at 0x801ADAAC).
 */
static inline f32 sinf(f32 x)
{
    return (f32) sin(x);
}

static inline f32 cosf(f32 x)
{
    return (f32) cos(x);
}

static inline f32 acosf(f32 x)
{
    return (f32) acos(x);
}

f64 atan2(f64 y, f64 x);

static inline f32 atan2f(f32 y, f32 x)
{
    return (f32) atan2(y, x);
}

f64 fmod(f64 x, f64 y);

/*
 * MSL's fmodf wrapper. MusyX's hw_volconv.c salCalcVolume (0x8015D7D0)
 * expands it six times with the same shape: fmr x to f1, lfd the double
 * 1.0, bl fmod, frsp into the variable's register.
 */
static inline f32 fmodf(f32 x, f32 y)
{
    return (f32) fmod(x, y);
}

f64 pow(f64 x, f64 y);

/*
 * MSL's powf wrapper. MusyX's synth_adsr.c adsrConvertTimeCents
 * (0x80158CD4) inlines it: the TU's literal pool opens with the inlined
 * body's constants (2.0 as a double, then 1.2715658e-08f) ahead of the
 * caller's own 1000.0f, which is the order MWCC gives only when the call
 * goes through an inline function.
 */
static inline f32 powf(f32 x, f32 y)
{
    return (f32) pow(x, y);
}

/*
 * fabs, __fpclassifyf, sqrtf and tanf are MSL's extern inlines: callers
 * expand them, and a call MWCC does not expand (past its inline depth)
 * goes to MSL's out-of-line copy (fabs 0x800CE59C, __fpclassifyf
 * 0x800CE718, sqrtf 0x800CE5A4, tanf 0x800CE688), as cobj.c's SetRoll
 * does for fabs and __fpclassifyf. The weak copies MWCC emits for such
 * calls are dead-stripped in favour of MSL's.
 */
inline f64 fabs(f64 x)
{
    return __fabs(x);
}

static inline f32 fabsf(f32 x)
{
    return (f32) fabs(x);
}

f64 tan(f64 x);

inline f32 tanf(f32 x)
{
    return (f32) tan(x);
}

inline s32 __fpclassifyf(f32 x)
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

#define fpclassify(x) __fpclassifyf((f32) (x))
#define isnan(x) (fpclassify(x) == FP_NAN)

/*
 * The estimate path works on a double copy of x: retail keeps the
 * frsqrte guess in its own register across all three steps and forms
 * 0.5 * guess in a separate one (robj.c 0x801AF3EC-0x801AF43C), which is
 * the allocation MWCC gives this shape.
 */
inline f32 sqrtf(f32 x)
{
    if (x > 0.0f) {
        f64 xd = x;
        f64 guess = __frsqrte(xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        guess = 0.5 * guess * (3.0 - guess * guess * xd);
        return (f32) (xd * guess);
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

#endif /* CRT_MATH_PPC_H */
