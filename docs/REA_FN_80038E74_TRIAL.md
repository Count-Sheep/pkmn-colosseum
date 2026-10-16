# PDA Button Callback Acceptance

Date: 2026-10-16. Target: `fn_80038E74`, 0x80038E74-0x80039004.
Source: `src/game/menu/pda_candidate_80038E74.c`.
Unit: `main/game/menu/pda_candidate_80038E74`.

The function and complete unit are **100% and linked**. This clears the last
member of the requested PC/People driver, save-handler and button-callback
decomp set. It does not implement native bindings or certify a headed run.

## Decisive Fix

The former source declared `lbl_80267060` as mutable bytes, although its
existing data owner, `src/game/data/rodata_80267060.c`, defines eight constant
24-byte records. Restoring constness and the record type changes the
scheduler's memory dependencies. The loop still checks the same eight IDs;
the enabled flag is the same byte at record offset 0x14. The coordinate
array is now declared as `f32 [8][3]`, matching its initialization accesses.

| Measured candidate | Match | Text bytes |
|---|---:|---:|
| Original byte-indexed source, live-byte fidelity checked | 96.4% | 404 |
| Mutable records, full-width index | 95.55% | 400 |
| Const byte-array declaration, widened enabled local | 99.45% | 400 |
| Const record array, u32 enabled, float X local | 99.8% | 400 |
| Const record array, u32 enabled, s16 X local | 100% | 400 |

At 99.8%, exactly four instruction words differed, exchanging r0 and r3
between the enabled flag and converted X coordinate. Converting into an
s16 local before the flag load fixes those four words. The final canonical
build reproduces the isolated result, including relocation destinations.

The first coordinate read remains volatile, and the function retains
`peephole off`. Removing volatile gives 99.8%; enabling peephole gives
76.99%. These are documented byte-match-first exceptions, not proof that
the original developers used volatile or that this is exception-free C.
No helper, artificial index narrowing, assembly, or object patching was added.

## Analysis And Rejected Paths

The prior REA/compiler trace established that the mutable full-width
candidate delayed its position-pointer add and prolonged temporary live
ranges. GC/1.3 and the GC/2.6 debugger lane reproduced identical code.
Counter-width, lookup-helper, expression-order, conversion, pointer-reuse,
and compiler-pass probes did not beat the original source. Twenty installed
compiler versions were checked against three seeds; none replaced the
needed declaration correction. Some early generated C variants failed
because declarations followed statements; these are failed probes, not
matching evidence. No failed or regressive variant was promoted.

The most useful harness lesson is to preserve declaration qualifiers from
the real data owner, then measure conversion width and timing together.
Changing only the local cast or statement order missed this combination.
The trace narrowed the search; the compiler and objdiff supplied acceptance.

## Validation

- `configure.py --no-progress`, all-source/report, full `ninja`, and progress.
- Independent SHA-1 check: all 26 canonical DOL/REL files pass.
- Entire allocated unit: only 400 text bytes, matching flags and alignment.
- All seven relocation destinations and the single export match retail.
- No extra emitted function or data; no split or symbol changes.
- Native policy freshly accepts this callback, its direct
  `menuGetCursorItemID` callee, and the five previously accepted driver/save
  functions, with their documented exceptions.
- Full pre-integration report comparison: 8,677 function identities
  preserved, no score or linked-function regressions.
- Callback static closure: four reachable callees, no rejected or unmapped
  functions. This check uses no native-owned exclusions or dispatch tables.
- Full callback quality scan, 41 quality tests, 2,361-file wrapper scan,
  and function-address metric integrity pass. The metric scan checks address
  uniqueness, not a separate DOL-only denominator comparison.

Local generated evidence is intentionally untracked under
`build/decomp-work/rea-audit/`: `verify_widget_acceptance.py`,
`widget-acceptance.json`, `widget-before-report.json`,
`pda-widget-const/`, `pda-widget-const-allocation/`, and `pda-widget-cleanup/`.
The acceptance record includes HEAD, source/report digests, raw text and
relocation digests, native policy results, and all canonical hash results.
Regenerate it after committing to bind that evidence to a clean revision.
