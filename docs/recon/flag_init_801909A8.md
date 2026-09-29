# Flag initialization, 0x801909A8–0x80190E34 (2026-09-29)

The active owner is `src/game/gs_range_8018FE30.c`; the scored object is
`main/game/gs_flag_candidate_801909A8`. It contains `fn_801909A8` and
`GSflagInitBitPos`, so neither is accepted for linking until the whole object
matches.

The retail zero-buffer warning uses the independently named string
`lbl_802742B8`, not an offset expression into `lbl_802741F8`. Referencing that
symbol directly improved `fn_801909A8` from 91.47849% to 93.0914%. Its
remaining differences are in the three zero-fill loops: the target and
candidate have the same unrolled store/control shape but different register
allocation, notably the first buffer in r31 versus r6 and the count/iterator
registers in later loops. A `memset` version measured only 36.505375%, and
separate block-local loop variables measured 84.62366%; both probes were
reverted. Do not claim linkage or replace the loops with an invented routine.

`GSflagInitBitPos` now measures 100.0%. The target's `rlwimi` writes showed a
six-bit width field, while its additions keep 32-bit running positions and
truncate only for the capacity calculation. The pointer-first loop update
matched the final two instructions. These are source-backed changes, not asm
or compiler directives. The exact function remains **unaccepted** because
`fn_801909A8` keeps their shared CodeCandidate object incomplete.

Validation: guarded `ninja -j2 all_source build/GC6E01/report.json`, guarded
full `ninja -j2`, and 27 quality-scan tests pass. The full build's SHA-1 check
reports both `main.dol` and `common_rel.rel` identical to retail.
