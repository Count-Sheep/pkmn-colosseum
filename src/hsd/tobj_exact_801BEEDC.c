/**
 * @file tobj_exact_801BEEDC.c
 * @brief sysdolphin tobj.c, HSD_TObjAddAnimAll (fn_801BEEDC): .text
 *        0x801BEEDC-0x801BF098, the last function of the TU.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on). HSD_TObjAddAnim,
 * lookupTextureAnim and the Tlut helpers have no retail symbol: the loop
 * re-tests HSD_TObjAddAnim's NULL guard, and HSD_TlutAlloc /
 * HSD_TlutLoadDesc are expanded inline (assert line 2209, "tlut").
 *
 * The allocation assert is sysdolphin's HSD_ASSERT, an expression
 * `(cond) ? (void) 0 : __assert(...)`; written as an if-statement MWCC
 * schedules the following memcpy's arguments differently (98.2%).
 * TOBJ_ASSERT below is that macro with the file/expression strings taken
 * by label, because this text-only carve cannot emit tobj.c's .sdata2.
 */

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_tobj.h"

extern void* fn_80193B10(s32 size);           /* hsdAllocMemPiece */
extern void fn_80193AF0(void* mem, s32 size); /* hsdFreeMemPiece  */
extern void* fn_801A6928(s32 size);           /* HSD_MemAlloc     */
extern void fn_801A6960(void* mem);           /* HSD_Free         */
extern void* memset(void* dst, int c, u32 n);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void __assert(const char* file, u32 line, const char* expr);

extern const char lbl_8047DEB0[7]; /* "tobj.c" */
extern const char lbl_8047DEC4[8]; /* "tlut" */

#define TOBJ_ASSERT(line, cond, expr) \
    ((cond) ? ((void) 0) : __assert(lbl_8047DEB0, line, expr))

static inline void HSD_TlutFree(HSD_Tlut* tlut)
{
    fn_80193AF0(tlut, sizeof(HSD_Tlut));
}

static inline void HSD_TlutRemove(HSD_Tlut* tlut)
{
    if (tlut != NULL) {
        HSD_TlutFree(tlut);
    }
}

static inline HSD_Tlut* HSD_TlutAlloc(void)
{
    HSD_Tlut* tlut = fn_80193B10(sizeof(HSD_Tlut));

    TOBJ_ASSERT(2209, tlut, lbl_8047DEC4);
    memset(tlut, 0, sizeof(HSD_Tlut));
    return tlut;
}

static inline HSD_Tlut* HSD_TlutLoadDesc(HSD_TlutDesc* tlutdesc)
{
    if (tlutdesc != NULL) {
        HSD_Tlut* tlut = HSD_TlutAlloc();
        memcpy(tlut, tlutdesc, sizeof(HSD_Tlut));
        return tlut;
    }
    return NULL;
}

static inline HSD_TexAnim* lookupTextureAnim(s32 id, HSD_TexAnim* texanim)
{
    HSD_TexAnim* ta;

    for (ta = texanim; ta != NULL; ta = ta->next) {
        if ((s32) ta->id == id) {
            return ta;
        }
    }
    return NULL;
}

static inline void HSD_TObjAddAnim(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    s32 i;
    HSD_TexAnim* ta;

    if (tobj != NULL) {
        if ((ta = lookupTextureAnim(tobj->anim_id, texanim)) != NULL) {
            if (tobj->aobj != NULL) {
                HSD_AObjRemove(tobj->aobj);
            }
            tobj->aobj = HSD_AObjLoadDesc(ta->aobjdesc);
            tobj->imagetbl = ta->imagetbl;

            if (tobj->tluttbl != NULL) {
                for (i = 0; tobj->tluttbl[i] != NULL; i++) {
                    HSD_TlutRemove(tobj->tluttbl[i]);
                }
                fn_801A6960(tobj->tluttbl);
            }

            if (ta->n_tluttbl != 0) {
                tobj->tluttbl = (HSD_Tlut**) fn_801A6928(
                    (s32) sizeof(HSD_Tlut*) * (ta->n_tluttbl + 1));
                for (i = 0; i < ta->n_tluttbl; i++) {
                    tobj->tluttbl[i] = HSD_TlutLoadDesc(ta->tluttbl[i]);
                }
                tobj->tluttbl[i] = NULL;
            } else {
                tobj->tluttbl = NULL;
            }
            tobj->tlut_no = (u8) -1;
        }
    }
}

/* HSD_TObjAddAnimAll */
void fn_801BEEDC(HSD_TObj* tobj, HSD_TexAnim* texanim)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjAddAnim(tp, texanim);
        }
    }
}
