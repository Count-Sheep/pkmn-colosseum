/**
 * @file state.c
 * @brief HSD render state cache: pixel-engine and channel modes, material
 *        colour, GX state setters and state invalidation.
 *
 * Retail TU: .text 0x801B25C4 - 0x801B3168.
 *
 * HAL sysdolphin (>= 1.3.0.0) state.c, reconstructed with the Melee
 * decompilation (doldecomp/melee, src/sysdolphin/baselib/state.c) as
 * reference. Functions appear in HAL source order; the library is built with
 * -inline auto,deferred, which emits them in reverse order.
 *
 * Not linked as one unit: HSD_StateSetZMode (fn_801B27DC) keeps a
 * scheduling difference that is permanent under the strict policy (see the
 * function). The exact ranges 0x801B25C4-0x801B27DC and
 * 0x801B2878-0x801B294C are built from state_801B25C4.c and
 * state_801B2878.c (included below at their places in the file); this file
 * provides the rest for the candidate units. The state words, material state
 * and invalidate table stay in their data units under their address names;
 * the channel set-ups keep this file's statics.
 */
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_lobj.h"
#include "hsd/hsd_tev.h"
#include "hsd/hsd_state.h"

extern void HSD_MulColor(GXColor* a, GXColor* b, GXColor* result);

/* GX */
extern void GXSetDstAlpha(u8 enable, u8 alpha);
extern void GXSetBlendMode(int type, int src_factor, int dst_factor, int op);
extern void GXSetZMode(u8 enable, int func, u8 update);
extern void fn_800BCEBC(u8 before_tex);   /* GXSetZCompLoc */
extern void fn_800BC618(int comp0, u8 ref0, int op, int comp1,
                        u8 ref1);         /* GXSetAlphaCompare */
extern void fn_800BCFDC(u8 enable);       /* GXSetDither */

#define GX_BM_NONE 0
#define GX_BM_BLEND 1
#define GX_BL_SRCALPHA 4
#define GX_BL_INVSRCALPHA 5
#define GX_LO_NOOP 15
#define GX_LEQUAL 3
#define GX_GREATER 4
#define GX_ALWAYS 7
#define GX_AOP_AND 0
#define GX_TEXCOORD_NULL 0xFF
#define GX_PASSCLR 4

static GXColor dark_matter = { 0x00, 0x00, 0x00, 0xFF };

/* Channel set-ups used by HSD_SetupChannelMode. */
static HSD_Chan spec_chan = {
    NULL, GX_COLOR1, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 1, 0, 0,
    0, GX_DF_NONE, 0, NULL
};
static HSD_Chan diffuse_chan = {
    NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 1, 0, 0,
    0, 2, 1, NULL
};
static HSD_Chan alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 1, 0, 0, 0, 2, 1, NULL
};
static HSD_Chan vtx_alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0, 1, 0, 2, 1, NULL
};
static HSD_Chan unlit_color_chan = {
    NULL, GX_COLOR0, 0, { 0, 0, 0, 0 }, { 0xFF, 0xFF, 0xFF, 0xFF }, 0, 0, 1,
    0, GX_DF_NONE, GX_AF_NONE, NULL
};
static HSD_Chan unlit_alpha_chan = {
    NULL, GX_ALPHA0, 0, { 0, 0, 0, 0 }, { 0x00, 0x00, 0x00, 0xFF }, 0, 0, 1,
    0, GX_DF_NONE, GX_AF_NONE, NULL
};

/* HSD_SetupChannelMode */
void fn_801B2F1C(u32 rendermode)
{
    u32 color_mode;
    u32 alpha_mode;
    int specular = 0;
    int vtx_alpha = 0;
    HSD_LObj* lobj;
    int i;
    int max;
    u8 alpha;

    color_mode = rendermode & 3;
    if (color_mode == 0) {
        color_mode = 1;
    }
    alpha_mode = rendermode & 0x6000;
    if (alpha_mode == 0) {
        alpha_mode = color_mode << 13;
    }

    if (rendermode & 8) {
        spec_chan.light_mask = HSD_LObjGetLightMaskSpecular();
        fn_801B3D1C(&spec_chan);
        specular = 1;
        max = HSD_LObjGetNbActive();
        for (i = 0; i < max; i++) {
            HSD_LObj* l = HSD_LObjGetActiveByIndex(i);
            if (l != NULL) {
                fn_801A6098(l, l->color, lbl_80465710.shininess);
            }
        }
    }

    if (rendermode & 4) {
        lobj = HSD_LObjGetActiveByID(0x100);
        if (lobj != NULL && (lobj->flags & 4)) {
            HSD_MulColor(&lbl_80465710.ambient, &lobj->color, &diffuse_chan.amb_color);
        } else {
            diffuse_chan.amb_color = dark_matter;
        }
        diffuse_chan.mat_src = (color_mode >> 1) & 1;
        diffuse_chan.light_mask = HSD_LObjGetLightMaskDiffuse();
        fn_801B3D1C(&diffuse_chan);

        if (alpha_mode & 0x4000) {
            alpha_chan.chan = GX_ALPHA1;
            vtx_alpha = 1;
            fn_801B3D1C(&vtx_alpha_chan);
        } else {
            alpha_chan.chan = GX_ALPHA0;
        }
        alpha_chan.light_mask = HSD_LObjGetLightMaskAlpha();
        if (lobj != NULL && (lobj->flags & 0x10)) {
            alpha = lobj->color.a;
        } else {
            alpha = 0;
        }
        if (alpha_chan.light_mask != 0) {
            alpha_chan.enable = GX_ENABLE;
            alpha_chan.mat_color.a = 0xFF;
            alpha_chan.amb_color.a = alpha;
        } else {
            alpha_chan.enable = GX_DISABLE;
            alpha_chan.mat_color.a = alpha;
        }
        fn_801B3D1C(&alpha_chan);
    } else {
        unlit_color_chan.mat_src = (color_mode >> 1) & 1;
        fn_801B3D1C(&unlit_color_chan);
        unlit_alpha_chan.mat_src = (alpha_mode >> 14) & 1;
        fn_801B3D1C(&unlit_alpha_chan);
    }

    if (specular) {
        if (!vtx_alpha) {
            fn_801B3AE8(GX_ALPHA1);
        }
        HSD_StateSetNumChans(2);
    } else if (vtx_alpha) {
        fn_801B3AE8(GX_COLOR1);
        HSD_StateSetNumChans(2);
    } else {
        fn_801B3AE8(GX_COLOR1A1);
        HSD_StateSetNumChans(1);
    }
}

static inline void HSD_StateSetBlendMode(int type, int src_factor,
                                         int dst_factor, int op);
static inline void HSD_StateSetAlphaCompare(int comp0, u8 ref0, int op,
                                            int comp1, u8 ref1);
static inline void HSD_StateSetDstAlpha(int enable, u8 alpha);
static inline void HSD_StateSetZCompLoc(int before_tex);
static inline void HSD_StateSetDither(int enable);

/* HSD_SetupPEMode */
void fn_801B29E4(u32 rendermode, HSD_PEDesc* pe)
{

    if (pe != NULL) {
        fn_801B278C(pe->flags & 1);
        fn_801B273C(pe->flags & 2);
        HSD_StateSetDstAlpha(pe->flags & 4, pe->dst_alpha);
        HSD_StateSetBlendMode(pe->type, pe->src_factor, pe->dst_factor,
                              pe->logic_op);
        fn_801B27DC(pe->flags & 0x10, pe->z_comp, pe->flags & 0x20);
        HSD_StateSetZCompLoc(pe->flags & 8);
        HSD_StateSetAlphaCompare(pe->alpha_comp0, pe->ref0, pe->alpha_op,
                                 pe->alpha_comp1, pe->ref1);
        HSD_StateSetDither(pe->flags & 0x40);
        return;
    }
    fn_801B278C(1);
    fn_801B273C(0);
    HSD_StateSetDstAlpha(0, 0);
    HSD_StateSetBlendMode((rendermode & RENDER_XLU) ? GX_BM_BLEND : GX_BM_NONE,
                          GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    fn_801B27DC(1, (rendermode & RENDER_ZMODE_ALWAYS) ? GX_ALWAYS : GX_LEQUAL,
                (GXBool) ((rendermode & RENDER_NO_ZUPDATE) ? GX_FALSE : GX_TRUE));
    if (!(rendermode & RENDER_NO_ZUPDATE) && (rendermode & RENDER_XLU)) {
        HSD_StateSetZCompLoc(0);
        HSD_StateSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_GREATER, 0);
    } else {
        HSD_StateSetZCompLoc(1);
        HSD_StateSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    }
    HSD_StateSetDither(0);
}

static inline void setupTevMode_last(void)
{
    if (fn_801B387C() == 0) {
        HSD_TevDesc tevdesc;
        tevdesc.flags = 0;
        tevdesc.stage = HSD_StateAssignTev();
        tevdesc.coord = GX_TEXCOORD_NULL;
        tevdesc.map = GX_TEXMAP_NULL;
        tevdesc.color = GX_COLOR0A0;
        tevdesc.u.tevop.tevmode = GX_PASSCLR;
        fn_801B3638(&tevdesc);
    }
}

/* HSD_SetupRenderModeWithCustomPE */
void fn_801B294C(u32 rendermode, HSD_PEDesc* pe)
{
    setupTevMode_last();
    fn_801B29E4(rendermode, pe);
    fn_801B3258();
    fn_801B3770();
    fn_801B3890();
    fn_801B2F1C(rendermode);
}

#include "src/hsd/state_801B2878.c"

static inline void HSD_StateSetBlendMode(int type, int src_factor,
                                         int dst_factor, int op)
{
    if (lbl_8047B348 != type || lbl_8047B344 != src_factor ||
        lbl_8047B340 != dst_factor || lbl_8047B33C != op)
    {
        GXSetBlendMode(type, src_factor, dst_factor, op);
        lbl_8047B348 = type;
        lbl_8047B344 = src_factor;
        lbl_8047B340 = dst_factor;
        lbl_8047B33C = op;
    }
}

/*
 * HSD_StateSetZMode
 *
 * Status: 88.72%, permanent under the strict policy. The only difference is
 * where the update flag's `or` lands: retail computes it (into r0) after the
 * z_enable compare, this source computes it (into r4) before the state load;
 * the instructions are otherwise identical. The same body is exact where
 * HSD_SetupPEMode expands it twice. Ruled out, each on the full TU and on the
 * function alone (the standalone copy does not depend on the rest of the TU):
 * - compilers GC/1.0-1.2.5n (worse), 1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1, 2.5,
 *   2.6, 2.7 (all the same 6 instructions off), 3.0a3-3.0a5.2 (worse);
 *   -O0/-O2/-O3/-O4 with ,p and ,s, -inline auto/noauto/smart/off/deferred,
 *   -opt nopeephole/noschedule, -proc 750/740/7400/603e/604/601/generic,
 *   -sdata, -use_lmw_stmw off, -fp_contract off, C++;
 * - callee and global types: GXSetZMode(u8/GXBool/int, int/u32/GXCompare,
 *   u8/GXBool/int); state words as extern or TU-static u8/GXBool and
 *   int/u32/GXCompare; parameters int/u8/u32/BOOL and func int/u32/enum
 *   (with the header prototype and hsd_video.c's BOOL/u32/BOOL);
 * - Melee's shape (parameters reassigned with ?:), != / ?: / !! / ?1:0 /
 *   (GXBool) casts, locals of GXBool/u8/int/u32/BOOL in every
 *   declaration and assignment order, compares against the conversion
 *   expression instead of the local, operands reversed, an early-return or
 *   negated condition, store orders;
 * - deferred-inlining context: callers before, after and on both sides of
 *   the definition, a static inline body behind a global wrapper;
 * - about 1,000 generated variants in all (a random search over the
 *   combinations of the above bottoms out at 5 instructions). A local copy
 *   of func declared after the flags gets closest (4) and is register
 *   shaping, so it is not used.
 */
void fn_801B27DC(int enable, int func, int update)
{
    GXBool z_enable = enable != 0;
    GXBool z_update = update != 0;

    if (lbl_8047B338 != z_enable || lbl_8047B334 != func ||
        lbl_8047B330 != z_update)
    {
        GXSetZMode(z_enable, func, z_update);
        lbl_8047B338 = z_enable;
        lbl_8047B334 = func;
        lbl_8047B330 = z_update;
    }
}

static inline void HSD_StateSetAlphaCompare(int comp0, u8 ref0, int op,
                                            int comp1, u8 ref1)
{
    if (lbl_8047B32C != comp0 || lbl_8047B328 != ref0 ||
        lbl_8047B324 != op || lbl_8047B320 != comp1 ||
        lbl_8047B31E != ref1)
    {
        fn_800BC618(comp0, ref0, op, comp1, ref1);
        lbl_8047B32C = comp0;
        lbl_8047B328 = ref0;
        lbl_8047B324 = op;
        lbl_8047B320 = comp1;
        lbl_8047B31E = ref1;
    }
}

static inline void HSD_StateSetDstAlpha(int enable, u8 alpha)
{
    GXBool dst_enable = enable != 0;

    if (lbl_8047B31B != dst_enable || lbl_8047B31A != alpha) {
        GXSetDstAlpha(dst_enable, alpha);
        lbl_8047B31B = dst_enable;
        lbl_8047B31A = alpha;
    }
}

static inline void HSD_StateSetZCompLoc(int before_tex)
{
    GXBool before = before_tex != 0;

    if (lbl_8047B319 != before) {
        fn_800BCEBC(before);
        lbl_8047B319 = before;
    }
}

static inline void HSD_StateSetDither(int enable)
{
    GXBool dither = enable != 0;

    if (lbl_8047B318 != dither) {
        fn_800BCFDC(dither);
        lbl_8047B318 = dither;
    }
}

#include "src/hsd/state_801B25C4.c"
