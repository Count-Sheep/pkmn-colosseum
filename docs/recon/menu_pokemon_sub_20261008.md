# menuPokemonSub exact owner, 2026-10-08

## Scope and status

CodexVerify matched `menuPokemonSub` at `0x8001B1EC` in
`main/game/menuPokemon_candidate_8001B1EC`. The retained source is 100%:
all 2,264 bytes and all 88 relocation records match retail. This is
working-tree evidence based on `7fe1001712fa82b3d9be5db53d9a6fe95970e97f`,
not a committed acceptance pin or a native-port execution claim.

The parent owns the `CodeCandidate` to `Matching` promotion, the shared
exception-row integration, and the final full link/seven canonical hashes.
The worker's pre-promotion report had 100% code/functions and
`metadata.complete: false`. That distinction must not be mistaken for a
remaining byte mismatch. No configure.py or RULE_EXCEPTIONS.md edit,
commit, push, or native-source change was made by this worker.

## Retained source and flags

The source changes are in `src/game/menuPokemon.c` and
`src/game/menuPokemon_candidate_8001B1EC.c`:

- The wrapper defines `MENU_POKEMON_SUB_ONLY`. Its corresponding carve
  includes only the requested function and its inlined dependencies,
  instead of emitting unrelated functions from the parent source.
- The carve restores declarations and the full source's inherited
  `optimization_level 4` / `peephole off` state. The ordinary source and
  existing sibling carve guards remain available.
- A single-field `u16` slot carrier replaces the former `u16` loop index;
  a single-field `s32` action carrier retains the action result inside
  case 2. Their types, values, tests, increments and external effects are
  unchanged. They correct MWCC's register choices and remove extra moves.
- The summary lookup in case 2 uses `menuPokemonGetSummaryPokemon`, an
  inline copy of the existing `menuPokemonGetPokemon` lookup with a
  single-field pointer carrier. It retains every range check, trainer
  fallback, pointer lookup, early return and validity check. The pointer
  field is initialized to zero before the switch, just as the original
  scalar was initialized. This helper has one caller and emits no function
  or allocated data.
- The final case-7 lookup still uses the original lookup helper and the
  existing `pokemon` local. No speculative m2c pointer types or expanded
  draft control flow were retained.
- The existing `cursor = work + 5; *++cursor = 0;` address form remains.
  It was already documented as allocation shaping.

No compiler or unit-flag change is needed: retain GC/2.0, the existing
base `-O4,p -inline auto` flags, and
`-use_lmw_stmw on -sdata 8 -sdata2 8`.

The decisive result was not a rewrite of the final lookup. After the slot
and action changes the function was 99.902824%, with eleven words using
r21 instead of retail's r22 in that lookup: five `li`, two `mr` destinations,
three `mr` sources, and one `cmplwi`. The summary-only inline pointer
carrier corrected that allocation without changing any other word.
A thin wrapper around the old helper did not preserve the match and was
not retained. Reusing the original `pokemon` local did preserve it.

This is exception-backed byte-match-first C, not a claim of recovered
original source abstractions or a strict unshaped-C result. The source
contains its local RULE-EXCEPTION marker. It introduces no assembly,
volatile barrier, opaque predicate, artificial runtime operation, new
external symbol, or policy category.

## Complete owned sections

The target and rebuilt source owner contain exactly one emitted function,
`menuPokemonSub`. Their only nonempty allocated section is:

| Section | Bytes | Alignment | Flags | SHA-256, identical target/source |
| --- | ---: | ---: | ---: | --- |
| .text | 2264 | 4 | 6 (allocated/executable) | eb88a2d10052ebc55f7836eec31fbffbb9b730adba98397057fe340a8192da88 |

There is no additional allocated data or emitted helper. The strict object
audit checks raw function bytes, relocation offsets/types/addends/symbol
identities, function binding/visibility, and complete allocated section
bytes/sizes/flags/alignment. No unresolved compiler-local constant or
constant-pool equivalence exception is needed for this owner.

Detailed generated evidence is in
`build/decomp-support/menu_sub_owner_audit_20261008.json`.
The retained source hashes recorded there are:

- menuPokemon.c:
  `3921d9f9e1eeb62619b4b7bf63243f64eb0bdfeb49f7283865e0248c67b3eefa`
- menuPokemon_candidate_8001B1EC.c:
  `6bea0975cc7d17aa94dc5ade24b6a49571d97e77d55b756ae107c842ce175d47`

## Included-source siblings

The final audit discovers configured owners through recursive quoted C
source includes, including the indirect 8001C7B8 and 8001D378 wrappers.
It rebuilds all 19 configured owners that include menuPokemon.c, plus the
separate dbgMenuClose owner. All checks run under the shared CodexVerify
build lock. No further permutation or source trial is part of this audit.

Fourteen owners have exact retail function bytes and relocations, identical
complete allocated sections, and the same emitted function set:

| Owner under main/game | Exact functions |
| --- | ---: |
| menuPokemon_island_80018594 | 2/2 |
| menuPokemon_island_80018F30 | 8/8 |
| menuPokemon_island_800194D0 | 1/1 |
| menuPokemon_island_800195E0 | 2/2 |
| menuPokemon_island_80019938 | 3/3 |
| menuPokemon_r57_80019B48_gc20p1_o4s | 1/1 |
| menuPokemon_r57_80019D5C_o1 | 1/1 |
| menuPokemon_r57_80019F6C_suffix | 4/4 |
| menuPokemon_candidate_8001B1EC | 1/1 |
| menuPokemon_island_8001BAC4 | 1/1 |
| menuPokemon_candidate_8001BEBC | 1/1 |
| menuPokemon_candidate_8001C064 | 1/1 |
| menuPokemon_r50_8001D378_o3 | 1/1 |
| dbgMenu_exact_801337E4 | 1/1 |

Six siblings were already incomplete. Each target-owned function was
compared against a freshly compiled HEAD-source baseline using its actual
unit compiler and flags. Their function bytes and relocation records are
unchanged, rather than merely showing the same rounded score:

| Owner under main/game | Function | Report match | Baseline result |
| --- | --- | ---: | --- |
| menuPokemon_r57_800181C4_prefix | fn_800181C4 | 98.090164% | unchanged |
| menuPokemon_candidate_80018A68 | fn_80018A68 | 98.64379% | unchanged |
| menuPokemon_candidate_800192A8 | fn_800192A8 | 99.60145% | unchanged |
| menuPokemon_candidate_800194E4 | fn_800194E4 | 99.68254% | unchanged |
| menuPokemon_candidate_80019754 | fn_80019754 | 92.85124% | unchanged |
| menuPokemon_r50_8001C7B8_prefix | fn_8001C7B8 | 98.46676% | unchanged |

This does not claim those six incomplete units are exact or link-ready.
Generated sibling evidence:
`build/decomp-support/menu-sub-siblings/audit-baseline.json`.

## Three verification gates

The prior gate work is preserved and rechecked after the retained changes:

| Gate | Complete owner | Exact functions | Bytes / relocations |
| --- | --- | ---: | ---: |
| fn_80019070 | menuPokemon_island_80018F30 | 8/8 | 888 / 40 |
| menuPokemonOpenSub | menuPokemon_island_8001BAC4 | 1/1 | 552 / 29 |
| dbgMenuClose | dbgMenu_exact_801337E4 | 1/1 | 44 / 3 |

The redundant fn_80019070 optimization pragma remains removed.
menuPokemonOpenSub retains its documented peephole control and existing
shared-constant import; its audited object is the final post-import object.
dbgMenuClose remains exact without its old propagation-control pragma.
See `native_menu_verification_20261008.md` for their provenance. This note
supersedes that note's stale 99.04594% MenuSub dependency statement, but
does not certify a whole native closure.

## Proposed shared rule row

The parent should replace the existing MenuSub row, preserving the current
policy and updating linked status only after promotion and final checks:

| menuPokemonSub (party-menu main loop; 100%, owner menuPokemon_candidate_8001B1EC) | src/game/menuPokemon.c | Address form, scalar carriers, summary-path inline copy, inherited compiler controls | Cursor pre-increment retains retail's r30/frame. u16 slot and s32 action carriers preserve the original values. A summary-only copy of menuPokemonGetPokemon uses a pointer carrier, correcting the later case-7 allocation to r22 without emitting a helper or data. The isolated carve restores inherited O4/peephole-off state. All 2264 text bytes, 88 relocations, and the complete allocated owner layout match. Evidence: docs/recon/menu_pokemon_sub_20261008.md. | Recover the original source abstractions and reunited TU settings without allocation-specific carriers or duplicated inline boundaries |

## Validation and integration boundary

The scoped working-tree quality scan, all 41 quality-scan tests, the
2,315-file live-wrapper scan, and `git diff --check` passed. Earlier full
`ninja -j 4` passed all seven canonical hashes before MenuSub promotion;
that was a regression check, not proof of this source object's linked use.
The parent performs the final full link and records the committed revision.
Existing Shift-JIS and compiler/linker warnings remain.

Of 33 direct function references in this owner, the pre-integration report
still places fn_8001C7B8 (98.46676%), fn_80097E58 (100% function in an
incomplete gbaCommunication owner), and memset (100% function in an
incomplete CRT owner) in incomplete units. These are static observations,
not new headed stops or claims about native library binding requirements.

## Final integration confirmation

The parent subsequently confirmed that
`game/menuPokemon_candidate_8001B1EC.c` was promoted to `Matching` and the
post-promotion full link passed all seven canonical hashes. MenuSub is
therefore exact and linked; this supersedes the pre-promotion status and
pending-link statements above. The final link result is parent-reported,
separate from this worker's recorded byte/relocation and sibling audits.
The committed acceptance revision remains for the parent handoff to record.

The final locked audit covered all 20 relevant owners: fourteen exact
owners, six incomplete siblings unchanged from freshly compiled HEAD
baselines, and all three verification gates still exact. Both CodexVerify
claims (menuPokemon.c and menuPokemon_candidate_8001B1EC.c) were released
after that audit. No owned build remained running, and no further source
edit or permutation was performed.
