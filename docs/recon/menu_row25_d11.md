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

## mailMain TU (fn_801D2404): one register pair from linkable

The retail TU is mailMain.o (Colosseum official map: mailMainReceiveTerminate, mailMainInit): 0x801D2080-0x801D2B4C. It owns the .sdata2 pool 0x8047E1B0-0x8047E1D8 (-1.0f, the fn_801D2404 floats and double, chkMailSend's 0.015625f/1000.0f and u16 double) and chkMailSend's jump table (.data 0x8036E130-0x8036E14C, at the end of data_8036DE70.c). lbl_80467390 is .bss in the bss_80466DE8 blob and can stay extern.

src/game/mailMain.c now has every function code-exact under the TU's flags (GC/1.3, -opt nopeephole), apart from pool and jump-table relocation names, except fn_801D228C:

- The sound start shared by SendByScrpt, SendAllMail and cbStep is one inline (mailMainStartSe).
- chkMailSend uses raw u32 getter results with u8/u16 casts at each compare, a no-argument fn_801D1338, and `if (cmp) eligible = TRUE` cases.
- The getters' real prototypes are used (fn_801D1650(u8), fn_801D1364(u16 mode), u16 fn_801D139C/fn_801D19A4, u32 fn_801D1864).

fn_801D228C is at 99.74%. At the level check, retail converts the fn_801EE614 result into r0 and the limit into r4. Ours converts them into r3 and r0. In our IR the call-result conversion (r54) precedes the limit conversion (r55), and r55 is colored first. Retail needs the limit conversion scheduled first, live across r3, with the call conversion colored first.

None of these reproduce it:

- about 150 type and prototype combinations;
- both operand orders, with and without both calls inline;
- inline helpers;
- loop forms;
- opt_*/scheduling/optimization_level pragmas;
- compiler versions from GC/1.3 to GC/2.6.

Once it's exact, link 0x801D2080-0x801D2B4C as one unit with that .sdata2 and .data. That needs two splits:

- battle_sdata2_8047E190 ends at 0x8047E1B0;
- data_8036DE70 ends at 0x8036E130.

The -1.0f then becomes a literal.

## fn_8001E644 (86.6%)

This uses the 0x8001E3E0 TU's pool (its int-to-float double 0x8047B7D8 and 0x8047B7E4) and a `{colors}` .rodata initializer (0x80266C20). Like the mailMain case, it needs the whole TU.
