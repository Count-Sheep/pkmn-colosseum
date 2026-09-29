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

## Lane D2 register replay (2026-09-29)

### Compiler

GC/1.3, 1.3.2, 2.0, 2.0p1, 2.5, 2.6 and 2.7 all give the same 225-row
aligned diff. GC/1.2.5n and 3.0a3 are much worse. The unit's compiler is
not the issue.

### Register replay (cadmic/mwcc-debugger, GC/2.6 = GC/1.3 output)

The first divergence is the masked `GSgfxSetChanCtrl(4, ...)` expansion.
Retail keeps the scaled channel index (`chan * 6`) in r5 and puts `both`
in the callee-saved r18. We get the reverse (`both` r12, index r18).

- The index is created by the **frontend** strength reduction as temp
  `@202 = chan * 6` (plus `@202 += 6` in the `both` branch). As a late
  frontend temp it gets a lower virtual register (r69) than the inline's
  locals (`done` @122, `both` @123, `chan` @124: r96/r95/r94). Within a
  colouring level the higher virtual register is coloured first, so `both`
  takes the last free volatile (r12) and the index falls to r18.
- In retail the index outranks even the backend temps (the `clrlwi.` test
  temp gets r6 because r5 is already the index), so retail's index is a
  **backend** loop-transform value, not a frontend temp.
- `#pragma opt_strength_reduction off` confirms it: all four channel-setter
  expansions then match retail apart from the callee-saved numbering
  (r20/r21 for r18/r19). The saved-TEV stage loop
  (`for (i < numTev)` with the memcpys) breaks, though: retail
  frontend-strength-reduces that loop into nine separate induction offsets
  (r21-r29: +1, +20, +20, +4, +5, +5, +3, +1, +4), which the backend alone
  merges. The pragma only works per function (inside the body or at the
  inline's definition it has no effect), so it cannot be scoped to the
  setter.

### Tried without success (2026-09-29, D2)

- `chan` as u32/u8/u16/s16/int: `int` and the unsigned types let the
  frontend fold the `chan == 4` test, which retail keeps; `s32` (long) is
  right.
- `both`/`done` as s32/int/u32/BOOL: fewer differing rows (156 at best),
  but retail's `clrlwi.` shows they are u8. All six declaration orders of
  `both`/`done`/`ctrl` tried.
- Loop forms `do/while`, `continue`, `chan = chan + 1`, `chanCtrl + chan`,
  and direct `chanCtrl[chan].field` stores.
- The setter as a macro with block-local `chan`/`both`/`done`: the frontend
  then folds `chan == 4` (retail does not), so the if-chain changes.
- `opt_strength_reduction_strict on`, `opt_loop_invariants off`,
  `opt_propagation off`, `opt_lifetimes off` (all worse or unchanged).

### Next step

Find a setter form where the frontend does not strength-reduce the
`while (!done)` loop but still keeps `chan` as an unfolded s32 parameter,
while the saved-TEV loop is still reduced. Alternatively, find evidence of
how GS's channel setter was really written (XD's `GSgfx_GCSetChanCtrl` is
a different, FIFO-writing design).
