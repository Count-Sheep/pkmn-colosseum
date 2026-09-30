/**
 * @file GScolsys2Thru_exact_80111470.c
 * @brief GScolsys2Thru: GScolsys2ThruGetEventList, 0x80111470 - 0x8011163C.
 *
 * Carved out of the GScolsys2Thru candidate (lane D10 made it exact there).
 * It gathers up to four "thru" event triangles at a point from every
 * enabled object, through the model or fixed-grid query, dropping
 * duplicate ids. It uses no pool literals, so it links on its own.
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

s32 GScolsys2ThruGetFixedMdlEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
    GScolsys2TriangleList* triList, GSfieldQueryTriangle* outTriangles);
s32 GScolsys2ThruGetMdlEventList(GScolsys2Vec3* point, GScolsys2Vec3* dirVec, f32 radius,
                GScolsys2TriangleList* triList, void* mtxInv, void* mtxFwd,
                GSfieldQueryTriangle* outTris);

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
