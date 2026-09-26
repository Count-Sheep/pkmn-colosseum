/**
 * @file hsd_tev.h
 * @brief HSD TEV/channel render state (tev.c).
 *
 * Colosseum address range: 0x801B3168 - 0x801B42C0.
 * Adapted from the Melee decompilation (doldecomp/melee, sysdolphin/baselib/
 * tev.h); HSD_Chan's layout (0x30 bytes) was checked against Colosseum's
 * channel code and prev_ch table.
 */
#ifndef HSD_TEV_H
#define HSD_TEV_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "hsd/hsd_objalloc.h"
#include "hsd/hsd_texp.h"

typedef struct _GXColorS10 {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
} GXColorS10;

typedef int GXColorSrc;
typedef int GXDiffuseFn;
typedef int GXAttnFn;

#define GX_SRC_REG 0

#define GX_COLOR0 0
#define GX_COLOR1 1
#define GX_ALPHA0 2
#define GX_ALPHA1 3
#define GX_COLOR1A1 5

#define GX_DF_NONE 0
#define GX_AF_NONE 2

typedef struct HSD_Chan HSD_Chan;
struct HSD_Chan {
    HSD_Chan* next;
    GXChannelID chan;
    u32 flags;
    GXColor amb_color;
    GXColor mat_color;
    u8 enable;
    GXColorSrc amb_src;
    GXColorSrc mat_src;
    int light_mask; /* GXLightID */
    GXDiffuseFn diff_fn;
    GXAttnFn attn_fn;
    HSD_AObj* aobj;
};

/* Functions still carrying address names in symbols.txt:
 *   fn_801B3168 _HSD_StateInvalidateTexCoordGen
 *   fn_801B3174 _HSD_StateInvalidateTevRegister
 *   fn_801B31A4 _HSD_StateInvalidateTevStage
 *   fn_801B31F4 _HSD_StateInvalidateColorChannel
 *   fn_801B3258 HSD_SetTevRegAll
 *   fn_801B3408 HSD_SetupTevStageAll    fn_801B3638 HSD_SetupTevStage
 *   fn_801B3770 HSD_StateSetNumTevStages
 *   fn_801B387C HSD_StateGetNumTevStages fn_801B3884 HSD_StateInitTev
 *   fn_801B3890 HSD_StateSetNumTexGens
 *   fn_801B3998 HSD_SetupChannelAll     fn_801B3D1C HSD_SetupChannel
 *   fn_801B3AE8 (disables a channel's lighting; no Melee counterpart)
 */
void HSD_RenderInitAllocData(void);
HSD_ObjAllocData* HSD_RenderGetAllocData(void);
HSD_ObjAllocData* HSD_TevRegGetAllocData(void);
HSD_ObjAllocData* HSD_ChanGetAllocData(void);
void fn_801B3D1C(HSD_Chan* ch);
void fn_801B3AE8(GXChannelID chan);
void HSD_StateSetNumChans(int num);
void fn_801B3998(HSD_Chan* ch);
void HSD_StateRegisterTexGen(int coord);
void fn_801B3890(void);
void fn_801B3884(void);
int fn_801B387C(void);
int HSD_StateAssignTev(void);
void fn_801B3770(void);
void fn_801B3638(HSD_TevDesc* desc);
void fn_801B3408(HSD_TevDesc* desc);
int HSD_Index2TevStage(int idx);
void fn_801B3258(void);
void fn_801B31F4(void);
void fn_801B31A4(void);
void fn_801B3174(void);
void fn_801B3168(void);

#endif /* HSD_TEV_H */
