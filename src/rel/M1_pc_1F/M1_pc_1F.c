/* REL 16: M1_pc_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1F8;
extern u32* lbl_8047A1F4;
extern void* lbl_8047973C;
extern u32* lbl_80479738;
extern void* lbl_80479734;
extern void* lbl_80479D24;
extern u32* lbl_80479D20;
extern void* lbl_80479D1C;

u32 lbl_16_data_0[10] = {
    0x00000000, 0x00000000, 0x00651001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_16_data_28[16] = {
    0x00B40000, 0xC1100000, 0x00000000, 0x42500000, 0x00000000, 0xC2400000, 0x00000000, 0xC1B00000,
    0x00B40000, 0x41F27AE1, 0x00000000, 0xC22CE148, 0x00B40000, 0xC12EB852, 0x00000000, 0xC1B3AE14,
};

u32 lbl_16_data_68[36] = {
    0xF2800000, 0x0000000A, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0x40800000, 0x00000000,
    0x41C00000, 0xF0900000, 0x00000030, 0x00000000, 0x00000001, 0x00000000, 0x01000006, 0xC1F00000,
    0x00000000, 0xC1400000, 0xB0400000, 0xFFA6000F, 0x00000000, 0x00000001, 0x01000009, 0x01000007,
    0x422C0000, 0x00000000, 0xC1400000, 0xF0800000, 0x00000027, 0x00000000, 0x00000001, 0x00000000,
    0x01000008, 0xC1200000, 0x00000000, 0xC22C0000,
};

u32 lbl_16_data_F8 = 0x00000001;

u32 lbl_16_data_FC = 0x00000004;

FieldTable lbl_16_data_100 = {&lbl_16_data_FC, lbl_16_data_28};

u32 lbl_16_data_108 = 0x00000004;

FieldTable lbl_16_data_10C = {&lbl_16_data_108, lbl_16_data_68};

void _prolog(void)
{
    lbl_8047A1F8 = lbl_16_data_0;
    lbl_8047A1F4 = &lbl_16_data_F8;
    lbl_8047973C = lbl_16_data_28;
    lbl_80479738 = &lbl_16_data_FC;
    lbl_80479734 = &lbl_16_data_100;
    lbl_80479D24 = lbl_16_data_68;
    lbl_80479D20 = &lbl_16_data_108;
    lbl_80479D1C = &lbl_16_data_10C;
}

void _epilog(void)
{
    lbl_8047A1F8 = 0;
    lbl_8047A1F4 = 0;
    lbl_8047973C = 0;
    lbl_80479738 = 0;
    lbl_80479734 = 0;
    lbl_80479D24 = 0;
    lbl_80479D20 = 0;
    lbl_80479D1C = 0;
}

void _unresolved(void)
{
}
