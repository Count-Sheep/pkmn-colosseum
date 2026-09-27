/**
 * @file gs_render_util_exact_800D2DE8.c
 * @brief GS camera screen projection, 0x800D2DE8 - 0x800D305C.
 *
 * A function-boundary carve of the GS render-utility TU on its flags
 * (GC/1.3, project defaults, no pragmas). Both functions project points
 * through the active camera with GXProject: fn_800D7BF8(0) is the view
 * matrix and fn_800D7BF8(2) the 4x4 projection, from which they build
 * GXProject's projection parameters (perspective: type 0, p00, p02, p11,
 * p12, p22, p23) and a fixed viewport. The .sdata2 constants they read
 * (0x8047CA00 - 0x8047CA0C) stay extern.
 */
#include "dolphin/types.h"

typedef f32 Mtx[3][4];
typedef f32 Mtx44[4][4];

extern void* fn_800D7BF8(u32 index);
extern void GSvecTransform(f32* out, Mtx m, const f32* in);
extern void GXProject(f32 x, f32 y, f32 z, Mtx mtx, const f32* pm,
                      const f32* vp, f32* sx, f32* sy, f32* sz);

extern const f32 lbl_8047CA00;
extern const f32 lbl_8047CA04;
extern const f32 lbl_8047CA08;
extern const f32 lbl_8047CA0C;

/* 0x800D2DE8 | 0x14C */
s32 fn_800D2DE8(f32* points, f32* screen, u32 count)
{
    f32 projection[7];
    f32 viewport[6];
    f32 view[3];
    Mtx44* proj;
    Mtx* viewMtx;
    u32 i;
    f32* src;
    f32* dst;
    s32 result;

    src = points;
    dst = screen;
    viewMtx = fn_800D7BF8(0);
    proj = fn_800D7BF8(2);
    if (viewMtx == NULL || proj == NULL) {
        return 0;
    }

    projection[0] = lbl_8047CA00;
    projection[1] = (*proj)[0][0];
    projection[2] = (*proj)[0][2];
    projection[3] = (*proj)[1][1];
    projection[4] = (*proj)[1][2];
    projection[5] = (*proj)[2][2];
    projection[6] = (*proj)[2][3];
    viewport[0] = lbl_8047CA00;
    viewport[1] = lbl_8047CA00;
    viewport[2] = lbl_8047CA04;
    viewport[3] = lbl_8047CA08;
    viewport[4] = lbl_8047CA00;
    viewport[5] = lbl_8047CA0C;

    result = 2;
    for (i = 0; i < count; i++, src += 3, dst += 3) {
        GSvecTransform(view, *viewMtx, src);
        if (view[2] >= (*proj)[2][3]) {
            result = 3;
            dst[0] = dst[1] = dst[2] = lbl_8047CA0C;
        } else {
            GXProject(src[0], src[1], src[2], *viewMtx, projection,
                      viewport, &dst[0], &dst[1], &dst[2]);
        }
    }
    return result;
}

/* 0x800D2F34 | 0x128 */
s32 fn_800D2F34(f32* point, f32* screen)
{
    f32 projection[7];
    f32 viewport[6];
    f32 view[3];
    Mtx* viewMtx;
    Mtx44* proj;
    f32 far;

    viewMtx = fn_800D7BF8(0);
    proj = fn_800D7BF8(2);
    if (viewMtx == NULL || proj == NULL) {
        return 0;
    }

    GSvecTransform(view, *viewMtx, point);
    if (view[2] >= (far = (*proj)[2][3])) {
        return 1;
    }

    projection[0] = lbl_8047CA00;
    projection[1] = (*proj)[0][0];
    projection[2] = (*proj)[0][2];
    projection[3] = (*proj)[1][1];
    projection[4] = (*proj)[1][2];
    projection[5] = (*proj)[2][2];
    projection[6] = far;
    viewport[0] = lbl_8047CA00;
    viewport[1] = lbl_8047CA00;
    viewport[2] = lbl_8047CA04;
    viewport[3] = lbl_8047CA08;
    viewport[4] = lbl_8047CA00;
    viewport[5] = lbl_8047CA0C;
    GXProject(point[0], point[1], point[2], *viewMtx, projection, viewport,
              &screen[0], &screen[1], &screen[2]);
    return 2;
}
