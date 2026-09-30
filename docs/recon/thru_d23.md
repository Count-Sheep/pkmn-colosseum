# GScolsys2Thru linked whole (lane D23, 2026-09-30)

This follows docs/recon/thru_d14.md. The GScolsys2Thru TU is now one
Matching unit, `src/game/GScolsys2Thru.c`:

- .text 0x801101B4-0x80111864: fn_801101B4, GetFixedMdlEventList,
  GetMdlEventList, GetEventList and GetEventID;
- .sdata2 0x8047CF48-0x8047CF60: the two {1, 2, 4} mask initialisers, then
  0.0f and 1.0f.

The pool entries are real literals, so D22's GetEventID extern stand-ins are
gone. `sdata2_8047CF60.c` keeps 0x8047CF60-0x8047CF68 for fn_80111864's
unit. Validation:

- the unit is 100%, with +10 byte-exact matches;
- check_regression reports no regressions;
- main.dol and common_rel.rel are OK;
- quality_scan is clean.

## The vertex-loop wall: two mechanisms

Retail's preheader is `addi dst,r1,verts; mr src,tri; li v,0`. Ours was
`mr src; addi dst; li v`, with the src/dst registers swapped.

### 1. Schedule order

The mechanism is MWCC's pre-RA list scheduler. Its tie-break is documented in
JackPriceBurns/mwcc `docs/SCHEDULER.md` at commit
ef08e865561446c072f457eaf89bde9030cccb30
(https://github.com/JackPriceBurns/mwcc). Candidates are compared on four
levels, in order:

1. urgency;
2. release count, the number of successors that this issue makes ready;
3. height;
4. a per-opcode byte: MR=0, ADDI=2, LI=4. Lower wins.

The three preheader instructions have no in-block successors. So they tie on
levels 1-3, and `mr` beats `addi` on the opcode byte, whatever the IR order.
Debugger dumps confirm this: the IR orders `li;addi;mr` and `li;mr;addi` both
schedule as `mr;addi;li`.

`addi` wins only when it has a successor it can release: a copy
`mr dst,X` of the address temporary X in the same block. After scheduling,
the allocator coalesces X into the dst IV, and the copy disappears. Two
conditions apply:

- **Coalescing.** `SpillCode_CoalesceCopies` (same repo, `src/backend/SpillCode.c`)
  merges two vregs only when both are inside the coalesce window.
  - Inline locals and parameters are inside the window.
  - User variables of the function are not. With one, the extra `mr` stays
    (mini test k5).
- **The copy surviving the frontend.** A dead `t = NULL` or a plain
  returned pointer is not enough; the frontend propagates `verts` back into
  the IV init. X needs a later use. It has to be a d-form memory use, which
  the backend folds back onto r1. addList's struct copies are that use.
  - A later call-argument use would keep X live instead (D14's -21 rows).

### 2. Registers

The GPR model is D16's `simp.py` with K=29 and `pass` sweeps, plus
`simg_sel.select`. It reproduces these dumps exactly.

- The low-degree tail pops in descending vreg order.
- `@` temporaries get vregs in reverse creation order.
- The inliner expands level by level. First the calls in the function
  (model pass @14x-15x, fixed pass @17x), then the calls inside those bodies
  (the fixed pass's checkPolyLine param `@203`, and so on).
- Strength-reduction IVs come after all of these. The dst IV is created
  before the src IV, whatever the source order.

What the what-if search needed:

- **fn_801101B4.** The order has to be count/fixed-normal, then src, then
  dst root, in pop order. The src pointer's temporary must be created after
  the fixed pass's `@203` and before the IVs. A user-incremented `src` local
  in an inline two levels below getMdlEventListPass does this
  (`mdlXformVerts` -> `mdlXformVertsBody`). The copy X is `dst = verts` in
  getMdlEventListPass, which addList reads.
- **GetMdlEventList.** The vregs must satisfy count < dst root < src.
  `xformed = mdlXformVertsCopy(mtxInv, tri, verts)` does this:
  - locals declared as count, dst, src, copy;
  - a user-incremented dst pointer copied from `copy`;
  - the increments written `count++, dst++, src++`, which gives the latch
    order `addi v; addi src; cmpwi; addi dst`;
  - the function returns `copy`, which addList reads.

## Forms that did not work (mini tests in scratchpad/d23/m)

These gave the same `mr` first:

- source order of the IV inits or the increments;
- argument-order wrappers, dead parameter binds and statement splits;
- user src/dst pointers;
- 120 declaration permutations.

`dst = mdlXformVertsCopy(...)` with one helper shared by both functions
gets fn_801101B4 to only 22 rows. The two functions need different shapes.

## Tools

The scratch tools are in `scratchpad/d23`:

- `mv.py`: mini-test variant runner.
- `rv.py`: real-file variant runner.
- `wi.py`: what-if renumbering over D16's simulator.
- `ord.py`: colour-order dump.
- `cm.sh`: merged-unit compare.
