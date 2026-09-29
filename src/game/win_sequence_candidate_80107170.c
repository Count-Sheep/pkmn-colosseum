/**
 * @file win_sequence_candidate_80107170.c
 * @brief winSeq: per-window/per-sprite animation sequences, 0x80107170 -
 *        0x80108580 (fn_80107170 .. winSetSequence).
 *
 * Built like the neighbouring winMsg TU with the peephole pass off, plus
 * -inline auto,deferred: winSetSequence (0x80108518, the last function)
 * is expanded into fn_80107F38, winSeqSetMenu and both paths of
 * fn_801081F8, and its out-of-line copy is still emitted at the end, which
 * is what deferred inlining gives for a function defined first. Deferred
 * mode emits functions in reverse definition order, so they are defined
 * here from the highest address down.
 *
 * The per-sprite and per-window move blocks (copy the drawable fields into
 * the shared target, run _winSeqMoveSub fn_800D3088() times, copy back)
 * are expanded six and two times respectively and have no out-of-line
 * copy, so they are static inline helpers. The shared targets are accessed
 * directly as globals: that is what gives retail's re-materialised
 * lis/addi pairs after each call.
 *
 * Still a candidate. _winSeqMoveSub's code is exact (its command numbers
 * are retail's jump-table indices and the flag byte is the relative/interp/
 * keep bitfield), but its .sdata2 pool entries (0x8047CE20-0x8047CE38:
 * 100.0f and the two int->float doubles) are shared with
 * winSpriteDrawTexture: XD's winSprite.cpp holds both _winSeqMoveSub and
 * winSpriteDrawTexture (TeamOrre/xd-decomp config/GXXE01/splits.txt
 * @4989794), so the retail TU is the winSeq and winSprite code together
 * (0x80106F98-0x801093C8, pool 0x8047CE20-0x8047CE48), and a carve of
 * _winSeqMoveSub alone can't own the pool. fn_801081F8 (99.8%, one
 * register pair swapped) is not exact yet.
 */
#include "game/win_sequence.h"

/* 0x80108518 */
void winSetSequence(WinSeqState* state, u32 id) {
    if ((u16)id == 0) {
        state->commands = NULL;
        state->commandIndex = 0;
    } else {
        memset(state, 0, sizeof(WinSeqState));
        state->commands = menuSeqBiosGetPtr((u16)id);
    }
}

/* 0x801081F8 */
void fn_801081F8(tagWINDOW_WORK* window, s32 itemId, u32 seqId) {
    tagSPRITE_WORK* sprite;
    s32 i;

    if (window == NULL) {
        for (i = 0; i < 0x168; i++) {
            sprite = &lbl_8047AD1C[i];
            if (sprite->flags != 0 && sprite->itemId == (u16)itemId) {
                winSetSequence(&sprite->sequence, seqId);
                winSeqMoveSprite(sprite);
            }
        }
    } else {
        sprite = windowSearchItemID(window, itemId);
        if (sprite == NULL) {
            return;
        }
        winSetSequence(&sprite->sequence, seqId);
        winSeqMoveSprite(sprite);
    }
}

/* 0x801080CC */
void winSeqSetMenu(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);

    if (window == NULL) {
        return;
    }
    winSetSequence(&window->menuSequence, seqId);
    winSeqMoveWindow(window);
}

/* 0x80107F38 */
void fn_80107F38(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);
    tagSPRITE_WORK* sprite;

    if (window == NULL) {
        return;
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSetSequence(&sprite->sequence, seqId);
        winSeqMoveSprite(sprite);
    }
}

/* 0x80107ED8 */
s32 winSeqIsCheck(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);

    if (window != NULL && window->menuSequence.commands == menuSeqBiosGetPtr((u16)seqId)) {
        return 1;
    }
    return 0;
}

/* 0x80107E78 */
s32 fn_80107E78(tagWINDOW_WORK* window, s32 itemId, u32 seqId) {
    tagSPRITE_WORK* sprite = windowSearchItemID(window, itemId);

    if (sprite != NULL && sprite->sequence.commands == menuSeqBiosGetPtr((u16)seqId)) {
        return 1;
    }
    return 0;
}

/* 0x801074D4 */
void _winSeqMoveSub(WinSeqTarget* target, WinSeqState* state) {
    WinSeqCommand* command;
    WinSeqSpriteData* sprite;
    f32 t;
    u8 stop;

    stop = 0;
    if (state->commands == NULL) {
        return;
    }

    if (state->positionMode != 0) {
        state->positionFrame++;
        switch (state->positionMode) {
        case 1:
            t = (f32)state->positionFrame / (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 2:
            t = ((f32)state->positionFrame * (f32)state->positionFrame /
                 (f32)state->positionDuration) /
                (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 3:
            {
                f64 durationRoot = sqrt((f64)state->positionDuration);
                t = (f32)(sqrt((f64)state->positionFrame) / durationRoot);
            }
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        }
        if (state->positionFrame >= state->positionDuration) {
            state->positionMode = 0;
        }
    }

    if (state->colorMode != 0) {
        state->colorFrame++;
        switch (state->colorMode) {
        case 1:
            t = (f32)state->colorFrame / (f32)state->colorDuration;
            target->color.r = state->startColor[0] +
                               t * (state->endColor[0] - state->startColor[0]);
            target->color.g = state->startColor[1] +
                               t * (state->endColor[1] - state->startColor[1]);
            target->color.b = state->startColor[2] +
                               t * (state->endColor[2] - state->startColor[2]);
            target->color.a = state->startColor[3] +
                               t * (state->endColor[3] - state->startColor[3]);
            break;
        }
        if (state->colorFrame >= state->colorDuration) {
            state->colorMode = 0;
        }
    }

    if (state->scaleMode != 0) {
        state->scaleFrame++;
        switch (state->scaleMode) {
        case 1:
            t = (f32)state->scaleFrame / (f32)state->scaleDuration;
            target->scaleX = state->startScaleX +
                             t * (state->endScaleX - state->startScaleX);
            target->scaleY = state->startScaleY +
                             t * (state->endScaleY - state->startScaleY);
            break;
        }
        if (state->scaleFrame >= state->scaleDuration) {
            state->scaleMode = 0;
        }
    }

next_command:
    if (state->delay > 0) {
        state->delay--;
        return;
    }

    command = &state->commands[state->commandIndex];
    switch (command->type) {
    case 0:
        state->commands = NULL;
        state->commandIndex = 0;
        stop = 1;
        break;
    case 1:
        state->delay = command->duration;
        break;
    case 3:
    case 9:
        if (command->type == 9) {
            state->endX = target->baseX;
            state->endY = target->baseY;
        } else {
            switch (command->relative) {
            case 0:
                state->endX = (s16)command->value0;
                state->endY = (s16)command->value1;
                break;
            case 1:
                state->endX = (s16)(target->x + command->value0);
                state->endY = (s16)(target->y + command->value1);
                break;
            }
        }
        if ((command->keep & 1) != 0) {
            state->endX = target->x;
        }
        if ((command->keep & 2) != 0) {
            state->endY = target->y;
        }
        state->positionDuration = command->duration;
        if (state->positionDuration == 0) {
            target->x = state->endX;
            target->y = state->endY;
            state->positionMode = 0;
        } else {
            state->startX = target->x;
            state->startY = target->y;
            state->positionFrame = 0;
            switch (command->interp) {
            case 0:
                state->positionMode = 1;
                break;
            case 1:
                state->positionMode = 2;
                break;
            case 2:
                state->positionMode = 3;
                break;
            case 3:
                state->positionMode = 1;
                break;
            }
        }
        break;
    case 7:
        state->startColor[0] = target->color.r;
        state->startColor[1] = target->color.g;
        state->startColor[2] = target->color.b;
        state->startColor[3] = target->color.a;
        state->endColor[0] = (u32)command->value0 >> 24;
        state->endColor[1] = (command->value0 >> 16) & 0xFF;
        state->endColor[2] = (command->value0 >> 8) & 0xFF;
        state->endColor[3] = command->value0;
        state->colorFrame = 0;
        state->colorDuration = command->duration;
        state->colorMode = 1;
        if (state->colorDuration == 0) {
            target->color.r = state->endColor[0];
            target->color.g = state->endColor[1];
            target->color.b = state->endColor[2];
            target->color.a = state->endColor[3];
            state->colorMode = 0;
        }
        break;
    case 6:
        state->startScaleX = target->scaleX;
        state->startScaleY = target->scaleY;
        state->endScaleX = (f32)command->value0 / 100.0f;
        state->endScaleY = (f32)command->value1 / 100.0f;
        state->scaleFrame = 0;
        state->scaleDuration = command->duration;
        state->scaleMode = 1;
        if (state->scaleDuration == 0) {
            target->scaleX = state->endScaleX;
            target->scaleY = state->endScaleY;
            state->scaleMode = 0;
        }
        break;
    case 2:
        if (command->value0 != 0) {
            target->flags = (s8)(target->flags | 2);
        } else {
            target->flags = (s8)(target->flags & ~2);
        }
        break;
    case 11:
        if (command->value0 != 0) {
            state->enabled = 1;
        } else {
            state->enabled = 0;
        }
        break;
    case 10:
        if (state->loopActive == 0) {
            state->loopActive = 1;
            state->loopCount = command->duration;
        }
        if (command->relative == 0) {
            state->loopCount--;
        }
        if (state->loopCount < 0) {
            state->loopCount = 0;
            state->loopActive = 0;
        } else {
            state->commandIndex = (s16)command->value0;
            goto next_command;
        }
        break;
    case 8:
        sprite = menuSpriteBiosGetPtr(command->value0);
        target->image = sprite->image;
        target->imageX = sprite->x;
        target->imageY = sprite->y;
        target->imageW = sprite->width;
        target->imageH = sprite->height;
        break;
    }

    if (stop != 0) {
        return;
    }
    state->commandIndex++;
    goto next_command;
}

/* 0x801071D0 */
void winSeqMoveMenu(tagWINDOW_WORK* window) {
    tagSPRITE_WORK* sprite;

    winSeqMoveWindow(window);
    for (sprite = window->sprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
}

/* 0x80107170 */
s32 fn_80107170(s32 id, s32 itemId) {
    tagSPRITE_WORK* sprite = windowSearchItemID(windowSearchID(id), itemId);

    if (sprite != NULL && sprite->sequence.commands != NULL && sprite->sequence.enabled == 0) {
        return 1;
    }
    return 0;
}
