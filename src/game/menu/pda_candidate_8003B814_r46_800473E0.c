#include "dolphin/types.h"

typedef struct PdaStatLine {
    u8* data;
    f32 count;
    u8 pad08[4];
    f32 drawn;
    s32 wait;
    f32 lifted;
    s32 segments;
    f32 height;
    f32 speed;
} PdaStatLine;

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

typedef struct GSspline {
    s32 kind;
    s32 state;
    u8 capacity;
    u8 valueCount;
    u8 pad0A;
    u8 keyCount;
    PdaVec3* vectors;
} GSspline;

extern GSspline* GSsplineCreate(s32 kind, s32 state, u8 capacity);
extern void GSsplineAddControlVectorValue(GSspline* spline, void* vec, f32 t);
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

void fn_800473E0(PdaStatLine* p)
{
    GSspline* spline;
    u8* data;
    PdaVec3 ctrl;
    PdaVec3 v0;
    PdaVec3 v1;
    f32 t;
    s32 num;
    s32 i;
    f32 step;
    u32 count;
    f32 dx;

    data = p->data;
    if (data == NULL) {
        return;
    }
    num = data[0] + 2;
    count = (u8)num;
    p->count = (f32)count;
    if (p->segments < (s32)count) {
        p->segments = p->segments + 1;
    } else if (p->wait < (s32)count) {
        p->wait = p->wait + 1;
    } else if (p->wait < (s32)count + 30) {
        p->wait = p->wait + 1;
        if (p->lifted < (f32)count) {
            p->lifted = p->lifted + p->speed;
        }
    } else {
        if (p->drawn < (f32)count) {
            p->drawn = p->drawn + p->speed;
            if (p->lifted < (f32)count) {
                p->lifted = p->lifted + p->speed;
            }
        } else {
            if (p->lifted < (f32)count) {
                p->lifted = p->lifted + p->speed;
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
    ctrl.z = p->height;
    dx = p->count;
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
    for (i = 0; i < (s32)p->drawn; i++) {
        GSvecCopy(&v0, &spline->vectors[i]);
        v0.z = 0.0f;
        GSvecCopy(&spline->vectors[i], &v0);
    }
    for (i = (s32)p->drawn; i < (s32)p->lifted; i++) {
        GSvecCopy(&v0, &spline->vectors[i]);
        v0.z = p->height /
               (f32)((s32)p->lifted - (s32)p->drawn) *
               (f32)(i - (s32)p->drawn);
        GSvecCopy(&spline->vectors[i], &v0);
    }
    for (i = 0; i < p->segments - 1; i++) {
        GSvecCopy(&v0, &spline->vectors[i]);
        GSvecCopy(&v1, &spline->vectors[i + 1]);
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
    }
    GSsplineFree(spline);
}

