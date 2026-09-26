/**
 * @file gs_material_exact_800DFE98.c
 * @brief GSmaterial 0x800DFE98 - 0x800DFEEC: _matGSmatObjLoad, the GS
 *        material MObj class's load method.
 *
 * The last function of the GSmaterial range (gs_material_800DF498.c). It is
 * exact and is carved from the candidate tail so it can link;
 * _matGSmatEnableEnvMapExt (0x800DFABC) still differs by a register
 * permutation and stays in gs_material_candidate_800DFABC.c. Same GC/1.3
 * flags as the rest of the range.
 */
#include "dolphin/types.h"
#include "hsd/hsd_mobj.h"

struct GSmaterial;

/* GS material MObj: an HSD_MObj with a back pointer to its GSmaterial. */
typedef struct GSmatObj {
    /* 0x00 */ HSD_MObj mobj;
    /* 0x20 */ struct GSmaterial* material;
} GSmatObj; /* size 0x24 */

extern HSD_MObjInfo lbl_8036CB30; /* hsdMObj */

int _matGSmatObjLoad(HSD_MObj* mobj, HSD_MObjDesc* desc)
{
    int result = lbl_8036CB30.load(mobj, desc);

    if (result != 0) {
        return result;
    }
    ((GSmatObj*)mobj)->material = NULL;
    return 0;
}
