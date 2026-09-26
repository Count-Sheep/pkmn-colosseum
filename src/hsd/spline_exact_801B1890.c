/**
 * @file spline_exact_801B1890.c
 * @brief splArcLengthPoint, the first function after shadow.c
 *        (0x801B1890 - 0x801B18D8).
 *
 * Split from hsd_texp_exact_801B1854.c when shadow.c (which ends at
 * 0x801B1890 with its alloc-data helpers) became its own translation unit;
 * the code is unchanged.
 */
#include "dolphin/types.h"

typedef struct SplineVec3 {
    f32 x;
    f32 y;
    f32 z;
} SplineVec3;

typedef struct HSD_Spline {
    u8 type;
    u8 pad_01;
    s16 numcv;
    f32 tension;
    SplineVec3* cv;
    f32 totalLength;
    f32* segLength;
    f32 (*segPoly)[5];
} HSD_Spline;

extern f32 fn_801B18D8(HSD_Spline* spline, f32 distance);
extern void fn_801B2038(SplineVec3* point, HSD_Spline* spline, f32 value);

void splArcLengthPoint(SplineVec3* point, HSD_Spline* spline, f32 distance)
{
    fn_801B2038(point, spline, fn_801B18D8(spline, distance));
}
