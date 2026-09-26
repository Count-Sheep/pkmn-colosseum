/*
 * people TU: the walk-to-position state (fn_801858C4) and its step
 * (fn_80185AAC), .text only.
 */
#include "game/people/people_inline.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

extern void fn_800E0168(void* dst, void* srcA, void* srcB);
extern u8 fn_80188214(u32 groupId, u32 index, f32 distance);
extern void fn_8018E9B4(PeopleEntry* entry, void* position, void* transform);
extern void fn_8018FC74(PeopleEntry* entry, void* position);
extern void* fn_8018FCBC(PeopleEntry* entry);

/* The people TU's pooled literal, read by symbol (people_sdata2_8047D790.c). */
extern const f64 lbl_8047D828; /* 0.0001f as a double */

s32 fn_80185AAC(PeopleEntry* entry);

/*
 * Place a person at `position` and record it as its transform. This is
 * fn_8018C0A8's body, which fn_801858C4 expands inline.
 */
static inline void peoplePlaceAt(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC74(entry, position);
        peopleSetTransform(entry, position);
    }
}

/*
 * Walk toward field_5C: stop once a step reports arrival, and once the model
 * has not moved for 60 frames, snap it to the target and stop.
 */
void fn_801858C4(PeopleEntry* entry)
{
    void* position;
    void* transform;
    GSvec delta;

    if (fn_80185AAC(entry) != 0) {
        entry->state = 0;
    }
    if (entry == NULL) {
        return;
    }
    position = fn_8018FCBC(entry);
    transform = peopleGetTransform(entry);
    fn_800E0168(&delta, position, transform);
    if (__fabs(delta.x) < lbl_8047D828 && __fabs(delta.y) < lbl_8047D828 &&
        __fabs(delta.z) < lbl_8047D828) {
        entry->pad97++;
        if (entry->pad97 > 60) {
            peoplePlaceAt(entry->groupId, entry->index, entry->field_5C);
            entry->state = 0;
            entry->pad97 = 0;
        }
    } else {
        entry->pad97 = 0;
    }
}

/*
 * Step a person toward field_5C. Returns 2 if the step failed, 1 if it
 * overshot (the person is then placed on the target), 0 otherwise.
 */
s32 fn_80185AAC(PeopleEntry* entry)
{
    GSvec delta;
    f32 oldLength;

    fn_800E0168(&delta, entry->field_5C, fn_8018FCBC(entry));
    oldLength = fn_800E008C(&delta);
    if (!fn_80188214(entry->groupId, entry->index, entry->moveSpeed)) {
        return 2;
    }
    fn_800E0168(&delta, entry->field_5C, fn_8018FCBC(entry));
    if (fn_800E008C(&delta) > oldLength) {
        fn_8018FC74(entry, entry->field_5C);
        fn_8018E9B4(entry, fn_8018FCBC(entry), peopleGetTransform(entry));
        return 1;
    }
    return 0;
}
