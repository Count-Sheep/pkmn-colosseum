/**
 * @file msgctrl_candidate_80131714.c
 * @brief msgctrl.c carve, 0x80131714 - 0x801317FC: msgctrlAlign,
 *        msgctrlShadow, msgctrlMenuMoney and msgctrlMenuFullDigit.
 *
 * Text only. Built with the TU's -O4,p and the peephole pass off (one
 * unit-wide flag in configure.py, as for msgctrl.c and
 * msgctrl_exact_80132A38.c); with the pass on, the digit wrappers lose
 * retail's unfolded argument moves. The same bodies are in
 * src/game/msgctrl.c.
 */
#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

s32 msgctrlAlign(EffectUtilCommandObj* obj)
{
    extern void GSmsgAdjustAlign(void*);
    u8* stream;

    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->alignMode = stream[0];
        GSmsgAdjustAlign(obj);
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}

u32 msgctrlShadow(EffectUtilCommandObj* obj)
{
    u8* stream;

    if (obj->activeFlag != 0) {
        stream = obj->stream;
        obj->field_02 = *stream;
    }
    stream = obj->stream;
    obj->stream = stream + 1;
    return 0;
}

void msgctrlMenuMoney(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80426FF0, 0x10, lbl_8047AE94, 4);
}

void msgctrlMenuFullDigit(void)
{
    _msgctrlMakeDigit__FPUslUll(lbl_80427010, 0x10, lbl_8047AE68, 5);
}
