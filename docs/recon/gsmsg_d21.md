# GSmsg pool object: lane D21 (2026-09-30)

This continues docs/recon/gsmsg_d14.md and docs/recon/gsmsg_d13.md.
Branch `claude/decomp-d21-gsmsg`.

## Result

| Commit | Change |
|---|---|
| 13a65418 | fn_800FD69C (XD `_msgMakeTexture`) 91.49% -> 100% (report). Every instruction now matches; only the pool relocations lbl_8047CD10 and lbl_8047CD4C differ, and the pool object fixes those. |

fn_800FAEF8 is unchanged at 99.39%, so the pool-owning object still
cannot link.

## fn_800FD69C: how it went exact

1. **The simulator located the wall.** In the multiply-assigned-`lo`
   form (`lo = ...; lo += (s32)buffer;`), D16's simplify simulator
   (`scratchpad/d16/simp.py`, K=29, `pass` sweeps) reproduces the
   allocation exactly. A what-if beam search moved nodes only within the
   frontend range (named/@ objects) or only within the backend-temp range.
   It stalled at 11 mismatches. It reaches 0 only when the `lo + image`
   value is a backend temp numbered after the `hi` shift temp: r120 in
   loop 1, r133 in loop 2. Across about 300 debugger dumps, a named or @
   frontend object never gets a vreg after a backend temp. So retail
   computes `lo + image` inside one expression, not as a variable.
2. **The lowering rule.** When MWCC lowers `EINDIRECT(EADD(EADD(X, Y), Z))`
   to an indexed store, it takes X as the base and emits `add T = Y + Z`.
   For `EADD(X, EADD(Y, Z))` it takes Y as the base. The frontend orders
   the operands of the inner sum first. With `hi` written `* 32`, the
   frontend puts `lo` first, which gives base = lo. With `<< 5` it keeps
   `hi` first. The retail form is therefore:

       *(u8*)((((x >> 3) + tile) << 5) + (((x & 7) + texel) >> 1) + buffer) = v;

   This gives `add lo,lo,image; stbx v,hi,lo`, with the chains interleaved
   as retail has them. The `(u8*)` cast is needed while `buffer` is a
   `u8*`.
3. **Second loop.** With the store fixed, the what-if needed only one
   more change: rowIndex numbered after the loop-2 xPos web temp. Reusing
   `row` for the second loop makes its counter a split web (@ temp):
   `for (row = 0, srcOffset = 0; row < arg3; row++)`. The `row = 0` has
   to come first, otherwise the result is `li r0,0; mr r3,r0`. This gives
   retail's `li r3,0; mr r0,r3`, with xPos in r10.

The scripts are in `scratchpad/d21`:

- `wi.py`: what-if with the retail want-map translated between dumps.
- `beam.py`: constrained beam search.
- `gen.py` / `gen2.py`: small-file store-form scans.
- `run.py`: variant runner.

## fn_800FAEF8: what the copy really is

- **`mr r7,r30` is made after register allocation.** The debugger dump of
  the current form has a pass, `backend-17-after-common-subexpression-elimination`,
  that runs after regalloc. It rewrites an `addi rX,base,0x5D0` into
  `mr rX,rY` when a physical register still holds that value. The
  pre-RA CSE passes (01, 12) are block-local and never make this copy. So
  retail needs, before RA:
  - A = `addi base,0x5D0` placed before memset, used by memset,
    msgSetFontInfo and the loop;
  - B = a second `addi base,0x5D0` after memset, used by the fill;
  - the flag store already folded to `0x5D0(base)`.
- **Add propagation (passes 03/07/11) is global.** It folds every d-form
  use of an addi, loads in other blocks included (checked in
  `dq/backend-03`: `lhz r88,r62,0x20` becomes `lhz r88,r66,0x5f0`). It
  then deletes the addi. Over all ~300 dumps, no addi temp whose remaining
  uses are only d-form memory ops survives to regalloc. So retail's B
  must have a non-d-form use, or a second definition, at pass 11. That
  use or definition then disappears during or after register allocation,
  for example a copy that RA coalesces. None of the forms below produces
  that.
- **Forms tried (all at or below 99.26% under cmp.py; the current form
  scores 99.26 in cmp.py and 99.39 in the report):**
  - A 504-variant grid over:
    - Head binding (`t = NULL`, none, returning an inline local
      `u8* r = t`, `return t`);
    - where the flag is set (Head, `base[0x5D0]`, `work[0]`);
    - where `work` is assigned (before Head, after Head, after Rest,
      after msgSetFontInfo, from Head's return);
    - Rest's argument and binding (including a local `p = t` copy);
    - msgSetFontInfo's argument.
  - A multiply-assigned `work` and a multiply-assigned `base`.
  - Reusing `str` as the fill pointer (it splits into its own web and is
    folded).
  - Labels, goto and `do {} while (0)` before the fill (no new block
    survives).
  - A msgSetFontInfo copy that reads the id through the fill pointer
    (bound and unbound).
- **Closest structure.** `work = msgPrintHead(base + 0x5D0)` with
  `static inline u8* msgPrintHead(u8* t) { u8* r = t; memset(r, 0, 0x68);
  t[0] = 1; return r; }` gives retail's A (r30 for memset,
  msgSetFontInfo and the loop) and the flag via `1488(r31)`. The fill is
  then folded to base offsets (97.13%). What is missing is a fill pointer
  that survives add propagation.

## Pool object

Not attempted, because fn_800FAEF8 is not exact. The plan in
gsmsg_d14.md still stands: `.text` 0x800F9D04-0x800FE35C plus `.sdata2`
0x8047CD00-0x8047CD50, which starts on an 8-byte boundary.
