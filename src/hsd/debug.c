/**
 * @file debug.c
 * @brief HAL sysdolphin debug.c: HSD_Panic and __assert,
 *        0x80196D78-0x80196EB4.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/debug.c) and checked against Colosseum's retail
 * code. The newer sysdolphin saves the panic context with its own
 * HSD_SaveContext (a hand-written copy of OSSaveContext that also stores
 * the GQRs and chains to OSFillFPUContext) instead of the SDK's
 * OSSaveContext, and __assert inlines HSD_Panic. The library is built with
 * deferred inlining, so MWCC emits the C functions in reverse source order.
 *
 * HSD_SaveContext (0x80196CE0) is assembly in HAL's source and is left to
 * the generated assembly; this unit starts at HSD_Panic and owns the rest
 * of debug.c's code and data. The panic context (lbl_80465080) stays a
 * global because HSD_SaveContext stores into it.
 */
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/types.h"
#include "sysdolphin/baselib/debug.h"

typedef void (*ReportCallback)(const char* str, u32 len);
/* Retail calls the panic callback with cr1 cleared, as for a variadic
 * function, so its type takes a variable argument list. */
typedef void (*PanicCallback)(OSContext* context, ...);

/* OSPanic */
void fn_800060F0(const char* file, s32 line, const char* msg, ...);
/* Saves into lbl_80465080 itself. */
void HSD_SaveContext(void);

/* HAL's panic context. */
OSContext lbl_80465080;

static ReportCallback reportCallback;
static PanicCallback panicCallback;

void __assert(const char* file, u32 line, const char* expr)
{
    OSReport("assertion \"%s\" failed", expr);
    HSD_Panic(file, line, "");
}

void HSD_Panic(const char* file, u32 line, const char* msg)
{
    if (panicCallback != NULL) {
        HSD_SaveContext();
        OSReport("%s in %s on line %d.\n", msg, file, line);
        panicCallback(&lbl_80465080);
    }
    fn_800060F0(file, line, msg);
}

void HSD_SetReportCallback(ReportCallback cb)
{
    reportCallback = cb;
}

void HSD_SetPanicCallback(PanicCallback cb)
{
    panicCallback = cb;
}
