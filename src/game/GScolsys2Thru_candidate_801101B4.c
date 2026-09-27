/**
 * @file GScolsys2Thru_candidate_801101B4.c
 * @brief GScolsys2Thru -- spatial grid / "thru" collision queries.
 *
 * Candidate for the GScolsys2Thru TU, 0x801101B4 - 0x80111864: the
 * owners of the .sdata2 pool 0x8047CF48-0x8047CF60. fn_80110084 belongs
 * to GScolsys2Human (linked in GScolsys2Human_exact_8010FFC4.c) and
 * fn_80111864 to GScolsys2Check (it shares that TU's pool
 * 0x8047CF60-0x8047CF68).
 *
 * fn_801101B4 previously carried an invented "GSfield_BuildCollisionGrid"
 * name/signature from an earlier bad campaign pass (same class of issue
 * documented in include/game/gs_colsys.h); reverted to the standard
 * fn_<addr> placeholder since no confirmed symbols.txt name exists yet.
 * GScolsys2ThruGetFixedMdlEventList similarly carried an invented "GSfield_GridLookup" name;
 * renamed to its confirmed name below.
 *
 * Address range: 0x801101B4 - 0x80111864
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* 0x801101B4 | 0x4E8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
typedef struct GSFieldWzxRegionLocal {
    u8 pad_00[0x2C];
    GScolsys2TriangleList* triangles;
    u8 pad_30[0x0C];
    u16 flags;
    u8 pad_3E[2];
} GSFieldWzxRegionLocal;

s32 fn_801101B4(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out)
{
#pragma optimization_level 4
    extern f32 lbl_8047CF58;
    extern f32 lbl_8047CF5C;
    GSFieldWzxData* wzx;
    GSFieldWzxRegionLocal* region;
    GScolsys2TriangleList* triList;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle temp[4];
    GSfieldQueryTriangle* tempWrite;
    GSfieldQueryTriangle* tempRead;
    GSfieldQueryTriangle* scan;
    GSfieldQueryTriangle* outSlot;
    GScolsys2Vec3 dirVec;
    GScolsys2Vec3 planePoint;
    GScolsys2Vec3 hitPoint;
    GScolsys2Vec3 transformedVerts[3];
    f32 resultT;
    f32 mtxInv[12];
    f32 mtxFwd[12];
    s32 regionIdx;
    s32 triIdx;
    s32 vertIdx;
    s32 scanIdx;
    s32 visible;
    s32 tempCount;
    s32 outCount;
    s32 hit;

    tempCount = 0;
    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    if (wzx == NULL) {
        return 0;
    }

    region = (GSFieldWzxRegionLocal*)wzx->regions;
    regionIdx = 0;
    while ((u32)regionIdx < wzx->regionCount && outCount < 4) {
        GScolsys2GetObjEnable(regionIdx, &visible);
        if (visible != 0) {
            triList = region->triangles;
            if (triList != NULL) {
                tempCount = 0;
                tempWrite = temp;
                if ((region->flags & 1) != 0) {
                    fn_8010CA30(mtxInv, regionIdx);
                    fn_8010C8D0(mtxFwd, regionIdx);
                    PSVECSubtract(end, start, &dirVec);
                    tri = triList->triangles;
                    triIdx = 0;
                    while ((u32)triIdx < triList->count && tempCount < 4) {
                        scan = temp;
                        scanIdx = 0;
                        while (scanIdx < tempCount) {
                            if (tri->id == scan->id) {
                                break;
                            }
                            scan++;
                            scanIdx++;
                        }
                        if (scanIdx >= tempCount) {
                            PSMTXMultVec(mtxFwd, &tri->normal, &planePoint);
                            if (!(PSVECDotProduct(&planePoint, &dirVec) >=
                                  lbl_8047CF58))
                            {
                                for (vertIdx = 0; vertIdx < 3; vertIdx++) {
                                    PSMTXMultVec(mtxInv, &tri->verts[vertIdx],
                                                &transformedVerts[vertIdx]);
                                }
                                if (GScolsys2UtilGetCpPlaneLine(
                                        (Vec3f*)&hitPoint, &resultT,
                                        (Vec3f*)&planePoint,
                                        (Vec3f*)transformedVerts,
                                        (Vec3f*)start, (Vec3f*)end) == 0)
                                {
                                    hit = 0;
                                } else if (resultT < lbl_8047CF58 ||
                                           resultT > lbl_8047CF5C)
                                {
                                    hit = 0;
                                } else if (GScolsy2UtilChkInTri(
                                               &hitPoint, transformedVerts,
                                               &planePoint) == 0)
                                {
                                    hit = 0;
                                } else {
                                    hit = 1;
                                }
                                if (hit != 0) {
                                    tempWrite->verts[0] = transformedVerts[0];
                                    tempWrite->verts[1] = transformedVerts[1];
                                    tempWrite->verts[2] = transformedVerts[2];
                                    tempWrite->normal = planePoint;
                                    tempWrite->id = tri->id;
                                    tempWrite++;
                                    tempCount++;
                                }
                            }
                        }
                        triIdx++;
                        tri++;
                    }
                } else {
                    PSVECSubtract(end, start, &dirVec);
                    tri = triList->triangles;
                    triIdx = 0;
                    while ((u32)triIdx < triList->count && tempCount < 4) {
                        scan = temp;
                        scanIdx = 0;
                        while (scanIdx < tempCount) {
                            if (tri->id == scan->id) {
                                break;
                            }
                            scan++;
                            scanIdx++;
                        }
                        if (scanIdx >= tempCount &&
                            !(PSVECDotProduct(&tri->normal, &dirVec) >=
                              lbl_8047CF58))
                        {
                            if (GScolsys2UtilGetCpPlaneLine(
                                    (Vec3f*)&hitPoint, &resultT,
                                    (Vec3f*)&tri->normal, (Vec3f*)tri,
                                    (Vec3f*)start, (Vec3f*)end) == 0)
                            {
                                hit = 0;
                            } else if (resultT < lbl_8047CF58 ||
                                       resultT > lbl_8047CF5C)
                            {
                                hit = 0;
                            } else if (GScolsy2UtilChkInTri(&hitPoint, tri,
                                                             &tri->normal) == 0)
                            {
                                hit = 0;
                            } else {
                                hit = 1;
                            }
                            if (hit != 0) {
                                tempWrite->verts[0] = tri->verts[0];
                                tempWrite->verts[1] = tri->verts[1];
                                tempWrite->verts[2] = tri->verts[2];
                                tempWrite->normal = tri->normal;
                                tempWrite->id = tri->id;
                                tempWrite++;
                                tempCount++;
                            }
                        }
                        triIdx++;
                        tri++;
                    }
                }

                tempRead = temp;
                triIdx = 0;
                while (triIdx < tempCount && outCount < 4) {
                    scan = out;
                    scanIdx = 0;
                    while (scanIdx < outCount) {
                        if (scan->id == tempRead->id) {
                            break;
                        }
                        scan++;
                        scanIdx++;
                    }
                    if (scanIdx >= outCount) {
                        outSlot = (GSfieldQueryTriangle*)((u8*)out +
                                                          outCount * 0x34);
                        *outSlot = *tempRead;
                        outCount++;
                    }
                    tempRead++;
                    triIdx++;
                }
            }
        }
        regionIdx++;
        region = (GSFieldWzxRegionLocal*)((u8*)region + 0x40);
    }
    return outCount;
}
#pragma pop

/* 0x8011069C | 0x7C8 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
typedef struct GSFieldFixedMdlCell {
    u32 firstIndex;
    u32 count;
} GSFieldFixedMdlCell;

typedef struct GSFieldFixedMdlEventList {
    /* 0x00 */ GScolsys2Triangle* triangles;
    /* 0x04 */ u8 pad_04[4];
    /* 0x08 */ GSFieldFixedMdlCell* cells;
    /* 0x0C */ u32* triangleIndices;
    /* 0x10 */ u16 cellCountX;
    /* 0x12 */ u16 cellCountZ;
    /* 0x14 */ f32 minX;
    /* 0x18 */ f32 minZ;
    /* 0x1C */ f32 cellWidth;
    /* 0x20 */ f32 cellDepth;
} GSFieldFixedMdlEventList;

s32 GScolsys2ThruGetFixedMdlEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
    GScolsys2TriangleList* triList, GSfieldQueryTriangle* outTriangles) {
    extern f32 lbl_8047CF58;
    extern f32 lbl_8047CF5C;
    GSFieldFixedMdlEventList* grid;
    GSFieldFixedMdlCell* cell;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle* out;
    GSfieldQueryTriangle* scan;
    GSfieldEdgeMasks edgeMasksA;
    GSfieldEdgeMasks edgeMasksB;
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    s32 x;
    s32 z;
    s32 outCount;
    s32 scanIdx;
    u32 triIdx;
    f32 radiusSq;
    GScolsys2Vec3 planePoint;
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    u16 flags;

    grid = (GSFieldFixedMdlEventList*)triList;
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
    out = outTriangles;
    outCount = 0;

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            u32* triIndexPtr = grid->triangleIndices + cell->firstIndex;
            u32 cellTriIdx = 0;

            while (cellTriIdx < cell->count && outCount < 4) {
                s32 hit;
                tri = grid->triangles + (*triIndexPtr);

                for (scan = outTriangles, scanIdx = 0; scanIdx < outCount;
                     scan++, scanIdx++) {
                    if (tri->id == scan->id) {
                        break;
                    }
                }
                if (scanIdx >= outCount &&
                    !(PSVECDotProduct(&tri->normal, dirVec) >= lbl_8047CF58)) {
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        lbl_8047CF58) {
                        hit = 0;
                    } else {
                        GScolsy2UtilGetCpPlanePoint(&cp, &tri->normal, tri, point);
                        if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                            hit = 0;
                        } else if (GScolsy2UtilChkInTri(&cp, tri, &tri->normal) ==
                                   0) {
                            hit = 0;
                        } else {
                            hit = 1;
                        }
                    }
                    if (hit != 0) {
                        out->verts[0] = tri->verts[0];
                        out->verts[1] = tri->verts[1];
                        out->verts[2] = tri->verts[2];
                        out->normal = tri->normal;
                        out->id = tri->id;
                        out++;
                        outCount++;
                    }
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    out = outTriangles + outCount;
    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            u32* triIndexPtr = grid->triangleIndices + cell->firstIndex;
            u32 cellTriIdx = 0;

            while (cellTriIdx < cell->count && outCount < 4) {
                s32 hit;
                s32 vertIdx;
                GScolsys2Vec3* vsrc;

                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    cellTriIdx++;
                    triIndexPtr++;
                    continue;
                }
                for (scan = outTriangles, scanIdx = 0; scanIdx < outCount;
                     scan++, scanIdx++) {
                    if (tri->id == scan->id) {
                        break;
                    }
                }
                if (scanIdx >= outCount &&
                    !(PSVECDotProduct(&tri->normal, dirVec) >= lbl_8047CF58)) {
                    edgeMasksA = lbl_8047CF48;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        lbl_8047CF58) {
                        hit = 0;
                    } else {
                        vsrc = tri->verts;
                        hit = 0;
                        for (vertIdx = 0; vertIdx < 3; vertIdx++, vsrc++) {
                            if ((flags & edgeMasksA.values[vertIdx]) != 0) {
                                s32 next = vertIdx + 1;
                                f32 lineT;
                                if (next >= 3) {
                                    next = 0;
                                }
                                lineT = GScolsys2UtilGetCpLinePoint(
                                    &lineCp, vsrc, &tri->verts[next], point);
                                if (!(lineT < lbl_8047CF58) &&
                                    !(lineT > lbl_8047CF5C) &&
                                    PSVECSquareDistance(&lineCp, point) < radiusSq) {
                                    hit = 1;
                                    break;
                                }
                            }
                        }
                    }
                    if (hit != 0) {
                        out->verts[0] = tri->verts[0];
                        out->verts[1] = tri->verts[1];
                        out->verts[2] = tri->verts[2];
                        out->normal = tri->normal;
                        out->id = tri->id;
                        out++;
                        outCount++;
                    }
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    out = outTriangles + outCount;
    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells + (startX + z * grid->cellCountX);
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            u32* triIndexPtr = grid->triangleIndices + cell->firstIndex;
            u32 cellTriIdx = 0;

            while (cellTriIdx < cell->count && outCount < 4) {
                s32 hit;
                s32 vertIdx;
                GScolsys2Vec3* vsrc;

                tri = grid->triangles + (*triIndexPtr);
                if ((tri->flags & 7) == 0) {
                    cellTriIdx++;
                    triIndexPtr++;
                    continue;
                }
                for (scan = outTriangles, scanIdx = 0; scanIdx < outCount;
                     scan++, scanIdx++) {
                    if (tri->id == scan->id) {
                        break;
                    }
                }
                if (scanIdx >= outCount &&
                    !(PSVECDotProduct(&tri->normal, dirVec) >= lbl_8047CF58)) {
                    edgeMasksB = lbl_8047CF50;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        lbl_8047CF58) {
                        hit = 0;
                    } else {
                        vsrc = tri->verts;
                        hit = 0;
                        for (vertIdx = 0; vertIdx < 3; vertIdx++, vsrc++) {
                            s32 next = vertIdx + 2;

                            if (next >= 3) {
                                next -= 3;
                            }
                            if ((flags & edgeMasksB.values[vertIdx]) != 0 &&
                                (flags & edgeMasksB.values[next]) != 0 &&
                                PSVECSquareDistance(vsrc, point) < radiusSq) {
                                hit = 1;
                                break;
                            }
                        }
                    }
                    if (hit != 0) {
                        out->verts[0] = tri->verts[0];
                        out->verts[1] = tri->verts[1];
                        out->verts[2] = tri->verts[2];
                        out->normal = tri->normal;
                        out->id = tri->id;
                        out++;
                        outCount++;
                    }
                }
                cellTriIdx++;
                triIndexPtr++;
            }
        }
    }

    return outCount;
}
#pragma pop

/* 0x80110E64 | 0x60C */
#pragma push
#pragma optimizewithasm off
s32 GScolsys2ThruGetMdlEventList(GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
                GScolsys2TriangleList* triList, void* mtxInv, void* mtxFwd,
                GSfieldQueryTriangle* outTris) {
    extern f32 lbl_8047CF58;
    extern f32 lbl_8047CF5C;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle* out;
    s32 outCount;
    s32 triIdx;
    GSfieldQueryTriangle* scan;
    s32 scanIdx;
    s32 vertIdx;
    GScolsys2Vec3* vdst;
    GScolsys2Vec3* vsrc;
    f32 radiusSq;
    GSfieldEdgeMasks edgeMasksA;
    GSfieldEdgeMasks edgeMasksB;
    GScolsys2Vec3 planePoint;
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GScolsys2Vec3 verts[3];
    f32 lineT;
    u16 flags;

    radiusSq = radius * radius;
    tri = triList->triangles;
    out = outTris;
    outCount = 0;
    triIdx = 0;
    while ((u32)triIdx < triList->count && outCount < 4) {
        scan = outTris;
        for (scanIdx = 0; scanIdx < outCount; scan++, scanIdx++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (scanIdx >= outCount) {
            s32 hit;
            PSMTXMultVec(mtxFwd, &tri->normal, &planePoint);
            if (!(PSVECDotProduct(&planePoint, dirVec) >= lbl_8047CF58)) {
                vdst = verts;
                vsrc = tri->verts;
                vertIdx = 0;
                do {
                    PSMTXMultVec(mtxInv, vsrc, vdst);
                    vertIdx++;
                    vsrc++;
                    vdst++;
                } while (vertIdx < 3);
                if (GScolsy2UtilGetSidePlanePoint(&planePoint, verts, point) < lbl_8047CF58) {
                    hit = 0;
                } else {
                    GScolsy2UtilGetCpPlanePoint(&cp, &planePoint, verts, point);
                    if (PSVECSquareDistance(&cp, point) >= radiusSq) {
                        hit = 0;
                    } else if (GScolsy2UtilChkInTri(&cp, verts, &planePoint) == 0) {
                        hit = 0;
                    } else {
                        hit = 1;
                    }
                }
                if (hit != 0) {
                    u16 id = tri->id;
                    out->verts[0] = verts[0];
                    out->verts[1] = verts[1];
                    out->verts[2] = verts[2];
                    out->normal = planePoint;
                    out->id = id;
                    outCount++;
                    out++;
                }
            }
        }
        triIdx++;
        tri++;
    }

    tri = triList->triangles;
    triIdx = 0;
    out = outTris + outCount;
    while ((u32)triIdx < triList->count && outCount < 4) {
        if ((tri->flags & 7) != 0) {
            scan = outTris;
            for (scanIdx = 0; scanIdx < outCount; scan++, scanIdx++) {
                if (tri->id == scan->id) {
                    break;
                }
            }
            if (scanIdx >= outCount) {
                s32 hit;
                PSMTXMultVec(mtxFwd, &tri->normal, &planePoint);
                if (!(PSVECDotProduct(&planePoint, dirVec) >= lbl_8047CF58)) {
                    vdst = verts;
                    vsrc = tri->verts;
                    vertIdx = 0;
                    do {
                        PSMTXMultVec(mtxInv, vsrc, vdst);
                        vertIdx++;
                        vsrc++;
                        vdst++;
                    } while (vertIdx < 3);
                    edgeMasksA = lbl_8047CF48;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&planePoint, verts, point) < lbl_8047CF58) {
                        hit = 0;
                    } else {
                        vsrc = verts;
                        for (vertIdx = 0; vertIdx < 3; vertIdx++, vsrc++) {
                            if ((flags & edgeMasksA.values[vertIdx]) != 0) {
                                s32 next = vertIdx + 1;
                                if (next >= 3) {
                                    next = 0;
                                }
                                lineT = GScolsys2UtilGetCpLinePoint(&lineCp, vsrc, &verts[next], point);
                                if (!(lineT < lbl_8047CF58 || lineT > lbl_8047CF5C)
                                    && PSVECSquareDistance(&lineCp, point) < radiusSq) {
                                    hit = 1;
                                    goto edge_test_done;
                                }
                            }
                        }
                        hit = 0;
                    }
                edge_test_done:
                    if (hit != 0) {
                        u16 id = tri->id;
                        out->verts[0] = verts[0];
                        out->verts[1] = verts[1];
                        out->verts[2] = verts[2];
                        out->normal = planePoint;
                        out->id = id;
                        outCount++;
                        out++;
                    }
                }
            }
        }
        triIdx++;
        tri++;
    }

    tri = triList->triangles;
    triIdx = 0;
    out = outTris + outCount;
    while ((u32)triIdx < triList->count && outCount < 4) {
        if ((tri->flags & 7) != 0) {
            scan = outTris;
            for (scanIdx = 0; scanIdx < outCount; scan++, scanIdx++) {
                if (tri->id == scan->id) {
                    break;
                }
            }
            if (scanIdx >= outCount) {
                s32 hit;
                PSMTXMultVec(mtxFwd, &tri->normal, &planePoint);
                if (!(PSVECDotProduct(&planePoint, dirVec) >= lbl_8047CF58)) {
                    vdst = verts;
                    vsrc = tri->verts;
                    vertIdx = 0;
                    do {
                        PSMTXMultVec(mtxInv, vsrc, vdst);
                        vertIdx++;
                        vsrc++;
                        vdst++;
                    } while (vertIdx < 3);
                    edgeMasksB = lbl_8047CF50;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&planePoint, verts, point) < lbl_8047CF58) {
                        hit = 0;
                    } else {
                        vsrc = verts;
                        for (vertIdx = 0; vertIdx < 3; vertIdx++, vsrc++) {
                            s32 next = vertIdx + 2;
                            if (next >= 3) {
                                next -= 3;
                            }
                            if ((flags & edgeMasksB.values[vertIdx]) != 0 && (flags & edgeMasksB.values[next]) != 0) {
                                if (PSVECSquareDistance(vsrc, point) < radiusSq) {
                                    hit = 1;
                                    goto vertex_test_done;
                                }
                            }
                        }
                        hit = 0;
                    }
                vertex_test_done:
                    if (hit != 0) {
                        u16 id = tri->id;
                        out->verts[0] = verts[0];
                        out->verts[1] = verts[1];
                        out->verts[2] = verts[2];
                        out->normal = planePoint;
                        out->id = id;
                        outCount++;
                        out++;
                    }
                }
            }
        }
        triIdx++;
        tri++;
    }
    return outCount;
}
#pragma pop

/* 0x80111470 | 0x1CC */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 GScolsys2ThruGetEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec,
    GSfieldQueryTriangle* out, f32 radius) {
    GSFieldWzxData* wzx;
    GSFieldWzxRegion* region;
    GSfieldQueryTriangle temporary[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    s32 enabled;
    s32 resultCount = 0;
    u32 regionIndex;

    wzx = (GSFieldWzxData*)fn_8010CBC0();
    region = wzx->regions;
    for (regionIndex = 0;
         regionIndex < wzx->regionCount && resultCount < 4;
         regionIndex++, region++) {
        GScolsys2TriangleList* list;
        GSfieldQueryTriangle* temporaryEntry;
        GSfieldQueryTriangle* outEntry;
        s32 temporaryCount;
        s32 i;

        GScolsys2GetObjEnable(regionIndex, &enabled);
        if (enabled == 0) {
            continue;
        }
        list = *(GScolsys2TriangleList**)((u8*)region + 0x2C);
        if (list == NULL) {
            continue;
        }
        if ((*(u16*)((u8*)region + 0x3C) & 1) != 0) {
            fn_8010CA30(mtxInv, regionIndex);
            fn_8010C8D0(mtxFwd, regionIndex);
            temporaryCount = GScolsys2ThruGetMdlEventList(
                point, dirVec, radius, list, mtxInv, mtxFwd, temporary);
        } else {
            temporaryCount = GScolsys2ThruGetFixedMdlEventList(
                point, dirVec, radius, list, temporary);
        }
        temporaryEntry = temporary;
        for (i = 0; i < temporaryCount && resultCount < 4;
             i++, temporaryEntry++) {
            s32 j;
            outEntry = out;
            for (j = 0; j < resultCount; j++, outEntry++) {
                if (outEntry->id == temporaryEntry->id) {
                    break;
                }
            }
            if (j == resultCount) {
                *outEntry = *temporaryEntry;
                resultCount++;
            }
        }
    }
    return resultCount;
}
#pragma pop

/* 0x8011163C | 0x228 */
s32 GScolsys2ThruGetEventID(
    GScolsys2Vec3* start, GScolsys2Vec3* end,
    f32 radius, GSfieldQueryTriangle* out)
{
    extern f32 PSVECMag(void*);
    extern const f32 lbl_8047CF58;
    extern const f32 lbl_8047CF5C;
    GSfieldQueryTriangle temporary[4];
    GScolsys2Vec3 direction;
    GScolsys2Vec3 point;
    f32 length;
    f32 position;
    f32 step;
    f32 sample;
    s32 outCount;

    outCount = 0;
    if (fn_8010CBC0() == NULL) {
        return 0;
    }

    PSVECSubtract(end, start, &direction);
    length = PSVECMag(&direction);
    if (length <= lbl_8047CF58) {
        return 0;
    }
    step = radius / length;
    if (step > lbl_8047CF5C) {
        step = lbl_8047CF5C;
    }

    position = lbl_8047CF58;
    while (position < lbl_8047CF5C && outCount < 4) {
        s32 temporaryCount;
        s32 i;

        sample = position + step;
        if (sample > lbl_8047CF5C) {
            sample = lbl_8047CF5C;
        }
        PSVECScale(&direction, &point, sample);
        PSVECAdd(&point, start, &point);
        temporaryCount =
            GScolsys2ThruGetEventList(&point, &direction, temporary, radius);

        for (i = 0; i < temporaryCount && outCount < 4; i++) {
            s32 j;
            for (j = 0; j < outCount; j++) {
                if (temporary[i].id == out[j].id) {
                    break;
                }
            }
            if (j == outCount) {
                out[outCount++] = temporary[i];
            }
        }
        if (step <= lbl_8047CF58) {
            break;
        }
        position += step;
    }
    return outCount;
}
