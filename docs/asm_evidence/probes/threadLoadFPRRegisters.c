/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

void threadLoadFPRRegisters(void) {
    GSThreadCtx* ctx = lbl_8047AC1C;
    f64* fpr = ctx->fpr;
    volatile f64 f0 = fpr[0];
    volatile f64 f1 = fpr[1];
    volatile f64 f2 = fpr[2];
    volatile f64 f3 = fpr[3];
}
