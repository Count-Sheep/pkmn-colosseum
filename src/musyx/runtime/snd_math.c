/**
 * @file snd_math.c
 * @brief MusyX 3D vector/matrix helpers, 0x8015FFDC - 0x8016039C.
 *
 * Follows the reference MusyX runtime's snd_math.c (AxioDL/musyx). The TU
 * owns its literal pool (.sdata2 0x8047D4B8 - 0x8047D4D8).
 *
 * Built with -fp_contract off: retail keeps every multiply and add separate
 * (salApplyMatrix 0x8015FFDC: fmuls/fadds, no fmadds), and all four
 * functions match with that one unit-wide setting.
 */
#include "dolphin/types.h"
#include "crt/math.h"

typedef struct SND_FVECTOR {
    f32 x;
    f32 y;
    f32 z;
} SND_FVECTOR;

typedef struct SND_FMATRIX {
    f32 m[3][3];
    f32 t[3];
} SND_FMATRIX;

/*
 * The MSL sqrtf of MusyX's library build: three Newton-Raphson steps on
 * the frsqrte estimate, the result stored through a volatile float
 * (retail salNormalizeVector 0x80160110 stfs/lfs round trip), and no NaN
 * handling. It differs from the later MSL copy in crt/math_ppc.h.
 */
static inline f32 sqrtf(f32 x)
{
    volatile f32 y;

    if (x > 0.0f) {
        f64 guess = __frsqrte((f64) x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        y = (f32) (x * guess);
        return y;
    }
    return x;
}

void salApplyMatrix(const SND_FMATRIX* mat, const SND_FVECTOR* in, SND_FVECTOR* out)
{
    out->x = mat->m[0][0] * in->x + mat->m[0][1] * in->y + mat->m[0][2] * in->z + mat->t[0];
    out->y = mat->m[1][0] * in->x + mat->m[1][1] * in->y + mat->m[1][2] * in->z + mat->t[1];
    out->z = mat->m[2][0] * in->x + mat->m[2][1] * in->y + mat->m[2][2] * in->z + mat->t[2];
}

f32 salNormalizeVector(SND_FVECTOR* vec)
{
    f32 l = sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);

    vec->x /= l;
    vec->y /= l;
    vec->z /= l;
    return l;
}

void salCrossProduct(SND_FVECTOR* out, const SND_FVECTOR* a, const SND_FVECTOR* b)
{
    out->x = (a->y * b->z) - (a->z * b->y);
    out->y = (a->z * b->x) - (a->x * b->z);
    out->z = (a->x * b->y) - (a->y * b->x);
}

void salInvertMatrix(SND_FMATRIX* out, const SND_FMATRIX* in)
{
    f32 a;
    f32 b;
    f32 c;
    f32 f;

    a = in->m[1][1] * in->m[2][2] - in->m[2][1] * in->m[1][2];
    b = -(in->m[1][0] * in->m[2][2] - in->m[2][0] * in->m[1][2]);
    c = in->m[1][0] * in->m[2][1] - in->m[2][0] * in->m[1][1];
    f = 1.0f / (in->m[0][0] * a + in->m[0][1] * b + in->m[0][2] * c);
    out->m[0][0] = f * a;
    out->m[1][0] = f * b;
    out->m[2][0] = f * c;
    out->m[0][1] = -f * (in->m[0][1] * in->m[2][2] - in->m[2][1] * in->m[0][2]);
    out->m[1][1] = f * (in->m[0][0] * in->m[2][2] - in->m[2][0] * in->m[0][2]);
    out->m[2][1] = -f * (in->m[0][0] * in->m[2][1] - in->m[2][0] * in->m[0][1]);
    out->m[0][2] = f * (in->m[0][1] * in->m[1][2] - in->m[1][1] * in->m[0][2]);
    out->m[1][2] = -f * (in->m[0][0] * in->m[1][2] - in->m[1][0] * in->m[0][2]);
    out->m[2][2] = f * (in->m[0][0] * in->m[1][1] - in->m[1][0] * in->m[0][1]);
    out->t[0] = (-in->t[0] * out->m[0][0] - in->t[1] * out->m[0][1]) - in->t[2] * out->m[0][2];
    out->t[1] = (-in->t[0] * out->m[1][0] - in->t[1] * out->m[1][1]) - in->t[2] * out->m[1][2];
    out->t[2] = (-in->t[0] * out->m[2][0] - in->t[1] * out->m[2][1]) - in->t[2] * out->m[2][2];
}
