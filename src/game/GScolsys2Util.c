/**
 * @file GScolsys2Util.c
 * @brief GScolsys2Util TU, 0x8010F4B8 - 0x8010FAF4
 *        (GScolsys2UtilGetCpPlaneLine .. GScolsy2UtilGetCpPlanePoint).
 *
 * Owns its .sdata2 pool 0x8047CF10-0x8047CF20 (0.0f, 1000000.0f,
 * -1000000.0f, padding), written as literals: retail reloads the 0.0f at
 * every use, which only a unit owning the constant gives. Project default
 * flags (GC/1.3), no pragmas.
 *
 * GScolsy2UtilChkInTri's edge test goes through colEdgeSide, which gives
 * retail's float-register order (see the helper); getCpPolyVec in
 * GScolsys2Walk.c runs the same test.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* 0x8010F4B8 | 0xEC */
extern void PSVECSubtract(const Vec3f* left, const Vec3f* right, Vec3f* result);
extern void PSVECScale(const Vec3f* vector, Vec3f* result, f32 scale);
extern void PSVECAdd(const Vec3f* left, const Vec3f* right, Vec3f* result);

s32 GScolsys2UtilGetCpPlaneLine(Vec3f* out, f32* tOut,
                                const Vec3f* normal,
                                const Vec3f* planePoint,
                                const Vec3f* lineStart,
                                const Vec3f* lineEnd)
{
    Vec3f direction;
    f32 denominator;
    f32 t;

    PSVECSubtract(lineEnd, lineStart, &direction);
    if ((denominator = normal->x * direction.x + normal->y * direction.y
                     + normal->z * direction.z) == 0.0f) {
        return 0;
    }

    t = (normal->x * (planePoint->x - lineStart->x)
       + normal->y * (planePoint->y - lineStart->y)
       + normal->z * (planePoint->z - lineStart->z)) / denominator;
    PSVECScale(&direction, &direction, t);
    PSVECAdd(&direction, lineStart, out);
    *tOut = t;
    return 1;
}

/* 0x8010F5A4 | 0xFC */

f32 GScolsys2UtilGetCpLinePoint(Vec3f* out, Vec3f* start, Vec3f* end,
                               Vec3f* point)
{
    Vec3f direction;
    f32 lengthSquared;
    f32 t;

    PSVECSubtract(end, start, &direction);
    lengthSquared = direction.x * direction.x + direction.y * direction.y
                  + direction.z * direction.z;
    if (0.0f == lengthSquared) {
        out->x = start->x;
        out->y = start->y;
        out->z = start->z;
        return 0.0f;
    }

    t = (direction.x * (point->x - start->x)
       + direction.y * (point->y - start->y)
       + direction.z * (point->z - start->z)) / lengthSquared;
    PSVECScale(&direction, &direction, t);
    PSVECAdd(&direction, start, out);
    return t;
}

/* 0x8010F6A0 | 0x7C */
void GScolsy2UtilGetPointExtentionLine(void* arg0, void* arg1, void* arg2, f32 t) {
    f32 v[3];
    extern void PSVECSubtract(void*, void*, void*);
    extern f32 PSVECMag(void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);

    PSVECSubtract(arg2, arg1, v);
    PSVECScale(v, v, t / PSVECMag(v));
    PSVECAdd(v, arg1, arg0);
}

/* 0x8010F71C | 0x338 */
#define COL_ABS(x) ((x) > 0.0f ? (x) : -(x))

/* RULE-EXCEPTION(title-path): inline helper whose only evidence is register
 * colouring - see docs/RULE_EXCEPTIONS.md. Which side of the edge
 * (x,z)->(ex,ez) the point (px,pz) is on; the same edge test as
 * getCpPolyVec in GScolsys2Walk.c. The edge deltas are written in place, so
 * they become the inline's own temporaries, created before the point
 * deltas; that is the float-register order retail has. */
static inline f32 colEdgeSide(f32 ez, f32 ex, f32 z, f32 x, f32 pz, f32 px)
{
    ez -= z;
    ex -= x;
    return ex * (pz - z) - ez * (px - x);
}

s32 GScolsy2UtilChkInTri(Vec3f* point, Vec3f* verts, Vec3f* normal)
{
    Vec3f pts[3];
    f32 absX;
    f32 absY;
    f32 absZ;
    f32 facing;
    s32 u;
    s32 v;
    f32 minY;
    f32 minX;
    f32 maxY;
    f32 maxX;
    f32 px;
    f32 py;
    s32 i;
    s32 next;

    absX = COL_ABS(normal->x);
    absY = COL_ABS(normal->y);
    absZ = COL_ABS(normal->z);
    if (absX < absY) {
        if (absY < absZ) {
            facing = normal->z;
            u = 0;
            v = 1;
        } else {
            facing = normal->y;
            u = 2;
            v = 0;
        }
    } else if (absX > absZ) {
        facing = -normal->x;
        u = 2;
        v = 1;
    } else {
        facing = normal->z;
        u = 0;
        v = 1;
    }

    if (facing < 0.0f) {
        for (i = 0; i < 3; i++) {
            pts[i].x = (&verts[i].x)[u];
            pts[i].y = (&verts[i].x)[v];
        }
    } else {
        for (i = 0; i < 3; i++) {
            pts[i].x = (&verts[2 - i].x)[u];
            pts[i].y = (&verts[2 - i].x)[v];
        }
    }

    minX = minY = 1000000.0f;
    maxX = maxY = -1000000.0f;
    px = (&point->x)[u];
    py = (&point->x)[v];
    for (i = 0; i < 3; i++) {
        f32 x;
        f32 y;

        x = pts[i].x;
        if (minX > x) {
            minX = x;
        }
        y = pts[i].y;
        if (minY > y) {
            minY = y;
        }
        if (maxX < x) {
            maxX = x;
        }
        if (maxY < y) {
            maxY = y;
        }
    }
    if (minX > px || minY > py || maxX < px || maxY < py) {
        return 0;
    }

    for (i = 0; i < 3; i++) {
        f32 x;
        f32 y;

        next = i + 1;
        if (next >= 3) {
            next = 0;
        }
        x = pts[i].x;
        y = pts[i].y;
        if (colEdgeSide(pts[next].y, pts[next].x, y, x, py, px) > 0.0f) {
            return 0;
        }
    }
    return 1;
}

/* 0x8010FA54 | 0xA0 */
void GScolsy2UtilGetCpPlanePoint(Vec3f* out, Vec3f* normal, Vec3f* verts, Vec3f* point) {
    f32 scale;
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);

    scale = (normal->x * (verts->x - point->x)
           + normal->y * (verts->y - point->y)
           + normal->z * (verts->z - point->z))
          / (normal->x * normal->x
           + normal->y * normal->y
           + normal->z * normal->z);
    PSVECScale(normal, out, scale);
    PSVECAdd(out, point, out);
}
