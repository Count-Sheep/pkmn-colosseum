#include "dolphin/types.h"

#pragma section ".data"

extern u8 lbl_8036CD88[];
extern void* jumptable_8036CD94[];
extern void* lbl_8036CFA8[];

extern u8 fn_801AEFF0[];
extern u8 fn_801B2654[];
extern u8 fn_801B26F8[];
extern u8 fn_801B2718[];
extern u8 fn_801B3168[];
extern u8 fn_801B3174[];
extern u8 fn_801B31A4[];
extern u8 fn_801B31F4[];

/* .data unit 0x8036CFA8..0x8036CFE8: the state invalidate table (state.c's
 * channel set-ups before it, 0x8036CE88, are owned by hsd/state.c). Originally
 * auto-carved 0x8036CD88..0x8036CFE8. Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

void* lbl_8036CFA8[16] = {
    (void*)0x00000001,
    (void*)((u8*)fn_801B2718),
    (void*)0x00000002,
    (void*)((u8*)fn_801B26F8),
    (void*)0x00000004,
    (void*)((u8*)fn_801B31F4),
    (void*)0x00000008,
    (void*)((u8*)fn_801B31A4),
    (void*)0x00000010,
    (void*)((u8*)fn_801B3174),
    (void*)0x00000020,
    (void*)((u8*)fn_801B3168),
    (void*)0x00000040,
    (void*)((u8*)fn_801B2654),
    (void*)0x00000000,
    (void*)0x00000000,
};
