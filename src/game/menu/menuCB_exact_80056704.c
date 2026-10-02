/** Exact menuCB selection callbacks, 0x80056704 - 0x80056854. */
#include "dolphin/types.h"

extern u32 lbl_8047A580;
extern u32 lbl_8047A584;
extern f32 lbl_8047A578;
extern f32 lbl_8047A57C;
extern f32 lbl_8047BEC0;
extern f32 lbl_8047BEC8;
extern f32 lbl_8047BECC;
extern u8 lbl_8026768C[];
extern void menuSetDisp(void* p, u32 enable);

/* RULE-EXCEPTION(user-approved): inherited nopeephole mode; see docs/RULE_EXCEPTIONS.md. */
u32 fn_80056704(void)
{
    u32 tbl[3];
    u32 cur;
    s32 val;

    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur - 1;
    lbl_8047A584 = cur;
    if ((s32)cur < 0) {
        lbl_8047A584 = 2;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    cur = lbl_8047A584;
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        menuSetDisp((void*)val, 1);
    }
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BEC8;
    return lbl_8047A584;
}

u32 fn_800567AC(void)
{
    u32 tbl[3];
    u32 cur;
    s32 val;

    cur = lbl_8047A584;
    lbl_8047A580 = cur;
    cur = cur + 1;
    lbl_8047A584 = cur;
    if ((s32)cur >= 3) {
        lbl_8047A584 = 0;
    }
    tbl[0] = ((u32*)lbl_8026768C)[0];
    tbl[1] = ((u32*)lbl_8026768C)[1];
    tbl[2] = ((u32*)lbl_8026768C)[2];
    cur = lbl_8047A584;
    if ((s32)cur < 0 || (s32)cur >= 3) {
        val = -1;
    } else {
        val = (s32)tbl[cur];
    }
    if (val >= 0) {
        menuSetDisp((void*)val, 1);
    }
    lbl_8047A57C = lbl_8047BEC0;
    lbl_8047A578 = lbl_8047BECC;
    return lbl_8047A584;
}
