# Card-e Transfer Unit Acceptance

Target: `fn_80084A8C`, `0x80084A8C`, 12,380 bytes (`0x305C`).
Unit: `main/game/menu/cardesavedata_candidate_80084A8C_o3`.
Source owner: `src/game/menu/cardesavedata.c`.

## Result

- Canonical function match: 99.82682% -> 100%.
- The entire single-function unit is linked and complete.
- All 12,380 allocated text bytes match retail, with identical section flags
  and alignment, all 367 relocation destinations, and the sole function export.
- All 26 canonical DOL/REL SHA-1 checks pass.
- The 8,677-function inventory is unchanged, with no score or linkage regressions.
- A fresh native policy assessment accepts the unit with its documented pool
  exception. This is not an exception-free source claim or a native runtime test.

Whole-project exact code rises from 2,304,708 to 2,317,088 bytes. Linked code
rises from 2,287,820 to 2,300,200 bytes. Exact functions rise from 8,541 to 8,542;
complete units rise from 1,917 to 1,918.

## What Fixed It

The existing draft had the right control flow and code size. Swapping the
`hero`/`k` declaration positions recovered their register allocation. Holding
the language getter's result in a `u8` local recovered the comparison operands.

The decisive fix was restoring the compiler's read-only pool context. The
already checked-in 416-byte `lbl_8026F2E8` pool contains twelve state flags,
four initial-state rows, 46 message records, and the two filenames. Separate
typed objects with `pool_data on` make MWCC use one shared pool base. Referring
to the named initial-state table then emits the retail address sequence and
the correct registers throughout the unrolled port-initialization loop.

Manual `data + 0x30 + mode * 0x10` arithmetic is behaviorally equivalent but
not equivalent compiler input. A single struct containing the whole pool also
failed to reproduce the named-object address form.

The wrapper now emits only the assigned function. Existing import tools verify
the whole read-only pool, four float/conversion literals, and the eight-entry
switch table before redirecting relocations to the existing data owners.
They do not change instruction bytes. `force_active` retains the sibling
status-display tables while that function remains outside this unit.
See `RULE_EXCEPTIONS.md` for the eventual removal condition.

## Pipeline Lessons

The stored m2c winner reproduced the old baseline. A total of 271 bounded probes
were recorded across 16 batches; 265 compiled. Arithmetic regrouping, casts,
temporary lifetimes, helper duplication and compiler options did not solve the
pool problem. GC/2.0, 2.5, 2.6 and 2.7 produced identical baseline instructions.

Compiler tracing verified GC/2.6 fidelity and captured the initial AST/backend
folding. Both trace runs hit their 180-second cap; no complete optimizer trace
is claimed. The earlier REA batch dossier covered only 16 bytes of this
12,380-byte function, so it was not usable as a whole-function analysis.

For near-exact functions with repeated shared-base accesses, recover actual
data objects and their pool order before spending a broad search on equivalent
pointer expressions. Validate object ownership and relocations, not only the
function percentage. The focused fix and rejected probes are recorded in
`remaining-function-notes.json` and `remaining-functions.csv`.

Local measurement artifacts and the acceptance verifier are under
`build/decomp-work/rea-audit/`, including `carde-acceptance.json` and
`verify_carde_acceptance.py`; generated objects and disassembly are not committed.
