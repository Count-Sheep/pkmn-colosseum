/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"

extern u32 lbl_8047AC38;
extern u32 lbl_8047AC3C;
extern u32 lbl_8047AC40;

typedef u32 (*GSnativeFunc)(u32, u32, u32, u32, u32, u32, u32, u32, ...);

u32 fn_800F106C(void) {
    GSnativeFunc func = (GSnativeFunc)lbl_8047AC38;
    f32* fa = (f32*)lbl_8047AC3C;
    u32* ia = (u32*)lbl_8047AC40;

    return func(ia[0], ia[1], ia[2], ia[3], ia[4], ia[5], ia[6], ia[7],
                fa[0], fa[1], fa[2], fa[3], fa[4], fa[5], fa[6], fa[7]);
}
