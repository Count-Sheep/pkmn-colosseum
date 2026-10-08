/**
 * @file GScolsys2Thru_r56_80111864_o4s.c
 * @brief fn_80111864, candidate only (score instrumentation chunk).
 *
 * By .sdata2 ownership fn_80111864 belongs to the GScolsys2Check TU: it
 * and GScolsys2CheckGetEventID share the pool 0x8047CF60-0x8047CF68,
 * separate from GScolsys2Thru's 0x8047CF48-0x8047CF60 (which has its
 * own 0.0f/1.0f).
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

extern f32 PSVECDotProduct(void* a, void* b);
extern s32 GScolsy2UtilChkInTri(void* point, void* verts, void* normal);
extern f32 lbl_8047CF60; /* 0.0f */
extern f32 lbl_8047CF64; /* 1.0f */

/* Append one triangle to the result list (GScolsys2Thru's addList). */
static inline void addEventList(GSfieldQueryTriangle* out, GScolsys2Vec3* verts,
                                GScolsys2Vec3* normal, u16 id)
{
    out->verts[0] = verts[0];
    out->verts[1] = verts[1];
    out->verts[2] = verts[2];
    out->normal = *normal;
    out->id = id;
}

/* The segment test of one triangle (GScolsys2Thru's checkPolyLine). */
static inline s32 checkEventPolyLine(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                     GScolsys2Vec3* verts, GScolsys2Vec3* normal)
{
    GScolsys2Vec3 hitPoint;
    f32 t;

    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)&hitPoint, &t, (Vec3f*)normal,
                                    (Vec3f*)verts, (Vec3f*)start,
                                    (Vec3f*)end) == 0) {
        return 0;
    }
    if (t < lbl_8047CF60 || t > lbl_8047CF64) {
        return 0;
    }
    if (GScolsy2UtilChkInTri(&hitPoint, verts, normal) == 0) {
        return 0;
    }
    return 1;
}

static inline void eventXformVertsBody(f32* m, GSfieldQueryTriangle* tri, GScolsys2Vec3* out)
{
    GScolsys2Vec3* src;
    s32 v;

    src = tri->verts;
    for (v = 0; v < 3; v++, src++) {
        PSMTXMultVec(m, src, &out[v]);
    }
}

static inline void eventXformVerts(f32* m, GSfieldQueryTriangle* tri, GScolsys2Vec3* out)
{
    eventXformVertsBody(m, tri, out);
}

/* The transformed-model pass of one region against the shared direction. */
static inline s32 getEventListPass(GScolsys2Vec3* start, GScolsys2Vec3* end,
                                   GScolsys2Vec3* dirVec, GSFieldWzxTriangleList* list,
                                   f32* mtxInv, f32* mtxFwd, GSfieldQueryTriangle* out)
{
    GScolsys2Vec3 normal;
    GScolsys2Vec3 verts[3];
    GSfieldQueryTriangle* top;
    GScolsys2Vec3* dst;
    GSfieldQueryTriangle* write;
    GSfieldQueryTriangle* scan;
    GSfieldQueryTriangle* tri;
    u32 i;
    s32 j;
    s32 count;

    count = 0;
    top = out;
    tri = (GSfieldQueryTriangle*)list->triangles;
    write = top;
    for (i = 0; i < list->triangleCount && count < 4; i++, tri++) {
        for (j = 0, scan = top; j < count; scan++, j++) {
            if (tri->id == scan->id) {
                break;
            }
        }
        if (j < count) {
            continue;
        }
        PSMTXMultVec(mtxFwd, &tri->normal, &normal);
        if (PSVECDotProduct(&normal, dirVec) >= lbl_8047CF60) {
            continue;
        }
        dst = verts;
        eventXformVerts(mtxInv, tri, dst);
        if (checkEventPolyLine(start, end, verts, &normal)) {
            addEventList(write, dst, &normal, tri->id);
            write++;
            count++;
        }
    }
    return count;
}

/* 0x80111864 | 0x338 */
s32 fn_80111864(GScolsys2Vec3* start, GScolsys2Vec3* end,
                GSfieldQueryTriangle* out)
{
    GSfieldQueryTriangle temp[4];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    GScolsys2Vec3 dirVec;
    GSFieldWzxData* wzx;
    GSFieldWzxTriangleList* list;
    GSfieldQueryTriangle* temporary;
    GSfieldQueryTriangle* scan;
    s32 j;
    s32 k;
    s32 enabled;
    u32 i;
    s32 tempCount;
    s32 outCount;
    GSFieldWzxRegion* region;

    outCount = 0;
    wzx = (GSFieldWzxData*)fn_8010CBC0();
    PSVECSubtract(end, start, &dirVec);
    region = wzx->regions;
    for (i = 0; i < wzx->regionCount && outCount < 4; i++, region++) {
        GScolsys2GetObjEnable(i, &enabled);
        if (enabled == 0) {
            continue;
        }
        list = region->floorTriangles;
        if (list == NULL) {
            continue;
        }
        fn_8010CA30(mtxInv, i);
        fn_8010C8D0(mtxFwd, i);
        temporary = temp;
        tempCount = getEventListPass(start, end, &dirVec, list, mtxInv, mtxFwd,
                                     temporary);
        for (j = 0; j < tempCount && outCount < 4; temporary++, j++) {
            for (scan = out, k = 0; k < outCount; scan++, k++) {
                if (scan->id == temporary->id) {
                    break;
                }
            }
            if (k >= outCount) {
                out[outCount++] = *temporary;
            }
        }
    }
    return outCount;
}
