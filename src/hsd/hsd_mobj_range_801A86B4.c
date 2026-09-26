/**
 * @file hsd_mobj_range_801A86B4.c
 * @brief sysdolphin mtx.c HSD_MtxSRTQuat, 0x801A86B4 - 0x801A8884.
 *
 * Standalone source for this split range. The research copy of the wider
 * 0x801A86B4 - 0x801AA608 range, used by the neighbouring candidate
 * wrappers, lives in hsd_mobj_range_801A86B4_full.c.
 */
#include "dolphin/types.h"
#include "dolphin/mtx.h"

typedef struct HSD_Quaternion {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
} HSD_Quaternion;

/* MSL float.h: FLT_EPSILON reads __float_epsilon (.sdata 0x80478ACC) through
 * an int array, so it is addressed with lis/lfs rather than r13. */
extern int lbl_80478ACC[];
#define FLT_EPSILON (*(f32*) lbl_80478ACC)

extern void PSMTXScale(f32 m[3][4], f32 x, f32 y, f32 z);
extern void PSMTXConcat(const f32 a[3][4], const f32 b[3][4],
                        f32 result[3][4]);
extern void PSMTXQuat(f32 m[3][4], const HSD_Quaternion* quat);

extern const f32 lbl_8047DC58;
extern const f32 lbl_8047DC5C;

#define HSD_MTX_FLOAT_ONE  lbl_8047DC58
#define HSD_MTX_FLOAT_ZERO lbl_8047DC5C

/*
 * Colosseum's sysdolphin divides by the parent scale with a divisor biased
 * away from zero (HSD_MtxReciprocal, expanded three times here) and folds
 * the inverse parent scale into the rotation instead of concatenating extra
 * scale matrices. Retail scales rot[0][2] by parent y, not z.
 */
static inline f32 HSD_MtxReciprocal(f32 x)
{
    if (x >= HSD_MTX_FLOAT_ZERO) {
        return HSD_MTX_FLOAT_ONE / (x + FLT_EPSILON);
    } else {
        return HSD_MTX_FLOAT_ONE / (x - FLT_EPSILON);
    }
}

void HSD_MtxSRTQuat(f32 m[3][4], Vec* scale, HSD_Quaternion* rotate,
                    Vec* translate, Vec* parent_scale)
{
    f32 rot[3][4];

    PSMTXScale(m, scale->x, scale->y, scale->z);
    PSMTXQuat(rot, rotate);
    if (parent_scale != NULL) {
        f32 ix = HSD_MtxReciprocal(parent_scale->x);
        f32 iy = HSD_MtxReciprocal(parent_scale->y);
        f32 iz = HSD_MtxReciprocal(parent_scale->z);

        rot[0][1] *= parent_scale->y * ix;
        rot[0][2] *= parent_scale->y * ix;
        rot[1][0] *= parent_scale->x * iy;
        rot[1][2] *= parent_scale->z * iy;
        rot[2][0] *= parent_scale->x * iz;
        rot[2][1] *= parent_scale->y * iz;
    }
    PSMTXConcat(rot, m, m);
    m[0][3] = translate->x;
    m[1][3] = translate->y;
    m[2][3] = translate->z;
}
