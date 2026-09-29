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

## Lane D5 (2026-09-29): 93.97% -> 98.52% raw, 17 differing rows

All work used the mwcc-debugger replay (GC/2.6 gives the same code as the
unit's GC/1.3) plus a small simulator of MWCC's GPR colouring that
reproduces the replay's simplify order and register choice exactly:
- **Simplify:** repeated ascending sweeps push every node whose degree is
  below 32. When a sweep pushes nothing, the highest-degree node is
  pushed.
- **Select:** nodes are coloured in reverse push order. Each takes r0, then
  the lowest free register in r3..r12. If none is free, it reuses the
  lowest non-conflicting callee-saved register already in use, else takes
  a new one from r31 downwards.
With it, a candidate renumbering can be tested in milliseconds before
looking for the source form that produces it.

What moved, in `src/game/gs_gfx_layer_candidate_800D892C.c`:
1. **Channel setter.** A candidate-local copy of the setter indexes
   `chanCtrl[(u32)chan]`. The cast stops the frontend strength reduction
   of `chan*6`, so the backend creates the offset, as in retail. All four
   setter expansions now match: `both` is in r18/r19 and the index in r5.
   The cast's only effect is where the reduction happens, so it counts as
   shaping. Other tries:
   - `#pragma opt_strength_reduction off` around only the inline's
     definition does nothing, because the pragma state at expansion time
     applies, not at definition time.
   - The `chanCtrl + chan` form, `chan += 1`, and the for, do-while and
     pointer-arithmetic forms all leave the frontend reduction in place.
2. **TEV-order setter.** A local copy with an `s32` stage (the shared
   header takes `u32`) puts the tevOrder offset (+3) third among the nine
   loop offsets, as in retail, instead of last.
3. **indEnable.** `tev.indEnable[i] = indEnable = (...)` (table entry
   first). indEnable is declared after numInd and numTev.
4. **`last` in the saved-TEV path.** A block-local `last` there, so the
   default path's `last` is the function-level variable. The default
   path then gets r21 (colour replay: its vreg must sit among the named
   locals, not the frontend split temps).
5. **Default-path count as a loop.** The default path counts texgens with
   the same `for (i = 0; i < 8; i++) if (attr[6 + i] == 1) last = i;`
   loop as the saved-TEV path, unrolled by MWCC. The eight separate load
   temps of the explicit per-slot form give the layer pointer 37
   interferences; the simulator shows it must be pushed in the first
   sweep (degree < 32) for retail's r6/r5/r4 split of layer pointer,
   flags and flag test.

Still open:
- **Saved-TEV loop's stage-source pointer.** Retail keeps it in r6 and
  the replay needs it coloured before the two load temps, which only a
  backend-numbered value (vreg above about 257) gives. Neither a named
  local at any scope, the pointer written into the arguments, a
  `stageSrc + i` form, nor a two-call stage-order helper reaches that.
- **Default path's slot-0 test.** Retail drops the branch but keeps the
  compare. The redundant `last = 0` folds only when `last` is a frontend
  split temp (the explicit form with a shared `last`) or a `?:` select.
  The `?:` form (`last = cond ? i : last;`, 98.78%, 25 rows) folds it,
  but the select's backend temp then takes r31 instead of r21.
- **Indirect-stage count.** The `++/--` placeholder is unchanged.
