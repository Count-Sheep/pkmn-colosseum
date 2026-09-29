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
 *
 * Lane D10 (2026-09-30): GScolsys2ThruGetEventList and
 * GScolsys2ThruGetEventID are instruction-exact. Both use the pool's
 * 0.0f/1.0f as literals, which the unit will own when it links, and a
 * `>=` duplicate test. GScolsys2ThruGetMdlEventList (97.9%) now follows
 * XD's structure: the stripped getCpPointPoly / getCpPointLine /
 * getCpPointPoint / addList inlines (NXXJ01.map GScolsys2Thru.o). What is
 * left there is per-pass register colouring.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* XD addList (UNUSED 0x70): append one triangle to the result list. */
static inline void addList(GSfieldQueryTriangle* out, GScolsys2Vec3* verts,
                           GScolsys2Vec3* normal, u16 id)
{
    out->verts[0] = verts[0];
    out->verts[1] = verts[1];
    out->verts[2] = verts[2];
    out->normal = *normal;
    out->id = id;
}

/*
 * XD GScolsys2Thru.o's stripped helpers (NXXJ01.map lines 6908-6913, all
 * UNUSED): getCpPointPoly (0xF8), getCpPointLine (0x124) and
 * getCpPointPoint (0x110). Each tests one triangle against the point for
 * the face, the flagged edges and the flagged corners, respectively.
 */
static inline s32 getCpPointPoly(GScolsys2Vec3* cp, GScolsys2Vec3* point, f32 radiusSq,
                                 GScolsys2Vec3* verts, GScolsys2Vec3* normal)
{
    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    GScolsy2UtilGetCpPlanePoint(cp, normal, verts, point);
    if (PSVECSquareDistance(cp, point) >= radiusSq) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(cp, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

static inline s32 getCpPointLine(GScolsys2Vec3* cp, GScolsys2Vec3* point, f32 radiusSq,
                                 GScolsys2Vec3* verts, GScolsys2Vec3* normal, u16 flags)
{
    u16 masks[3] = {1, 2, 4};
    s32 i;
    s32 next;
    f32 t;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        if (flags & masks[i]) {
            next = i + 1;
            if (next >= 3) {
                next = 0;
            }
            t = GScolsys2UtilGetCpLinePoint(cp, &verts[i], &verts[next], point);
            if (t < 0.0f || t > 1.0f) {
                continue;
            }
            if (PSVECSquareDistance(cp, point) < radiusSq) {
                return 1;
            }
        }
    }
    return 0;
}

static inline s32 getCpPointPoint(GScolsys2Vec3* point, f32 radiusSq,
                                  GScolsys2Vec3* verts, GScolsys2Vec3* normal, u16 flags)
{
    u16 masks[3] = {1, 2, 4};
    s32 i;
    s32 prev;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        prev = i + 2;
        if (prev >= 3) {
            prev -= 3;
        }
        if ((flags & masks[i]) && (flags & masks[prev])) {
            if (PSVECSquareDistance(&verts[i], point) < radiusSq) {
                return 1;
            }
        }
    }
    return 0;
}


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
                                  0.0f))
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
                                } else if (resultT < 0.0f ||
                                           resultT > 1.0f)
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
                              0.0f))
                        {
                            if (GScolsys2UtilGetCpPlaneLine(
                                    (Vec3f*)&hitPoint, &resultT,
                                    (Vec3f*)&tri->normal, (Vec3f*)tri,
                                    (Vec3f*)start, (Vec3f*)end) == 0)
                            {
                                hit = 0;
                            } else if (resultT < 0.0f ||
                                       resultT > 1.0f)
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
                    !(PSVECDotProduct(&tri->normal, dirVec) >= 0.0f)) {
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        0.0f) {
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
                    !(PSVECDotProduct(&tri->normal, dirVec) >= 0.0f)) {
                    edgeMasksA = lbl_8047CF48;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        0.0f) {
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
                                if (!(lineT < 0.0f) &&
                                    !(lineT > 1.0f) &&
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
                    !(PSVECDotProduct(&tri->normal, dirVec) >= 0.0f)) {
                    edgeMasksB = lbl_8047CF50;
                    flags = tri->flags;
                    if (GScolsy2UtilGetSidePlanePoint(&tri->normal, tri, point) <
                        0.0f) {
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
s32 GScolsys2ThruGetMdlEventList(GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
                GScolsys2TriangleList* triList, void* mtxInv, void* mtxFwd,
                GSfieldQueryTriangle* outTris) {
    GScolsys2Vec3 verts[3];
    GScolsys2Vec3 normal;
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle* out;
    GSfieldQueryTriangle* scan;
    f32 radiusSq;
    s32 outCount;
    u32 i;
    s32 k;
    s32 v;

    radiusSq = radius * radius;
    outCount = 0;
    tri = triList->triangles;
    out = outTris;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        for (k = 0, scan = outTris; k < outCount; scan++, k++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (getCpPointPoly(&cp, point, radiusSq, verts, &normal)) {
            addList(out, verts, &normal, tri->id);
            outCount++;
            out++;
        }
    }

    tri = triList->triangles;
    out = &outTris[outCount];
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0, scan = outTris; k < outCount; scan++, k++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (getCpPointLine(&lineCp, point, radiusSq, verts, &normal, tri->flags)) {
            addList(out, verts, &normal, tri->id);
            outCount++;
            out++;
        }
    }

    tri = triList->triangles;
    out = &outTris[outCount];
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0, scan = outTris; k < outCount; scan++, k++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (k < outCount) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (getCpPointPoint(point, radiusSq, verts, &normal, tri->flags)) {
            addList(out, verts, &normal, tri->id);
            outCount++;
            out++;
        }
    }
    return outCount;
}

/* 0x80111470 | 0x1CC */
s32 GScolsys2ThruGetEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec,
    GSfieldQueryTriangle* out, f32 radius) {
    GSfieldQueryTriangle temporary[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    s32 enabled;
    GSFieldWzxData* wzx;
    GSFieldWzxRegion* region;
    GScolsys2TriangleList* list;
    u32 regionIndex;
    s32 temporaryCount;
    s32 outCount;
    s32 i;
    s32 j;

    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    region = wzx->regions;
    for (regionIndex = 0; regionIndex < wzx->regionCount && outCount < 4;
         regionIndex++, region++) {
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
        for (i = 0; i < temporaryCount && outCount < 4; i++) {
            for (j = 0; j < outCount; j++) {
                if (out[j].id == temporary[i].id) {
                    break;
                }
            }
            if (j >= outCount) {
                out[outCount++] = temporary[i];
            }
        }
    }
    return outCount;
}

/* 0x8011163C | 0x228 */
s32 GScolsys2ThruGetEventID(
    GScolsys2Vec3* start, GScolsys2Vec3* end,
    f32 radius, GSfieldQueryTriangle* out)
{
    extern f32 PSVECMag(void*);
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
    if (length <= 0.0f) {
        return 0;
    }
    step = radius / length;
    if (step > 1.0f) {
        step = 1.0f;
    }

    position = 0.0f;
    while (position < 1.0f && outCount < 4) {
        s32 temporaryCount;
        s32 i;

        sample = position + step;
        if (sample > 1.0f) {
            sample = 1.0f;
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
            if (j >= outCount) {
                out[outCount++] = temporary[i];
            }
        }
        if (step <= 0.0f) {
            break;
        }
        position += step;
    }
    return outCount;
}
