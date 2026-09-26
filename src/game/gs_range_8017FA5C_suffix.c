/**
 * @file gs_range_8017FA5C_suffix.c
 * @brief gs small-block heap, 0x8017FA5C - 0x8017FDB0: free-size walk of the
 *        free list (fn_8017FA5C) and the K&R-style free (fn_8017FB08).
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * like the rest of it at `-opt level=0` with `-inline deferred`; deferred
 * inlining emits a unit's functions in reverse source order, so fn_8017FB08
 * is written before fn_8017FA5C.
 *
 * Open differences (why this stays a candidate): register priority.
 * Retail ranks the free-list head above the walk counter (fn_8017FA5C) and
 * the inline list heads above the prev-end/data pair (fn_8017FB08); the
 * recovered bodies give those variables the reverse order.
 */
#include "game/gs_range_8017FA5C_shared.h"

static inline GsRangeMemNode* memFindBlock(void* data)
{
    GsRangeMemNode* block;
    s32 i;

    block = lbl_80455070;
    for (i = 0; i < 0x1000; i++) {
        if (block->data == data) {
            return block;
        }
        block++;
    }
    return NULL;
}

static inline GsRangeMemNode* memFindPrev(GsRangeMemNode* target)
{
    GsRangeMemNode* a = NULL;
    GsRangeMemNode* b = NULL;
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;

    for (p = head->next;; p = p->next) {
        a = target;
        b = p->next;
        if (a == b) {
            return p;
        }
    }
}

static inline GsRangeMemNode* memAbsorbNext(GsRangeMemNode* block)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!block) {
        return NULL;
    }
    for (p = head->next;; p = p->next) {
        end = (u8*)block->data + block->size;
        data = p->data;
        if (end == data) {
            block->size += p->size;
            p->data = NULL;
            prev = memFindPrev(p);
            prev->next = p->next;
            if (p == lbl_8047B1D0) {
                lbl_8047B1D0 = p->next;
            }
            p->previous = NULL;
            p->next = NULL;
            return p;
        }
        if (p == lbl_8047B1D0) {
            break;
        }
    }
    return NULL;
}


void fn_8017FB08(void* allocation)
{
    u8* end = NULL;
    u8* data = NULL;
    GsRangeMemNode* bp;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!allocation) {
        return;
    }
    bp = memFindBlock(allocation);
    if (!bp) {
        return;
    }
    for (p = lbl_8047B1D0; !(bp > p && bp < p->next); p = p->next) {
        if (p >= p->next && (bp > p || bp < p->next)) {
            break;
        }
    }
    if (bp->previous) {
        end = (u8*)bp->previous->data + bp->previous->size;
        prev = bp->previous;
    }
    data = bp->data;
    if (end == data) {
        bp->previous->size += bp->size;
        bp->data = NULL;
        bp->next = NULL;
        bp->previous = NULL;
        while (memAbsorbNext(prev)) {
        }
        return;
    }
    if (p->next->data == (u8*)bp->data + bp->size) {
        bp->size += p->next->size;
        bp->next = p->next->next;
        p->next->data = NULL;
    } else {
        bp->next = p->next;
    }
    if (bp->data == (u8*)p->data + p->size) {
        p->size += bp->size;
        p->next = bp->next;
        bp->data = NULL;
    } else {
        p->next = bp;
    }
    lbl_8047B1D0 = p;
}

u32 fn_8017FA5C(void)
{
    GsRangeMemNode* node;
    u32 sum;
    s32 count;
    GsRangeMemNode* head;

    sum = 0;
    head = lbl_8047B1D0;
    if (!lbl_8047B1D0) {
        sum = lbl_80455048.remaining;
    } else {
        count = 0;
        node = head->next;
        for (;;) {
            count++;
            if ((u32)node <= 0x80000000) {
                return sum;
            }
            if (node) {
                sum += node->size;
            }
            if (node == lbl_8047B1D0) {
                break;
            }
            node = node->next;
        }
        sum += lbl_80455048.remaining;
    }
    return sum;
}
