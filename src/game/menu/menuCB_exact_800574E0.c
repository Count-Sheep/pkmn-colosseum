/** Exact menuCB Pokemon state helpers, 0x800574E0 - 0x80057538. */
#include "dolphin/types.h"

extern u8 lbl_803A9768[];

typedef struct MenuCBPokemonState {
    u32 data[78];
} MenuCBPokemonState;

u8* fn_800574E0(void)
{
    return lbl_803A9768 + *(u32*)(lbl_803A9768 + 0x278) * 0x138 + 8;
}

void fn_800574FC(u8* src)
{
    MenuCBPokemonState* dstState;
    MenuCBPokemonState* srcState;
    u32 slot;

    slot = *(u32*)(lbl_803A9768 + 0x278);
    dstState = (MenuCBPokemonState*)(lbl_803A9768 + slot * 0x138 + 8);
    srcState = (MenuCBPokemonState*)src;
    *dstState = *srcState;
}
