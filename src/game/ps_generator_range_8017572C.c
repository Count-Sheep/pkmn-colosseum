/**
 * @file ps_generator_range_8017572C.c
 * @brief ps* -- particle generator pool: kill/remove/init and id allocation
 *        (0x8017572C - 0x80175F6C).
 *
 * This is the tail of HAL's particle generator module (Melee's
 * sysdolphin/baselib/generator.c: psGetNewIDNum = hsd_8039D1EC,
 * genPosUpdate = hsd_8039D214, psInitGenerator = hsd_8039D354, the list
 * removal helper hsd_8039D3AC, psKillGenerator = hsd_8039D4DC), in the
 * Genius Sonority fork: generators are recycled through a free list
 * (lbl_8047B18C) instead of HSD_ObjFree, and removal no longer unrefs a JObj.
 *
 * Compiler: GC/1.3.2 -O4,p with -inline auto,deferred. The functions are
 * written in HAL's (Melee's) source order, the reverse of their addresses:
 *   - deferred inlining emits the unit in reverse definition order, which is
 *     what puts psKillAllGenerator first and psGetNewIDNum last;
 *   - psKillAllGenerator auto-inlines psKillGenerator (both are also emitted
 *     standalone). GC/1.3 does not auto-inline a body that large; GC/1.3.2
 *     does, and every function below compiles identically under 1.3.2 and
 *     2.0;
 *   - psRemoveGenerator inlines psKillAllGenerator, which is defined after it,
 *     so the inlining is deferred.
 *
 * psDeleteGenerator is Melee's hsd_8039D3AC. Colosseum has no standalone
 * copy; it is expanded in psKillGenerator, psKillGeneratorID and (through
 * psKillGenerator) psKillAllGenerator and psRemoveGenerator.
 *
 * The exact runs are linked from their own units:
 *   game/ps_generator_exact_801758D8.c  psKillGeneratorID, psKillGenerator
 *   game/ps_generator_exact_80175DF0.c  psInitGenerator, genPosUpdate,
 *                                       psGetNewIDNum
 * psKillAllGenerator (98.5%: gen/next/prev colour as r30/r29/r31 where retail
 * has r31/r30/r29) and psRemoveGenerator stay candidates. Retail's
 * psRemoveGenerator also inlines psSetBillboardCamera(NULL) (0x80173624),
 * which lives with the particle interpreter; the candidate spells it out.
 *
 * Unit selection: PS_GENERATOR_SPLIT restricts the build to the groups
 * named by PS_GENERATOR_INIT, PS_GENERATOR_REMOVE, PS_GENERATOR_KILL and
 * PS_GENERATOR_KILLALL.
 */

#include "dolphin/types.h"
#include "hsd/hsd_object.h"

#if !defined(PS_GENERATOR_SPLIT)
#define PS_GENERATOR_INIT
#define PS_GENERATOR_REMOVE
#define PS_GENERATOR_KILL
#define PS_GENERATOR_KILLALL
#endif

typedef struct GenPosJObj {
    u8 pad00[0x14];
    u32 flags;
    u8 pad18[0x2C];
    f32 matrix[3][4];
} GenPosJObj;

typedef struct GenPosGenerator {
    u8 pad00[0x20];
    f32 positionX;
    f32 positionY;
    f32 positionZ;
    u8 pad2C[0x5C];
    u16 flags;
    u8 pad8A[0x1A];
    GenPosJObj* jobj;
} GenPosGenerator;

typedef struct psAppSRT {
    struct psAppSRT* next;
    struct psGenerator* gp; /* owning generator */
    u8 pad08[0x2A];
    u16 usedCount;
} psAppSRT;

typedef struct psGenerator {
    /* 0x00 */ struct psGenerator* next;
    /* 0x04 */ u32 flags;
    /* 0x08 */ f32 random;
    /* 0x0C */ u8 pad0C[4];
    /* 0x10 */ u16 genLife;
    /* 0x12 */ u16 type;
    /* 0x14 */ u8 pad14[4];
    /* 0x18 */ u16 idnum;
    /* 0x1A */ u8 pad1A[0x32];
    /* 0x4C */ u32 numChild;
    /* 0x50 */ psAppSRT* appsrt;
    /* 0x54 */ u8 pad54[0x60];
} psGenerator; /* 0xB4 */

extern void* memset(void* dst, int val, u32 size);
extern void* fn_801A6928(s32 size); /* HSD_MemAlloc */
extern void fn_801A6960(void* p);   /* HSD_Free */
extern void* fn_801A3E64(void* list);
extern void fn_8019D9DC(GenPosJObj* jobj); /* HSD_JObjSetupMatrixSub */
extern void __assert(const char* file, u32 line, const char* condition);
extern void psKillAllParticle(void);
extern void psKillGeneratorChild(psGenerator* gen);
extern s32 psRemoveGeneratorAppSRT(psGenerator* gen);

extern const f32 lbl_8047D6B0; /* 0.0f */
extern const char lbl_8047D6E0[7]; /* "jobj.h" */
extern const char lbl_8047D6E8[5]; /* "jobj" */
extern u16 lbl_80478C38;       /* last generator id, starts at 0x100 */
extern u16 lbl_8047B112;
extern u16 lbl_8047B118;       /* active generator count */
extern u32 lbl_8047B180;       /* pending generator list */
extern psGenerator* lbl_8047B184; /* list cursor / previous generator */
extern psGenerator* lbl_8047B188; /* active generator list */
extern psGenerator* lbl_8047B18C; /* free generator list */
extern HSD_Obj* lbl_8047B190;  /* billboard camera (psSetBillboardCamera) */
extern u32 lbl_8047B194;
extern u32 lbl_8047B198;

void psKillGenerator(psGenerator* gen);
void psKillAllGenerator(void);

#if defined(PS_GENERATOR_INIT)

static inline s32 genPosJObjMtxIsDirty(GenPosJObj* jobj)
{
    s32 result;

    if (jobj == NULL) {
        __assert(lbl_8047D6E0, 0x25D, lbl_8047D6E8);
    }
    result = FALSE;
    if (!(jobj->flags & 0x800000) && (jobj->flags & 0x40)) {
        result = TRUE;
    }
    return result;
}

static inline void genPosJObjSetupMatrix(GenPosJObj* jobj)
{
    if (jobj == NULL || !genPosJObjMtxIsDirty(jobj)) {
        return;
    }
    fn_8019D9DC(jobj);
}

u16 psGetNewIDNum(void)
{
    lbl_80478C38++;
    if (lbl_80478C38 < 256) {
        lbl_80478C38 = 256;
    }
    return lbl_80478C38;
}

void genPosUpdate(GenPosGenerator* generator)
{
    GenPosJObj* jobj;

    if (generator != NULL && !(generator->flags & 2) &&
        (generator->flags & 1)) {
        jobj = generator->jobj;
        if (jobj != NULL) {
            genPosJObjSetupMatrix(jobj);
            generator->positionX = generator->jobj->matrix[0][3];
            generator->positionY = generator->jobj->matrix[1][3];
            generator->positionZ = generator->jobj->matrix[2][3];
        }
    }
}

void psInitGenerator(s32 count)
{
    s32 i;
    psGenerator* gen;

    lbl_8047B188 = NULL;
    lbl_8047B18C = NULL;
    for (i = count - 1; i >= 0; i--) {
        gen = fn_801A6928(sizeof(psGenerator));
        memset(gen, 0, sizeof(psGenerator));
        if (gen == NULL) {
            return;
        }
        gen->next = lbl_8047B18C;
        lbl_8047B18C = gen;
    }
    lbl_8047B118 = 0;
    lbl_8047B112 = 0;
    lbl_8047B180 = 0;
    lbl_8047B190 = NULL;
    lbl_8047B198 = 0;
    lbl_8047B194 = 0;
    lbl_8047B184 = NULL;
}

#endif /* PS_GENERATOR_INIT */

/*
 * Unlinks gen (whose predecessor is prev) and returns it to the free list,
 * unless children or a shared application SRT still need it: then it is
 * left to expire next frame and becomes the new predecessor.
 */
static inline psGenerator* psDeleteGenerator(psGenerator* gen, psGenerator* prev)
{
    if (gen->type & 0x80) {
        psKillGeneratorChild(gen);
    }
    if (gen->numChild != 0) {
        gen->random = lbl_8047D6B0;
        gen->genLife = 1;
        return gen;
    }
    if (gen->type & 0x3800) {
        psAppSRT* srt = gen->appsrt;

        if (srt != NULL && srt->gp == gen && srt->usedCount != 1) {
            gen->random = lbl_8047D6B0;
            gen->genLife = 1;
            return gen;
        }
    }
    if (prev == NULL) {
        lbl_8047B188 = gen->next;
    } else {
        prev->next = gen->next;
    }
    if (gen->appsrt != NULL) {
        psRemoveGeneratorAppSRT(gen);
    }
    gen->next = lbl_8047B18C;
    lbl_8047B18C = gen;
    lbl_8047B118--;
    return prev;
}

#if defined(PS_GENERATOR_REMOVE)

void psRemoveGenerator(void)
{
    psGenerator* gen;

    psKillAllParticle();
    psKillAllGenerator();

    gen = lbl_8047B18C;
    while (gen != NULL) {
        psGenerator* next = gen->next;

        fn_801A6960(gen);
        gen = next;
    }
    lbl_8047B18C = NULL;

    /*
     * psSetBillboardCamera(NULL), which retail inlines here (0x80173624 is
     * the standalone copy; it lives with the particle interpreter, so this
     * candidate spells the body out).
     */
    {
        HSD_Obj* old = lbl_8047B190;

        if (old != NULL) {
            if (old != NULL && ref_DEC(old)) {
                if (old != NULL) {
                    HSD_CLASS_METHOD(old)->release((HSD_Class*)old);
                    HSD_CLASS_METHOD(old)->destroy((HSD_Class*)old);
                }
            }
            lbl_8047B190 = NULL;
        }
    }
}

#endif /* PS_GENERATOR_REMOVE */

#if defined(PS_GENERATOR_KILL)

void psKillGenerator(psGenerator* gen)
{
    psGenerator* cur;

    cur = lbl_8047B188;
    lbl_8047B184 = NULL;
    while (cur != NULL) {
        if (cur == gen) {
            lbl_8047B184 = psDeleteGenerator(gen, lbl_8047B184);
            if (lbl_8047B184 != NULL) {
                while (lbl_8047B184->next != NULL) {
                    lbl_8047B184 = lbl_8047B184->next;
                }
            } else if (lbl_8047B188 != NULL) {
                lbl_8047B184 = lbl_8047B188;
                while (lbl_8047B184->next != NULL) {
                    lbl_8047B184 = lbl_8047B184->next;
                }
            }
            return;
        }
        lbl_8047B184 = cur;
        cur = cur->next;
    }
}

void psKillGeneratorID(s32 id)
{
    psGenerator* gen = lbl_8047B188;

    lbl_8047B184 = NULL;
    while (gen != NULL) {
        psGenerator* next = gen->next;

        if (gen->idnum == (u16)id) {
            lbl_8047B184 = psDeleteGenerator(gen, lbl_8047B184);
        } else {
            lbl_8047B184 = gen;
        }
        gen = next;
    }
}

#endif /* PS_GENERATOR_KILL */

#if defined(PS_GENERATOR_KILLALL)

void psKillAllGenerator(void)
{
    psGenerator* gen = lbl_8047B188;

    while (gen != NULL) {
        psGenerator* next = gen->next;

        psKillGenerator(gen);
        gen = next;
    }
    while (lbl_8047B180 != 0) {
        lbl_8047B180 = (u32)fn_801A3E64((void*)lbl_8047B180);
    }
}

#endif /* PS_GENERATOR_KILLALL */
