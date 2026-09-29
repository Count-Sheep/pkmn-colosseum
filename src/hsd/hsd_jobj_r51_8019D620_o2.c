/* HSD JObj's recursive dirty walker and its out-of-line header-inline copy. */
#include "hsd/hsd_jobj.h"

/* RULE-EXCEPTION(title-path): named stand-ins for jobj.c's pooled assert
 * literals; see docs/RULE_EXCEPTIONS.md. */
extern char lbl_8047DB34;
extern char lbl_8047DB3C;

inline BOOL fn_8019D980(HSD_JObj* jobj)
{
    if (jobj == NULL) {
        __assert(&lbl_8047DB34, 605, &lbl_8047DB3C);
    }
    return !(jobj->flags & JOBJ_USER_DEF_MTX) &&
           (jobj->flags & JOBJ_MTX_DIRTY);
}

void fn_8019D620(HSD_JObj* jobj)
{
    jobj->flags |= JOBJ_MTX_DIRTY;
    if (!(jobj->flags & JOBJ_INSTANCE)) {
        jobj = jobj->child;
        while (jobj) {
            if (!(jobj->flags & JOBJ_MTX_INDEP_PARENT) &&
                !fn_8019D980(jobj))
            {
                fn_8019D620(jobj);
            }
            jobj = jobj->next;
        }
    }
}
