/**
 * @file gs_colsys_exact_8010EFE4.c
 * @brief GScolsys2 sphere-vs-edge resolution and swept-sphere tests,
 *        0x8010EFE4 - 0x8010F4B8.
 *
 * A function-boundary carve of the sphere-query TU (fn_8010E53C through
 * fn_8010F320; it owns the .sdata2 pool 0x8047CEF0-0x8047CF10 with its
 * own 0.0f/1.0f, so it is separate from the walk and util TUs).
 * fn_8010EFE4 pushes a sphere out of every enabled CCD object's primary
 * or alternate edge set (up to ten passes); fn_8010F188/fn_8010F320 step
 * a sphere from start to end in radius-sized slices and test each slice
 * with it. Project default flags (GC/1.3), no pragmas; the pool stays
 * extern.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

typedef f32 ColMtx[3][4];

typedef struct ColDrawGroup {
    u8* data;
    u32 count;
} ColDrawGroup;

/* One CCD object record, 0x40 bytes. */
typedef struct ColDrawObject {
    Vec3f trans;
    Vec3f rot;
    Vec3f scale;
    void* model;
    ColDrawGroup* edgeGroup0;
    ColDrawGroup* faceGroup0;
    ColDrawGroup* faceGroup1;
    ColDrawGroup* edgeGroup1;
    ColDrawGroup* faceGroup2;
    u16 flags;
    u8 pad_3E[2];
} ColDrawObject;

typedef struct ColDrawScene {
    ColDrawObject* objects;
    u32 count;
} ColDrawScene;

extern ColDrawScene* fn_8010CBC0(void);
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern s32 fn_8010CA30(ColMtx out, u32 index);
extern s32 fn_8010C8D0(ColMtx out, u32 index);
extern s32 fn_8010E53C(Vec3f* point, ColDrawGroup* group, f32 radius,
                       Vec3f* result);
extern s32 fn_8010EB28(Vec3f* point, ColDrawGroup* group, ColMtx inverse,
                       ColMtx forward, f32 radius, Vec3f* result);

/* 0x8010EFE4 | 0x1A4 */
s32 fn_8010EFE4(Vec3f* segStart, Vec3f* segEnd, f32 radius,
                Vec3f* result, s32 useAlternate)
{
    ColDrawScene* scene;
    ColDrawObject* object;
    Vec3f current;
    Vec3f hitPoint;
    ColMtx inverse;
    ColMtx forward;
    s32 pass;
    s32 hits;
    u32 index;
    Vec3f* hitPointPtr;
    s32 enabled;
    s32 collided;
    ColDrawGroup* group;

    current = *segEnd;
    scene = fn_8010CBC0();
    hitPointPtr = result == NULL ? NULL : &hitPoint;

    for (pass = 0; pass < 10; pass++) {
        object = scene->objects;
        hits = 0;
        for (index = 0; index < scene->count; index++, object++) {
            GScolsys2GetObjEnable(index, &enabled);
            if (enabled == 0) {
                continue;
            }
            if (useAlternate) {
                group = object->edgeGroup1;
                if (group == NULL) {
                    continue;
                }
            } else {
                group = object->edgeGroup0;
                if (group == NULL) {
                    continue;
                }
            }
            if ((object->flags & 1) != 0) {
                fn_8010CA30(inverse, index);
                fn_8010C8D0(forward, index);
                collided = fn_8010EB28(&current, group, inverse, forward,
                                       radius, hitPointPtr);
            } else {
                collided = fn_8010E53C(&current, group, radius, hitPointPtr);
            }
            if (collided != 0) {
                if (result == NULL) {
                    return 1;
                }
                hits++;
                if (hitPointPtr != NULL) {
                    current = *hitPointPtr;
                }
            }
        }
        if (hits <= 0) {
            break;
        }
    }

    if (pass <= 0) {
        return 0;
    }
    *result = current;
    return 1;
}

/* 0x8010F188 | 0x198 */
s32 fn_8010F188(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    Vec3f segmentEnd;
    Vec3f segmentStart;
    Vec3f direction;
    f32 distance;
    f32 step;
    f32 next;
    f32 position;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = lbl_8047CF00;
    if (distance > step) {
        step = radius / distance;
        if (step > lbl_8047CF04) {
            step = lbl_8047CF04;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = lbl_8047CF00;
    while (position < lbl_8047CF04) {
        next = position + step;
        if (next > lbl_8047CF04) {
            next = lbl_8047CF04;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010EFE4(&segmentStart, &segmentEnd, radius, result, 1)) {
            return 1;
        }
        if (step <= lbl_8047CF00) {
            break;
        }
        position += step;
    }
    return 0;
}

/* 0x8010F320 | 0x198 */
s32 fn_8010F320(Vec3f* start, Vec3f* end, f32 radius, Vec3f* result)
{
    extern f32 PSVECDistance(const Vec3f*, const Vec3f*);
    extern void PSVECSubtract(const Vec3f*, const Vec3f*, Vec3f*);
    extern void PSVECScale(const Vec3f*, Vec3f*, f32);
    extern void PSVECAdd(const Vec3f*, const Vec3f*, Vec3f*);
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    Vec3f segmentEnd;
    Vec3f segmentStart;
    Vec3f direction;
    f32 distance;
    f32 step;
    f32 next;
    f32 position;

    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    distance = PSVECDistance(start, end);
    step = lbl_8047CF00;
    if (distance > step) {
        step = radius / distance;
        if (step > lbl_8047CF04) {
            step = lbl_8047CF04;
        }
    }

    PSVECSubtract(end, start, &direction);
    position = lbl_8047CF00;
    while (position < lbl_8047CF04) {
        next = position + step;
        if (next > lbl_8047CF04) {
            next = lbl_8047CF04;
        }
        PSVECScale(&direction, &segmentStart, position);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&direction, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010EFE4(&segmentStart, &segmentEnd, radius, result, 0)) {
            return 1;
        }
        if (step <= lbl_8047CF00) {
            break;
        }
        position += step;
    }
    return 0;
}
