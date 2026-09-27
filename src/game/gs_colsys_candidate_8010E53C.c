/**
 * @file gs_colsys_candidate_8010E53C.c
 * @brief GScolsys2 sphere queries, 0x8010E53C - 0x8010EFE4 (candidate only;
 *        not linked).
 *
 * The first two functions of the GScolsys2 sphere TU (0x8010E53C -
 * 0x8010F4B8, pool 0x8047CEF0 - 0x8047CF10 with its own 0.0f); the rest of
 * the TU is linked as carves (gs_colsys_exact_8010EFE4.c and on).
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

typedef f32 ColMtx[3][4];

typedef struct ColDrawGroup {
    u8* data;
    u32 count;
} ColDrawGroup;

extern void PSMTXMultVec(ColMtx, const f32*, f32*);
typedef struct GSFieldFixedMdlCell {
    u32 firstIndex;
    u32 count;
} GSFieldFixedMdlCell;

typedef struct GScolsys2Triangle {
    Vec3f verts[3];
    Vec3f normal;
    u16 flags;
    u16 id;
} GScolsys2Triangle;

typedef struct GSFieldFixedMdlEventList {
    /* 0x00 */ GScolsys2Triangle* triangles;
    /* 0x04 */ u8 pad_04[4];
    /* 0x08 */ GSFieldFixedMdlCell* cells;
    /* 0x0C */ u32* triangleIndices;
    /* 0x10 */ u16 cellCountX;
    /* 0x12 */ u16 cellCountZ;
    /* 0x14 */ f32 cellWidth;
    /* 0x18 */ f32 cellDepth;
    /* 0x1C */ f32 minX;
    /* 0x20 */ f32 minZ;
} GSFieldFixedMdlEventList;

typedef struct GScolsys2TriangleList {
    GScolsys2Triangle* triangles;
    u32 count;
} GScolsys2TriangleList;

typedef struct GScolsys2Region {
    u8 pad_00[0x24];
    void* triList;
    u8 pad_28[0x14];
    u16 flags;
    u8 pad_3E[2];
} GScolsys2Region;

/* 0x8010E53C | 0x5EC */
typedef struct GSfieldEdgeMasks {
    u16 values[3];
} GSfieldEdgeMasks;

extern f32 GScolsy2UtilGetSidePlanePoint(void*, void*, void*);
extern void GScolsy2UtilGetCpPlanePoint(void*, void*, void*, void*);
extern s32 GScolsy2UtilChkInTri(void*, void*, void*);
extern void GScolsy2UtilGetPointExtentionLine(void*, void*, void*, f32);
f32 GScolsys2UtilGetCpLinePoint(Vec3f*, Vec3f*, Vec3f*, Vec3f*);

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010E53C(Vec3f* point, void* data, f32 radius, Vec3f* result) {
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    extern const f32 lbl_8047CF08[];
    extern f32 PSVECSquareDistance(void* a, void* b);
    GSFieldFixedMdlEventList* grid;
    GSFieldFixedMdlCell* cell;
    GScolsys2Triangle* tri;
    GSfieldEdgeMasks edgeMasksA;
    GSfieldEdgeMasks edgeMasksB;
    Vec3f planePoint;
    Vec3f cp;
    Vec3f lineCp;
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    s32 x;
    s32 z;
    s32 hit;
    s32 edge;
    f32 radiusSq;

    grid = data;
    startX = (s32)((point->x - radius - grid->minX) / grid->cellWidth);
    if (startX < 0) {
        startX = 0;
    }
    startZ = (s32)((point->z - radius - grid->minZ) / grid->cellDepth);
    if (startZ < 0) {
        startZ = 0;
    }
    endX = (s32)((point->x + radius - grid->minX) / grid->cellWidth);
    if (endX > (s32)grid->cellCountX - 1) {
        endX = (s32)grid->cellCountX - 1;
    }
    endZ = (s32)((point->z + radius - grid->minZ) / grid->cellDepth);
    if (endZ > (s32)grid->cellCountZ - 1) {
        endZ = (s32)grid->cellCountZ - 1;
    }

    radiusSq = radius * radius;
    edgeMasksA.values[0] = 1;
    edgeMasksA.values[1] = 2;
    edgeMasksA.values[2] = 4;
    edgeMasksB = edgeMasksA;

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                    lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    GScolsy2UtilGetCpPlanePoint(&cp, &tri->normal,
                                                (Vec3f*)tri, point);
                    if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                        hit = 0;
                    } else if (GScolsy2UtilChkInTri(&cp, tri, &tri->normal) ==
                               0)
                    {
                        hit = 0;
                    } else {
                        lineCp = cp;
                        hit = 1;
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                                     lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    hit = 0;
                } else if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri,
                                                         point) <
                           lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    hit = 0;
                    for (edge = 0; edge < 3; edge++) {
                        s32 next = edge + 1;
                        f32 lineT;

                        if (next >= 3) {
                            next = 0;
                        }
                        if ((tri->flags & edgeMasksA.values[edge]) == 0) {
                            continue;
                        }
                        lineT = GScolsys2UtilGetCpLinePoint(
                            &lineCp, &tri->verts[edge], &tri->verts[next],
                            point);
                        if (lineT < lbl_8047CF00 || lineT > lbl_8047CF04) {
                            continue;
                        }
                        if (PSVECSquareDistance(&lineCp, point) < radiusSq) {
                            hit = 1;
                            break;
                        }
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(
                        result, &lineCp, point, lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    for (z = startZ; z <= endZ; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX; x++, cell++) {
            u32* triIndexPtr;
            u32 cellTriIdx;

            triIndexPtr = grid->triangleIndices + cell->firstIndex;
            cellTriIdx = 0;
            while (cellTriIdx < cell->count) {
                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    hit = 0;
                } else if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri,
                                                         point) <
                           lbl_8047CF00)
                {
                    hit = 0;
                } else {
                    hit = 0;
                    for (edge = 0; edge < 3; edge++) {
                        s32 next = edge + 2;

                        if (next >= 3) {
                            next -= 3;
                        }
                        if ((tri->flags & edgeMasksB.values[edge]) == 0 ||
                            (tri->flags & edgeMasksB.values[next]) == 0)
                        {
                            continue;
                        }
                        if (PSVECSquareDistance(&tri->verts[edge], point) <
                            radiusSq)
                        {
                            lineCp = tri->verts[edge];
                            hit = 1;
                            break;
                        }
                    }
                }
                if (hit != 0) {
                    if (result == NULL) {
                        return 1;
                    }
                    GScolsy2UtilGetPointExtentionLine(
                        result, &lineCp, point, lbl_8047CF08[0] + radius);
                    return 1;
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    return 0;
}
#pragma pop

/* 0x8010EB28 | 0x4BC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_8010EB28(Vec3f* point, void* data, ColMtx inverse,
                ColMtx forward, f32 radius, Vec3f* result) {
    extern const f32 lbl_8047CF00;
    extern const f32 lbl_8047CF04;
    extern const f32 lbl_8047CF08[];
    extern f32 PSVECSquareDistance(void* a, void* b);
    ColDrawGroup* group;
    GScolsys2Triangle* tri;
    Vec3f transformedVerts[3];
    Vec3f transformedNormal;
    Vec3f planePoint;
    Vec3f cp;
    Vec3f lineCp;
    Vec3f* currentVert;
    Vec3f* otherVert;
    u16 edgeMasksA[3];
    u16 edgeMasksB[3];
    u32 i;
    s32 edge;
    s32 next;
    s32 hit;
    f32 radiusSq;
    f32 lineT;

    group = data;
    tri = (GScolsys2Triangle*)group->data;
    radiusSq = radius * radius;

    i = 0;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        currentVert = tri->verts;
        otherVert = transformedVerts;
        for (edge = 0; edge < 3; edge++, currentVert++, otherVert++) {
            PSMTXMultVec(inverse, (const f32*)currentVert,
                         (f32*)otherVert);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            GScolsy2UtilGetCpPlanePoint(&cp, &transformedNormal,
                                        transformedVerts, point);
            if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                hit = 0;
            } else if (GScolsy2UtilChkInTri(&cp, transformedVerts,
                                            &transformedNormal) == 0)
            {
                hit = 0;
            } else {
                lineCp = cp;
                hit = 1;
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    edgeMasksA[0] = 1;
    edgeMasksA[1] = 2;
    edgeMasksA[2] = 4;

    i = 0;
    tri = (GScolsys2Triangle*)group->data;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        if ((tri->flags & 7) == 0) {
            continue;
        }
        for (edge = 0; edge < 3; edge++) {
            PSMTXMultVec(inverse, (const f32*)&tri->verts[edge],
                         (f32*)&transformedVerts[edge]);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            hit = 0;
            currentVert = transformedVerts;
            for (edge = 0; edge < 3; edge++, currentVert++) {
                next = edge + 1;
                if (next >= 3) {
                    next = 0;
                }
                if ((tri->flags & edgeMasksA[edge]) == 0) {
                    continue;
                }
                otherVert = &transformedVerts[next];
                lineT = GScolsys2UtilGetCpLinePoint(&lineCp, currentVert,
                                                    otherVert, point);
                if (lineT < lbl_8047CF00 || lineT > lbl_8047CF04) {
                    continue;
                }
                if (PSVECSquareDistance(&lineCp, point) < radiusSq) {
                    hit = 1;
                    break;
                }
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    edgeMasksB[0] = 1;
    edgeMasksB[1] = 2;
    edgeMasksB[2] = 4;

    i = 0;
    tri = (GScolsys2Triangle*)group->data;
    hit = 0;
    for (; i < group->count && hit == 0;
         i++, tri = (GScolsys2Triangle*)((u8*)tri + 0x34)) {
        if ((tri->flags & 7) == 0) {
            continue;
        }
        for (edge = 0; edge < 3; edge++) {
            PSMTXMultVec(inverse, (const f32*)&tri->verts[edge],
                         (f32*)&transformedVerts[edge]);
        }
        PSMTXMultVec(forward, (const f32*)&tri->normal, (f32*)&transformedNormal);

        if (GScolsy2UtilGetSidePlanePoint(&transformedNormal, transformedVerts,
                                          point) < lbl_8047CF00)
        {
            hit = 0;
        } else {
            hit = 0;
            for (edge = 0; edge < 3; edge++) {
                next = edge + 2;
                if (next >= 3) {
                    next -= 3;
                }
                if ((tri->flags & edgeMasksB[edge]) == 0 ||
                    (tri->flags & edgeMasksB[next]) == 0)
                {
                    continue;
                }
                if (PSVECSquareDistance(&transformedVerts[edge], point) <
                    radiusSq)
                {
                    lineCp = transformedVerts[edge];
                    hit = 1;
                    break;
                }
            }
        }
    }
    if (hit != 0) {
        if (result == NULL) {
            return 1;
        }
        GScolsy2UtilGetPointExtentionLine(result, &lineCp, point,
                                         lbl_8047CF08[0] + radius);
        return 1;
    }

    return 0;
}
#pragma pop
