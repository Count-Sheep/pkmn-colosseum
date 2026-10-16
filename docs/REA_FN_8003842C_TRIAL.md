# PDA Driver: Data-Pool And Allocator Recovery

Target: `fn_8003842C`, `0x8003842C-0x800388C4` (1,176 bytes).
Unit: `main/game/menu/pda_candidate_8003842C`.
Date: 2026-10-16. Base revision: `ac4e47fe88f933dcd4f7a983199bb3b6c84aa555`.

## Result And Scope

The live canonical report measures the function and its complete section set
at 100%. The unit now owns `.text` (1,176 bytes), `.data` (240 bytes), and
`.bss` (280 bytes). The complete source unit is linked as `Matching`, all 26
canonical hashes pass, and fresh native policy assessment is `accepted`.

This is the PC/People driver requested by the native team, not an implementation
of its native binding or acceptance of the incomplete save-path owner containing
`fn_80038A0C` and `fn_80038E74`.

## What Changed

1. Recovered local data-pool context instead of treating the tables and model
   as arbitrary offsets from external byte-array pointers. The existing
   192-byte template contains two independently copied four-row tables, each
   96 bytes. They are declared separately at `0x802E51C8` and `0x802E5228`.
   The adjacent position/float objects retain their bytes and offsets.
2. Moved those existing data definitions and the three existing BSS objects
   into the driver owner. The remaining data/BSS owners begin at `0x802E52B8`
   and `0x803A65B0`. No data bytes were invented and no address was moved.
3. Selected GC/1.3.2 C++ with C linkage. In the measured C variants, BSS
   allocation followed first reference, giving the wrong model/row order.
   C++ emitted the existing definitions in their correct declaration order.
   This is an explicitly documented code-generation choice, not proof of the
   original source language.
4. Set source before destination in each repeated reset. Gave the initial
   expansion's second loop its own destination, index, and source locals,
   in that declaration order. This resolved its six differing register operands.
5. Retained scoped `peephole off` and `opt_lifetimes off`. These preserve the
   retail copies, including the extra save-result copy. The source is tagged
   beside the settings and listed in `RULE_EXCEPTIONS.md`.

The standalone source no longer depends on the large candidate parent. Its
unused pointer parameters, unused local reference struct, unused helper, and
unrelated type/declaration context were removed and the live output rechecked.

## Diagnostic Lessons

The incumbent was 98.146255% in the canonical report and 97.908165% in the
standalone comparison. Preserve both comparison modes; do not substitute one
score for the other.

Forty initial pointer/result probes did not improve the incumbent. A further
118 records explored peephole rules, versions, scheduling and data ownership.
These include controls and are not 158 unique source transformations.

Reading the pinned MWCC implementation's
[pooling logic](https://github.com/rayanht/mwcc/blob/92f893fbcbb23d9833be5fc24ac9e069090f8729/src/backend/TOC.c)
suggested restoring the referenced objects. This was compiler research, not
consulting another Colosseum decompilation.

GC/1.3 and GC/2.6 produced identical stripped incumbents, but only 91.731544%
equivalent pooled candidates. A baseline compiler-fidelity check is therefore
not reusable after a context mutation. The diagnostic driver correctly stopped
before tracing the unequal candidate. With correct context, GC/1.3.2 through
GC/2.7 agreed; GC/1.3 remained different.

Storage experiments tested emission controls, visibility, initializers and
language mode. Force-active and common-off did not repair the C BSS order.
Explicit zero initializers still emitted into `.data`; failed or ineffective
pragma probes were not accepted as storage evidence. Two generator mistakes
(an overly broad static replacement and duplicate language flags) were fixed;
their failed builds are retained, not counted as successful source probes.

The layout-correct candidate reached 99.11565% standalone with six raw changed
instructions. A new candidate-specific GC/1.3.2-to-GC/2.6 fidelity check passed
at 100% before collecting complete frontend/backend/allocator traces. The
allocator evidence motivated distinct second-loop locals. The final isolated
text was byte-identical despite a standalone score of 99.2517%, because its
pooled relocation representation was compared against a text-only target.
After restoring real ownership in the canonical splits, the report was 100%.
No comparison setting or instruction bytes were changed to obtain that result.

## Evidence

Local, ignored artifacts under `build/decomp-work/rea-audit/`:

- `pda-driver-bss-order*/results.json`: storage probes, warnings, object offsets.
- `pda-driver-pooled-loops/results.json`: 54 compiler/local-layout records.
- `pda-driver-pooled-loop-trace/`: candidate-specific fidelity and full traces.
- `pda-driver-pool-allocation/results.json`: 34 focused allocation records;
  `s32_dest_index_source` is the first raw-exact variant.
- `verify_driver_acceptance.py` and `driver-acceptance.json`: raw section bytes,
  section alignment/flags, global exports and actual relocation destinations,
  native policy, report completeness and remaining-target regression audit.

Relocation comparison resolves defined symbols to section plus offset and
retains external names and addends. It does not equate wrong data layouts or
ignore relocations. The full canonical link remains the final address check.

The harness handoff is in the sibling project at
`Pokemon-Decomp-Harness/docs/REA_HARNESS_IMPROVEMENT_HANDOFF_2026-10-16.md`.

## Validation

- Configure, `ninja all_source build/GC6E01/report.json`, full `ninja`, and
  `configure.py progress` passed; independent SHA-1 checks passed all 26 files.
- All three allocated sections have identical bytes or BSS allocation, sizes,
  flags and alignment. All eight global exports retain their section offsets
  and sizes; all 65 relocation destinations match. Only the driver is emitted
  as a function.
- Fresh native assessment accepts both pragmas with no issues. The first
  assessment rejected the distant file-header tag; adding the same documented
  exception tag beside the pragmas fixed that provenance issue, without
  changing generated code or weakening the checker.
- 41 quality tests passed. No remaining-target score regressions were found.
- Linked code increases by 1,176 bytes and one complete unit; exact functions
  increase by one. Already-linked data transferred between owners is not new
  progress; the net newly linked data allocation is 280 BSS bytes.

Report SHA-256:
`342c850cb281c3de396f95421c229a3ac2e0b2518f0bbf6b63b5ea84f2eec1d0`.
Text SHA-256:
`97749f8dd198612dca49ab71cb151471c42da381c67dac441a6ce16de20d60e3`.
Data SHA-256:
`84182c06c1bef8ec9fe7935cb57679e2cc41cea30b45dd5a1c352d759ff13fad`.
Relocation-destination record SHA-256:
`d4e3a160ed1e25a85d93f56985ab108f58860c370e6e23fb15cef2034cd4bc56`.
