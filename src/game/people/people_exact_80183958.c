/**
 * @file people_exact_80183958.c
 * @brief fn_80183958 / fn_8018397C, 0x80183958 - 0x801839A0.
 *
 * Function-boundary carve of the people TU (see people.c): two script-ID
 * lookups with no jump table, no pooled constant and no data. Same flags as
 * the TU (PEOPLE_TU_CFLAGS: GC/1.3 -O4,p, -inline noauto,deferred, read-only
 * strings), no pragmas; deferred mode emits in reverse definition order, so
 * fn_8018397C is defined first.
 */
#include "dolphin/types.h"

extern u8* fn_801170A4(u32 groupId, u32 index);
extern u32 floorCharacterBiosGetMoveSctID(u8* character);
extern u32 floorCharacterBiosGetTalkSctID(u8* character);

/* The talk script of a person's floor-character record. */
u32 fn_8018397C(u32 groupId, u32 index)
{
    return floorCharacterBiosGetTalkSctID(fn_801170A4(groupId, index));
}

/* The move script of a person's floor-character record. */
u32 fn_80183958(u32 groupId, u32 index)
{
    return floorCharacterBiosGetMoveSctID(fn_801170A4(groupId, index));
}
