# fn_802232F4 matching research

Updated 2026-10-11. Native port blocker: `main/game/fight_range_802230BC`.
Source owner: `src/game/fight_range_80211A00.c`, compiled through the original
`src/game/fight_range_802230BC.c` wrapper. The experimental C++ owner is removed;
the original C path and split remain unchanged. Research began at revision:
`f2fe8946395f152b0e19df3209539cb08f353d9c`.

## Verified result

- `fn_802230BC`: 100%, 568 bytes.
- `fn_802232F4`: 100%, 1840 bytes, 460 instructions.
- Whole unit: 100%, 2408 code bytes, two functions, `complete: true`.
- The entire .text section is byte-identical to retail. All 22 sibling and 97
  target relocations match; no extra data, helpers, or symbol drift.
- configure.py marks the unit `Matching`. The generated main.elf link graph
  names `build/GC6E01/src/game/fight_range_802230BC.o`, not the retail object.
- The source-backed full link passes all 26 canonical DOL/REL hashes.
- New exact-source progress: one function / 1840 bytes. Newly linked progress:
  two functions / 2408 bytes. The previously exact sibling was not newly matched.
- The final implementation is C. `statusMenuId` is u16 and `gridOpened` is a
  plain u8, declared together near the end of the locals. The buffer is read
  directly through `lbl_80478278`. No C++ wrapper, grid aggregate, explicit
  buffer local, level-copy identity, or loop-invariant pragma remains.
- Retained shaping: declaration order, existing scalar EXP carriers, initialized
  pointer/alternate-menu copies, the alternate-menu identity OR, and the scoped
  `opt_propagation off`. These are tagged under the existing user-approved policy.

No native-port source was changed or headed run claimed. Publication state is
separate from this local build validation.

The following sections are a chronological research record; their earlier
scores, source-owner descriptions, and incomplete states are superseded above.

## Searches already tried

Earlier passes covered declaration permutations, register qualifiers, scalar
widths, pointer and return types, one-field aggregates, field layouts, variable
reuse, inline identity helpers, function prototypes, loop shapes, compiler
options, harness rewrites, and decomp-permuter. These did not reach exactness.

The follow-up pass tested larger inline EXP-stage helpers, nested guards versus
continue statements, correctly incrementing do/while forms, union lifetime
groups, pointer arithmetic for oldLevel, inline output parameters, big-endian
low-byte union access, 64-bit intermediates, and independent statement ordering.
None improved on 99.652176%. Trials were compiled and compared under the campaign
build lock: 611 trial entries, including baselines, with 603 compiling. Trial
changes were not applied to the active source.

A useful alternate starting point is `u32 statusMenuId` plus signed narrow
`oldLevel` (`s8` or `s16`). It has 23 differing instructions at 99.63043%, but
adds an incorrect sign extension where retail copies the level result, and
changes the source register of the following byte mask. This is not an
improvement suitable for landing. Attempts to separate a wide call result from
a narrow lifetime, move the conversion after the guard, or use a helper output
parameter did not remove that extra conversion while retaining the allocation.

## Earlier compiler allocation investigation

The subsequent pass made 563 diagnostic trial entries, including baselines;
559 compiled. None exceeded 99.652176%. These were experiments, not accepted
source changes. Families included separate alternate-menu locals, full-function
inlining, level-loop reference parameters, scoped return declarations, enum and
bitfield representations, redundant-expression lifetime probes, and byte-sized
level-increment forms. The latest local m2c batch still references draft
`e27aae228820f4fd78551b1e523353051ce783af9b22fefce78cde63cc55c675`,
not a newly improved implementation.

Compiler cross-check: GC 1.3, 1.3.2, 1.3.2r, 2.0, 2.0p1, 2.5, 2.6, and 2.7
all produced the same score and 32 differing rows. Earlier versions and the
successfully compiled 3.x versions were worse. Four 3.x trials failed with
Shift JIS diagnostics; they are not measurements. The configured compiler was
not changed.

A new GC/2.6 allocation trace was captured for the preserved aggregate-flag
candidate. Its allocation simulator reproduces all 89 active register nodes
without discrepancies. GC/2.6 is a debugging stand-in, not the acceptance
compiler; GC/1.3 remains the measurement authority.

Important distinction: a virtual-register ordering search found a coloring
that agrees with every mapped retail saved-register assignment. **No C source
has yet produced that ordering.** The declaration-only search did not improve
the model's ten differing saved-register values in 500,000 trials. Moving only
the three generated temporaries reduced that count to five; allowing five
named values to move as well reached zero in the model.

The useful generated nodes in that earlier trace are r69 (alternate menu ID),
r166 (the level-call result), and r169 (the level-status buffer address).
The source's `oldLevel` node r67 is already unused before register allocation:
its uses have become uses of the call-result temporary r166. This explains why
reordering that declaration alone cannot move the saved level. The next source
experiment must change that propagation or the generated temporaries' ordering,
while retaining the existing opcode sequence. A correct abstract coloring does
not justify changing object bytes, using assembly, or marking the unit linked.

## Initialized-copy improvement

The next pass inspected the compiler's copy propagation and tested initialized
copy chains. A single copy followed by an idempotent OR, with front-end
propagation disabled for this function, improved the actual GC/1.3 object from
99.652176% to 99.76087%. Both operands are initialized from the same level-call
result; there is no added call, memory access, or uninitialized value.

In the new GC/2.6 debugging trace, the backend still treats this as an OR before
allocation, then emits it with identical inputs as the existing move. The
named oldLevel node survives, unlike the older candidate. The allocation model
reproduces this trace, but a declaration-only search still did not improve its
eight differing saved-register nodes. That model does not account for the
remaining byte-mask source-register mismatch.

Explicit buffer-local trials initially hoisted the address too far and added
stack traffic. Disabling front-end loop-invariant motion restored the existing
instruction sequence for some forms, but did not improve their register
allocation. Buffer OR forms added an unwanted move. Inline level-loop helpers
and scheduling/peephole changes did not improve the new candidate. These
diagnostic variants were not applied to the repository.

New trace: `/tmp/f232-trace-or`; model input: `/tmp/f232-or-graph.dat`.
The earlier trace directories and zero-mismatch abstract ordering below refer
to the older source, not this candidate. Source-level exactness remains open.

## Alternate-menu improvement

Separating the alternate branch's menu handle from nextExp initially added an
unwanted move. `altMenuCopy = altMenuId; altMenuId |= altMenuCopy;` retains
the named scalar while emitting the original single move. A fresh GC/2.6 trace
(`/tmp/f232-trace-altor`) confirms this, and the allocation simulator reproduces
all 89 active nodes. The named-node search reached five remaining mismatched
saved-register values, versus eight before. Its predicted declaration order
was compiled with GC/1.3 in the actual owner: 99.847824%, 460 instructions,
14 register-only mismatches. The live source object independently reproduces
that result; this is a measured source improvement, not only a model result.

The remaining values are party, msgId, pokemonPtr, the dark-Pokemon fightOut,
and the level-status buffer. The level guard's byte-mask source also differs.
Both identity operations use fully initialized equal operands. No helper,
additional call, memory access, or instruction was retained.

Other trials in this pass: out-of-line source blocks changed instruction
layout; inline level-up helpers needed reference parameters to avoid copies,
but their best form reproduced the old 22 differences. Its trace still gave
the buffer a generated temporary. Buffer identities, reused locals, chains of
up to 16 initialized copies, and runtime-value cancellation did not improve the
preserved result. Scalarizing the remaining locals as one-field aggregates
also reproduced the same 14 differences. Removing either retained OR made the
result worse. A long generated trial filename hit MWCC's filename limit; that
trial batch was rerun with short names, rather than counting the missing object
as a successful compilation.

Reproduction artifacts: `/tmp/f232-stages/altor_top_new_or.c`,
`altorder_named.c` and their objects; `/tmp/f232-altor-graph.dat`,
`/tmp/f232-altor-order-named.txt`, and `/tmp/f232_apply_alt_order.py`.
The altor trace is from before declaration reordering; do not use its numeric
virtual-register IDs as though they describe the final reordered source.

## Local research artifacts

Temporary, untracked artifacts from this pass:

- `/tmp/f232-stages/*-results.json`: scores for each new experiment family.
- `/tmp/f232_stage_helpers.py` and `/tmp/f232_*.py`: experiment generators.
- `/tmp/f232-permuter-current/fn_802232F4`: isolated work unit; its baseline
  score is 160 (32 register differences at weight 5).
- `/tmp/f232-current-diff.json`: isolated comparison for the preserved shape.
- `/tmp/f232trace`: earlier compiler-allocation trace. It predates the aggregate
  flag change and must not be treated as a trace of the current candidate.
- `/tmp/f232-trace-current`: refreshed trace for the aggregate-flag candidate.
- `/tmp/f232_trace_current.py`: isolated trace capture driver.
- `/tmp/f232_graph_export.py`, `/tmp/f232_graph_search.cpp`,
  `/tmp/f232-graph.dat`, and `/tmp/f232-order-focus.txt`: validated allocation
  model and the abstract zero-mismatch ordering, not an accepted C candidate.

Generated full-source trial copies and objects from these passes are disposable;
the result JSON files and experiment generators are retained separately.

The normal full link continues to use retail code for this incomplete unit.
Passing canonical hashes in that state does not accept this source candidate.

## Validation history before the initialized-copy change

All four project commands passed through the campaign build lock:
`python3 configure.py --no-progress`,
`ninja -j2 all_source build/GC6E01/report.json`, `ninja -j2`, and
`python3 configure.py progress`. The full link reported `26 files OK`.
The fresh report confirms 100% for the sibling and 99.652176% for this function;
the complete unit's fuzzy score is 99.73422%, with `complete: false`.

A direct comparison of the built source object confirmed 460 aligned
instructions, exactly 32 register-only differences, and matching symbolic
relocation targets. The quality scanner passed the changed source, and
`git diff --check` passed. Existing encoding and linker warnings appeared
during the build, which exited successfully.

The allocation-investigation follow-up rebuilt the report, reran the direct
object and quality checks, and rechecked all 26 canonical hashes successfully.
`ninja -j2` had no remaining work. The report still records this unit as
incomplete at the percentages above. Cleanup removed 1,105 generated trial
files (348.5 MiB); result JSON and research scripts were retained. The source
owner claim was released. No additional source change was retained in this
follow-up, and no matching or linking progress is claimed for it.

## Validation of the initialized-copy candidate

The four normal project commands passed again under the campaign build lock:
`python3 configure.py --no-progress`,
`ninja -j2 all_source build/GC6E01/report.json`, `ninja -j2`, and
`python3 configure.py progress`. An explicit
`build/tools/dtk shasum -q -c config/GC6E01/build.sha1` also reported
`26 files OK`. Existing Shift JIS and linker warnings remain.

The rebuilt report records fn_802230BC at 100%, fn_802232F4 at 99.76087%,
and the unit at 99.817276% fuzzy with `complete: false`. Direct object
comparison confirms all 460 instructions align, with exactly 22 register-only
differences and unchanged branch destinations and symbolic relocation targets.
The changed-source quality scan and `git diff --check` passed.

No exact-source or newly linked progress is claimed. The incomplete unit still
uses retail code during the canonical link. Changes remain local and uncommitted.
Generated trial sources and objects were cleaned from `/tmp/f232-stages`,
reducing it from 464 MiB to 928 KiB; the best OR candidate, scores, generators,
and compiler traces were retained. The source claim is released after validation.

## Validation of the alternate-menu candidate

All four project validation commands passed again after applying the new source
and declaration order. The fresh report records fn_802232F4 at 99.847824%,
fn_802230BC at 100%, and the unit at 99.88372% fuzzy with `complete: false`.
The direct comparison confirms 1840 bytes, 460 aligned instructions, and exactly
14 register-only differing rows, with identical branch destinations and
symbolic relocations. The changed-source quality scan and `git diff --check`
pass. Both the normal full build and an explicit canonical SHA-1 check report
`26 files OK`. Existing encoding and linker warnings are unchanged.

This is fuzzy-match progress only; no new exact function or linked unit is
claimed. The source remains local and uncommitted. The owner claim is released
after validation. Disposable generated trials are removed, retaining the best
candidate, the pre-reorder candidate needed to reproduce the model, results,
generators, and compiler traces.

## Pointer-copy and buffer investigation

A bounded decomp-permuter pass tested 4,000 candidates from the 14-row source.
Its useful change was an initialized temporary for `fightPokemonGetPokemonPtr`:
`pokemonPtrResult = fightPokemonGetPokemonPtr(pokemon);`
`pokemonPtr = pokemonPtrResult;`. The unrelated type-spelling mutation was not
retained. The actual owner object independently measured 99.858696%, with
1840 bytes, 460 aligned instructions, and 13 register-only differences.

The remaining saved-register mismatch is the three-way permutation in the
table above, plus the level guard reading saved r28 instead of return r3.
The isolated trace is `/tmp/f232-trace-ptrcopy`. A coalesced named temporary
assigned to physical r3 remains in interference-neighbor counts. The earlier
raw-cost allocator model does not reproduce this candidate. Keeping those
neighbors and using cost/degree for the forced removal step reproduces every
active allocation. A temporary-order search can then reach all mapped retail
saved registers; declaration-only permutations did not improve the baseline.
This remains a model result, not byte-exact source.

Full backend traces identify common-subexpression elimination as the first
pass that substitutes a generated buffer-address temporary for an explicit
local inside the loop. Copy elimination removes the now-dead local later.
A scalar buffer initialized before the level loop, with loop-invariant
optimization disabled and its declaration between menuId and oldLevel,
gets every saved register right. However, it emits `addi r0,...; mr r29,r0`
instead of the retail `addi r29,...`, and still has the wrong level-mask source.
It is 1844 bytes, not 1840, and has not been applied. Coalescing does not merge
this named local with the address temporary despite their non-interference.

Important local artifacts:
- `/tmp/f232-stages/ptrcopy_minimal.c` and `.o`: preserved 13-row candidate.
- `/tmp/f232-stages/bufreg_register_u32_loop.c` and `.o`: extra-move alternate.
- `/tmp/f232-trace-bufclose-correct`: verified trace of that alternate.
- `/tmp/f232-trace-bufplain`: buffer-local elimination trace.
- `/tmp/f232-permuter-alt/fn_802232F4/output-65-1`: original permuter result.
- `/tmp/f232-ptrcopy-graph.dat` and `/tmp/f232_graph_search_ratio`: corrected
  allocation search; the older export script's raw-cost fidelity message does
  not describe this corrected search.

Do not use `/tmp/f232-trace-bufclose` for the extra-move candidate: that earlier
isolation omitted its loop-invariant pragma. The `-correct` trace includes it.

## Follow-up tests and checkpoint validation

A second bounded permuter run tested 8,000 candidates from the preserved
13-row source in 685.6 seconds. Its final score remained 65; no result was
applied. Source trials around buffer-call inlining, byte-mask capture, and
initialized identity expressions also did not beat the preserved candidate.
Internal-style dollar-prefixed local identifiers were rejected by the canonical
compiler pipeline, not accepted as a way to enter its temporary register range.

One ordinary-C alternate fixes the level-mask source but leaves 16 differing
register rows at 99.82609%, without changing the 1840-byte size. It initializes
oldLevelCopy directly from the call, then assigns oldLevel. Its four remaining
generated nodes are grid 81, status 83, level 170, and buffer 173 in the verified
isolated trace `/tmp/f232-trace-mask16`. The corrected cost/degree model reproduces
its actual allocation; a declaration-only search did not improve it. Source and
object: `/tmp/f232-stages/cfmask_124_oldLevel_before.*`. This is an alternate
research starting point, not a retained improvement. Capturing the byte before
the preserved identity OR instead adds instructions and is not suitable to land.

All four standard project validation commands passed for the preserved
pointer-copy source, through the campaign build lock. The rebuilt report now
records fn_802232F4 at 99.858696%, fn_802230BC at 100%, and the unit at 99.89203%
fuzzy with `complete: false`. The direct object check confirms 460 aligned
instructions, 13 register-only differences, matching branch destinations, and
matching symbolic relocation targets. The changed-source quality scan and
`git diff --check` pass. The full build and separate explicit SHA-1 check each
report `26 files OK`. Existing encoding and linker warnings remain.

Validation log: `/tmp/f232-checkpoint-validation.log`. Experiment result files
record 2,815 trial entries, 2,783 compiling, separately from the bounded permuter
runs. No new exact function or linked unit is claimed. This unit still uses
retail code for the canonical link, and source changes remain local/uncommitted.

## Return/output and temporary-binding follow-up

A further 354 isolated trials all compiled but did not improve the live
99.858696% candidate. No source or compiler-setting change was retained.
The result files in `/tmp/f232-stages` cover:

- `level-output-results.json`: inline return/output helpers with plain, copied,
  aggregate, and identity-OR levels. The best returns to the 16-row alternate.
- `current-cpp-results.json`: current-source C++ baselines and const-reference
  bindings for the level/buffer. Bindings add instructions; no unit language
  change is justified. Only the unchanged C++ baseline reproduces 13 rows.
- `close-address-results.json`: unsigned cancelling offsets and zero-index
  expressions around the extra-move buffer candidate. Its move persists.
- `buffer-initializer-results.json`: initialized structures, unions, arrays,
  nested aggregates, and const variants. These alter the instruction sequence.
- `predicate-results.json`: combined level-read/output/predicate inline forms.
  These add instructions and are not suitable candidates.
- `wide-buffer-results.json`: 64-bit buffer carriers and shifted halves.
  None removes the extra move while preserving the retail instructions.
- `late-identity-results.json`: initialized identity copies at later call and
  loop boundaries for status, grid, and level. All add instructions or worsen
  allocation; they do not disappear as useful register-allocation no-ops.

The four standard validation commands, explicit canonical SHA-1 check, direct
object comparison, source-quality scan, and `git diff --check` passed again.
The report remains 99.858696% for fn_802232F4, 100% for its sibling, and
99.89203% fuzzy / incomplete for the unit. All 26 canonical hashes pass with
retail fallback for this unit. The cumulative stage logs now contain 3,169
entries, 3,137 compiling. This follow-up is negative research evidence, not
additional exact-source, fuzzy-match, or linked progress.

## C++ buffer-wrapper improvement

The accepted `src/game/floor_character_exact_80116D30.cpp` and its research
notes provided a useful scalar-wrapper precedent. A minimal buffer-only wrapper
fixes the final saved-register permutation in this target: statusMenuId r31,
gridOpened r30, and the level buffer r29. The wrapper's assignment and conversion
inline completely. Unlike that precedent, this candidate uses the existing
GC/1.3 unit flags, not `-opt nopeephole`; function-local propagation and
loop-invariant motion remain disabled. The wrapper is explicitly documented as
code-generation shaping under the existing exception policy, not original
source recovery.

Both function bodies now live in `fight_range_802230BC.cpp`, with unmangled C
linkage. The sibling's formerly unspecified local declarations are given their
actual call arities for C++ parsing, retaining the existing documented narrow
prototypes. The sibling remains exactly 568 bytes. A structured ELF audit against
the retail object confirms its bytes and all 22 relocations are unchanged, both
objects define exactly these two functions, and each has only a 2408-byte
allocatable .text section. There are no emitted wrapper helpers or additional
data sections. All 97 target relocations agree.

The remaining mismatch follows the level-result save: retail masks return
register r3; the candidate masks saved r28. Both hold the same value, but this
is not byte-exact acceptance. Source and object: the active C++ owner and
`/tmp/f232-stages/owner_candidate.*`; isolated winner:
`/tmp/f232-stages/scalar_default_8.*`.

A separate level-assignment wrapper (`lvassign_8.cpp`, also `lvassign_11.cpp`)
produces every required instruction/register but swaps the adjacent level save
and mask. It is 1840 bytes and 460 actual instructions; objdiff's 461 aligned
rows include insert/delete alignment, not an extra emitted instruction. Its
GC/2.6 debugging trace at `/tmp/f232-trace-swap` shows post-allocation peephole
forwarding the mask input to r3, followed by final scheduling moving the mask
before the save. GC/1.3 remains the acceptance compiler. Source-level identities,
assignment forms, narrow types, scheduling settings, and empty control-flow
boundaries have not removed this final ordering/register discrepancy.

The newer batch `8cda85ec8fb3a4f97d11` contains assembly-only draft
`a951b4adefeeda61183c7688d1dd46f4fe7a1a5cbc14fdde052bcf70e2414ab5` for this
target; typed context generation failed on a K&R declaration. Fresh isolated
typed-context m2c variants compile but score about 81.58-87.63%, below the active
candidate. No harness changes were made and no harness runs were interrupted.

No new exact function or linked unit is claimed. Canonical hashes while the
owner remains CodeCandidate verify the retail-fallback build, not acceptance
of the C++ candidate.

Checkpoint validation for the C++ owner passed through the campaign build lock:
configure --no-progress; ninja all_source and report; full ninja; configure
progress; explicit dtk shasum; changed-source quality scan; direct instruction
comparison; structured whole-owner ELF audit; and git diff --check. Both the
full build and explicit hash check reported 26 files OK. The rebuilt report
confirms fn_802232F4 at 99.98913%, its sibling at 100%, and the incomplete unit
at 99.99169% fuzzy. Existing linker floating-point-setting warnings remain.
Log: `/tmp/f232-checkpoint-validation.log`.

GC/1.3 through GC/2.7 reproduce the one-operand candidate and swapped-pair
alternate; the older tested compilers are worse. Additional optimization-flag,
identity-output, and intrinsic-mask probes did not improve the active source.
These were isolated experiments, not changes to the configured compiler.
Sources remain local and uncommitted.

## Final resolution and validation

The last trace comparison established that the plain assignment preserves the
block's already-scheduled flag through the final peephole pass. The close C++
level wrapper deletes a redundant move after allocation, clears that flag, and
causes final scheduling to swap the save/mask pair. Trace:
`/tmp/f232-trace-plain-current`; contrast `/tmp/f232-trace-swap`.

Rather than retaining that wrapper, the exact form starts from the plain level
assignment. Returning statusMenuId to u16 and moving it near the end of the
declarations fixes its register. Returning gridOpened to an ordinary u8 next to
statusMenuId fixes the grid register. Removing the explicit C++ buffer local
and using lbl_80478278 directly then fixes the buffer/level pair. The isolated
`lastpair_0.cpp` first reached 100%; the full original C owner also reaches 100%
for both functions. Removing the dead oldLevelCopy and the loop-invariant
pragma retains exactness. Removing opt_propagation off does not (97.934784%),
so the documented scoped pragma is retained.

Final scratch source: `/tmp/f232-stages/exact_c_clean.c`. The active source is
the original shared C owner, not that scratch file. No address splits, symbol
renames, compiler changes, assembly, or policy extensions were needed.

Validation was rerun on the working tree based on
`b8f6aa28fa47fd316fb0a75bf3861b20763b35e1`:

- `python3 configure.py --no-progress`
- `ninja -j2 all_source build/GC6E01/report.json`
- `ninja -j2`: successful source-backed link and 26 canonical hashes OK.
- `python3 configure.py progress`
- `build/tools/dtk shasum -q -c config/GC6E01/build.sha1`: 26 files OK.
- Whole-owner byte/relocation audit and direct link-input check: passed.
- Added-source quality scan: passed.
- `.github/scripts/check_asm_wrappers.py`: 2335 source files, no live .inc wrappers.
- `git diff --check`: passed.

The final report records both functions at 100% and the unit complete. The
source object supplies the complete 2408-byte range in main.elf. All 119
relocations match; there are no allocatable data sections in this unit. Existing
shared-source encoding and linker-setting warnings remain. Validation log:
`/tmp/f232-exact-validation.log`; verifier: `/tmp/f232_verify_exact.py`.

The fix is packaged as a focused source/evidence commit. Unrelated
floor_character.c, debug_source.c, and editor-workspace changes were left
untouched. This record does not claim a native port or headed-run result.
