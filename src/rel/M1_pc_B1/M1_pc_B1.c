/* REL 17: M1_pc_B1.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1F0;
extern u32* lbl_8047A1EC;
extern void* lbl_80479730;
extern u32* lbl_8047972C;
extern void* lbl_80479728;
extern void* lbl_80479D18;
extern u32* lbl_80479D14;
extern void* lbl_80479D10;
extern void* lbl_80479160;
extern u32* lbl_8047915C;
extern void* lbl_80479158;

u32 lbl_17_data_0[10] = {
    0x00000000, 0x00000000, 0x00671001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_17_data_28[12] = {
    0x00B40000, 0xC22C0000, 0x00000000, 0xC1200000, 0x00B40000, 0xBE19999A, 0x00000000, 0x40051EB8,
    0x00B40000, 0xC1B3999A, 0x00000000, 0xC157AE14,
};

u32 lbl_17_data_58[9] = {
    0xF0900000, 0x00000036, 0x00000000, 0x00000001, 0x00000000, 0x01000007, 0x00000000, 0x00000000,
    0xC1680000,
};

u32 lbl_17_data_7C[6] = {
    0xBF88F5C3, 0x00000000, 0x40D3851F, 0x42836148, 0x42A175C3, 0x00000000,
};

u32 lbl_17_data_94 = 0x00000001;

u32 lbl_17_data_98 = 0x00000003;

FieldTable lbl_17_data_9C = {&lbl_17_data_98, lbl_17_data_28};

u32 lbl_17_data_A4 = 0x00000001;

FieldTable lbl_17_data_A8 = {&lbl_17_data_A4, lbl_17_data_58};

u32 lbl_17_data_B0 = 0x00000001;

FieldTable lbl_17_data_B4 = {&lbl_17_data_B0, lbl_17_data_7C};

void _prolog(void)
{
    lbl_8047A1F0 = lbl_17_data_0;
    lbl_8047A1EC = &lbl_17_data_94;
    lbl_80479730 = lbl_17_data_28;
    lbl_8047972C = &lbl_17_data_98;
    lbl_80479728 = &lbl_17_data_9C;
    lbl_80479D18 = lbl_17_data_58;
    lbl_80479D14 = &lbl_17_data_A4;
    lbl_80479D10 = &lbl_17_data_A8;
    lbl_80479160 = lbl_17_data_7C;
    lbl_8047915C = &lbl_17_data_B0;
    lbl_80479158 = &lbl_17_data_B4;
}

void _epilog(void)
{
    lbl_8047A1F0 = 0;
    lbl_8047A1EC = 0;
    lbl_80479730 = 0;
    lbl_8047972C = 0;
    lbl_80479728 = 0;
    lbl_80479D18 = 0;
    lbl_80479D14 = 0;
    lbl_80479D10 = 0;
    lbl_80479160 = 0;
    lbl_8047915C = 0;
    lbl_80479158 = 0;
}

void _unresolved(void)
{
}
