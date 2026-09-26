/**
 * @file gs_range_8017FA5C_exact_801808B4.c
 * @brief ARQ entry busy poll and completion callback (0x801808B4 - 0x8018094C).
 *
 * Carved out of the 0x8017FA5C - 0x80180C78 retail unit so the exact
 * functions can link on their own; built like the rest of it at
 * `-opt level=0` with `-inline deferred` (which emits a unit's functions in
 * reverse source order, hence fn_801808E4 first), with no local pragmas.
 */
#include "game/gs_range_8017FA5C_shared.h"

void fn_801808E4(GsRangeARQEntry* entry)
{
    GsRangeARQEntry* e = entry;

    e->mode = 0;
    if (e->callback) {
        e->callback((void*)e->flush, e->callbackArg);
    }
    e->state = 0;
    DCFlushRange(e->src, e->size);
}

s32 fn_801808B4(void* handle)
{
    GsRangeARQEntry* entry = handle;

    if (entry->mode != 1) {
        entry->state = 0;
    }
    return entry->state;
}
