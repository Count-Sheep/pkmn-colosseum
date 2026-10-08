#include "dolphin/types.h"

typedef f32 Mtx[3][4];

asm void PSMTXCopy(const register Mtx src, register Mtx dst) {
    nofralloc
    psq_l f0, 0(src), 0, 0
    psq_st f0, 0(dst), 0, 0
    psq_l f1, 8(src), 0, 0
    psq_st f1, 8(dst), 0, 0
    psq_l f2, 16(src), 0, 0
    psq_st f2, 16(dst), 0, 0
    psq_l f3, 24(src), 0, 0
    psq_st f3, 24(dst), 0, 0
    psq_l f4, 32(src), 0, 0
    psq_st f4, 32(dst), 0, 0
    psq_l f5, 40(src), 0, 0
    psq_st f5, 40(dst), 0, 0
    blr
}
