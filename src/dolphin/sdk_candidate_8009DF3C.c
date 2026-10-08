typedef int BOOL;

extern void __RAS_OSDisableInterrupts_begin(void);
extern void __RAS_OSDisableInterrupts_end(void);

asm BOOL OSDisableInterrupts(void) {
    nofralloc
entry __RAS_OSDisableInterrupts_begin
    mfmsr r3
    rlwinm r4, r3, 0, 17, 15
    mtmsr r4
entry __RAS_OSDisableInterrupts_end
    rlwinm r3, r3, 17, 31, 31
    blr
}

asm BOOL OSEnableInterrupts(void) {
    nofralloc
    mfmsr r3
    ori r4, r3, 0x8000
    mtmsr r4
    rlwinm r3, r3, 17, 31, 31
    blr
}

asm BOOL OSRestoreInterrupts(register BOOL level) {
    nofralloc
    cmpwi level, 0
    mfmsr r4
    beq disable
    ori r5, r4, 0x8000
    b restore
disable:
    rlwinm r5, r4, 0, 17, 15
restore:
    mtmsr r5
    rlwinm r3, r4, 17, 31, 31
    blr
}
