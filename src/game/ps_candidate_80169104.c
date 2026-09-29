/**
 * @file ps_candidate_80169104.c
 * @brief HAL particle.c: psSetGeneratorAngleRadiusScale, 0x80169104 -
 *        0x80169340, with its switch jump table (.data 0x8036BF80).
 *
 * Function-boundary carve of particle.c (see src/game/particle.c for the TU
 * extent and the body this copies). The object owns the switch's jump
 * table, the TU's only .data (0x8036BF80 - 0x8036BFA4), which MWCC emits
 * from the switch itself. The command lists it reads are the .bss array
 * psCmdList defined by ps_exact_8016A01C.c. The average's divisor 3.0f is
 * the first entry of particle.c's .sdata2 pool (0x8047D5B0); the pool's
 * other two entries belong to the unlinked psRemoveParticle chunk and a
 * data object cannot start at 0x8047D5B4, so the pool stays inside
 * game/data/sdata2_8047D560.c and this carve refers to 3.0f by its pool
 * name instead of emitting its own literal.
 *
 * Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

extern HSD_PSCmdList** lbl_80452AC8[PS_NUM_BANK]; /* psCmdList */

#define psCmdList lbl_80452AC8
#define GEN_CMD(gp) psCmdList[bank][idx]

/* RULE-EXCEPTION(title-path): named stand-in for particle.c's pooled
 * literal 3.0f - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D5B0; /* 3.0f */

void psSetGeneratorAngleRadiusScale(HSD_Generator* gp, f32* scale, u8 motion)
{
    u16 type = gp->type & 0xF;
    s32 bank = gp->bank;
    s32 idx = gp->gfxIdx;
    f32 avg = (scale[0] + scale[1] + scale[2]) / lbl_8047D5B0;

    switch (type) {
    case 0:
    case 3:
    case 4:
        gp->aux.cone.minAngle = avg * GEN_CMD(gp)->param1;
        gp->aux.cone.maxAngle = avg * GEN_CMD(gp)->param2;
        break;
    case 1:
        gp->aux.line.x2 = scale[0] * GEN_CMD(gp)->param1;
        gp->aux.line.y2 = scale[1] * GEN_CMD(gp)->param2;
        gp->aux.line.z2 = scale[2] * GEN_CMD(gp)->param3;
        break;
    case 6:
    case 7:
        gp->aux.cone.minAngle = avg * GEN_CMD(gp)->param1;
        gp->aux.cone.maxAngle = avg * GEN_CMD(gp)->param2;
        gp->aux.cone.height = avg * GEN_CMD(gp)->param3;
        break;
    case 5:
        gp->aux.rect.xx = gp->aux.rect.x = scale[0] * GEN_CMD(gp)->param1;
        gp->aux.rect.yy = gp->aux.rect.y = scale[1] * GEN_CMD(gp)->param2;
        gp->aux.rect.zz = gp->aux.rect.z = scale[2] * GEN_CMD(gp)->param3;
        break;
    case 8:
        gp->aux.sphere.latRange = avg * GEN_CMD(gp)->param1;
        gp->aux.sphere.lonRange = avg * GEN_CMD(gp)->param2;
        break;
    }

    gp->radius = avg * GEN_CMD(gp)->radius;
    if (motion == 1) {
        gp->grav *= avg;
        gp->fric *= avg;
        gp->posFlags |= 0x1000;
    }
    gp->scale2.x = scale[0];
    gp->scale2.y = scale[1];
    gp->scale2.z = scale[2];
}
