/* REL 22: M1_shop_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1C8;
extern u32* lbl_8047A1C4;
extern void* lbl_804796F4;
extern u32* lbl_804796F0;
extern void* lbl_804796EC;
extern void* lbl_80479CDC;
extern u32* lbl_80479CD8;
extern void* lbl_80479CD4;

u32 lbl_22_data_0[10] = {
    0x00000000, 0x00000000, 0x00711001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_22_data_28[8] = {
    0x00B40000, 0x00000000, 0x00000000, 0x42800000, 0x00000000, 0x00000000, 0x00000000, 0x41C80000,
};

u32 lbl_22_data_48[36] = {
    0xF0900000, 0xFFA60031, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0xC1B00000, 0x00000000,
    0x41600000, 0xF0900000, 0xFFA60037, 0x00000000, 0x00000001, 0x00000000, 0x01000006, 0x42080000,
    0x00000000, 0x41400000, 0xF0000000, 0x00560030, 0x00000000, 0x00000001, 0x00000000, 0x01000007,
    0x42140000, 0x00000000, 0xC0E00000, 0xF2800000, 0x0000000D, 0x00000000, 0x00000001, 0x00000000,
    0x01000008, 0xC1800000, 0x00000000, 0xC2480000,
};

u32 lbl_22_data_D8 = 0x00000001;

u32 lbl_22_data_DC = 0x00000002;

FieldTable lbl_22_data_E0 = {&lbl_22_data_DC, lbl_22_data_28};

u32 lbl_22_data_E8 = 0x00000004;

FieldTable lbl_22_data_EC = {&lbl_22_data_E8, lbl_22_data_48};

void _prolog(void)
{
    lbl_8047A1C8 = lbl_22_data_0;
    lbl_8047A1C4 = &lbl_22_data_D8;
    lbl_804796F4 = lbl_22_data_28;
    lbl_804796F0 = &lbl_22_data_DC;
    lbl_804796EC = &lbl_22_data_E0;
    lbl_80479CDC = lbl_22_data_48;
    lbl_80479CD8 = &lbl_22_data_E8;
    lbl_80479CD4 = &lbl_22_data_EC;
}

void _epilog(void)
{
    lbl_8047A1C8 = 0;
    lbl_8047A1C4 = 0;
    lbl_804796F4 = 0;
    lbl_804796F0 = 0;
    lbl_804796EC = 0;
    lbl_80479CDC = 0;
    lbl_80479CD8 = 0;
    lbl_80479CD4 = 0;
}

void _unresolved(void)
{
}
