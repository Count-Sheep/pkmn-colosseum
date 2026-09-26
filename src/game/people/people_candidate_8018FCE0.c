/**
 * @file people_candidate_8018FCE0.c
 * @brief peopleAlloc (fn_8018FCE0), 0x8018FCE0 - 0x8018FD88.
 *
 * Standalone source for the split range; the body is the one previously
 * reached through the people.c include wrapper.
 */
#include "game/people/people.h"

extern void* memset(void* dst, int val, u32 size);
extern f32 lbl_8047D8B0;          /* default moveSpeed constant */
extern s32 lbl_8047B1F8;          /* maximum people count */
extern PeopleEntry* lbl_8047B200; /* people array (heap-allocated) */

#define gPeopleMaxCount lbl_8047B1F8
#define gPeopleArray    lbl_8047B200

/*
 * Find the first free (inactive) slot in the people array, zero it, and mark
 * it active with its self-pointer, no shadow and the default move speed.
 * Returns NULL when every slot is in use.
 */
PeopleEntry* fn_8018FCE0(void)
{
    PeopleEntry* entry;
    s32 maxCount;
    int i;
    PeopleEntry* found;
    f32 moveSpeed;

    maxCount = gPeopleMaxCount;
    entry = gPeopleArray;

    for (i = 0; maxCount > 0; maxCount--) {
        if (i < 0 || gPeopleMaxCount <= i) {
            found = NULL;
        } else {
            found = entry;
        }

        if (found->active == 0) {
            memset(found, 0, PEOPLE_ENTRY_SIZE);

            found->active = 1;
            moveSpeed = lbl_8047D8B0;
            found->selfPtr = found;
            found->shadowId = -1;
            found->moveSpeed = moveSpeed;

            return found;
        }

        entry = (PeopleEntry*)((u8*)entry + PEOPLE_ENTRY_SIZE);
        i++;
    }

    return NULL;
}
