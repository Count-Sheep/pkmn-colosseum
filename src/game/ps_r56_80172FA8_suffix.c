/**
 * @file ps_r56_80172FA8_suffix.c
 * @brief HAL psinterpret.c: modifyDir, 0x80172FA8 - 0x801732A0.
 *
 * Function-boundary carve of psinterpret.c (see src/game/psinterpret.c for
 * the TU extent and the body this copies). No jump table and no data of its
 * own; it reads the float-NaN word (lbl_80478AC0, MSL's) and FLT_MIN, and
 * entries of psinterpret.c's .sdata2 pool: 0.0f (0x8047D630), the inlined
 * MSL sqrtf's 0.5, 3.0 and 0.0 doubles (0x8047D650, 0x8047D680,
 * 0x8047D688), the f32 +/-pi/2 (0x8047D694, 0x8047D698), 2.0 and pi
 * (0x8047D6A0, 0x8047D6A8). That pool is shared with psInterpretParticle0,
 * which is not exact yet, and is linked as game/data/sdata2_8047D630.c and
 * sdata2_8047D690.c, so this carve refers to the entries by their pool names
 * instead of emitting its own literals; sqrtf below is math_ppc.h's body
 * with those names. modifyDirGenBase (0x801732A0) stays in the candidate
 * chunk ps_candidate_801732A0.c.
 *
 * Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

/* math_ppc.h's own sqrtf is set aside (renamed, never used) for the copy
 * below that reads the pooled constants by name. */
#define sqrtf math_ppc_sqrtf
#include "crt/math_ppc.h"
#undef sqrtf

/* RULE-EXCEPTION(title-path): named stand-ins for psinterpret.c's pooled
 * literals (0.0f, sqrtf's 0.5/3.0/0.0, +/-pi/2, 2.0 and pi) - see
 * docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D630;     /* 0.0f */
extern const f64 lbl_8047D650;     /* 0.5 */
extern const f64 lbl_8047D680;     /* 3.0 */
extern const f64 lbl_8047D688;     /* 0.0 */
extern const f32 lbl_8047D694;     /* (f32) M_PI_2 */
extern const f32 lbl_8047D698;     /* (f32) -M_PI_2 */
extern const f64 lbl_8047D6A0;     /* 2.0 */
extern const f64 lbl_8047D6A8;     /* M_PI */

#include "crt/float.h"

extern f32 fn_801ADC7C(void); /* HSD_Randf */

/* MSL's sqrtf (crt/math_ppc.h) with the pooled constants named. */
inline f32 sqrtf(f32 x)
{
    if (x > lbl_8047D630) {
        f64 xd = x;
        f64 guess = __frsqrte(xd);
        guess = lbl_8047D650 * guess * (lbl_8047D680 - guess * guess * xd);
        guess = lbl_8047D650 * guess * (lbl_8047D680 - guess * guess * xd);
        guess = lbl_8047D650 * guess * (lbl_8047D680 - guess * guess * xd);
        return (f32) (xd * guess);
    } else if (x < lbl_8047D688) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}

/* Turns the velocity by a random direction on a cone of half-angle `angle`
 * around its current direction. */
void modifyDir(HSD_Particle* pp, f32 angle)
{
    f32 vx;
    f32 vy;
    f32 vz;
    f32 v;
    f32 sx;
    f32 cx;
    f32 sy;
    f32 cy;
    f32 len;
    f32 rnd;
    f32 ry;
    f32 rx;
    f32 r;
    f32 u;
    f32 w;

    vx = pp->vel.x;
    vy = pp->vel.y;
    vz = pp->vel.z;
    if (fabs(vz) < FLT_MIN) {
        rx = vy >= lbl_8047D630 ? lbl_8047D694 : lbl_8047D698;
    } else {
        rx = atan2f(vy, vz);
    }
    sx = sinf(rx);
    cx = cosf(rx);
    w = vy * sx + vz * cx;
    if (fabs(w) < FLT_MIN) {
        ry = vx >= lbl_8047D630 ? lbl_8047D694 : lbl_8047D698;
    } else {
        ry = atan2f(vx, w);
    }
    sy = sinf(ry);
    cy = cosf(ry);
    len = sqrtf(vx * vx + vy * vy + vz * vz);
    rnd = lbl_8047D6A0 * (lbl_8047D6A8 * fn_801ADC7C());
    r = len * sinf(angle);
    u = r * cosf(rnd);
    v = r * sinf(rnd);
    w = len * cosf(angle);
    pp->vel.x = u * cy + w * sy;
    pp->vel.y = sy * (-u * sx) + v * cx + cy * (w * sx);
    pp->vel.z = sy * (-u * cx) - v * sx + cy * (w * cx);
}
