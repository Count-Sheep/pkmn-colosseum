#include "dolphin/types.h"

void GXInitFifoLimits(u32* fifo, u32 base, u32 end) {
    fifo[3] = base;
    fifo[4] = end;
}
