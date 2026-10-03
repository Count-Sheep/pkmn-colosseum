/* REL 131: s1_out.fsys field tables and their publication lifecycle.
 * Record shapes follow the retail strides; unnamed words retain their bits.
 * Built with the same SN ProDG -O0 -G0 pipeline as common_rel and mail.
 */
#include "dolphin/types.h"

extern void* lbl_8047916C;
extern u32* lbl_80479168;
extern void* lbl_80479164;
extern void* lbl_80479D48;
extern u32* lbl_80479D44;
extern void* lbl_80479D40;
extern void* lbl_80479760;
extern u32* lbl_8047975C;
extern void* lbl_80479758;
extern void* lbl_8047A258;
extern u32* lbl_8047A254;

typedef struct FieldPersonRecord {
    u32 words[6];
    f32 position[3];
} FieldPersonRecord;

typedef struct FieldPointRecord {
    u32 key;
    f32 position[3];
} FieldPointRecord;

typedef struct FieldTable {
    u32* count;
    void* entries;
} FieldTable;

f32 lbl_131_data_0[2][6] = {
    {174.14f, 0.0f, 58.28f, 110.81f, 24.98f, 0.46f},
    {-94.29f, 0.0f, 50.11f, 110.81f, 24.98f, -0.25f},
};

FieldPersonRecord lbl_131_data_30[7] = {
    {{0x10000000, 0x4E, 0x00020000, 1, 0, 0}, {85.0f, 0.0f, -12.0f}},
    {{0x10000000, 0x4D, 0x00030000, 1, 0, 0}, {-36.0f, 0.0f, 23.0f}},
    {{0x10000000, 0x010E001C, 0, 1, 0x0100000E, 0x0100000F}, {-20.0f, 0.37f, 48.0f}},
    {{0x70800000, 0x58, 0x00010000, 1, 0x0100000C, 0x0100000D}, {32.3f, 0.0f, 6.7f}},
    {{0x70800000, 0x1E, 0x025D0000, 1, 0, 0x01000011}, {56.47f, 0.0f, 10.64f}},
    {{0x70800000, 1, 0x025D0000, 1, 0, 0x01000012}, {58.17f, 0.0f, 10.64f}},
    {{0x70800000, 0x00B4004B, 0x02590000, 1, 0, 0x01000013}, {57.7f, 0.0f, 47.9f}},
};

FieldPointRecord lbl_131_data_12C[5] = {
    {0x00B40000, {50.0f, 0.0f, 70.0f}},
    {0, {-13.0f, 10.83f, -24.48f}},
    {0x005A0000, {-141.96f, 0.0f, 66.17f}},
    {0x010E0000, {244.36f, 0.0f, 65.79f}},
    {0x00B40000, {0.0f, 0.0f, 230.0f}},
};

u32 lbl_131_data_17C[1][10] = {
    {0, 0, 0x00E21002, 0x00E21003, 0, 0, 0, 0, 0, 0},
};

u32 lbl_131_data_1A4 = 2;
FieldTable lbl_131_data_1A8 = {&lbl_131_data_1A4, lbl_131_data_0};
u32 lbl_131_data_1B0 = 7;
FieldTable lbl_131_data_1B4 = {&lbl_131_data_1B0, lbl_131_data_30};
u32 lbl_131_data_1BC = 5;
FieldTable lbl_131_data_1C0 = {&lbl_131_data_1BC, lbl_131_data_12C};
u32 lbl_131_data_1C8 = 1;

void _prolog(void)
{
    lbl_8047916C = lbl_131_data_0;
    lbl_80479168 = &lbl_131_data_1A4;
    lbl_80479164 = &lbl_131_data_1A8;
    lbl_80479D48 = lbl_131_data_30;
    lbl_80479D44 = &lbl_131_data_1B0;
    lbl_80479D40 = &lbl_131_data_1B4;
    lbl_80479760 = lbl_131_data_12C;
    lbl_8047975C = &lbl_131_data_1BC;
    lbl_80479758 = &lbl_131_data_1C0;
    lbl_8047A258 = lbl_131_data_17C;
    lbl_8047A254 = &lbl_131_data_1C8;
}

void _epilog(void)
{
    lbl_8047916C = 0;
    lbl_80479168 = 0;
    lbl_80479164 = 0;
    lbl_80479D48 = 0;
    lbl_80479D44 = 0;
    lbl_80479D40 = 0;
    lbl_80479760 = 0;
    lbl_8047975C = 0;
    lbl_80479758 = 0;
    lbl_8047A258 = 0;
    lbl_8047A254 = 0;
}

void _unresolved(void)
{
}
