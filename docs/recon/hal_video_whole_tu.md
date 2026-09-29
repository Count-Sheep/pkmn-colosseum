# HAL video.c whole-object linkage

The title frame-finish path calls `fn_801BF6AC` and `fn_801BF8A0`.
Previously `fn_801BF6AC` was 100% only inside an unlinked `CodeCandidate`,
while the legacy `hsd_video.c` version of `fn_801BF8A0` was 97.63158%.
Head/tail text carves linked, but their shared video literals and state were
separate data units.

The authentic HAL source arrangement is one `video.c` translation unit under
the existing GC/1.3.2 library flags. Its small draw-done waiting accessor is
out of line in retail and in [Melee's `video.c` at commit
`1e4b3b5adc74e52e420a86b3dd0da4bb67867cee`](https://github.com/doldecomp/melee/blob/1e4b3b5adc74e52e420a86b3dd0da4bb67867cee/src/sysdolphin/baselib/video.c),
where a local `dont_inline` pragma surrounds that same accessor. With that
pragma tagged as `RULE-EXCEPTION(title-path)` in our source, the full native
object emits the retail `fn_801BF8A0` (380 bytes) exactly. The linked object
also keeps Colosseum's newer `fn_801BF6AC` (500 bytes) exact.

`configure.py` and `splits.txt` now assign the whole 0x801BF1F0–0x801C0270
text range to `hsd/video.c`, together with its .rodata
0x802756F8–0x80275780, .bss 0x804657C0–0x80466DB8, .sbss
0x8047B380–0x8047B388, and .sdata2 0x8047DF30–0x8047DF40. The
canonical report marks all 13 functions 100%, 4,224/4,224 code bytes matched,
5,784/5,784 data bytes complete, and the single object complete/linked.
Section fuzzy percentages for .bss and .sdata2 are not themselves the link
gate; the report's complete-unit status and the full retail hashes are.

Guarded `configure.py --no-progress`, `ninja -j2 all_source
build/GC6E01/report.json`, and `ninja -j2` pass. The latter checks both
`main.dol` and `common_rel.rel` against `config/GC6E01/build.sha1`.
`test_quality_scan.py` passes. `check_object_map_freeze.py` still fails against
an older, broadly different topology baseline (2,287 expected versus 2,184
current units, including unrelated common_rel changes). That freeze snapshot
must be reconciled in a separate integration review; it was not widened here.

This is a linked title-path exception, not a strict-pristine source win: the
local compiler-control directive remains on `docs/RULE_EXCEPTIONS.md` until
source or vendor build evidence can replace it with a clean form.
