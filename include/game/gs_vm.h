/**
 * @file gs_vm.h
 * @brief GSvm -- Genius Sonority script virtual machine (shared layout and
 * operand-stack helpers).
 *
 * The GS VM translation unit, as far as the retail data shows:
 *   .text   0x800F106C-0x800F78A4: fn_800F106C (hand-written native-call
 *           trampoline) up to fn_800F7758, the VM initializer (it prints
 *           the pool's last string, "GSvm ... failed", and fills
 *           lbl_80401BF8; its code is currently scored in input.c's unit).
 *   .rodata 0x80271068-0x80271300: one literal pool, "Stack overflow.\n",
 *           "Stack underflow.\n", the undefined-operand message, ... up to
 *           the init failure message. Handlers address it through a base
 *           register (base+0x14, +0x28, +0x120, ...).
 *   .data   0x803155D0-0x80315678: the 0x26-entry opcode table (0 = fn_800F6BBC
 *           ... 0x25 = fn_800F10E8) and "_vmThreadCreate", the name
 *           fn_800F7318 prints.
 *   .sdata  0x80478B00 (VM manager), .sdata2 0x8047CCB8-0x8047CCC8 ("\n",
 *           0.0f, the int-to-float bias), .sbss2 0x8047E710 (the zero
 *           initializer of a GSVMValue), .sbss 0x8047AC38-0x8047AC48 (the
 *           native-call registers), .bss 0x80401A78-0x80401C10 (print
 *           buffers, native-call argument images, init state).
 * Compiler: GC/1.3.2 -O4,p. With pooled literals, GC/1.3 passes the pool's
 * first string as "mr r3,r31"; retail has "addi r3,r31,0x0" (fn_800F1A0C,
 * fn_800F1E38, fn_800F6D18), which GC/1.3.2 emits.
 *
 * Because the pool is TU-local and read by nearly every function in the
 * range, functions that keep its base in a register (fn_800F1A0C,
 * fn_800F1E38, fn_800F6BC4, fn_800F6D18, fn_800F7434, ...) can only be
 * linked once the whole TU is exact and owns its data; functions that
 * address each string separately are linked as .text-only units with the
 * strings extern.
 */
#ifndef GS_VM_H
#define GS_VM_H

#include "dolphin/types.h"

/* GS VM interpreter context (one script task). */
typedef struct GSVMCtx {
    /* 0x00 */ u8    unk00[0x14];
    /* 0x14 */ u8*   ip;
    /* 0x18 */ u32*  globals;
    /* 0x1C */ s32   frame;
    /* 0x20 */ u8    unk20[0x28 - 0x20];
    /* 0x28 */ s32   stackCount;
    /* 0x2C */ u8    unk2C[0x6C - 0x2C];
    /* 0x6C */ u32   stack[0x41]; /* operand stack; a push guards depth > 0x40 */
} GSVMCtx;

/* A GS VM operand-stack slot. Script values are raw 32-bit words that a
 * native call's argument-type table reinterprets as an integer or a float;
 * the pops return it by value, which is why the target spills each popped
 * value through a stack temporary. */
typedef union GSVMValue {
    u32 u;
    s32 s;
    f32 f;
    char* p;
} GSVMValue;

extern void GSlogWritef(const char* fmt, ...);
extern u8 lbl_80271068[];                /* "Stack overflow.\n" */
extern u8 lbl_8027107C[];                /* "Stack underflow.\n" */

/*
 * Operand-stack helpers shared by the GS VM opcode handlers.  Recovered as
 * static inline: the handlers expand the same push/pop/return sequences
 * (fn_800F10E8, fn_800F13D0 twice, fn_800F16C0), each push evaluates its
 * value before the overflow guard, and every pop routes its by-value result
 * through a stack temporary before it lands in the caller's own slot.
 */

/* Frame-relative slot: n == 0 is the callee index, n >= 1 the arguments. */
static inline u32* GSvmFrameSlot(GSVMCtx* ctx, s32 n)
{
    return &ctx->stack[ctx->frame + n];
}

static inline void GSvmPush(GSVMCtx* ctx, u32 value)
{
    s32 sp;

    sp = ctx->stackCount;
    if (sp > 0x40) {
        GSlogWritef((const char*)lbl_80271068);
    } else {
        ctx->stackCount = sp + 1;
        ctx->stack[sp] = value;
    }
}

static inline GSVMValue GSvmPop(GSVMCtx* ctx)
{
    GSVMValue value;
    s32 sp;

    sp = ctx->stackCount;
    if (sp <= 0) {
        GSlogWritef((const char*)lbl_8027107C);
        value.u = ctx->stack[0];
    } else {
        sp--;
        ctx->stackCount = sp;
        value.u = ctx->stack[sp];
    }
    return value;
}

/* Leave a native call frame: pop the saved argument count and frame base,
 * restore the frame and discard the call's arguments. */
static inline void GSvmReturn(GSVMCtx* ctx)
{
    GSVMValue argc;
    GSVMValue frame;
    s32 i;

    argc = GSvmPop(ctx);
    frame = GSvmPop(ctx);
    ctx->frame = frame.s;
    for (i = 0; i < argc.s; i++) {
        GSvmPop(ctx);
    }
}

#endif /* GS_VM_H */
