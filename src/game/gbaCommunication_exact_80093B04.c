#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

void fn_80093B04(u32 a, u32 b)
{
    u32 r31;
    u32 result;

    r31 = b;
    result = OSGetCurrentThread();
    if (r31 != 0) {
        if (r31 != result) {
            return;
        }
    }
    fn_800A257C((void*)result, 0x10);
    OSYieldThread();
}
