# Trainer Setup Unit Acceptance Evidence

`fn_8006B5D0` (`0x8006B5D0-0x8006B6B4`) now matches 100% in
`main/game/menu/menu_middle_r48_8006B5D0_prefix`. The unit owns only this
228-byte function and has no owned data or constant-pool sections.

## Source Recovery

The existing `MenuMiddleWork` and `MenuMiddleTrainerSlot` declarations describe
the four battle slots and their party copies. A normal indexed loop over those
arrays recovers retail's pointer induction and instruction order. The previous
candidate manually advanced byte pointers, introducing an extra pointer copy
and a different loop schedule.

The external trainer-ID and input-device tables have their actual types and
sizes: `lbl_8047C038` is an eight-byte `.sdata2` array of four `u16` values,
and `lbl_80267DD8` is a sixteen-byte `.rodata` array of four `u32` values.
These sizes are recorded in `config/GC6E01/symbols.txt`.

The m2c draft in batch `71e57738f2f4f348400c` independently confirms the
calls, field offsets, copying, and rule-mode dispatch. Its inferred `u16`
loop counter introduces an extra truncation when compiled; the accepted source
retains the original candidate's `u32` counter.

No compiler flags, split boundaries, shared headers, assembly, pragmas, or
policy exceptions were added. The existing GC/1.3 O4p/no-peephole unit settings
remain unchanged. The source owner now contains only this unit's function,
rather than including the broad shared menu source.

## Measurements

All build and comparison commands ran through the campaign build lock as
`CodexMenuMiddle`:

- `ninja all_source build/GC6E01/report.json` passed.
- `tools/decomp_work/fdiff.py` reports all 57 instructions equal.
- The regenerated report records 100% for the function and entire text section.
- GNU PowerPC objcopy extraction of both complete `.text` sections gives
  identical 228-byte strings, SHA-256:
  `5d4edc5eb192d912288dfb1f0ffa9e4360083b4a7bc8aceef8b6702aa969e452`.
- GNU PowerPC readelf comparison verifies all 11 relocations by offset, type,
  symbol name, and addend. ELF symbol-table indices differ as expected and are
  not compared as symbol identities.

## Integration Pending

Change the existing `configure.py` entry for
`game/menu/menu_middle_r48_8006B5D0_prefix.c` from `CodeCandidate` to `Matching`.
No additional configuration change is needed. This worker did not modify
configuration, commit, push, or claim completed linkage. The integrating parent
must regenerate configuration, run the full canonical link/hash checks and
policy checks, then refresh the report/dashboard before reporting linked
progress. Expected gain: one exact function (228 bytes), and independently one
linked unit (228 bytes) after that validation.
