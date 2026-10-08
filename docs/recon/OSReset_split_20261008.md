# OSReset split repair, 2026-10-08

The SDK split introduced by `f559f0cc1` created text ranges for the prefix,
the hand-written `Reset` routine, and the suffix, but left the prefix and
suffix wrappers empty. The report consequently lost seven function
measurements even though their C remained in `src/dolphin/os/OSReset.c`.

The wrappers now select their ranges with `OSRESET_PREFIX` and
`OSRESET_SUFFIX`. The common source preserves the original complete-TU form
when neither selector is present. In the split build:

- prefix owns `__OSReboot`, `OSRegisterResetFunction`, and `fn_8009FEBC`;
- `OSReset_exact.c` continues to own the unchanged 112-byte registered
  hand-written `Reset_8009FF50` routine;
- suffix owns `__OSDoHotReset`, `OSResetSystem`, `OSGetResetCode`, and
  `__OSResetSWInterruptHandler`.

`CallResetFunctions` and `CancelThreads` remain inline in the suffix, matching
retail's expansions. Calls to the isolated reset routine use its exported split
symbol. The suffix aliases retail's existing SBSS at `0x8047A738` through
`0x8047A760`; it emits no BSS and therefore does not duplicate or move state.

The prefix measures 99.6223%: `OSRegisterResetFunction` and `fn_8009FEBC` are
exact, while the previously known `__OSReboot` candidate remains 99.49519%
because MWCC retains one redundant address copy. The suffix is 100% across all
1,012 bytes and all four functions and is linked. The exact assembly island is
unchanged and remains linked.

Validation: configure, source/report build, full link, all seven canonical
SHA-1 checks, strict object-map freeze, whole-tree asm-wrapper scan, and quality
tests pass. The repaired report records 8,352/8,620 exact functions,
2,146,896 matched code bytes, and 2,092,520 linked code bytes.
