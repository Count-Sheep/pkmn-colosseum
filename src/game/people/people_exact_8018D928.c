/*
 * people TU: the two person lookups and the people close/hide helpers
 * (0x8018D928-0x8018DCA8), .text only.
 *
 * fn_8018DB68 carries both lookups expanded inline, as every earlier caller
 * in the TU does; it reads them from game/people/people_inline.h
 * (peopleFindSelf/peopleFindBySelf have these two functions' bodies) so the
 * object emits only the retail symbols.
 */
#include "game/people/people_inline.h"

extern void* fn_800F7108(u16 id);
extern void GSthreadBlock(void* thread);
extern void fn_8018DCA8(PeopleEntry* entry, u8 releaseWalkList);

/* Find the active person whose self pointer is `self`. */
PeopleEntry* peopleSearchID(PeopleEntry* self)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->selfPtr != self) continue;
        return entry;
    }
    return NULL;
}

/*
 * Resolve (groupId, index) to a person's self pointer; if no person of that
 * group has the index, fall back to any group and warn.
 */
PeopleEntry* fn_8018D998(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite(lbl_80273FD8, groupId, index);
        return entry->selfPtr;
    }
    return NULL;
}

/* Hide every active person and block its script thread. */
void fn_8018DA88(void)
{
    s32 i;
    PeopleEntry* entry;
    void* thread;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry == NULL) continue;
        entry->visible = 0;
        thread = fn_800F7108(entry->flagId);
        if (thread != NULL) {
            GSthreadBlock(thread);
        }
    }
}

/* Close every active person. */
void fn_8018DB04(u8 releaseWalkList)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        fn_8018DCA8(entry, releaseWalkList);
    }
}

/* Close the person (groupId, index). */
void fn_8018DB68(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018DCA8(entry, 1);
    }
}
