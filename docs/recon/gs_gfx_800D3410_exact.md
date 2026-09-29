# `fn_800D3410`: linked GSgfx frame-setup carve

The report previously scored `fn_800D3410` (0x800D3410–0x800D361C,
524 text bytes) at 100% while the unit remained a `CodeCandidate`. Its
wrapper included all of `gs_gfx.c`; promoting that wrapper directly produces
multiply-defined GSgfx symbols at the DOL link. A function score alone was
therefore not accepted progress.

`src/game/gs_gfx_exact_800D3410.c` retains the recovered C function body in
a text-only, single-function translation unit with its external declarations.
The split and `configure.py` now select that unit as `Matching`, leaving the
adjacent 0x800D36B4 fog setter in its incomplete candidate. No assembly,
`.inc`, compiler-control pragma, asset data, or new behavior was added.

Validation: guarded `configure.py --no-progress`, guarded
`ninja -j2 all_source build/GC6E01/report.json`, guarded full `ninja -j2`,
the `build.sha1` DOL and common-REL checks, the quality scan of every new
source line, and all 27 quality-scan tests pass. The report marks the new unit
`complete: true`, 524/524 code bytes and one exact function. This does not
resolve the neighboring `fn_800D36B4` or make the native floor scene ready.
