/**
 * @file ps_generator_candidate_80173718.c
 * @brief psCreateGeneratorID (0x80173718 - 0x80173F98).
 *
 * HAL's generator.c psCreateGeneratorID (Melee: hsd_8039F05C) in the Genius
 * Sonority fork: 64 banks, the generator taken from the free list, the
 * record's graphics index kept at +0x8A, and the position/rotation/scale
 * block at +0x88..+0xB0 initialised at the end. Built like the generator
 * tail (GC/1.3.2 -O4,p, -inline auto,deferred). psGeneratorAlloc is the
 * fork's version of Melee's hsd_8039D9C8 (pop or allocate a generator, link
 * it after the list cursor, give it a fresh id), auto-inlined here; its
 * standalone copy is dead-stripped.
 *
 * Every instruction matches retail; what keeps this a candidate is data
 * ownership. The float constants are literals (0.0f, 1.0f, 2 pi, +-pi/2 and
 * MSL sqrtf's 0.5/3.0/0.0), and MWCC colours literal-pool loads differently
 * from loads of extern constants (with the constants declared as extern
 * symbols the zero/one pair and the first sqrtf swap registers, 99.65%).
 * The literals are generator.c's .sdata2 pool (0x8047D6B0 - 0x8047D6E0),
 * which psExecGenerator, generateParticle and the kill functions also read,
 * so it can only be owned once the whole generator.c unit is exact.
 * The symbols keep their address names.
 */
#include "dolphin/types.h"
#include "crt/float.h"
#include "crt/math_ppc.h"

typedef struct PSAppSRT {
    u8 pad00[0x4];
    struct psGenerator* gp; /* 0x04 */
    u8 pad08[0x6A];
    u8 isGenerator;         /* 0x72 */
} PSAppSRT;

typedef struct PSCmdList {
    u16 type;     /* 0x00 */
    u16 texGroup; /* 0x02 */
    u16 genLife;  /* 0x04 */
    u16 life;     /* 0x06 */
    u32 kind;     /* 0x08 */
    f32 grav;     /* 0x0C */
    f32 fric;     /* 0x10 */
    f32 vx;       /* 0x14 */
    f32 vy;       /* 0x18 */
    f32 vz;       /* 0x1C */
    f32 radius;   /* 0x20 */
    f32 angle;    /* 0x24 */
    f32 random;   /* 0x28 */
    f32 size;     /* 0x2C */
    f32 param1;   /* 0x30 */
    f32 param2;   /* 0x34 */
    f32 param3;   /* 0x38 */
    u8 cmdList[1]; /* 0x3C */
} PSCmdList;

typedef struct PSTexGroup {
    u8 pad00[0x16];
    u16 palflag; /* 0x16 */
} PSTexGroup;

typedef struct psGenerator {
    struct psGenerator* next; /* 0x00 */
    u32 kind;                 /* 0x04 */
    f32 random;               /* 0x08 */
    f32 count;                /* 0x0C */
    u16 genLife;              /* 0x10 */
    u16 type;                 /* 0x12 */
    u8 bank;                  /* 0x14 */
    u8 linkNo;                /* 0x15 */
    u8 texGroup;              /* 0x16 */
    u8 x17;                   /* 0x17 */
    u16 idnum;                /* 0x18 */
    u16 life;                 /* 0x1A */
    u8* cmdList;              /* 0x1C */
    f32 posX;                 /* 0x20 */
    f32 posY;                 /* 0x24 */
    f32 posZ;                 /* 0x28 */
    f32 velX;                 /* 0x2C */
    f32 velY;                 /* 0x30 */
    f32 velZ;                 /* 0x34 */
    f32 grav;                 /* 0x38 */
    f32 fric;                 /* 0x3C */
    f32 size;                 /* 0x40 */
    f32 radius;               /* 0x44 */
    f32 angle;                /* 0x48 */
    u32 numChild;             /* 0x4C */
    PSAppSRT* appsrt;         /* 0x50 */
    union {
        struct {
            f32 minAngle; /* 0x54 */
            f32 maxAngle; /* 0x58 */
            f32 height;   /* 0x5C */
        } cone;
        struct {
            f32 x2; /* 0x54 */
            f32 y2; /* 0x58 */
            f32 z2; /* 0x5C */
        } line;
        struct {
            f32 x, y, z;    /* 0x54 */
            f32 xx, xy, xz; /* 0x60 */
            f32 yx, yy, yz; /* 0x6C */
            f32 zx, zy, zz; /* 0x78 */
            u16 flag;       /* 0x84 */
        } rect;
        struct {
            f32 speed;    /* 0x54 */
            f32 latMid;   /* 0x58 */
            f32 latRange; /* 0x5C */
            f32 lonMid;   /* 0x60 */
            f32 lonRange; /* 0x64 */
        } sphere;
    } aux;
    u16 posFlags;             /* 0x88 */
    u16 gfxIdx;               /* 0x8A */
    f32 rotX, rotY, rotZ;     /* 0x8C */
    f32 sclX, sclY, sclZ;     /* 0x98 */
    void* jobj;               /* 0xA4 */
    f32 sclX2, sclY2, sclZ2;  /* 0xA8 */
} psGenerator; /* 0xB4 */

extern void* memset(void* dst, int val, u32 size);
extern void* fn_801A6928(u32 size); /* HSD_MemAlloc */
extern f32 fn_801ADC7C(void);       /* HSD_Randf */
extern PSAppSRT* psAddGeneratorAppSRT(psGenerator* gen, u8 type);

extern u16 lbl_80478C38;          /* last generator id */
extern u16 lbl_8047B112;          /* peak generator count */
extern u16 lbl_8047B118;          /* live generator count */
extern psGenerator* lbl_8047B184; /* list cursor */
extern psGenerator* lbl_8047B188; /* active generators */
extern psGenerator* lbl_8047B18C; /* free generators */
extern void (*lbl_8047B194)(psGenerator* gen); /* custom-type callback */

extern PSTexGroup** lbl_804529C8[64]; /* per-bank texture groups */
extern PSCmdList** lbl_80452AC8[64];  /* per-bank generator records */
extern s32 lbl_80452CC8[64];          /* per-bank record count */

psGenerator* psGeneratorAlloc(void)
{
    psGenerator* gen;

    if (lbl_8047B18C == NULL) {
        lbl_8047B18C = fn_801A6928(sizeof(psGenerator));
        memset(lbl_8047B18C, 0, sizeof(psGenerator));
    }
    gen = lbl_8047B18C;
    if (gen == NULL) {
        return NULL;
    }

    lbl_8047B118++;
    if (lbl_8047B118 > lbl_8047B112) {
        lbl_8047B112 = lbl_8047B118;
    }
    lbl_8047B18C = gen->next;

    if (lbl_8047B184 == NULL || lbl_8047B184->next == NULL) {
        if (lbl_8047B188 == NULL) {
            gen->next = NULL;
            lbl_8047B188 = gen;
        } else {
            gen->next = lbl_8047B188->next;
            lbl_8047B188->next = gen;
        }
    } else {
        gen->next = lbl_8047B184->next->next;
        lbl_8047B184->next->next = gen;
    }

    lbl_80478C38++;
    if (lbl_80478C38 < 0x100) {
        lbl_80478C38 = 0x100;
    }
    gen->idnum = lbl_80478C38;
    gen->appsrt = NULL;
    gen->x17 = 0;
    return gen;
}

psGenerator* psCreateGeneratorID(s32 linkNo, s32 bank, s32 idx)
{
    psGenerator* gen;

    if (bank >= 64) {
        return NULL;
    }
    if (linkNo >= 8) {
        return NULL;
    }
    if (idx >= lbl_80452CC8[bank]) {
        return NULL;
    }
    if (lbl_80452AC8[bank][idx] == NULL) {
        return NULL;
    }

    gen = psGeneratorAlloc();
    if (gen != NULL) {
        gen->type = lbl_80452AC8[bank][idx]->type;
        gen->bank = bank;
        gen->linkNo = linkNo;
        gen->gfxIdx = idx;
        gen->kind = lbl_80452AC8[bank][idx]->kind;
        gen->texGroup = lbl_80452AC8[bank][idx]->texGroup;
        gen->life = lbl_80452AC8[bank][idx]->life;
        gen->genLife = lbl_80452AC8[bank][idx]->genLife;
        gen->posZ = 0.0f;
        gen->posY = 0.0f;
        gen->posX = 0.0f;
        gen->velX = lbl_80452AC8[bank][idx]->vx;
        gen->velY = lbl_80452AC8[bank][idx]->vy;
        gen->velZ = lbl_80452AC8[bank][idx]->vz;
        gen->grav = lbl_80452AC8[bank][idx]->grav;
        gen->fric = lbl_80452AC8[bank][idx]->fric;
        gen->size = lbl_80452AC8[bank][idx]->size;
        gen->cmdList = lbl_80452AC8[bank][idx]->cmdList;
        gen->radius = lbl_80452AC8[bank][idx]->radius;
        gen->angle = lbl_80452AC8[bank][idx]->angle;
        gen->random = lbl_80452AC8[bank][idx]->random;

        if (gen->kind & 0x100) {
            if (gen->random < 0.0f) {
                gen->count = (1.0f + gen->random > FLT_EPSILON) ? 1.0f : 0.0f;
            } else {
                gen->count = 1.0f - FLT_EPSILON;
            }
        } else if (gen->random < 0.0f) {
            gen->count = 0.0f;
        } else {
            gen->count = fn_801ADC7C();
        }

        {
            PSTexGroup* tg = lbl_804529C8[bank][gen->texGroup];

            if (tg != NULL && tg->palflag != 0) {
                gen->kind |= 0x10;
            }
        }

        gen->numChild = 0;

        switch (gen->type & 0xF) {
        case 0:
        case 3:
        case 4: {
            PSCmdList* c = lbl_80452AC8[bank][idx];

            if (0.0f == c->param1 && 0.0f == c->param2) {
                gen->aux.cone.minAngle = 0.0f;
                gen->aux.cone.maxAngle = 6.2831855f;
            } else {
                gen->aux.cone.minAngle = c->param1;
                gen->aux.cone.maxAngle = lbl_80452AC8[bank][idx]->param2;
            }
            break;
        }
        case 1:
            gen->aux.line.x2 = lbl_80452AC8[bank][idx]->param1;
            gen->aux.line.y2 = lbl_80452AC8[bank][idx]->param2;
            gen->aux.line.z2 = lbl_80452AC8[bank][idx]->param3;
            break;
        case 2:
            break;
        case 6:
        case 7: {
            PSCmdList* c = lbl_80452AC8[bank][idx];

            if (0.0f == c->param1 && 0.0f == c->param2) {
                gen->aux.cone.minAngle = 0.0f;
                gen->aux.cone.maxAngle = 6.2831855f;
            } else {
                gen->aux.cone.minAngle = c->param1;
                gen->aux.cone.maxAngle = lbl_80452AC8[bank][idx]->param2;
            }
            gen->aux.cone.height = lbl_80452AC8[bank][idx]->param3;
            break;
        }
        case 5:
            gen->aux.rect.xx = gen->aux.rect.x = lbl_80452AC8[bank][idx]->param1;
            gen->aux.rect.yy = gen->aux.rect.y = lbl_80452AC8[bank][idx]->param2;
            gen->aux.rect.zz = gen->aux.rect.z = lbl_80452AC8[bank][idx]->param3;
            gen->aux.rect.zy = 0.0f;
            gen->aux.rect.zx = 0.0f;
            gen->aux.rect.yz = 0.0f;
            gen->aux.rect.yx = 0.0f;
            gen->aux.rect.xz = 0.0f;
            gen->aux.rect.xy = 0.0f;
            gen->aux.rect.flag = 0;
            if (lbl_80452AC8[bank][idx]->param1 < 0.0f) {
                gen->aux.rect.flag |= 1;
            }
            if (lbl_80452AC8[bank][idx]->param2 < 0.0f) {
                gen->aux.rect.flag |= 2;
            }
            if (lbl_80452AC8[bank][idx]->param3 < 0.0f) {
                gen->aux.rect.flag |= 4;
            }
            break;
        case 8: {
            f32 mag;

            gen->aux.sphere.speed = sqrtf(gen->velZ * gen->velZ +
                                               (gen->velX * gen->velX + gen->velY * gen->velY));
            mag = sqrtf(gen->velX * gen->velX + gen->velZ * gen->velZ);
            if (mag < FLT_MIN) {
                if (gen->velY >= 0.0f) {
                    gen->aux.sphere.latMid = 1.5707964f;
                } else {
                    gen->aux.sphere.latMid = -1.5707964f;
                }
            } else {
                gen->aux.sphere.latMid = atan2f(gen->velY, mag);
            }
            if (fabs(gen->velX) < FLT_MIN) {
                if (gen->velZ >= 0.0f) {
                    gen->aux.sphere.lonMid = 1.5707964f;
                } else {
                    gen->aux.sphere.lonMid = -1.5707964f;
                }
            } else {
                gen->aux.sphere.lonMid = atan2f(gen->velZ, gen->velX);
            }
            gen->aux.sphere.latRange = lbl_80452AC8[bank][idx]->param1;
            if (gen->aux.sphere.latRange < 0.0f) {
                gen->aux.sphere.latRange = -gen->aux.sphere.latRange;
                gen->aux.sphere.speed = -gen->aux.sphere.speed;
            }
            gen->aux.sphere.lonRange = lbl_80452AC8[bank][idx]->param2;
            break;
        }
        default:
            if (lbl_8047B194 != NULL) {
                lbl_8047B194(gen);
            }
            break;
        }

        if (gen->kind & 0x20000) {
            gen->type |= 0x800;
            psAddGeneratorAppSRT(gen, 0);
            if (gen->appsrt != NULL) {
                gen->appsrt->isGenerator = 1;
                gen->appsrt->gp = gen;
            }
        }

        gen->posFlags = 2;
        gen->jobj = NULL;
        gen->rotX = gen->rotY = gen->rotZ = 0.0f;
        gen->sclX = gen->sclY = gen->sclZ = 1.0f;
        gen->sclX2 = gen->sclY2 = gen->sclZ2 = 1.0f;
    }
    return gen;
}
