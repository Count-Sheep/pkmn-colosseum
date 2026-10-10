/* REL 19: M1_houseA_2F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1E0;
extern u32* lbl_8047A1DC;
extern void* lbl_80479028;
extern u32* lbl_80479024;
extern void* lbl_80479020;
extern void* lbl_80479718;
extern u32* lbl_80479714;
extern void* lbl_80479710;
extern void* lbl_80479D00;
extern u32* lbl_80479CFC;
extern void* lbl_80479CF8;

u32 lbl_19_data_0[10] = {
    0x00000000, 0x00000000, 0x006B1001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_19_data_28[96] = {
    0x01000100, 0x00000000, 0x006B1002, 0x0D561000, 0x00000000, 0x0D571800, 0x01000100, 0x00000001,
    0x006B1002, 0x108C1000, 0x00000000, 0x108B1800, 0x01000100, 0x00000002, 0x006B1002, 0x108C1000,
    0x00000001, 0x108B1800, 0x01000100, 0x00000003, 0x006B1002, 0x108E1000, 0x00000000, 0x108D1800,
    0x01000100, 0x00000004, 0x006B1002, 0x108E1000, 0x00000001, 0x108D1800, 0x01000100, 0x00000005,
    0x006B1002, 0x105B1000, 0x00000000, 0x105A1800, 0x01000100, 0x00000006, 0x006B1002, 0x105B1000,
    0x00000001, 0x105A1800, 0x01000100, 0x00000007, 0x006B1002, 0x105B1000, 0x00000002, 0x105A1800,
    0x01000100, 0x00000008, 0x006B1002, 0x105B1000, 0x00000003, 0x105A1800, 0x01000100, 0x00000009,
    0x006B1002, 0x105B1000, 0x00000004, 0x105A1800, 0x01000100, 0x0000000A, 0x006B1002, 0x105B1000,
    0x00000005, 0x105A1800, 0x01000100, 0x0000000B, 0x006B1002, 0x11191000, 0x00000000, 0x11181800,
    0x01000100, 0x0000000C, 0x006B1002, 0x11191000, 0x00000001, 0x11181800, 0x01000100, 0x0000000D,
    0x006B1002, 0x11191000, 0x00000002, 0x11181800, 0x01000100, 0x0000000E, 0x006B1002, 0x11191000,
    0x00000003, 0x11181800, 0x01000100, 0x0000000F, 0x006B1002, 0x11191000, 0x00000004, 0x11181800,
};

u32 lbl_19_data_1A8[8] = {
    0x005A0000, 0x41B2E148, 0x00000000, 0xC1F851EC, 0x00B40000, 0xC17B5C29, 0x00000000, 0xC05147AE,
};

u32 lbl_19_data_1C8[9] = {
    0xF0800000, 0x00000019, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0x00000000, 0x00000000,
    0xC1A40000,
};

u32 lbl_19_data_1EC = 0x00000001;

u32 lbl_19_data_1F0 = 0x00000010;

FieldTable lbl_19_data_1F4 = {&lbl_19_data_1F0, lbl_19_data_28};

u32 lbl_19_data_1FC = 0x00000002;

FieldTable lbl_19_data_200 = {&lbl_19_data_1FC, lbl_19_data_1A8};

u32 lbl_19_data_208 = 0x00000001;

FieldTable lbl_19_data_20C = {&lbl_19_data_208, lbl_19_data_1C8};

void _prolog(void)
{
    lbl_8047A1E0 = lbl_19_data_0;
    lbl_8047A1DC = &lbl_19_data_1EC;
    lbl_80479028 = lbl_19_data_28;
    lbl_80479024 = &lbl_19_data_1F0;
    lbl_80479020 = &lbl_19_data_1F4;
    lbl_80479718 = lbl_19_data_1A8;
    lbl_80479714 = &lbl_19_data_1FC;
    lbl_80479710 = &lbl_19_data_200;
    lbl_80479D00 = lbl_19_data_1C8;
    lbl_80479CFC = &lbl_19_data_208;
    lbl_80479CF8 = &lbl_19_data_20C;
}

void _epilog(void)
{
    lbl_8047A1E0 = 0;
    lbl_8047A1DC = 0;
    lbl_80479028 = 0;
    lbl_80479024 = 0;
    lbl_80479020 = 0;
    lbl_80479718 = 0;
    lbl_80479714 = 0;
    lbl_80479710 = 0;
    lbl_80479D00 = 0;
    lbl_80479CFC = 0;
    lbl_80479CF8 = 0;
}

void _unresolved(void)
{
}
