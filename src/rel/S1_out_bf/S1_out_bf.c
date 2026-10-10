/* REL 129: S1_out_bf.fsys (floor 1 field variant) table publication lifecycle. */
#include "dolphin/types.h"

extern void* lbl_80479E60;
extern u32* lbl_80479E5C;

u32 lbl_129_data_0[10] = {
    0x00000000, 0x0DA81602, 0x0DA81001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_129_data_28 = 1;

void _prolog(void)
{
    lbl_80479E60 = lbl_129_data_0;
    lbl_80479E5C = &lbl_129_data_28;
}

void _epilog(void)
{
    lbl_80479E60 = 0;
    lbl_80479E5C = 0;
}

void _unresolved(void)
{
}
