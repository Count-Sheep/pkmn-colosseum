/** Exact debug-menu main loop owner, 0x80133810 - 0x80133BE4. */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

typedef struct DbgMenuWindow {
    u32 field_00;
    u32 key;
    u8 pad_08[0x8C];
    union {
        u16 cursorPosition;
        struct {
            s8 page;
            s8 row;
        } cursor;
    };
} DbgMenuWindow;

void dbgMenuMain(u8 flag)
{
    u32 (*fp)(void);
    u32 result;

    fp = (u32 (*)(void))lbl_80478F88;
    if (fp == NULL) {
        result = 0;
    } else {
        result = fp();
    }
    if (result == 0) {
        return;
    }
    if (lbl_8047AED0 == 0) {
        return;
    }
    if (menuIsCheck(lbl_80478848) != 0 ? 1 : 0) {
        return;
    }
    lbl_8047AED1 = flag;
    do {
        if (_dbgMenuSub__Fl(0) < 0) {
            return;
        }
    } while (lbl_8047AED1 == 1);
}

static inline s32 dbgMenuInvokeCallback(
    EffectUtilEntryCallback callback, s32 valueIndex, DbgMenuWindow* window)
{
    return callback(valueIndex, window->cursor.page + window->cursor.row);
}

static inline s32 dbgMenuGetCursorIndex(DbgMenuWindow* window)
{
    return window->cursor.page + window->cursor.row;
}

static inline s32 dbgMenuGetValidatedLink(s32 valueIndex)
{
    EffectUtilCountFunc countFunc;
    EffectUtilEntryFunc entryFunc;
    EffectUtilEntry* entry;
    s32 count;
    s32 link;

    if (valueIndex <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= valueIndex)) {
        return 0;
    }
    entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
    entry = entryFunc == NULL ? NULL : entryFunc(valueIndex);
    link = entry == NULL ? 0 : entry->link;
    if ((s16)link <= 0 ||
        (countFunc = (EffectUtilCountFunc)lbl_80478F88,
         count = countFunc == NULL ? 0 : countFunc(), count <= (s16)link)) {
        return 0;
    }
    return link;
}

s32 _dbgMenuSub__Fl(s32 offset)
{
    s32 prevOffset;
    s32 result;
    u32 sceneId;
    s32 key;
    s32 valueIndex;
    s32 link;
    s32 callbackResult;
    s32* outValue;
    DbgMenuWindow* obj;
    EffectUtilEntry* entry;
    EffectUtilEntryFunc entryFunc;
    EffectUtilEntryCallback callback;

    prevOffset = offset - 1;
    sceneId = lbl_80478848 + offset;
    result = 0;

    for (;;) {
        if (offset < 0) {
            key = 0;
        } else {
            key = (s32)lbl_80478848 + prevOffset;
        }
        outValue = (s32*)(lbl_8047AEDC + _dbgMenuGetMenuNo__Fl(key) * sizeof(s32));
        valueIndex = menuOpenCustom(sceneId, key, outValue, 0, 1, 0);
        obj = windowSearchID(sceneId);
        if (obj != NULL) {
            *outValue = obj->cursor.page + obj->cursor.row;
        } else {
            *outValue = 0;
        }

        if (valueIndex == -1) {
            if (offset == 0) {
                result = -1;
            }
            break;
        }

        valueIndex = _dbgMenuGetItemNo__FP14tagWINDOW_WORKl(obj, dbgMenuGetCursorIndex(obj));
        link = dbgMenuGetValidatedLink(valueIndex);

        if ((s16)link != 0) {
            entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
            if (entryFunc == NULL) {
                entry = NULL;
            } else {
                entry = entryFunc(valueIndex);
            }
            if (entry == NULL) {
                callback = NULL;
            } else {
                callback = entry->callback;
            }
            if (callback != NULL) {
                callbackResult = dbgMenuInvokeCallback(callback, valueIndex, obj);
            } else {
                callbackResult = 1;
            }
            if (callbackResult == 0) {
                menuClose(lbl_80478848);
                return 1;
            }
            if (callbackResult == -1 || (s16)link == 1) {
                continue;
            }
            if (_dbgMenuSub__Fl(offset + 1) != 1) {
                continue;
            }
            return 1;
        }
        menuClose(lbl_80478848);
        entryFunc = (EffectUtilEntryFunc)lbl_80478F8C;
        if (entryFunc == NULL) {
            entry = NULL;
        } else {
            entry = entryFunc(valueIndex);
        }
        if (entry == NULL) {
            callback = NULL;
        } else {
            callback = entry->callback;
        }
        if (callback != NULL) {
            callback(valueIndex, dbgMenuGetCursorIndex(obj));
        }
        return 1;
    }
    menuClose(sceneId);
    return result;
}

u32 _dbgMenuGetMenuNum__FP14tagWINDOW_WORKPl(u32 arg0, u32* outMax)
{
    u32 index;
    u32 value;
    u32 max;
    s32 done;

    if (outMax != NULL) {
        *outMax = 0;
    }
    index = 0;
    do {
        value = _dbgMenuGetMsgID__FP14tagWINDOW_WORKl((void*)arg0, index);
        if (outMax != NULL) {
            value = GSmsgGetRect(value) >> 16;
            max = *outMax;
            if ((s32)max < (s32)value) {
                *outMax = value;
            }
        }
        done = _dbgMenuCheckTerminate__FP14tagWINDOW_WORKl((void*)arg0, index++);
    } while (done == 0);
    return index;
}
