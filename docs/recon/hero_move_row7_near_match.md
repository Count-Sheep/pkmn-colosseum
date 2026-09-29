# Hero-move row 7 near matches (2026-09-29)

The boot row's `fn_8013024C` is byte-exact but remains unaccepted because
its full `hero_move` translation unit does not link. The bounded source-owner
trials below did not change that acceptance gate.

On 2026-09-29, the retained H2 branch forms were reviewed against the live
owner and integrated selectively. `getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK`
rose from 83.40625% to 95.734375% by expressing the four part transforms and
height/step updates as loops. `updateAnimation__Ff15HEROMOVE_MEMBER` rose from
96.86793% to 99.962265% after using its actual `void` result, the
`fn_8018F4C8` `u8` loop-output signature already declared in
`include/game/people/people_inline.h`, and literal animation thresholds.
The report still lists both as incomplete `CodeCandidate` functions. The raw
`updateAnimation` diff is now mostly standalone-wrapper literal relocation
names and branch destinations, not evidence of whole-TU linkage. None of the
other hero-move residuals changed score, and the exact boot-row function still
awaits the full object.

In a second bounded pass, `fn_8012E388` moved from 83.54851% to 100% using
the XD-grounded `updateLeaderMovement` source shape reviewed from H3 commit
`86a8dafe`: the existing `getResID`/model helpers, byte-sized stick readings,
and the original zero-stick and dead-zone branches. No asm, pragma, or new
policy exception was introduced, and the other hero-move scores did not move.
Its `CodeCandidate` object also contains incomplete `fn_8012D7F0` and
`fn_8012DE94`, so this is exact-source progress only; it is not linked or
accepted for the boot closure.

A third bounded pass applied H3's source-backed `fn_8012D7F0` shape, including
the XD `getLeaderLog` ring-copy inline helper, typed position vectors, the
unscaled direction on its short path, the hit-Y assignment, and the
zero-length projection case. The existing unsigned history fields are read as
signed ring counters locally, so this pass did not change the shared header.
The canonical score rose from 94.55765% to 98.84706%; all other hero-move
scores stayed fixed. The remaining raw differences include saved-register
choices in the projection arithmetic and standalone-chunk relocation names.
`fn_8012D7F0` remains incomplete, and this source change does not link or
accept its object.

`initFloor__Fv` in `main/game/hero_move_r46_8012EBD4` remains 99.86928% in
the canonical report. Its instruction sequence aligns; the substantive raw
objdiff differences are the saved-register choices in the inlined `getRot`
and `setRot` model lookups (retail r28/r27 versus candidate r25/r28), plus
expected standalone-chunk literal-symbol relocation names. A 50-second
policy-safe rewrite search tried 103 variants and found none above its raw
99.60784% baseline. The register replay helper could not run because its
optional debugging tools were unavailable. No source change was retained for
this function.

`cbPoison__Fl15FootStepCounterl` in
`main/game/hero_move_candidate_8012AD50` improved from 99.44238% to
99.47955% in the canonical report by placing the independent
`expiredCount = 0` initialization before `livingPoisoned = 0`. The change
preserves the order of all externally visible operations and does not add a
dead store or compiler directive. The raw rewrite scorer measured
99.36803% to 99.405205% across 107 variants; only this one swap improved.
The unit remains `CodeCandidate`, so this is exact-source progress only,
not a linked or accepted boot function.

Validation: guarded `ninja -j2 all_source build/GC6E01/report.json` and
guarded full `ninja -j2` succeeded. The resulting `main.dol` and
`common_rel.rel` SHA1s match both lines of `config/GC6E01/build.sha1`.
