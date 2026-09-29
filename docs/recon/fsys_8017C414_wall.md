# `fn_8017C414` candidate wall (2026-09-28)

Owner: `src/game/fsys/fsys_file_candidate_8017C414.c`, a candidate-only
wrapper around the active definition in `fsys_file_candidates.c`. Retail size
is `0x154` (85 instructions). The current source scores 93.90588% and is
unlinked; no exact or linked progress is claimed.

The accepted-looking status switch is not the mismatch. The target and our
object have the same 85 instruction slots and the switch tests, status values,
`fn_80167E64` call and interrupt restore are in the same order. The target
instead allocates a 0x20-byte frame and saves `r29`/`r30`/`r31`; the candidate
allocates 0x10 and saves only `r30`/`r31`. Retail executes `mr r29,r3` before
`fn_8017A624(arg)` despite no later read of that argument. With that extra
saved register, retail places `OSDisableInterrupts`' result in `r30`, its
active slot in `r31`, and builds `lbl_80453FEC` in `r5`. The candidate uses
`r31`, `r30` and `r4` respectively. Retail also loads `tocBuffer` into `r0`,
tests it, then loads it into `r3` for `fn_80167E64`; the candidate loads it
into `r3` only once. The prologue, epilogue, manager-address instructions,
slot stores and close branch therefore differ even though the switch logic is
the same.

`fn_8017A814` and `fn_8017A95C` share the status/close/interrupt body with
this routine. Their candidate functions are each 99.756096% and differ at the
three `r4` versus `r5` manager-address instructions. Existing source notes in
`gs_range_8017A5FC.c` record the earlier helper, declaration, optimization,
prototype and volatile probes; none reproduced `r5` by natural C. In this
pass, assigning `arg` to a local then casting it to void (as in the sibling
candidate) changed no instruction or score. Replacing the one-use local
`FSYSManager* mgr` with direct `lbl_80453FEC.activeSlot` access also stayed
at 93.90588%. Both trials were restored. There is no surviving edit to the
active function.

The dead argument copy cannot be justified as observable behavior, and adding
dummy use or compiler controls solely to color registers would not be a
strict-policy win. The existing candidate contains a local optimization
pragma and cannot be promoted as strict source. A future exact title-path
exception would still need an individually tagged source rule exception, a
complete object linked as `Matching`, the full retail DOL/REL SHA gate, and
the quality scan; a 100% candidate score alone is not accepted linkage.

## Follow-up: compile setting and source form (2026-09-29)

The wrapper is built with `-O4,p` in `objdiff.json`, while nearby FSYS read
units use per-object `-opt level=0` in `configure.py`. A temporary local
optimization-level-0 compile of this function alone, retaining its original
`mgr` local and `!= NULL` test, scored 95.752945%. At level 0, replacing the
one-use manager local with `lbl_80453FEC.activeSlot` and spelling the pointer
test as `if (slot->tocBuffer)` scored **97.74117%**. These source forms preserve
the access and null-test semantics; the second spelling also emits retail's
`cmplwi r0,0` and duplicate load, rather than a zero materialization and
`cmplw`. Optimization level 1 scored 95.25883%, and restoring level 4 with
the same source forms returned to the original 93.90588%.

The level-0 variant was 85 instructions against retail's 85, with the shared
status switch and close path aligned. The remaining differences are a stack
spill/reload of `arg` before `fn_8017A624` instead of retail's dead `mr
r29,r3`, and `r4` rather than `r5` for the three manager-address instructions;
the save/restore register differences follow from the argument spill. No
source or compile-setting trial from this pass survives: the original
function and report score were restored. A future attempt should establish
the correct per-object compiler setting using independently matched FSYS
neighbours, then solve the register differences through authentic source
structure. A local optimization pragma or dummy parameter use is not a
strict-policy result. The function remains an unlinked `CodeCandidate`.
