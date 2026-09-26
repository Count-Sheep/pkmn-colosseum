/**
 * @file gs_vm_exact_800F16C0.c
 * @brief GS script VM native "print" call.
 *
 * Address range: 0x800F16C0 - 0x800F1A0C (fn_800F16C0).
 *
 * The .bss buffers and the "\n" literal (.sdata2 lbl_8047CCB8) stay extern:
 * this unit is .text only.
 *
 * gs_thread.c includes this file so the candidate units built from it keep
 * seeing the same definitions.
 */

#include "dolphin/types.h"
#include "game/gs_vm.h"

extern void GSlogWrite(const void* fmt, ...);
extern s32 sprintf(u8* buf, const char* fmt, ...);

extern u8 lbl_80401A78[];                /* single conversion spec */
extern u8 lbl_80401AB8[];                /* print output buffer */
extern const u8 lbl_8047CCB8[2];         /* "\n" (.sdata2) */

/* 0x800F16C0 | 0x34C */
/*
 * Native "print" call: expands the script's printf-style format string
 * (argument 0) with the following arguments into a 0x100-byte buffer and
 * writes it to the log.  Supports %d/%x/%c/%f/%s conversions (with any
 * flags copied through to sprintf) and the two-character escape "\n".
 * One operand slot, value, is read first as the format pointer and then as
 * each argument: the target keeps the format read in the variable's own
 * stack slot and gives the argument reads a separate, later-allocated slot
 * (0x1c vs 0x8), which is how MWCC splits one variable's independent uses.
 * A %-conversion cut off by the end of the string adds whatever len held
 * before, as retail does.
 */
s32 fn_800F16C0(GSVMCtx* ctx)
{
    u16 argc;
    s32 len;
    s32 argIdx;
    s8* fmt;
    u8* out;
    s32 n;
    u32* args;
    GSVMValue value;
    u8* ip;

    ctx->ip += 2;
    ip = ctx->ip;
    argc = *(u16*)ip;
    ctx->ip = ip + 2;
    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, argc);
    ctx->frame = ctx->stackCount - (argc + 2);
    out = lbl_80401AB8;
    argIdx = 1;
    args = &ctx->stack[ctx->frame];
    value.u = args[0];
    fmt = (s8*)value.p;
    while (*fmt != 0) {
        if (*fmt == '%') {
            value.u = ctx->stack[ctx->frame + argIdx++];
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
                len = sprintf(out, (const char*)lbl_8047CCB8);
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
    GSlogWrite(lbl_80401AB8);
    GSvmReturn(ctx);
    return 1;
}
