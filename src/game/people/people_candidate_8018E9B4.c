/*
 * people TU: fn_8018E9B4, a person's collision-checked move.
 *
 * Exact, but a CodeCandidate: it reads the TU's pooled literals (0.0f, 3.0f,
 * 10.0f, 8.5f, -1000000.0f). Declared as extern constants instead, the
 * step limit 10.0f takes a different FP register (the literal load is
 * treated as rematerialisable), so the match needs the literal and the
 * literal needs the TU's .sdata2, which the rest of the TU reads by symbol.
 */
#include "game/people/people.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

/* One floor height hit from fn_8010E138. */
typedef struct PeopleFloorHit {
    f32 height;
    f32 field_04;
    f32 field_08;
} PeopleFloorHit;

extern void* GSresGetResource(u32 group, u32 id);
extern u32 fn_800F7BC4(s32 pad);
extern void fn_80101B90(u32 color);
extern void GSvecCopy(void* dst, void* src);
extern void PSVECSubtract(void* a, void* b, void* out);
extern void PSVECAdd(void* a, void* b, void* out);
extern s32 GScolsys2ThruGetEventID(void* from, void* to, void* events, f32 radius);
extern s32 GScolsys2HumanCollision(s32 id, void* from, void* to, void* hit);
extern s32 fn_8010F320(void* from, void* to, void* hit, f32 radius);
extern s32 fn_801101B4(void* from, void* to, void* events);
extern s32 fn_8010E138(void* position, PeopleFloorHit* hits);
extern void heroMoveSetEventList(u8 kind, void* events, s32 count);
extern f32 fn_8018F5E4(const PeopleInfoBiosEntry* info);
extern void* peopleInfoBiosGetPtr(void* scriptObj);
extern void fn_8018FC74(PeopleEntry* entry, void* position);

/*
 * Move a person from `transform` (its last position) to `position`, applying
 * the collision checks its flags ask for; `position` receives the result
 * and the model is placed there. Always returns 1.
 */
s32 fn_8018E9B4(PeopleEntry* entry, GSvec* position, GSvec* transform)
{
    u8 events[0xD0];
    PeopleFloorHit hits[8];
    GSvec last;
    GSvec result;
    GSvec from;
    GSvec to;
    GSvec hit;
    GSvec delta;
    PeopleInfoBiosEntry* info;
    u8* resource;
    f32 radius;
    f32 bestAny;
    f32 bestStep;
    s32 count;
    s32 i;
    BOOL foundStep;

    resource = GSresGetResource(0, 2);
    if (resource != NULL && resource[1] != 0 && (fn_800F7BC4(1) & 0x200)) {
        fn_8018FC74(entry, position);
        return 1;
    }

    fn_80101B90(0xFF);
    if (peopleTestFlags(entry, 0x700)) {
        result = *position;
        last = *transform;
        GSvecCopy(&from, &last);
        GSvecCopy(&to, &result);
        from.y += 8.5f;
        to.y += 8.5f;

        info = peopleInfoBiosGetPtr(entry->scriptRef);
        if (info == NULL) {
            radius = 3.0f;
        } else {
            radius = fn_8018F5E4(info);
        }

        if (peopleTestFlags(entry, 0x800)) {
            count = GScolsys2ThruGetEventID(&from, &to, events, radius);
            heroMoveSetEventList(2, events, count);
        }

        result = to;
        if (peopleTestFlags(entry, 0x400)) {
            if (GScolsys2HumanCollision(entry->shadowId, &last, &result, &hit) == 6) {
                result = hit;
            }
        }

        if (peopleTestFlags(entry, 0x100)) {
            if (fn_8010F320(&from, &result, &hit, radius) != 0) {
                PSVECSubtract(&hit, &result, &delta);
                PSVECAdd(&result, &delta, &result);
            }
        }

        if (peopleTestFlags(entry, 0x800)) {
            count = fn_801101B4(&from, &result, events);
            heroMoveSetEventList(1, events, count);
        }

        if (peopleTestFlags(entry, 0x200)) {
            count = fn_8010E138(&result, hits);
            if (count >= 2) {
                bestStep = -1000000.0f;
                bestAny = bestStep;
                foundStep = FALSE;
                for (i = 0; i < count; i++) {
                    if (bestAny < hits[i].height) {
                        bestAny = hits[i].height;
                    }
                    if (hits[i].height - result.y >= 10.0f) {
                        continue;
                    }
                    if (bestStep < hits[i].height) {
                        bestStep = hits[i].height;
                        foundStep = TRUE;
                    }
                }
                if (foundStep) {
                    result.y = bestStep;
                } else {
                    result.y = bestAny;
                }
            } else if (count > 0) {
                result.y = hits[0].height;
            } else {
                result.y = 0.0f;
            }
        }

        *position = result;
    }
    fn_80101B90(0xFF0000);
    fn_8018FC74(entry, position);
    return 1;
}
