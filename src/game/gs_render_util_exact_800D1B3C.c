/* GS camera view/inverse matrix accessors, 0x800D1B3C - 0x800D1EB8. */
#include "dolphin/types.h"
#include "game/gs_render_util.h"

extern f32 lbl_80478ACC[];
/* RULE-EXCEPTION(title-path): volatile extern stands in for this TU's own
 * pooled 0.0f literal so each absolute-value comparison reloads it (a plain
 * 0.0f literal gives retail's instructions but mints a new pool entry this
 * text-only carve cannot own); same form as fn_800D258C; see
 * docs/RULE_EXCEPTIONS.md. */
extern volatile f32 lbl_8047C998;
extern double lbl_8047C9A0;

extern void fn_800E0628(void* dst, void* src);
extern void* HSD_CObjGetViewingMtxPtr(void* cobj);
extern void HSD_CObjGetEyePosition(void* cobj, void* data);
extern void HSD_CObjGetUpVector(void* cobj, void* data);
extern void HSD_CObjGetInterest(void* cobj, void* data);
extern void fn_800E0168(void* dst, void* src1, void* src2);
extern void fn_800E0218(void* mtx, void* eye, void* up, void* interest);
extern void GSmtxMakeXRotation(void* mtx, f32 angle);
extern void GSmtxMakeYRotation(void* mtx, f32 angle);
extern void GSmtxMakeZRotation(void* mtx, f32 angle);
extern void fn_800E05C0(void* mtx, f32 x, f32 y, f32 z);
extern void fn_800E0290(void* dst, void* lhs, void* rhs);
extern void fn_800E0264(void* dst, void* src);

/*
 * Rebuilds the camera's view matrix. Retail expands this body identically in
 * fn_800D1B3C, fn_800D1D00 and fn_800D258C (repeated expansion). Pokemon XD
 * (GXXE01) names it cameraBuildMatrix__FP9_GScamera (0x198 bytes, dead-stripped
 * from the NXXJ01 demo map because every use is inlined); XD's callers
 * GScameraGetInvMatrixPtr, GScameraGetMatrixPtr and GScameraSetActiveCamera
 * sit where these three Colosseum functions do. References: TeamOrre/xd-decomp
 * config/GXXE01/symbols.txt @ 4989794e, StarsMmd/Colo-XD-PBR-symbol-maps
 * NXXJ01.map @ 6b51d3af.
 */
static inline void cameraBuildMatrix(GSRenderCamera* c) {
    if (c->isAnimating != 0) {
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
    } else if (c->useLookAt == 1) {
        f32 tmp[3];
        fn_800E0168(tmp, &c->eye, &c->interest);
        {
            f32 ax = tmp[0];
            ax = ax > lbl_8047C998 ? ax : -ax;
            if (ax < lbl_80478ACC[0]) {
                ax = tmp[1];
                ax = ax > lbl_8047C998 ? ax : -ax;
                if (ax < lbl_80478ACC[0]) {
                    ax = tmp[2];
                    ax = ax > lbl_8047C998 ? ax : -ax;
                    if (ax < lbl_80478ACC[0]) {
                        f32 v = c->interest.x;
                        c->interest.x = (f32)(v + lbl_8047C9A0);
                    }
                }
            }
        }
        fn_800E0218(c->viewMtx, &c->eye, &c->upVector, &c->interest);
    } else {
        f32 tmp3[3][4];
        f32 tmp1[3][4];
        f32 tmp2[3][4];
        GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
        GSmtxMakeYRotation(tmp1, -c->rotation.y);
        GSmtxMakeZRotation(tmp2, -c->rotation.z);
        fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp1);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp2);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp3);
    }
    c->dirty = 0;
}

/* XD: GScameraGetInvMatrixPtr. */
void* fn_800D1B3C(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;

    if (c->dirty != 0) {
        cameraBuildMatrix(c);
    }
    fn_800E0264(c->projectionMtx, c->viewMtx);
    return c->projectionMtx;
}

/* XD: GScameraGetMatrixPtr. */
void* fn_800D1D00(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;

    if (c->dirty != 0) {
        cameraBuildMatrix(c);
    }
    return c->viewMtx;
}
