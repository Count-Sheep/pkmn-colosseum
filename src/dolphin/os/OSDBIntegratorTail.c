#include "dolphin/types.h"

typedef void (*OSExceptionHandler)(u8 exception, void* context, u32 dsisr,
                                   u32 dar);

extern OSExceptionHandler* OSExceptionTable_8047A6C4;
extern void __OSDBINTSTART(void);
extern void __OSDBINTEND(void);
extern void __OSDBJUMPEND(void);

static asm void __OSDBIntegrator(void) {
    nofralloc
entry __OSDBINTSTART
    li r5, 0x40
    mflr r3
    stw r3, 0xC(r5)
    lwz r3, 0x8(r5)
    oris r3, r3, 0x8000
    mtlr r3
    li r3, 0x30
    mtmsr r3
    blr
entry __OSDBINTEND
}

asm void fn_8009A0C0(void) {
    nofralloc
    bla 0x60
entry __OSDBJUMPEND
}

#pragma peephole off
OSExceptionHandler __OSSetExceptionHandler(u8 exception,
                                            OSExceptionHandler handler) {
    OSExceptionHandler* entry = &OSExceptionTable_8047A6C4[exception];
    OSExceptionHandler old = *entry;

    *entry = handler;
    return old;
}
#pragma peephole reset
