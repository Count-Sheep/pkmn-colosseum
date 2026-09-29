/**
 * @file dobj_exact_801993A4.c
 * @brief HAL dobj.c: DObjLoad (0x801993A4 - 0x80199568).
 *
 * The HSD_DObj class's load method: load the next DObj of the chain (with
 * HSD_DObjLoadDesc's class lookup expanded), the MObj and the PObj, then set
 * the blending flags from the MObj's render mode.
 *
 * Off the title path. User-approved rule exception (2026-09-29, this
 * function only; docs/RULE_EXCEPTIONS.md). With the sysdolphin library
 * flags and the natural Melee shape the code is identical except that MWCC
 * gives dobj r31 and desc r28 where retail uses desc r31 / dobj r30 / inner
 * desc r29 / inner dobj r28 (96.9%); a 2026-09-27 carve with helper inlines
 * reached 95.4% in every arrangement tried. The only exact form is the one
 * kept below from the legacy hsd_dobj.c: a local optimization_level 1
 * pragma under GC/1.3 -use_lmw_stmw on, copies of both parameters into
 * locals, and the pooled literals read through named extern stand-ins.
 * Each construct is tagged RULE-EXCEPTION(user-approved).
 */
#include "hsd/hsd_dobj.h"
#include "hsd/hsd_class.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_pobj.h"

extern void OSReport(const char* fmt, ...);
extern void HSD_Panic(const char* file, s32 line, const char* msg);
extern void __assert(const char* file, u32 line, const char* expr);
extern HSD_ClassInfo* fn_80193748(const char* class_name); /* hsdSearchClassInfo */
extern void* fn_80193828(HSD_ClassInfo* info);             /* hsdNew */

extern HSD_DObjInfo lbl_8036C7A0;  /* hsdDObj */
extern HSD_ClassInfo* lbl_8047B260; /* default_class */

/* RULE-EXCEPTION(user-approved): named extern stand-ins for dobj.c's own pooled literals - see docs/RULE_EXCEPTIONS.md */
extern char lbl_8027472C[]; /* unknown-render-mode format string */
extern char lbl_8047DA18;   /* "dobj.c" */
extern char lbl_8047DA20;   /* "dobj" */
extern char lbl_8047DA28;   /* panic message */

#pragma push
/* RULE-EXCEPTION(user-approved): local compiler-control pragma - see docs/RULE_EXCEPTIONS.md */
#pragma optimization_level 1
int DObjLoad(HSD_DObj* dobj_arg, HSD_DObjDesc* desc_arg)
{
    HSD_DObjDesc* desc;
    HSD_DObj* dobj;
    HSD_DObjDesc* subdesc;
    HSD_DObj* sub;
    HSD_ClassInfo* info;

    /* RULE-EXCEPTION(user-approved): parameter copies used only for register allocation - see docs/RULE_EXCEPTIONS.md */
    dobj = dobj_arg;
    desc = desc_arg;
    subdesc = desc->next;
    if (subdesc == NULL) {
        sub = NULL;
    } else {
        if (subdesc->class_name == NULL
            || (info = fn_80193748(subdesc->class_name)) == NULL)
        {
            info = lbl_8047B260 ? lbl_8047B260 : (HSD_ClassInfo*) &lbl_8036C7A0;
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(&lbl_8047DA18, 0x214, &lbl_8047DA20);
            }
        } else {
            sub = (HSD_DObj*) fn_80193828(info);
            if (sub == NULL) {
                __assert(&lbl_8047DA18, 0x181, &lbl_8047DA20);
            }
        }
        /* RULE-EXCEPTION(user-approved): parameter re-copy used only for register allocation - see docs/RULE_EXCEPTIONS.md */
        dobj = dobj_arg;
        {
            void (**vtbl)(void) = (void (**)(void)) HSD_CLASS_METHOD(sub);
            ((int (*)(HSD_DObj*, HSD_DObjDesc*)) vtbl[0x40 / 4])(sub, subdesc);
        }
    }
    dobj->next = sub;
    dobj->mobj = HSD_MObjLoadDesc(desc->mobjdesc);
    dobj->pobj = HSD_PObjLoadDesc(desc->pobjdesc);

    if (dobj->mobj != NULL) {
        u32 type = dobj->mobj->rendermode;
        type = type & 0x60000000;
        switch (type) {
        case 0x00000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x02;
            }
            break;
        case 0x40000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x08;
            }
            break;
        case 0x60000000:
            if (dobj != NULL) {
                dobj->flags = (dobj->flags & ~0x0E) | 0x04;
            }
            break;
        default:
            OSReport(lbl_8027472C, dobj->mobj->rendermode);
            HSD_Panic(&lbl_8047DA18, 0x13F, &lbl_8047DA28);
            break;
        }
    }
    return 0;
}
#pragma pop
