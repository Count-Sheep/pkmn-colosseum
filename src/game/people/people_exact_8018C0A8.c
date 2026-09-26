/*
 * people TU: the (groupId, index) position/visibility/flag accessors
 * (0x8018C0A8-0x8018CB5C), .text only. Each looks the person up with the
 * TU's two lookups expanded inline (game/people/people_inline.h).
 */
#include "game/people/people_inline.h"

extern void fn_8018FB2C(PeopleEntry* entry, u8 animId);
extern void fn_8018FB60(PeopleEntry* entry, u8 animId);
extern void fn_8018FC74(PeopleEntry* entry, void* position);

/*
 * Show or hide a person's shadow; it stays hidden while the model is.
 * This is fn_8018CA20's body. fn_8018C1E8 expands it inline, which shows as
 * the copy of `visible` (mr r27,r31) the expansion needs because it may
 * clear its own parameter.
 */
static inline void peopleSetShadowVisible(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        if (entry->animId == 0) {
            visible = 0;
        }
        fn_8018FB2C(entry, visible);
    }
}

/* Move a person's model to `position` and record it as the transform. */
void fn_8018C0A8(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC74(entry, position);
        peopleSetTransform(entry, position);
    }
}

/* Show or hide a person; its shadow follows (fn_8018CA20, expanded inline). */
void fn_8018C1E8(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    fn_8018FB60(entry, visible);
    peopleSetShadowVisible(groupId, index, visible);
}

u8 fn_8018C424(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    return peopleTestFlags(entry, mask);
}

u32 fn_8018C558(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    return entry->flags;
}

void fn_8018C69C(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleClearFlags(entry, mask);
    }
}

void fn_8018C7C8(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, mask);
    }
}

void fn_8018C8F4(u32 groupId, u32 index, u32 flags)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleWriteFlags(entry, flags);
    }
}

/* Show or hide a person's shadow (peopleSetShadowVisible's body). */
void fn_8018CA20(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        if (entry->animId == 0) {
            visible = 0;
        }
        fn_8018FB2C(entry, visible);
    }
}
