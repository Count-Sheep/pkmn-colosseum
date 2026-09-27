/**
 * @file GScolsys2Util.c
 * @brief GScolsy2UtilChkInTri, 0x8010F71C - 0x8010FA54 (candidate only).
 *
 * The GScolsys2Util TU runs from GScolsys2UtilGetCpPlaneLine (0x8010F4B8)
 * to GScolsy2UtilGetCpPlanePoint (0x8010FAF4) and owns the .sdata2 pool
 * 0x8047CF10-0x8047CF20 (0.0f, 1000000.0f, -1000000.0f, padding); its
 * other four functions are linked as carves, which read the pool as
 * extern. Retail reloads the 0.0f literal at every use here, which only a
 * unit owning the constant gives, so this candidate uses the literals
 * (emitted in the same order as the retail pool) and the TU can only link
 * as a whole once this function is exact.
 *
 * Checked 2026-09-27 as one whole-TU unit (text 0x8010F4B8-0x8010FAF4,
 * .sdata2 0x8047CF10-0x8047CF20, literals): the four carved functions stay
 * 100% and this one reaches the same 99.56% as here. Left: in the
 * unrolled edge test retail gives (py - y) f2 and pts[next].y f3, we give
 * them the other way round. getCpPolyVec (GScolsys2Walk.c) runs the same
 * box and edge test and stops at the same register pair; product operand
 * order, named difference/edge temporaries (any declaration position) and
 * scalar or pointer inline helpers leave it unchanged.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* 0x8010F71C | 0x338 */
#define COL_ABS(x) ((x) > 0.0f ? (x) : -(x))

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
        if ((pts[next].x - x) * (py - y) - (pts[next].y - y) * (px - x) >
            0.0f) {
            return 0;
        }
    }
    return 1;
}
