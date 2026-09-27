/**
 * @file gs_vm_exact_800F16C0.c
 * @brief GS script VM native "print" call.
 *
 * Address range: 0x800F16C0 - 0x800F1A0C (fn_800F16C0).
 *
 * A .text-only unit carved from the GS VM translation unit (game/gs_vm.c,
 * which holds the same definition and passes "\n" as a literal). Here the
 * "\n" literal (.sdata2 lbl_8047CCB8) and the .bss buffers stay extern.
 */

#include "dolphin/types.h"
#include "game/gs_vm.h"

extern void GSlogWrite(const char* fmt, ...);
extern s32 sprintf(u8* buf, const char* fmt, ...);

extern const u8 lbl_8047CCB8[2];         /* "\n" (.sdata2) */

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
    GSlogWrite((const char*)lbl_80401AB8);
    GSvmReturn(ctx);
    return 1;
}
