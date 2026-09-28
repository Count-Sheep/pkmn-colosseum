/**
 * @file floor_character_exact_80116F68.c
 * @brief Floor-character BIOS transform getters, 0x80116F68 - 0x80117038.
 *
 * floorCharacterBiosGetRot and floorCharacterBiosGetPos of the
 * floorCharacterBios unit, carved at their function boundaries. The unit's
 * .sdata2 pool is 0x8047CFC0 - 0x8047CFD0 (0.0f, the degree-to-radian
 * factor and the s16-to-float conversion constant). Only
 * floorCharacterBiosGetRot reads it (floor_event's pool ends at 0x8047CFC0,
 * field_camera's starts at 0x8047CFD0), so this carve owns that range and
 * the literals are written in place; MWCC pools them in first-use order.
 *
 * Built with the unit's flags, which include -opt nopeephole (see
 * configure.py): floorCharacterBiosGetPos copies its record into r5 and
 * then compares it (mr r5,r3; cmplwi r5,0) where the peephole pass would
 * fold the two into mr.
 */
#include "dolphin/types.h"

extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);

s32 floorCharacterBiosGetRot(u8* ptr, void* rot)
{
    if (ptr == NULL) {
        return 0;
    }
    if (rot == NULL) {
        return 0;
    }
    set__5GSvecFfff(rot, 0.0f, 0.017453292f * *(s16*) (ptr + 0x4), 0.0f);
    return 1;
}

s32 floorCharacterBiosGetPos(u8* ptr, void* pos)
{
    if (ptr == NULL) {
        return 0;
    }
    if (pos == NULL) {
        return 0;
    }
    set__5GSvecFfff(pos, *(f32*) (ptr + 0x18), *(f32*) (ptr + 0x1C),
                    *(f32*) (ptr + 0x20));
    return 1;
}
