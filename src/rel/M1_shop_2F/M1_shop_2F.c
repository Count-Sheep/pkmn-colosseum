/* REL 23: M1_shop_2F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1C0;
extern u32* lbl_8047A1BC;
extern void* lbl_804796E8;
extern u32* lbl_804796E4;
extern void* lbl_804796E0;
extern void* lbl_80479CD0;
extern u32* lbl_80479CCC;
extern void* lbl_80479CC8;

u32 lbl_23_data_0[10] = {
    0x00000000, 0x00000000, 0x00731001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_23_data_28[4] = {
    0x00B40000, 0x00000000, 0x00000000, 0xC1800000,
};

u32 lbl_23_data_38[27] = {
    0xF2800000, 0x00000009, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0x41880000, 0x00000000,
    0xC1A80000, 0xB0400000, 0xFFA6000B, 0x00000000, 0x00000001, 0x01000008, 0x01000006, 0x41600000,
    0x00000000, 0x42240000, 0xF0800000, 0x005A0037, 0x00000000, 0x00000001, 0x00000000, 0x01000007,
    0xC20C0000, 0x00000000, 0x41A00000,
};

u32 lbl_23_data_A4 = 0x00000001;

u32 lbl_23_data_A8 = 0x00000001;

FieldTable lbl_23_data_AC = {&lbl_23_data_A8, lbl_23_data_28};

u32 lbl_23_data_B4 = 0x00000003;

FieldTable lbl_23_data_B8 = {&lbl_23_data_B4, lbl_23_data_38};

void _prolog(void)
{
    lbl_8047A1C0 = lbl_23_data_0;
    lbl_8047A1BC = &lbl_23_data_A4;
    lbl_804796E8 = lbl_23_data_28;
    lbl_804796E4 = &lbl_23_data_A8;
    lbl_804796E0 = &lbl_23_data_AC;
    lbl_80479CD0 = lbl_23_data_38;
    lbl_80479CCC = &lbl_23_data_B4;
    lbl_80479CC8 = &lbl_23_data_B8;
}

void _epilog(void)
{
    lbl_8047A1C0 = 0;
    lbl_8047A1BC = 0;
    lbl_804796E8 = 0;
    lbl_804796E4 = 0;
    lbl_804796E0 = 0;
    lbl_80479CD0 = 0;
    lbl_80479CCC = 0;
    lbl_80479CC8 = 0;
}

void _unresolved(void)
{
}
