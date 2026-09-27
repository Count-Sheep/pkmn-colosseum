/**
 * @file gs_vm_candidate_800F716C.c
 * @brief GS script VM: stop the scripts of a thread group.
 *
 * Address range: 0x800F716C - 0x800F7274 (fn_800F716C).
 *
 * A .text-only piece of the GS VM translation unit (game/gs_vm.c holds the
 * same definition). Every instruction matches except the order of the group compare
 * (retail "cmplw r3,r30", this "cmplw r30,r3"); see gs_vm.c. CodeCandidate.
 */

#include "dolphin/types.h"
#include "game/gs_thread.h"
#include "game/gs_vm.h"

extern u32 fn_800F0374(GSThread* thread);

/* 0x800F716C | 0x108: stop every script running on a thread of `group`
 * (the thread's +0x0C field, read by fn_800F0374; floor teardown passes
 * the floor id). */
s32 fn_800F716C(u32 group)
{
    GSVMCtx* ctx;
    s32 i;

    for (i = 0; i < lbl_80478B00->count; i++) {
        ctx = &lbl_80478B00->contexts[i];
        if (ctx->status != 0 && ctx->thread != NULL && fn_800F0374(ctx->thread) == group) {
            GSvmStopByKey(ctx->key);
        }
    }
    return 0;
}
