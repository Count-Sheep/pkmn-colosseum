/**
 * GSgfx frame setup, 0x800D3410 - 0x800D361C.
 * This text-only carve keeps the recovered body from gs_gfx.c in its own
 * translation unit so only its exact function is linked.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "game/gs_gfx.h"
#include "game/gs_model.h"

extern u8 lbl_804001F0[];
extern u8 lbl_8047AA90;
extern f32 lbl_8047CA00;
extern f32 lbl_8047CA08;
extern f32 lbl_8047CA04;

extern void GSgfxBackFBDoFrame(void);
extern void fn_801BF8A0(s32 a);
extern void fn_801E16F0(void);
extern void fn_801BF6AC(void);
extern void GStextureConvertFromHW(void* a, s32 b);
extern void fn_800B8E74(void);
extern void GXInvalidateTexAll(void);
extern void fn_800D13C4(void* a);
extern void fn_800DC874(void* a);
extern void fn_801183EC(void* a);
extern void modelShadowRender__FP10GSgfxLayer(void);
extern void fn_8019C708(s32 a);
extern void HSD_SetEraseColor(u8 a, u8 b, u8 c, u8 d);
extern void HSD_EraseRect(s32 a, s32 b, s32 c, f32 d, f32 e, f32 f, f32 g, f32 h);
extern u32 OSGetTick(void);

void fn_800D3410(void* arg0, u8 arg1) {
    u32* state;
    u32 sc;
    u8 r30;
    u32 startTick;
    u32 tick;

    state = (u32*)lbl_8047AA80;
    sc = state[0xC / 4];

    if ((u32)(sc + 0x01020000U) == 0xFEFEU) {
        state[0xC / 4] = (u32)arg0;
        r30 = 0;

        {
            u32* s4 = (u32*)lbl_8047AA80;
            sc = s4[0xC / 4];
            if ((u32)(sc + 0x01020000U) != 0xFEFEU) {
                if (((u8*)s4)[0x49D] == 0) {
                    GSgfxBackFBDoFrame();
                    fn_801BF8A0(0);
                    fn_801E16F0();
                    fn_801BF6AC();
                } else {
                    if (sc != 0) {
                        GStextureConvertFromHW((void*)sc, 1);
                        r30 = 0;
                    }
                }
                ((u8*)lbl_8047AA80)[0x49D] = 1;
                if (r30 != 0) {
                    fn_800B8E74();
                    if (*(u32*)((u8*)lbl_8047AA80 + 0xC) != 0) {
                        GXInvalidateTexAll();
                    }
                }
            }
        }

        lbl_8047AA90 = arg1;

        if (arg1 != 0) {
            startTick = OSGetTick();
            fn_800D13C4((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
            tick = OSGetTick();
            ((u32*)&lbl_804001F0)[0x2C / 4] = tick - startTick;

            startTick = OSGetTick();
            fn_800DC874((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
            tick = OSGetTick();
            ((u32*)&lbl_804001F0)[0x30 / 4] = tick - startTick;

            startTick = OSGetTick();
            fn_800E3928((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
            tick = OSGetTick();
            ((u32*)&lbl_804001F0)[0x34 / 4] = tick - startTick;

            startTick = OSGetTick();
            fn_801183EC((void*)*(u32*)((u8*)lbl_8047AA80 + 0x54));
            tick = OSGetTick();
            ((u32*)&lbl_804001F0)[0x38 / 4] = tick - startTick;
        }

        if (arg0 == 0) {
            startTick = OSGetTick();
            modelShadowRender__FP10GSgfxLayer();
            tick = OSGetTick();
            ((u32*)&lbl_804001F0)[0x3C / 4] = tick - startTick;

            fn_8019C708(0);

            if (((u8*)lbl_8047AA80)[0x19] != 0) {
                HSD_SetEraseColor(
                    ((u8*)lbl_8047AA80)[0x1C],
                    ((u8*)lbl_8047AA80)[0x1D],
                    ((u8*)lbl_8047AA80)[0x1E],
                    ((u8*)lbl_8047AA80)[0x1F]
                );
                HSD_EraseRect(1, 1, 0,
                    lbl_8047CA00,
                    lbl_8047CA08,
                    lbl_8047CA00,
                    lbl_8047CA04,
                    lbl_8047CA00);
            }

            ((u8*)lbl_8047AA80)[0x49D] = 0;
        } else {
            fn_8019C708(3);
        }
    }
}
