/**
 * @file floor_range_80111DF8.c
 * @brief Floor map resource/state helpers ahead of floor.c (candidate only).
 *
 * Address range: 0x80111DF8 - 0x80112380 (fn_80111DF8, fn_80111F2C,
 * fn_8011207C, fn_80112260).
 *
 * These four functions read floor.c's .rodata (lbl_802720B0, inside
 * floor.c's 0x80272088-0x802721FA) and call the floorDataBios and
 * floorRead helpers, and in XD floor.o directly follows GScolsys2Sun.o
 * (NXXJ01.map lines 6934-6935, StarsMmd/Colo-XD-PBR-symbol-maps @
 * 6b51d3a), so they are the start of the floor TU. GScolsys2Sun, which
 * used to share this candidate range, is linked on its own
 * (GScolsys2Sun.c). None of the four is exact yet; they stay a
 * CodeCandidate built with floor.c's -opt nopeephole (the mr rX,r3 +
 * cmplwi rX,0 pairs show the peephole off).
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"
#include "hsd/hsd_archive.h"

typedef struct FloorArchiveResourceLists {
    u32* models;
    u32 unused04;
    u32* lights;
} FloorArchiveResourceLists;

extern const char lbl_802720B0[];
extern void* GSresGetResource(u32 groupId, u32 resourceId);
extern void* floorDataBiosGetCurrentPtr(void);
extern u32 floorDataBiosGetMapResID(void*);
extern u32 floorDataBiosGetGroupID(void*);
extern u32 floorReadMakeModelResID(u32);
extern u32 floorReadMakeLightResID(u32);
extern void GSmodelPushState(void*, void*);
extern void GSlightPushState(void*, void*);
extern void GSmodelSetVisibility(void*, u8);
extern void GSlightSetActive(void*, u8);

/* 0x80111DF8 | 0x134 */
u32 fn_80111DF8(void) {
    FloorArchiveResourceLists* lists;
    void* floor;
    u32 mapId;
    u32 groupId;
    u32 baseId;
    u32 modelCount;
    u32 lightCount;
    u32 modelIndex;
    u32 lightIndex;

    modelCount = 0;
    lightCount = 0;
    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    if (GSresGetResource(groupId,
                                          baseId | modelIndex) != NULL) {
                        modelCount++;
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    if (GSresGetResource(groupId,
                                          baseId | lightIndex) != NULL) {
                        lightCount++;
                    }
                }
            }
        }
    }
    return (modelCount + lightCount) * 0x74;
}

/* 0x80111F2C | 0x150 */
void fn_80111F2C(u8* state) {
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 resourceId;
    u32 baseId;
    u32 modelIndex;
    u32 lightIndex;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    resourceId = baseId | modelIndex;
                    resource = GSresGetResource(groupId, resourceId);
                    if (resource != NULL) {
                        *(u32*)(state + 0) = resourceId;
                        *(u32*)(state + 4) = 1;
                        GSmodelPushState(resource, state + 8);
                        state += 0x74;
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    resourceId = baseId | lightIndex;
                    resource = GSresGetResource(groupId, resourceId);
                    if (resource != NULL) {
                        *(u32*)(state + 0) = resourceId;
                        *(u32*)(state + 4) = 2;
                        GSlightPushState(resource, state + 8);
                        state += 0x74;
                    }
                }
            }
        }
    }
}

/* 0x8011207C | 0x1E4 */
void fn_8011207C(u8* state, u32 stateSize) {
    extern void GSmodelPopState(void*, void*);
    extern void GSlightPopState(void*, void*);
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 resourceId;
    u32 baseId;
    u32 stateCount = stateSize / 0x74;
    u32 modelIndex;
    u32 lightIndex;
    u32 j;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId == 0) {
        return;
    }
    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    lists = HSD_ArchiveGetPublicAddress(
        GSresGetResource(groupId, mapId), lbl_802720B0);
    if (lists == NULL) {
        return;
    }
    if (lists->models != NULL) {
        baseId = floorReadMakeModelResID(mapId);
        for (; lists->models[modelIndex] != 0; modelIndex++) {
            resourceId = baseId | modelIndex;
            resource = GSresGetResource(groupId, resourceId);
            if (resource != NULL) {
                u8* record = state;
                for (j = 0; j < stateCount; j++, record += 0x74) {
                    if (*(u32*)record == resourceId) {
                        switch (*(u32*)(record + 4)) {
                        case 1:
                            GSmodelPopState(resource, record + 8);
                            break;
                        case 2:
                            GSlightPopState(resource, record + 8);
                            break;
                        }
                        break;
                    }
                }
            }
        }
    }
    if (lists->lights != NULL) {
        baseId = floorReadMakeLightResID(mapId);
        for (; lists->lights[lightIndex] != 0; lightIndex++) {
            resourceId = baseId | lightIndex;
            resource = GSresGetResource(groupId, resourceId);
            if (resource != NULL) {
                u8* record = state;
                for (j = 0; j < stateCount; j++, record += 0x74) {
                    if (*(u32*)record == resourceId) {
                        switch (*(u32*)(record + 4)) {
                        case 1:
                            GSmodelPopState(resource, record + 8);
                            break;
                        case 2:
                            GSlightPopState(resource, record + 8);
                            break;
                        }
                        break;
                    }
                }
            }
        }
    }
}

/* 0x80112260 | 0x120 */
void fn_80112260(s32 visible) {
    FloorArchiveResourceLists* lists;
    void* floor;
    void* resource;
    u32 mapId;
    u32 groupId;
    u32 baseId;
    u32 modelIndex;
    u32 lightIndex;

    modelIndex = 0;
    lightIndex = 0;
    floor = floorDataBiosGetCurrentPtr();
    mapId = floorDataBiosGetMapResID(floor);
    if (mapId != 0) {
        floor = floorDataBiosGetCurrentPtr();
        groupId = floorDataBiosGetGroupID(floor);
        lists = HSD_ArchiveGetPublicAddress(
            GSresGetResource(groupId, mapId), lbl_802720B0);
        if (lists != NULL) {
            if (lists->models != NULL) {
                baseId = floorReadMakeModelResID(mapId);
                for (; lists->models[modelIndex] != 0; modelIndex++) {
                    resource = GSresGetResource(groupId, baseId | modelIndex);
                    if (resource != NULL) {
                        GSmodelSetVisibility(resource, (u8)visible);
                    }
                }
            }
            if (lists->lights != NULL) {
                baseId = floorReadMakeLightResID(mapId);
                for (; lists->lights[lightIndex] != 0; lightIndex++) {
                    resource = GSresGetResource(groupId, baseId | lightIndex);
                    if (resource != NULL) {
                        GSlightSetActive(resource, (u8)visible);
                    }
                }
            }
        }
    }
}
