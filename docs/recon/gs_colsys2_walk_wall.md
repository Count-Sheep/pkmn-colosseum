# `GScolsys2Walk.c` whole-object closure

This retail TU owns `.text` 0x8010DE00-0x8010E53C and `.sdata2`
0x8047CEE0-0x8047CEF0. It remains an unlinked `CodeCandidate` because two of
its three functions are not exact:

| Function | Current fuzzy match | Residual |
| --- | ---: | --- |
| `GScolsys2WalkGetLayer` | 100% | Text exact, but object cannot yet link. |
| `getCpPolyVec__FP5GSvecP5GSvecP5GSvecP5GSvec` | 99.38356% | An `f2`/`f3` allocation swap in the three unrolled edge tests; constant-pool relocation names differ. |
| `fn_8010E138` | 99.66926% | An `r3`/`r4`/`r5` allocation cycle in grid-cell indexing. |

The candidate compiler-generated `.sdata2` pool is 12 bytes, versus 16 bytes
in the retail split. Its load relocations use compiler labels (`@25`, `@66`,
`@67`) rather than the retail `lbl_8047CEE0`/`E4`/`E8` labels. Exact code
alone would not establish whole-object linkage without resolving authentic
data ownership and section size.

2026-09-28 bounded checks: swapping grid `x`/`z` declaration order and
introducing a named cell index had no improvement. Extracting natural edge
differences into local float variables either left `getCpPolyVec` unchanged or
reduced its score; swapping the edge loop's `x`/`z` declarations also reduced
the score. All probes were reverted. No shaping, inline assembly, pragma, or
fabricated constant was retained.

After restoration, the shared-lock `all_source`/report build, `ninja -j2`,
and retail `shasum -a 1 -c config/GC6E01/build.sha1` passed (DOL and REL OK).
These hashes validate the still-unlinked build, not acceptance of this TU.
