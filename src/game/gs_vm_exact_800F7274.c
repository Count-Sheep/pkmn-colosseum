/**
 * @file gs_vm_exact_800F7274.c
 * @brief GS script VM: stop the script with a key.
 *
 * Address range: 0x800F7274 - 0x800F7318 (fn_800F7274).
 *
 * A .text-only piece of the GS VM translation unit (game/gs_vm.c holds the
 * same definition). It does not read the string pool, so the unit is exact on its
 * own with the manager pointer extern.
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"
#include "game/gs_vm.h"

/* 0x800F7274 | 0xA4: stop the script with `key`. */
s32 fn_800F7274(u16 key)
{
    return GSvmStopByKey(key);
}
