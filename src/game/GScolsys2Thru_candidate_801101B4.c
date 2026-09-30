/**
 * @file GScolsys2Thru_candidate_801101B4.c
 * @brief GScolsys2Thru -- spatial grid / "thru" collision queries.
 *
 * Candidate for the GScolsys2Thru TU, 0x801101B4 - 0x80111470, with the
 * .sdata2 pool 0x8047CF48-0x8047CF60 (still in sdata2_8047CF48.c).
 * GScolsys2ThruGetEventList (0x80111470) is linked in
 * GScolsys2Thru_exact_80111470.c; GScolsys2ThruGetEventID (0x8011163C) is
 * GScolsys2Thru_candidate_8011163C.c, and links with this range because
 * both read the pool's 0.0f/1.0f. fn_80110084 belongs
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
 * Address range: 0x801101B4 - 0x80111470
 *
 * Lane D10 (2026-09-30): GScolsys2ThruGetEventList and
 * GScolsys2ThruGetEventID are instruction-exact. Both use the pool's
 * 0.0f/1.0f as literals, which the unit will own when it links, and a
 * `>=` duplicate test. GScolsys2ThruGetMdlEventList (97.9%) now follows
 * XD's structure: the stripped getCpPointPoly / getCpPointLine /
 * getCpPointPoint / addList inlines (NXXJ01.map GScolsys2Thru.o). What is
 * left there is per-pass register colouring.
 *
 * Lane D13 (2026-09-30): fn_801101B4 87.1% -> 99.16%, rewritten in XD's
 * shape (GXXE01.map GScolsys2Thru.o: GScolsys2ThruPassEventID 0x3B0 with
 * getMdlEventListPass 0x200, which Colosseum inlines): a model pass and a
 * fixed pass (each with its own direction and hit locals, as the stack
 * layout shows), a shared segment test, addList, and the merge through a
 * post-incremented index. What is left: the two strength-reduced pointers
 * of the vertex-transform loop take r15/r16 the other way round (retail
 * numbers the source pointer first; no source form tried changes it).
 *
 * GScolsys2ThruGetFixedMdlEventList 58.5% -> 99.29%: XD getFixedMdlEventList,
 * the three passes of GetMdlEventList over the grid cells the sphere
 * touches. The grid has the walk grid's layout (cell size at 0x14, origin at
 * 0x1C); each base (cells, indices, triangles) is loaded into its pointer
 * before the offset is added; the duplicate test indexes the output list.
 * GScolsys2ThruGetMdlEventList 97.9% -> 99.24% with the same indexed
 * duplicate test and addList(&outTris[outCount], ...). What is left in all
 * three: the vertex-transform loop's pointer pair (fn_801101B4,
 * GetMdlEventList), the grid bounds' float registers (GetFixedMdl), and
 * the pool labels, which match once the unit owns the pool.
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
typedef struct GSFieldWzxRegionLocal {
    u8 pad_00[0x2C];
    GScolsys2TriangleList* triangles;
    u8 pad_30[0x0C];
    u16 flags;
    u8 pad_3E[2];
} GSFieldWzxRegionLocal;

/* The segment test of one triangle: the plane crossing must lie on the
 * segment and inside the triangle. */
static inline s32 checkPolyLine(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                GScolsys2Vec3* verts, GScolsys2Vec3* normal)
{
    GScolsys2Vec3 hitPoint;
    f32 t;

    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)&hitPoint, &t, (Vec3f*)normal,
                                    (Vec3f*)verts, (Vec3f*)start,
                                    (Vec3f*)end) == 0) {
        return 0;
    }
    if (t < 0.0f || t > 1.0f) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(&hitPoint, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

/* XD getMdlEventListPass (0x200): the transformed-model pass. */
static inline s32 getMdlEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                      GScolsys2TriangleList* list, f32* mtxInv,
                                      f32* mtxFwd, GSfieldQueryTriangle* out)
{
    GScolsys2Vec3 normal;
    GScolsys2Vec3 dirVec;
    GScolsys2Vec3 verts[3];
    GSfieldQueryTriangle* top;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* scan;
    GScolsys2Triangle* tri;
    u32 i;
    s32 j;
    s32 v;
    s32 count;

    count = 0;
    PSVECSubtract(end, start, &dirVec);
    top = out;
    tri = list->triangles;
    write = top;
    for (i = 0; i < list->count && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, &dirVec) >= 0.0f) {
            continue;
        }
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(mtxInv, &tri->verts[v], &verts[v]);
        }
        if (checkPolyLine(start, end, verts, &normal)) {
            addList(write, verts, &normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

/* The fixed-model pass (inline in XD GScolsys2ThruPassEventID). */
static inline s32 getFixedMdlEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                           GScolsys2TriangleList* list,
                                           GSfieldQueryTriangle* out)
{
    GScolsys2Vec3 dirVec;
    GScolsys2Triangle* tri;
    u32 i;
    s32 count;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* top;
    GSfieldQueryTriangle* scan;
    s32 j;

    count = 0;
    PSVECSubtract(end, start, &dirVec);
    top = out;
    tri = list->triangles;
    write = top;
    for (i = 0; i < list->count && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        if (PSVECDotProduct(&tri->normal, &dirVec) >= 0.0f) {
            continue;
        }
        if (checkPolyLine(start, end, tri->verts, &tri->normal)) {
            addList(write, tri->verts, &tri->normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

/* XD GScolsys2ThruPassEventID (0x3B0 + getMdlEventListPass 0x200). */
s32 fn_801101B4(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out)
{
    GSfieldQueryTriangle temp[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    GSFieldWzxData* wzx;
    GSFieldWzxRegionLocal* region;
    GScolsys2TriangleList* list;
    GSfieldQueryTriangle* scan;
    GSfieldQueryTriangle* read;
    s32 j;
    s32 k;
    s32 enabled;
    u32 i;
    s32 tempCount;
    s32 outCount;

    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    if (wzx == NULL) {
        return 0;
    }
    region = (GSFieldWzxRegionLocal*)wzx->regions;
    for (i = 0; i < wzx->regionCount && outCount < 4; i++, region++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        list = region->triangles;
        if (list == NULL) {
            continue;
        }
        if ((region->flags & 1) != 0) {
            fn_8010CA30(mtxInv, i);
            fn_8010C8D0(mtxFwd, i);
            tempCount = getMdlEventListPass(start, end, list, mtxInv, mtxFwd, temp);
        } else {
            tempCount = getFixedMdlEventListPass(start, end, list, temp);
        }
        for (read = temp, j = 0; j < tempCount && outCount < 4; read++, j++) {
            for (scan = out, k = 0; k < outCount; scan++, k++) {
                if (scan->id == read->id) {
                    break;
                }
            }
            if (k >= outCount) {
                out[outCount++] = *read;
            }
        }
    }
    return outCount;
}

/* 0x8011069C | 0x7C8 */
typedef struct GSFieldFixedMdlCell {
    u32 firstIndex;
    u32 count;
} GSFieldFixedMdlCell;

/* The fixed model's triangle grid (the walk grid's layout, GScolsys2Walk.c). */
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

/* XD getFixedMdlEventList (0x7C8): the three passes of
 * GScolsys2ThruGetMdlEventList over the grid cells the sphere touches. */
s32 GScolsys2ThruGetFixedMdlEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
    GScolsys2TriangleList* triList, GSfieldQueryTriangle* outTris)
{
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GSFieldFixedMdlEventList* grid;
    GSfieldQueryTriangle* scan;
    f32 radiusSq;
    GScolsys2Vec3 lo;
    GScolsys2Vec3 hi;
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    GScolsys2Triangle* tri;
    u32 j;
    GSFieldFixedMdlCell* cell;
    u32* index;
    s32 x;
    s32 z;
    s32 k;
    s32 outCount;

    grid = (GSFieldFixedMdlEventList*)triList;
    outCount = 0;
    lo.x = point->x - radius;
    lo.z = point->z - radius;
    hi.x = point->x + radius;
    hi.z = point->z + radius;
    startX = (lo.x - grid->minX) / grid->cellWidth;
    if (startX < 0) {
        startX = 0;
    }
    startZ = (lo.z - grid->minZ) / grid->cellDepth;
    if (startZ < 0) {
        startZ = 0;
    }
    endX = (hi.x - grid->minX) / grid->cellWidth;
    if (endX > grid->cellCountX - 1) {
        endX = grid->cellCountX - 1;
    }
    endZ = (hi.z - grid->minZ) / grid->cellDepth;
    if (endZ > grid->cellCountZ - 1) {
        endZ = grid->cellCountZ - 1;
    }
    radiusSq = radius * radius;

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointPoly(&cp, point, radiusSq, tri->verts, &tri->normal)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointLine(&lineCp, point, radiusSq, tri->verts, &tri->normal,
                                   tri->flags)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }

    for (z = startZ; z <= endZ && outCount < 4; z++) {
        cell = grid->cells;
        cell += startX + z * grid->cellCountX;
        for (x = startX; x <= endX && outCount < 4; x++, cell++) {
            index = grid->triangleIndices;
            index += cell->firstIndex;
            for (j = 0; j < cell->count && outCount < 4; j++, index++) {
                tri = grid->triangles;
                tri += *index;
                if (!(tri->flags & 7)) {
                    continue;
                }
                for (k = 0; k < outCount; k++) {
                    if (tri->id == outTris[k].id) {
                        break;
                    }
                }
                if (k < outCount) {
                    continue;
                }
                if (PSVECDotProduct(&tri->normal, dirVec) >= 0.0f) {
                    continue;
                }
                if (getCpPointPoint(point, radiusSq, tri->verts, &tri->normal,
                                    tri->flags)) {
                    addList(&outTris[outCount], tri->verts, &tri->normal, tri->id);
                    outCount++;
                }
            }
        }
    }
    return outCount;
}

/* 0x80110E64 | 0x60C */
s32 GScolsys2ThruGetMdlEventList(GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
                GScolsys2TriangleList* triList, void* mtxInv, void* mtxFwd,
                GSfieldQueryTriangle* outTris) {
    GScolsys2Vec3 verts[3];
    GScolsys2Vec3 normal;
    GScolsys2Vec3 cp;
    GScolsys2Vec3 lineCp;
    GScolsys2Triangle* tri;
    GSfieldQueryTriangle* scan;
    f32 radiusSq;
    s32 outCount;
    u32 i;
    s32 k;
    s32 v;

    radiusSq = radius * radius;
    outCount = 0;
    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
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
            addList(&outTris[outCount], verts, &normal, tri->id);
            outCount++;
        }
    }

    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
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
            addList(&outTris[outCount], verts, &normal, tri->id);
            outCount++;
        }
    }

    tri = triList->triangles;
    for (i = 0; i < triList->count && outCount < 4; i++, tri++) {
        if (!(tri->flags & 7)) {
            continue;
        }
        for (k = 0; k < outCount; k++) {
            if (tri->id == outTris[k].id) {
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
            addList(&outTris[outCount], verts, &normal, tri->id);
            outCount++;
        }
    }
    return outCount;
}
