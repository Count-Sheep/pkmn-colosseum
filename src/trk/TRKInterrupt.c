#include "dolphin/types.h"

typedef struct TRKExceptionStatus {
    u32 words[3];
    u8 pad_0C;
    u8 exceptionDetected;
    u8 pad_0E[2];
} TRKExceptionStatus;

extern TRKExceptionStatus gTRKExceptionStatus_80313824;
extern void TRK__read_aram(void* data, u32 address, u32* length);
extern void TRK__write_aram(void* data, u32 address, u32* length);

/* fn_800C11F4 - 0x800C11F4 | size: 0x24
 * MetroTRK targimpl.c ReadFPSCR (upstream name; the symbol keeps its address).
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_fpscr.md */
asm void fn_800C11F4(register f64* data) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd f31, 0x10(r1)
    psq_st f31, 0x20(r1), 0, 0
    mffs f31
    stfd f31, 0(r3)
    psq_l f31, 0x20(r1), 0, 0
    lfd f31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

/* fn_800C1218 - 0x800C1218 | size: 0x24
 * MetroTRK targimpl.c WriteFPSCR (upstream name; the symbol keeps its address).
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_fpscr.md */
asm void fn_800C1218(register f64* data) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd f31, 0x10(r1)
    psq_st f31, 0x20(r1), 0, 0
    lfd f31, 0(r3)
    mtfsf 0xff, f31
    psq_l f31, 0x20(r1), 0, 0
    lfd f31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

s32 TRKTargetAccessARAM(void* data, u32 address, u32* length, BOOL read)
{
    s32 error = 0;
    TRKExceptionStatus saved = gTRKExceptionStatus_80313824;

    gTRKExceptionStatus_80313824.exceptionDetected = FALSE;
    if (read) {
        TRK__read_aram(data, address, length);
    } else {
        TRK__write_aram(data, address, length);
    }
    if (gTRKExceptionStatus_80313824.exceptionDetected) {
        *length = 0;
        error = 0x702;
    }
    gTRKExceptionStatus_80313824 = saved;
    return error;
}
