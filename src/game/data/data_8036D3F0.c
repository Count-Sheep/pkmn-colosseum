#include "dolphin/types.h"

#pragma section ".data"

extern void* lbl_8036D3F0[];
extern u8 lbl_8036D43C[];
extern u8 lbl_8036D46C[];
extern u8 lbl_8036D48C[];
extern u8 lbl_8036D510[];
extern u8 lbl_8036D594[];
extern void* jumptable_8036D5A4[];
extern void* jumptable_8036D5D0[];
extern void* jumptable_8036D5F0[];
extern void* jumptable_8036D610[];
extern void* jumptable_8036D630[];
extern void* jumptable_8036D650[];
extern void* jumptable_8036D670[];
extern void* jumptable_8036D690[];
extern void* jumptable_8036D6B0[];
extern void* jumptable_8036D6D0[];
extern void* jumptable_8036D6F0[];
extern void* jumptable_8036D714[];
extern void* jumptable_8036D734[];
extern void* jumptable_8036D798[];
extern void* jumptable_8036D7C0[];
extern void* jumptable_8036D7F0[];
extern void* jumptable_8036D820[];
extern void* jumptable_8036D850[];
extern void* jumptable_8036D880[];
extern void* jumptable_8036D8B0[];
extern void* jumptable_8036D8E0[];
extern void* jumptable_8036D910[];
extern void* jumptable_8036D940[];
extern void* jumptable_8036D974[];
extern void* jumptable_8036D9A4[];
extern void* jumptable_8036D9D4[];
extern void* jumptable_8036DA04[];
extern void* jumptable_8036DA34[];
extern void* jumptable_8036DA64[];
extern void* jumptable_8036DA94[];
extern void* jumptable_8036DAC4[];
extern void* jumptable_8036DAF4[];
extern void* jumptable_8036DB24[];
extern void* jumptable_8036DB54[];
extern void* jumptable_8036DB84[];
extern void* jumptable_8036DBB4[];
extern void* jumptable_8036DBE4[];
extern void* jumptable_8036DC14[];
extern void* jumptable_8036DC44[];
extern void* jumptable_8036DC74[];

extern u8 CObjForeachAnim[];
extern u8 DObjForeachAnim[];
extern u8 HSD_ForeachAnim[];
extern u8 HSD_Index2PosNrmMtx[];
extern u8 HSD_Index2TexMtx[];
extern u8 HSD_TObjAssignResources[];
extern u8 HSD_TObjSetup[];
extern u8 JObjForeachAnim[];
extern u8 LObjForeachAnim[];
extern u8 MakeColorGenTExp[];
extern u8 TObjInfoInit[];
extern u8 fn_801BAC8C[];
extern u8 fn_801BDA58[];
extern u8 fn_801BE85C[];

/* Auto-carved .data unit 0x8036D3F0..0x8036DCA4 (46 objects). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* lbl_8036D3F0[19] = {
    (void*)((u8*)TObjInfoInit),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
};

u8 lbl_8036D43C[48] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_8036D46C[32] = {
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x09,
};

u8 lbl_8036D48C[132] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
};

u8 lbl_8036D510[132] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x08,
    0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x07,
    0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
};

u8 lbl_8036D594[16] = {
    0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

void* jumptable_8036D5A4[11] = {
    (void*)((u8*)HSD_Index2TexMtx + 0x2C),
    (void*)((u8*)HSD_Index2TexMtx + 0x34),
    (void*)((u8*)HSD_Index2TexMtx + 0x3C),
    (void*)((u8*)HSD_Index2TexMtx + 0x44),
    (void*)((u8*)HSD_Index2TexMtx + 0x4C),
    (void*)((u8*)HSD_Index2TexMtx + 0x54),
    (void*)((u8*)HSD_Index2TexMtx + 0x5C),
    (void*)((u8*)HSD_Index2TexMtx + 0x64),
    (void*)((u8*)HSD_Index2TexMtx + 0x6C),
    (void*)((u8*)HSD_Index2TexMtx + 0x74),
    (void*)((u8*)HSD_Index2TexMtx + 0x7C),
};

void* jumptable_8036D5D0[8] = {
    (void*)((u8*)HSD_TObjSetup + 0x5C),
    (void*)((u8*)HSD_TObjSetup + 0x64),
    (void*)((u8*)HSD_TObjSetup + 0x6C),
    (void*)((u8*)HSD_TObjSetup + 0x74),
    (void*)((u8*)HSD_TObjSetup + 0x7C),
    (void*)((u8*)HSD_TObjSetup + 0x84),
    (void*)((u8*)HSD_TObjSetup + 0x8C),
    (void*)((u8*)HSD_TObjSetup + 0x94),
};

void* jumptable_8036D5F0[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x510),
    (void*)((u8*)HSD_TObjAssignResources + 0x518),
    (void*)((u8*)HSD_TObjAssignResources + 0x520),
    (void*)((u8*)HSD_TObjAssignResources + 0x528),
    (void*)((u8*)HSD_TObjAssignResources + 0x530),
    (void*)((u8*)HSD_TObjAssignResources + 0x538),
    (void*)((u8*)HSD_TObjAssignResources + 0x540),
    (void*)((u8*)HSD_TObjAssignResources + 0x548),
};

void* jumptable_8036D610[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x490),
    (void*)((u8*)HSD_TObjAssignResources + 0x498),
    (void*)((u8*)HSD_TObjAssignResources + 0x4A0),
    (void*)((u8*)HSD_TObjAssignResources + 0x4A8),
    (void*)((u8*)HSD_TObjAssignResources + 0x4B0),
    (void*)((u8*)HSD_TObjAssignResources + 0x4B8),
    (void*)((u8*)HSD_TObjAssignResources + 0x4C0),
    (void*)((u8*)HSD_TObjAssignResources + 0x4C8),
};

void* jumptable_8036D630[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x408),
    (void*)((u8*)HSD_TObjAssignResources + 0x410),
    (void*)((u8*)HSD_TObjAssignResources + 0x418),
    (void*)((u8*)HSD_TObjAssignResources + 0x420),
    (void*)((u8*)HSD_TObjAssignResources + 0x428),
    (void*)((u8*)HSD_TObjAssignResources + 0x430),
    (void*)((u8*)HSD_TObjAssignResources + 0x438),
    (void*)((u8*)HSD_TObjAssignResources + 0x440),
};

void* jumptable_8036D650[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x388),
    (void*)((u8*)HSD_TObjAssignResources + 0x390),
    (void*)((u8*)HSD_TObjAssignResources + 0x398),
    (void*)((u8*)HSD_TObjAssignResources + 0x3A0),
    (void*)((u8*)HSD_TObjAssignResources + 0x3A8),
    (void*)((u8*)HSD_TObjAssignResources + 0x3B0),
    (void*)((u8*)HSD_TObjAssignResources + 0x3B8),
    (void*)((u8*)HSD_TObjAssignResources + 0x3C0),
};

void* jumptable_8036D670[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x2F4),
    (void*)((u8*)HSD_TObjAssignResources + 0x2FC),
    (void*)((u8*)HSD_TObjAssignResources + 0x304),
    (void*)((u8*)HSD_TObjAssignResources + 0x30C),
    (void*)((u8*)HSD_TObjAssignResources + 0x314),
    (void*)((u8*)HSD_TObjAssignResources + 0x31C),
    (void*)((u8*)HSD_TObjAssignResources + 0x324),
    (void*)((u8*)HSD_TObjAssignResources + 0x32C),
};

void* jumptable_8036D690[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x22C),
    (void*)((u8*)HSD_TObjAssignResources + 0x234),
    (void*)((u8*)HSD_TObjAssignResources + 0x23C),
    (void*)((u8*)HSD_TObjAssignResources + 0x244),
    (void*)((u8*)HSD_TObjAssignResources + 0x24C),
    (void*)((u8*)HSD_TObjAssignResources + 0x254),
    (void*)((u8*)HSD_TObjAssignResources + 0x25C),
    (void*)((u8*)HSD_TObjAssignResources + 0x264),
};

void* jumptable_8036D6B0[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x180),
    (void*)((u8*)HSD_TObjAssignResources + 0x188),
    (void*)((u8*)HSD_TObjAssignResources + 0x190),
    (void*)((u8*)HSD_TObjAssignResources + 0x198),
    (void*)((u8*)HSD_TObjAssignResources + 0x1A0),
    (void*)((u8*)HSD_TObjAssignResources + 0x1A8),
    (void*)((u8*)HSD_TObjAssignResources + 0x1B0),
    (void*)((u8*)HSD_TObjAssignResources + 0x1B8),
};

void* jumptable_8036D6D0[8] = {
    (void*)((u8*)HSD_TObjAssignResources + 0x104),
    (void*)((u8*)HSD_TObjAssignResources + 0x10C),
    (void*)((u8*)HSD_TObjAssignResources + 0x114),
    (void*)((u8*)HSD_TObjAssignResources + 0x11C),
    (void*)((u8*)HSD_TObjAssignResources + 0x124),
    (void*)((u8*)HSD_TObjAssignResources + 0x12C),
    (void*)((u8*)HSD_TObjAssignResources + 0x134),
    (void*)((u8*)HSD_TObjAssignResources + 0x13C),
};

void* jumptable_8036D6F0[9] = {
    (void*)((u8*)MakeColorGenTExp + 0x84),
    (void*)((u8*)MakeColorGenTExp + 0x8C),
    (void*)((u8*)MakeColorGenTExp + 0x94),
    (void*)((u8*)MakeColorGenTExp + 0x9C),
    (void*)((u8*)MakeColorGenTExp + 0xA4),
    (void*)((u8*)MakeColorGenTExp + 0xAC),
    (void*)((u8*)MakeColorGenTExp + 0xB8),
    (void*)((u8*)MakeColorGenTExp + 0xC0),
    (void*)((u8*)MakeColorGenTExp + 0xCC),
};

void* jumptable_8036D714[8] = {
    (void*)((u8*)fn_801BDA58 + 0x16C),
    (void*)((u8*)fn_801BDA58 + 0x174),
    (void*)((u8*)fn_801BDA58 + 0x17C),
    (void*)((u8*)fn_801BDA58 + 0x184),
    (void*)((u8*)fn_801BDA58 + 0x18C),
    (void*)((u8*)fn_801BDA58 + 0x194),
    (void*)((u8*)fn_801BDA58 + 0x19C),
    (void*)((u8*)fn_801BDA58 + 0x1A4),
};

void* jumptable_8036D734[25] = {
    (void*)((u8*)fn_801BE85C + 0x5F4),
    (void*)((u8*)fn_801BE85C + 0x40),
    (void*)((u8*)fn_801BE85C + 0x118),
    (void*)((u8*)fn_801BE85C + 0x124),
    (void*)((u8*)fn_801BE85C + 0x130),
    (void*)((u8*)fn_801BE85C + 0x13C),
    (void*)((u8*)fn_801BE85C + 0xF4),
    (void*)((u8*)fn_801BE85C + 0x100),
    (void*)((u8*)fn_801BE85C + 0x10C),
    (void*)((u8*)fn_801BE85C + 0xBC),
    (void*)((u8*)fn_801BE85C + 0x98),
    (void*)((u8*)fn_801BE85C + 0x154),
    (void*)((u8*)fn_801BE85C + 0x170),
    (void*)((u8*)fn_801BE85C + 0x1CC),
    (void*)((u8*)fn_801BE85C + 0x228),
    (void*)((u8*)fn_801BE85C + 0x284),
    (void*)((u8*)fn_801BE85C + 0x2E0),
    (void*)((u8*)fn_801BE85C + 0x33C),
    (void*)((u8*)fn_801BE85C + 0x398),
    (void*)((u8*)fn_801BE85C + 0x3F4),
    (void*)((u8*)fn_801BE85C + 0x450),
    (void*)((u8*)fn_801BE85C + 0x4AC),
    (void*)((u8*)fn_801BE85C + 0x508),
    (void*)((u8*)fn_801BE85C + 0x564),
    (void*)((u8*)fn_801BE85C + 0x5C0),
};
