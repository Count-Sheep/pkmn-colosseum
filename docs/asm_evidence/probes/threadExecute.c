/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

extern GSThreadCtx* lbl_8047AC24;
extern GSThreadCtx* lbl_8047AC20;
extern u32 lbl_8047AC10;
extern void threadSaveGPRRegisters(void);
extern void threadLoadGPRRegisters(void);
extern void threadLoadFPRRegisters(void);

void threadExecute(void) {
    GSThreadCtx* ctx;
    void (*resume)(u32, u32, u32);

    lbl_8047AC1C = lbl_8047AC24;
    threadSaveGPRRegisters();
    ctx = lbl_8047AC1C;
    ctx->gpr[1] = (u32)&ctx;
    lbl_8047AC1C = lbl_8047AC20;
    threadLoadGPRRegisters();
    if (lbl_8047AC10 != 0) {
        threadLoadFPRRegisters();
    }
    ctx = lbl_8047AC1C;
    resume = (void (*)(u32, u32, u32))ctx->lr;
    resume(ctx->gpr[3], 0, ctx->gpr[5]);
}
