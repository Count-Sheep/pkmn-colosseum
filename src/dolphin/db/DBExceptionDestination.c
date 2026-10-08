#include "dolphin/db/DB.h"

extern void __DBExceptionDestinationAux(void);

asm void __DBExceptionDestination(void) {
    nofralloc
    mfmsr r3
    ori r3, r3, 0x30
    mtmsr r3
    b __DBExceptionDestinationAux
}
