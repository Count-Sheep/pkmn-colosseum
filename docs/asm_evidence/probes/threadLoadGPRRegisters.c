/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

void threadLoadGPRRegisters(void) {
    register GSThreadCtx* ctx = lbl_8047AC1C;
    register u32 r4 = ctx->gpr[4];
    register u32 r5 = ctx->gpr[5];
    register u32 r6 = ctx->gpr[6];
    register u32 r7 = ctx->gpr[7];
    register u32 r8 = ctx->gpr[8];
    register u32 r9 = ctx->gpr[9];
    register u32 r10 = ctx->gpr[10];
    (void)ctx->gpr[0];
    (void)ctx->gpr[2];
    (void)r4; (void)r5; (void)r6; (void)r7; (void)r8; (void)r9; (void)r10;
}
