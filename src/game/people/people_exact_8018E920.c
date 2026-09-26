#include "game/people/people.h"

/*
 * Floor save/restore handler triple (load, save, size), the layout
 * fn_800FF4D4 copies into its GSFloorResHandler table.
 */
typedef struct PeopleFloorResFuncs {
    void* func[3];
} PeopleFloorResFuncs;

extern void* GSlightCreate(void);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetActive(void* light, u8 active);
extern void fn_800FF4D4(void* data, u8 typeId);

/*
 * { peopleBiosPopData, peopleBiosPushData, peopleBiosGetPushDataSize }: the
 * .rodata image of this function's local initializer, kept extern because
 * this unit owns only .text. Declared without const: retail loads the three
 * words after the prologue's register saves, as for the compiler's own
 * initializer copy; a const extern lets the scheduler hoist them.
 */
extern PeopleFloorResFuncs lbl_80273F90;
/* The people system's two shadow lights (see fn_8018F470). */
extern void* lbl_8047B1F0[2];

void fn_8018E920(u32 maxPeople)
{
    PeopleFloorResFuncs funcs = lbl_80273F90;
    s32 i;
    void** light;

    peopleInit(maxPeople);
    for (i = 0, light = lbl_8047B1F0; i < 2; i++, light++) {
        *light = GSlightCreate();
        GSlightSetType(*light, 2);
        GSlightSetActive(*light, 0);
    }
    fn_800FF4D4(&funcs, 1);
}
