/**
 * @file GScolsys2Thru_candidate_8011163C.c
 * @brief GScolsys2Thru: GScolsys2ThruGetEventID, 0x8011163C - 0x80111864.
 *
 * Text-only carve (lane D10 made it instruction-exact). It samples the
 * segment start-end at radius steps and merges GScolsys2ThruGetEventList's
 * results.
 *
 * RULE-EXCEPTION(user-approved): extern named stand-ins for the Thru TU's
 * own pool literals, one read through a pointer cast - see
 * docs/RULE_EXCEPTIONS.md. The 0.0f / 1.0f are the pool entries 0x8047CF58 /
 * 0x8047CF5C (defined in sdata2_8047CF48.c), which the unlinked rest of the
 * TU (GScolsys2Thru_candidate_801101B4.c) reads by name, so this carve cannot
 * own them. Retail reloads the literals at each use; plain extern reads are
 * CSE'd (98.4%). Reading the loop's starting 0.0f through a pointer cast
 * keeps it a separate load, as the literal is. The clean form needs the
 * whole Thru TU linked with its pool.
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

/* RULE-EXCEPTION(user-approved): pool stand-ins - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047CF58; /* 0.0f */
extern const f32 lbl_8047CF5C; /* 1.0f */

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
    if (length <= lbl_8047CF58) {
        return 0;
    }
    step = radius / length;
    if (step > lbl_8047CF5C) {
        step = lbl_8047CF5C;
    }

    position = *(const f32*)&lbl_8047CF58;
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
            if (j >= outCount) {
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
