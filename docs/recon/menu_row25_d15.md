# Row 25 (menu system): lane D15 notes

This follows docs/recon/menu_row25_d11.md. The GSmsg blockers (fn_800FBB34, fn_800FD348 and the rest of gs_msg*) belong to lane D14 and are not covered here.

## Linked

- **mailMain TU** (commit c1c489ed). This links fn_801D2404.
  - The last register pair in fn_801D228C came from a missing argument. fn_801EE67C takes `(id, value)`: its prologue saves r4. The call is `fn_801EE67C(object, limit)`.
  - With the call fixed, the limit's u8->int conversion coalesces with r4. The level conversion is then colored first and takes r0.
  - None of the ~150 compare and type variants could get there, because the pair order came from the argument register, not from the compare.
  - `game/mailMain.c` owns .text 0x801D2080-0x801D2B4C, the pool 0x8047E1B0-0x8047E1D8 and chkMailSend's jump table 0x8036E130-0x8036E14C.
- **winSeq/winSprite TU tail** (commit 24c8f9e3). This links _winSeqMoveSub and winSpriteDrawTexture.
  - `game/win_sprite.c` covers .text 0x80107170-0x801093C8. It owns the pool 0x8047CE20-0x8047CE48, the jump table 0x8035B400-0x8035B430 and the .bss 0x80404B68-0x80404BF0.
  - The unit has to reach 0x801093C8 because winSpriteDraw and winSpriteAdd read the pool's 1.0f.
  - fn_801081F8:
    - windowSearchItemID takes a u16 id, since it extends its own argument.
    - fn_801081F8, fn_80107E78 and fn_80107170 take u16 item ids, and winSetSequence takes a u16 id.
    - The hoisted item-id conversion is then a frontend variable numbered below the loop IV.
  - winSpriteDrawTexture:
    - The v/u/y/x arrays are file statics. MWCC addresses them off the pooled .bss base, which gives the per-array `addi rN, base, off` bases and the per-array loop IVs.
    - Statics are laid out in reverse definition order.
    - GC/1.3 folds the pooled offsets into the store displacements. GC/1.3.2 keeps them out, as retail does. Every other function in the TU is identical under the two versions.
    - fn_800D5CB8 takes u8 colours, which removes the hoisted conversion copies before the draw loop.
    - The centre is `(f32)abs / 2.0f`. MWCC turns that into `* 0.5f` with the operand order retail has; written as `* 0.5f`, the parser puts the constant first.

## fn_8001E644: code-exact, walled on its TU (commit ca4364fc)

The rewrite in gs_pcbox_range_8001E3E0.c matches every instruction. The only difference is the relocation of its int->float conversion constant: retail reads the pooled double 0x8047B7D8.

That pool is shared across a large range:

- 0x8047B7D0/0x8047B7D8 are read by menuSub (0x8001DACC), menuPokemon (0x8001D624), fn_8001E644 and fn_8001EC08.
- 0x8047B7C0 is read by 0x80019F6C and 0x8001C7B8.
- The .rodata 0x80266C00-0x80266C2C is read by 0x800181C4 and fn_8001E644.

So the TU runs from about 0x800181C4 to 0x8001F304. The function can link only with that whole unit and its pool, which includes about 20 functions still at 60-95%.

A carve would bring its own conversion double, and there is no named slot for it that the other asm objects would still resolve.

## menuCursorNormal (98.42%): still walled

I tried the following. None beat D11's result by much, and none changed the retail pattern: up spilled late (r23), right/left/down colored r29-r27, and the phase-2 ranges swapped.

| Attempt | Best result |
|---|---|
| Random declaration orders, 400 samples over all 15 locals | 98.63% |
| Separate phase-2 variables (xRange2, yRange2, index2, best2) | 97.83% |
| Direction-flag types (BOOL, int, long) | unchanged |
| Direction-flag types (s8, s16, u16) | instructions change |
| wrap as u8 or s32, keys as u32 or s32 | no gain |

From the GC/2.6 replay:

- Every long-lived web has degree 70-136, far above K=29. So the colour order comes entirely from spill choice by cost/degree.
- The four direction flags each cost 33, the same as data, so ties decide their order.
- Retail spills `up` before xRange/yRange/index and down/left/right. That needs `up` to have a lower cost/degree than the others, or the others a higher degree. So retail probably has a different use count or live range for `up`, while the instructions stay the same.

I found nothing in the retail code that shows where that difference comes from.
