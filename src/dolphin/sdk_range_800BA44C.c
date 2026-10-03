/**
 * @file sdk_range_800BA44C.c
 * @brief dolphin-sdk code, 0x800BA44C - 0x800BAE5C (11 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef enum GXChannelID_800BA4C8 {
    GX_COLOR0_800BA4C8,
    GX_COLOR1_800BA4C8,
    GX_ALPHA0_800BA4C8,
    GX_ALPHA1_800BA4C8,
    GX_COLOR0A0_800BA4C8,
    GX_COLOR1A1_800BA4C8,
} GXChannelID_800BA4C8;

typedef enum GXColorSrc_800BA6F4 {
    GX_SRC_REG_800BA6F4,
    GX_SRC_VTX_800BA6F4,
} GXColorSrc_800BA6F4;

typedef enum GXDiffuseFn_800BA6F4 {
    GX_DF_NONE_800BA6F4,
    GX_DF_SIGN_800BA6F4,
    GX_DF_CLAMP_800BA6F4,
} GXDiffuseFn_800BA6F4;

typedef enum GXAttnFn_800BA6F4 {
    GX_AF_SPEC_800BA6F4,
    GX_AF_SPOT_800BA6F4,
    GX_AF_NONE_800BA6F4,
} GXAttnFn_800BA6F4;

typedef enum GXTexFmt_800BA91C {
    GX_TF_I4_800BA91C = 0,
    GX_TF_I8_800BA91C = 1,
    GX_TF_IA4_800BA91C = 2,
    GX_TF_IA8_800BA91C = 3,
    GX_TF_RGB565_800BA91C = 4,
    GX_TF_RGB5A3_800BA91C = 5,
    GX_TF_RGBA8_800BA91C = 6,
    GX_TF_C4_800BA91C = 8,
    GX_TF_C8_800BA91C = 9,
    GX_TF_C14X2_800BA91C = 10,
    GX_TF_CMPR_800BA91C = 14,
    GX_TF_Z8_800BA91C = 17,
    GX_TF_Z16_800BA91C = 19,
    GX_TF_Z24X8_800BA91C = 22,
    GX_CTF_R4_800BA91C = 32,
    GX_CTF_RA4_800BA91C = 34,
    GX_CTF_RA8_800BA91C = 35,
    GX_CTF_A8_800BA91C = 39,
    GX_CTF_R8_800BA91C = 40,
    GX_CTF_G8_800BA91C = 41,
    GX_CTF_B8_800BA91C = 42,
    GX_CTF_RG8_800BA91C = 43,
    GX_CTF_GB8_800BA91C = 44,
    GX_CTF_Z4_800BA91C = 48,
    GX_CTF_Z8M_800BA91C = 58,
    GX_CTF_Z8L_800BA91C = 59,
    GX_CTF_Z16L_800BA91C = 60,
} GXTexFmt_800BA91C;

typedef struct GXColor_800BA4C8 {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor_800BA4C8;

typedef struct GXData_800BA4C8 {
    /* 0x000 */ u16 field_000;
    /* 0x002 */ u16 bpSent;
    /* 0x004 */ u8 pad_004[0xA4];
    /* 0x0A8 */ u32 ambColor[2];
    /* 0x0B0 */ u32 matColor[2];
    /* 0x0B8 */ u8 pad_0B8[0x14C];
    /* 0x204 */ u32 genMode;
    /* 0x208 */ u8 pad_208[0x2EC];
    /* 0x4F4 */ u32 dirtyState;
} GXData_800BA4C8;

typedef union GXFifo_800BA4C8 {
    u8 u8;
    u32 u32;
    f32 f32;
} GXFifo_800BA4C8;

extern GXData_800BA4C8* const gx;
volatile GXFifo_800BA4C8 GXWGFifo_800BA5BC : 0xCC008000;

typedef struct GXLightObj_800BA44C {
    u8 pad_00[0xC];
    u32 color;
    f32 a0;
    f32 a1;
    f32 a2;
    f32 k0;
    f32 k1;
    f32 k2;
    f32 px;
    f32 py;
    f32 pz;
    f32 nx;
    f32 ny;
    f32 nz;
} GXLightObj_800BA44C;

#if !defined(GX_EXACT_800BA4C8_800BA7C0)
/* GXLight.c: GXLoadLightObjImm, the only function of this unit. The light
   copy is the vendor's paired-single inline asm (PushLight); evidence:
   docs/asm_evidence/gx_transform.md */
static inline void PushLight(register const GXLightObj_800BA44C* lt_obj,
                             register volatile void* dest) {
    register u32 zero, color;
    register f32 a0_a1, a2_k0, k1_k2;
    register f32 px_py, pz_dx, dy_dz;

    asm {
        lwz     color, 12(lt_obj)
        xor     zero, zero, zero
        psq_l   a0_a1, 16(lt_obj), 0, 0
        psq_l   a2_k0, 24(lt_obj), 0, 0
        psq_l   k1_k2, 32(lt_obj), 0, 0
        psq_l   px_py, 40(lt_obj), 0, 0
        psq_l   pz_dx, 48(lt_obj), 0, 0
        psq_l   dy_dz, 56(lt_obj), 0, 0
        stw     zero, 0(dest)
        stw     zero, 0(dest)
        stw     zero, 0(dest)
        stw     color, 0(dest)
        psq_st  a0_a1, 0(dest), 0, 0
        psq_st  a2_k0, 0(dest), 0, 0
        psq_st  k1_k2, 0(dest), 0, 0
        psq_st  px_py, 0(dest), 0, 0
        psq_st  pz_dx, 0(dest), 0, 0
        psq_st  dy_dz, 0(dest), 0, 0
    }
}

void GXLoadLightObjImm(GXLightObj_800BA44C* light, u32 lightID) {
    u32 addr;
    u32 idx;

    idx = 31 - __cntlzw(lightID);
    idx &= 7;
    addr = 0x600 + idx * 0x10;

    GXWGFifo_800BA5BC.u8 = 0x10;
    GXWGFifo_800BA5BC.u32 = addr | 0xF0000;
    PushLight(light, &GXWGFifo_800BA5BC.u32);
    gx->bpSent = 1;
}
#else

#define SET_REG_FIELD_800BA4C8(reg, size, shift, value)                       \
    do {                                                                       \
        (reg) = ((u32)(reg) & ~(((1 << (size)) - 1) << (shift))) |            \
                ((u32)(value) << (shift));                                     \
    } while (0)

void fn_800BA4C8(GXChannelID_800BA4C8 chan, GXColor_800BA4C8 ambColor) {
    u32 reg;
    u32 colorIndex;

    switch (chan) {
    case GX_COLOR0_800BA4C8:
        reg = gx->ambColor[0];
        SET_REG_FIELD_800BA4C8(reg, 24, 8, *(u32*)&ambColor >> 8);
        colorIndex = 0;
        break;
    case GX_COLOR1_800BA4C8:
        reg = gx->ambColor[1];
        SET_REG_FIELD_800BA4C8(reg, 24, 8, *(u32*)&ambColor >> 8);
        colorIndex = 1;
        break;
    case GX_ALPHA0_800BA4C8:
        reg = gx->ambColor[0];
        SET_REG_FIELD_800BA4C8(reg, 8, 0, ambColor.a);
        colorIndex = 0;
        break;
    case GX_ALPHA1_800BA4C8:
        reg = gx->ambColor[1];
        SET_REG_FIELD_800BA4C8(reg, 8, 0, ambColor.a);
        colorIndex = 1;
        break;
    case GX_COLOR0A0_800BA4C8:
        reg = *(u32*)&ambColor;
        colorIndex = 0;
        break;
    case GX_COLOR1A1_800BA4C8:
        reg = *(u32*)&ambColor;
        colorIndex = 1;
        break;
    default:
        return;
    }

    GXWGFifo_800BA5BC.u8 = 0x10;
    GXWGFifo_800BA5BC.u32 = colorIndex + 0x100A;
    GXWGFifo_800BA5BC.u32 = reg;
    gx->bpSent = 1;
    gx->ambColor[colorIndex] = reg;
}

void fn_800BA5BC(GXChannelID_800BA4C8 chan, GXColor_800BA4C8 matColor) {
    u32 reg;
    u32 colorIndex;

    switch (chan) {
    case GX_COLOR0_800BA4C8:
        reg = gx->matColor[0];
        SET_REG_FIELD_800BA4C8(reg, 24, 8, *(u32*)&matColor >> 8);
        colorIndex = 0;
        break;
    case GX_COLOR1_800BA4C8:
        reg = gx->matColor[1];
        SET_REG_FIELD_800BA4C8(reg, 24, 8, *(u32*)&matColor >> 8);
        colorIndex = 1;
        break;
    case GX_ALPHA0_800BA4C8:
        reg = gx->matColor[0];
        SET_REG_FIELD_800BA4C8(reg, 8, 0, matColor.a);
        colorIndex = 0;
        break;
    case GX_ALPHA1_800BA4C8:
        reg = gx->matColor[1];
        SET_REG_FIELD_800BA4C8(reg, 8, 0, matColor.a);
        colorIndex = 1;
        break;
    case GX_COLOR0A0_800BA4C8:
        reg = *(u32*)&matColor;
        colorIndex = 0;
        break;
    case GX_COLOR1A1_800BA4C8:
        reg = *(u32*)&matColor;
        colorIndex = 1;
        break;
    default:
        return;
    }

    GXWGFifo_800BA5BC.u8 = 0x10;
    GXWGFifo_800BA5BC.u32 = colorIndex + 0x100C;
    GXWGFifo_800BA5BC.u32 = reg;
    gx->bpSent = 1;
    gx->matColor[colorIndex] = reg;
}

void fn_800BA6B0(u8 numChans) {
    gx->genMode = (gx->genMode & ~0x70u) | ((u32)numChans << 4);
    GXWGFifo_800BA5BC.u8 = 0x10;
    GXWGFifo_800BA5BC.u32 = 0x1009;
    GXWGFifo_800BA5BC.u32 = numChans;
    gx->dirtyState |= 4;
}

void fn_800BA6F4(GXChannelID_800BA4C8 chan, u8 enable,
                 GXColorSrc_800BA6F4 ambSrc, GXColorSrc_800BA6F4 matSrc,
                 u32 lightMask, GXDiffuseFn_800BA6F4 diffFn,
                 GXAttnFn_800BA6F4 attnFn) {
    u32 chanIdx = chan & 3;
    u32 reg;

    reg = ((u32)enable << 1) | matSrc;
    SET_REG_FIELD_800BA4C8(reg, 1, 6, ambSrc);

    if (attnFn == GX_AF_SPEC_800BA6F4) {
        diffFn = GX_DF_NONE_800BA6F4;
    }

    SET_REG_FIELD_800BA4C8(reg, 2, 7, diffFn);
    SET_REG_FIELD_800BA4C8(reg, 1, 9,
                           attnFn != GX_AF_NONE_800BA6F4);
    SET_REG_FIELD_800BA4C8(reg, 1, 10,
                           attnFn != GX_AF_SPEC_800BA6F4);
    SET_REG_FIELD_800BA4C8(reg, 4, 2, lightMask & 0xF);
    SET_REG_FIELD_800BA4C8(reg, 4, 11, (lightMask >> 4) & 0xF);

    GXWGFifo_800BA5BC.u8 = 0x10;
    GXWGFifo_800BA5BC.u32 = chanIdx + 0x100E;
    GXWGFifo_800BA5BC.u32 = reg;

    if (chan == GX_COLOR0A0_800BA4C8) {
        GXWGFifo_800BA5BC.u8 = 0x10;
        GXWGFifo_800BA5BC.u32 = 0x1010;
        GXWGFifo_800BA5BC.u32 = reg;
    } else if (chan == GX_COLOR1A1_800BA4C8) {
        GXWGFifo_800BA5BC.u8 = 0x10;
        GXWGFifo_800BA5BC.u32 = 0x1011;
        GXWGFifo_800BA5BC.u32 = reg;
    }

    gx->bpSent = 1;
}
#endif /* GX_EXACT_800BA4C8_800BA7C0 */
