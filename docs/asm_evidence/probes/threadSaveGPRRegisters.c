/* Closest C candidate for the compiler probe (see docs/asm_evidence/gs_thread.md). */
#include "dolphin/types.h"
#include "game/gs_thread.h"

extern GSThreadCtx* lbl_8047AC1C;

void threadSaveGPRRegisters(void) {
    GSThreadCtx* ctx = lbl_8047AC1C;
    s32 i;
    for (i = 0; i < 32; i++) {
        if (i != 1) {
            ctx->gpr[i] = 0;
        }
    }
}
