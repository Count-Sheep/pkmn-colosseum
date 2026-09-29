/**
 * @file gs_colsys_exact_8010D20C.c
 * @brief GScolsys2 debug draw: fn_8010D20C (XD drawWalkMdl), 0x8010D20C -
 *        0x8010D3C8, with its .sdata2 literals 0x8047CEC8 - 0x8047CEE0.
 *
 * Draws a walk-mesh triangle group, one triangle fan per triangle, coloured
 * from the triangle's surface, attribute and layer bits. It is the head of
 * XD's GScolsys2Draw.o (NXXJ01.map: drawWalkMdl 0x1E8; StarsMmd/
 * Colo-XD-PBR-symbol-maps), carved out of gs_colsys_candidate_8010D3C8.c
 * and built with that range's flags (GC/1.3 -O3).
 *
 * The bitfields are read into locals (surface, attr, layer) declared
 * between the triangle index and the triangle pointer. That numbering
 * gives the index a low enough colouring degree that it is coloured after
 * the vertex cursor (r26), as in retail; with the bitfields read inline
 * the index takes r30.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern void* memset(void*, int, unsigned long);

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

extern void PSMTXMultVec(ColMtx, const f32*, f32*);
extern void fn_800D6A00(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32, f32, f32);
extern void fn_800D5CB8(s32, u8, u8, u8, u8);
extern void fn_800D6728(void);

void fn_8010D20C(ColTriGroup* group, ColMtx matrix, ColMtx normalMatrix)
{
    u32 i;
    s32 surface;
    s32 attr;
    s32 level;
    s32 layer;
    ColTri* tri;
    s32 v;
    GXColor color;
    Vec3f pos;

    fn_800D6A00(3);
    tri = group->tris;
    for (i = 0; i < group->count; i++, tri++) {
        memset(&color, 0, sizeof(color));
        color.a = 0xC0;
        surface = tri->surface;
        attr = tri->attr;
        color.g = 127.0f * (surface / 15.0f) + 128.0f;
        level = attr + 1;
        if (level >= 16) {
            level = 0;
        }
        color.b = 255.0f * (level / 15.0f);
        layer = tri->layer;
        if (layer > 0) {
            color.r = layer * 4 + 0xC0;
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
