/**
 * @file ps_exact_8016A2D0.c
 * @brief HAL particle.c: psInitDataBankLocate, 0x8016A2D0 - 0x8016A644.
 *
 * The highest-address function of particle.c (see src/game/particle.c for
 * the TU extent), carved out at its function boundary: it touches no data
 * (no pool, no tables, no strings), calls nothing, and is the last
 * function of the unit, so the carve covers exactly its split range. Built
 * with the particle library flags (GC/1.3.2 -O4,p -inline auto,deferred
 * -use_lmw_stmw on -sdata 8 -sdata2 8 -str reuse,readonly), no local
 * pragmas. The body is particle.c's, unchanged.
 *
 * An unknown bank version skips straight to the kind fix-up with num, num2
 * and base unset. Retail does the same (the default edge of its switch
 * reaches the loop with no initialising instruction).
 */
#include "dolphin/types.h"
#include "sysdolphin/baselib/psstructs.h"

void psInitDataBankLocate(s32* cmdBank, s32* texBank, s32* formBank)
{
    s32 i;
    s32 num;
    s32 num2;
    HSD_PSCmdList** base;
    s32* ptr;
    s32* group;
    s32* groups;
    u32 fi;
    HSD_PSFormGroup* fg;
    s32 j;
    s32 k;
    s32 num_groups;

    /* An unknown version skips straight to the kind fix-up below with num,
     * num2 and base unset: retail sets none of them on that path. */
    switch (*(u16*) cmdBank) {
    case 0:
        num2 = cmdBank[1];
        base = (HSD_PSCmdList**) (cmdBank + 2);
        num = 0;
        for (i = 0; i < num2; i++) {
            cmdBank[i + 2] += (s32) cmdBank;
        }
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        num = cmdBank[1];
        num2 = cmdBank[2] + num;
        base = (HSD_PSCmdList**) (cmdBank + 3 - num);
        ptr = cmdBank;
        for (j = 0; j < cmdBank[2]; j++) {
            if (ptr[3] != 0) {
                ptr[3] += (s32) cmdBank;
            }
            ptr++;
        }
        break;
    }

    for (i = num; i < num2; i++) {
        if (base[i] != NULL) {
            base[i]->kind &= 0xF1FFFFFF;
            base[i]->kind |= 0x08000000;
        }
    }

    num_groups = texBank[0];
    group = groups = texBank + 1;
    for (k = 1; k <= num_groups; k++) {
        if (group[0] != 0) {
            group[0] += (s32) texBank;
        }
        group++;
    }

    for (k = 0; k < num_groups; k++) {
        HSD_PSTexGroup* tg = (HSD_PSTexGroup*) groups[k];

        if (tg != NULL) {
            u32 ti;

            for (ti = 0; ti < ((HSD_PSTexGroup*) groups[k])->num; ti++) {
                if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                    ((HSD_PSTexGroup*) groups[k])->texTable[ti] += (u32) texBank;
                }
            }
            tg = (HSD_PSTexGroup*) groups[k];
            if (tg->fmt == 8 || tg->fmt == 9 || tg->fmt == 10) {
                if (tg->palflag & 1) {
                    if (tg->texTable[tg->num] != NULL) {
                        tg->texTable[tg->num] += (u32) texBank;
                    }
                } else if (tg->palnum != 0) {
                    for (ti = tg->num;
                         ti < ((HSD_PSTexGroup*) groups[k])->num +
                                  ((HSD_PSTexGroup*) groups[k])->palnum;
                         ti++)
                    {
                        if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                            ((HSD_PSTexGroup*) groups[k])->texTable[ti] +=
                                (u32) texBank;
                        }
                    }
                } else {
                    for (ti = tg->num; ti < ((HSD_PSTexGroup*) groups[k])->num * 2;
                         ti++)
                    {
                        if (((HSD_PSTexGroup*) groups[k])->texTable[ti] != NULL) {
                            ((HSD_PSTexGroup*) groups[k])->texTable[ti] +=
                                (u32) texBank;
                        }
                    }
                }
            }
        }
    }

    if (formBank == NULL) {
        return;
    }
    for (i = 1; i <= num_groups; i++) {
        if (formBank[i] != 0) {

            formBank[i] += (s32) formBank;
            fg = (HSD_PSFormGroup*) formBank[i];
            for (fi = 0; fi < fg->num; fi++) {
                if (fg->formTable[fi] != NULL) {
                    fg->formTable[fi] += (u32) formBank;
                }
            }
        }
    }
}
