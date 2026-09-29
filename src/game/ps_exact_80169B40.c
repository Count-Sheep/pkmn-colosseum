/**
 * @file ps_exact_80169B40.c
 * @brief HAL particle.c: psGenerateParticle0, 0x80169B40 - 0x80169DF8.
 *
 * Function-boundary carve of particle.c (see src/game/particle.c for the TU
 * extent and the body this copies). No jump table, no .bss and no data of
 * its own; it reads two entries of particle.c's .sdata2 pool, 0.0f
 * (0x8047D5B4) and 1.0f (0x8047D5B8). That pool is linked inside
 * game/data/sdata2_8047D560.c (a data object cannot start at 0x8047D5B4, so
 * it stays there until psRemoveParticle, the pool's other reader, is exact
 * and the TU links as a whole), so this carve refers to the entries by their
 * pool names instead of emitting its own literals. psGenerateParticleID0
 * (0x80169A48, ps_candidate_80169A48.c) reaches the pooled bank tables from
 * one base, which only the whole TU reproduces, and psRemoveParticle
 * (0x80169DF8, ps_candidate_80169DF8.c) is not exact yet; both stay
 * candidate chunks.
 *
 * Built with the particle library flags (GC/1.3.2 -O4,p -inline
 * auto,deferred -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly),
 * no local pragmas.
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

extern HSD_Particle* _psListNew(HSD_Particle* parent, u32 linkNo);
extern u16 psGetNewIDNum(void);
extern s32 psAttachParticleAppSRT(HSD_Particle* pp, HSD_psAppSRT* appsrt);
extern HSD_Particle* psInterpretParticle0(HSD_Particle* pp, HSD_Particle* prev);

/* RULE-EXCEPTION(title-path): named stand-ins for particle.c's pooled
 * literals 0.0f and 1.0f - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047D5B4; /* 0.0f */
extern const f32 lbl_8047D5B8; /* 1.0f */

HSD_Particle* psGenerateParticle0(HSD_Particle* parent, s32 linkNo, s32 bank,
                                  u64 kind, u16 texGroup, u8* list, s32 life,
                                  f32 x, f32 y, f32 z, f32 vx, f32 vy, f32 vz,
                                  f32 size, f32 grav, f32 fric, s32 palflag,
                                  HSD_Generator* gp, s32 flgInterpret)
{
    HSD_Particle* pp;

    pp = _psListNew(parent, linkNo);
    if (pp == NULL) {
        return NULL;
    }

    if (gp != NULL) {
        pp->idnum = gp->idnum;
    } else {
        pp->idnum = psGetNewIDNum();
    }

    pp->appsrt = NULL;
    if (gp != NULL && gp->appsrt != NULL) {
        psAttachParticleAppSRT(pp, gp->appsrt);
    }

    pp->bank = bank;
    pp->linkNo = linkNo;
    pp->kind = kind;
    pp->texGroup = texGroup;
    pp->pos.x = x;
    pp->pos.y = y;
    pp->pos.z = z;
    pp->vel.x = vx;
    pp->vel.y = vy;
    pp->vel.z = vz;
    pp->size = size;
    pp->grav = grav;
    pp->fric = fric;
    pp->life = life + 1;
    pp->cmdList = list;
    pp->cmdMarkPtr = 0;
    pp->cmdPtr = 0;

    if (palflag != 0) {
        pp->kind |= 0x10;
    }

    pp->cmdWait = list != NULL;
    pp->poseNum = 0;
    pp->palNum = 0xFF;
    pp->primCol.a = 0xFF;
    pp->primCol.b = 0xFF;
    pp->primCol.g = 0xFF;
    pp->primCol.r = 0xFF;
    pp->envCol.a = 0;
    pp->envCol.b = 0;
    pp->envCol.g = 0;
    pp->envCol.r = 0;
    pp->envColCount = 0;
    pp->primColCount = 0;
    pp->sizeCount = 0;
    pp->envColRemain = 0;
    pp->primColRemain = 0;
    pp->aCmpMode = 0x33;
    if (((pp->kind >> 22) & 3) >= 2) {
        pp->aCmpParam1 = 0;
    } else {
        pp->aCmpParam1 = 1;
    }
    pp->aCmpParam2 = 0xFF;
    pp->aCmpRemain = 0;
    pp->aCmpCount = 0;
    pp->rotateCount = 0;
    pp->x68 = lbl_8047D5B4;
    pp->rotateTarget = lbl_8047D5B4;
    pp->rotate = lbl_8047D5B4;
    pp->gen = gp;
    if (gp != NULL) {
        gp->numChild++;
    }

    pp->pJObjOfs = 0;
    pp->matColRemain = 0;
    pp->matColCount = 0;
    pp->matRGB = 0xFF;
    pp->matA = 0xFF;
    pp->ambColRemain = 0;
    pp->ambColCount = 0;
    pp->ambRGB = 0xFF;
    pp->ambA = 0xFF;
    pp->trail = lbl_8047D5B8;

    if (flgInterpret != 0) {
        psInterpretParticle0(pp, NULL);
    }
    return pp;
}
