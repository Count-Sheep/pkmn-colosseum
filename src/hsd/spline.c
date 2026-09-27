/**
 * @file spline.c
 * @brief HSD splines: Hermite interpolation, spline evaluation and
 *        arc-length parameterisation.
 *
 * Retail TU: .text 0x801B1890 - 0x801B25C4, .sdata2 0x8047DE00 - 0x8047DE50.
 *
 * HAL sysdolphin spline.c, reconstructed with the Melee decompilation
 * (doldecomp/melee, src/sysdolphin/baselib/spline.c) as reference. Functions
 * appear in HAL source order; the library is built with -inline
 * auto,deferred, which emits them in reverse order.
 *
 * Colosseum's version differs from Melee's: the Simpson's-rule step of
 * splArcLengthGetParameter is a function of its own (fn_801B1AD0), called
 * once per bisection step; the square root is the later MSL sqrtf (NaN for
 * negative and NaN input); splGetSplinePoint keeps one control-point pointer
 * for the cases it reads directly and hands the curve helpers their own.
 *
 * Not linked yet: splGetSplinePoint (fn_801B2038) keeps a register-
 * allocation difference in its cardinal case, and the functions share one
 * .sdata2 literal pool that the unlinked range still references by address
 * name, so the exact functions cannot be linked on their own either. The
 * candidate unit spline_candidate_801B18D8.c scores 0x801B18D8-0x801B25C4;
 * splArcLengthPoint (no constants) is linked from spline_exact_801B1890.c.
 *
 * Linking plan once fn_801B2038 is exact: one Matching unit hsd/spline.c
 * covering .text 0x801B1890-0x801B25C4 and .sdata2 0x8047DE00-0x8047DE50
 * (replacing spline_exact_801B1890.c, spline_candidate_801B18D8.c and
 * hsd_sdata2_8047DE00.c; only this range references lbl_8047DE00-48, and
 * the compiled pool is byte-identical), on GC/1.3.2 with the tobj/mobj
 * flags (GC/1.3 through 2.7 give identical code here). lbl_8047DE48 needs
 * size 0x4 in symbols.txt: 0x8047DE4C is alignment padding before state.c's
 * 8-aligned pool, and the 0x8 size makes the .sdata2 section score 97.4%.
 *
 * Cardinal-case notes (2026-09-27): the instruction multiset is identical
 * to retail and GC/1.3-2.7, -O/-opt/-schedule variants, helper parameter
 * order, tension as a parameter or a local, cp/tension placement, u2/u3 as
 * caller values, and the bezier/B-spline helpers' declaration orders do not
 * close it. The helper's local declaration order and the order of the
 * weight assignments do move the colouring: declaring car0 first-ish and
 * assigning car1, car3, car2, car0 puts car0 in f0, tension in f2, car1 in
 * f10, car2 in f7 and car3 in f8 as retail does, but u2/u3 then land in
 * f11/f9 instead of f12/f11 (and objdiff scores that 92.8%, below this
 * source's 93.07%). An exhaustive search over all 720 declaration orders x
 * 24 assignment orders with car0 in the first two slots, plus 2,500 random
 * mixed variants, found nothing closer.
 *
 * Retail colouring of the cardinal expansion (the third inline expansion
 * in fn_801B2038, after Bezier and the first B-spline): u2 f12, u3 f11,
 * tension f2, car0 f0, car1 f10, car2 f7, car3 f8 (u stays in f1). The
 * closest source so far declares the helper's locals in the order
 * car0, u2, car3, car2, car1, u3 and assigns them u2, u3, car1, car3,
 * car2, car0. That matches retail except u2 f11 and u3 f9, so 36
 * instructions still differ, all in this case.
 * Also tried, without closing the gap:
 *   - all 720 declaration orders with the assignments in source order:
 *     best 46 instructions off, with tension in f12 and car1 in f0;
 *   - writing the Bezier or B-spline helper in place: that breaks those
 *     cases' own colouring, so both really are inline expansions;
 *   - writing the cardinal code in place: best 48 off;
 *   - changing the order the helpers are defined in, or the cardinal
 *     helper's parameter order: no effect at all.
 * The assignment order moves the colouring as well as the declaration
 * order, so the FPR colouring does not follow the saved-GPR model in the
 * shared lane preamble directly. Next step: replay the colouring with the
 * public MWCC reconstruction (github.com/JackPriceBurns/mwcc,
 * src/backend/Coloring.c), as lane FL2 did to close gs_floor (its replay
 * scripts are in the campaign scratchpad under colouring/), rather than
 * searching orders blindly.
 */
#include "hsd/hsd_spline.h"
#include "crt/math_ppc.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

/* splGetHelmite */
f32 fn_801B2560(f32 fterm, f32 time, f32 p0, f32 p1, f32 d0, f32 d1)
{
    f32 _3t2_T2;
    f32 _2t3_T3;
    f32 t3_T2;
    f32 t2_T;
    f32 t2;
    f32 _1_T2;

    _1_T2 = time * time;
    t2 = fterm * fterm;
    t2_T = _1_T2 * fterm;
    t3_T2 = t2 * (_1_T2 * time);
    _2t3_T3 = 2.0F * t3_T2 * fterm;
    _3t2_T2 = 3.0F * _1_T2 * t2;

    return (d1 * (t3_T2 - t2_T)) + ((d0 * (time + ((t3_T2 - t2_T) - t2_T))) +
                                    ((p0 * (1.0F + (_2t3_T3 - _3t2_T2))) +
                                     (p1 * (-_2t3_T3 + _3t2_T2))));
}

static inline void splGetCardinalPoint(Vec3* p, Vec3* cp, f32 tension, f32 u)
{
    f32 u2 = u * u;
    f32 u3 = u2 * u;
    f32 car0 = tension * (-u3 + 2.0F * u2 - u);
    f32 car1 = ((2.0F - tension) * u3) + ((tension - 3.0F) * u2) + 1.0F;
    f32 car2 = ((tension - 2.0F) * u3) + ((3.0F - (2.0F * tension)) * u2) +
               (tension * u);
    f32 car3 = tension * (u3 - u2);

    p->x = (cp[0].x * car0) + (cp[1].x * car1) + (cp[2].x * car2) +
           (cp[3].x * car3);
    p->y = (cp[0].y * car0) + (cp[1].y * car1) + (cp[2].y * car2) +
           (cp[3].y * car3);
    p->z = (cp[0].z * car0) + (cp[1].z * car1) + (cp[2].z * car2) +
           (cp[3].z * car3);
}

static inline void splGetBSplinePoint(Vec3* p, Vec3* cp, f32 u)
{
    f32 u2 = u * u;
    f32 u3 = u2 * u;
    f32 u_1 = 1.0F - u;
    f32 k1_6 = (1.0F / 6.0F);
    f32 b0 = k1_6 * u_1 * u_1 * u_1;
    f32 b1 = k1_6 * (4.0F + (3.0F * u3 - 6.0F * u2));
    f32 b2 = k1_6 * (3.0F * (-u3 + u2 + u) + 1.0F);
    f32 b3 = k1_6 * u3;

    p->x = (cp[0].x * b0) + (cp[1].x * b1) + (cp[2].x * b2) + (cp[3].x * b3);
    p->y = (cp[0].y * b0) + (cp[1].y * b1) + (cp[2].y * b2) + (cp[3].y * b3);
    p->z = (cp[0].z * b0) + (cp[1].z * b1) + (cp[2].z * b2) + (cp[3].z * b3);
}

static inline void splGetBezierPoint(Vec3* p, Vec3* cp, f32 u)
{
    f32 u_1 = 1.0F - u;
    f32 u2 = u * u;
    f32 u_12 = u_1 * u_1;
    f32 bez0 = u_12 * u_1;
    f32 bez1 = 3.0F * u * u_12;
    f32 bez2 = 3.0F * u2 * u_1;
    f32 bez3 = u2 * u;

    p->x = (cp[0].x * bez0) + (cp[1].x * bez1) + (cp[2].x * bez2) +
           (cp[3].x * bez3);
    p->y = (cp[0].y * bez0) + (cp[1].y * bez1) + (cp[2].y * bez2) +
           (cp[3].y * bez3);
    p->z = (cp[0].z * bez0) + (cp[1].z * bez1) + (cp[2].z * bez2) +
           (cp[3].z * bez3);
}

/* splGetSplinePoint */
void fn_801B2038(Vec3* p, HSD_Spline* spline, f32 u)
{
    Vec3* cp;
    s16 idx;
    if (u < 0.0F || u > 1.0F) {
        return;
    }

    if (u < 1.0F) {
        u = (u * (spline->numcv - 1));
        idx = u;
        u = u - (f32) idx;
        switch (spline->type) {
        case 0:
            cp = &spline->cv[idx];
            p->x = (u * (cp[1].x - cp[0].x)) + cp[0].x;
            p->y = (u * (cp[1].y - cp[0].y)) + cp[0].y;
            p->z = (u * (cp[1].z - cp[0].z)) + cp[0].z;
            return;
        case 1:
            splGetBezierPoint(p, &spline->cv[idx * 3], u);
            return;
        case 2:
            splGetBSplinePoint(p, &spline->cv[idx], u);
            return;
        case 3:
            splGetCardinalPoint(p, &spline->cv[idx], spline->tension, u);
            return;
        }
    } else {
        idx = spline->numcv - 1;
        switch (spline->type) {
        case 0:
            cp = &spline->cv[idx];
            *p = *cp;
            return;
        case 1:
            cp = &spline->cv[idx * 3];
            *p = *cp;
            return;
        case 2:
            splGetBSplinePoint(p, &spline->cv[idx] - 1, 1.0F);
            return;
        case 3:
            cp = &spline->cv[idx] + 1;
            *p = *cp;
            return;
        }
    }
}

static inline f32 splArcLengthPolynomial(const f32 coeffs[5], f32 t)
{
    f32 t2 = t * t;
    f32 t3 = t2 * t;
    f32 t4 = t3 * t;
    f32 result = (coeffs[0] * t4) + (coeffs[1] * t3) + (coeffs[2] * t2) +
                 (coeffs[3] * t) + coeffs[4];

    if ((result < 0.0F) && (result > -0.001F)) {
        result = 0.0F;
    }

    return sqrtf(result);
}

/* Simpson's rule over [start, end] of one segment's arc-length integrand. */
f32 fn_801B1AD0(const f32 coeffs[5], f32 start, f32 end)
{
    f32 dx = (end - start) / 8.0F;
    f32 t = start + dx;
    f32 middle = 0.0F;
    s32 i;

    for (i = 2; i <= 8; ++i) {
        if (!(i & 1)) {
            middle += 4.0F * splArcLengthPolynomial(coeffs, t);
        } else {
            middle += 2.0F * splArcLengthPolynomial(coeffs, t);
        }
        t += dx;
    }
    return dx *
           (middle + splArcLengthPolynomial(coeffs, start) +
            splArcLengthPolynomial(coeffs, end)) /
           3.0F;
}

/* splArcLengthGetParameter */
f32 fn_801B18D8(HSD_Spline* spl, f32 arg1)
{
    s32 idx = 0;
    f32 start = 0.0F;
    f32 end = 1.0F;
    f32 result;

    if (arg1 <= 0.0F) {
        return 0.0F;
    }

    if (arg1 >= 1.0F) {
        return 1.0F;
    }

    while (spl->segLength[idx + 1] < arg1) {
        idx++;
    }

    switch (spl->type) {
    case 0: {
        result = (arg1 - spl->segLength[idx]) /
                 (spl->segLength[idx + 1] - spl->segLength[idx]);
    } break;
    case 1:
    case 2:
    case 3: {
        f32 var_f22 = spl->totalLength * (arg1 - spl->segLength[idx]);

        while (ABS(start - end) >= 0.00001F) {
            f32 simpsons;

            result = (start + end) / 2.0F;
            simpsons = fn_801B1AD0(spl->segPoly[idx], start, result);
            if (var_f22 < (0.00001F + simpsons)) {
                end = result;
            } else {
                start = result;
                var_f22 -= simpsons;
            }
        }
    } break;
    }

    return (result + idx) / (spl->numcv - 1.0F);
}

void splArcLengthPoint(Vec3* p, HSD_Spline* spline, f32 u)
{
    fn_801B2038(p, spline, fn_801B18D8(spline, u));
}
