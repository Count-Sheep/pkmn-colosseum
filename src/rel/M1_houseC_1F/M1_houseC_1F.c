/* REL 21: M1_houseC_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1D0;
extern u32* lbl_8047A1CC;
extern void* lbl_80479700;
extern u32* lbl_804796FC;
extern void* lbl_804796F8;
extern void* lbl_80479CE8;
extern u32* lbl_80479CE4;
extern void* lbl_80479CE0;
extern void* lbl_80479040;
extern u32* lbl_8047903C;
extern void* lbl_80479038;

u32 lbl_21_data_0[10] = {
    0x00000000, 0x00000000, 0x006F1001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_21_data_28[4] = {
    0x00B40000, 0xC1000000, 0x00000000, 0x41F00000,
};

u32 lbl_21_data_38[18] = {
    0xB0400000, 0xFF9C0018, 0x00000000, 0x00000001, 0x01000007, 0x01000005, 0x41E40000, 0x41000000,
    0xC1940000, 0xF0800000, 0x00B40019, 0x00000000, 0x00000001, 0x00000000, 0x01000006, 0xC1880000,
    0x00000000, 0xC1980000,
};

u32 lbl_21_data_80[96] = {
    0x01000100, 0x00000000, 0x006F1002, 0x0D561000, 0x00000000, 0x0D571800, 0x01000100, 0x00000001,
    0x006F1002, 0x108C1000, 0x00000000, 0x108B1800, 0x01000100, 0x00000002, 0x006F1002, 0x108C1000,
    0x00000001, 0x108B1800, 0x01000100, 0x00000003, 0x006F1002, 0x108E1000, 0x00000000, 0x108D1800,
    0x01000100, 0x00000004, 0x006F1002, 0x108E1000, 0x00000001, 0x108D1800, 0x01000100, 0x00000005,
    0x006F1002, 0x105B1000, 0x00000000, 0x105A1800, 0x01000100, 0x00000006, 0x006F1002, 0x105B1000,
    0x00000001, 0x105A1800, 0x01000100, 0x00000007, 0x006F1002, 0x105B1000, 0x00000002, 0x105A1800,
    0x01000100, 0x00000008, 0x006F1002, 0x105B1000, 0x00000003, 0x105A1800, 0x01000100, 0x00000009,
    0x006F1002, 0x105B1000, 0x00000004, 0x105A1800, 0x01000100, 0x0000000A, 0x006F1002, 0x105B1000,
    0x00000005, 0x105A1800, 0x01000100, 0x0000000B, 0x006F1002, 0x11191000, 0x00000000, 0x11181800,
    0x01000100, 0x0000000C, 0x006F1002, 0x11191000, 0x00000001, 0x11181800, 0x01000100, 0x0000000D,
    0x006F1002, 0x11191000, 0x00000002, 0x11181800, 0x01000100, 0x0000000E, 0x006F1002, 0x11191000,
    0x00000003, 0x11181800, 0x01000100, 0x0000000F, 0x006F1002, 0x11191000, 0x00000004, 0x11181800,
};

u32 lbl_21_data_200 = 0x00000001;

u32 lbl_21_data_204 = 0x00000001;

FieldTable lbl_21_data_208 = {&lbl_21_data_204, lbl_21_data_28};

u32 lbl_21_data_210 = 0x00000002;

FieldTable lbl_21_data_214 = {&lbl_21_data_210, lbl_21_data_38};

u32 lbl_21_data_21C = 0x00000010;

FieldTable lbl_21_data_220 = {&lbl_21_data_21C, lbl_21_data_80};

void _prolog(void)
{
    lbl_8047A1D0 = lbl_21_data_0;
    lbl_8047A1CC = &lbl_21_data_200;
    lbl_80479700 = lbl_21_data_28;
    lbl_804796FC = &lbl_21_data_204;
    lbl_804796F8 = &lbl_21_data_208;
    lbl_80479CE8 = lbl_21_data_38;
    lbl_80479CE4 = &lbl_21_data_210;
    lbl_80479CE0 = &lbl_21_data_214;
    lbl_80479040 = lbl_21_data_80;
    lbl_8047903C = &lbl_21_data_21C;
    lbl_80479038 = &lbl_21_data_220;
}

void _epilog(void)
{
    lbl_8047A1D0 = 0;
    lbl_8047A1CC = 0;
    lbl_80479700 = 0;
    lbl_804796FC = 0;
    lbl_804796F8 = 0;
    lbl_80479CE8 = 0;
    lbl_80479CE4 = 0;
    lbl_80479CE0 = 0;
    lbl_80479040 = 0;
    lbl_8047903C = 0;
    lbl_80479038 = 0;
}

void _unresolved(void)
{
}
