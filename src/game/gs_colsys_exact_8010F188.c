/**
 * @file gs_colsys_exact_8010F188.c
 * @brief GScolsys2 swept-sphere segment tests, 0x8010F188 - 0x8010F4B8.
 *
 * A function-boundary carve of the sphere-query TU (fn_8010E53C through
 * fn_8010F320; it owns the .sdata2 pool 0x8047CEF0-0x8047CF10 with its
 * own 0.0f/1.0f, so it is separate from the walk and util TUs). These
 * are its last two functions: they step a sphere of the given radius
 * from start to end in radius-sized slices and test each slice with
 * fn_8010EFE4 against the alternate (fn_8010F188) or primary
 * (fn_8010F320) edge set. Project default flags (GC/1.3), no pragmas;
 * the pool stays extern.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern void* fn_8010CBC0(void);
extern s32 fn_8010EFE4(Vec3f* segStart, Vec3f* segEnd, f32 radius,
                       Vec3f* result, s32 useAlternate);

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
