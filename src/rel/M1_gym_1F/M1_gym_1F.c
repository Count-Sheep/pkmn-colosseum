/* REL 24: M1_gym_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1B8;
extern u32* lbl_8047A1B4;
extern void* lbl_804796DC;
extern u32* lbl_804796D8;
extern void* lbl_804796D4;
extern void* lbl_80479CC4;
extern u32* lbl_80479CC0;
extern void* lbl_80479CBC;

u32 lbl_24_data_0[10] = {
    0x00000000, 0x00000000, 0x01C91002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_24_data_28[8] = {
    0x00B40000, 0x00000000, 0x00000000, 0x42AC0000, 0x00000000, 0x00000000, 0x41000000, 0xC1500000,
};

u32 lbl_24_data_48[90] = {
    0xF0800000, 0x00000006, 0x00000000, 0x00000001, 0x00000000, 0x01000007, 0xC1600000, 0x00000000,
    0x424C0000, 0xB0400000, 0x00B40005, 0x00000000, 0x00000001, 0x00000000, 0x01000008, 0xC2A20000,
    0x41000000, 0x41600000, 0xB0400000, 0xFFA60008, 0x00000000, 0x00000001, 0x00000000, 0x01000009,
    0xC2880000, 0x41000000, 0x40000000, 0xB0400000, 0xFFA60007, 0x00000000, 0x00000001, 0x00000000,
    0x0100000A, 0xC2880000, 0x41000000, 0xC1000000, 0xF0000000, 0x00B4001B, 0x00000000, 0x00000001,
    0x00000000, 0x0100000B, 0xC2B40000, 0x41000000, 0xC1F00000, 0xD0500000, 0xFFA60028, 0x000E0000,
    0x00000001, 0x0100000D, 0x0100000C, 0x4295199A, 0x41000000, 0x00000000, 0x70000000, 0x00000007,
    0x00060000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xC2140000, 0x70000000,
    0x00000008, 0x00070000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xC2140000,
    0x70000000, 0x00000009, 0x00080000, 0x00000001, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xC2140000, 0x70000000, 0x0000000A, 0x00090000, 0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0xC2140000,
};

u32 lbl_24_data_1B0 = 0x00000001;

u32 lbl_24_data_1B4 = 0x00000002;

FieldTable lbl_24_data_1B8 = {&lbl_24_data_1B4, lbl_24_data_28};

u32 lbl_24_data_1C0 = 0x0000000A;

FieldTable lbl_24_data_1C4 = {&lbl_24_data_1C0, lbl_24_data_48};

void _prolog(void)
{
    lbl_8047A1B8 = lbl_24_data_0;
    lbl_8047A1B4 = &lbl_24_data_1B0;
    lbl_804796DC = lbl_24_data_28;
    lbl_804796D8 = &lbl_24_data_1B4;
    lbl_804796D4 = &lbl_24_data_1B8;
    lbl_80479CC4 = lbl_24_data_48;
    lbl_80479CC0 = &lbl_24_data_1C0;
    lbl_80479CBC = &lbl_24_data_1C4;
}

void _epilog(void)
{
    lbl_8047A1B8 = 0;
    lbl_8047A1B4 = 0;
    lbl_804796DC = 0;
    lbl_804796D8 = 0;
    lbl_804796D4 = 0;
    lbl_80479CC4 = 0;
    lbl_80479CC0 = 0;
    lbl_80479CBC = 0;
}

void _unresolved(void)
{
}
