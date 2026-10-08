# Native menu verification gates, 2026-10-08

CodexVerify audited the working tree based on
`7fe1001712fa82b3d9be5db53d9a6fe95970e97f`. This is integration evidence,
not a new committed acceptance pin. Other source workers were active; no
native code, configure.py, shared exception registry, or other owners were
edited. No commit or push was made.

## Changes and acceptance

- Removed the redundant `optimization_level 4` before `fn_80019070`.
  The owner's existing GC/2.0 `-O4,p -opt nopeephole` flags suffice.
  All eight owner functions remain exact, not just this function.
- Removed the complete `push / opt_propagation off / pop` block from
  `dbgMenuClose`. Its unchanged C body is exact under existing GC/1.3 flags.
  No replacement compiler flag is required.
- Retained `menuPokemonOpenSub`'s scoped `peephole off`, adding a local
  exception marker. Removing it changes instruction scheduling and merges
  the low-halfword mask/compare into a record-form instruction. The trial
  fdiff had 129/143 aligned entries identical (90.2%, not an objdiff score).
  Restoring the existing setting restores exact raw bytes and relocations.
  This remains exception-backed acceptance, not a strict-policy win.

The existing byte-match-first decision in docs/RULE_EXCEPTIONS.md permits
tagged, documented compiler controls. No new policy category is requested.
The parent must integrate the documentation addition below before marking
the OpenSub policy gate cleared.

## Full owner evidence

After rebuilding source objects, `m2c_object_evidence.compare` passed for
every owned function. This compares raw function bytes, relocation offsets,
types, addends, symbol identities, and function symbol attributes. No
constant-label normalization was needed. Allocated section bytes, sizes,
alignment and flags also agree exactly. All three objects contain only
allocated `.text` (alignment 4, flags 6), with no extra allocated sections.

| Owner in main/game | Functions | Exact bytes | Exact relocations |
|---|---:|---:|---:|
| menuPokemon_island_80018F30 | 8/8 | 888 | 40 |
| menuPokemon_island_8001BAC4 | 1/1 | 552 | 29 |
| dbgMenu_exact_801337E4 | 1/1 | 44 | 3 |

Target and candidate text SHA-256 values are identical:

- 80018F30 owner: `aa727fe94c6caac4f3b2d276d3a14461352334b287b8265465e2eabe7370d152`
- fn_80019070 alone (104 bytes, 4 relocations): `9429e93ed35bb4d04820ce42e5a30bfe3c2c8aee0dfbe4c3cd2fafcdd11745db`
- menuPokemonOpenSub: `8eed5b8722a883c4a07dbf86fdbd1cf9b98364ea8b6f605eeaafdaeecb7118db`
- dbgMenuClose: `ecc4764e4fd97dc858a61891fa3041441324707507f672c8f5f7a9d09e1c2a4a`

The eight island functions are fn_80018F30, fn_80018F54, fn_80018F88,
fn_80019064, fn_80019070, fn_800190D8, fn_80019118, and fn_80019204.
OpenSub's existing guarded constant_import redirects its generated signed
conversion bias to lbl_8047B7B8. The audited object is the final post-import
object used by the linker. The existing constant-import exception remains
necessary and was not removed or broadened.

Fresh report: 2026-10-08T20:26:19.711936+00:00, SHA-256
`d8d6b5ac78173fd8654fbefbc6aa6c9c520fbc18e3b4682e31e52878d9c65d48`.
All three units are Matching/complete, 100% code and functions. Generated
build.ninja confirms all three linked source objects. No newly linked or
newly exact progress is claimed.

## Provenance and dependencies

- fn_80019070's allocation-local source was matched in
  `524be88d4177dc5ad82e87ce0125ca2de9df86aa`; its eight-function island was
  linked in `323a83c4c`. Its existing block-local-pointer exception remains.
- OpenSub's byte-sized first parameter was accepted in
  `659a386c2abccc2a88f263a64d8ee10da5223354`; its constant-import island was
  linked in `2ab7a51cd`. This audit establishes measured reconstruction,
  not a claim that the original source used these pragmas.
- dbgMenuClose was linked in `84c30c783`. Its removed pragma was unnecessary
  in the present compiler configuration.

fn_80019070 has no direct calls. dbgMenuClose calls only menuClose, exact
and linked. Seven of OpenSub's eight distinct direct callees are exact and
linked; **menuPokemonSub remains 99.04594%, unlinked** in
main/game/menuPokemon_candidate_8001B1EC. Thus acceptance of OpenSub itself
does not authorize bypassing that dependency or certify its full closure.
This audit does not claim native binding completion or headed execution.

## Validation and parent integration

Claims were held under CodexVerify for menuPokemon.c, both island wrappers,
and dbgMenu_exact_801337E4.c. Exact functions are absent from the unmatched
queue, so CLI claims rejected them; the same Coordinator.claim API reserved
their actual owners without altering the queue. All builds, diffs, and
verification ran through `local_campaign.py build --worker CodexVerify`.

Passed: targeted object builds; `ninja all_source build/GC6E01/report.json`;
full `ninja` with **7 files OK**; no-live-wrapper scan (2,315 files); all 41
quality-scan tests. Existing Shift-JIS and compiler/linker warnings remain.
Parent must rerun final committed-diff policy checks and fresh full build
after combining workers, then record the committed SHA in the handoff.

Required shared documentation changes, intentionally left to the parent:

1. Update the existing fn_80019070 exception row's stale owner/status from
   menuPokemon_r57_800181C4_prefix/unlinked to
   menuPokemon_island_80018F30/linked. Keep its pointer-allocation exception;
   note that the redundant optimization pragma was removed and cite this note.
2. Extend the existing menuPokemonOpenSub row (currently constant_import
   only) to cover its existing scoped peephole-control pragma in
   src/game/menuPokemon.c. Explain that it preserves the retail instruction
   schedule and separate mask/compare; 552 bytes and 29 relocations match.
   A clean fix needs natural source/original TU flags without local controls,
   plus reunion with the shared bias owner to remove constant_import.
3. No dbgMenuClose exception row or configure.py change is required.

The relevant source changes are limited to menuPokemon.c and
dbgMenu_exact_801337E4.c plus this note. Existing floor_character.c,
debug_source.c and concurrent workers' changes were untouched. Wrappers,
configuration and `.inc` files were not changed. No final clean-tree or
committed-revision verification is claimed by this worker.
