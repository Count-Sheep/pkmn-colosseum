# `fn_800D892C` GSgfx pipeline restore wall

This is the source-backed `GSgfx_ConfigurePipeline` candidate in
`src/game/gs_gfx_layer_candidate_800D892C.c`. It is **not linked** and must
not be counted as accepted decompilation or used as a native recomp body.

On the 2026-09-29 `GC6E01` build, canonical `report.json` scores the 2,320-byte
function at 94.501724%. Raw `objdiff-cli diff -p . -u
main/game/gs_gfx_layer_candidate_800D892C fn_800D892C` scores it at
93.97069%; 236 of 590 aligned rows differ, including 16 one-sided rows.
The first substantive divergence is instruction 46, in the first inlined
`GSgfxSetChanCtrl` expansion. Retail holds the `both` flag in r18 and keeps
the channel byte offset in r5; this candidate instead uses r12 and r18.
The second expansion starts with the same allocation difference (retail r19,
candidate r18). The unmasked channel expansions then add one-sided moves and
shift the branch destinations by 0x10 bytes. In the saved-TEV path, the two
versions still perform the same observed field loads, stores, calls, and
memcopies, but use a different saved-register allocation.

The masked setter's observed value flow agrees on the enable, ambient,
material, light-mask, diffuse, and attenuation values. The discrepancy is
therefore not evidence for changing a channel argument. Nor does the current
diff justify an asm wrapper, compiler pragma, or register-forcing source
rewrite. A compiler register replay was attempted with
`local_campaign.py explain fn_800D892C`, but the optional debugger tools are
not installed, so it returned `register replay unavailable`.

A 2026-09-29 local-helper trial copied the existing channel setter into this
owner and moved its `ctrl`, `done`, and `both` declarations. All four calls used
that copy. It compiled with the same 94.501724% report score, 93.97069% raw
score, and 2,320-byte retail size; the trial was reverted. A separate
35-second `local_campaign.py rewrite fn_800D892C` search scored 133
semantics-preserving C rewrites and found no improvement over 93.97069%. It
restored the owner source. Neither result supports a change to the setter's
arguments or to its observed store sequence.

The source's `numIndStages++` / `numIndStages--` pair also remains an explicit
placeholder: the retail code does store the incremented byte and then its
decremented value, but the original high-level construct is not established.
That semantic provenance is a separate acceptance wall even if the current
byte score improves. Next work should recover the original setter/helper
source shape or the indirect-stage construct from callsite/data evidence,
then measure the entire object and its link status.
