/**
 * @file psstructs.h
 * @brief HAL sysdolphin particle structures as Colosseum's fork lays them
 *        out (particle.c, psappsrt.c, psdisp.c, psinterpret.c).
 *
 * Adapted from Melee's sysdolphin/baselib/psstructs.h (doldecomp/melee).
 * The fork differs from Melee's:
 *   - HSD_Particle has one more float after rotateTarget (+0x68), so every
 *     field from primColRemain on is 4 bytes further, and it ends at the
 *     application SRT (0x94 bytes; no userdata or callback);
 *   - HSD_Generator has no user function block or callback; it carries its
 *     own position flags, rotation/scale block, JObj and scale copy after
 *     the shape parameters (0xB4 bytes);
 *   - 64 data banks instead of 65.
 * Every offset below is read from the retail code of the functions that
 * use it.
 */
#ifndef SYSDOLPHIN_BASELIB_PSSTRUCTS_H
#define SYSDOLPHIN_BASELIB_PSSTRUCTS_H

#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "dolphin/mtx.h"
#include "hsd/hsd_forward.h"

#define PS_NUM_LINK 16
#define PS_NUM_BANK 64

/* size: 0x1C */
typedef struct _HSD_PSTexGroup {
    u32 num;         /* 0x00 */
    u32 fmt;         /* 0x04 */
    u32 tlutfmt;     /* 0x08 */
    u32 width;       /* 0x0C */
    u32 height;      /* 0x10 */
    u16 palnum;      /* 0x14 */
    u16 palflag;     /* 0x16 */
    u8* texTable[1]; /* 0x18 */
} HSD_PSTexGroup;

/* size: 0x8 */
typedef struct _HSD_PSFormGroup {
    u32 num;          /* 0x00 */
    u8* formTable[1]; /* 0x04 */
} HSD_PSFormGroup;

/* size: 0x40 */
typedef struct _HSD_PSCmdList {
    u16 type;      /* 0x00 */
    u16 texGroup;  /* 0x02 */
    u16 genLife;   /* 0x04 */
    u16 life;      /* 0x06 */
    u32 kind;      /* 0x08 */
    f32 grav;      /* 0x0C */
    f32 fric;      /* 0x10 */
    f32 vx;        /* 0x14 */
    f32 vy;        /* 0x18 */
    f32 vz;        /* 0x1C */
    f32 radius;    /* 0x20 */
    f32 angle;     /* 0x24 */
    f32 random;    /* 0x28 */
    f32 size;      /* 0x2C */
    f32 param1;    /* 0x30 */
    f32 param2;    /* 0x34 */
    f32 param3;    /* 0x38 */
    u8 cmdList[1]; /* 0x3C */
} HSD_PSCmdList;

struct HSD_Generator;

typedef struct HSD_psAppSRT {
    struct HSD_psAppSRT* next;                      /* 0x00 */
    struct HSD_Generator* gp;                       /* 0x04 */
    Vec translate;                                  /* 0x08 */
    Vec rot;                                        /* 0x14 */
    f32 x20;                                        /* 0x20 */
    Vec scale;                                      /* 0x24 */
    u8 status;                                      /* 0x30 */
    u8 frameNum;                                    /* 0x31 */
    u16 usedCount;                                  /* 0x32 */
    Mtx mmtx;                                       /* 0x34 */
    f32 ssx;                                        /* 0x64 */
    f32 ssy;                                        /* 0x68 */
    void (*freefunc)(struct HSD_psAppSRT* appsrt);  /* 0x6C */
    u16 idnum;                                      /* 0x70 */
    u8 x72;                                         /* 0x72 */
} HSD_psAppSRT;

/* size: 0x94 */
typedef struct HSD_Particle {
    struct HSD_Particle* next;  /* 0x00 */
    u32 kind;                   /* 0x04 */
    u8 bank;                    /* 0x08 */
    u8 texGroup;                /* 0x09 */
    u8 poseNum;                 /* 0x0A */
    u8 palNum;                  /* 0x0B */
    u16 sizeCount;              /* 0x0C */
    u16 primColCount;           /* 0x0E */
    u16 envColCount;            /* 0x10 */
    GXColor primCol;            /* 0x12 */
    GXColor envCol;             /* 0x16 */
    u16 cmdWait;                /* 0x1A */
    u8 loopCount;               /* 0x1C */
    u8 linkNo;                  /* 0x1D */
    u16 idnum;                  /* 0x1E */
    u8* cmdList;                /* 0x20 */
    u16 cmdPtr;                 /* 0x24 */
    u16 cmdMarkPtr;             /* 0x26 */
    u16 cmdLoopPtr;             /* 0x28 */
    u16 life;                   /* 0x2A */
    Vec vel;                    /* 0x2C */
    f32 grav;                   /* 0x38 */
    f32 fric;                   /* 0x3C */
    Vec pos;                    /* 0x40 */
    f32 size;                   /* 0x4C */
    f32 rotate;                 /* 0x50 */
    u16 aCmpCount;              /* 0x54 */
    u8 aCmpMode;                /* 0x56 */
    u8 aCmpParam1;              /* 0x57 */
    u8 aCmpParam2;              /* 0x58 */
    u8 pJObjOfs;                /* 0x59 */
    u16 matColCount;            /* 0x5A */
    u16 ambColCount;            /* 0x5C */
    u16 rotateCount;            /* 0x5E */
    f32 sizeTarget;             /* 0x60 */
    f32 rotateTarget;           /* 0x64 */
    f32 x68;                    /* 0x68 */
    u16 primColRemain;          /* 0x6C */
    u16 envColRemain;           /* 0x6E */
    GXColor primColTarget;      /* 0x70 */
    GXColor envColTarget;       /* 0x74 */
    u16 matColRemain;           /* 0x78 */
    u16 ambColRemain;           /* 0x7A */
    u16 aCmpRemain;             /* 0x7C */
    u8 aCmpParam1Target;        /* 0x7E */
    u8 aCmpParam2Target;        /* 0x7F */
    u8 matRGB;                  /* 0x80 */
    u8 matA;                    /* 0x81 */
    u8 ambRGB;                  /* 0x82 */
    u8 ambA;                    /* 0x83 */
    u8 matRGBTarget;            /* 0x84 */
    u8 matATarget;              /* 0x85 */
    u8 ambRGBTarget;            /* 0x86 */
    u8 ambATarget;              /* 0x87 */
    f32 trail;                  /* 0x88 */
    struct HSD_Generator* gen;  /* 0x8C */
    HSD_psAppSRT* appsrt;       /* 0x90 */
} HSD_Particle;

/* size: 0xB4 */
typedef struct HSD_Generator {
    struct HSD_Generator* next; /* 0x00 */
    u32 kind;                   /* 0x04 */
    f32 random;                 /* 0x08 */
    f32 count;                  /* 0x0C */
    u16 genLife;                /* 0x10 */
    u16 type;                   /* 0x12 */
    u8 bank;                    /* 0x14 */
    u8 linkNo;                  /* 0x15 */
    u8 texGroup;                /* 0x16 */
    u8 x17;                     /* 0x17 */
    u16 idnum;                  /* 0x18 */
    u16 life;                   /* 0x1A */
    u8* cmdList;                /* 0x1C */
    Vec pos;                    /* 0x20 */
    Vec vel;                    /* 0x2C */
    f32 grav;                   /* 0x38 */
    f32 fric;                   /* 0x3C */
    f32 size;                   /* 0x40 */
    f32 radius;                 /* 0x44 */
    f32 angle;                  /* 0x48 */
    u32 numChild;               /* 0x4C */
    HSD_psAppSRT* appsrt;       /* 0x50 */
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
            f32 vel; /* 0x54 */
        } tornado;
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
    u16 posFlags;               /* 0x88 */
    u16 gfxIdx;                 /* 0x8A */
    Vec rot;                    /* 0x8C */
    Vec scale;                  /* 0x98 */
    HSD_JObj* jobj;             /* 0xA4 */
    Vec scale2;                 /* 0xA8 */
} HSD_Generator;

#endif /* SYSDOLPHIN_BASELIB_PSSTRUCTS_H */
