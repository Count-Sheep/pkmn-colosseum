# Memory-card translation-unit residuals (2026-09-28)

The reconstructed `src/game/memcard.c` is a whole-unit candidate. Its code,
rodata, switch tables, BSS, sdata2 and sbss are interdependent; in particular,
`fn_801D0090`'s 4-aligned switch table lies inside the shared `.data` range.
Do not promote a single-function carve that changes these section positions.
The two residual functions must be made exact, then the full object must link
with retail DOL/REL hashes and the quality scan passing.

The canonical report has `fn_801CDB04` at 99.99676% fuzzy. In a live
`objdiff-cli diff` on `main/game/field_candidate_801CDB04`, the only real
instruction difference is the last-write poll's `card_result` store: retail
`stw r30,0x28(r4)` versus candidate `stw r3,0x28(r4)` after
`CARDGetResultCode` and `mr r30,r3`. Equivalent local polling code directly in
`memcardCheckLastWrite` regressed the function. Returning the field assignment
from `memcardPollResult`, chained assignment from the CARD call, splitting
the last-write result declaration from assignment, assigning in the switch
expression, and naming the case-42 next state all retained the same store.
No candidate was promoted.

`fn_801CF9C8` is 99.51923% fuzzy in the canonical report. Its header loop
uses `r6` for the task pointer and `r8` for the slot header where retail uses
`r8` and `r6`; the rest of the loop has the corresponding register swap.
Naming and reusing a task pointer in the loop did not change the output.
Giving the loop its own slot-header local or moving the `header` declaration
before `j` or after `i` regressed the score. The previous candidate's selected
header arithmetic form remains intact. No change was accepted.

Both source probes were restored and the two candidate objects rebuilt under
the campaign lock. Exact-source and linked progress remain unchanged.
