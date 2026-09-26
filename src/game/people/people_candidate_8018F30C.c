/*
 * people TU: fn_8018F30C, the per-frame shadow-light update.
 *
 * Exact as plain C (the people.c copy sat under a leaked
 * optimization_level pragma). Still a CodeCandidate: retail reloads the
 * 0.0f literal inside the light loop instead of reusing the register that
 * holds it for position.y, which MWCC does only for a literal. Read as an
 * extern constant, the value stays in f30. So the match needs the TU's
 * pooled .sdata2, which the rest of the TU reads by symbol.
 */
#include "game/people/people.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

extern void set__5GSvecFfff(GSvec* vec, f32 x, f32 y, f32 z);
extern void GSvecCopy(void* dst, void* src);
extern u8 GSmodelGetVisibility(void* model);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetPosition(void* light, void* position);
extern void* fn_8018FCBC(PeopleEntry* entry);

/* The people system's two shadow lights (see fn_8018E920 and fn_8018F470). */
extern void* lbl_8047B1F0[2];

/*
 * The player characters are people 100 and 101 of group 0. Both shadow-light
 * users (fn_8018E1C4 and fn_8018F30C) expand this test identically before
 * turning it into a light number with `!= TRUE`.
 */
static inline u8 peopleIsHero(u32 groupId, u32 index)
{
    if (groupId == 0 && (index == 100 || index == 101)) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Aim the two shadow lights at the first visible person of each kind (light
 * 0: the player characters, light 1: everyone else) from 2500 units above.
 */
void fn_8018F30C(void)
{
    s32 lightIndex;
    s32 i;
    PeopleEntry* entry;
    void* model;
    GSvec position;
    u8 hero;

    if (peopleGetMaxCount() == 0) {
        return;
    }
    for (lightIndex = 0; lightIndex < 2; lightIndex++) {
        set__5GSvecFfff(&position, 0.0f, 0.0f, 0.0f);
        for (i = 0; i < peopleGetMaxCount(); i++) {
            entry = peopleGetEntry(i);
            if (!entry->active) {
                continue;
            }
            model = peopleGetModel(entry);
            if (model == NULL || !GSmodelGetVisibility(model)) {
                continue;
            }
            hero = peopleIsHero(entry->groupId, entry->index);
            if ((hero != TRUE) != lightIndex) {
                continue;
            }
            GSvecCopy(&position, fn_8018FCBC(entry));
            break;
        }
        position.y = 0.0f;
        GSlightSetTarget(lbl_8047B1F0[lightIndex], &position);
        position.y = 2500.0f;
        GSlightSetPosition(lbl_8047B1F0[lightIndex], &position);
    }
}
