/**
 * MetroTRK Processor/ppc/Export/targsupp: the file-I/O trap stubs,
 * .text 0x800C29F0 - 0x800C2A10. Each is `twui r0, 0` (written `twi 31`,
 * which assembles to the same word) followed by `blr`; the host debugger
 * services the trap. Built with -func_align 8, matching the retail
 * placement (8-aligned after the TRKTarget_residual_800C25FC object).
 */
#include "dolphin/types.h"

/* TRKAccessFile - 0x800C29F0 | size: 0x8
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_targsupp.md */
asm u32 TRKAccessFile(void) {
    nofralloc
    twi 31, r0, 0
    blr
}

/* TRKOpenFile - 0x800C29F8 | size: 0x8
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_targsupp.md */
asm u32 TRKOpenFile(void) {
    nofralloc
    twi 31, r0, 0
    blr
}

/* TRKCloseFile - 0x800C2A00 | size: 0x8
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_targsupp.md */
asm u32 TRKCloseFile(void) {
    nofralloc
    twi 31, r0, 0
    blr
}

/* TRKPositionFile - 0x800C2A08 | size: 0x8
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_targsupp.md */
asm u32 TRKPositionFile(void) {
    nofralloc
    twi 31, r0, 0
    blr
}
