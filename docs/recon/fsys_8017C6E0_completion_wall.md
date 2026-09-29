# `fn_8017C6E0` first-load entry completion wall

The retail function is 0x1AC bytes. The C source now measures
100.0% raw byte match and its one-function object links at the retail address.
It flushes the loaded entry, advances a
compressed entry to status 103/state 2, or completes a plain entry and invokes
the group's done callback. It is accepted only under the recorded title-path
source-policy exception, not as a strict source win.

The instruction diff shows the first divergence in the archive lookup: retail
loads `slot->entryIndex` into `r0`, homes it at stack +0x18, and reloads it
before indexing. The candidate keeps the index in `r17`. Retail then keeps
the lookup result in `r22`, copies it into `r28`, and copies that into `r31`
for the done-callback expansion. The candidate's earlier allocation keeps
the corresponding values in `r23`/`r31`, shifting the later group/callback
registers. The flush/state logic and callback calls are structurally present.

A prior focused test extracted the flush/state/done logic into a `static inline`
helper taking the entry pointer. MWCC produced byte-identical output
(89.392525%), so that change was reverted. A narrower `fsysGetCurrentEntry`
helper produced 89.24299%, and adding an `entry` local in it produced
88.252335%; both were reverted. Placing the entire status action in a
`static inline s32 fsysCompleteSlot(FSYSSlot*)` and returning its value from
`fn_8017C6E0` produced a 100.0% raw match without changing the action's
runtime semantics. That source boundary explains the index stack home and
both entry-pointer copies. The helper has only this call site and lacks an
independent source identity, so it is tagged `RULE-EXCEPTION(title-path)` in
the source and listed in `docs/RULE_EXCEPTIONS.md`, not claimed as a strict win.

Shared-lock `configure.py --no-progress`, `all_source`/report and full
`ninja -j2` passed with the object marked Matching. The report marks the
function 100.0% and the object `complete: true`. Both compiled and target
objects have only `.text` of 0x1AC bytes, one public `fn_8017C6E0` symbol,
no data section or surviving helper symbol, and the same eight `.text`
relocations (offsets 0x6c, 0x8c, 0x9e, 0xae, 0xb0, 0x114, 0x134, 0x144;
same types and symbol targets). Quality-scan tests (27) and a working-source
scan passed.
The retail SHA-1s match: `main.dol` 870e8b9693ca780782d80f22a6a4572d8ba9458f,
`common_rel.rel` e816761813900aa0fd4f87d012b3b2a19d2f003e.

`tools/check_object_map_freeze.py` currently fails on pre-existing,
repository-wide topology drift (expected 2,287 units versus 2,191 in the live
report, including DOL/REL totals). This one-function promotion did not change
the number or names of units. The freeze file was not modified here.
