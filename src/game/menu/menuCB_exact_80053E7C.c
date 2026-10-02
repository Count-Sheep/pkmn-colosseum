/**
 * @file menuCB_exact_80053E7C.c
 * @brief Pokemon nickname callback.
 */
#include "dolphin/types.h"

typedef struct MenuCBPane MenuCBPane;

extern void* fn_80057270(MenuCBPane* pane);
extern void* pokemonBiosGetNicknamePtr(void* pokemon);
extern void msgctrlSetValue(s32 id, u32 value);
extern void fn_800FB680(s32 x, s32 y, s32 color, u32 msgId);

s32 fn_80053E7C(MenuCBPane* pane)
{
    void* pokemon;
    void* nickname;

    pokemon = fn_80057270(pane);
    if (pokemon == NULL) {
        return 0;
    }
    nickname = pokemonBiosGetNicknamePtr(pokemon);
    msgctrlSetValue(0x37, (u32)nickname);
    fn_800FB680(0, 0, -1, 0xe7);
    return 0;
}
