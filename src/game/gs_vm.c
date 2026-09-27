/**
 * @file gs_vm.c
 * @brief GSvm -- Genius Sonority script virtual machine (the whole unit).
 *
 * Address range: 0x800F10E8 - 0x800F78A4 (fn_800F10E8 .. fn_800F7758), with
 * its data: the .rodata string pool 0x80271068-0x80271300, the opcode table
 * and fn_800F7318's name (.data 0x803155D0-0x80315678), the manager pointer
 * (.sdata 0x80478B00), "\n", 0.0f and the int-to-float bias (.sdata2
 * 0x8047CCB8-0x8047CCC8), the GSVMValue zero initializer (.sbss2
 * 0x8047E710), the native-call registers (.sbss 0x8047AC38) and buffers
 * (.bss 0x80401A78-0x80401C10). See include/game/gs_vm.h. The unit opens
 * with fn_800F106C, the hand-written native-call trampoline, which stays in
 * its own unit.
 *
 * Build: GC/1.3.2 -O4,p with the project's normal inlining, -rostr (the
 * pool is .rodata). No pragmas. Every definition is in address order and
 * the string pool comes out in retail's first-use order.
 *
 * This file is the complete unit. Until all of it is exact it is scored as
 * a CodeCandidate over 0x800F1A0C-0x800F7068 (the pool-owning middle); the
 * functions that do not keep the pool base in a register are linked from
 * .text-only units with their data extern (gs_vm_exact_800F10E8.c,
 * gs_vm_exact_800F13D0.c, gs_vm_exact_800F16C0.c, gs_vm_exact_800F7068.c,
 * gs_vm_exact_800F7274.c, gs_vm_exact_800F7318.c, input_exact_800F75FC.c,
 * input_candidate_800F760C.c, input_exact_800F76E4.c, input.c's
 * fn_800F7758); fn_800F716C and fn_800F7434 are candidates of their own.
 *
 * Reconstructed static inline helpers (all repeated expansions):
 * GSvmPush/GSvmPushValue/GSvmPop/GSvmFrameSlot/GSvmEnterNative/GSvmReturn
 * (gs_vm.h), GSvmGetOperand (every operand read: two per binary operator,
 * one in fn_800F24F4/fn_800F5404/fn_800F55DC/fn_800F57F0/fn_800F5A3C/
 * fn_800F5CA0), and in gs_vm.h GSvmFindByKey (fn_800F6D18, fn_800F7068,
 * fn_800F7108, fn_800F716C, fn_800F7274) and GSvmStopByKey (fn_800F716C,
 * fn_800F7274).
 * setValue (fn_800F24F4) is named by retail's own message, "setValue
 * 例外エラー...", the same way "_codeStructAssign:" names fn_800F2264; it
 * has no symbol of its own, so it was inlined.
 *
 * Not yet exact (they block linking the unit):
 *   fn_800F5A3C  89.9  the loop hoists the operand descriptor's bit tests;
 *                      retail tests them in the loop and keeps the start
 *                      index in a register.
 *   fn_800F5CA0  99.5  the descriptor and the pool base swap r30/r31.
 *   fn_800F6D18  98.5  retail computes key+1 into r0 and copies it to the
 *                      key's register (mr r29,r0); here the key is
 *                      incremented in place.
 *   fn_800F716C  99.8  retail compares "cmplw r3,r30" (group, id); this
 *                      gives "cmplw r30,r3". MWCC keeps the source order
 *                      when both sides are pointers (fn_800F0374
 *                      returning void* and a void* group) or when the
 *                      call result goes through a local first; nothing
 *                      else shows the thread group (the floor id,
 *                      fn_800FF560) to be a pointer or that local, so
 *                      neither is used.
 */

#include "dolphin/types.h"
#include "crt/stdarg.h"
#include "game/gs_thread.h"
#define GS_VM_TU
#include "game/gs_vm.h"

extern void* memset(void* dest, int val, u32 n);
extern s32 sprintf(u8* buf, const char* fmt, ...);
extern f64 fmod(f64 value, f64 modulus);
extern void GSlogWrite(const char* fmt, ...);
extern void GSlogWritef(const char* fmt, ...);
extern u32 fn_800F106C(void);
extern u32 fn_800D3088(void);
extern void _threadSwitch(void);
extern u32 fn_800F0374(GSThread* thread);
extern void GSthreadTerminate(GSThread* thread);
extern u32 fn_800FF560(void);
extern void GSthreadSetArgs(void* thread, s32 count, ...);
extern u16 _toolentryAlloc__FUl(u32 size);   /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);         /* GSmemGetPtr */

u8 lbl_80401A78[0x40];          /* print: one conversion spec */
u8 lbl_80401AB8[0x100];         /* print: output buffer */
f32 lbl_80401BB8[8];            /* native call: float argument image */
u32 lbl_80401BD8[8];            /* native call: integer argument image */
GSVMPool lbl_80401BF8;          /* the VM manager */
GSVMPool* lbl_80478B00 = &lbl_80401BF8;
u32 (*lbl_8047AC38)(void);      /* native call: function */
f32* lbl_8047AC3C;              /* native call: float argument image */
u32* lbl_8047AC40;              /* native call: integer argument image */

/*
 * Operand fetch.  Each opcode byte is followed by one descriptor byte per
 * operand: 0 is undefined (logged, reads as 0), bit 0x80 means the value
 * itself is on the operand stack, otherwise the stack holds a variable index
 * into the frame (0x40) or the globals, and 0x20/0x100 ask for the variable's
 * address instead of its contents.  The low six bits are the type (2 = int,
 * anything else is read as a float).
 */
static inline GSVMValue GSvmGetOperand(GSVMCtx* ctx, u16 desc)
{
    GSVMValue value = {0};
    GSVMValue idx;
    GSVMValue* addr;

    if (desc == 0) {
        GSlogWritef("変数の型が未定義です:[%02x]\n", desc);
        return value;
    }
    if (desc & 0x80) {
        value = GSvmPop(ctx);
    } else {
        idx = GSvmPop(ctx);
        if (desc & 0x20) {
            if (desc & 0x40) {
                value.u = (u32)&ctx->stack[ctx->frame + idx.s];
            } else {
                value.u = (u32)&ctx->globals[idx.s];
            }
        } else {
            if (desc & 0x40) {
                addr = &ctx->stack[ctx->frame + idx.s];
            } else {
                addr = &ctx->globals[idx.s];
            }
            if (desc & 0x100) {
                value.u = (u32)addr;
            } else {
                value = *addr;
            }
        }
    }
    return value;
}

/* 0x800F10E8 | 0x2E8 */
/* Script VM "call native" opcode: builds a call frame from the operand
 * stack, looks the callee up in the native-function table (0xC-byte records:
 * function pointer + 8 argument-type bytes, 2 = float, 0 = end), splits the
 * arguments into the integer/float register images consumed by
 * fn_800F106C, stores the result in the frame's return slot and unwinds. */
s32 fn_800F10E8(GSVMCtx* ctx)
{
    u8* record;
    u32 result;
    s32 valueCount;
    s32 intArgCount;
    s32 floatArgCount;
    s32 i;

    result = 0;
    GSvmEnterNative(ctx);

    record = (u8*)lbl_80478B00->natives + GSvmFrameSlot(ctx, 0)->u * 0xC;
    lbl_8047AC38 = *(u32 (**)(void))record;
    if (lbl_8047AC38 != NULL) {
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
                lbl_80401BB8[floatArgCount++] = GSvmFrameSlot(ctx, i + 1)->f;
            } else {
                lbl_80401BD8[intArgCount++] = GSvmFrameSlot(ctx, i + 1)->u;
            }
        }
        lbl_8047AC3C = lbl_80401BB8;
        lbl_8047AC40 = lbl_80401BD8;
        result = fn_800F106C();
    }

    ctx->stack[ctx->frame - 1].u = result;
    GSvmReturn(ctx);
    return 1;
}

/* 0x800F13D0 | 0x2F0 */
/*
 * Native "wait" call: yields the script thread (_threadSwitch) until the
 * per-resume deltas from fn_800D3088 add up to argument 0.  The VM state byte
 * (+0x04) is 2 while waiting; an abort request (4 -> 3) leaves early with 0.
 */
s32 fn_800F13D0(GSVMCtx* ctx)
{
    s32 wait;
    s32 elapsed;
    GSVMValue* args;

    GSvmEnterNative(ctx);
    args = &ctx->stack[ctx->frame];
    wait = args[0].s;
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

/* 0x800F16C0 | 0x34C */
/*
 * Native "print" call: expands the script's printf-style format string
 * (argument 0) with the following arguments into a 0x100-byte buffer and
 * writes it to the log.  Supports %d/%x/%c/%f/%s conversions (with any
 * flags copied through to sprintf) and the two-character escape "\n".
 * A %-conversion cut off by the end of the string adds whatever len held
 * before, as retail does.
 */
s32 fn_800F16C0(GSVMCtx* ctx)
{
    s32 len;
    s32 argIdx;
    s8* fmt;
    u8* out;
    s32 n;
    GSVMValue* args;
    GSVMValue value;

    GSvmEnterNative(ctx);
    out = lbl_80401AB8;
    argIdx = 1;
    args = &ctx->stack[ctx->frame];
    value = args[0];
    fmt = (s8*)value.p;
    while (*fmt != 0) {
        if (*fmt == '%') {
            value = ctx->stack[ctx->frame + argIdx++];
            n = 0;
            for (;;) {
                lbl_80401A78[n++] = *fmt;
                if (*fmt == 'd' || *fmt == 'x' || *fmt == 'c') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.s);
                    break;
                } else if (*fmt == 'f') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.f);
                    break;
                } else if (*fmt == 's') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.p);
                    break;
                } else if (*fmt == 0) {
                    break;
                }
                fmt++;
            }
            out += len;
        } else if (*fmt == '\\') {
            switch (fmt[1]) {
            case 'n':
                len = sprintf(out, "\n");
                fmt++;
                out += len;
                break;
            default:
                *out++ = *fmt;
                break;
            }
        } else {
            *out++ = *fmt;
        }
        fmt++;
        if (out >= lbl_80401AB8 + 0xFF) {
            break;
        }
    }
    *out = 0;
    GSlogWrite((const char*)lbl_80401AB8);
    GSvmReturn(ctx);
    return 1;
}

/* 0x800F1A0C | 0x42C */
/* Script VM logical OR: the first descriptor names the right-hand operand
 * (top of stack), the second the left; the result (0/1) is pushed. */
s32 fn_800F1A0C(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s || rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s || rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f || rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f || rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F1E38 | 0x42C */
/* Script VM logical AND; same operand handling as fn_800F1A0C. */
s32 fn_800F1E38(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s && rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s && rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f && rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f && rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}



/* 0x800F2264 | 0x290 */
/* Script VM struct assignment (_codeStructAssign): copies `count` words
 * from the source variable block (or the top of the stack) to the
 * destination variable block and pushes the copied words. */
s32 fn_800F2264(GSVMCtx* ctx)
{
    u8 srcDesc;
    u8 dstDesc;
    u16 count;
    GSVMValue srcIdx;
    GSVMValue dstIdx;
    GSVMValue* src;
    GSVMValue* dst;
    s32 i;

    srcDesc = *ctx->ip++;
    dstDesc = *ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    if (dstDesc & 0x80) {
        GSlogWritef("_codeStructAssign:  エラー：定数に数値のセットはできません\n");
        return 1;
    }
    if (srcDesc & 0x80) {
        ctx->stackCount -= count;
        src = &ctx->stack[ctx->stackCount];
    } else {
        srcIdx = GSvmPop(ctx);
        if (srcDesc & 0x40) {
            src = &ctx->stack[ctx->frame + srcIdx.s];
        } else {
            src = &ctx->globals[srcIdx.s];
        }
    }
    dstIdx = GSvmPop(ctx);
    if (dstDesc & 0x40) {
        dst = &ctx->stack[ctx->frame + dstIdx.s];
    } else {
        dst = &ctx->globals[dstIdx.s];
    }
    for (i = 0; i < count; i++) {
        dst[i] = src[i];
    }
    for (i = 0; i < count; i++) {
        GSvmPushValue(ctx, dst[i]);
    }
    return 1;
}

/* setValue */
static inline void setValue(GSVMCtx* ctx, u16 desc, GSVMValue value)
{
    GSVMValue idx;

    if (desc == 0) {
        GSlogWritef("変数の型が未定義:[%02x]\n", desc);
    } else if (desc & 0x80) {
        GSlogWritef("setValue 例外エラー：定数に数値のセットはできません\n");
    } else {
        idx = GSvmPop(ctx);
        if (desc & 0x40) {
            ctx->stack[ctx->frame + idx.s] = value;
        } else {
            ctx->globals[idx.s] = value;
        }
    }
}

/* 0x800F24F4 | 0x2E0 */
/* Script VM assignment (setValue): converts the source operand to the
 * destination's type, stores it and pushes it. */
s32 fn_800F24F4(GSVMCtx* ctx)
{
    u8 dstDesc;
    u8 srcDesc;
    GSVMValue value;

    srcDesc = *ctx->ip++;
    dstDesc = *ctx->ip++;
    value = GSvmGetOperand(ctx, srcDesc);
    if ((dstDesc & 0x3F) != (srcDesc & 0x3F)) {
        if ((srcDesc & 0x3F) == 2) {
            value.f = value.s;
        } else {
            value.s = value.f;
        }
    }
    setValue(ctx, dstDesc, value);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F27D4: script VM "lhs != rhs" (operands as in fn_800F1A0C). */
s32 fn_800F27D4(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s != rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s != rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f != rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f != rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F2BE8: script VM "lhs == rhs" (operands as in fn_800F1A0C). */
s32 fn_800F2BE8(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s == rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s == rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f == rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f == rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F2FF8: script VM "lhs >= rhs" (operands as in fn_800F1A0C). */
s32 fn_800F2FF8(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s >= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s >= rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f >= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f >= rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3418: script VM "lhs > rhs" (operands as in fn_800F1A0C). */
s32 fn_800F3418(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s > rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s > rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f > rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f > rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3830: script VM "lhs <= rhs" (operands as in fn_800F1A0C). */
s32 fn_800F3830(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s <= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s <= rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f <= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f <= rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3C50: script VM "lhs < rhs" (operands as in fn_800F1A0C). */
s32 fn_800F3C50(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s < rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s < rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f < rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f < rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4068: script VM "lhs - rhs" (operands as in fn_800F1A0C). */
s32 fn_800F4068(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s - rhs.s;
        } else {
            result.f = lhs.s - rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f - rhs.s;
        } else {
            result.f = lhs.f - rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4440: script VM "lhs + rhs" (operands as in fn_800F1A0C). */
s32 fn_800F4440(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s + rhs.s;
        } else {
            result.f = lhs.s + rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f + rhs.s;
        } else {
            result.f = lhs.f + rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4818: script VM "lhs % (fmod of the integer lhs when a float is involved) rhs" (operands as in fn_800F1A0C). */
s32 fn_800F4818(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if (rhs.s != 0) {
        if ((lhsDesc & 0x3F) == 2U) {
            if ((rhsDesc & 0x3F) == 2U) {
                result.s = lhs.s % rhs.s;
            } else {
                result.f = fmod(lhs.s, rhs.f);
            }
        } else {
            if ((rhsDesc & 0x3F) == 2U) {
                result.f = fmod(lhs.s, rhs.f);
            } else {
                result.f = fmod(lhs.s, rhs.f);
            }
        }
    } else {
        GSlogWritef("０で除算しようとしました\n");
        result.u = 0;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4C38: script VM "lhs / rhs" (operands as in fn_800F1A0C). */
s32 fn_800F4C38(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if (rhs.s != 0) {
        if ((lhsDesc & 0x3F) == 2U) {
            if ((rhsDesc & 0x3F) == 2U) {
                result.s = lhs.s / rhs.s;
            } else {
                result.f = lhs.s / rhs.f;
            }
        } else {
            if ((rhsDesc & 0x3F) == 2U) {
                result.f = lhs.f / rhs.s;
            } else {
                result.f = lhs.f / rhs.f;
            }
        }
    } else {
        GSlogWritef("０で除算しようとしました\n");
        result.u = 0;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F502C: script VM "lhs * rhs" (operands as in fn_800F1A0C). */
s32 fn_800F502C(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s * rhs.s;
        } else {
            result.f = lhs.s * rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f * rhs.s;
        } else {
            result.f = lhs.f * rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}


/* 0x800F5404 | 0x1D8: push an operand. */
s32 fn_800F5404(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F55DC | 0x214: "-operand". */
s32 fn_800F55DC(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;
    GSVMValue result;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    if ((desc & 0x3F) == 2) {
        result.s = -value.s;
    } else {
        result.f = -value.f;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F57F0 | 0x24C: "!operand". */
s32 fn_800F57F0(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;
    GSVMValue result;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    if ((desc & 0x3F) == 2) {
        result.s = !value.s;
    } else {
        result.f = !value.f;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F5A3C | 0x264: push `count` consecutive variables, each preceded by
 * its index, starting at the popped index. */
s32 fn_800F5A3C(GSVMCtx* ctx)
{
    u8 desc;
    u16 count;
    GSVMValue start;
    GSVMValue value;
    s32 i;

    desc = *ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    start = GSvmPop(ctx);
    for (i = 0; i < count; i++) {
        GSvmPush(ctx, start.u + i);
        value = GSvmGetOperand(ctx, desc);
        GSvmPushValue(ctx, value);
    }
    return 1;
}

/* 0x800F5CA0 | 0x24C: keep the popped index and push the address of the
 * variable it names. */
s32 fn_800F5CA0(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue index;
    GSVMValue value;

    desc = *ctx->ip++;
    index = GSvmPop(ctx);
    GSvmPush(ctx, index.u);
    value = GSvmGetOperand(ctx, desc | 0x100);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F5EEC: script VM "lhs | (bitwise) rhs" (operands as in fn_800F1A0C). */
s32 fn_800F5EEC(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s | rhs.s;
        } else {
            result.s = lhs.s | (s32)rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (s32)lhs.f | rhs.s;
        } else {
            result.s = (s32)lhs.f | (s32)rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F62BC: script VM "lhs & (bitwise) rhs" (operands as in fn_800F1A0C). */
s32 fn_800F62BC(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s & rhs.s;
        } else {
            result.s = lhs.s & (s32)rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (s32)lhs.f & rhs.s;
        } else {
            result.s = (s32)lhs.f & (s32)rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F668C | 0x80: push the immediate word that follows the type byte. */
s32 fn_800F668C(GSVMCtx* ctx)
{
    ctx->ip++;
    GSvmPush(ctx, *(u32*)ctx->ip);
    ctx->ip += 4;
    return 1;
}

/* 0x800F670C | 0xA0: branch to the bank offset that follows when the
 * popped value is zero. */
s32 fn_800F670C(GSVMCtx* ctx)
{
    GSVMValue cond;

    cond = GSvmPop(ctx);
    if (cond.s != 0) {
        ctx->ip += 4;
    } else {
        ctx->ip = (u8*)ctx->script + *(u32*)ctx->ip;
    }
    return 1;
}

/* 0x800F67AC | 0x1C: branch to the bank offset that follows. */
s32 fn_800F67AC(GSVMCtx* ctx)
{
    ctx->ip = (u8*)ctx->script + *(u32*)ctx->ip;
    return 1;
}

/* 0x800F67C8 | 0x184: return from a script function. */
s32 fn_800F67C8(GSVMCtx* ctx)
{
    GSVMValue argc;
    GSVMValue frame;
    GSVMValue ret;
    s32 i;

    argc = GSvmPop(ctx);
    frame = GSvmPop(ctx);
    ctx->frame = frame.s;
    ret = GSvmPop(ctx);
    ctx->ip = (u8*)ret.p;
    for (i = 0; i < argc.s; i++) {
        GSvmPop(ctx);
    }
    if (ctx->ip == NULL) {
        ctx->status = 3;
        return 0;
    }
    return 1;
}

/* 0x800F694C | 0x168: call a script function. */
s32 fn_800F694C(GSVMCtx* ctx)
{
    u32 func;
    u16 argc;
    u16 locals;
    GSVMScript* script;

    func = *(u32*)ctx->ip;
    ctx->ip += 4;
    argc = *(u16*)ctx->ip;
    ctx->ip += 2;
    locals = *(u16*)ctx->ip;
    ctx->ip += 2;
    GSvmPush(ctx, (u32)ctx->ip);
    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, locals);
    ctx->frame = ctx->stackCount - (argc + locals + 3);
    script = ctx->script;
    if (func >= script->funcCount) {
        GSlogWritef("設定する関数のインデックス[%d]が不正です\n", func);
    } else {
        ctx->ip = (u8*)script + script->funcOffsets[func];
    }
    return 1;
}

/* 0x800F6AB4 | 0xA0: drop `count` stack values. */
s32 fn_800F6AB4(GSVMCtx* ctx)
{
    u16 count;
    s32 i;

    ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    for (i = 0; i < count; i++) {
        GSvmPop(ctx);
    }
    return 1;
}

/* 0x800F6B54 | 0x58: push -1. */
s32 fn_800F6B54(GSVMCtx* ctx)
{
    GSvmPush(ctx, -1);
    return 1;
}

/* 0x800F6BAC | 0x10: stop the script. */
s32 fn_800F6BAC(GSVMCtx* ctx)
{
    ctx->status = 3;
    return 0;
}

/* 0x800F6BBC | 0x8: no operation. */
s32 fn_800F6BBC(GSVMCtx* ctx)
{
    return 1;
}

/* Opcode handlers, indexed by the opcode byte. */
s32 (*lbl_803155D0[0x26])(GSVMCtx* ctx) = {
    fn_800F6BBC, fn_800F6BAC, fn_800F6B54, fn_800F6AB4,
    fn_800F694C, fn_800F67C8, fn_800F67AC, fn_800F670C,
    fn_800F668C, NULL,        NULL,        NULL,
    NULL,        fn_800F62BC, fn_800F5EEC, fn_800F5CA0,
    fn_800F5A3C, fn_800F57F0, fn_800F55DC, fn_800F5404,
    fn_800F502C, fn_800F4C38, fn_800F4818, fn_800F4440,
    fn_800F4068, fn_800F3C50, fn_800F3830, fn_800F3418,
    fn_800F2FF8, fn_800F2BE8, fn_800F27D4, fn_800F24F4,
    fn_800F2264, fn_800F1E38, fn_800F1A0C, fn_800F16C0,
    fn_800F13D0, fn_800F10E8,
};

/* 0x800F6BC4 | 0x154: run a script until it stops; returns (and passes
 * to the completion callback) the value left on the stack. */
u32 fn_800F6BC4(GSVMCtx* ctx)
{
    u8 op;
    s32 (*handler)(GSVMCtx*);
    s32 i;
    GSVMValue result;

    for (;;) {
        if (ctx->status == 0) {
            GSlogWrite("script %08X terminated by status == STANDBY\n", ctx->scriptId);
            break;
        }
        if (ctx->status == 3) {
            ctx->status = 0;
            break;
        }
        op = *ctx->ip++;
        if ((s32)op >= 0x26) {
            GSlogWritef("サポートされていないコードです:%02x\n", op);
        } else {
            handler = lbl_803155D0[op];
            if (handler != NULL) {
                handler(ctx);
            }
        }
        if (ctx->stackCount > 0) {
            for (i = ctx->stackCount - 1; i >= 0; i--) {
            }
        }
    }
    if (ctx->status == 4) {
        ctx->status = 0;
    }
    result = GSvmPop(ctx);
    if (ctx->callback != NULL) {
        ctx->callback(ctx, result.u);
    }
    return result.u;
}

/* 0x800F6D18 | 0x350: set up a context to run function `scriptId & 0xFFFF`
 * of script `scriptId >> 16` with `argc` arguments. */
GSVMCtx* fn_800F6D18(u32 scriptId, u32 argc, va_list args)
{
    GSVMPool* pool;
    GSVMCtx* ctx;
    GSVMScript* script;
    u32 i;
    u16 count;
    u16 key;
    u16 lastKey;
    u32 func;

    for (i = 0; i < lbl_80478B00->count; i++) {
        ctx = &lbl_80478B00->contexts[i];
        if (ctx->status == 4) {
            ctx->status = 0;
        }
        if (ctx->status == 3) {
            ctx->status = 0;
        }
    }
    pool = lbl_80478B00;
    count = pool->count;
    ctx = pool->contexts;
    for (i = 0; i < count; i++) {
        ctx = &pool->contexts[i];
        if (ctx->status == 0) {
            break;
        }
    }
    if (i == count) {
        GSlogWritef("スクリプトの起動に失敗しました:[%08x] タスクが足りません\n", scriptId);
        return NULL;
    }
    memset(ctx, 0, sizeof(GSVMCtx));
    for (script = lbl_80478B00->scripts; script != NULL; script = script->next) {
        if (script->id == (scriptId >> 16)) {
            ctx->script = script;
            ctx->globals = (GSVMValue*)((u8*)script + script->globalsOffset);
            break;
        }
    }
    if (script == NULL) {
        GSlogWritef("スクリプトの起動に失敗しました:[%08x] ＩＤが見つかりません\n", scriptId);
        return NULL;
    }
    pool = lbl_80478B00;
    lastKey = pool->lastKey;
    key = lastKey;
    for (;;) {
        key++;
        if (key == lastKey) {
            GSlogWritef("スクリプトの起動に失敗しました:[%08x] ハンドルの確保に失敗\n", scriptId);
            return NULL;
        }
        if (key == 0) {
            continue;
        }
        if (GSvmFindByKey(key) == NULL) {
            break;
        }
    }
    pool->lastKey = key;
    func = scriptId & 0xFFFF;
    script = ctx->script;
    if (func >= script->funcCount) {
        GSlogWritef("設定する関数のインデックス[%d]が不正です\n", func);
    } else {
        ctx->ip = (u8*)script + script->funcOffsets[func];
    }
    ctx->status = 1;
    ctx->scriptId = scriptId;
    ctx->key = key;
    GSvmPush(ctx, 0);
    for (i = 0; i < argc; i++) {
        GSvmPush(ctx, va_arg(args, u32));
    }
    GSvmPush(ctx, 0);
    GSvmPush(ctx, 0);
    GSvmPush(ctx, argc);
    return ctx;
}

/* 0x800F7068 | 0xA0: is the script with `key` still running?  With `wait`,
 * yields until it is not. */
s32 fn_800F7068(u16 key, u8 wait)
{
    for (;;) {
        if (GSvmFindByKey(key) == NULL) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

/* 0x800F7108 | 0x64: the thread running the script with `key`. */
GSThread* fn_800F7108(u16 key)
{
    GSVMCtx* ctx;

    ctx = GSvmFindByKey(key);
    if (ctx == NULL) {
        return NULL;
    }
    return ctx->thread;
}

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

/* 0x800F7274 | 0xA4: stop the script with `key`. */
s32 fn_800F7274(u16 key)
{
    return GSvmStopByKey(key);
}

/* 0x800F7318 | 0x11C */
/*
 * Allocates a script task for `script` (fn_800F6D18 pushes argc variadic
 * arguments onto its stack), then runs it on a new GS thread whose entry is
 * the script executor fn_800F6BC4. `done` is called with the task and its
 * result when the script ends. Returns the task key, or 0.
 * The failure message prints the function's own name: retail's .data holds
 * "_vmThreadCreate" right after the opcode table, which is where MWCC puts
 * __FUNCTION__'s string. Under its address name the string reads
 * "fn_800F7318"; the unit's .data can only match once the symbol carries its
 * real name.
 */
u16 fn_800F7318(u32 affinity, u32 script, u32 stackSize, u32 autoStart,
                void* done, u32 argc, ...)
{
    va_list args;
    GSVMCtx* ctx;
    u32 priority;
    GSThread* thread;

    va_start(args, argc);
    ctx = fn_800F6D18(script, argc, args);
    priority = fn_800FF560();
    if (ctx == NULL) {
        return 0;
    }
    thread = GSthreadCreate(affinity, priority, stackSize, 1, autoStart, fn_800F6BC4);
    if (thread != NULL) {
        ctx->thread = thread;
        ctx->callback = (void (*)(GSVMCtx*, u32))done;
        GSthreadSetArgs(thread, 1, ctx);
    } else {
        GSlogWrite("[%s] スレッドのさくせいにしっぱい\n", __FUNCTION__);
    }
    return ctx->key;
}

/* 0x800F7434 | 0x1C8: run a script to completion on the calling thread. */
u32 fn_800F7434(u32 script, u32 argc, ...)
{
    va_list args;
    GSVMCtx* ctx;

    va_start(args, argc);
    ctx = fn_800F6D18(script, argc, args);
    if (ctx == NULL) {
        return 0;
    }
    return fn_800F6BC4(ctx);
}

/* 0x800F75FC | 0x10: install the native-function table. */
s32 fn_800F75FC(void* natives)
{
    lbl_80478B00->natives = natives;
    return 0;
}

/* 0x800F760C | 0xD8: unlink `script` from the loaded-script list, then
 * mark every context running it for termination (state 3). */
s32 fn_800F760C(GSVMScript* script)
{
    GSVMPool* pool = lbl_80478B00;
    GSVMScript* prev;
    GSVMScript* cur;
    u32 offset;
    GSVMCtx* ctx;
    s32 i;
    u8 state;

    cur = pool->scripts;
    if (cur == script) {
        pool->scripts = cur->next;
    } else {
        prev = cur;
        while ((cur = prev->next) != NULL) {
            if (cur == script) {
                prev->next = cur->next;
                goto scan;
            }
            prev = cur;
        }
        if (cur == NULL) {
            GSlogWritef("スクリプトのアンマップに失敗しました:[%d]\n", script);
            return -1;
        }
    }

scan:
    i = 0;
    offset = 0;
    state = 3;
    while (i < (s32)lbl_80478B00->count) {
        ctx = (GSVMCtx*)((u8*)lbl_80478B00->contexts + offset);
        if (ctx->status != 0) {
            if (script->id == (u16)(ctx->scriptId >> 16)) {
                ctx->status = state;
            }
        }
        offset += sizeof(GSVMCtx);
        i++;
    }
    return 0;
}

/* 0x800F76E4 | 0x74: link a loaded script into the list and relocate it. */
void fn_800F76E4(GSVMScript* script)
{
    GSVMScript* head;
    u32 off;
    u32* tbl;
    s32 i;

    script->next = NULL;
    head = lbl_80478B00->scripts;
    if (head == NULL) {
        lbl_80478B00->scripts = script;
    } else {
        lbl_80478B00->scripts = script;
        script->next = head;
    }
    if (script->relocated != 0) {
        return;
    }
    tbl = (u32*)((u8*)script + script->relocOffset);
    i = 0;
    while (i < (s32)script->relocCount) {
        off = *tbl;
        tbl += 1;
        i += 1;
        *(u32*)((u8*)script + off) += (u32)script;
    }
    script->relocated = 1;
}

/* 0x800F7758 | 0x14C: allocate `count` script VM contexts and reset the
 * manager. */
s32 fn_800F7758(u16 count)
{
    GSVMCtx* ctx;
    s32 i;

    memset(&lbl_80401BF8, 0, sizeof(GSVMPool));
    lbl_80478B00 = &lbl_80401BF8;
    lbl_80478B00->handle = _toolentryAlloc__FUl(count * sizeof(GSVMCtx));
    if (lbl_80478B00->handle == 0) {
        GSlogWritef("GSvmの初期化に失敗しました\n");
        return -1;
    }

    lbl_80478B00->contexts = (GSVMCtx*)fn_800E27B0(lbl_80478B00->handle);
    ctx = lbl_80478B00->contexts;
    for (i = 0; i < count; i++) {
        ctx->script = NULL;
        ctx->status = 0;
        ctx++;
    }
    lbl_80478B00->count = count;
    return 0;
}
