# toolentryDebugPokemonCreate matching evidence

Status: function byte exact and owner linked; all seven canonical hashes pass.

## Parent Integration

The owner is now `Matching`. `TOOLENTRY_DEBUG_ONLY` limits its emitted code to
the declared debug function; existing sibling wrappers retain their original
guards. The existing `toolentryTaisenFreePokemonData` definition stays visible
as static inline in this owner because the compiler expands it into the caller.
Removing that definition prevented inlining and failed the hash check; keeping
it inline-only preserves the exact caller without emitting a duplicate symbol.
The complete DOL and six configured REL hashes passed after this integration.

- Symbol: `toolentryDebugPokemonCreate`, `0x8025CDB8`, 692 bytes.
- Owner: `main/game/toolentry`, source `src/game/toolentry.c`.
- Baseline objdiff score: 99.45087%.
- Final source score: 100%. No compiler-setting changes, assembly or `.inc` edits.
- The owner contains this one report function. The source also supplies sibling
  wrappers, so any eventual source fix must check those wrappers for regressions.

## Exact Result

The register-allocation trace led to isolating the initial creation loop in
`toolentryCreateDebugPokemonList`. Its pointer and index are allocated before
the release/copy temporaries, correcting their r30/r31 allocation. Reusing `j`
for the second initialization loop's player index and `i` for its Pokemon index
corrects the remaining nine register operands. The helper preserves every call,
argument, global-bound reload and pointer increment. The changed loop-counter
values are overwritten or dead before any subsequent use.

This is a one-use inline boundary chosen for matching, not proof of an original
helper. It is explicitly tagged `RULE-EXCEPTION(user-approved)` under the existing
byte-match-first decision. The parent must register this additional shaping in
`docs/RULE_EXCEPTIONS.md`; no policy category needs changing. Suggested dedicated
row (the earlier helper/empty-loop row remains applicable):

| Function | Source | Rule | Evidence | Clean fix |
| --- | --- | --- | --- | --- |
| toolentryDebugPokemonCreate (100%, pending parent link) | src/game/toolentry.c | One-use inline boundary and loop-index reuse for allocation | `toolentryCreateDebugPokemonList` holds the unchanged initial creation loop; using j/i in the second initialization loop completes the retail register allocation. Complete 692-byte function and all 44 relocations match; the helper emits no standalone symbol. See this evidence document. | Recover the original helper and local-lifetime organization without the matching-only boundary. |

Verification used the existing ELF parser in
`tools/decomp_work/permuter/owner_extract.py` against the retail target object
and the canonical GC/1.3 source object:

- Raw function bytes: 692/692 identical.
- Function-byte SHA-256 on both sides:
  `17ab8f53062ca15bf84aee5551a66d29fbbec5a09069a632cc9e77b65560a655`.
- All 44 relocations have identical function-relative offset, type, target
  symbol name and addend.
- No nonempty allocatable non-text sections occur in either object.
- No `toolentryCreateDebugPokemonList` standalone symbol is emitted.
- objdiff reports 100% for the function and complete target text section.
- Rebuilt included-source siblings remain exact: `fn_8025D164`, `toolentryCopyHero`,
  `toolentryTaisenGetEntryPokemonNum`, and `toolentryTaisenGetPokemonNum` all 100%.

The following sections retain the unsuccessful first pass and diagnostic probes
so future cleanup does not repeat them.

## Previous Residual

The original baseline had the same 173 instructions and control-flow shape as retail.
There are 18 register-operand differences: retail's release handles and hero/
Pokemon copy sources use r30, while the candidate uses r31; the final four-player
copy loop uses the opposite register. This is not byte exact, despite the small
weighted-score gap. No linkage was attempted with the candidate substituted.

The generated draft in batch `71e57738f2f4f348400c` is available at the harness
artifact directory ending in
`94cba5e3194c3c36418b5536db45913c5e53056b9534d15d963e01f064144470`.
Its direct-copy expressions confirm the existing call order and sizes, but its
inferred narrow loop variables and pointer/integer declarations are not accepted
source or matching evidence by themselves.

## Earlier Experiments

None of the following improved the baseline score:

- Reusing or separating the stage-specific player and Pokemon loop indices;
  block-local indices; for/do-while forms; equivalent continue/else forms.
- Direct hero-copy statements in place of the existing inline helper. A version
  with the initialization-loop indices adjusted left only the two release-handle
  register pairs and a redundant move at the first savedata copy. Its additional
  instruction reduced the actual objdiff score to 99.27746%, even though its
  aligned instruction display looked closer.
- Correcting local declarations against the existing definitions of
  `fn_8006B09C`, `savedataGetStatus`, and the GSmem handle operations.
- Aggregate hero copies, using the layout established by `heroBiosCopy` and
  `toolentryCopyHero`. MWCC emitted an inline copy loop instead of retail's
  memcpy call; this form was rejected.
- An isolated C++-language probe, motivated by the documented toolentry.cpp
  origin. This did not improve the match and does not justify changing language.

The following serial compiler probes used the unchanged complete source and
separate ignored object outputs. All other project options were retained.

| Compiler | Optimization | Scheduling | Score |
| --- | --- | --- | --- |
| GC/1.2.5 | O4,s | on | 75.87862% |
| GC/1.2.5 | O4,s | off | 70.28902% |
| GC/1.2.5 | O4,p | on | 72.87862% |
| GC/1.2.5 | O4,p | off | 66.79191% |
| GC/1.3, GC/1.3.2, GC/2.0 | O4,s | on | 99.45087% |
| GC/1.3, GC/1.3.2, GC/2.0 | O4,s | off | 88.34682% |
| GC/1.3, GC/1.3.2, GC/2.0 | O4,p | on | 92.71098% |
| GC/1.3, GC/1.3.2, GC/2.0 | O4,p | off | 81.57226% |

These are diagnostic results, not permission to change compiler options.

## Register-allocation trace

The local sibling `mwcc-debugger` and `retrowin32` tools successfully traced the
unchanged source using their supported GC/2.6 compiler. This is a diagnostic proxy,
not acceptance under the canonical GC/1.3 configuration.

Ignored outputs are under `build/GC6E01/toolentry_regalloc/`. The most useful files
are `regalloc-gpr-pass-1-assigned.txt`, `regalloc-gpr-pass-1-all.txt`,
`backend-10-before-regalloc.txt`, and `frontend-01-ast-after-optimizations.txt`.

The trace explains the baseline's coloring: the inlined first release handle
is allocated r31 before other saved-register values, then the hero-copy source
temporaries also take r31. The final player-index range interferes with its copy
source and is assigned r30. Ordinary declaration changes that leave the same
frontend range splits do not address this ordering. A useful next investigation
is the original helper and variable-lifetime structure, using the trace to check
whether a semantically evidenced reconstruction changes the first saved-register
assignment. Dummy aliases, register-only shaping and new pragmas were not adopted.

## Reproduction and Integration Gate

All builds, diffs and compiler/debugger probes in this investigation ran through
the campaign build lock as worker `CodexToolentry`.

```sh
python3 tools/local_campaign.py build --worker CodexToolentry -- \
  python3 tools/decomp_work/fdiff.py toolentryDebugPokemonCreate main/game/toolentry
python3 tools/local_campaign.py build --worker CodexToolentry -- \
  build/tools/objdiff-cli diff -p . -u main/game/toolentry \
  -o build/GC6E01/toolentry_current.json --format json
```

The completed byte-exact owner and sibling audit permit the parent to change
`(CodeCandidate, "game/toolentry.c")` to `(Matching, "game/toolentry.c")` in
`configure.py`, rebuild the report, perform the full link and check all canonical
hashes. The final link and canonical hashes are intentionally left to the parent,
which owns `configure.py` and the shared exception registry. No link acceptance is
claimed before those steps succeed.
