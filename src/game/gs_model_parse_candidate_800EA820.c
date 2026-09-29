/**
 * @file gs_model_parse_candidate_800EA820.c
 * @brief GSmodel parse (XD GSmodel/parse.c): _modelParseJObjDispDObj,
 *        _modelParseJObjDispSub and _modelParseDObjDisp,
 *        0x800EA820 - 0x800EAFE4.
 *
 * The parse TU is built with GC/1.3.2 and -inline auto,deferred. Deferred
 * inlining emits the functions in reverse source order, so they are written
 * bottom-up (helpers first, as in the C source) and come out in retail
 * order. It is also why _modelParseDObjDisp is expanded inside
 * _modelParseJObjDispSub although it is defined after it in the retail
 * text; XD's DispSub (0x3A4) likewise contains its DObjDisp (0x348).
 *
 * The per-PObj helpers are XD's stripped parse.c inlines (NXXJ01.map lines
 * 6195-6199, all UNUSED: _modelParsePObjDisp 0x334,
 * _modelParsePObjSetupMtx 0x234, _modelParseLoadSharedMatrix 0x154,
 * _modelParseLoadRigidMatrix 0x88; StarsMmd/Colo-XD-PBR-symbol-maps).
 * modelParsePObjDisp is expanded twice (here and in DispSub, via the
 * inlined DObjDisp) and modelParseLoadRigidMatrix twice per expansion.
 * _modelParseLoadEnvelopeMatrix follows as its own candidate
 * (gs_model_parse_candidate_800EAFE4.c).
 */
#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

typedef struct HSD_JObj {
    u8 _pad0[0x08];
    struct HSD_JObj* next;
    struct HSD_JObj* parent;
    struct HSD_JObj* child;
    u32 flags;
    u8 _pad18[0x2C];
    f32 matrix[3][4];
    u8 _pad74[0x4];
    f32 (*envelopemtx)[4];
} HSD_JObj;

#define JOBJ_PTCL   (1 << 5)
#define JOBJ_SPLINE (1 << 14)
#define union_type_dobj(o) \
    ((o)->flags & (JOBJ_PTCL | JOBJ_SPLINE) ? FALSE : TRUE)

typedef void (*GSModelPObjDisp)(HSD_PObj* pobj, f32 vmtx[3][4], f32 pmtx[3][4],
                               f32 smtx[3][4], void* arg);

#include "hsd/hsd_dobj.h"

extern const u8 lbl_8047CC00[7];
extern const u8 lbl_8047CC08[5];
extern void __assert(const char* file, u32 line, const char* condition);

static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert((const char*)lbl_8047CC00, 0x25d, (const char*)lbl_8047CC08);
    }
    result = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

extern void fn_8019D9DC(HSD_JObj*);
static inline void HSD_JObjSetupMatrix(HSD_JObj* jobj)
{
    if (jobj == NULL || !HSD_JObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

extern f32 lbl_804016A0[12];
extern void fn_800E064C(f32* matrix);
extern void fn_8019D9DC(HSD_JObj*);
extern void HSD_JObjMakePositionMtx(HSD_JObj*, f32* obj_mtx, f32* out);
void _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* vmtx, f32* pmtx, HSD_TrspMask pass,
    u8 is_visible, GSModelPObjDisp disp, void* arg);
extern f32 lbl_804016D0[24];
extern const f32 lbl_8047CC18;
extern const f32 lbl_8047CC1C;

extern void fn_8019F024(HSD_JObj* jobj);
extern void fn_801AB63C(u32 first, u32 second);
extern void HSD_DObjSetCurrent(HSD_DObj* dobj);
extern HSD_JObj* HSD_JObjGetCurrent(void);
extern void HSD_PObjGetMtxMark(s32 index, u32* object, u32* mark);
extern void fn_801AB5F8(s32 index, void* object, s32 mark);
extern void PSMTXConcat(f32* left, f32* right, f32* out);
extern void fn_800E0628(void* dst, void* src);
void _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(
    HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* matrices);

static inline void modelParseLoadRigidMatrix(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    HSD_JObj* current;
    u32 marked_object;
    u32 mark;

    current = HSD_JObjGetCurrent();
    HSD_PObjGetMtxMark(0, &marked_object, &mark);
    if (marked_object != (u32)current || mark != 1) {
        fn_801AB5F8(0, current, 1);
        fn_800E0628(mtx, pmtx);
    }
}

static inline void modelParseLoadSharedMatrix(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    HSD_JObj* current;
    u32 marked_object;
    u32 mark;
    u8 setup_current = FALSE;
    u8 setup_joint = FALSE;
    f32 matrix[12];


    current = HSD_JObjGetCurrent();
    HSD_PObjGetMtxMark(0, &marked_object, &mark);
    if (marked_object != (u32)current && mark != 1) {
        setup_current = TRUE;
    }
    fn_801AB5F8(0, current, 1);
    HSD_PObjGetMtxMark(1, &marked_object, &mark);
    if (marked_object != (u32)pobj->u.jobj && mark != 1) {
        setup_joint = TRUE;
    }
    fn_801AB5F8(1, pobj->u.jobj, 1);
    if (setup_current || setup_joint) {
        if (setup_current) {
            fn_800E0628(mtx, pmtx);
        }
        if (setup_joint) {
            HSD_JObjSetupMatrix(pobj->u.jobj);
            PSMTXConcat(vmtx, (f32*)((u8*)pobj->u.jobj + 0x44), matrix);
            fn_800E0628(mtx + 12, matrix);
        }
    }
}

static inline void modelParsePObjSetupMtx(HSD_PObj* pobj, f32* vmtx, f32* pmtx, f32* mtx)
{
    switch (pobj->flags & 0x3000) {
    case 0:
        if (pobj->u.jobj == NULL) {
            modelParseLoadRigidMatrix(pobj, vmtx, pmtx, mtx);
        } else {
            modelParseLoadSharedMatrix(pobj, vmtx, pmtx, mtx);
        }
        break;
    case 0x1000:
        modelParseLoadRigidMatrix(pobj, vmtx, pmtx, mtx);
        break;
    case 0x2000:
        _modelParseLoadEnvelopeMatrix__FP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtx(pobj, vmtx, pmtx, mtx);
        break;
    }
}

static inline void modelParsePObjDisp(HSD_PObj* pobj, f32* vmtx, f32* pmtx, u8 is_visible,
                                      GSModelPObjDisp disp, void* arg)
{
    if (pobj->flags & 0x800) {
        return;
    }
    if (is_visible) {
        modelParsePObjSetupMtx(pobj, vmtx, pmtx, lbl_804016D0);
    }
    if ((pobj->flags & 0x3000) == 0x1000) {
        disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, NULL, arg);
    } else if (disp != NULL) {
        if (is_visible) {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, (f32(*)[4])lbl_804016D0, arg);
        } else {
            disp(pobj, (f32(*)[4])vmtx, (f32(*)[4])pmtx, NULL, arg);
        }
    }
}

void _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_DObj* dobj, f32* vmtx, f32* pmtx, u8 is_visible,
    GSModelPObjDisp disp, void* arg)
{
    HSD_PObj* pobj;

    for (pobj = dobj->pobj; pobj != NULL; pobj = pobj->next) {
        modelParsePObjDisp(pobj, vmtx, pmtx, is_visible, disp, arg);
    }
}

void _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* vmtx, f32* pmtx, HSD_TrspMask pass,
    u8 is_visible, GSModelPObjDisp disp, void* arg)
{
    HSD_DObj* dobj;
    u32 pass_mask;

    fn_8019F024(jobj);
    pass_mask = (u32)pass << 1;
    fn_801AB63C(0, 0);
    for (dobj = *(HSD_DObj**)((u8*)jobj + 0x18); dobj != NULL; dobj = dobj->next) {
        if ((dobj->flags & 1) == 0 && (dobj->flags & pass_mask) != 0) {
            HSD_DObjSetCurrent(dobj);
            _modelParseDObjDisp__FP9_HSD_DObjP5GSmtxP5GSmtxbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(dobj, vmtx, pmtx, is_visible, disp, arg);
        }
    }
    HSD_DObjSetCurrent(NULL);
    fn_8019F024(NULL);
}

void _modelParseJObjDispDObj__FP9_HSD_JObjP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(
    HSD_JObj* jobj, f32* obj_mtx, HSD_TrspMask trsp_mask,
    u8 is_visible, GSModelPObjDisp disp, void* arg)
{
    f32 vmtx[12];
    u32 passes;

    if (jobj->flags & 0x10) {
        return;
    }
    passes = jobj->flags & ((u32)trsp_mask << 18);
    if (passes == 0) {
        return;
    }
    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D9DC(jobj);
    }
    if (obj_mtx == NULL) {
        fn_800E064C(lbl_804016A0);
        obj_mtx = lbl_804016A0;
    }
    HSD_JObjMakePositionMtx(jobj, obj_mtx, vmtx);
    if (passes & 0x00040000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 1, is_visible, disp, arg);
    }
    if (passes & 0x00100000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 4, is_visible, disp, arg);
    }
    if (passes & 0x00080000) {
        _modelParseJObjDispSub__FP9_HSD_JObjP5GSmtxP5GSmtx12HSD_TrspMaskbPFP9_HSD_PObjP5GSmtxP5GSmtxP5GSmtxPv_vPv(jobj, obj_mtx, vmtx, 2, is_visible, disp, arg);
    }
}
