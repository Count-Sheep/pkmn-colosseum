#include "dolphin/types.h"

/* NAME-T1 fn_80089F58 */
u32 gbaCommandGetGBAChangePokemon(u32 value)
{
    return value & 0xFFFF;
}

u32 fn_80089F60(u32 value)
{
    return (value >> 8) & 0xFF;
}

u32 fn_80089F68(u32 value)
{
    return value & 0xFF;
}

/* NAME-T1 fn_80089F70 */
u32 gbaCommandGetAction(u32 value)
{
    return value >> 16;
}
