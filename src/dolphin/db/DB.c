/**
 * @file DB.c
 * @brief Dolphin SDK db.c, 0x800A2C74 - 0x800A2CCC: __DBExceptionDestinationAux
 *        and the asm __DBExceptionDestination. DBInit, the exception-mark
 *        helpers and DBPrintf link from their own units.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSContext.h"

extern void OSReport(const char* format, ...);
extern void OSDumpContext(OSContext* context);
extern void PPCHalt(void);
extern char lbl_803118D8[]; /* "DBExceptionDestination\n" */

void __DBExceptionDestinationAux(void) {
    u32* contextAddr;
    OSContext* context;

    contextAddr = (u32*)0xC0;
    context = (OSContext*)(*contextAddr + 0x80000000);
    OSReport(lbl_803118D8);
    OSDumpContext(context);
    PPCHalt();
}

/* Hand-written Dolphin SDK asm; evidence: docs/asm_evidence/db.md.
 * It turns address translation back on and tail-branches to
 * __DBExceptionDestinationAux (a declared branch target). */
asm void __DBExceptionDestination(void) {
    nofralloc

    mfmsr   r3
    ori     r3, r3, 0x30
    mtmsr   r3
    b       __DBExceptionDestinationAux
}
