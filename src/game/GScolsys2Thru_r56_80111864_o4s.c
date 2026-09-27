/**
 * @file GScolsys2Thru_r56_80111864_o4s.c
 * @brief fn_80111864, candidate only (score instrumentation chunk).
 *
 * By .sdata2 ownership fn_80111864 belongs to the GScolsys2Check TU: it
 * and GScolsys2CheckGetEventID share the pool 0x8047CF60-0x8047CF68,
 * separate from GScolsys2Thru's 0x8047CF48-0x8047CF60 (which has its
 * own 0.0f/1.0f).
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"

/* 0x80111864 | 0x338 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
s32 fn_80111864(void* a, void* b, void* c) {
#pragma optimization_level 4
    extern f32 PSVECDotProduct(void* a, void* b);
    extern s32 GScolsy2UtilChkInTri(void* a, void* b, void* c);
    extern f32 lbl_8047CF60;
    extern f32 lbl_8047CF64;
    u8* wzx;
    u8* region;
    u8* triList;
    u8* tri;
    u8* tempWrite;
    u8* tempRead;
    u8* out;
    u8* scan;
    s32 outCount;
    s32 outOffset;
    s32 regionIdx;
    s32 tempCount;
    s32 triIdx;
    s32 scanIdx;
    s32 vertIdx;
    s32 visible;
    s32 hit;
    f32 resultT;
    u8 temp[0xD0];
    f32 mtxInv[12];
    f32 mtxFwd[12];
    f32 verts[9];
    f32 dirVec[3];
    f32 planePoint[3];
    f32 hitPoint[3];

    outCount = 0;
    outOffset = 0;
    wzx = (u8*)fn_8010CBC0();
    PSVECSubtract(b, a, dirVec);
    region = *(u8**)wzx;
    regionIdx = 0;
    while ((u32)regionIdx < *(u32*)(wzx + 4)) {
        GScolsys2GetObjEnable(regionIdx, &visible);
        if (visible != 0) {
            triList = *(u8**)(region + 0x30);
            if (triList != NULL) {
                fn_8010CA30(mtxInv, regionIdx);
                fn_8010C8D0(mtxFwd, regionIdx);
                tempWrite = temp;
                tempCount = 0;
                tri = *(u8**)triList;
                triIdx = 0;
                while ((u32)triIdx < *(u32*)(triList + 4)) {
                    scan = temp;
                    scanIdx = 0;
                    if (tempCount > 0) {
                        do {
                            if (*(u16*)(tri + 0x30) == *(u16*)(scan + 0x30)) {
                                break;
                            }
                            scan += 0x34;
                            scanIdx++;
                        } while (scanIdx < tempCount);
                    }
                    if (scanIdx < tempCount) {
                        goto next_triangle;
                    }
                    PSMTXMultVec(mtxFwd, tri + 0x24, planePoint);
                    if (PSVECDotProduct(planePoint, dirVec) >= lbl_8047CF60) {
                        goto next_triangle;
                    }
                    scan = (u8*)verts;
                    vertIdx = 0;
                    do {
                        PSMTXMultVec(mtxInv, tri + (vertIdx * 0xC), scan);
                        vertIdx++;
                        scan += 0xC;
                    } while (vertIdx < 3);
                    if (GScolsys2UtilGetCpPlaneLine((Vec3f*)hitPoint, &resultT,
                                                   (const Vec3f*)planePoint,
                                                   (const Vec3f*)verts,
                                                   (const Vec3f*)a,
                                                   (const Vec3f*)b) == 0) {
                        hit = 0;
                    } else if ((resultT < lbl_8047CF60) || (resultT > lbl_8047CF64)) {
                        hit = 0;
                    } else if (GScolsy2UtilChkInTri(hitPoint, verts, planePoint) == 0) {
                        hit = 0;
                    } else {
                        hit = 1;
                    }
                    if (hit != 0) {
                        *(u32*)(tempWrite + 0x00) = *(u32*)((u8*)verts + 0x00);
                        *(u32*)(tempWrite + 0x04) = *(u32*)((u8*)verts + 0x04);
                        *(u32*)(tempWrite + 0x08) = *(u32*)((u8*)verts + 0x08);
                        *(u32*)(tempWrite + 0x0C) = *(u32*)((u8*)verts + 0x0C);
                        *(u32*)(tempWrite + 0x10) = *(u32*)((u8*)verts + 0x10);
                        *(u32*)(tempWrite + 0x14) = *(u32*)((u8*)verts + 0x14);
                        *(u32*)(tempWrite + 0x18) = *(u32*)((u8*)verts + 0x18);
                        *(u32*)(tempWrite + 0x1C) = *(u32*)((u8*)verts + 0x1C);
                        *(u32*)(tempWrite + 0x20) = *(u32*)((u8*)verts + 0x20);
                        *(u32*)(tempWrite + 0x24) = *(u32*)((u8*)planePoint + 0x00);
                        *(u32*)(tempWrite + 0x28) = *(u32*)((u8*)planePoint + 0x04);
                        *(u32*)(tempWrite + 0x2C) = *(u32*)((u8*)planePoint + 0x08);
                        *(u16*)(tempWrite + 0x30) = *(u16*)(tri + 0x30);
                        tempWrite += 0x34;
                        tempCount++;
                    }
                next_triangle:
                    triIdx++;
                    tri += 0x34;
                    if (tempCount >= 4) {
                        break;
                    }
                }
                tempRead = temp;
                triIdx = 0;
                while (triIdx < tempCount) {
                    scan = (u8*)c;
                    scanIdx = 0;
                    if (outCount > 0) {
                        do {
                            if (*(u16*)(scan + 0x30) == *(u16*)(tempRead + 0x30)) {
                                break;
                            }
                            scan += 0x34;
                            scanIdx++;
                        } while (scanIdx < outCount);
                    }
                    if (scanIdx < outCount) {
                        goto next_temp;
                    }
                    out = (u8*)c + outOffset;
                    *(u32*)(out + 0x00) = *(u32*)(tempRead + 0x00);
                    *(u32*)(out + 0x04) = *(u32*)(tempRead + 0x04);
                    *(u32*)(out + 0x08) = *(u32*)(tempRead + 0x08);
                    *(u32*)(out + 0x0C) = *(u32*)(tempRead + 0x0C);
                    *(u32*)(out + 0x10) = *(u32*)(tempRead + 0x10);
                    *(u32*)(out + 0x14) = *(u32*)(tempRead + 0x14);
                    *(u32*)(out + 0x18) = *(u32*)(tempRead + 0x18);
                    *(u32*)(out + 0x1C) = *(u32*)(tempRead + 0x1C);
                    *(u32*)(out + 0x20) = *(u32*)(tempRead + 0x20);
                    *(u32*)(out + 0x24) = *(u32*)(tempRead + 0x24);
                    *(u32*)(out + 0x28) = *(u32*)(tempRead + 0x28);
                    *(u32*)(out + 0x2C) = *(u32*)(tempRead + 0x2C);
                    *(u32*)(out + 0x30) = *(u32*)(tempRead + 0x30);
                    outCount++;
                    outOffset += 0x34;
                next_temp:
                    tempRead += 0x34;
                    triIdx++;
                    if (outCount >= 4) {
                        break;
                    }
                }
            }
        }
        regionIdx++;
        region += 0x40;
        if (outCount >= 4) {
            break;
        }
    }
    return outCount;
}
#pragma pop
