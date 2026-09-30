# Recomp boot row 40 (floor manager) and the people-frame colsys blockers (lane D13, 2026-09-30)

## Linked this lane

| Commit | Unit | Functions |
|---|---|---|
| 2fe5686e | gs_thread_hi_range_800FEC34.c | fn_800FED3C, fn_800FEE68, fn_800FEF8C (bodies written out; the macro hid them from the policy check) |
| 6ebc0dd5 | gs_model_parse_exact_800EAFE4.c (+ .rodata 0x80270EB8-0x80270EE8) | _modelParseLoadEnvelopeMatrix |
| 71e8179f, f07b1b5e | gs_colsys_exact_8010D20C.c (.text 0x8010D20C-0x8010DE00, .sdata2 0x8047CEB8-0x8047CEE0) | fn_8010D20C, GScolsys2Draw, fn_8010D8D4 |
| 04506f45 | GScolsys2Thru_exact_80111470.c | GScolsys2ThruGetEventList |
| 95eac1c8 | GScolsys2Walk_exact_8010E138.c | fn_8010E138 |

Row 40's blocking list is clear apart from D9's GSmsg pair.

## Method: reading the colouring order

The mwcc-debugger dump (GC/2.6 replays these units exactly) gives the
interference graph. D7's simulator reproduces MWCC's order with K=29 for GPRs:
simplify sweeps the nodes in vreg-number order and pushes any node whose
degree is below K; select pops the nodes in reverse. FPR nodes here are all
low-degree, so they are coloured in reverse creation order, each taking the
lowest free register. `scratchpad/d13/vmap.py` maps every virtual register to
ours and retail's physical register (dump -> final order -> retail listing),
and `whatif.py` tries renumberings. The numbering rules seen:

- User variables come first, in reverse declaration order (an inner scope
  numbers first).
- Inline-function locals and parameters are frontend `@` temporaries, and
  their vregs rise in declaration order.
- Codegen temporaries come last, in creation order. The string-pool base and
  call results are codegen temporaries.

Source changes that moved nodes:

- Turning a frontend temporary into a codegen one or back. HSD_JObjMtxIsDirty
  returning its condition directly removes its `result` temp, and the string
  base is then coloured first.
- Adding user variables between two others. The bitfield locals in
  fn_8010D20C take the loop index into the first sweep.
- Swapping inline-local declaration orders (fn_8010E138's z/x, and the draw
  helpers).

## Other forms that mattered

- **Prototype widths.** fn_800D5CB8 declared with u8 parameters stops the
  frontend hoisting the red byte, which is read straight from a struct local at
  offset 0. The s32 prototype that cursor_bios.c uses hoists all four bytes.
- **NULL tests in the caller.** The debug-draw helpers' groups are tested by
  the callers, not inside the helpers.
- **Base loaded, then offset added.** Retail loads the grid's cells, indices
  and triangle bases straight into the pointer's callee-saved register and
  adds the offset afterwards (`cell = grid->cells; cell += ...`). A single
  `&grid->cells[...]` expression loads into a scratch register instead. Both
  grid functions in the Walk and Thru TUs need this.
- **Indexing instead of scan pointers.** The duplicate tests index the output
  list (`outTris[k].id`), and appends use `addList(&outTris[outCount], ...)`.
  XD's addList takes the list and an index
  (addList__FP25@class$515GScolsys2Thru_ciP5GSvecP5GSvecUs). With scan
  pointers, passes 2 and 3 swap the two volatile registers.
- **String pools.** A unit can own anonymous literal pools only when no
  unlinked unit reads the same constants by name. An owned literal does not
  export `lbl_...`, so the link fails. In that case, split the function out
  (if it uses no literals) or link the whole TU. An extern stand-in is CSE'd,
  and a volatile one reloads too often.
- **Pool names.** When code reads pool entries through named statics, name
  them after the pool labels (the colsys colours). objdiff then pairs them.

## Walls left

- **getCpPolyVec (GScolsys2Walk.c, 99.38%) and GScolsy2UtilChkInTri (99.56%).**
  In the unrolled edge test, retail has (p.z - z) in f2 and (vn.z - z) in f3.
  FPR colouring is greedy in reverse creation order, so retail must create
  (p.z - z) after the subtracted product's operands. MWCC generates
  `a*b - c*d` operands left to right. None of these changed the order:
  about 40 forms, including operand orders, `-c*d + a*b`, a comparison of the
  two products, locals for each term, memory operands, casts, and a
  cross-product inline with every argument order (read-only inline arguments
  are substituted). Until this moves, GetLayer stays unlinked: it shares the
  Walk pool with getCpPolyVec.
- **GScolsys2Thru candidate** (0x801101B4-0x80111470):
  - fn_801101B4 99.16%, GetFixedMdlEventList 99.29%, GetMdlEventList 99.24%.
  - Common to fn_801101B4 and GetMdl: the vertex-transform loop's two
    strength-reduced pointers take r15/r16 (r19/r20) swapped. Retail creates
    the destination pointer first. No argument form, statement split, inline,
    or unprototyped call changed it.
  - GetFixedMdl also still assigns the grid bounds' float registers
    differently.
  - GScolsys2ThruGetEventID (GScolsys2Thru_candidate_8011163C.c, 100% as a
    candidate) can link only together with this range, because both read the
    pool's 0.0f/1.0f.
