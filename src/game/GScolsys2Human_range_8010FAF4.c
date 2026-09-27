/**
 * @file GScolsys2Human_range_8010FAF4.c
 * @brief GScolsys2Human (tail) -- human/character collision queries.
 *
 * Candidate for the head of the GScolsys2Human TU (0x8010FAF4 -
 * 0x8010FFC4): fn_8010FAF4 and GScolsys2HumanCollision, the owners of the
 * TU's .sdata2 pool 0x8047CF20-0x8047CF48. The TU continues with
 * GScolsys2HumanEnable and fn_80110084, linked as the carve
 * GScolsys2Human_exact_8010FFC4.c.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

extern GSColFloor* GScolsys2GetCurFloor(void);

typedef union GScolsys2HumanFloatShape {
    f32 value;
    u32 bits;
} GScolsys2HumanFloatShape;

extern const f32 lbl_8047CF20;
extern const f64 lbl_8047CF28;
extern const f64 lbl_8047CF30;
extern const f64 lbl_8047CF38;
extern const f32 lbl_8047CF40;
extern const f32 lbl_8047CF44;
extern f32 lbl_80478AC0[];
extern f64 __frsqrte(f64 value);

static inline f32 GScolsys2HumanSqrt(f32 value)
{
    GScolsys2HumanFloatShape shape;
    f64 estimate;
    u32 exponent;
    s32 fpclass;

    if (value > lbl_8047CF20) {
        estimate = __frsqrte(value);
        estimate = lbl_8047CF28 * estimate *
                   (lbl_8047CF30 - value * (estimate * estimate));
        estimate = lbl_8047CF28 * estimate *
                   (lbl_8047CF30 - value * (estimate * estimate));
        estimate = lbl_8047CF28 * estimate *
                   (lbl_8047CF30 - value * (estimate * estimate));
        return (f32)(value * estimate);
    }
    if ((f64)value < lbl_8047CF38) {
        return lbl_80478AC0[0];
    }

    shape.value = value;
    exponent = shape.bits & 0x7F800000;
    switch (exponent) {
    case 0x7F800000:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 1 : 2;
        break;
    case 0:
        fpclass = (shape.bits & 0x007FFFFF) != 0 ? 5 : 3;
        break;
    default:
        fpclass = 4;
        break;
    }
    if (fpclass == 1) {
        return lbl_80478AC0[0];
    }
    return value;
}

/* 0x8010FAF4 | 0x304 */
s32 fn_8010FAF4(
    u8* source, s32 excludedIndex, GScolsys2Vec3* segmentStart,
    GScolsys2Vec3* segmentEnd, GScolsys2Vec3* result)
{

    extern void* fn_8018D998(s32, s32);
    extern void* peopleSearchID(void*);
    extern GScolsys2Vec3* fn_8018FCBC(void*);
    u8* floor;
    GScolsys2Vec3 current;
    GScolsys2Vec3 adjusted;
    s32 pass;

    (void)segmentStart;
    current = *segmentEnd;
    floor = (u8*)GScolsys2GetCurFloor();
    pass = 0;

    do {
        s32 collided;
        s32 index;
        u8* entry;

        collided = 0;
        entry = floor + 0xA00;
        for (index = 0; index < 0x30; index++, entry += 0x14) {
            GScolsys2Vec3* other;
            void* person;
            f32 dx;
            f32 dz;
            f32 distance;
            f32 combinedRadius;
            f32 scale;

            if (index == excludedIndex ||
                (*(u16*)(entry + 0x10) & 1) == 0 ||
                (*(u16*)(entry + 0x10) & 2) != 0) {
                continue;
            }
            person = peopleSearchID(
                fn_8018D998(*(s32*)(entry + 0), *(s32*)(entry + 4)));
            if (person == NULL) {
                continue;
            }
            other = fn_8018FCBC(person);
            if (other == NULL) {
                continue;
            }
            if (other->y >= current.y + *(f32*)(source + 0xC) ||
                other->y + *(f32*)(entry + 0xC) <= current.y) {
                continue;
            }

            dx = current.x - other->x;
            dz = current.z - other->z;
            distance = GScolsys2HumanSqrt(dx * dx + dz * dz);
            combinedRadius =
                *(f32*)(entry + 8) + *(f32*)(source + 8);
            if (distance >= combinedRadius) {
                continue;
            }
            if (result == NULL) {
                return 1;
            }
            if (distance <= lbl_8047CF20) {
                distance = lbl_8047CF40;
            }
            scale = (lbl_8047CF44 + combinedRadius) / distance;
            adjusted.x = other->x + dx * scale;
            adjusted.y = current.y;
            adjusted.z = other->z + dz * scale;
            collided = 1;
            break;
        }
        if (!collided) {
            break;
        }
        current = adjusted;
        pass++;
    } while (pass < 10);

    if (pass <= 0) {
        return 0;
    }
    *result = current;
    return 1;
}

/* Look up an in-use floor human slot. GScolsys2HumanCollision and
 * GScolsys2HumanEnable both expand this same status-code sequence. */
static inline s32 GScolsys2HumanGetEvent(s32 index, GSColFloorEvent** out)
{
    GSColFloor* floor;
    GSColFloorEvent* event;

    if (index < 0 || index >= 48) {
        return 4;
    }
    floor = GScolsys2GetCurFloor();
    if (floor == NULL) {
        return 1;
    }
    event = &floor->events[index];
    if ((event->flags & 1) == 0) {
        return 4;
    }
    *out = event;
    return 0;
}

/* 0x8010FDF8 | 0x1CC */
s32 GScolsys2HumanCollision(s32 index, GScolsys2Vec3* start,
                            GScolsys2Vec3* end, GScolsys2Vec3* result)
{
    extern f32 PSVECDistance(void*, void*);
    extern void PSVECSubtract(void*, void*, void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);
    GSColFloorEvent* event;
    GScolsys2Vec3 delta;
    GScolsys2Vec3 segmentStart;
    GScolsys2Vec3 segmentEnd;
    f32 distance;
    f32 step;
    f32 t;
    f32 next;
    s32 status;

    status = GScolsys2HumanGetEvent(index, &event);
    if (status != 0) {
        return status;
    }

    distance = PSVECDistance(start, end);
    if (distance > lbl_8047CF20) {
        step = event->radius;
        step /= distance;
        if (step > lbl_8047CF40) {
            step = lbl_8047CF40;
        }
    } else {
        step = lbl_8047CF20;
    }
    PSVECSubtract(end, start, &delta);
    t = lbl_8047CF20;
    while (t < lbl_8047CF40) {
        next = t + step;
        if (next > lbl_8047CF40) {
            next = lbl_8047CF40;
        }
        PSVECScale(&delta, &segmentStart, t);
        PSVECAdd(&segmentStart, start, &segmentStart);
        PSVECScale(&delta, &segmentEnd, next);
        PSVECAdd(&segmentEnd, start, &segmentEnd);
        if (fn_8010FAF4((u8*)event, index, &segmentStart, &segmentEnd,
                        result)) {
            return 6;
        }
        if (step <= lbl_8047CF20) {
            break;
        }
        t += step;
    }
    return 7;
}
