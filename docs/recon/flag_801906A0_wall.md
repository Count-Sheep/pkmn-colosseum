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

## Resolution (2026-09-29, lane D2): exact and linked

fn_801906A0 is Pokemon XD's GSflagGet with its local helper _flagGet
inlined. XD keeps both out of line (TeamOrre/xd-decomp symbols.txt @
4989794e: GSflagGet 0x801A0364 size 0x40, _flagGet 0x801A03E8 size 0xA8
scope:local; trevor403/xd-asm @ b1087f18). GSflagGet looks up the state
buffer from the definition's type bits and calls `_flagGet(buffer,
definitions, flagId)`, which logs "ERROR[GSflagGet]:Initialization has not
finished." (Colosseum's lbl_80274284) when the buffer is missing.

The MWCC register replay (GC/2.6, same code as GC/1.3) explains the old
wall. A function's own named locals get the lowest virtual registers, so
they are coloured after frontend temporaries. In the flat form, the CSE
temp holding the loaded definition table and the `wordIndex << 2` temp
outrank typeAndWidth/buffer and bitWidth, which rotates r4/r5/r6 and
r0/r4/r6. As _flagGet's parameters and locals, the table pointer and the
bit fields take retail's registers. The body reads bitOffset before the
width (XD's _flagGet also loads the halfword first). It is admitted under
the same-engine sister-title clause and is not a rule exception.

The owner range was carved: gs_range_8018FE30.c now covers 0x8018FE30 -
0x801906A0; gs_flag_get_exact_801906A0.c (Matching, 0xBC, data-free)
holds fn_801906A0; and _flagSet (0x8019075C) is scored through the chunk
gs_range_8018FE30_suffix_8019075C.c. Retail DOL/REL SHA-1 pass.
