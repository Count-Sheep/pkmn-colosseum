/**
 * @file gs_msg_exact_800FDFE4.c
 * @brief _msgGetLength__FPCUs, 0x800FDFE4 - 0x800FE010.
 *
 * Function-boundary carve of the GSmsg TU (see gs_msg.c): a message's
 * length in characters from its byte size. No jump table, no pooled
 * constant, no data; _msgGetSize__FPCUs is called by its global symbol.
 * GC/1.3 -O4,p with the TU's unit-wide -opt nopeephole, no pragmas.
 */
#include "dolphin/types.h"

extern s32 _msgGetSize__FPCUs(const void* str);

s32 _msgGetLength__FPCUs(const void* str) {
    s32 r;
    r = _msgGetSize__FPCUs(str);
    return (s32)(((u32)r + 1) >> 1) - 1;
}
