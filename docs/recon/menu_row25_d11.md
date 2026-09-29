# Row 25 (menu system): what's left after lane D11

Linked in this lane:

- windowDrawSprite2 and windowGetPortKeyInfo, as window carves.
- fn_801093C8 (XD menuOffScreenDraw), inside the menuOffScreen TU with its pool.
- _menuGetGcKeyInfo and _menuUpdateKeyInfo, as menu carves.
- fn_8001EA98, as a one-function carve.

The rest of the blocking list is below. Each entry says why it doesn't link yet.

## menuCursorNormal (98.42%, menu_r50_80102F38_o3)

The problem is register allocation only. Every instruction matches except the callee-saved assignment:

- **Retail** (r31 down to r20): current, window, right, left, down, index, xRange, yRange, up, data, wrap, best. In phase 2, xRange/yRange/index are r24/r25/r26 and best is r21.
- **Ours**: current, up, down, left, right, window, data, wrap, xRange, yRange, index, best.

GC/2.6 produces byte-identical code for this function, so the mwcc-debugger can replay it. The replay shows the GPR simplify order exactly:

- K=29 non-restarting sweep.
- Spill choice by lowest cost/degree, with ties going to the higher vreg.
- The simulator is `scratchpad/d11/simcopy/model.py` (lane scratch).

Searching all orderings of the 15 named webs reaches at most 9/15 retail colours. Merging the phase-1 and phase-2 webs doesn't get there either. So retail's interference graph differs, not just its numbering. In retail, `up` is spilled before down/left/right and index/xRange/yRange are simplified after it, which means those webs have higher degree in retail.

None of these move it:

- declaration order;
- inline `keys & N` tests;
- opt_* pragmas (opt_lifetimes off: 95.97%).

## _winSeqMoveSub (code exact) and winSpriteDrawTexture (90.9%)

_winSeqMoveSub is exact in win_sequence_candidate_80107170.c. It can't be carved: its .sdata2 pool entries (0x8047CE20-0x8047CE38: 100.0f and the two int-to-float doubles) are also referenced by winSpriteDrawTexture. XD's winSprite.cpp holds both functions (TeamOrre/xd-decomp `config/GXXE01/splits.txt` @4989794, 0x8010C3E4-0x8010E8C0). So the retail TU is winSeq+winSprite, 0x80106F98-0x801093C8, with pool 0x8047CE20-0x8047CE48. I tried a carve owning 0x8047CE20-0x8047CE38 and the jump table; it failed to link because win_sprite.o needs lbl_8047CE28/30.

The smallest linkable unit is 0x801074D4-0x80108C14 (from _winSeqMoveSub through winSpriteDrawTexture), owning:

- the pool 0x8047CE20-0x8047CE48;
- the jump table .data 0x8035B400-0x8035B430.

It needs two more functions exact:

- **fn_801081F8 (99.78%)**: retail colours the i*0x78 IV r31 and (u16)itemId r29. The frontend's hoisted (u16)itemId copy is propagated into its rlwinm vreg (65), which sits above the IV (62). Local-variable forms and the opt_* pragmas don't change it.
- **winSpriteDrawTexture (90.9%)**: retail addresses each corner array from its own base (`addi rN, work, 0x58; stfs 4(rN)`) and gives each array its own loop IV. A struct member array folds the offset instead. A `((f32*)((u8*)work + off))[i]` form reproduces the loop IVs (92.9%) but not the straight-line stores.

## fn_801D2404 (code exact) and fn_8001E644 (86.6%)

- **fn_801D2404**: exact in mailMain_r54b_801D23C0_suffix. Its literals (0.0f, 1.0f, 180.0f and the double at 0x8047E1B4-0x8047E1C8) sit in the middle of the mailMain TU pool (0x8047E1B0-0x8047E1D8). They're unaligned, so it links only with that TU.
- **fn_8001E644**: uses the 0x8001E3E0 TU's pool (its int-to-float double 0x8047B7D8 and 0x8047B7E4) and a `{colors}` .rodata initializer (0x80266C20). Like fn_801D2404, it needs the whole TU.
