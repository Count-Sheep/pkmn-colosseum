/**
 * @file sdk_candidate_800B5C5C.c
 * @brief Dolphin SDK GXInit.c ("Feb  7 2003" build), 0x800B5C5C - 0x800B671C:
 *        __GXDefaultTexRegionCallback, __GXDefaultTlutRegionCallback,
 *        __GXShutdown and GXInit (the 0x800B5E8C wrapper measures GXInit).
 *
 * GXInit.c is built with the peephole pass off (see the linked
 * __GXInitGX unit, sdk_candidate_800B671C.c), so the bodies sit inside
 * '#pragma peephole off'.
 */
#include "dolphin/types.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

typedef struct GXTlutRegion {
    /* 0x00 */ u8 _00[0x10];
} GXTlutRegion;

typedef struct GXTexRegion {
    u32 unk[4];
} GXTexRegion;

typedef struct GXFifoObj {
    /* 0x00 */ u8* base;
    /* 0x04 */ u8* top;
    /* 0x08 */ u32 size;
    /* 0x0C */ u32 hiWatermark;
    /* 0x10 */ u32 loWatermark;
    /* 0x14 */ void* rdPtr;
    /* 0x18 */ void* wrPtr;
    /* 0x1C */ s32 count;
    /* 0x20 */ u8 wrap;
    /* 0x21 */ u8 _21[3];
} GXFifoObj;

typedef struct GXData {
    /* 0x000 */ u8 _000[0x08];
    /* 0x008 */ u32 cpEnable;
    /* 0x00C */ u32 cpStatus;
    /* 0x010 */ u8 _010[0x0C];
    /* 0x01C */ u32 vatA[8];
    /* 0x03C */ u32 vatB[8];
    /* 0x05C */ u8 _05C[0x20];
    /* 0x07C */ u32 lpSize;
    /* 0x080 */ u8 _080[0x38];
    /* 0x0B8 */ u32 suTs0[8];
    /* 0x0D8 */ u32 suTs1[8];
    /* 0x0F8 */ u32 suScis0;
    /* 0x0FC */ u32 suScis1;
    /* 0x100 */ u32 tref[8];
    /* 0x120 */ u32 iref;
    /* 0x124 */ u32 bpMask;
    /* 0x128 */ u8 _128[0x08];
    /* 0x130 */ u32 tevc[16];
    /* 0x170 */ u32 teva[16];
    /* 0x1B0 */ u32 tevKsel[8];
    /* 0x1D0 */ u32 cmode0;
    /* 0x1D4 */ u32 cmode1;
    /* 0x1D8 */ u32 zmode;
    /* 0x1DC */ u32 peCtrl;
    /* 0x1E0 */ u8 _1E0[0x1C];
    /* 0x1FC */ u32 cpTex;
    /* 0x200 */ u8 _200[0x04];
    /* 0x204 */ u32 genMode;
    /* 0x208 */ GXTexRegion defaultTexRegions[8];
    /* 0x288 */ GXTexRegion defaultTexRegionsCI[4];
    /* 0x2C8 */ u32 nextTexRgn;
    /* 0x2CC */ u32 nextTexRgnCI;
    /* 0x2D0 */ GXTlutRegion defaultTlutRegions[20];
    /* 0x410 */ u8 _410[0x8C];
    /* 0x49C */ u32 texmapId[16];
    /* 0x4DC */ u32 field_4DC;
    /* 0x4E0 */ u32 field_4E0;
    /* 0x4E4 */ u8 _4E4[0x08];
    /* 0x4EC */ u32 perfSel;
    /* 0x4F0 */ u8 inDispList;
    /* 0x4F1 */ u8 dlSaveContext;
    /* 0x4F2 */ u8 tcsManEnab;
    /* 0x4F3 */ u8 dirtyVAT;
    /* 0x4F4 */ u32 dirtyState;
} GXData;

/* .bss 0x803FC860: gxData, then FifoObj at +0x4F8 (one anchor). */
static GXData gxData;
static GXFifoObj FifoObj;
extern GXData* const gx;
extern u16* __memReg;
extern u16* __cpReg;
extern OSTime OSGetTime(void);
extern void GXSetBreakPtCallback(void* callback);
extern void fn_800B8FD8(void* callback);
extern void fn_800B90A4(void* callback);
extern void __GXAbort(void);
extern void __GXInitGX(void);

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
void* fn_800B5C5C(void* object) {
    s32 format;
    u32 count;
    extern u32 GXGetTexObjFmt(void* object);

    format = GXGetTexObjFmt(object);
    if (format != 8 && format != 9 && format != 10) {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2c8);
        *(u32*)(data + 0x2c8) = count + 1;
        return data + ((count & 7) << 4) + 0x208;
    } else {
        u8* data = (u8*)gx;
        count = *(u32*)(data + 0x2cc);
        *(u32*)(data + 0x2cc) = count + 1;
        return data + ((count & 3) << 4) + 0x288;
    }
}

GXTlutRegion* __GXDefaultTlutRegionCallback(u32 index) {
    GXTlutRegion* region;

    if (index >= 20) {
        region = NULL;
    } else {
        region = &gx->defaultTlutRegions[index];
    }
    return region;
}

#define GX_GET_MEM_REG(offset) (*(volatile u16*)((volatile u16*)(__memReg) + (offset)))
#define GX_SET_CP_REG(offset, val) (*(volatile u16*)((volatile u16*)(__cpReg) + (offset)) = val)
#define GX_WRITE_U32(val) (*(volatile u32*)0xCC008000 = (val))

static inline u32 GXReadMEMReg(u32 addrHi, u32 addrLo)
{
    u32 hiStart;
    u32 hiNew;
    u32 lo;

    hiStart = GX_GET_MEM_REG(addrHi);
    do {
        hiNew = hiStart;
        hiStart = GX_GET_MEM_REG(addrHi);
        lo = GX_GET_MEM_REG(addrLo);
    } while (hiStart != hiNew);

    return (hiStart << 16) | lo;
}

s32 __GXShutdown_800C6260(BOOL final)
{
    static u32 peCount;
    static OSTime time;
    static u32 calledOnce;
    u32 newPeCount;
    OSTime newTime;

    if (!final) {
        if (!calledOnce) {
            peCount = GXReadMEMReg(0x27, 0x28);
            time = OSGetTime();
            calledOnce = 1;
            return FALSE;
        }

        newTime = OSGetTime();
        newPeCount = GXReadMEMReg(0x27, 0x28);

        if (newTime - time < 10) {
            return FALSE;
        }

        if (newPeCount != peCount) {
            peCount = newPeCount;
            time = newTime;
            return FALSE;
        }
    } else {
        GXSetBreakPtCallback(NULL);
        fn_800B8FD8(NULL);
        fn_800B90A4(NULL);

        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);

        PPCSync();

        GX_SET_CP_REG(1, 0);
        GX_SET_CP_REG(2, 3);

        ((u8*)gx)[0x4F2] = 1;

        __GXAbort();
    }

    return TRUE;
}

#define GX_WRITE_U8(value)  (*(volatile u8*)0xCC008000 = (value))

#define SET_REG_FIELD(reg, size, shift, val) \
    do { \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) | ((u32)(val) << (shift)); \
    } while (0)
/* GX_SET_REG(reg, x, st, end): big-endian bit numbering, st..end */
#define GX_SET_REG(reg, x, st, end) SET_REG_FIELD(reg, (end) - (st) + 1, 31 - (end), x)
#define GXWGFifo_s8 (*(volatile u8*)0xCC008000)
#define GXWGFifo_s32 (*(volatile u32*)0xCC008000)
#define GX_BP_LOAD_REG(data) GXWGFifo_s8 = 0x61; GXWGFifo_s32 = (data);
#define GX_CP_LOAD_REG(addr, data) GXWGFifo_s8 = 0x08; GXWGFifo_s8 = (addr); GXWGFifo_s32 = (data);
#define GX_XF_LOAD_REG(addr, data) GXWGFifo_s8 = 0x10; GXWGFifo_s32 = (addr); GXWGFifo_s32 = (data);

static void EnableWriteGatherPipe(void)
{
    u32 hid2;
    hid2 = PPCMfhid2();
    PPCMtwpar(0x0C008000);
    hid2 |= 0x40000000;
    PPCMthid2(hid2);
}

void* GXInit(void* base, u32 size) {
    static u32 resetFuncRegistered;
    extern const char* __GXVersion;
    extern u8 GXResetFuncInfo_80312AD0[];
    extern u16* __peReg;
    extern u32* __piReg;
    extern void OSRegisterVersion();
    extern void OSRegisterResetFunction();
    extern void GXSetMisc();
    extern void __GXFifoInit();
    extern void GXInitFifoBase();
    extern void GXSetCPUFifo();
    extern void GXSetGPFifo();
    extern void __GXPEInit();
    extern void __GXFlushTextureState();
    extern void GXInitTexCacheRegion();
    extern void GXInitTlutRegion();
    extern void __GXSetTmemConfig();
    u32 i;

    OSRegisterVersion(__GXVersion);
    gx->inDispList = 0;
    gx->dlSaveContext = 1;
    gx->tcsManEnab = 1;
    gx->field_4DC = 0;
    gx->field_4E0 = 0;
    GXSetMisc(1, 0);

    __piReg = (u32*)0xCC003000;
    __cpReg = (u16*)0xCC000000;
    __peReg = (u16*)0xCC001000;
    __memReg = (u16*)0xCC004000;
    __GXFifoInit();
    GXInitFifoBase(&FifoObj, base, size);
    GXSetCPUFifo(&FifoObj);
    GXSetGPFifo(&FifoObj);
    if (!resetFuncRegistered) {
        OSRegisterResetFunction(GXResetFuncInfo_80312AD0);
        resetFuncRegistered = 1;
    }
    __GXPEInit();
    EnableWriteGatherPipe();

    gx->genMode = 0;
    GX_SET_REG(gx->genMode, 0, 0, 7);
    gx->bpMask = 255;
    GX_SET_REG(gx->bpMask, 0xF, 0, 7);
    gx->lpSize = 0;
    GX_SET_REG(gx->lpSize, 34, 0, 7);

    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = 0xFF;
        GX_SET_REG(gx->tevc[i], 0xC0 + i * 2, 0, 7);
        GX_SET_REG(gx->teva[i], 0xC1 + i * 2, 0, 7);
        GX_SET_REG(gx->tevKsel[i / 2], 0xF6 + i / 2, 0, 7);
        GX_SET_REG(gx->tref[i / 2], 0x28 + i / 2, 0, 7);
    }

    gx->iref = 0;
    GX_SET_REG(gx->iref, 0x27, 0, 7);

    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        GX_SET_REG(gx->suTs0[i], 0x30 + i * 2, 0, 7);
        GX_SET_REG(gx->suTs1[i], 0x31 + i * 2, 0, 7);
    }

    GX_SET_REG(gx->suScis0, 0x20, 0, 7);
    GX_SET_REG(gx->suScis1, 0x21, 0, 7);
    GX_SET_REG(gx->cmode0, 0x41, 0, 7);
    GX_SET_REG(gx->cmode1, 0x42, 0, 7);
    GX_SET_REG(gx->zmode, 0x40, 0, 7);
    GX_SET_REG(gx->peCtrl, 0x43, 0, 7);
    GX_SET_REG(gx->cpTex, 0, 23, 24);

    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    {
        u32 val1;
        u32 val2;

        val2 = *(u32*)0x800000F8 / 500;
        __GXFlushTextureState();
        val1 = (val2 / 2048) | 0x69000400;
        GX_BP_LOAD_REG(val1);
        __GXFlushTextureState();
        val1 = (val2 / 4224) | 0x46000200;
        GX_BP_LOAD_REG(val1);
    }

    for (i = 0; i < 8; i++) {
        GX_SET_REG(gx->vatA[i], 1, 1, 1);
        GX_SET_REG(gx->vatB[i], 1, 0, 0);
        GX_CP_LOAD_REG(i | 0x80, gx->vatB[i]);
    }

    {
        u32 reg1 = 0;
        u32 reg2 = 0;

        GX_SET_REG(reg1, 1, 31, 31);
        GX_SET_REG(reg1, 1, 30, 30);
        GX_SET_REG(reg1, 1, 29, 29);
        GX_SET_REG(reg1, 1, 28, 28);
        GX_SET_REG(reg1, 1, 27, 27);
        GX_SET_REG(reg1, 1, 26, 26);
        GX_XF_LOAD_REG(0x1000, reg1);
        GX_SET_REG(reg2, 1, 31, 31);
        GX_XF_LOAD_REG(0x1012, reg2);
    }

    {
        u32 reg = 0;
        GX_SET_REG(reg, 1, 31, 31);
        GX_SET_REG(reg, 1, 30, 30);
        GX_SET_REG(reg, 1, 29, 29);
        GX_SET_REG(reg, 1, 28, 28);
        GX_SET_REG(reg, 0x58, 0, 7);
        GX_BP_LOAD_REG(reg);
    }

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gx->defaultTexRegions[i], 0, i * 0x8000, 0, 0x80000 + i * 0x8000, 0);
    }
    for (i = 0; i < 4; i++) {
        GXInitTexCacheRegion(&gx->defaultTexRegionsCI[i], 0, (i * 2 + 8) * 0x8000, 0, (i * 2 + 9) * 0x8000, 0);
    }
    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gx->defaultTlutRegions[i], 0xC0000 + 0x2000 * i, 16);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(&gx->defaultTlutRegions[i + 16], 0xE0000 + 0x8000 * i, 64);
    }

    __cpReg[3] = 0;
    GX_SET_REG(gx->perfSel, 0, 4, 7);
    GX_CP_LOAD_REG(0x20, gx->perfSel);
    GX_XF_LOAD_REG(0x1006, 0);
    GX_BP_LOAD_REG(0x23000000);
    GX_BP_LOAD_REG(0x24000000);
    GX_BP_LOAD_REG(0x67000000);

    __GXSetTmemConfig(0);
    __GXInitGX();
    return &FifoObj;
}

#pragma peephole reset
