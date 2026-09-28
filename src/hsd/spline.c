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
 * shared lane preamble directly.
 *
 * Replay (lane SP1, 2026-09-27). The compiler internals were dumped with
 * cadmic/mwcc-debugger (GC/2.6 profile; GC/2.6 emits the same code as
 * GC/1.3.2 for this file) and two exact models were fitted and checked:
 *   - Scheduling: at these flags only the pre-allocation pass reorders; the
 *     post-allocation pass leaves every block unchanged, so the final order
 *     is the order the allocator colours. The pre-allocation pass is the
 *     list scheduler in JackPriceBurns/mwcc docs/SCHEDULER.md, with a
 *     6-entry in-flight ring and the deadline base equal to the block's own
 *     maximum height. It reproduces all 15 dumped blocks exactly.
 *   - FPR colouring: Coloring_SimplifyGraph with K = 32. The scan goes up
 *     the virtual registers, and every node that neighbours the parameter
 *     u (f32) also counts f1, because a coalesced node stays in the graph.
 *     Selection then takes the lowest free colour. This reproduces every
 *     colour in 12 dumped variants. The helper's objects are numbered
 *     tension first, then its locals in declaration order, and all of them
 *     come below the lowering temps, so they are coloured last unless their
 *     degree is 32 or more.
 * What the models show about retail:
 *   - Retail's cardinal order is not a schedule of this source's dependence
 *     graph. No lowering order of the same graph comes within 29 of the 58
 *     positions (hill-climbed). Neither do the assignment/declaration
 *     orders, operand orders, the helper's argument order, a tension or u
 *     copy, or extra latency. One extra dead FP instruction gets to 19,
 *     and two get to 17. From cycle 0 (tension issued ahead of extsh), the
 *     retail decisions need a different graph.
 *   - Colouring retail's own order: B (2.0F - tension), c1 (the 2.0F for
 *     2.0F * u2) and D (-u3) sit one register higher than the lowest free
 *     one (f7/f9/f8 where f6/f8/f7 are free). The only fit is a hidden FP
 *     value that holds f6 at the fnmsubs (3.0F - 2.0F * tension), is
 *     removed after allocation, and is numbered among car1's lowering
 *     temps (after B, before C). The locals must also be numbered with car0
 *     below u3, e.g. u2, car0, u3, car1, car2, car3. With both, every retail
 *     register comes out.
 *   - A dead local (object) cannot be that value, because it is coloured
 *     after the temps. Expression statements are dropped by the front end.
 *     Dead locals do change the schedule: `f32 x = u2;` reproduces retail's
 *     first 15 instructions but not the rest.
 * So retail's helper performs one more FP computation than this source. It
 * is a temp that is dead or deleted after allocation, created while car1
 * is lowered. Its source form is not identified. The coordinator ruled that
 * no dead or unused computation may be added.
 *
 * Forms tried after that ruling (real compiler, compared with retail):
 *   - Weights in coefficient-first form with several term orders,
 *     coefficient-last and Horner forms, 2.0F * tension, tension * 2.0F
 *     and tension + tension, and 3.0F - (tension + tension).
 *   - Named coefficient locals (2 - t, t - 3, t - 2, 3 - 2t, u3 - u2) in
 *     random statement and declaration orders (400 samples), with and
 *     without the caller's cp variable.
 *   - Accumulate forms such as `car3 = tension; car3 *= u3 - u2;` in every
 *     weight order.
 *   None reproduces retail's instruction order. Forms that keep the same
 *   dependence graph cannot change the schedule. Only 2t, 2.0F * tension
 *   and fmsubs-producing forms change the instructions themselves.
 * Model result: the nearest dependence graph (16 of 58 positions off) adds
 * a copy of tension that feeds only car3's multiply. Real accumulate forms
 * give that shape (sdiff 18) but keep the fmr in the final code, because
 * the copy's live range meets tension's.
 * Exact but rejected (lane SP1, 2026-09-27): reading tension through a
 * local copy in splGetCardinalPoint (`f32 t = tension;`, needed at least on
 * car1's (2.0F - t) and (t - 3.0F)) matches fn_801B2038 byte for byte. The
 * coalescer deletes the copy's fmr, but the copy stays in the graph the
 * first scheduling pass sees, so the tension load issues at cycle 0 and
 * retail's registers follow. The copy's only effect is scheduling and
 * register numbering, which CAMPAIGN_OPERATIONS.md forbids ("temporaries
 * used only to manipulate allocation or scheduling"), so it is not applied.
 * Passing tension into an inline weight helper does not create the copy: the
 * inliner substitutes a real function's parameters and locals directly and
 * copies only an inlined function's own locals.
 * Dump PCode stage 00 of a candidate against retail with the scratchpad
 * sp1 tools (dbg/run.py, hid.py, greedy.py) before compiling blind
 * variants.
 *
 * Lane B15 (2026-09-28), sister-build evidence and more forms.  Counts are
 * normalised instruction diff lines against retail; this source scores 106.
 *   - Naruto GNT4 (doldecomp/gnt4 b6c32473, asm/sysdolphin/spline.s,
 *     splGetSplinePoint 0x801C4F78, 292 instructions) is instruction-for-
 *     instruction this file's splGetSplinePoint built with GC/2.5 -O4,p
 *     -inline auto; only the psq_st/psq_l register spelling differs.  That
 *     confirms the HAL source, including the cardinal helper as written
 *     here.  At -O4 the tension load issues right after u*u, as in
 *     Colosseum, but -O4 also CSEs the constants (292 instructions against
 *     retail's 330).  Colosseum's HSD build is the -O1 build.  Every
 *     -opt level=2/3/4 variant (with nocse, noprop, nolifetimes, nodead,
 *     nostrength, noloop, nopeep or -schedule on) gives 292-304 instructions.
 *     Every level=1 variant gives the same 106.  At -O1, GC/1.0-1.2.5n give
 *     329 instructions, 1.3-2.7 give 106 (2.0p1 108), and 3.0a gives 450.
 *   - Melee (doldecomp/melee 70ee84f) has the same helpers.  Its caller form
 *     (`f32 t = u * (numcv - 1)` instead of reusing u) scores 244.  Adding
 *     Melee's forward prototypes, or making the helpers plain `static` (auto-
 *     inlined), changes nothing.
 *   - Pokemon XD JP demo linker map (NXXJ01.map, StarsMmd/Colo-XD-PBR-
 *     symbol-maps 6b51d3af; a primary map with stripped "UNUSED" entries),
 *     hsdbase_gs.a ...\GSsysdolphin_baselib\baselib\spline.o:
 *     splArcLengthPoint 0x48, splArcLengthGetParameter 0x200, splArcLength
 *     0x508, splArcIntegrand 0x13C (UNUSED), splGetSplinePoint 0x490,
 *     splGetBezierPoint 0x9C, splGetBSplinePoint 0xDC and splGetCardinalPoint
 *     0xDC (all three UNUSED), splGetHelmite 0x60.  This source, with the
 *     three point helpers and the polynomial (as splArcIntegrand) made
 *     global, gives exactly these nine sizes with GC/1.3.2-2.6 -O4,p.  With
 *     the unit's -O1 it gives Colosseum's sizes (0x48, 0x1F8, 0x568, 0x528,
 *     0x64).  So the helpers were global functions that were auto-inlined
 *     and then dead-stripped; making them global does not change fn_801B2038
 *     (still 106).  Map names: fn_801B18D8 = splArcLengthGetParameter,
 *     fn_801B1AD0 = splArcLength (not a new Simpson helper), fn_801B2038 =
 *     splGetSplinePoint, fn_801B2560 = splGetHelmite.  These are not renamed
 *     here, because the recomp boot manifest names fn_801B2038.
 *     -inline auto/on/smart/all/deferred/level and -proc 750/740 give the
 *     same 106; the other -proc values give 224-264.
 *   - wowjinxy/KAR (cb94b157) src/sysdolphin/spline.c is NonMatching and is
 *     written with __fmadds intrinsics, so it is not source evidence.
 *   - Also tried: the helper taking the HSD_Spline* and reading
 *     spline->tension at every use (336 instructions), or into a local
 *     (106), or into a local declared after u3 (102); the cardinal code in
 *     place with spline->tension repeated (336); a K&R definition (332, two
 *     frsp); an f64 tension (334); const/register parameters (106); double
 *     constants in car0/car1/car2 (they add lfd/fadd/frsp; retail has only
 *     single-precision constants and operations here).
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
