/**
 * @file gs_vm_candidate_800F13D0.c
 * @brief GS script VM native "wait" call.
 *
 * Address range: 0x800F13D0 - 0x800F16C0 (fn_800F13D0).
 *
 * Every instruction matches except the register chosen for argc: retail
 * keeps it in r29 (shared with the hoisted constant 3), this source gets
 * r28 (shared with wait). An inline helper returning argc gives r29, but
 * the same helper moves argc off its retail register in fn_800F10E8 and
 * fn_800F16C0, so it is register shaping and is not used. CodeCandidate.
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"
#include "game/gs_vm.h"

extern u32 fn_800D3088(void);
extern void _threadSwitch(void);

/* 0x800F13D0 | 0x2F0 */
/*
 * Native "wait" call: yields the script thread (_threadSwitch) until the
 * per-resume deltas from fn_800D3088 add up to argument 0.  The VM state byte
 * (+0x04) is 2 while waiting; an abort request (4 -> 3) leaves early with 0.
 */
s32 fn_800F13D0(GSVMCtx* ctx)
{
    u16 argc;
    s32 wait;
    s32 elapsed;
    u32* args;
    u8* ip;

    ctx->ip += 2;
    ip = ctx->ip;
    argc = *(u16*)ip;
    ctx->ip = ip + 2;
    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, argc);
    ctx->frame = ctx->stackCount - (argc + 2);
    args = &ctx->stack[ctx->frame];
    wait = args[0];
    ctx->status = 2;
    for (elapsed = 0; elapsed < wait; elapsed += fn_800D3088()) {
        if (ctx->status == 4) {
            ctx->status = 3;
        }
        if (ctx->status == 3) {
            GSvmReturn(ctx);
            return 0;
        }
        _threadSwitch();
    }
    ctx->status = 1;
    GSvmReturn(ctx);
    return 1;
}
