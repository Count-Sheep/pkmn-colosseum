# Trainer Pokemon Matching Checkpoint

Worker: `CodexToolentry`. Status: **byte-exact and linked (Matching), 2026-10-10** — see the last section.
Claim released after final validation. Former token:
`afa28eaf165a4bfa8b76463512c5809f`.

## Measurement

- Function: `fightTrainerCreateFightTrainerPokemonDataIdToPokemon`, `0x801F9F78`.
- Unit: `main/game/fight_trainer_r51_801F9F78_gc20_o2`.
- Edited owner: `src/game/fight_trainer_range_801F7F80.c`.
- Canonical GC/1.3 object: **98.33731%**, up from **97.740295%**.
- Retail and candidate text both contain 335 instructions / 1,340 bytes.
- Direct ELF comparison: 68 instruction words differ, containing 96 differing
  bytes. This is NOT a byte-exact or link-ready result.
- All 45 relocation tuples match, including section-relative offset, relocation
  type, symbol identity and addend.
- Both objects have only one nonempty allocated section: `.text`, size 1,340,
  alignment 4. The candidate emits only the requested function, no helper body.
- Measurement JSON: `build/GC6E01/trainer_checkpoint.json` (local build artifact).
- Source SHA-256 at checkpoint:
  `772ec3927ebf58a86fd3aca1d98884a62795080d1839d53519f9990d33d1cfd2`.

All compilation, objdiff and ELF verification ran through
`tools/local_campaign.py build --worker CodexToolentry -- COMMAND`.
The owned build sessions are drained. No configuration/shared documentation,
native source, commit or push was changed for this checkpoint. Sibling units and
the full canonical link have not been revalidated for this incomplete candidate.

## Retained Changes

The candidate restores the two missing copy instructions using explicit move-slot
conversion and a separate 16-bit shadow index. The trainer argument is a numeric
ID: the accepted `pokemonCreateRndFit` definition takes `u32 id`, not a pointer.
Separate stat and move counters improve allocation. A widened `u64` trainer-ID
carrier currently changes allocation without generating 64-bit operations.

That carrier and redundant conversions are provisional byte-match-first shaping,
not a claim about original source. The source now has the requested scoped
`RULE-EXCEPTION(user-approved)` tag, explicitly marked incomplete. Before policy
acceptance, either replace the shaping with a natural exact form or have the
parent add the corresponding existing byte-match-first `RULE_EXCEPTIONS` row.
The tag does not claim that the shared registry row already exists. No new
policy category is requested.
No assembly, `.inc`, or compiler-control pragma remains in this candidate.

## Residual And Next Approach

The main Pokemon pointer now uses retail's r31. Species, name, level and the
shadow index also have the desired long-lived registers. The held-item call
result still receives r30 instead of r24, shifting dark ID, sex, nature, ability,
friendship and trainer ID down one register. Stat-loop counter allocation and
effort-array address setup also differ. The final shadow-stat setter reverses
the index conversion and signed-value conversion order.

The next useful target is the held-item result's lifetime/coalescing boundary,
followed by stat-loop counter ownership. A diagnostic 64-bit item carrier put
all long-lived registers in the correct locations, but lost the required 16-bit
item-ID mask; it was rejected, not retained as progress. Explicitly restoring the
mask reintroduced the high-priority call-result temporary. A larger natural
initialization/application inline boundary is still worth trying; trivial
aliases, item getter/setter wrappers and type round-trips did not solve it.

Register-only debugger capture is available in the ignored
`build/GC6E01/trainer_regalloc_probe.py`: GC/2.6 diagnostic proxy, expensive
AST/PCode dumps disabled, 25-second deadline, process groups drained in `finally`.
It completed in seconds for the direct body. GC/2.6 does not inline the large
whole-function helper exactly like GC/1.3, so do not treat such a trace as the
canonical allocator. Canonical GC/1.3 object bytes remain the authority.

Batch `71e57738f2f4f348400c` has a partial m2c draft for this target. Its
`fightTrainerGetStatus` context is wrongly declared void, producing unset return
register errors; it is not a drop-in implementation. The apparently impossible
four-way move-ID conjunction is present in retail and has been preserved.

## Final Bounded Experiment Checkpoint

At the parent's wrap-up request, all owned build sessions were drained and the
best source restored. Final reproduction command:

```sh
python3 tools/local_campaign.py build --worker CodexToolentry -- python3 build/GC6E01/trainer_checkpoint_verify.py
```

This rebuilt the canonical GC/1.3 object, reran objdiff, and compared text,
relocations, allocated sections, and definitions using the ELF parser. Detailed
local evidence is `build/GC6E01/trainer_checkpoint_evidence.json`. Target and
candidate both have exactly one defined function, global/default visibility;
`.text` flags are 6 and alignment is 4. No helper or data definition remains.

The following scratch-only trials produced no improvement and were rejected:

- Combined ability/item inline application helper: same 98.33731% with a signed
  32-bit argument. A wide argument corrected long-lived register choices but
  replaced the required item-ID mask with a move; 98.32239%, not acceptable.
- Twelve narrowing forms, including explicit casts, masks, and shifts. The
  correct mask brought back call-result coalescing; one wide shift expression
  triggered an MWCC internal compiler error. No such expression was retained.
- C one-field carriers with direct, return-value, and output-pointer fetches:
  stack traffic and 1,352/1,356-byte bodies, rejected.
- C++ conversion/assignment carriers, including reference application helpers:
  no improvement. Diagnostic no-peephole/nopropagation combinations grew the
  body. No C++ or compiler-option change was retained or requested.
- Larger post-fetch application helpers starting at creation, stat application,
  or item application, with normal/reversed parameter order: at most 98.247765%.
- Duplicated high/low-word item carriers and alternate narrowing combinations:
  no improvement, rejected.

Scratch scripts/artifacts are under `build/GC6E01/trainer_*_trials*`. Each batch
was serial and bounded under the campaign lock. No new debugger capture was
started, and James's/Main's scratch files were not changed.

The residual remains the held-item anonymous call-result temporary taking r30
instead of r24, shifting six other long-lived values; stat-loop register choices,
effort-base setup order, and the final shadow-stat argument preparation also
differ. A fresh allocator trace of the restored body, focused on why the item
copy coalesces, is more promising than repeating the rejected carrier forms.

No Matching change is warranted. Sibling units, aggregate link, seven hashes,
and shared policy validation remain for the parent; this checkpoint does not
claim those checks passed. The wrapper was not modified. No commit/push or
shared configuration/documentation edits were made by this worker.

## Follow-up, 2026-10-10 (agent b6): 99.23881%

Three source changes, all measured on GC/1.3 (GC/1.3.2, 2.0, 2.5, 2.6 and 2.7
emit identical code for this unit, so the GC/2.6 mwcc-debugger replay is
faithful here):

- `stat` is declared between `nameId` and `level` and is used only by the
  first stat loop; every other loop uses `i` (brute-forced over all 128
  i/stat assignments).
- The final shadow-stat value is held in a loop-local `u16` before
  `fn_801EE894`, which gives retail's index-before-value argument order.
- `pokemonSetStatus` is declared `void` inside the function (its definition
  in `pokemon_range_8011F5FC.c` is void). The u32 prototype adds a
  result-copy block after each call, which reverses the order in which loop
  code motion hoists the effort/effortIds bases.

What remains is the held-item value alone (50 words, all of them register
renumbering). The replay shows the cause: `item = fightTrainerGetStatus(...)`
lowers to `mr r91,r3; mr r41(item),r91`. Backend copy propagation then
replaces every use of r41 with the call temporary r91, the highest vreg in
the high-degree group, so it pops right after `pokemon` and takes r30. Retail
colours item as the named vreg r41 (between `trainer` and `darkIndex`).
Small-file probes show that the copy survives only if item has a persistent
move use into a physical register (a plain call argument) or a second
reaching definition. Stores, compares, masks, casts, inline helpers, getters,
switch, `register`, int/u32/void*/enum return types, initialisers and the
`opt_*` pragmas do not stop it.

With the item temporarily passed unmasked in the zero branch (a diagnostic,
not retail), every other register, loop and base matches. That leaves 4
words: that call plus one `addi r29` scheduling slot. An `s64`/`u64` item
carrier (or an unprototyped `pokemonDoItemSoubi`) gets to 3 words, but MWCC
then passes the low word with `mr r4` and drops retail's `clrlwi r4,r24,16`.
Every form that restores the mask brings back the propagation. An `s16` item
also colours correctly, because the `extsh` def is not a copy, but it emits
`extsh`/`extsh.` where retail has `mr`/`cmpwi`.

Next idea: find a construct that gives `item` a persistent physical-register
move use, or a second reaching definition, which later passes delete.

## Resolution, 2026-10-10 (agent b2): 100%, linked

The held-item copy is avoided rather than kept alive. MWCC lowers a call
result into a named variable through a temporary (`mr r92,r3; mr r42,r92`)
only when the assignment is a plain copy. When the value passes through an
operation, the operation reads r3 directly (`extsh r42,r3`, `rlwinm r42,r3`),
and a copy from a physical register is never propagated (this is why
parameters keep their own webs). The one operation that is an identity and
still prints as `mr` is `or rD,rS,rS`, so:

```c
item = itemValue = fightTrainerGetStatus(NULL, pokemonDataId, 0x16, 0);
item |= itemValue;
```

lowers to `or r42,r3,r3` (`itemValue` is dead after propagation). The
frontend folds `item |= item`, `t | t` after `t = ...`, and `(t = x, t | t)`,
so the separate `itemValue` operand is needed. `(t = x) | t` also reaches 100%
but is unsequenced (undefined behaviour), so it was rejected. Other identity
operations with a backend-only constant (`<< zero`, `| zero`, `^ zero`,
`>> zero`, `& minusOne`) keep the named web but print as `rotlwi`, `ori`,
`xori` or `srawi`: 99.82089% with one word left.

With the item web fixed, the `u64` trainer carrier is no longer needed:
`s32 trainer` (a signed local copy of `trainerId`) matches; `u32` or the bare
parameter gives 99.149254%. The `(u16)` on `darkIndex = darkId` and the
`(u8)` on the two `fn_801EE8F4(darkIndex)` tests were redundant and are gone;
the remaining conversions are required (removing any costs 20–100 words).

Result: 100% on GC/1.3 with the unit's existing flags. The unit is `Matching`,
a fresh full link ends `7 files OK`, and `check_regression.py` against the
b8e1238de baseline reports no regressions.
