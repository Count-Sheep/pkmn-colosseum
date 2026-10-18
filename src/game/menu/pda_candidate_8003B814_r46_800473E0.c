#include "dolphin/types.h"

typedef struct PdaOrbitPoint {
    f32 angle;
    f32 pad04[6];
    f32 alpha;
    f32 pad20;
} PdaOrbitPoint;

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

extern void* GSsplineCreate(s32 a, s32 b, s32 count);
extern void GSsplineAddControlVectorValue(void* spline, void* vec, f32 t);
extern void GSsplineFree(void* spline);
extern void GSvecCopy(void* dst, void* src);
extern u8 lbl_80314E08[];
extern void fn_800D5648(f32 width);
extern void fn_800D5BA0(s32 index, u32 color);
extern void fn_800D61E4(s32 x, s32 y);
extern void fn_800D6728(void);
extern void fn_800D67BC(s32 count);
extern void fn_800D6A00(s32 primitive);
extern void fn_800D7820(void* data);
extern void fn_800D888C(s32 mode);
extern void fn_800D88DC(s32 mode);

void fn_800473E0(PdaOrbitPoint* point)
{
    u8* p = (u8*)point;
    u8* data;
    void* spline;
    PdaVec3 ctrl;
    PdaVec3 v0;
    PdaVec3 v1;
    u32 count;
    s32 num;
    s32 i;
    s32 off;
    f32 step;
    f32 t;
    f32 dx;
    f32 zero;

    data = *(u8**)p;
    if (data == NULL) {
        return;
    }
    num = data[0] + 2;
    count = (u8)num;
    *(f32*)(p + 4) = (f32)count;
    if (*(s32*)(p + 0x18) < (s32)count) {
        *(s32*)(p + 0x18) = *(s32*)(p + 0x18) + 1;
    } else if (*(s32*)(p + 0x10) < (s32)count) {
        *(s32*)(p + 0x10) = *(s32*)(p + 0x10) + 1;
    } else if (*(s32*)(p + 0x10) < (s32)count + 30) {
        *(s32*)(p + 0x10) = *(s32*)(p + 0x10) + 1;
        if (*(f32*)(p + 0x14) < (f32)count) {
            *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
        }
    } else {
        if (*(f32*)(p + 0xc) < (f32)count) {
            *(f32*)(p + 0xc) = *(f32*)(p + 0xc) + *(f32*)(p + 0x20);
            if (*(f32*)(p + 0x14) < (f32)count) {
                *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
            }
        } else {
            if (*(f32*)(p + 0x14) < (f32)count) {
                *(f32*)(p + 0x14) = *(f32*)(p + 0x14) + *(f32*)(p + 0x20);
            }
        }
    }
    spline = GSsplineCreate(3, 1, (u8)num);
    if (spline == NULL) {
        return;
    }
    step = 1.0f / (f32)(u8)num;
    ctrl.x = 0.0f;
    ctrl.y = 0.0f;
    ctrl.z = *(f32*)(p + 0x1c);
    dx = *(f32*)(p + 4);
    GSsplineAddControlVectorValue(spline, &ctrl, 0.0f);
    dx = 233.0f / dx;
    t = step;
    ctrl.x = ctrl.x + dx;
    for (i = 0; i < (s32)count - 2; i++) {
        ctrl.y = (f32)(s8)data[i + 1];
        ctrl.y = -ctrl.y;
        GSsplineAddControlVectorValue(spline, &ctrl, t);
        t = t + step;
        ctrl.x = ctrl.x + dx;
    }
    ctrl.y = 0.0f;
    ctrl.x = 233.0f;
    GSsplineAddControlVectorValue(spline, &ctrl, 1.0f);
    off = 0;
    zero = 0.0f;
    for (i = 0; i < (s32)*(f32*)(p + 0xc); i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        v0.z = zero;
        GSvecCopy((u8*)*(void**)((u8*)spline + 0xc) + off, &v0);
        off += 0xc;
    }
    off = (s32)*(f32*)(p + 0xc) * 0xc;
    for (i = (s32)*(f32*)(p + 0xc); i < (s32)*(f32*)(p + 0x14); i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        v0.z = *(f32*)(p + 0x1c) /
               (f32)((s32)*(f32*)(p + 0x14) - (s32)*(f32*)(p + 0xc)) *
               (f32)(i - (s32)*(f32*)(p + 0xc));
        GSvecCopy((u8*)*(void**)((u8*)spline + 0xc) + off, &v0);
        off += 0xc;
    }
    off = 0;
    for (i = 0; i < *(s32*)(p + 0x18) - 1; i++) {
        GSvecCopy(&v0, (u8*)*(void**)((u8*)spline + 0xc) + off);
        GSvecCopy(&v1, (u8*)*(void**)((u8*)spline + 0xc) + (i + 1) * 0xc);
        fn_800D88DC(1);
        fn_800D888C(6);
        fn_800D7820(lbl_80314E08);
        fn_800D6A00(1);
        fn_800D5648(2.0f);
        fn_800D67BC(2);
        fn_800D61E4((s32)(25.0f + v0.x),
                    (s32)(0.2265625f * v0.y + 50.0f));
        fn_800D5BA0(0, (u8)(s32)v0.z | 0x361e0500);
        fn_800D61E4((s32)(25.0f + v1.x),
                    (s32)(0.2265625f * v1.y + 50.0f));
        fn_800D5BA0(0, (u8)(s32)v1.z | 0x361e0500);
        fn_800D6728();
        fn_800D5648(1.0f);
        off += 0xc;
    }
    GSsplineFree(spline);
}

