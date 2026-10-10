/* REL 25: M1_gym_B1.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1B0;
extern u32* lbl_8047A1AC;
extern void* lbl_804796D0;
extern u32* lbl_804796CC;
extern void* lbl_804796C8;
extern void* lbl_80479CB8;
extern u32* lbl_80479CB4;
extern void* lbl_80479CB0;

u32 lbl_25_data_0[10] = {
    0x00000000, 0x00000000, 0x01CB1002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_25_data_28[4] = {
    0x00000000, 0x00000000, 0x41A00000, 0xC2580000,
};

u32 lbl_25_data_38[72] = {
    0xF0800000, 0x0000000C, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0x41600000, 0x00000000,
    0xC1700000, 0xF0000000, 0x00000008, 0x00000000, 0x00000001, 0x00000000, 0x01000006, 0xC1880000,
    0x00000000, 0x42000000, 0xF0000000, 0x005A0053, 0x00000000, 0x00000001, 0x00000000, 0x0100000A,
    0xC1800000, 0x00000000, 0x4223851F, 0xF0000000, 0xFFA60007, 0x00000000, 0x00000001, 0x00000000,
    0x01000007, 0x42D40000, 0x00000000, 0x42500000, 0xF0800000, 0x005A0057, 0x00000000, 0x00000001,
    0x00000000, 0x0100000B, 0x42B40000, 0x00000000, 0x42500000, 0xF2800000, 0x00000009, 0x00000000,
    0x00000001, 0x00000000, 0x01000008, 0x42C40000, 0x00000000, 0xC2280000, 0xF0000000, 0x005A0006,
    0x00000000, 0x00000001, 0x00000000, 0x01000009, 0xC2980000, 0x00000000, 0xC20C0000, 0xF0800000,
    0xFFA60054, 0x00000000, 0x00000001, 0x00000000, 0x0100000C, 0xC2833333, 0x00000000, 0xC20C0000,
};

u32 lbl_25_data_158 = 0x00000001;

u32 lbl_25_data_15C = 0x00000001;

FieldTable lbl_25_data_160 = {&lbl_25_data_15C, lbl_25_data_28};

u32 lbl_25_data_168 = 0x00000008;

FieldTable lbl_25_data_16C = {&lbl_25_data_168, lbl_25_data_38};

void _prolog(void)
{
    lbl_8047A1B0 = lbl_25_data_0;
    lbl_8047A1AC = &lbl_25_data_158;
    lbl_804796D0 = lbl_25_data_28;
    lbl_804796CC = &lbl_25_data_15C;
    lbl_804796C8 = &lbl_25_data_160;
    lbl_80479CB8 = lbl_25_data_38;
    lbl_80479CB4 = &lbl_25_data_168;
    lbl_80479CB0 = &lbl_25_data_16C;
}

void _epilog(void)
{
    lbl_8047A1B0 = 0;
    lbl_8047A1AC = 0;
    lbl_804796D0 = 0;
    lbl_804796CC = 0;
    lbl_804796C8 = 0;
    lbl_80479CB8 = 0;
    lbl_80479CB4 = 0;
    lbl_80479CB0 = 0;
}

void _unresolved(void)
{
}
