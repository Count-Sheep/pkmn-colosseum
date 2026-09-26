/**
 * @file tobj_exact_801BE800.c
 * @brief sysdolphin tobj.c, HSD_TObjAnimAll (fn_801BE800): .text
 *        0x801BE800-0x801BE85C.
 *
 * Built with the sysdolphin library flags (GC/1.3.2 -O4,p -O1
 * -inline auto,deferred -use_lmw_stmw on). HSD_TObjAnim has no retail
 * symbol; the loop re-tests its NULL guard (an inline fingerprint), so it is
 * recovered as static inline. Text-only unit carved from tobj.c: its
 * neighbours TObjLoad and TObjUpdateFunc are not exact yet.
 */

#include "hsd/hsd_aobj.h"
#include "hsd/hsd_tobj.h"

static inline void HSD_TObjAnim(HSD_TObj* tobj)
{
    if (tobj == NULL) {
        return;
    }
    HSD_AObjInterpretAnim(tobj->aobj, tobj, HSD_TOBJ_METHOD(tobj)->update);
}

/* HSD_TObjAnimAll */
void fn_801BE800(HSD_TObj* tobj)
{
    HSD_TObj* tp;

    if (tobj != NULL) {
        for (tp = tobj; tp != NULL; tp = tp->next) {
            HSD_TObjAnim(tp);
        }
    }
}
