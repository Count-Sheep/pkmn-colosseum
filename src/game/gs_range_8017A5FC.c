/**
 * @file gs_range_8017A5FC.c
 * @brief fsys read callbacks, 0x8017A5FC - 0x8017AAA4 (4 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01); the range name stays honest until internal TU
 * structure is proven.
 *
 * Optimisation-level-0 code like the rest of fsys (peephole and scheduling
 * on): each carve builds with the unit-wide "-opt level=0" and no local
 * pragmas (see configure.py).
 */
#include "dolphin/types.h"

#include "game/fsys/fsys.h"

/* Address: 0x8017A5FC | size: 0x28 */
extern FSYSManager lbl_80453FEC;
void fn_8017A5FC(void);
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A5FC_PREFIX)
void fn_8017A5FC(void)
{
    FSYSSlot* slot;

    slot = lbl_80453FEC.activeSlot;
    slot->status = 0x12f;
}
#endif

/* Address: 0x8017A624 | size: 0x1F0 */
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A624_MIDDLE)
extern s32 fn_801808B4(void* req);
extern void* fn_801807A8(void* src, void* dst, u32 size);
extern u8 fn_80167E98(void* work, void* addr, s32 length, s32 offset,
                       void* callback);
extern void* lbl_8047B1C0[2]; /* DVD staging buffers */

s32 fn_8017A624(FSYSSlot* slot)
{
    FSYSArchiveHeader* archive;
    void* dvdBuf;

    archive = slot->archiveData;
    if (slot->dmaAsyncRequest) {
        while (fn_801808B4(slot->dmaAsyncRequest)) {
        }
    }
    if (slot->dmaCopyDst) {
        slot->dmaAsyncRequest = fn_801807A8(lbl_8047B1C0[lbl_80453FEC.field_24],
                                            slot->dmaCopyDst, slot->dmaChunkSize);
    } else {
        slot->dmaAsyncRequest = 0;
    }
    slot->dmaBytesRemaining -= 0x20000;
    if (slot->dmaBytesRemaining <= 0) {
        if (slot->fileInfo0 && slot->callbackA) {
            ((void (*)(s32, u32, u32))slot->callbackA)(slot->loadMode, slot->callbackB,
                                                       slot->callbackC);
        }
        return 1;
    }
    lbl_80453FEC.field_24 ^= 1;
    dvdBuf = lbl_8047B1C0[lbl_80453FEC.field_24];
    if (slot->dmaBytesRemaining < 0x20000) {
        slot->dmaChunkSize = (slot->dmaBytesRemaining + 0x1F) & ~0x1F;
    } else {
        slot->dmaChunkSize = 0x20000;
    }
    slot->dmaSrcOffset += 0x20000;
    slot->dmaDstOffset += 0x20000;
    if (slot->dmaCopyDst) {
        slot->dmaCopyDst = (u8*)slot->dmaCopyDst + 0x20000;
    }
    slot->status = 0x12E;
    if (archive->flags & 1) {
        if (!slot->tocBuffer) {
            fn_80167E98((void*)slot->fileInfo0, dvdBuf, slot->dmaChunkSize,
                        slot->dmaDstOffset, fn_8017A5FC);
        } else {
            fn_80167E98(slot->tocBuffer, dvdBuf, slot->dmaChunkSize,
                        slot->dmaDstOffset, fn_8017A5FC);
        }
    } else {
        fn_80167E98((void*)slot->fileInfo0, dvdBuf, slot->dmaChunkSize,
                    slot->dmaDstOffset, fn_8017A5FC);
    }
    return 0;
}
#endif

/* Address: 0x8017A814 | size: 0x148 */
#if !defined(GS_RANGE_8017A5FC_SPLIT) || defined(GS_RANGE_8017A814_SUFFIX)
extern u32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(u32 level);
extern void fn_80167E64(void* file);

/*
 * fn_8017A814 and fn_8017A95C are the ARQ completion callbacks that
 * fn_8017B5C0 and fn_8017BD34 pass to fn_80180584 (userData = the slot).
 * Both advance the active slot's status past its read step and close the
 * slot's external file. Not yet exact: retail copies userData into r30 and
 * never reads it, and its first temporary after OSDisableInterrupts lands
 * in r5 where this source gets r4.
 *
 * R38 lane (2026-09-28): the same body is expanded a third time in
 * fn_8017C414 after its fn_8017A624 call, and that copy also builds the
 * lbl_80453FEC address in r5, so the r5 comes from the shared helper, not
 * from the (result, userData) signature. No tested form (the body as an
 * inline taking the callback argument, the slot or nothing; a helper
 * returning the active slot; an FSYSManager pointer; `request` read before
 * or after OSDisableInterrupts; GC/1.3 and GC/2.0; -opt level=0/1/2, -O0,
 * nopeephole) gets r5. `(void)request;` is also register-only evidence
 * (it only keeps the r30 load), so this stays a candidate either way.
 */
void fn_8017A814(s32 result, void* userData)
{
    FSYSSlot* slot;
    FSYSSlot* request;
    u32 enabled;

    request = (FSYSSlot*)userData;
    (void)request;
    enabled = OSDisableInterrupts();
    slot = lbl_80453FEC.activeSlot;

    switch (slot->status) {
    case 1:
        slot->status = 2;
        break;
    case 3:
        slot->status = 4;
        break;
    case 0x65:
        slot->status = 0x96;
        break;
    case 0xC8:
        slot->status = 0xC9;
        break;
    case 0x12F:
        slot->status = 0x130;
        break;
    case 0x190:
        slot->status = 0x191;
        break;
    case 2:
    case 4:
    case 0x64:
    case 0xC9:
    case 0x12D:
        break;
    default:
        slot->status = 0x3E8;
        slot->archiveHandle = 1;
        break;
    }

    if (slot->tocBuffer) {
        fn_80167E64(slot->tocBuffer);
        slot->tocBuffer = NULL;
    }
    OSRestoreInterrupts(enabled);
}

/* Address: 0x8017A95C | size: 0x148 */
void fn_8017A95C(s32 result, void* userData)
{
    FSYSSlot* slot;
    FSYSSlot* request;
    u32 enabled;

    request = (FSYSSlot*)userData;
    (void)request;
    enabled = OSDisableInterrupts();
    slot = lbl_80453FEC.activeSlot;

    switch (slot->status) {
    case 1:
        slot->status = 2;
        break;
    case 3:
        slot->status = 4;
        break;
    case 0x65:
        slot->status = 0x96;
        break;
    case 0xC8:
        slot->status = 0xC9;
        break;
    case 0x12F:
        slot->status = 0x130;
        break;
    case 0x190:
        slot->status = 0x191;
        break;
    case 2:
    case 4:
    case 0x64:
    case 0xC9:
    case 0x12D:
        break;
    default:
        slot->status = 0x3E8;
        slot->archiveHandle = 1;
        break;
    }

    if (slot->tocBuffer) {
        fn_80167E64(slot->tocBuffer);
        slot->tocBuffer = NULL;
    }
    OSRestoreInterrupts(enabled);
}
#endif
