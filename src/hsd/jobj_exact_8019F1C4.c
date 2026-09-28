/**
 * @file jobj_exact_8019F1C4.c
 * @brief HAL jobj.c: fn_8019F1C4, 0x8019F1C4 - 0x8019F718.
 *
 * A single exact function carved out of the jobj.c range, built with the
 * HSD library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on -str reuse,readonly) and no local pragmas, like
 * jobj_exact_8019F01C.c. The retail body is the recursive function with
 * its self-calls expanded by the same size-limited auto-inlining; it
 * uses no jump table, no pooled constant and no data.
 */
#include "dolphin/types.h"
#include "hsd/hsd_dobj.h"
#include "sysdolphin/baselib/jobj.h"

/* Counts the vertices of the tree's visible DObjs (both totals of
 * HSD_DObjCountVertices); an instance counts its instanced tree. */
void fn_8019F1C4(HSD_JObj* jobj, s32* total_a, s32* total_b)
{
    HSD_JObj* child;
    s32 sum_a = 0;
    s32 sum_b = 0;

    if (jobj != NULL) {
        if (jobj->flags & JOBJ_INSTANCE) {
            fn_8019F1C4(jobj->child, &sum_a, &sum_b);
        } else {
            if (!(jobj->flags & JOBJ_HIDDEN) && union_type_dobj(jobj)) {
                HSD_DObjCountVertices(jobj->u.dobj, &sum_a, &sum_b);
            }
            for (child = jobj->child; child != NULL; child = child->next) {
                s32 child_a;
                s32 child_b;
                fn_8019F1C4(child, &child_a, &child_b);
                sum_a += child_a;
                sum_b += child_b;
            }
        }
    }
    if (total_a) {
        *total_a = sum_a;
    }
    if (total_b) {
        *total_b = sum_b;
    }
}
