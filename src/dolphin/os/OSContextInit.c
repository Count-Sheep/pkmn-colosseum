#include "dolphin/db/DB.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"

#define OS_FPUCONTEXT (*(OSContext* volatile*)0x800000D8)

void __OSContextInit(void) {
    __OSSetExceptionHandler(OS_EXCEPTION_FLOATING_POINT,
                            (__OSExceptionHandler)OSSwitchFPUContext);
    OS_FPUCONTEXT = 0;
    DBPrintf("FPU-unavailable handler installed\n");
}
