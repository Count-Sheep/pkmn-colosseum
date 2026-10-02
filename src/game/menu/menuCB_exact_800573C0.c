/** Exact menuCB state helpers, 0x800573C0 - 0x800574A8. */
#include "dolphin/types.h"

extern u8 lbl_803A9768[];
extern f32 lbl_8047A588;
extern f32 lbl_8047BF00;
extern f32 lbl_8047BF04;

u32 fn_800573C0(void)
{
    s32 state;

    if (*(f32*)(lbl_803A9768 + 0x288) <= lbl_8047BF00) {
        state = *(s32*)lbl_803A9768;
        if (state == 0 || state == 3) {
            return 0;
        }
    }
    return 1;
}

void fn_80057400(void)
{
    s32 value;

    value = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1);
    *(u32*)(lbl_803A9768 + 0x278) = value;
    if (value > 1) {
        *(u32*)(lbl_803A9768 + 0x278) = 0;
    }
}

u32 fn_80057428(void)
{
    return !(lbl_8047A588 >= lbl_8047BF04);
}

void fn_8005744C(void)
{
    lbl_8047A588 = lbl_8047BF00;
}

typedef struct MenuCBPokemonState {
    u32 data[78];
} MenuCBPokemonState;

void fn_80057458(u8* src)
{
    s32 next = (s32)(*(u32*)(lbl_803A9768 + 0x278) + 1) % 2;
    MenuCBPokemonState* dstState =
        (MenuCBPokemonState*)(lbl_803A9768 + next * 0x138 + 8);
    MenuCBPokemonState* srcState = (MenuCBPokemonState*)src;

    *dstState = *srcState;
}
