#include "dolphin/types.h"

asm void Config24MB_8009F54C(void) {
    nofralloc
    li r7, 0x0
    lis r4, 0x0
    addi r4, r4, 0x2
    lis r3, 0x8000
    addi r3, r3, 0x1ff
    lis r6, 0x100
    addi r6, r6, 0x2
    lis r5, 0x8100
    addi r5, r5, 0xff
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

asm void Config48MB_8009F5CC(void) {
    nofralloc
    li r7, 0x0
    lis r4, 0x0
    addi r4, r4, 0x2
    lis r3, 0x8000
    addi r3, r3, 0x3ff
    lis r6, 0x200
    addi r6, r6, 0x2
    lis r5, 0x8200
    addi r5, r5, 0x1ff
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

asm void RealMode_8009F64C(register u32 address) {
    nofralloc
    clrlwi r3, r3, 2
    mtsrr0 r3
    mfmsr r3
    rlwinm r3, r3, 0, 28, 25
    mtsrr1 r3
    rfi
}
