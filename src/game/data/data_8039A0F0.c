#include "dolphin/types.h"

extern u8 fn_80220868[];

void* jumptable_8039A0F0[8] = {
    (void*)(fn_80220868 + 0x244),
    (void*)(fn_80220868 + 0x20C),
    (void*)(fn_80220868 + 0x214),
    (void*)(fn_80220868 + 0x21C),
    (void*)(fn_80220868 + 0x224),
    (void*)(fn_80220868 + 0x22C),
    (void*)(fn_80220868 + 0x234),
    (void*)(fn_80220868 + 0x23C),
};

void* jumptable_8039A110[8] = {
    (void*)(fn_80220868 + 0xEC),
    (void*)(fn_80220868 + 0xB4),
    (void*)(fn_80220868 + 0xBC),
    (void*)(fn_80220868 + 0xC4),
    (void*)(fn_80220868 + 0xCC),
    (void*)(fn_80220868 + 0xD4),
    (void*)(fn_80220868 + 0xDC),
    (void*)(fn_80220868 + 0xE4),
};
