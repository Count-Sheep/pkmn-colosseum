/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

extern GSThreadCtx* lbl_8047AC24;
extern GSThreadCtx* lbl_8047AC20;
extern u32 lbl_8047AC10;
extern u32 lbl_8047AC18;
extern u32 lbl_8047AC14;
extern void threadSaveGPRRegisters(void);
extern void threadLoadGPRRegisters(void);
extern void threadSaveFPRRegisters(void);

u32 _threadSwitch(u32 func, u32 retaddr, u32 arg) {
    GSThreadCtx* ctx;

    lbl_8047AC18 = func;
    lbl_8047AC14 = arg;
    lbl_8047AC1C = lbl_8047AC20;
    threadSaveGPRRegisters();
    ctx = lbl_8047AC1C;
    ctx->lr = retaddr;
    ctx->gpr[3] = lbl_8047AC18;
    ctx->gpr[5] = lbl_8047AC14;
    ctx->gpr[1] = (u32)&ctx;
    if (lbl_8047AC10 != 0) {
        threadSaveFPRRegisters();
    }
    lbl_8047AC1C = lbl_8047AC24;
    threadLoadGPRRegisters();
    return lbl_8047AC1C->gpr[3];
}
