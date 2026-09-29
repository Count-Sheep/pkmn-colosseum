/**
 * @file gs_range_8017FA5C_suffix.c
 * @brief gs small-block heap, 0x8017FB08 - 0x8017FDB0: K&R-style free.
 *
 * CodeCandidate residual of the 0x8017FA5C - 0x80180C78 retail unit, built
 * like the rest of it at `-opt level=0` with `-inline deferred`.
 *
 * Open difference (why this stays a candidate): register priority. Retail
 * ranks the inline list heads above the prev-end/data pair (fn_8017FB08).
 *
 * G2 lane notes: at level 0 the ranking follows reference weight, then
 * declaration order. The exact fn_8017FA5C is carved into its own object.
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
    GsRangeMemNode* p;

    for (p = lbl_8047B1D0->next;; p = p->next) {
        a = target;
        b = p->next;
        if (a == b) {
            return p;
        }
    }
}

static inline s32 memAreAdjacent(GsRangeMemNode* left, GsRangeMemNode* right)
{
    u8* end = (u8*)left->data + left->size;
    u8* data = right->data;

    return end == data;
}

static inline GsRangeMemNode* memAbsorbNext(GsRangeMemNode* block)
{
    GsRangeMemNode* head = lbl_8047B1D0;
    GsRangeMemNode* p;
    GsRangeMemNode* prev;

    if (!block) {
        return NULL;
    }
    for (p = head->next;; p = p->next) {
        if (memAreAdjacent(block, p)) {
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
