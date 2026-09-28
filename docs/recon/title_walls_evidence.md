# Title-path walls: inline-helper evidence recon (2026-09-28, lane RC)

Six title-path functions are exact only when part of their body is written as
a `static inline` helper. The "Reconstructed inline helpers" policy
(docs/CAMPAIGN_OPERATIONS.md) admits such a helper only when the target itself
proves it existed: repeated expansion at two or more call sites, a listed
inline fingerprint, or an inlining-only copy (2026-09-27 clause). This recon
searched Colosseum (DOL and REL 125), Pokemon XD, and the DOL/REL strings for
that evidence. Base: 085a52e7.

**Decision (user, 2026-09-28).** A named Pokémon XD function with the same call
sequence is now admitted evidence: see the "Same-engine sister-title function"
clause in docs/CAMPAIGN_OPERATIONS.md. The three XD-backed helpers below
(`floorInitScene`, `peopleMoveTypeRandomRot`, `peopleUpdateShadows`) have been
merged. Where this report says "not admitted", it describes the policy at the
time of the recon.

## Summary

| Wall | Now | Repeated expansion in Colosseum | Written-policy fingerprint | XD evidence | Admissible under the written policy |
|---|---|---|---|---|---|
| floor.c `fn_801129CC` (map-visibility + sun phase) | 99.89% | none (closest canon 0.46, other code) | none (register choice only) | **yes**: XD `_floorInitScene__FP11GSfloor_dd_` has the same body as a standalone static function | no |
| people.c `fn_80181850` (a) case-5 body | 99.52% | none | none (register choice only) | **yes**: XD `_peopleMoveTypeRandomRot__FP13tagPeopleWork`, called from the same dispatch slot | no |
| people.c `fn_80181850` (b) shadow-receiver block | (same function) | none | none: retail has *fewer* instructions than the in-place form, not an extra copy | **yes**: XD `_peopleUpdateShadows__FP13tagPeopleWork`, same calls in the same order | no |
| dbgMenu.c `fn_80132C6C` (item reset) | 99.54% | none (only the unroller's own 8 copies) | none | none (no XD counterpart found) | no |
| gs_vm.c `fn_800F6D18` / `fn_800F5CA0` | 99.81% / 99.46% | none for any block | none | none (XD rewrote the VM as C++ `GSscript::Tiga`) | no |
| THPPlayer.c `fn_801E2CA8` | 98.67% | n/a (the existing FillStreamBuffer helper is already admitted, 12 expansions) | none | none (XD replaced the player with `GSmovieStreaming`) | no |

Nothing became admissible under the written policy, so no source change was
committed on the lane branch. The XD evidence for floor and people is exact at
the call level and is recorded below. Admitting it would need a policy decision
(sister-build evidence is not one of the written categories). The tested
helper versions are kept on the side branch `rc-xd-helper-proposal` (see "Tested
helper versions" below), so it is quick to apply if the policy is extended.

## Sources and reproduction

- Colosseum asm: `build/GC6E01/asm/**/*.s` and
  `build/GC6E01/common_rel/asm/**/*.s` from `python3 configure.py --no-progress && ninja all_source build/GC6E01/report.json`.
  REL 125's text is only `_prolog`/`_epilog`/`_unresolved`, so it holds no
  candidate expansions.
- Matcher: `tools/find_inline_expansions.py` (committed with this report).
  `block START END` scores every same-length window in every other function
  (difflib ratio over normalised instructions: every non-fixed register becomes
  `R`/`F`, branch targets `L`, callees and relocations kept); `canon=` repeats
  the score with registers renamed by first appearance (1.000 = same data
  flow). `calls A B ...` finds call subsequences. `--ghidra-dir` searches a
  Ghidra-style dump of another build instead, rewriting both sides to one
  dialect (`mr`/`or`, `clrlwi`/`slwi`/`extrwi` -> `rlwinm`, relocations -> `SYM`).
  Validation: the known 4-site helper `GSvmFindByKey` (gs_vm.h) is found at all
  four sites (`block 0x800F6E94 0x800F6EDC --min-score 0.7`: fn_800F7108 0.944,
  fn_800F7068/fn_800F716C/fn_800F7274 0.889).
- Pokemon XD symbols: TeamOrre/xd-decomp `config/GXXE01/symbols.txt`
  (commit 4989794e, sha1 27f60f8a). These are XD's real names, including
  `scope:local` statics, so a static helper in XD shows up by name.
  `gh api repos/TeamOrre/xd-decomp/contents/config/GXXE01/symbols.txt -H "Accept: application/vnd.github.raw" > xd_symbols.txt`
- Pokemon XD asm: trevor403/xd-asm (commit b1087f18), one Ghidra-style file
  per function (`code/func_FUN_<addr>.s`).
  `gh api repos/trevor403/xd-asm/tarball > xd-asm.tar.gz && mkdir xd-asm && tar -xzf xd-asm.tar.gz -C xd-asm --strip-components 1`
  No XD disc, DOL or map is on this machine: `mdfind -name GXXE01` and
  `find ~ -maxdepth 6 -iname "*GXX*"` find only save files (`GXXE.raw`, `.gci`);
  the XD ISO listed in the USB/RomM inventories is on a drive that is not mounted.
- Strings: `strings -n 5 orig/GC6E01/sys/main.dol | grep -i -E "initscene|updateshadow|randomrot|movetype|floorclear|setinitpos|dbgmenu|toolentry"`
  and the same on `common_rel.rel`: no helper names (only `floorInitMap`,
  `_floorInitCharacters`, `peopleOpenSub`, and similar function-name log strings).

Commands for every Colosseum-side query in this report (run from the repo root):

```sh
T=tools/find_inline_expansions.py
X="--ghidra-dir xd-asm/code --ghidra-symbols xd_symbols.txt"
python3 $T block 0x80112BA4 0x80112C30 --min-score 0.6     # floor loop in the DOL
python3 $T calls floorReadMakeModelResID GSresGetResource GSmodelSetVisibility --gapped
python3 $T block 0x80181DC8 0x80181E84 --min-score 0.6     # people shadow block
python3 $T block 0x801819C8 0x80181A6C --min-score 0.6     # people case 5
python3 $T block 0x80132F38 0x80132F5C --min-score 0.7     # dbgMenu item reset
python3 $T block 0x800F6D50 0x800F6D90 --min-score 0.7     # gs_vm blocks (also 0x800F6D90, 0x800F6DF8, 0x800F6E54)
python3 $T block 0x80112B8C 0x80112C98 --min-score 0.5 $X  # floor phase in XD
python3 $T block 0x80112BA4 0x80112C30 --min-score 0.5 $X
python3 $T block 0x80112A94 0x80112B8C --min-score 0.5 $X  # floorEnterReset, second half
python3 $T block 0x80181DC8 0x80181E84 --min-score 0.5 $X  # people shadow block in XD
python3 $T block 0x80132C6C 0x80132D58 --min-score 0.45 $X # dbgMenu in XD
python3 $T block 0x800F6D18 0x800F6DC8 --min-score 0.5 $X  # gs_vm in XD
python3 $T block 0x801E2CA8 0x801E2DA8 --min-score 0.45 $X # THP in XD
```

## 1. floor.c `fn_801129CC` (99.89%)

The wall: in the map-visibility loop retail colours `index` (r28) before the
strength-reduced table offset (r27); every in-place form gives the reverse.
It is register choice only, with no extra or missing instruction.

**Colosseum.** The loop idiom (`scene_data` public address, `models[i] != NULL`,
`GSresGetResource(group, baseId | i)`) occurs in seven other functions, but
none repeats this body. They are hand-written variants with a hoisted group
ID, a different callee or a lights loop: fn_80112260 0.771/canon 0.457,
fn_80111DF8 0.743/0.429, fn_80111F2C 0.686, fn_8011207C 0.657, fn_801D6E58
0.686, `_wazaSequenceEffectEntryStart` 0.686, battle camera/grid 0.629. This
body alone calls `floorDataBiosGetCurrentPtr`+`floorDataBiosGetGroupID` inside
the loop (`calls floorDataBiosGetCurrentPtr floorDataBiosGetGroupID GSresGetResource GSmodelSetVisibility`
has one hit, 0x80112BF8). No fingerprint: nothing is re-tested on a stale CR,
no return is routed through a temp, and no copy is extra.

**XD.** `_floorInitScene__FP11GSfloor_dd_` (XD 0x8011F368, size 0x1D4,
`scope:local`) is the best XD match for the whole phase (0x80112B8C-0x80112C98:
0.642) and for the loop (0.800, canon 0.800). It is called from
`_floorInitializeCommon` with the floor record. Its body, call for call:

| XD 0x8011F368 | Colosseum 0x80112B8C |
|---|---|
| `floorDataBiosGetGroupID(floor)`, result discarded | same (0x80112B90, the "result unused in retail" call) |
| `floorDataBiosGetMapResID(floor)`; `beq` | same |
| `li r28,0` (index), `floorDataBiosGetCurrentPtr` -> r31 | `li r28,0`, `floorDataBiosGetCurrentPtr` -> r31 |
| `floorGetCurrentGroupID()`, `GSresGetResource(g, data->mapResId)` | the same, with `floorGetCurrentGroupID` expanded (Colosseum's out-of-line copy is fn_80113F48) |
| `HSD_ArchiveGetPublicAddress(.., "scene_data")`, `models` test | same |
| `floorReadMakeModelResID`, loop: `floorGetCurrentGroupID(); or r4,r31,r28; GSresGetResource; GSmodelSetVisibility(m, 1)` | same |
| lens flare off, sun: `floorDataBiosGetSunResID`, model part/position, `GSlensFlareSetSunPosition`, `GSlensFlareSetVisible(1)` | `fn_801ED640(0)`, `floorDataBiosGetSunResID`, `GSmodelGetPart`/`GSpartGetTransform`, `fn_801ED648`, `fn_801ED640(1)` (XD `GSlensFlareSetVisible` 0x801ED628 and `GSlensFlareSetSunPosition` 0x801ED6C0 sit where Colosseum's fn_801ED640/fn_801ED648 are) |

XD's standalone copy colours the loop the way Colosseum's in-place source does:
index r28, offset r29, so the offset is coloured first. Colosseum retail has the
reverse order, which the GC/2.6 replay reaches only when `index` belongs to an
inlined body. This is still register choice, so it is not a written fingerprint.

The same XD function list also shows Colosseum's already admitted
`floorEnterReset` as two XD statics, `_floorClearFlag__Fv` (0x80120654) and
`_floorSetInitPosition__Fv` (0x80120548, 0.726 against 0x80112A94-0x80112B8C).
The standalone `_floorClearFlag` zeroes its two area locals with two separate
`li` (`li r30,0x0; li r29,0x0`). That is an independent confirmation that
retail Colosseum's `li r28,0; mr r31,r28` is the inlined form, the premise of
the 2026-09-27 clause.

Other leads from the same comparison (names not applied): fn_800FF548 /
fn_800FF554 / fn_800FF52C = XD `floorIsPop` / `floorIsPush` /
`floorIsFloorPushed`; fn_80113F48 = `floorGetCurrentGroupID`; the kind switch
after `fn_80117E58` corresponds to XD `_floorInitShadow__FP11GSfloor_dd_`
(`GSmodelSetShadowTextureSize(0x280, 0x1E0)` / `(0x180, 0x180)`,
`GSmodelSetShadowBoundExpansion`).
XD's floor.cpp also holds `floorMapSetDisp`/`floorMapResourceDataIterate` and
the push/pop/count callbacks. Their Colosseum counterparts (fn_80112260,
fn_80111DF8, fn_80111F2C, fn_8011207C) sit in
`GScolsys2Sun_range_80111C24`, which suggests floor.c's TU starts earlier than
0x80112380. That is a lead only; splits were not changed.

Verdict: no Colosseum-internal evidence. The XD evidence is exact at the call level.

## 2. people.c `fn_80181850` (99.52%)

### (a) case-5 body (turn after peopleCalcRange)

**Colosseum.** No second expansion: the best window is 0.610/canon 0.293 in
fn_80184D80 (different callees: `fn_800E0BE4`, `sin`). The allocation wall is
register choice only.

**XD.** `peopleDaemon` (XD 0x80298814) dispatches the move types in a switch
whose arms are `_peopleMoveTypeLinear` (0x8029A558), `_peopleMoveTypeList(b)`
(0x8029A0C8, twice, with false/true), `_peopleMoveTypeRandomWalk` (0x80299E54)
and `_peopleMoveTypeRandomRot` (0x80299D30). Colosseum's `fn_80181850` has the
same switch: case 1 `fn_801858C4`, cases 2/3 `fn_8018524C(entry, FALSE/TRUE)`,
case 4 `fn_80184D80`, and case 5 written inline. XD
`_peopleMoveTypeRandomRot__FP13tagPeopleWork` (0x124 bytes, `scope:local`)
switches on `subState` at +0x55 with cases 0 -> 1 -> 2 falling through. Case 0
decays the blend factor and clamps it at 0. Case 1 computes π + `field_40` +
π/2·rand, takes `fmod` by 2π, starts the turn and sets subState 2. Case 2 sets
wait = f·rand + f and subState 0. That is Colosseum's case 5 at 0x801819A0,
including the +0x55 and +0x40 offsets. XD moved the float fields
(0x84/0x88/0x8C -> 0x90/0x94/0x98) and uses `HSD_Randf`/`_getLastFrameSec`, so
the match is structural. The instruction-level score is below 0.45 because of
the differing float code.

### (b) shadow-receiver block

**Colosseum.** No second expansion (`floorDataBiosGetShadowReciveNum`
together with `GScolsys2WalkGetLayer` occurs only here; other ShadowReciveID
users, fn_8025DF38 and wazaSequenceSys, are different loops). The in-place form
emits one instruction *more* than retail (`mr r0,r3 ... mr r27,r0` vs
`mr r27,r3`). The 2026-09-27 clause covers the opposite case, an extra copy
that only inlining produces, so this is not a written fingerprint. XD's
standalone version has the same `mr r0,r3; ...; mr r30,r0` copy as our in-place
build.

**XD.** `_peopleUpdateShadows__FP13tagPeopleWork` (XD 0x802A0328, size
0x114, `scope:local`) is the best hit in XD for 0x80181DC8-0x80181E84 (0.617).
Call order: `floorDataBiosGetCurrentPtr` (`or.` test) ->
`floorGetCurrentGroupID` -> `peopleBiosGetPos(entry, &pos)` ->
`GScolsys2WalkGetLayer(&pos, &layer, &subLayer)` (both zeroed on failure) ->
`floorDataBiosGetShadowReciveNum` -> both layers `< num` -> count 1,
`GSresGetResource(group, ShadowReciveID(floor, layer))` -> if the layers
differ, count 2 and the second surface -> `GSmodelSetShadowSurface(entry->model
(+0x8), count, surfaces)`. XD inserts one extra call,
`gimmickBoxSetKenShadowOnBoxIfOnBoxFloor`. Colosseum's fn_8018FC98 takes the
place of XD `peopleBiosGetPos`: both are 0x24 bytes and both are called as
`(entry, &pos)`. The post-loop call fn_8018F30C (0x164) is XD's
`_peoplePostUpdateShadows__Fv` (0x154), with the same calls: max count,
entry, model, `GSmodelGetVisibility`, position, `GSlightSetTarget`,
`GSlightSetPosition`. In XD the shadow pass
moved out of the daemon into `peopleUpdateShadow` (0x80298768), which calls
`_peopleUpdateShadows(entry)` per person and then `_peoplePostUpdateShadows()`.

Verdict: no Colosseum-internal evidence. For both helpers XD has a same-named
static function with the same body and signature (`tagPeopleWork*`).

## 3. dbgMenu.c `fn_80132C6C` (99.54%)

**Colosseum.** The nine-store item reset (`-1,0,0,0,0,-1,0,0,0` into a 0x20
item) appears only as the unroller's own eight copies inside fn_80132C6C
(canon 1.000, same function) and nowhere else in the DOL or REL. The pool
globals lbl_8047AEB0-lbl_8047AECC are referenced by fn_80132C6C alone (no
reader anywhere in the DOL), so there is no second function that could share a
helper. The slot loop and the pool set-up have no match at 0.7.

**XD.** No function has the item store shape (a grep of `xd-asm/code` for
`sth ..,0x6(` + `stb ..,0x14(` + `stw ..,0x18(` + `stw ..,0x1c(` + `li ..,-0x1`
finds none). The slot/pool set-up has no match at 0.45, and the main-init call
(`0x10, 0x40, 1, 0x2000`) is not in XD. XD's `dbgMenu.cpp` (0x801553A0-0x801560EC)
is an unrelated window-menu module.

Verdict: none found.

## 4. gs_vm.c `fn_800F6D18` / `fn_800F5CA0`, THPPlayer.c `fn_801E2CA8`

- fn_800F6D18: no block has a second expansion at 0.7 (reap loop
  0x800F6D50, free-slot search 0x800F6D90, script lookup 0x800F6DF8, key
  advance 0x800F6E54). The key search at 0x800F6E94 is the already admitted
  `GSvmFindByKey`, confirmed at four other sites. XD has no C VM (it is the
  C++ `GSscript::Tiga`/`AppScript`), and no XD window scores 0.5.
- fn_800F5CA0: no XD or DOL match at 0.5 for its head.
- fn_801E2CA8: its only helper (FillStreamBuffer) is already admitted by
  twelve expansions. XD replaced the SDK THP player with `GSmovieStreaming`
  (`_THPPlayerSetBuffer__FP18__GSmovieStreaming`, etc.), and no XD window scores
  0.45 against fn_801E2CA8. The remaining difference is three swapped register
  pairs with no helper shape.

Verdict: none found.

## Tested helper versions (not committed on the lane branch)

On branch `rc-xd-helper-proposal` (one commit on top of the lane branch), the
XD-shaped helpers were built and compared against the base report:

- floor.c: `static inline void floorInitScene(FloorData* floor)` spanning
  0x80112B8C-0x80112C98 (the unused group-ID call through the sun phase, as in
  XD). `fn_801129CC` 99.89 -> **100.0**, `main/game/floor` 100.0%. No other
  function changes. Linking floor.c would still need its .rodata
  (0x80272088-0x802721FA) and .sdata2 (0x8047CF70-0x8047CF9C) moved into its
  split.
- people.c: `peopleMoveTypeRandomRot(PeopleEntry*)` (case 5) and
  `peopleUpdateShadows(PeopleEntry*)` (shadow block), both named after the XD
  statics. `fn_80181850` 99.52 -> **100.0**. No other function changes. The
  people TU cannot link yet because other functions in it are still not exact
  (fn_8018524C 99.34, fn_8018CD08 98.17, and others).
