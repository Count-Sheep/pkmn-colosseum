/**
 * @file jobj_exact_801A05EC.c
 * @brief HAL jobj.c: HSD_JObjUnref (fn_801A05EC), 0x801A05EC - 0x801A0744.
 *
 * A single exact function carved out of the jobj.c range, built with the
 * HSD library flags (GC/1.3.2 -O4,p -O1 -inline auto,deferred
 * -use_lmw_stmw on -str reuse,readonly) and no local pragmas. The body is
 * Melee's HSD_JObjUnref (jobj.c) with object.h's ref_DEC / iref_CNT /
 * iref_INC / iref_DEC and class.h's hsdDelete expanded as retail does.
 * Colosseum's ref_DEC tests the count and decrements it as two statements
 * (two loads), unlike Melee's "ref_count-- == 0"; ref_DEC_801A0D48 is its
 * out-of-line copy in the same module.
 *
 * jobj.c's string pool is not owned here, so iref_INC's assert names its
 * two retail strings directly: lbl_80274AF4 ("object.h") and lbl_80274B00
 * ("HSD_OBJ(o)->ref_count_individual != 0"), which is how retail addresses
 * them in this function. The symbol keeps its address name.
 */
#include "hsd/hsd_class.h"
#include "hsd/hsd_jobj.h"

extern const char lbl_80274AF4[]; /* "object.h" */
extern const char lbl_80274B00[]; /* "HSD_OBJ(o)->ref_count_individual != 0" */

static inline BOOL jobjRefDec(void* o)
{
    BOOL ret;

    if ((ret = (HSD_OBJ(o)->ref_count == HSD_OBJ_NOREF))) {
        return ret;
    }
    ret = (HSD_OBJ(o)->ref_count == 0);
    HSD_OBJ(o)->ref_count -= 1;
    return ret;
}

static inline s32 jobjIRefCnt(void* o)
{
    return HSD_OBJ(o)->ref_count_individual;
}

static inline void jobjIRefInc(void* o)
{
    HSD_OBJ(o)->ref_count_individual++;
    if (!(HSD_OBJ(o)->ref_count_individual != 0)) {
        __assert(lbl_80274AF4, 0x9E, lbl_80274B00);
    }
}

static inline BOOL jobjIRefDec(void* o)
{
    BOOL ret;

    if ((ret = (HSD_OBJ(o)->ref_count_individual == 0))) {
        return ret;
    }
    HSD_OBJ(o)->ref_count_individual -= 1;
    return HSD_OBJ(o)->ref_count_individual == 0;
}

static inline void jobjDelete(void* object)
{
    if (object == NULL) {
        return;
    }
    HSD_CLASS_METHOD(object)->release((HSD_Class*) object);
    HSD_CLASS_METHOD(object)->destroy((HSD_Class*) object);
}

void fn_801A05EC(HSD_JObj* jobj)
{
    if (jobj != NULL && jobjRefDec(jobj)) {
        if (jobjIRefCnt(jobj) - 1 < 0) {
            jobjDelete(jobj);
        } else {
            jobjIRefInc(jobj);
            HSD_JOBJ_METHOD(jobj)->release_child(jobj);
            if (jobjIRefDec(jobj)) {
                jobjDelete(jobj);
            }
        }
    }
}
