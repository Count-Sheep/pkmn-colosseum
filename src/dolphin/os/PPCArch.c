#include "dolphin/os/PPCArch.h"

extern u32 PPCMfhid0(void);
extern void PPCMthid0(u32 hid0);

#define HID2 920
#define WPAR 921

union FpscrUnion {
    f64 f;
    struct {
        u32 fpscr_pad;
        u32 fpscr;
    } u;
};

u32 PPCMffpscr(void) {
    union FpscrUnion m;

    asm {
        mffs fp31
        stfd fp31, m.f;
    }

    return m.u.fpscr;
}

void PPCMtfpscr(register u32 newFPSCR) {
    union FpscrUnion m;

    asm {
        li    r4, 0
        stw   r4, m.u.fpscr_pad;
        stw   newFPSCR, m.u.fpscr
        lfd   fp31, m.f
        mtfsf 0xff, fp31
    }
}

asm u32 PPCMfhid2(void) {
    nofralloc
    mfspr r3, HID2
    blr
}

asm void PPCMthid2(register u32 newhid2) {
    nofralloc
    mtspr HID2, newhid2
    blr
}

asm void PPCMtwpar(register u32 newwpar) {
    nofralloc
    mtspr WPAR, newwpar
    blr
}

void PPCDisableSpeculation(void) {
    u32 hid0 = PPCMfhid0();
    hid0 |= 0x200;
    PPCMthid0(hid0);
}
