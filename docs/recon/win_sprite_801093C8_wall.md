# `fn_801093C8` title/menu draw candidate

The suffix carve in `src/game/win_sprite.c` remains a `CodeCandidate` and is not
linked. After restoring source-defined arithmetic, the raw function objdiff is
99.61078% (`build/GC6E01/report.json` reports 99.67066% fuzzy). Neither number
is accepted decompilation progress.

The prior expression assigned `lbl_8047CE48` to `progress` in the left operand
of a subtraction while also reading `progress` in the right operand. Those
operations are unsequenced in C, so this was undefined behavior. It happened
to produce a higher fuzzy score (99.91018% in an earlier report), but its
`fmuls f1, f3, f3` squared the progress register rather than multiplying the
retail fade value by progress. Do not restore this expression for its score.

The current source computes the observed fade formula as
`lbl_8047CE48 - (lbl_8047CE48 * progress) / lbl_8047CE4C`. The remaining
instruction differences are an `f2`/`f3`/`f4` allocation cycle around the
interpolation at instructions 44-55; opcode sequence and behavior are otherwise
the same. Two earlier relocations also name compiler-generated `@28`/`@30`
constants rather than retail `lbl_8047CE60`/`lbl_8047CE68`. The current
`win_sprite_suffix_801093C8.c` split contains only `.text`, while those retail
constants are owned by the separate `.sdata2` carve
`gs_model_sdata2_8047CD98.c`. Linkage requires authentic data/TU ownership,
not a forced `Matching` status or an artificial constant stand-in.

Validation: `ninja -j2 all_source build/GC6E01/report.json` and `ninja -j2`
completed through the shared campaign build lock; the full build checked both
retail SHA-1 hashes (`main.dol` and `common_rel.rel`: OK). This does not imply
the candidate function is linked.
