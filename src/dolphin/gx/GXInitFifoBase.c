#include "dolphin/gx/GX.h"

void GXInitFifoLimits(GXFifoObj* fifo, u32 hiWatermark, u32 loWatermark);
void GXInitFifoPtrs(GXFifoObj* fifo, void* readPtr, void* writePtr);

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size) {
    fifo->base = base;
    fifo->top = (u8*)base + size - 4;
    fifo->size = size;
    fifo->count = 0;
    GXInitFifoLimits(fifo, size - 0x4000, (size >> 1) & ~0x1f);
    GXInitFifoPtrs(fifo, base, base);
}
