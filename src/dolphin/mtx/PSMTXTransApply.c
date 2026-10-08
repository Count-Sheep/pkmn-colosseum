#include "dolphin/types.h"

typedef f32 Mtx[3][4];

asm void PSMTXTransApply(const register Mtx src, register Mtx dst,
                         register f32 xT, register f32 yT, register f32 zT) {
    nofralloc
    psq_l f4, 0(src), 0, 0
    frsp xT, xT
    psq_l f5, 8(src), 0, 0
    frsp yT, yT
    psq_l f7, 24(src), 0, 0
    frsp zT, zT
    psq_l f8, 40(src), 0, 0
    psq_st f4, 0(dst), 0, 0
    ps_sum1 f5, xT, f5, f5
    psq_l f6, 16(src), 0, 0
    psq_st f5, 8(dst), 0, 0
    ps_sum1 f7, yT, f7, f7
    psq_l f9, 32(src), 0, 0
    psq_st f6, 16(dst), 0, 0
    ps_sum1 f8, zT, f8, f8
    psq_st f7, 24(dst), 0, 0
    psq_st f9, 32(dst), 0, 0
    psq_st f8, 40(dst), 0, 0
    blr
}
