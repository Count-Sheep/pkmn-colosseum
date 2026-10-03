/* Name-entry controller and button callback, 0x800280FC-0x800281F4.
 * RULE-EXCEPTION(user-approved): retained volatile state/fade reads;
 * see docs/RULE_EXCEPTIONS.md.
 */
#include "dolphin/types.h"

extern void winSeqSetMenu(void*, s32);
extern f32 lbl_8047B930;
extern f32 lbl_8047B950;
extern f32 lbl_8047B934;

s32 menuNameEntryCtrl(void* r3) {
    u8* r30;
    u8* r31;
    f32* fptr;
    f32 f0;
    f32 f1;
    f32 f2;
    s32 state;
    s32 flag;
    u8 one;

    r30 = (u8*)r3;
    state = (s32)(s8)*(volatile u8*)(r30 + 1);
    r31 = *(u8**)(r30 + 0x60);
    switch (state) {
    case 0:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x56);
            one = 1;
            **(f32**)(r31 + 0x30) = lbl_8047B930;
            r30[2] = one;
        }
        break;
    case 2:
        fptr = *(f32**)(r31 + 0x30);
        f0 = lbl_8047B950;
        f2 = *(volatile f32*)fptr;
        f1 = lbl_8047B934;
        f0 = f2 + f0;
        *fptr = f0;
        if (f0 >= f1) {
            fptr = *(f32**)(r31 + 0x30);
            *fptr = *fptr - f1;
        }
        break;
    case 3:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            winSeqSetMenu(*(void**)(r30 + 4), 0x5a);
            r30[2] = 1;
        }
        break;
    }
    return 0;
}

void menuNameEntryButton(void) { }
