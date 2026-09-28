/**
 * @file gs_gfx_exact_800D3190.c
 * @brief fn_800D3190, 0x800D3190 - 0x800D3410.
 *
 * Function-boundary carve of the GSgfx TU (see gs_gfx.c): finish a frame.
 * Waits for the GX draw-done flag (lbl_8047AA91, giving up after three timer
 * ticks' worth of bus clock), then resolves the frame's render target the
 * way fn_800D30F0(1) does, or rolls the frame statistics block over when
 * there is none, and, when profiling is on (lbl_8047AA90), times the
 * per-frame update passes into the statistics block.
 *
 * No jump table, no pooled constant; its data is the .bss statistics block
 * (lbl_804001F0) and the .sbss state pointer and flags (lbl_8047AA80,
 * lbl_8047AA90, lbl_8047AA91), kept extern. GC/1.3 -O4,p like the TU, no
 * pragmas.
 */
#include "dolphin/types.h"
#include "game/gs_gfx.h"

extern u32 lbl_804001F0[]; /* frame statistics block */
extern u8 lbl_8047AA90;    /* profiling enabled */
extern u8 lbl_8047AA91;    /* GX draw done */

extern void GXFlush(void);
extern void GXSetDrawDone(void);
extern void OSYieldThread(void);
extern u32 OSGetTick(void);
extern void fn_8019C6FC(void);
extern void GSgfxBackFBDoFrame(void);
extern void fn_801BF8A0(s32 value);
extern void fn_801E16F0(void);
extern void fn_801BF6AC(void);
extern void GStextureConvertFromHW(void* texture, s32 mode);
extern void fn_800B8E74(void);
extern void GXInvalidateTexAll(void);
extern void fn_800D1070(u32 frames);
extern void fn_800DC6D8(u32 frames);
extern void fn_800E3884(u32 frames, s32 mode);
extern void fn_801181B0(s32 frames);

void fn_800D3190(void)
{
    u32 start;
    u32 end;
    void* renderTarget;
    u8 flag;
    s32 count;

    if (lbl_8047AA80->renderTarget == (void*)0xFEFEFEFEU) {
        return;
    }

    lbl_8047AA91 = 0;
    GXFlush();
    GXSetDrawDone();
    start = OSGetTick();

    while (lbl_8047AA91 == 0) {
        OSYieldThread();
        if (lbl_8047AA91 == 0) {
            end = OSGetTick();
            if ((end - start) / (*(u32*)0x800000F8 / 4) > 3) {
                lbl_8047AA91 = 1;
            }
        }
    }

    fn_8019C6FC();

    renderTarget = lbl_8047AA80->renderTarget;
    if (renderTarget != NULL) {
        flag = 1;
        if (renderTarget != (void*)0xFEFEFEFEU) {
            if (lbl_8047AA80->progressiveFlag == 0) {
                GSgfxBackFBDoFrame();
                fn_801BF8A0(0);
                fn_801E16F0();
                fn_801BF6AC();
            } else if (renderTarget != NULL) {
                GStextureConvertFromHW(renderTarget, 1);
                flag = 0;
            }
            lbl_8047AA80->progressiveFlag = 1;
            if (flag != 0) {
                fn_800B8E74();
                if (lbl_8047AA80->renderTarget != NULL) {
                    GXInvalidateTexAll();
                }
            }
        }
    } else {
        lbl_804001F0[0] = lbl_804001F0[1];
        lbl_804001F0[2] = lbl_804001F0[3];
        lbl_804001F0[4] += 1;
        lbl_804001F0[1] = 0;
        lbl_804001F0[3] = 0;
    }

    lbl_8047AA80->renderTarget = (void*)0xFEFEFEFEU;

    if (lbl_8047AA90 == 0) {
        return;
    }

    start = OSGetTick();
    fn_800D1070(lbl_8047AA80->frameDelta);
    end = OSGetTick();
    lbl_804001F0[11] += end - start;

    start = OSGetTick();
    fn_800DC6D8(lbl_8047AA80->frameDelta);
    end = OSGetTick();
    lbl_804001F0[12] += end - start;

    start = OSGetTick();
    fn_800E3884(lbl_8047AA80->frameDelta, 0);
    end = OSGetTick();
    lbl_804001F0[13] += end - start;

    count = lbl_8047AA80->frameDelta;
    lbl_804001F0[14] = 0;
    while (count-- != 0) {
        start = OSGetTick();
        fn_800E3884(1, 1);
        end = OSGetTick();
        lbl_804001F0[13] += end - start;

        start = OSGetTick();
        fn_801181B0(1);
        end = OSGetTick();
        lbl_804001F0[14] += end - start;
    }
}
