/**
 * @file ps_exact_80172908.c
 * @brief HSD_MTXSRT (0x80172908 - 0x80172928).
 *
 * A data-free function of HAL's psinterpret.c (0x8016F430 - 0x80173624),
 * carved like its already linked neighbour _psListGetNext: the
 * interpreter's application-SRT command builds its matrix through this
 * forwarder to HSD_MtxSRT. Built with the particle library flags
 * (GC/1.3.2 -O4,p -inline auto,deferred -use_lmw_stmw on -sdata 8
 * -sdata2 8 -str reuse,readonly).
 */
#include "dolphin/mtx.h"
#include "dolphin/types.h"

extern void HSD_MtxSRT(Mtx m, Vec* scale, Vec* rot, Vec* trans, Vec* scale2);

void HSD_MTXSRT(Mtx m, Vec* scale, Vec* rot, Vec* trans, Vec* scale2)
{
    HSD_MtxSRT(m, scale, rot, trans, scale2);
}
