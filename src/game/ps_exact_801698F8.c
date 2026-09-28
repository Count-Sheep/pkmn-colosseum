/**
 * @file ps_exact_801698F8.c
 * @brief HAL particle.c: psKillAllParticle, psGenerateParticle,
 *        0x801698F8 - 0x80169A48.
 *
 * Function-boundary carve of particle.c (see src/game/particle.c for the TU
 * extent). No jump table, no pooled constant; the only data is the point
 * JObj table lbl_80452DC8 (.bss, defined by ps_exact_8016A01C.c), kept
 * extern. Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas. Deferred inlining emits the unit in reverse definition
 * order, so psGenerateParticle comes first. The bodies are particle.c's.
 *
 * psKillAllParticle expands psKillParticle, which expands
 * psDeletePntJObjwithParticle, as retail does. Both are real particle.c
 * functions (psKillParticle is Melee's, only ever inlined and dead-stripped
 * in Colosseum; psDeletePntJObjwithParticle's out-of-line copy is at
 * 0x801696D0 in ps_exact_80169340.c). They are given here as inline
 * definitions, the unit's own bodies, so that they emit no symbol.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

extern void fn_801A05EC(HSD_JObj* jobj); /* HSD_JObjUnref */
extern void _psListDelete(HSD_Particle* pp, HSD_Particle* parent);
extern HSD_Particle* _psListGetFirst(s32 linkNo);
extern s32 psRemoveParticleAppSRT(HSD_Particle* pp);
extern HSD_Particle* psGenerateParticle0(HSD_Particle* parent, s32 linkNo, s32 bank,
                                         u64 kind, u16 texGroup, u8* list, s32 life,
                                         f32 x, f32 y, f32 z, f32 vx, f32 vy, f32 vz,
                                         f32 size, f32 grav, f32 fric, s32 palflag,
                                         HSD_Generator* gp, s32 flgInterpret);

extern HSD_JObj* lbl_80452DC8[8]; /* point JObjs */

#define psPointJObj lbl_80452DC8

HSD_Particle* psGenerateParticle(s32 linkNo, s32 bank, u64 kind, u16 texGroup,
                                 u8* list, s32 life, s32 palflag, f32 x, f32 y,
                                 f32 z, f32 vx, f32 vy, f32 vz, f32 size,
                                 f32 grav, f32 fric, HSD_Generator* gp)
{
    return psGenerateParticle0(NULL, linkNo, bank, kind, texGroup, list, life, x,
                               y, z, vx, vy, vz, size, grav, fric, palflag, gp,
                               1);
}

inline void psDeletePntJObjwithParticle(HSD_Particle* pp)
{
    if (pp->kind & 0x8000) {
        u32 no = (pp->kind >> 12) & 7;

        if (psPointJObj[no] != NULL) {
            fn_801A05EC(psPointJObj[no]);
            psPointJObj[no] = NULL;
        }
    }
}

inline void psKillParticle(HSD_Particle* pp)
{
    HSD_Particle* prev;
    HSD_Particle* p;

    prev = NULL;
    p = _psListGetFirst(pp->linkNo);
    while (p != NULL) {
        if (p == pp) {
            if (pp->gen != NULL) {
                pp->gen->numChild--;
            }
            if (pp->appsrt != NULL) {
                psRemoveParticleAppSRT(pp);
            }
            psDeletePntJObjwithParticle(p);
            _psListDelete(p, prev);
            break;
        }
        prev = p;
        p = p->next;
    }
}

void psKillAllParticle(void)
{
    HSD_Particle* pp;
    HSD_Particle* next;
    s32 i;

    for (i = 0; i < PS_NUM_LINK; i++) {
        for (pp = _psListGetFirst(i); pp != NULL; pp = next) {
            next = pp->next;
            psKillParticle(pp);
        }
    }
}
