/**
 * @file jobj.c
 * @brief HAL jobj.c, partial: HSD_JObjSetupMatrixSub and the IK solvers,
 *        0x8019D980 - 0x8019F01C.
 *
 * The start of a real jobj.c reconstruction (the TU runs 0x8019CE50 -
 * 0x801A4000 and is still split into candidate chunks around the linked
 * exact carve-outs). The three chunks covering this range include this
 * file and build it with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on -str reuse,readonly), no local
 * pragmas. The code is exact; what objdiff still counts against it are
 * relocations to literal-pool and .rodata objects, which only pair once the
 * whole TU (with its .sdata2 0x8047DB20.. and .rodata 0x80274AA0..) is
 * rebuilt. The file is named jobj.c because the assert file name
 * (__FILE__ = "jobj.c") is part of the code.
 *
 * Adapted from the Melee decompilation (doldecomp/melee,
 * src/sysdolphin/baselib/jobj.c), in HAL's order (deferred inlining emits
 * the functions in reverse). Colosseum differences read from retail:
 *  - the class's update method (HSD_JObjInfo + 0x50) is what
 *    HSD_RObjUpdateAll (fn_801AED88) gets, not a static JObjUpdateFunc;
 *  - jobj_get_effector_checked is auto-inlined into both solvers (assert
 *    2093 "eff"); its out-of-line copy is dead-stripped;
 *  - resolveIKJoint1: the solver's temporaries are single variables where
 *    Melee's decompiler split them (the joint-2 length is reused for the
 *    signed height; (a - b)^2 and the sqrt products are written in place),
 *    the distance threshold is the literal 1e-8f, jobj->scl and
 *    jobj->parent are re-read, and it copies an unused (0, 0, 1) vector;
 *  - local declarations are ordered to give retail's register assignment
 *    (MWCC colours locals partly by declaration order);
 *  - fn_8019D980 is the out-of-line HSD_JObjMtxIsDirty (jobj.h:605) the
 *    rest of jobj.c calls;
 *  - assert lines follow Colosseum's longer file.
 * resolveIKJoint2's only difference is at entry: retail loads 1.0f and the
 * (1,1,1) copy before jobj->child (read through the saved jobj register);
 * ours hoists the child load.
 */
#include "crt/math_ppc.h"
#include "dolphin/mtx.h"
#include "dolphin/types.h"
#include "hsd/hsd_robj.h"
#include "sysdolphin/baselib/debug.h"
#include "sysdolphin/baselib/jobj.h"

#define M_PI 3.14159265358979323846

#define JOBJ_JOINT1   (1 << 21)
#define JOBJ_JOINT2   (2 << 21)
#define JOBJ_EFFECTOR (3 << 21)
#define JOBJ_JOINT    (3 << 21)

typedef struct HSD_JObjInfoColosseum {
    HSD_JObjInfo parent;
    HSD_ObjUpdateFunc update;
} HSD_JObjInfoColosseum;

extern f32 PSVECDotProduct(const Vec* a, const Vec* b);
extern void PSVECScale(const Vec* src, Vec* dst, f32 scale);
extern void PSVECAdd(const Vec* a, const Vec* b, Vec* ab);
extern void PSVECSubtract(const Vec* a, const Vec* b, Vec* ab);
extern void PSVECNormalize(const Vec* src, Vec* unit);
extern void PSVECCrossProduct(const Vec* a, const Vec* b, Vec* axb);
extern void PSMTXRotAxisRad(Mtx m, const Vec* axis, f32 rad);
extern void PSMTXMultVec(const Mtx m, const Vec* src, Vec* dst);
extern void HSD_MtxGetTranslate(Mtx m, Vec3* vec);
extern HSD_RObj* HSD_RObjGetByType(HSD_RObj* robj, u32 type, u32 subtype);
extern BOOL fn_801AFCAC(HSD_RObj* robj, u32 type, Vec3* pos); /* HSD_RObjGetGlobalPosition */
extern void fn_801AED88(HSD_RObj* robj, void* obj, HSD_ObjUpdateFunc func); /* HSD_RObjUpdateAll */
#define HSD_RObjGetGlobalPosition fn_801AFCAC
#define HSD_RObjUpdateAll fn_801AED88

#define HSD_JOBJ_UPDATE(o) (((HSD_JObjInfoColosseum*) HSD_JOBJ_METHOD(o))->update)

static inline HSD_JObj* jobj_get_joint2(HSD_JObj* jobj)
{
    while (jobj != NULL) {
        if ((jobj->flags & JOBJ_EFFECTOR) == JOBJ_JOINT2) {
            return jobj;
        }
        jobj = jobj->next;
    }
    return NULL;
}

static inline HSD_JObj* jobj_get_effector(HSD_JObj* jobj)
{
    while (jobj != NULL) {
        if ((jobj->flags & JOBJ_EFFECTOR) == JOBJ_EFFECTOR) {
            return jobj;
        }
        jobj = jobj->next;
    }
    return NULL;
}

HSD_JObj* jobj_get_effector_checked(HSD_JObj* eff)
{
    eff = jobj_get_effector(eff);
    HSD_ASSERT(2093, eff);
    if (HSD_RObjGetByType(eff->robj, REFTYPE_JOBJ, 1) != NULL) {
        return eff;
    } else {
        return NULL;
    }
}

extern const Vec3 lbl_80274AAC;
extern const Vec3 lbl_80274AB8;

void resolveIKJoint1(HSD_JObj* jobj)
{
    Vec3 spBC = { 1.0F, 1.0F, 1.0F };
    Vec3 spB0;
    Vec3 unused;
    Vec3 sp98;
    Vec3 sp8C;
    Vec3 sp80;
    Vec3 sp74;
    Vec3 sp68;
    Vec3 sp5C;
    Vec3 sp50;
    Mtx sp20;
    f32 temp_f30;
    f32 var_f29;
    f32 temp_f31;
    f32 var_f28;
    f32 var_f27;
    f32 temp_f26;
    f32 temp_f5;
    f32 temp_f5_2;
    f32 var_f1;
    f32 var_f4;
    f32 var_f4_2;
    f32 var_f4_4;
    HSD_IKHint* new_var;
    HSD_JObj* var_r28;
    s32 var_r30;
    HSD_JObj* var_r31;
    HSD_RObj* robj;
    var_r30 = 0;
    var_f29 = 0.0F;
    var_r31 = jobj_get_joint2(jobj->child);
    spB0 = lbl_80274AAC;
    unused = lbl_80274AB8;
    if (jobj->scl != NULL) {
        spBC = *jobj->scl;
    }
    robj = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
    HSD_ASSERT(2140, robj);
    new_var = &robj->u.ik_hint;
    temp_f26 = new_var->rotate_x;
    temp_f30 = new_var->bone_length * spBC.x;
    if (var_r31 != NULL) {
        robj = HSD_RObjGetByType(var_r31->robj, REFTYPE_IKHINT, 0);
        HSD_ASSERT(2151, robj);
        var_f29 = robj->u.ik_hint.bone_length * var_r31->scale.x * spBC.x;
        var_r30 = robj->flags & 4 ? 1 : 0;
        var_r28 = jobj_get_effector_checked(var_r31->child);
    } else {
        var_r28 = jobj_get_effector_checked(jobj->child);
    }
    if (var_r28 != NULL) {
        if ((HSD_RObjGetByType(jobj->robj, REFTYPE_JOBJ, 3) == NULL) &&
            (jobj != NULL))
        {
            if (jobj->robj != NULL) {
                HSD_RObjUpdateAll(jobj->robj, jobj, HSD_JOBJ_UPDATE(jobj));
                if (HSD_JObjMtxIsDirty(jobj)) {
                    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                    jobj->flags &= ~JOBJ_MTX_DIRTY;
                }
            }
        }
        if (jobj->parent != NULL) {
            HSD_MtxGetTranslate(jobj->parent->mtx, &spB0);
        }
        HSD_RObjGetGlobalPosition(var_r28->robj, 1, &var_r28->translate);
        PSVECSubtract(&var_r28->translate, &spB0, &sp8C);
        temp_f31 = PSVECDotProduct(&sp8C, &sp8C);

        if (temp_f31 > 1e-8F) {
            sp68 = sp8C;
            if (HSD_RObjGetGlobalPosition(jobj->robj, 3, &sp5C)) {
                PSVECSubtract(&sp5C, &spB0, &sp5C);
                if (temp_f26 != 0.0F) {
                    PSMTXRotAxisRad(sp20, &sp68, temp_f26);
                    PSMTXMultVec(sp20, &sp5C, &sp5C);
                }
                PSVECCrossProduct(&sp68, &sp5C, &sp50);
                PSVECCrossProduct(&sp50, &sp68, &sp5C);
            } else {
                sp50.x = jobj->mtx[0][2];
                sp50.y = jobj->mtx[1][2];
                sp50.z = jobj->mtx[2][2];
                PSVECCrossProduct(&sp50, &sp68, &sp5C);
                PSVECCrossProduct(&sp68, &sp5C, &sp50);
            }
            var_f4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp50, &sp50)));
            PSVECScale(&sp50, &sp80, var_f4);
            var_f4_2 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp5C, &sp5C)));
            PSVECScale(&sp5C, &sp74, var_f4_2);
            temp_f5 = temp_f30 * temp_f30;
            var_f28 = var_f29 * var_f29;
            var_f27 = 0.25F * (((2.0F * (temp_f5 + var_f28)) - temp_f31) -
                               (((temp_f5 - var_f28) * (temp_f5 - var_f28)) /
                                temp_f31));
            if (var_f27 < 0.0F) {
                var_f27 = 0.0F;
            }
            temp_f5_2 = (temp_f5 - var_f27) / temp_f31;
            var_f1 = temp_f5_2 * sqrtf(1.0F / (1e-10F + temp_f5_2));
            var_f29 = var_f27 * sqrtf(1.0F / (1e-10F + var_f27));
        } else {
            var_f1 = 0.0F;
            var_f29 = temp_f30;
        }
        if (var_r30 != 0) {
            var_f29 = -var_f29;
        }
        if ((var_f28 - var_f27) < temp_f31) {
            PSVECScale(&sp8C, &sp98, var_f1);
        } else {
            PSVECScale(&sp8C, &sp98, -var_f1);
        }
        PSVECScale(&sp74, &sp5C, var_f29);
        PSVECAdd(&sp98, &sp5C, &sp98);
        var_f4_4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp98, &sp98)));
        PSVECScale(&sp98, &sp98, var_f4_4);
        jobj->mtx[0][0] = sp98.x * spBC.x;
        jobj->mtx[1][0] = sp98.y * spBC.x;
        jobj->mtx[2][0] = sp98.z * spBC.x;
        PSVECCrossProduct(&sp80, &sp98, &sp5C);
        jobj->mtx[0][1] = sp5C.x * spBC.y;
        jobj->mtx[1][1] = sp5C.y * spBC.y;
        jobj->mtx[2][1] = sp5C.z * spBC.y;
        jobj->mtx[0][2] = sp80.x * spBC.z;
        jobj->mtx[1][2] = sp80.y * spBC.z;
        jobj->mtx[2][2] = sp80.z * spBC.z;
        jobj->mtx[0][3] = spB0.x;
        jobj->mtx[1][3] = spB0.y;
        jobj->mtx[2][3] = spB0.z;
    }
}

/* Melee's HSD_JObj_803B94C4 (zero) and HSD_JObj_803B94D0 (one); the
 * (0, 0, 1) vector between them is Colosseum's. resolveIKJoint1 copies it
 * into a local it never reads. */
/* Melee's HSD_JObj_803B94C4 (zero) and HSD_JObj_803B94D0 (one); the
 * (0, 0, 1) vector between them is Colosseum's. resolveIKJoint1 copies it
 * into a local it never reads (retail stores it to the stack only). */
const Vec3 lbl_80274AAC = { 0.0F, 0.0F, 0.0F };
const Vec3 lbl_80274AB8 = { 0.0F, 0.0F, 1.0F };
const Vec3 lbl_80274AC4 = { 1.0F, 1.0F, 1.0F };

void resolveIKJoint2(HSD_JObj* jobj)
{
    Vec3 spA0;
    Vec3 sp94;
    Vec3 sp88;
    Vec3 sp7C;
    Vec3 sp70;
    Vec3 sp64;
    Mtx sp34;
    Vec3 sp28;
    Vec3 sp1C;
    f32 temp_f1_4;
    f32 var_f1_2;
    f32 var_f31;
    f32 var_f4;
    f32 var_f4_2;
    HSD_JObj* var_r29;
    HSD_RObj* temp_r28;
    HSD_RObj* robj;
    HSD_RObj* temp_r29;
    s32 var_r27;
    s32 var_r30;
    HSD_RObj* robj2;

    var_f31 = 1.0F;
    spA0 = lbl_80274AC4;
    var_r29 = jobj_get_effector_checked(jobj->child);
    if (var_r29 == NULL || jobj->parent == NULL) {
        return;
    }
    if (jobj->scl != NULL) {
        spA0 = *jobj->scl;
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp88.x = mtx[0][3];
        sp88.y = mtx[1][3];
        sp88.z = mtx[2][3];
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp7C.x = mtx[0][0];
        sp7C.y = mtx[1][0];
        sp7C.z = mtx[2][0];
    }
    var_f4 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp7C, &sp7C)));
    PSVECScale(&sp7C, &sp7C, var_f4);
    if (jobj->parent->scl != NULL) {
        var_f31 = jobj->parent->scl->x;
    }
    robj = HSD_RObjGetByType(jobj->parent->robj, REFTYPE_IKHINT, 0);
    HSD_ASSERT(2309, robj);
    PSVECScale(&sp7C, &sp7C, robj->u.ik_hint.bone_length * var_f31);
    PSVECAdd(&sp88, &sp7C, &sp94);
    PSVECSubtract(&var_r29->translate, &sp94, &sp7C);
    PSVECScale(&sp7C, &sp7C,
               sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp7C, &sp7C))));
    temp_r28 = HSD_RObjGetByType(jobj->robj, 0x20000000, 5);
    temp_r29 = HSD_RObjGetByType(jobj->robj, 0x20000000, 6);
    if ((temp_r28 != NULL) || (temp_r29 != NULL)) {
        var_r27 = 0;
        robj2 = HSD_RObjGetByType(jobj->robj, REFTYPE_IKHINT, 0);
        HSD_ASSERT(2343, robj2);
        var_r30 = robj2->flags & 4 ? 1 : 0;
        {
            MtxPtr mtx = jobj->parent->mtx;
            sp28.x = mtx[0][0];
            sp28.y = mtx[1][0];
            sp28.z = mtx[2][0];
        }
        PSVECNormalize(&sp28, &sp28);
        temp_f1_4 = PSVECDotProduct(&sp28, &sp7C);
        if (temp_f1_4 >= 1.0F) {
            var_f1_2 = 0.0F;
        } else if (temp_f1_4 <= -1.0F) {
            var_f1_2 = M_PI;
        } else {
            var_f1_2 = acosf(temp_f1_4);
        }
        if (var_r30 == 0) {
            var_f1_2 = -var_f1_2;
        }
        if (temp_r28 != NULL && var_f1_2 < temp_r28->u.limit) {
            var_f1_2 = temp_r28->u.limit;
            var_r27 = 1;
        } else if (temp_r29 != NULL) {
            if (temp_r29->u.limit < var_f1_2) {
                var_f1_2 = temp_r29->u.limit;
                var_r27 = 1;
            }
        }
        if (var_r27 != 0) {
            {
                MtxPtr mtx = jobj->parent->mtx;
                sp1C.x = mtx[0][2];
                sp1C.y = mtx[1][2];
                sp1C.z = mtx[2][2];
            }
            PSMTXRotAxisRad(sp34, &sp1C, var_f1_2);
            PSMTXMultVec(sp34, &sp28, &sp7C);
        }
    }
    {
        MtxPtr mtx = jobj->parent->mtx;
        sp64.x = mtx[0][2];
        sp64.y = mtx[1][2];
        sp64.z = mtx[2][2];
    }
    PSVECCrossProduct(&sp64, &sp7C, &sp70);
    var_f4_2 = sqrtf(1.0F / (1e-10F + PSVECDotProduct(&sp70, &sp70)));
    PSVECScale(&sp70, &sp70, var_f4_2);
    PSVECCrossProduct(&sp7C, &sp70, &sp64);
    jobj->mtx[0][0] = sp7C.x * spA0.x;
    jobj->mtx[1][0] = sp7C.y * spA0.x;
    jobj->mtx[2][0] = sp7C.z * spA0.x;
    jobj->mtx[0][1] = sp70.x * spA0.y;
    jobj->mtx[1][1] = sp70.y * spA0.y;
    jobj->mtx[2][1] = sp70.z * spA0.y;
    jobj->mtx[0][2] = sp64.x * spA0.z;
    jobj->mtx[1][2] = sp64.y * spA0.z;
    jobj->mtx[2][2] = sp64.z * spA0.z;
    jobj->mtx[0][3] = sp94.x;
    jobj->mtx[1][3] = sp94.y;
    jobj->mtx[2][3] = sp94.z;
}

/* HSD_JObjSetupMatrixSub */
void fn_8019D9DC(HSD_JObj* jobj)
{
    Vec3 sp28;
    Vec3 sp1C;
    Vec3 sp10;
    HSD_RObj* robj;
    HSD_JObj* parent;
    f32 x_scale;

    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
    jobj->flags &= ~JOBJ_MTX_DIRTY;
    if (!(jobj->flags & JOBJ_USER_DEF_MTX)) {
        switch (jobj->flags & JOBJ_JOINT) {
        case JOBJ_JOINT1:
            resolveIKJoint1(jobj);
            break;
        case JOBJ_JOINT2:
            resolveIKJoint2(jobj);
            break;
        case JOBJ_EFFECTOR:
            parent = jobj->parent;
            x_scale = 1.0F;
            if (parent != NULL) {
                robj = HSD_RObjGetByType(parent->robj, REFTYPE_IKHINT, 0);
                if (robj != NULL) {
                    sp1C.x = parent->mtx[0][3];
                    sp1C.y = parent->mtx[1][3];
                    sp1C.z = parent->mtx[2][3];
                    sp10.x = parent->mtx[0][0];
                    sp10.y = parent->mtx[1][0];
                    sp10.z = parent->mtx[2][0];
                    PSVECScale(&sp10, &sp10,
                               sqrtf(1.0F /
                                     (1e-10F + PSVECDotProduct(&sp10, &sp10))));
                    if (parent->scl != NULL) {
                        x_scale = parent->scl->x;
                    }
                    PSVECScale(&sp10, &sp10,
                               robj->u.ik_hint.bone_length * x_scale);
                    PSVECAdd(&sp1C, &sp10, &sp28);
                    jobj->mtx[0][3] = sp28.x;
                    jobj->mtx[1][3] = sp28.y;
                    jobj->mtx[2][3] = sp28.z;
                }
            }
            break;
        default:
            if (jobj->robj != NULL && jobj != NULL && jobj->robj != NULL) {
                HSD_RObjUpdateAll(jobj->robj, jobj, HSD_JOBJ_UPDATE(jobj));
                if (HSD_JObjMtxIsDirty(jobj)) {
                    HSD_JOBJ_METHOD(jobj)->make_mtx(jobj);
                    jobj->flags &= ~JOBJ_MTX_DIRTY;
                }
            }
            break;
        }
        jobj->flags &= ~JOBJ_MTX_DIRTY;
    }
}

BOOL fn_8019D980(HSD_JObj* jobj)
{
    return HSD_JObjMtxIsDirty(jobj);
}
