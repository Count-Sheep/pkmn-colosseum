#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork;

extern PdaSceneWork lbl_803A6818;
extern u16* lbl_8047A4E4;
extern u16 lbl_8047A4E8;
extern u16 memoDataGetPokemonID(u16* data, u32 index);
extern u16 memoDataGetCount(u16* data);
extern void fn_8003E394(void);

s32 fn_8003D818(void)
{
    u8* work;
    struct {
        u16 count;
        u32 index;
    } memo;

    work = (u8*)&lbl_803A6818;
    memo.count = 0;
    memo.index = memo.count;
    while ((u16)memo.index < memoDataGetCount(0)) {
        lbl_8047A4E4[(u16)memo.index] = memoDataGetPokemonID(0, memo.index);
        memo.count++;
        memo.index++;
    }
    lbl_8047A4E8 = memo.count;
    work[0x158] = 0;
    work[0x159] = 0;
    work[0x15b] = 0;
    work[0x15a] = 0;
    work[0x15c] = 0;
    fn_8003E394();
    return lbl_803A6818.field_10 = lbl_8047A4E8;
}
