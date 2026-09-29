# `GSgfxBackFBDoFrame` exact-source linkage

## 2026-09-29: whole-object wall resolved

The text-only range `0x800DC560–0x800DC874` is now split at the existing
function boundary `0x800DC6D8`. `gs_light_exact_800DC560.c` owns only
`GSgfxBackFBDoFrame` (`0x178` bytes), and
`gs_light_candidate_800DC6D8.c` retains `fn_800DC6D8` (`0x19C` bytes).
The broad prefix source previously also emitted `GSlightPopState`, so the
new wrappers explicitly exclude that unrelated function; its existing
candidate owner is unchanged. No data section is moved or invented.

Canonical `build/GC6E01/report.json` reports
`main/game/gs_light_exact_800DC560` as `complete: true`, 376/376 matched
bytes and `GSgfxBackFBDoFrame` 100%. The sibling remains incomplete at
99.70874%. Guarded configure, all-source/report, and full `ninja -j2`
passed; both `main.dol` and `common_rel.rel` matched their configured retail
SHA-1 hashes. The 27 quality-scan tests passed, and a diff-scoped quality
scan accepted the new gates and wrappers. The old unsplit-unit wall below is
retained as investigation history, not the current acceptance state.

## Earlier unsplit-unit investigation

On 2026-09-29, the source-backed candidate in `src/game/gs_light.c` reached
100% fuzzy/raw match for `GSgfxBackFBDoFrame` (376/376 retail bytes). The
change replaced a separate `found`-pointer loop with the same inline
four-slot active-texture lookup shape already recovered in the adjacent
`gs_gfx_backfb.c` API. The target's four unrolled slot checks, direct found
pointer and register allocation now agree; the source still converts the
image, invokes its callback, unlocks a completed capture, recomputes the
active flag and increments the frame counter in the observed order.

This is **not accepted or port-ready** under the whole-object rule. The
declared `main/game/gs_light_candidate_800DC560` CodeCandidate also owns
`fn_800DC6D8`, which is 99.70874% in the canonical report (raw objdiff
99.660194%). Its remaining difference is in the case-0 animation-end float
comparison: the retail sequence puts the end value in f0 and the current
frame in f1, then uses `fcmpo`/`cror eq,gt,eq`; the candidate chooses the
opposite temporary order and `cror eq,lt,eq`. The source comparison is
semantically equivalent for the ordered values observed there, but its
original C expression/codegen shape is not established. Equivalent
`frame >= end`, load-order and declaration-scope trials did not improve the
object, so they were reverted. No split was changed to isolate the exact
function or bypass whole-object linkage.

Guarded `ninja -j2 all_source build/GC6E01/report.json` and `ninja -j2`
pass. The rebuilt `main.dol` SHA-1 is
`870e8b9693ca780782d80f22a6a4572d8ba9458f`, matching
`config/GC6E01/build.sha1`; common REL's configured hash is unchanged.
Next work needs source evidence for the sibling's case-0 float expression
or a legitimate original-object ownership discovery. Until the complete
object links, the recomp must leave this callee unbound.

## 2026-09-29 follow-up: `fn_800DC6D8`

The retail/candidate instruction comparison narrows the case-0 difference to
the float registers and comparison direction. Retail loads the end offset in
`f0`, the current frame in `f1`, computes the end in `f0`, and compares
`frame >= end`; the candidate loads them into `f1`/`f0` and compares
`end <= frame`. Its store uses the candidate's `f1`. Two additional natural
source forms were compiled under the campaign build lock: comparing the
record field directly without a `frame` local, and updating the case-local
limit before comparing/storing it. Neither improved the raw 99.660194% match.
An inline repeated end expression regressed to 98.00971% by emitting another
load/subtract before the store. All trials were reverted.

The raw objdiff also reports the unsigned-delta conversion's double constant
as retail `lbl_8047CA80` versus candidate compiler literal `@84`. Both
correspond to the same `2^52` conversion value; this is a relocation/ownership
difference, not evidence to inject an explicit hand-written conversion. The
canonical report must continue to mark the whole object unlinked.

## 2026-09-29 resolution: `fn_800DC6D8` linked

Both residuals are resolved (branch `claude/recomp-blockers-gfx`). The
case-0 compare is written `if (frame >= (end = limit - lbl_8047CA74))`:
assigning the end value inside the condition gives retail's f0/f1 order,
while a separate `end = ...;` statement swaps them. The `(f32)delta`
conversion bias at 0x8047CA80 is used only by this function, so the unit
now owns `.sdata2` 0x8047CA80–0x8047CA88 and MWCC's own literal fills it.
`sdata2_8047CA70.c` is split around it (`sdata2_8047CA88.c` holds the
rest). `gs_light_candidate_800DC6D8.c` is now a standalone source and
links as Matching with retail DOL/REL SHA-1.
