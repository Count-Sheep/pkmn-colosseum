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

## menuSub TU linked whole, so fn_8001E644 links

The 0x8047B7D8 double belongs to a small TU, 0x8001D718-0x8001EF78 (menuSub.o), not the large menuPokemon range. Its pool 0x8047B7C8-0x8047B808 repeats 0.0f and the signed conversion double from the neighbouring pools, so it is a separate TU. Nothing outside the range reads it.

`src/game/menuSub.c` links it with its pool, fn_8001E644's colour initializer (.rodata 0x80266C20-0x80266C30) and fn_8001EC08's vertex work area (.bss 0x803A1D60-0x803A1F88). It builds with GC/1.3.2 and -opt nopeephole.

New exact functions:

- **menuSubOpenSelect**: passes `&flags` (the parameter itself).
- **fn_8001DACC**: a sprite draw callback `(context, sprite, shift)`, rewritten from the pseudo-asm.
- **fn_8001E58C**: takes int corners, narrowed at the s16 fn_800D61E4 call.
- **fn_8001EC08**:
  - the rings are file statics addressed off the pooled .bss base;
  - `color /= 2` gives retail's signed divide;
  - `y + (height + 2)`;
  - the angle is inline in the sin/cos calls, which orders the pool as 30.0, 2*pi, 1/32, 15.0.

MWCC lays pooled statics out in first-reference order. Retail needs the 15.0 ring before the 30.0 ring, but fn_8001EC08 writes the 30.0 ring first. An unreferenced static helper that references them in retail's order fixes the layout. The linker strips it, and the DOL/REL SHA-1 pass. This is tagged RULE-EXCEPTION(title-path) and listed in docs/RULE_EXCEPTIONS.md. It replaces the old fn_8001EA98 1.0f extern stand-in.

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

### Second pass (D14's inline-return renumbering)

I tried taking values through one-line inline returns at 10 sites:

- the four direction flags;
- the xRange/yRange initialisers;
- wrap, data and current;
- the phase-1 index reset.

I combined these with random declaration orders (1200 samples). The best is 98.67%.

scratchpad/d13's vmap/whatif simulator doesn't reproduce our own allocation for this function. For example, it colours window r25 where the compiler gives r26. So I couldn't use it to steer, and the wall stands.

## People-frame colsys (lane D15)

### Linked in 4fd057e4

getCpPolyVec (GScolsys2Walk, 99.38%) and GScolsy2UtilChkInTri (99.56%) are exact.

- FPRs are coloured in reverse creation order, so retail creates the edge deltas (vn - v) before the point deltas.
- `colEdgeSide(ez, ex, z, x, pz, px)` writes `ez -= z; ex -= x;` into its own parameters. That makes them inline temporaries created first; it then returns `ex * (pz - z) - ez * (px - x)`.
- The helper is tagged RULE-EXCEPTION.
- GScolsys2Walk (with GetLayer) is linked, and GScolsys2Util is linked as the whole TU 0x8010F4B8-0x8010FAF4 with its pool.

### GScolsys2Thru: still walled

The vertex-transform loop's two strength-reduced pointers take r15/r16 the other way round in fn_801101B4 and GetMdlEventList (99.16% and 99.24%).

In our frontend dump, the loop's pointer temporaries are already created destination first (@211 dst, @212 src).

These didn't give the retail pair:

- src or dst as a user pointer, in function scope or block scope;
- statement splits;
- argument-reordering inline wrappers (their arguments are substituted before the loop pass).

The closest was a src user pointer in getMdlEventListPass (fn_801101B4 99.22%). It gets the loop's registers right but still issues the two preheader instructions in the other order, and moves the fixed pass's normal pointer from r17 to r16.

