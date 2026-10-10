/* REL 26: M1_stadium_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1A8;
extern u32* lbl_8047A1A4;
extern void* lbl_804796C4;
extern u32* lbl_804796C0;
extern void* lbl_804796BC;
extern void* lbl_80479CAC;
extern u32* lbl_80479CA8;
extern void* lbl_80479CA4;
extern void* lbl_80479154;
extern u32* lbl_80479150;
extern void* lbl_8047914C;

u32 lbl_26_data_0[10] = {
    0x00000000, 0x00000000, 0x01CE1003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_26_data_28[12] = {
    0x00B40000, 0x00000000, 0x00000000, 0x43280000, 0x00000000, 0x4297A3D7, 0x00000000, 0xC1711EB8,
    0x00000000, 0xC297A3D7, 0x00000000, 0xC1711EB8,
};

u32 lbl_26_data_58[27] = {
    0xF0800000, 0x00000008, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0xC2180000, 0x00000000,
    0x42980000, 0xF0800000, 0x00000007, 0x00000000, 0x00000001, 0x00000000, 0x01000006, 0x428E0000,
    0x00000000, 0x41B80000, 0xF0900000, 0x00000036, 0x00000000, 0x00000001, 0x00000000, 0x01000007,
    0x00000000, 0x00000000, 0x00000000,
};

u32 lbl_26_data_C4[18] = {
    0x00000000, 0x00000000, 0x43280000, 0x430075C3, 0x42791EB8, 0x00000000, 0xC294199A, 0x00000000,
    0xC1BCA3D7, 0x42B60000, 0x42DC0000, 0x00000000, 0x42982E14, 0x00000000, 0xC1AC28F6, 0x42B60000,
    0x42DC0000, 0x00000000,
};

u32 lbl_26_data_10C = 0x00000001;

u32 lbl_26_data_110 = 0x00000003;

FieldTable lbl_26_data_114 = {&lbl_26_data_110, lbl_26_data_28};

u32 lbl_26_data_11C = 0x00000003;

FieldTable lbl_26_data_120 = {&lbl_26_data_11C, lbl_26_data_58};

u32 lbl_26_data_128 = 0x00000003;

FieldTable lbl_26_data_12C = {&lbl_26_data_128, lbl_26_data_C4};

void _prolog(void)
{
    lbl_8047A1A8 = lbl_26_data_0;
    lbl_8047A1A4 = &lbl_26_data_10C;
    lbl_804796C4 = lbl_26_data_28;
    lbl_804796C0 = &lbl_26_data_110;
    lbl_804796BC = &lbl_26_data_114;
    lbl_80479CAC = lbl_26_data_58;
    lbl_80479CA8 = &lbl_26_data_11C;
    lbl_80479CA4 = &lbl_26_data_120;
    lbl_80479154 = lbl_26_data_C4;
    lbl_80479150 = &lbl_26_data_128;
    lbl_8047914C = &lbl_26_data_12C;
}

void _epilog(void)
{
    lbl_8047A1A8 = 0;
    lbl_8047A1A4 = 0;
    lbl_804796C4 = 0;
    lbl_804796C0 = 0;
    lbl_804796BC = 0;
    lbl_80479CAC = 0;
    lbl_80479CA8 = 0;
    lbl_80479CA4 = 0;
    lbl_80479154 = 0;
    lbl_80479150 = 0;
    lbl_8047914C = 0;
}

void _unresolved(void)
{
}
