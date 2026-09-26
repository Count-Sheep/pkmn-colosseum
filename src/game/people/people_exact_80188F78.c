/*
 * people TU: the follow-a-person movement setters and the talkable toggle
 * (0x80188F78-0x80189490), .text only. Lookups are the TU's two lookups
 * expanded inline (game/people/people_inline.h).
 */
#include "game/people/people_inline.h"

extern void* GSmodelGetPart(void* model, s32 index);
extern void GSpartRegisterRotation(void* part, void* rotation, s32 order);
extern void GSpartFree(void* part);

extern void* fn_8018FCBC(PeopleEntry* entry);

void fn_80188FA0(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex);

/* Make a person follow person (0, 100). */
void fn_80188F78(u32 groupId, u32 index)
{
    fn_80188FA0(groupId, index, 0, 100);
}

/*
 * Point a person's head part at `position` (kept in threadHandle). Retail
 * expands this same sequence in fn_80188CA0 (position = the person's own
 * target) and fn_80188FA0 (position = the followed person's position).
 */
static inline void peopleSetLookTarget(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    void* model;
    void* part;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        model = peopleGetModel(entry);
        if (model != NULL) {
            info = peopleInfoBiosGetPtr(entry->scriptRef);
            if (info != NULL) {
                partIndex = fn_8018F698(info);
                if (partIndex >= 0) {
                    entry->threadHandle = position;
                    part = GSmodelGetPart(model, partIndex);
                    GSpartRegisterRotation(part, entry->headRotation, 3);
                    GSpartFree(part);
                }
            }
        }
    }
}

/* Make a person walk after another person, looking at it. */
void fn_80188FA0(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex)
{
    PeopleEntry* entry;
    PeopleEntry* target;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    target = peopleFindBySelf(peopleFindSelf(targetGroupId, targetIndex));
    if (target == NULL) {
        return;
    }
    peopleSetLookTarget(groupId, index, fn_8018FCBC(target));
    entry->moveType = PEOPLE_MOVE_WALK_PATH;
    entry->walkPathId = targetGroupId;
    entry->walkPathParam = targetIndex;
}

/* Set or clear a person's talkable flag; returns its previous state. */
u8 fn_80189328(u32 groupId, u32 index, u8 enable)
{
    PeopleEntry* entry;
    u8 wasTalkable;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    wasTalkable = peopleTestFlags(entry, PEOPLE_FLAG_TALKABLE);
    if (enable) {
        peopleSetFlags(entry, PEOPLE_FLAG_TALKABLE);
    } else {
        peopleClearFlags(entry, PEOPLE_FLAG_TALKABLE);
    }
    return wasTalkable;
}
