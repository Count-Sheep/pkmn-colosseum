/* REL 20: M1_houseB_1F.fsys field tables and their publication lifecycle.
 * Generated from the retail module layout; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as the other floor RELs.
 */
#include "dolphin/types.h"

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

extern void* lbl_8047A1D8;
extern u32* lbl_8047A1D4;
extern void* lbl_8047901C;
extern u32* lbl_80479018;
extern void* lbl_80479014;
extern void* lbl_8047970C;
extern u32* lbl_80479708;
extern void* lbl_80479704;
extern void* lbl_80479CF4;
extern u32* lbl_80479CF0;
extern void* lbl_80479CEC;

u32 lbl_20_data_0[10] = {
    0x00000000, 0x00000000, 0x006D1001, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000,
};

u32 lbl_20_data_28[96] = {
    0x01000100, 0x00000000, 0x006D1002, 0x0D561000, 0x00000000, 0x0D571800, 0x01000100, 0x00000001,
    0x006D1002, 0x108C1000, 0x00000000, 0x108B1800, 0x01000100, 0x00000002, 0x006D1002, 0x108C1000,
    0x00000001, 0x108B1800, 0x01000100, 0x00000003, 0x006D1002, 0x108E1000, 0x00000000, 0x108D1800,
    0x01000100, 0x00000004, 0x006D1002, 0x108E1000, 0x00000001, 0x108D1800, 0x01000100, 0x00000005,
    0x006D1002, 0x105B1000, 0x00000000, 0x105A1800, 0x01000100, 0x00000006, 0x006D1002, 0x105B1000,
    0x00000001, 0x105A1800, 0x01000100, 0x00000007, 0x006D1002, 0x105B1000, 0x00000002, 0x105A1800,
    0x01000100, 0x00000008, 0x006D1002, 0x105B1000, 0x00000003, 0x105A1800, 0x01000100, 0x00000009,
    0x006D1002, 0x105B1000, 0x00000004, 0x105A1800, 0x01000100, 0x0000000A, 0x006D1002, 0x105B1000,
    0x00000005, 0x105A1800, 0x01000100, 0x0000000B, 0x006D1002, 0x11191000, 0x00000000, 0x11181800,
    0x01000100, 0x0000000C, 0x006D1002, 0x11191000, 0x00000001, 0x11181800, 0x01000100, 0x0000000D,
    0x006D1002, 0x11191000, 0x00000002, 0x11181800, 0x01000100, 0x0000000E, 0x006D1002, 0x11191000,
    0x00000003, 0x11181800, 0x01000100, 0x0000000F, 0x006D1002, 0x11191000, 0x00000004, 0x11181800,
};

u32 lbl_20_data_1A8[4] = {
    0x00B40000, 0xC2200000, 0x00000000, 0x41200000,
};

u32 lbl_20_data_1B8[18] = {
    0xB0400000, 0x0000000F, 0x00000000, 0x00000001, 0x01000007, 0x01000006, 0x40400000, 0x00000000,
    0xC05AE148, 0xF0800000, 0x00B4000D, 0x00000000, 0x00000001, 0x00000000, 0x01000005, 0xC1E80000,
    0x00000000, 0xC0C00000,
};

u32 lbl_20_data_200 = 0x00000001;

u32 lbl_20_data_204 = 0x00000010;

FieldTable lbl_20_data_208 = {&lbl_20_data_204, lbl_20_data_28};

u32 lbl_20_data_210 = 0x00000001;

FieldTable lbl_20_data_214 = {&lbl_20_data_210, lbl_20_data_1A8};

u32 lbl_20_data_21C = 0x00000002;

FieldTable lbl_20_data_220 = {&lbl_20_data_21C, lbl_20_data_1B8};

void _prolog(void)
{
    lbl_8047A1D8 = lbl_20_data_0;
    lbl_8047A1D4 = &lbl_20_data_200;
    lbl_8047901C = lbl_20_data_28;
    lbl_80479018 = &lbl_20_data_204;
    lbl_80479014 = &lbl_20_data_208;
    lbl_8047970C = lbl_20_data_1A8;
    lbl_80479708 = &lbl_20_data_210;
    lbl_80479704 = &lbl_20_data_214;
    lbl_80479CF4 = lbl_20_data_1B8;
    lbl_80479CF0 = &lbl_20_data_21C;
    lbl_80479CEC = &lbl_20_data_220;
}

void _epilog(void)
{
    lbl_8047A1D8 = 0;
    lbl_8047A1D4 = 0;
    lbl_8047901C = 0;
    lbl_80479018 = 0;
    lbl_80479014 = 0;
    lbl_8047970C = 0;
    lbl_80479708 = 0;
    lbl_80479704 = 0;
    lbl_80479CF4 = 0;
    lbl_80479CF0 = 0;
    lbl_80479CEC = 0;
}

void _unresolved(void)
{
}
