/**
 * @file GScolsys2Util.c
 * @brief GScolsys2Util -- small collision-query leaf helpers.
 *
 * Candidate for GScolsy2UtilChkInTri, 0x8010F71C - 0x8010FA54. The
 * GScolsys2Util TU runs from GScolsys2UtilGetCpPlaneLine (0x8010F4B8)
 * to GScolsy2UtilGetCpPlanePoint and owns the .sdata2 pool
 * 0x8047CF10-0x8047CF20; its other functions are linked as carves.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"
#include "game/world/gs_field.h"

/* 0x8010F71C | 0x338 */
extern const f32 lbl_8047CF10; /* 0.0f */
extern const f32 lbl_8047CF14; /* 1e6f */
extern const f32 lbl_8047CF18; /* -1e6f */

#define COL_ABS(x) ((x) > lbl_8047CF10 ? (x) : -(x))

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

    if (facing < lbl_8047CF10) {
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

    minX = minY = lbl_8047CF14;
    maxX = maxY = lbl_8047CF18;
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
        next = i + 1;
        if (next >= 3) {
            next = 0;
        }
        if ((pts[next].x - pts[i].x) * (py - pts[i].y) -
                (pts[next].y - pts[i].y) * (px - pts[i].x) >
            lbl_8047CF10) {
            return 0;
        }
    }
    return 1;
}
