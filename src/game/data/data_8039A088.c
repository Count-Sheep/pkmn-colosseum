#include "dolphin/types.h"

extern u8 fn_8021D40C[];

void* jumptable_8039A088[8] = {
    (void*)(fn_8021D40C + 0x6C),
    (void*)(fn_8021D40C + 0x78),
    (void*)(fn_8021D40C + 0x25C),
    (void*)(fn_8021D40C + 0x154),
    (void*)(fn_8021D40C + 0x194),
    (void*)(fn_8021D40C + 0x25C),
    (void*)(fn_8021D40C + 0x1CC),
    (void*)(fn_8021D40C + 0x24C),
};
