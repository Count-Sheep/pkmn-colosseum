# Archive Queue Worker Acceptance

`fn_80057E70` (`0x80057E70-0x80057F94`) is 100% matched and linked in
`main/game/menu/menuColosseumBattle_r56_80057E70_prefix`.

## Evidence

The Colosseum m2c batch `71e57738f2f4f348400c` recovered the short-circuit
archive-entry query/request condition. Its draft still omitted the entry-hash
argument to `fn_8017B07C`, because the candidate declaration lacked a prototype.
The accepted callee in `src/game/fsys/fsys_file.c` takes `(fileHandle, nameHash)`
and compares that hash with archive entries. Retail's caller loads the current
hash into r4 for its loop condition and retains it for this query. Supplying
`*entry` explicitly recovers that data flow and removes the last register
mismatch without compiler changes, assembly, or a policy exception.

The original candidate was 99.863014%. The corrected 292-byte function has all
73 instructions matching. The complete unit is now `Matching`; the sibling
`fn_80057F94` remains 100% matched and linked (444 bytes). No split boundaries
or compiler flags changed.

## Validation

Commands ran through the campaign build lock:

```sh
python3 configure.py --no-progress
ninja all_source build/GC6E01/report.json
ninja
python3 configure.py progress
python3 .github/scripts/check_metric_integrity.py build/GC6E01/report.json build/GC6E01/main.dol
python3 .github/scripts/check_asm_wrappers.py
```

All seven canonical binary hashes pass. Metric integrity reports 8,620 distinct
function addresses with no duplicate claims; the source scan finds no live
included-assembly wrappers.

Exact-source progress increases by one function and 292 bytes. Linked progress
increases by one unit and 292 bytes, independently. Totals after acceptance:
8,357/8,620 exact functions; 2,147,988 matched code bytes; 2,093,612 linked code
bytes (83.76378%). Unrelated pre-existing working-tree edits were not included.

The same batch's `menuShopOpen` draft was also checked, but its direct-global
variants did not improve the current match. Those exploratory edits were
restored; that function remains incomplete.
