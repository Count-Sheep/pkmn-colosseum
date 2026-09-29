# `fn_801906A0` flag lookup: current register wall

This function is in `src/game/gs_range_8018FE30.c` and is reachable from the
floor/loading-worker closure (recomp inventory row 20). The 2026-09-28 retail
objdiff is 97.23404% for the 188-byte function; it is **not** linked and is
not accepted decompilation progress.

All 47 instructions align structurally. The remaining substantive differences
are register choices: retail holds the flag-definition pointer in `r6`, the
type/width byte in `r4`, and the selected state buffer in `r5`; the current C
candidate uses `r4`, `r5`, and `r6` respectively. The downstream mask and word
loads inherit this three-register cycle. The branch-address differences shown
by objdiff are relocations, not a changed control-flow graph.

`local_campaign.py rewrite fn_801906A0 --seconds 100` tried 192 safe
source-equivalent rewrites without improving the score. Replacing the local
`states` pointer with a direct access to `lbl_80478EEC` also compiled to the
same 97.23404% function. A natural typed `FlagDefinition*` access instead of
the byte-pointer/offset access regressed to 87.55319%. Both probes were
reverted and the baseline object was rebuilt. Further work needs a
source-shape/register-allocation lead; do not tag the current function as
exact or port it as accepted code.

A second bounded pass (2026-09-28) also found that declaration-order changes,
using a byte-width `typeAndWidth`, const-qualifying the definitions pointer,
and moving the independent state-table load after the type-byte load all emit
the identical 97.23404% register cycle. Keeping a separate per-entry
definition pointer regressed to 91.914894%, adding instructions. All probes
were reverted. The containing six-function object has five other non-exact
functions, so even an exact isolated form would need a valid carve or a full
object match before becoming linked progress.

A third bounded check tested `u16 bitOffset`, matching the definition table's
stored field width. It kept the same three-register cycle and changed the
target `srwi` into `srawi` through integer promotion, so it regressed and was
reverted. The rebuilt owner object reproduces the original 47-instruction
diff. No source improvement or linkage is claimed.
