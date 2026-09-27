/**
 * @file GScolsys2Walk.c
 * @brief GScolsys2 walk-layer queries, 0x8010DE00 - 0x8010E53C.
 *
 * One retail TU (candidate; not linked yet): GScolsys2WalkGetLayer,
 * getCpPolyVec and the walk-height collector fn_8010E138. It owns its
 * .sdata2 pool, 0x8047CEE0 - 0x8047CEF0 (0.0f, 1000000.0f, -1000000.0f,
 * padding); retail reloads the 0.0f literal at each use, which only a TU
 * that owns the constant reproduces.
 *
 * getCpPolyVec keeps its C++ symbol as a C identifier, as the core TU does
 * for _offsetCCD. Built as C++ on GC/1.3, MWCC copies the 12-byte hit
 * record member by member (lhz/sth, lbz/stb); retail copies it with three
 * lwz/stw pairs, which is the C struct copy.
 *
 * fn_8010E138's collectors and the add-hit test are static inline: the
 * add-hit sequence is expanded twice (same order and constants), each
 * collector's count is routed through its own register and copied to the
 * caller's (mr r7), and the grid collector's early "return 0" re-clears
 * its count register on both failure paths.
 *
 * Left (2026-09-27): getCpPolyVec 99.38 - in the edge test retail gives
 * (p.z - v.z) f2 and (vn.z - v.z) f3, we give them the other way round;
 * fn_8010E138 99.67 - the grid cell x/z/cellCountX take r5/r3/r4 instead
 * of retail's r3/r4/r5.
 */
#include "dolphin/types.h"

typedef struct GSvec {
    f32 x, y, z;
} GSvec;

typedef f32 ColMtx[3][4];

/* One collision triangle, 0x34 bytes. */
typedef struct ColTri {
    /* 0x00 */ GSvec verts[3];
    /* 0x24 */ GSvec normal;
    /* 0x30 */ u8 surface : 4;
    /* 0x30 */ u8 attr : 4;
    /* 0x31 */ u8 layer : 4;
    /* 0x31 */ u8 subLayer : 4;
    /* 0x32 */ u16 id;
} ColTri;

typedef struct ColTriList {
    ColTri* tris;
    u32 count;
} ColTriList;

typedef struct ColGridCell {
    u32 first;
    u32 count;
} ColGridCell;

/* Fixed-model triangle grid, bucketed in the x/z plane. */
typedef struct ColGrid {
    /* 0x00 */ ColTri* tris;
    /* 0x04 */ u32 triCount;
    /* 0x08 */ ColGridCell* cells;
    /* 0x0C */ u32* indices;
    /* 0x10 */ u16 cellCountX;
    /* 0x12 */ u16 cellCountZ;
    /* 0x14 */ f32 cellWidth;
    /* 0x18 */ f32 cellDepth;
    /* 0x1C */ f32 minX;
    /* 0x20 */ f32 minZ;
} ColGrid;

/* One CCD object, 0x40 bytes. */
typedef struct ColObj {
    /* 0x00 */ u8 pad_00[0x24];
    /* 0x24 */ void* walk;
    /* 0x28 */ u8 pad_28[0x14];
    /* 0x3C */ u16 flags;
    /* 0x3E */ u8 pad_3E[2];
} ColObj;

typedef struct ColScene {
    ColObj* objs;
    u32 count;
} ColScene;

/* One walkable height under a point, 0xC bytes. */
typedef struct ColWalkHit {
    f32 height;
    u16 surface;
    u16 attr;
    u8 layer;
    u8 subLayer;
} ColWalkHit;

extern ColScene* fn_8010CBC0(void);
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern void fn_8010CA30(ColMtx out, u32 index);
extern void fn_8010C8D0(ColMtx out, u32 index);
extern void PSMTXMultVec(ColMtx m, const GSvec* src, GSvec* dst);
s32 fn_8010E138(GSvec* point, ColWalkHit* out);

#define COL_ABS(x) ((x) > 0.0f ? (x) : -(x))

/* 0x8010DE00 | 0xF0 */
s32 GScolsys2WalkGetLayer(GSvec* position, u8* layer, u8* subLayer)
{
    ColWalkHit hits[8];
    s32 count;
    s32 i;
    s32 closest;
    f32 distance;
    f32 closestDistance;

    count = fn_8010E138(position, hits);
    if (count <= 0) {
        return 0;
    }

    closestDistance = COL_ABS(position->y - hits[0].height);
    closest = 0;
    for (i = 1; i < count; i++) {
        distance = COL_ABS(position->y - hits[i].height);
        if (closestDistance > distance) {
            closest = i;
            closestDistance = distance;
        }
    }

    *layer = hits[closest].layer;
    *subLayer = hits[closest].subLayer;
    return 1;
}

/* 0x8010DEF0 | 0x248 */
s32 getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(GSvec* out, GSvec* point, GSvec* vertices, GSvec* normal)
{
    f32 minZ;
    f32 minX;
    f32 maxZ;
    f32 maxX;
    f32 height;
    s32 i;
    s32 next;

    minX = minZ = 1000000.0f;
    maxX = maxZ = -1000000.0f;
    for (i = 0; i < 3; i++) {
        f32 x;
        f32 z;

        x = vertices[i].x;
        if (minX > x) {
            minX = x;
        }
        z = vertices[i].z;
        if (minZ > z) {
            minZ = z;
        }
        if (maxX < x) {
            maxX = x;
        }
        if (maxZ < z) {
            maxZ = z;
        }
    }
    if (minX > point->x || minZ > point->z || maxX < point->x ||
        maxZ < point->z) {
        return 0;
    }

    for (i = 0; i < 3; i++) {
        f32 x;
        f32 z;

        next = i + 1;
        if (next >= 3) {
            next = 0;
        }
        x = vertices[i].x;
        z = vertices[i].z;
        if ((vertices[next].x - x) * (point->z - z) -
                (vertices[next].z - z) * (point->x - x) >
            0.0f) {
            return 0;
        }
    }

    if (0.0f == normal->y) {
        return 0;
    }
    height = -(normal->x * (point->x - vertices[0].x) +
               normal->y * (point->y - vertices[0].y) +
               normal->z * (point->z - vertices[0].z)) /
             normal->y;
    out->x = point->x;
    out->y = point->y + height;
    out->z = point->z;
    return 1;
}

/* Append the height of a hit point to the list unless it already holds
 * it. */
static inline s32 ColWalkAddHit(ColWalkHit* list, s32 count, GSvec* cp,
                                ColTri* tri)
{
    s32 i;

    for (i = 0; i < count; i++) {
        if (cp->y == list[i].height) {
            break;
        }
    }
    if (i < count) {
        return 0;
    }

    list[count].height = cp->y;
    if (tri->surface == 15) {
        list[count].surface = 0xFFFF;
    } else {
        list[count].surface = tri->surface;
    }
    if (tri->attr == 15) {
        list[count].attr = 0xFFFF;
    } else {
        list[count].attr = tri->attr;
    }
    list[count].layer = tri->layer;
    list[count].subLayer = tri->subLayer;
    return 1;
}

/* Collect the walk heights under a point from a movable object's
 * triangle list, transformed into the object's placement. */
static inline s32 ColWalkCollectList(ColWalkHit* list, GSvec* point,
                                     ColTriList* tris, ColMtx matrix,
                                     ColMtx normalMatrix)
{
    GSvec verts[3];
    GSvec normal;
    GSvec cp;
    ColTri* tri;
    u32 j;
    s32 v;
    s32 count;

    count = 0;
    tri = tris->tris;
    for (j = 0; j < tris->count && count < 8; j++, tri++) {
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(matrix, &tri->verts[v], &verts[v]);
        }
        PSMTXMultVec(normalMatrix, &tri->normal, &normal);
        if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(&cp, point, verts,
                                                        &normal) != 0) {
            if (ColWalkAddHit(list, count, &cp, tri)) {
                count++;
            }
        }
    }
    return count;
}

/* Collect the walk heights under a point from a fixed model's grid. */
static inline s32 ColWalkCollectGrid(ColWalkHit* list, GSvec* point,
                                     ColGrid* grid)
{
    GSvec cp;
    ColTri* tri;
    ColGridCell* cell;
    u32* index;
    u32 j;
    s32 x;
    s32 z;
    s32 count;

    count = 0;
    x = (point->x - grid->minX) / grid->cellWidth;
    if (x < 0 || x >= grid->cellCountX) {
        return 0;
    }
    z = (point->z - grid->minZ) / grid->cellDepth;
    if (z < 0 || z >= grid->cellCountZ) {
        return 0;
    }

    cell = &grid->cells[x + z * grid->cellCountX];
    index = &grid->indices[cell->first];
    for (j = 0; j < cell->count && count < 8; j++, index++) {
        tri = &grid->tris[*index];
        if (getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec(&cp, point, tri->verts,
                                                        &tri->normal) != 0) {
            if (ColWalkAddHit(list, count, &cp, tri)) {
                count++;
            }
        }
    }
    return count;
}
/* 0x8010E138 | 0x404 */
s32 fn_8010E138(GSvec* point, ColWalkHit* out)
{
    ColWalkHit temp[8];
    ColMtx matrix;
    ColMtx normalMatrix;
    ColScene* scene;
    ColObj* obj;
    void* walk;
    u32 i;
    s32 total;
    s32 enabled;
    s32 k;
    s32 count;

    total = 0;
    scene = fn_8010CBC0();
    if (scene == NULL) {
        return 0;
    }

    obj = scene->objs;
    for (i = 0; i < scene->count && total < 8; i++, obj++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        walk = obj->walk;
        if (walk == NULL) {
            continue;
        }

        if (obj->flags & 1) {
            fn_8010CA30(matrix, i);
            fn_8010C8D0(normalMatrix, i);
            count = ColWalkCollectList(temp, point, walk, matrix,
                                       normalMatrix);
        } else {
            count = ColWalkCollectGrid(temp, point, walk);
        }

        for (k = 0; k < count && total < 8; k++, total++) {
            out[total] = temp[k];
        }
    }

    return total;
}
