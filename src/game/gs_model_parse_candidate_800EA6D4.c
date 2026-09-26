/**
 * @file gs_model_parse_candidate_800EA6D4.c
 * @brief _modelParseSetupInstanceMtx, 0x800EA6D4 - 0x800EA7E4.
 *
 * Standalone source for this split range of the GSmodel parse TU.
 */
#include "dolphin/types.h"

typedef struct HSD_JObj {
    u8 _pad0[0x08];
    struct HSD_JObj* next;
    struct HSD_JObj* parent;
    struct HSD_JObj* child;
    u32 flags;
    u8 _pad18[0x2C];
    f32 matrix[3][4];
} HSD_JObj;

extern const char lbl_8047CC00[7];
extern const char lbl_8047CC08[5];
extern void __assert(const char* file, u32 line, const char* condition);
extern void fn_8019D9DC(HSD_JObj* jobj);
extern u32 PSMTXInverse(f32 src[3][4], f32 inv[3][4]);
extern void PSMTXConcat(f32 a[3][4], f32 b[3][4], f32 ab[3][4]);

/*
 * The jobj.h matrix-dirty test this TU was built with (assert line 0x25D).
 * The TU keeps an out-of-line copy at 0x800EA60C (gs_model_parse_exact_
 * 800EA60C.c); here it is expanded at both call sites.
 */
static inline BOOL HSD_JObjMtxIsDirty(HSD_JObj* jobj)
{
    BOOL result;

    if (jobj == NULL) {
        __assert(lbl_8047CC00, 0x25D, lbl_8047CC08);
    }
    result = FALSE;
    if (!(jobj->flags & 0x00800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

void _modelParseSetupInstanceMtx__FP5GSmtxP9_HSD_JObjP5GSmtx(
    f32 parent[3][4], HSD_JObj* jobj, f32 dst[3][4])
{
    HSD_JObj* child;

    if (jobj != NULL && HSD_JObjMtxIsDirty(jobj)) {
        fn_8019D9DC(jobj);
    }
    child = jobj->child;
    if (child != NULL && HSD_JObjMtxIsDirty(child)) {
        fn_8019D9DC(child);
    }
    PSMTXInverse(jobj->child->matrix, dst);
    PSMTXConcat(jobj->matrix, dst, dst);
    if (parent != NULL) {
        PSMTXConcat(parent, dst, dst);
    }
}
