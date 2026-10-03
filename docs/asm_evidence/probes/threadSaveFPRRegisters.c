/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

void threadSaveFPRRegisters(void) {
    GSThreadCtx* ctx = lbl_8047AC1C;
    f64* fpr = ctx->fpr;
    s32 i;
    for (i = 0; i < 32; i++) {
        fpr[i] = 0.0;
    }
}
