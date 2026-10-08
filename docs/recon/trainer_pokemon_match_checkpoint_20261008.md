# Trainer Pokemon Matching Checkpoint

Worker: `CodexToolentry`. Status: **incomplete / CodeCandidate**.
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
