# Trainer Setup Dependency Audit

Read-only audit of `fn_8006B5D0` and `toolentryDebugPokemonCreate` using the
existing native call graph and `build/GC6E01/report.json`, last written
2026-10-08 20:29:02 UTC. The call-graph cache fingerprint was independently
verified against all 2,445 disassembly inputs and matches.

## Actual Decomp Gap

Only one additional unmatched or unlinked game function was found in the
transitive graph of either root:

- `fightTrainerCreateFightTrainerPokemonDataIdToPokemon`, `0x801F9F78`:
  **97.740295%, unlinked**, 1,340 bytes.
- Unit/source wrapper: `src/game/fight_trainer_r51_801F9F78_gc20_o2.c`.
- Source owner included by that wrapper:
  `src/game/fight_trainer_range_801F7F80.c`.
- Path: `fn_8006B5D0` -> `fn_8006AABC` ->
  `fightTrainerCreateFightTrainerDataIdToHero` ->
  `fightTrainerCreateFightTrainerPokemonDataIdToPokemon`.
- Its unit owns only that function. This is a static dependency, not a new
  headed-run observation.

## Other Results

All direct game callees of both roots are 100% and linked. The visible
transitive walk visits 929 symbols from `fn_8006B5D0` and 901 from
`toolentryDebugPokemonCreate`, stopping expansion at the native tooling's
host-library boundaries. It continues through incomplete game functions to
avoid hiding deeper gaps. There are no symbols absent from the report in
either walk. The toolentry root has no additional transitive game-code
matching/linkage gap.

Root acceptance itself is excluded: `fn_8006B5D0` is exact but awaits its
parent's `Matching` configuration change; toolentry is being edited by its
assigned worker, so its transient report score is not an acceptance result.
`memcpy` is a host-boundary CRT dependency, not a new game decomp target.

This audits report exactness and unit linkage, not source-policy provenance
or native bindings. Static edges include direct calls, tail calls and visible
address-taken callbacks. Generic pointer tables and unresolved runtime
dispatch are not expanded; this is not a proof that all runtime paths are
clear. No source/configuration changes or new matching work were undertaken.
