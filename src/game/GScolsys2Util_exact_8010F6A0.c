/**
 * @file GScolsys2Util_exact_8010F6A0.c
 * @brief GScolsy2UtilGetPointExtentionLine, 0x8010F6A0 - 0x8010F71C.
 *
 * A function-boundary carve of the GScolsys2Util TU
 * (GScolsys2UtilGetCpPlaneLine .. GScolsy2UtilGetCpPlanePoint,
 * 0x8010F4B8 - 0x8010FAF4; it owns the .sdata2 pool 0x8047CF10-0x8047CF20).
 * This function uses no data. Project default flags (GC/1.3), no pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

/* 0x8010F6A0 | 0x7C */
void GScolsy2UtilGetPointExtentionLine(void* arg0, void* arg1, void* arg2, f32 t) {
    f32 v[3];
    extern void PSVECSubtract(void*, void*, void*);
    extern f32 PSVECMag(void*);
    extern void PSVECScale(void*, void*, f32);
    extern void PSVECAdd(void*, void*, void*);

    PSVECSubtract(arg2, arg1, v);
    PSVECScale(v, v, t / PSVECMag(v));
    PSVECAdd(v, arg1, arg0);
}
