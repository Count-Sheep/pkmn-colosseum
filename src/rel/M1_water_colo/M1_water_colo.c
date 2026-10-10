/* REL 157: M1_water_colo.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_80479D88;
extern u32* lbl_80479D84;
extern void* lbl_804791CC;
extern u32* lbl_804791C8;
extern void* lbl_804791C4;

u32 lbl_157_data_0[10] = {
    0x00000000, 0x008C1605, 0x008C1002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_157_data_28[4] = {
    0x00B40000, 0x00000000, 0x00000000, 0x00000000,
};

u32 lbl_157_data_38 = 0x00000001;

u32 lbl_157_data_3C = 0x00000001;

FieldTable lbl_157_data_40 = {&lbl_157_data_3C, lbl_157_data_28};

void _prolog(void)
{
    lbl_80479D88 = lbl_157_data_0;
    lbl_80479D84 = &lbl_157_data_38;
    lbl_804791CC = lbl_157_data_28;
    lbl_804791C8 = &lbl_157_data_3C;
    lbl_804791C4 = &lbl_157_data_40;
}

void _epilog(void)
{
    lbl_80479D88 = 0;
    lbl_80479D84 = 0;
    lbl_804791CC = 0;
    lbl_804791C8 = 0;
    lbl_804791C4 = 0;
}

void _unresolved(void)
{
}
