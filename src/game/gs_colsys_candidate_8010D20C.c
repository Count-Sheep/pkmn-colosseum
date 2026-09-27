/**
 * @file gs_colsys_candidate_8010D20C.c
 * @brief GScolsys2 debug draw, 0x8010D20C - 0x8010DE00 (candidate only;
 *        not linked).
 *
 * The tail of the GScolsys2 core TU (pool 0x8047CEB8 - 0x8047CEE0). The
 * walk-layer TU that follows is GScolsys2Walk.cpp and the sphere queries
 * are gs_colsys_candidate_8010E53C.c.
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

typedef union ColDrawColor {
    u32 packed;
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    } channel;
} ColDrawColor;

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
extern u32 lbl_8047CEB8;
extern u32 lbl_8047CEBC;
extern u32 lbl_8047CEC0;
extern u32 lbl_8047CEC4;

static inline void ColDrawSetColor(ColDrawColor color)
{
    fn_800D5CB8(0, color.channel.r, color.channel.g,
                color.channel.b, color.channel.a);
}

void fn_8010D20C(ColTriGroup* group, ColMtx matrix, ColMtx normalMatrix)
{
    ColTri* tri;
    u32 i;
    GXColor color;
    Vec3f pos;
    s32 level;
    s32 v;

    fn_800D6A00(3);
    tri = group->tris;
    for (i = 0; i < group->count; i++, tri++) {
        memset(&color, 0, sizeof(color));
        color.a = 0xC0;
        level = tri->surface;
        color.g = 127.0f * (level / 15.0f) + 128.0f;
        level = tri->attr + 1;
        if (level >= 16) {
            level = 0;
        }
        color.b = 255.0f * (level / 15.0f);
        level = tri->layer;
        if (level > 0) {
            color.r = level * 4 + 0xC0;
        }

        fn_800D67BC(3);
        for (v = 0; v < 3; v++) {
            PSMTXMultVec(matrix, &tri->verts[v].x, &pos.x);
            fn_800D6680(pos.x, pos.y, pos.z);
            fn_800D5CB8(0, color.r, color.g, color.b, color.a);
        }
        fn_800D6728();
    }
}

static inline void ColDrawEdges(ColMtx matrix, ColDrawGroup* group,
                                ColDrawColor color, u32 stride)
{
    ColVec3 transformed[3];
    u8* element;
    u32 i;
    s32 vertex;
    s32 next;

    if (group == NULL) {
        return;
    }
    element = group->data;
    for (i = 0; i < group->count; i++, element += stride) {
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
            ColDrawSetColor(color);
            fn_800D6680(transformed[next][0], transformed[next][1],
                        transformed[next][2]);
            ColDrawSetColor(color);
            fn_800D6728();
        }
    }
}

static inline void ColDrawFaces(ColMtx matrix, ColDrawGroup* group,
                                ColDrawColor color, u32 stride)
{
    ColVec3 transformed;
    u8* element;
    u32 i;
    s32 vertex;

    if (group == NULL) {
        return;
    }
    element = group->data;
    fn_800D6A00(3);
    for (i = 0; i < group->count; i++, element += stride) {
        fn_800D67BC(3);
        for (vertex = 0; vertex < 3; vertex++) {
            PSMTXMultVec(matrix, *(ColVec3*)(element + vertex * 12),
                         transformed);
            fn_800D6680(transformed[0], transformed[1], transformed[2]);
            ColDrawSetColor(color);
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
    ColDrawColor color;
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

        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup0, color, 0x34);
        color.packed = lbl_8047CEBC;
        ColDrawFaces(matrix, object->faceGroup0, color, 0x34);
        color.packed = lbl_8047CEC0;
        ColDrawFaces(matrix, object->faceGroup1, color, 0x34);
        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup1, color, 0x34);
        color.packed = lbl_8047CEC4;
        ColDrawFaces(matrix, object->faceGroup2, color, 0x30);
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
    ColDrawColor color;
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

        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup0, color, 0x34);
        color.packed = lbl_8047CEBC;
        ColDrawFaces(matrix, object->faceGroup0, color, 0x34);
        color.packed = lbl_8047CEC0;
        ColDrawFaces(matrix, object->faceGroup1, color, 0x34);
        color.packed = lbl_8047CEB8;
        ColDrawEdges(matrix, object->edgeGroup1, color, 0x34);
        color.packed = lbl_8047CEC4;
        ColDrawFaces(matrix, object->faceGroup2, color, 0x30);
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

