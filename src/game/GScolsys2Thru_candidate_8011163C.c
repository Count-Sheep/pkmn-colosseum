/**
 * @file GScolsys2Thru_candidate_8011163C.c
 * @brief GScolsys2Thru: GScolsys2ThruGetEventID, 0x8011163C - 0x80111864
 *        (candidate only; not linked).
 *
 * Instruction-exact (lane D10). It samples the segment start-end at radius
 * steps and merges GScolsys2ThruGetEventList's results. Its 0.0f / 1.0f
 * are the Thru TU's pool literals (0x8047CF58 / 0x8047CF5C); retail
 * reloads them at each use, which only literals give (an extern stand-in
 * is CSE'd, 98.3%). The rest of the TU (GScolsys2Thru_candidate_801101B4.c)
 * reads the same two constants by name, so this function can link only
 * together with it, when the unit owns the pool.
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

s32 GScolsys2ThruGetEventList(
    GScolsys2Vec3* point, GScolsys2Vec3* dirVec,
    GSfieldQueryTriangle* out, f32 radius);

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
