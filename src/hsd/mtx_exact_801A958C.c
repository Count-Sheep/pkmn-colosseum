/**
 * @file mtx_exact_801A958C.c
 * @brief HAL mtx.c: HSD_MtxGetRotationMtx (fn_801A958C), 0x801A958C -
 *        0x801A98CC.
 *
 * The pure rotation of m, orthonormalised starting from axis `a`, with axis
 * `b` as the secondary one (new in Colosseum's HAL version; see
 * src/hsd/mtx.c). Body is mtx.c's, under the HSD library flags (GC/1.3.2
 * -O4,p -O1 -inline auto,deferred -use_lmw_stmw on -str reuse,readonly),
 * no local pragmas.
 *
 * Text-only unit. The 0.0f it stores is the first literal of mtx.c's float
 * pool (lbl_8047DC5C, owned by hsd_sdata2_8047DC48.c), which the other
 * mtx.c chunks also read, so it stays extern until the whole mtx.c TU links
 * (blocked by HSD_MtxGetScale's phantom f31 save; see src/hsd/mtx.c).
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"

extern void PSVECNormalize(const Vec* src, Vec* unit);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* axb);

/* RULE-EXCEPTION(title-path): named extern stand-in for mtx.c's own pooled
 * 0.0f literal - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047DC5C;

/* HSD_MtxGetRotationMtx */
void fn_801A958C(Mtx m, Mtx dst, char a, char b)
{
    Vec3 x;
    Vec3 y;
    Vec3 z;

    switch (a) {
    case 'x':
    case 'X':
        x.x = m[0][0];
        x.y = m[1][0];
        x.z = m[2][0];
        PSVECNormalize(&x, &x);
        switch (b) {
        case 'z':
        case 'Z':
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&x, &y, &z);
            break;
        default:
            y.x = m[0][1];
            y.y = m[1][1];
            y.z = m[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&z, &x, &y);
            break;
        }
        break;
    case 'y':
    case 'Y':
        y.x = m[0][1];
        y.y = m[1][1];
        y.z = m[2][1];
        PSVECNormalize(&y, &y);
        switch (b) {
        case 'x':
        case 'X':
            x.x = m[0][0];
            x.y = m[1][0];
            x.z = m[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&x, &y, &z);
            PSVECCrossProduct(&y, &z, &x);
            break;
        default:
            z.x = m[0][2];
            z.y = m[1][2];
            z.z = m[2][2];
            PSVECNormalize(&z, &z);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&x, &y, &z);
            break;
        }
        break;
    default:
        z.x = m[0][2];
        z.y = m[1][2];
        z.z = m[2][2];
        PSVECNormalize(&z, &z);
        switch (b) {
        case 'y':
        case 'Y':
            y.x = m[0][1];
            y.y = m[1][1];
            y.z = m[2][1];
            PSVECNormalize(&y, &y);
            PSVECCrossProduct(&y, &z, &x);
            PSVECCrossProduct(&z, &x, &y);
            break;
        default:
            x.x = m[0][0];
            x.y = m[1][0];
            x.z = m[2][0];
            PSVECNormalize(&x, &x);
            PSVECCrossProduct(&z, &x, &y);
            PSVECCrossProduct(&y, &z, &x);
            break;
        }
        break;
    }

    dst[0][0] = x.x;
    dst[1][0] = x.y;
    dst[2][0] = x.z;
    dst[0][1] = y.x;
    dst[1][1] = y.y;
    dst[2][1] = y.z;
    dst[0][2] = z.x;
    dst[1][2] = z.y;
    dst[2][2] = z.z;
    dst[0][3] = lbl_8047DC5C;
    dst[1][3] = lbl_8047DC5C;
    dst[2][3] = lbl_8047DC5C;
}
