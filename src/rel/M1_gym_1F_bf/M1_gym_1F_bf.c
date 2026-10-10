/* REL 133: M1_gym_1F_bf.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

extern void* lbl_80479E48;
extern u32* lbl_80479E44;

u32 lbl_133_data_0[10] = {
    0x00000000, 0x0D911604, 0x0D911001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_133_data_28 = 0x00000001;

void _prolog(void)
{
    lbl_80479E48 = lbl_133_data_0;
    lbl_80479E44 = &lbl_133_data_28;
}

void _epilog(void)
{
    lbl_80479E48 = 0;
    lbl_80479E44 = 0;
}

void _unresolved(void)
{
}
