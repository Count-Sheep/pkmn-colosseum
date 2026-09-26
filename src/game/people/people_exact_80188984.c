/*
 * people TU: fn_80188984 (wait for a head turn) and fn_80188AF4 (stop
 * looking), .text only.
 */
#include "game/people/people_inline.h"

extern void _threadSwitch(void);
extern void* GSmodelGetPart(void* model, s32 index);
extern void fn_800EE288(void* part);
extern void GSpartFree(void* part);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);

/* The people TU's pooled 0.0f, read by symbol (people_sdata2_8047D790.c). */
extern const f32 lbl_8047D7A0;

/*
 * Report whether a person's head is still turning toward its target. With
 * `wait`, yield until the turn has finished and return FALSE.
 */
BOOL fn_80188984(u32 groupId, u32 index, u8 wait)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    for (;;) {
        if (entry->headRotation[0] == entry->headTarget[0] &&
            entry->headRotation[1] == entry->headTarget[1]) {
            return FALSE;
        }
        if (wait) {
            _threadSwitch();
            continue;
        }
        return TRUE;
    }
}

/* Stop a person looking at anything: reset its head part and look target. */
void fn_80188AF4(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    void* model;
    void* part;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    partIndex = fn_8018F698(info);
    if (partIndex < 0) {
        return;
    }
    part = GSmodelGetPart(model, partIndex);
    fn_800EE288(part);
    GSpartFree(part);
    entry->threadHandle = NULL;
    set__5GSvecFfff(entry->headTarget, lbl_8047D7A0, lbl_8047D7A0, lbl_8047D7A0);
    entry->moveType = PEOPLE_MOVE_NONE;
}
