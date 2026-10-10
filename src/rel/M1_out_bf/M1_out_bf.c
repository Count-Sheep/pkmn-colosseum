/* REL 132: M1_out_bf.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

extern void* lbl_80479E50;
extern u32* lbl_80479E4C;

u32 lbl_132_data_0[10] = {
    0x00000000, 0x0D901602, 0x0D901001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_132_data_28 = 0x00000001;

void _prolog(void)
{
    lbl_80479E50 = lbl_132_data_0;
    lbl_80479E4C = &lbl_132_data_28;
}

void _epilog(void)
{
    lbl_80479E50 = 0;
    lbl_80479E4C = 0;
}

void _unresolved(void)
{
}
