# GSmsg pool object: lane D13 (2026-09-30)

This continues docs/recon/gsmsg_d14.md, which covers the object plan and
everything tried before. Branch `claude/decomp-d13-gsmsg`.

## Result

| Commit | Change |
|---|---|
| 7aa4bc98 | fn_800FAEF8 97.49% -> 99.39% (candidate) |

fn_800FD69C is unchanged at 91.49%. The pool-owning object still needs both
functions at 100%.

## How MWCC keeps a register copy (from mwcc-debugger IR)

These observations come from `scratchpad/d13/msg/dbg*`:

- **Parameter binding.** The inliner substitutes an argument expression
  into the body unless the body assigns the parameter. A bound parameter
  becomes a frontend temporary (`@NNN`). With an expression argument
  (`base + 0x5D0`), that temporary keeps `(temp + off)` addressing instead
  of folding to `base + (0x5D0 + off)`. However, the frontend still drops
  the binding when no later statement reuses the temporary through CSE.
- **Frontend CSE.** The frontend CSE maps a later `x = base + 0x5D0` onto
  the most recent temporary that holds the value, and emits
  `x = @temp`.
- **Backend CSE and copy propagation.** When two `addi rX,base,0x5d0` sit
  in different blocks (a call ends a block) and the first belongs to a
  temporary, the backend CSE turns the second into `mr rY,rX`. Copy
  propagation keeps that copy. When both sit in the same block, the copy
  is created and then propagated away. For two user variables in
  different blocks, no copy is formed.

fn_800FAEF8 (7aa4bc98) now splits _msgInitTask (NXXJ01.map GSmsg.o,
UNUSED 0x6C) into two inlines:

- msgPrintHead does the memset and sets the flag.
- msgPrintRest fills the fields.

Each binds its pointer through a dead `t = NULL;`. This produces retail's
two-register shape: one `addi` pointer for the memset, and a copy made
after the memset for the stores.

## What is left in fn_800FAEF8

- **The two pointers' roles are swapped.** Retail keeps the memset pointer
  (r30) for msgSetFontInfo and the loop, and the copy (r7) dies after the
  stores. In our form, `work = base + 0x5D0` is CSE'd onto the fill
  temporary, so the copy is the one that lives on. About 150 variants
  failed to flip it: statement orders, a work variable before or after
  each inline, labels and `goto` between the inlines, expression
  spellings (`&base[0x5D0]`, `(u32)` adds, casts), a separate pointer
  variable, and `#pragma opt_propagation off` / `opt_common_subs off`.
  They are in `scratchpad/d13/msg/v*.py`.
- **msgSetFontInfo addressing.** In this form, msgSetFontInfo's stores fold
  to base offsets instead of going through r30.

## fn_800FD69C

- D14's multiply-defined `lo` (`lo = ...; lo += (s32)buffer;
  *(u8*)(lo + hi*32) = v;`) gives retail's `add lo,lo,image; stbx`. The
  frontend normalizes the operand order, so `stbx` operand order follows
  register numbering.
- The second loop's `li r3,0; mr r0,r3` is probably the same copy
  mechanism: a CSE'd zero between two loop variables that are both
  multiply defined. Source copies (`srcOffset = rowIndex`) are propagated
  away.
