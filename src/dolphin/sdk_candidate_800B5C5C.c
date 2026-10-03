/** Candidate-only residual range. */
#include "src/dolphin/sdk_range_800AE3F0.c"

extern GXData* const gx;
extern u16* __memReg;
extern u16* __cpReg;
extern OSTime OSGetTime(void);
extern void GXSetBreakPtCallback(void* callback);
extern void fn_800B8FD8(void* callback);
extern void fn_800B90A4(void* callback);
extern void PPCSync(void);
extern void __GXAbort(void);

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

#pragma peephole off
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
#pragma peephole reset
