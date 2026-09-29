# `fn_800D892C` GSgfx pipeline restore wall

This is the source-backed `GSgfx_ConfigurePipeline` candidate in
`src/game/gs_gfx_layer_candidate_800D892C.c`. **Resolved by lane D6
(2026-09-29): byte-exact and linked as Matching under the byte-match-first
policy**, with tagged rule exceptions (see the D6 section below and
docs/RULE_EXCEPTIONS.md). The history below is kept for the record.

## Lane D6 (2026-09-29): 98.52% -> 100%, linked

1. **XD counterpart: none.** XD's GSgfx is the C++ `GSgfxLayer` rewrite
   (NXXJ01.map objects `GSgfx\layerState.o`, `layer.o`, `layerFunc.o`;
   StarsMmd/Colo-XD-PBR-symbol-maps). No function in trevor403/xd-asm
   @ b1087f18efdb2d502b0f615ca55e5c1ba84f0344 has this function's memcpy
   sizes (5/5/4/4/0x14, 0x10, 0x54), and none loads bytes 0, 1 and 2 of one
   pointer into three table lookups. TeamOrre/xd-decomp @ 4989794e has no
   source for it.
2. **Wall 1 (stage-source pointer): `#pragma opt_dead_assignments off`.**
   Applied to the whole function (single-pragma and pair sweep over
   loop_invariants, common_subs, propagation, lifetimes, strength_reduction,
   dead_assignments, dead_code, unroll_loops, strength_reduction_strict,
   peephole, scheduling, unroll_count, optimization_level 1-4, and the
   per-unit flags `-inline deferred`, `-opt nopropagation`/`noloop`/`nocse`/
   `nolifetimes`/`nostrength`/`nodeadcode`), only dead_assignments off
   helps: the pointer takes r6 and the load order matches (99.66%, 2 rows,
   only the slot-0 branch left). opt_loop_invariants off gives 98.24%.
   `optimization_level 4` resets it.
3. **Wall 2 (slot-0 test): shared split temp.** The replays show why the
   fold happens. CSE (pass 16) drops a redundant `li` into a *temp* in
   the same extended block, but it never drops one into a named variable:
   `next = last; if (c) next = i; last = next;` keeps the branch with `next`
   in r18. The `?:` form folds because its select temp is a backend temp,
   but that temp is numbered about 307 and takes r31.
   Retail's `last` is a frontend *live-range split* temp. When `last` and
   `i` are function-level and shared by both paths, the splitter gives the
   default path's `last` range its own temp. The explicit per-slot form
   puts `last = 0` in the same block as the first test, so CSE folds it.
   The splitter numbers `last`'s temp after the loop-counter temps
   (highest `@`, so lowest vreg, so r21) only when the TEV-load path sets
   `i` before `last` (`for (i = 0, last = 0; ...)`). Declaration order and
   variable names have no effect; first-use order does.
4. **Layer-pointer trio.** The explicit form's eight load temps broke the
   r6/r5/r4 trio (D5). Loading through one `u8 en` local
   (`en = desc->attr[6 + k].enabled; if (en == 1) last = k;`) gives one
   node and 100%. An `int` `en` gives 8 rows; `u32` also works.
5. With the pragma, the `(u32)chan` cast is still needed (98.74% without
   it).

Linked: one-function object, retail DOL/REL SHA-1 OK, check_regression
against 027b1e6c clean (+1 exact).

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

### Lane D5 follow-up: the two coordinator angles (both still walled)

**1. Stage-source pointer through a cast index.** Every form scored worse
than the committed 98.52% raw (17 rows):
- `&stageSrc[(u32)i]`: 96.70%, 76 rows.
- The same cast in both loops: 96.09%, 87 rows.
- The cast in the default loop only: 97.86%, 34 rows.
- The byte-offset form `(u8*)stageSrc + (u32)i * 4`: 96.44%, 77 rows.
- `[(u8)i]` and `[i & 0xFF]`: 96.93%, 47 rows.
The cast does move the reduction to the backend, but the backend then
builds a separate +4 induction (a new callee-saved r17, and the frame
grows by 0x10). Retail's pointer is `lbl + (r29 + 0x42e)`, where r29 is
the frontend's shared i*4 offset that the colour and TEV-input copies also
use. So retail's pointer is not an induction value; it is computed each
iteration from the shared offset. In the colouring replay it must be
coloured before the two backend index temps (vreg above about 257). Its
degree is 26, so it is pushed in the first sweep at its own vreg. A named
local at any scope, a frontend temp, or an inline parameter all rank too
low.

**2. Ternary `last` (r31 instead of r21).** The `?:` result is a temp
created when the backend lowers the expression (r304 in the replay). The
loop unroll copies it into each iteration, and copy propagation keeps
r304 and drops `last` (vreg 40, left with no neighbours). The simulator
gives r21 only for a vreg between 37 and 43, the named-local range.
Neither named-local route helps:
- A named intermediate (`next = c ? i : last; last = next;`, block-local
  or function-level, before or after `last`) is folded away, so the
  output is unchanged.
- `(c) ? (last = i) : last` and `if/else continue` fall back to the
  branch form (17 rows).
- Moving the `?:` to the saved-TEV loop instead, or to both loops, gives
  50-58 rows.
The other way to fold the slot-0 branch is the frontend's own folding.
That works for a split temp in the explicit per-slot form (x1: shared
`last`, unrolled by hand), but the explicit form's eight load temps give
the layer pointer 37 interferences and break the r6/r5/r4 trio. The
simulator finds no renumbering of the pointer, flags or `last` nodes that
fixes the trio under that interference graph.

Best forms, for the record:
- 98.52% raw, 17 rows (committed): only the stage-source pointer and the
  slot-0 branch differ.
- 98.78%, 25 rows: the `?:` default loop; only the stage-source pointer
  and `last` in r31 differ.

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
