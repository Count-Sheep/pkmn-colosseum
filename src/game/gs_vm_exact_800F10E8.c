/**
 * @file gs_vm_exact_800F10E8.c
 * @brief GS script VM "call native" opcode.
 *
 * Address range: 0x800F10E8 - 0x800F13D0 (fn_800F10E8).
 *
 * It follows fn_800F106C, the hand-written native-call trampoline that opens
 * the GS VM code: that function saves r0 into its own frame and clears
 * cr1eq although it passes float arguments in f1-f8, which compiled code
 * never does, so it stays in its own unit. The GS VM's .rodata strings,
 * .bss argument images and .sbss call registers stay extern: this unit is
 * .text only.
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"
#include "game/gs_vm.h"

extern void* memset(void* dest, int val, u32 n);
extern u32 fn_800F106C(void);

extern u32 lbl_80478B00;                 /* GS VM manager */
extern u8 lbl_80401BB8[];                /* native call: float argument image */
extern u8 lbl_80401BD8[];                /* native call: integer argument image */
extern u32 lbl_8047AC38;                 /* native call: function */
extern u32 lbl_8047AC3C;                 /* native call: float argument image */
extern u32 lbl_8047AC40;                 /* native call: integer argument image */

/* 0x800F10E8 | 0x2E8 */
/* Script VM "call native" opcode: builds a call frame from the operand
 * stack, looks the callee up in the native-function table (0xC-byte records:
 * function pointer + 8 argument-type bytes, 2 = float, 0 = end), splits the
 * arguments into the integer/float register images consumed by
 * fn_800F106C, stores the result in the frame's return slot and unwinds. */
s32 fn_800F10E8(GSVMCtx* ctx)
{
    u32 argCount;
    u8* record;
    u32 result;
    s32 valueCount;
    s32 intArgCount;
    s32 floatArgCount;
    s32 i;

    result = 0;
    ctx->ip += 2;
    argCount = *(u16*)ctx->ip;
    ctx->ip += 2;

    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, argCount);
    ctx->frame = ctx->stackCount - (argCount + 2);

    record = (u8*)*(u32*)((u8*)lbl_80478B00 + 0x10) + *GSvmFrameSlot(ctx, 0) * 0xC;
    lbl_8047AC38 = *(u32*)record;
    if (lbl_8047AC38 != 0) {
        floatArgCount = 0;
        intArgCount = 0;
        memset(lbl_80401BD8, 0, 0x20);
        memset(lbl_80401BB8, 0, 0x20);
        valueCount = ctx->stackCount - ctx->frame - 3;
        if (valueCount > 8) {
            valueCount = 8;
        }
        for (i = 0; i < valueCount; i++) {
            if (record[i + 4] == 0) {
                break;
            }
            if (record[i + 4] == 2) {
                ((f32*)lbl_80401BB8)[floatArgCount++] = ((GSVMValue*)GSvmFrameSlot(ctx, i + 1))->f;
            } else {
                ((u32*)lbl_80401BD8)[intArgCount++] = *GSvmFrameSlot(ctx, i + 1);
            }
        }
        lbl_8047AC3C = (u32)lbl_80401BB8;
        lbl_8047AC40 = (u32)lbl_80401BD8;
        result = fn_800F106C();
    }

    ctx->stack[ctx->frame - 1] = result;
    GSvmReturn(ctx);
    return 1;
}
