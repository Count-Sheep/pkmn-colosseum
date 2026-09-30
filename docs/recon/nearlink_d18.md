# Near-link sweep, non-fight half (lane D18, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d18-nearlink`.
The list was built from `build/GC6E01/report.json`: incomplete units whose
inexact functions all score 98% or higher. fight_* units belong to D17, and
the units on D16's, D14's, D13's and D11's documented walls were skipped.
Every link passed configure, `ninja all_source report`, a full `ninja` (main.dol and
common_rel.rel SHA-1 OK), `quality_scan` and `check_regression`.

## Linked

| Commit | Unit | Functions | Fix |
|---|---|---|---|
| 6e5deed9 | gs_scratch_alloc_800EEC38 | GSscratchAlloc | Single-use wrapper GSscratchFindFreeAllocation (RULE-EXCEPTION, title path) |
| 15bf4d9f | battle/battle_grid_r56_801C3114_prefix | fn_801C3114, fn_801C31EC, battleGridUpdate | One file-scope `extern lbl_80466DE8`. Each block-scope extern is its own MWCC object, so they blocked address and compare CSE. The carve owns its int-to-float literal (split change) |
| 03f7a163 | hero_move_r46_8012FCD4_suffix | heroMoveInit, heroMoveSyncWithHero, fn_8013024C | Standalone carve: static inline copies of the party helpers and pool stand-ins (`p[-1]/p[0]` table store, pointer-cast second read of 12.0f) |
| b3b35968 | dolphin/sdk_candidate_8009CD38 | OSFatal, Halt | XD's form: timeout written in the loop condition over the address-bound `__OSBusClock`. The carve owns OSFatal's pool (split change) |
| 142fcd5d | gs_material_candidate_800DFABC | _matGSmatEnableEnvMapExt | u16 extension mask (found with D16's simp.py) and the texture local declared first |
| 62110d4d | menu/menu_r56b_800714C8_suffix | _menuPop, _menuPush, fn_8007162C, menuCB_InitMenu, fn_8007169C | s32-array index for the flag clear. `-opt nopeephole`. fn_8007169C added to FORCEACTIVE |
| fad34e99 | gbaCommunication_candidate_800965C8_gc20 | fn_800965C8 | u32 windowDrawSprite prototype, u8 status copy, sex cases 0-2, `-0x100 \| byte`, s8 state read, reused locals. `-opt nopeephole` |
| 4a4e6f55 | hero_move_r46_8012EBD4 | heroMoveMain .. initFloor (8 functions) | Standalone carve. initFloor's register pair comes out as retail with the stand-ins; its 10.0f is read through a const pointer cast |

## Walled or skipped

- **cameraUpdate** (camera_candidate_80177A64, 99.96). The in-place frame
  needs the XD-named view helpers (see camera.c). A carve would also need
  retail's int-to-float bias `lbl_8047D738`, but MWCC always emits its own
  local copy for `(f32)` conversions. cameraPlayAnime and cameraInit, which
  are unlinked, read the same symbol, so the carve can neither own nor
  reference it. The manual union conversion gives fsub+frsp, not retail's
  fsubs. It links only with the whole camera TU (needs cameraDispInfo's
  .rodata).
- **fn_80077ED4** (menu/menu_candidate_80077ED4, 98.53). Same conversion
  constant wall: `lbl_8047C0F0`/`F8` are also read by the unlinked
  menu_candidate_r47_80078390_o2 and r47_800788BC units. The other
  difference is the load order and FPR colouring of the wait loop's
  0.0f/limit literals (the ETCTOOL_WAIT pattern).
- **fn_801E09E0** (gs_candidate_801E09E0, 99.17). Its pool
  (0x8047E3F0-0x8047E424) is shared with the unlinked unit at 0x801DF474,
  so it needs stand-ins, and it has two open diffs:
  - The sequence table base is materialised into r0 and then moved; retail
    uses `addi r30, r4, lbl@l`. Moving the assignment or using the global
    directly made it worse (94-98.5).
  - The floorOpenObject return goes through `mr r0,r3 .. mr r29,r0`, with
    `addi r4,r30,0` for `&table[0]`. That points to a late-known index or
    an inline return.
- **fn_8013E258** (effect_visual r41 chunk, 98.88). The source reads the
  pool through `*(f32*)&u32` stand-ins, and the int-to-float literal is the
  TU's. It needs the pool rework first: `const f32` stand-ins, or linking
  the effect_visual TU.
- **fn_8017B4BC** (fsys_file_r48 prefix, 98.79). The body is an -O0/-O1
  shaped form (volatiles, self-assignments). Low value, not attempted.
- **fn_80025F84** (gs_title). The owner was claimed by Codex-title-reloc.

## Techniques that worked

- Block-scope `extern` declarations of the same global in several
  functions stop MWCC from sharing the address and loads after inlining. A
  single file-scope declaration fixed battleGridUpdate.
- A standalone carve with static inline copies of the TU's helpers can
  change interference enough to dissolve a documented register wall
  (initFloor).
- When an extern stand-in replaces a literal used twice (`x = K; x += K`),
  two plain reads merge into one value and shift the FPR colouring. Reading
  the second one as `*(f32*)&K` keeps two loads, like two literals. For a
  compare constant, a `const` extern read through a pointer cast gave
  retail's registers.
- A block copy of an extern struct relocates its second word as `sym+4`,
  and objdiff scores that below 100. `s32* p = &ids.id[1]; p[-1] = A; p[0]
  = B;` keeps the stack stores and names each word's symbol.
- An unreferenced function in a linked carve is dead-stripped. Add it to
  `config.force_active_symbols["main"]`.

## Worth a look next

The report also lists several dozen incomplete units whose functions are all
already 100% (for example THPDec_range_801E6578, sdk_range_800C5458,
camera_candidate_801786F4, field_range_801DF790 and sdk_r51_800B0694_prefix).
Each needs only the carve work used above: pool or stand-in handling, split
ownership, FORCEACTIVE, and a check that the object emits nothing extra.

## Second sweep: all-100% but unlinked units (2026-09-30)

These units already scored 100% on every function but did not link. The
blocker was always data ownership, function order, dead-stripping or a
wrong unit boundary, never code.

| Commit | Unit | What it took |
|---|---|---|
| 5bc356f7 | gs_task_residual_80006908 | Own its switch table (.data 0x802E28D0-0x802E28EC) out of the data_8027A500.c blob |
| 8bf6d903 | sdk_candidate_8009ED70 (OSLink tail) | Own OSUnlink's string (.data 0x80311840-0x80311868). OSLinkFixed added to FORCEACTIVE |
| 1404bb8f | OSCache_l2_8009B628 (new) | Split the C tail off OSCache_privileged_suffix and own its strings. `asm { sync }` became `__sync()`. The asm LC* routines stay unlinked |
| fc505964 | sdk_range_800BB81C (new, 23 functions) | Merged three chunks so one object owns the pool 0x8047C370-0x8047C388 and the TEV presets. The "_gc13" chunk is exact under GC/1.2.5n |
| 34db7633 | sdk_range_800C5458 (MSL) | Own .rodata 0x8026FF00-0x8026FFE0, the __two_exp table and the 0.0 literal |
| 3b2ee24e | dbgMenu_candidate_80133250, dbgMenu_candidate_801334A8, sdk_candidate_800A35E4_suffix | Flip to Matching. The two debug callbacks were added to FORCEACTIVE |
| d9904b37 | sdk_range_800BF33C (TRK) | Source order fixed: usr_puts_serial comes before TRKDispatchMessage. Own the dispatch table |
| 3dec8f69 | dvd/DVD_range_800A7880 (new) | Merged DVDCancelAsync, DVDCancel and DVDCheckDisk so that DVDCheckDisk's 4-aligned table follows DVDCancelAsync's inside one 8-aligned .data section |

### Left for later

- **THPDec_range_801E6578** and **THPDec_range_801E5A28**: the THP decoder
  is one TU with static tables and .bss, and its middle is `asm {}` blocks.
  It needs asm evidence entries before any of it can link.
- **OSCache_privileged_prefix and _suffix**: the SDK's hand-written asm cache
  routines. They stay unlinked.
- **Camera chunks** (80176C78, 801786F4, 80179E04): part of the camera TU
  wall (cameraUpdate, the shared int-to-float bias, cameraDispInfo .rodata).
- **field_range_801DF790 and _r41_801E075C**: the int-to-float biases in
  the pool 0x8047E3F0-0x8047E424 are also read by the unlinked
  gs_candidate_801E09E0 (99.17). Link those three together once
  fn_801E09E0 is exact.
- **sdk_r58_800A2D38_prefix and sdk_r59_* (mtx.c)**: score-only partitions
  sharing the mtx pool, plus allowlisted paired-single asm. Link mtx.c as
  one unit.
- **Range carves still to do**: the CARD (sdk_r51_800B0694_prefix,
  sdk_r51_800B1464_inline_noauto, sdk_candidate_800B3978), GX
  (sdk_candidate_800B671C, sdk_r52_800BD7A0_prefix, sdk_candidate_800BD16C,
  sdk_range_800B8AE8), OSThread, gba_conv/gba_misc, msgctrl, hero_move and
  people_data chunks. Each compiles a whole range file, so each needs a
  standalone carve like the ones above. Check data ownership with
  `scratchpad/d18/secchk.py` (compiled vs target sections and globals)
  before flipping.

### Lessons

- A unit whose .data starts at a 4-aligned address cannot stand alone:
  MWCC gives .data sections 8-byte alignment. Merge it with the object whose
  data precedes it.
- If a linked carve changes the DOL size, compare `main.elf` symbol
  addresses with symbols.txt. The first shifted symbol points at a
  dead-stripped function (add it to FORCEACTIVE) or a wrong function order
  in the source.
