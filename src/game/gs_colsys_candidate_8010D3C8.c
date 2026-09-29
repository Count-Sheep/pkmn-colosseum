/**
 * @file gs_colsys_candidate_8010D3C8.c
 * @brief GScolsys2 debug draw, 0x8010D3C8 - 0x8010DE00 (candidate only;
 *        not linked).
 *
 * The tail of the GScolsys2 core TU (pool 0x8047CEB8 - 0x8047CEE0). It
 * matches XD's GScolsys2Draw.o (NXXJ01.map: drawHitMdl 0x134, drawWalkMdl
 * 0x1E8, makeDisplayListFixedObj, GScolsys2Draw; drawSunMdl / drawCheckMdl
 * / drawThruMdl UNUSED 0xCC each). drawWalkMdl, fn_8010D20C, is linked on
 * its own (gs_colsys_exact_8010D20C.c, with the pool's floats
 * 0x8047CEC8 - 0x8047CEE0). GScolsys2Draw here records the fixed objects'
 * display list (XD makeDisplayListFixedObj), and fn_8010D8D4 draws the rest
 * and replays it (XD GScolsys2Draw). The edge and face helpers take XD's
 * names; each face helper has its own colour.
 *
 * Status (lane D10): GScolsys2Draw 96.2%, fn_8010D8D4 93.4% (register
 * order, one colour byte not hoisted in the face loops). See
 * docs/recon/gs_colsys_draw_d10.md.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern GSColSysState lbl_80404C68;
extern s32 GScolsys2GetObjEnable(s32 index, s32* enabled);
extern void* memset(void*, int, unsigned long);

typedef f32 ColVec3[3];
typedef f32 ColMtx[3][4];

typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;

typedef struct ColTri {
    /* 0x00 */ Vec3f verts[3];
    /* 0x24 */ Vec3f normal;
    /* 0x30 */ u8 surface : 4;
    /* 0x30 */ u8 attr : 4;
    /* 0x31 */ u8 layer : 4;
    /* 0x31 */ u8 subLayer : 4;
    /* 0x32 */ u16 id;
} ColTri;

typedef struct ColTriGroup {
    ColTri* tris;
    u32 count;
} ColTriGroup;

typedef struct ColDrawGroup {
    u8* data;
    u32 count;
} ColDrawGroup;

typedef struct ColDrawObject {
    u8 pad_00[0x24];
    void* model;
    ColDrawGroup* edgeGroup0;
    ColDrawGroup* faceGroup0;
    ColDrawGroup* faceGroup1;
    ColDrawGroup* edgeGroup1;
    ColDrawGroup* faceGroup2;
    u16 flags;
    u8 pad_3E[2];
} ColDrawObject;

typedef struct ColDrawScene {
    ColDrawObject* objects;
    u32 count;
} ColDrawScene;


extern ColDrawScene* fn_8010CBC0(void);
extern void fn_800DA028(s32);
extern void fn_800D7820(void*);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800DA4C4(s32, s32, s32);
extern void fn_800DA1E8(s32, s32, s32);
extern void fn_800D9ED8(s32);
extern void fn_8010CA30(ColMtx out, u32 index);
extern void fn_8010C8D0(ColMtx out, u32 index);
extern void PSMTXMultVec(ColMtx, const ColVec3, ColVec3);
extern void fn_800D6A00(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32, f32, f32);
extern void fn_800D5CB8(s32, u8, u8, u8, u8);
extern void fn_800D6728(void);
extern void* GScolsys2Draw(void);
extern void GSgfxDLDraw(void*);
extern void fn_800D30AC(void);
extern void fn_8010D20C(ColTriGroup* group, ColMtx matrix, ColMtx normalMatrix);


/* The TU pool at 0x8047CEB8 starts with these four colours. */
static const GXColor sHitColor = {0xFF, 0xFF, 0xFF, 0xFF};
static const GXColor sSunColor = {0xFF, 0x00, 0xFF, 0xC0};
static const GXColor sCheckColor = {0xFF, 0xFF, 0x00, 0xC0};
static const GXColor sThruColor = {0x00, 0xFF, 0xFF, 0xC0};

static inline void drawHitMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed[3];
    u8* element;
    u32 i;
    s32 vertex;
    s32 next;
    GXColor color;

    if (head == NULL) {
        return;
    }
    /* Retail rereads the colour bytes from the stack after every call;
     * only an address-taken copy gives that. */
    *(u32*)&color = *(const u32*)&sHitColor;
    element = head->data;
    for (i = 0; i < head->count; i++, element += 0x34) {
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed[vertex]);
        }
        fn_800D6A00(1);
        for (vertex = 0; vertex < 3; vertex++) {
            next = vertex + 1;
            if (next >= 3) {
                next = 0;
            }
            fn_800D67BC(2);
            fn_800D6680(transformed[vertex][0], transformed[vertex][1],
                        transformed[vertex][2]);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
            fn_800D6680(transformed[next][0], transformed[next][1],
                        transformed[next][2]);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
            fn_800D6728();
        }
    }
}

static inline void drawSunMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u8* element;
    u32 i;
    s32 vertex;
    GXColor color;

    if (head == NULL) {
        return;
    }
    color = sSunColor;
    element = head->data;
    fn_800D6A00(3);
    for (i = 0; i < head->count; i++, element += 0x34) {
        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
        }
        fn_800D6728();
    }
}

static inline void drawCheckMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u8* element;
    u32 i;
    s32 vertex;
    GXColor color;

    if (head == NULL) {
        return;
    }
    color = sCheckColor;
    element = head->data;
    fn_800D6A00(3);
    for (i = 0; i < head->count; i++, element += 0x34) {
        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
        }
        fn_800D6728();
    }
}

static inline void drawThruMdl(ColDrawGroup* head, ColMtx matrix)
{
    ColVec3 transformed;
    u8* element;
    u32 i;
    s32 vertex;
    GXColor color;

    if (head == NULL) {
        return;
    }
    color = sThruColor;
    element = head->data;
    fn_800D6A00(3);
    for (i = 0; i < head->count; i++, element += 0x30) {
        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
        }
        fn_800D6728();
    }
}

/* Record the collision-debug geometry into a display list. */
#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(10000)
void* GScolsys2Draw(void)
{
    extern u8 GSgfxDLBegin(void* buffer, u32 size);
    extern void* GSgfxDLEnd(void);
    extern s32 printf(const char*, ...);
    extern const char lbl_80272050[];
    ColDrawScene* scene;
    ColDrawObject* object;
    ColMtx matrix;
    ColMtx normalMatrix;
    u32 i;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return NULL;
    }

    fn_800DA028(1);
    fn_800D7820(*(void**)((u8*)&lbl_80404C68 + 0x3708));
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800DA4C4(1, 6, 7);
    fn_800DA1E8(1, 2, 1);
    fn_800D9ED8(0);

    if (GSgfxDLBegin(*(void**)((u8*)&lbl_80404C68 + 0x3708), 0x80000) == 0) {
        printf(lbl_80272050);
        return NULL;
    }

    object = scene->objects;
    for (i = 0; i < scene->count; i++, object++) {
        if ((object->flags & 1) != 0) {
            continue;
        }

        fn_8010CA30(matrix, i);
        fn_8010C8D0(normalMatrix, i);
        if (object->model != NULL) {
            fn_8010D20C(object->model, matrix, normalMatrix);
        }

        drawHitMdl(object->edgeGroup0, matrix);
        drawSunMdl(object->faceGroup0, matrix);
        drawCheckMdl(object->faceGroup1, matrix);
        drawHitMdl(object->edgeGroup1, matrix);
        drawThruMdl(object->faceGroup2, matrix);
    }

    return GSgfxDLEnd();
}
#pragma pop

#pragma push
#pragma inline_depth(8)
#pragma inline_max_size(10000)
void fn_8010D8D4(void)
{
    ColDrawScene* scene;
    ColDrawObject* object;
    ColMtx matrix;
    ColMtx normalMatrix;
    u8* state;
    u8* layer;
    void* displayList;
    u32 activeLayer;
    u32 i;

    scene = fn_8010CBC0();
    if (scene == NULL) {
        return;
    }

    fn_800DA028(1);
    state = (u8*)&lbl_80404C68;
    fn_800D7820(*(void**)(state + 0x3708));
    fn_800D88DC(1);
    fn_800D888C(6);
    fn_800DA4C4(1, 6, 7);
    fn_800DA1E8(1, 2, 1);
    fn_800D9ED8(0);

    object = scene->objects;
    activeLayer = *(u32*)(state + 0x3704);
    layer = state + activeLayer * 0xDC0 + 4;
    for (i = 0; i < scene->count; i++, object++, layer += 0x28) {
        if ((*(u16*)(layer + 0x24) & 1) != 0 ||
            (object->flags & 1) == 0) {
            continue;
        }

        fn_8010CA30(matrix, i);
        fn_8010C8D0(normalMatrix, i);
        if (object->model != NULL) {
            fn_8010D20C(object->model, matrix, normalMatrix);
        }

        drawHitMdl(object->edgeGroup0, matrix);
        drawSunMdl(object->faceGroup0, matrix);
        drawCheckMdl(object->faceGroup1, matrix);
        drawHitMdl(object->edgeGroup1, matrix);
        drawThruMdl(object->faceGroup2, matrix);
    }

    displayList = *(void**)(state + 0x370C);
    if (displayList == NULL) {
        displayList = GScolsys2Draw();
        *(void**)(state + 0x370C) = displayList;
    }
    if (displayList != NULL) {
        GSgfxDLDraw(displayList);
        fn_800D30AC();
    }
}
#pragma pop

