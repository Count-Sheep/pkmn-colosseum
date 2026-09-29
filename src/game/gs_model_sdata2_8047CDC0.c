#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

typedef struct Sdata2PaddedU16 {
    u16 value;
    u16 pad;
} Sdata2PaddedU16;

/* 0x8047CD80-0x8047CDC0 is the literal pool of gapp_exact_80101B90.c. */
SDATA2 const f32 lbl_8047CDC0 = 0.0f;
SDATA2 const f32 lbl_8047CDC4 = 50.0f;
/* 0x8047CDC0-0x8047CDC8 are the menu TU pool's first entries (read by
 * menu_r50_80102014_prefix.c); 0x8047CDC8-0x8047CDE0 is the rest of that
 * pool, owned by menu_exact_80103614.c. */
