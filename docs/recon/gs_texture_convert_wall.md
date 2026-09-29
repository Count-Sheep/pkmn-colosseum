# `GStextureConvertFromHW` renderer closure

On 2026-09-29, the source candidate in `src/game/gs_texture.c` improved from
61.18254% to 93.98412% in the canonical GC6E01 report (raw objdiff
93.59524%, 504 retail bytes). The declared object improved from 65.97143%
to 85.65238%. No symbol was promoted: `main/game/gs_texture` remains an
incomplete CodeCandidate, and its other function `fn_800EF098` remains
73.15476%.

The retained source changes preserve the observed behavior. The display
dimensions are loaded from 16-bit fields into signed 32-bit locals, matching
retail's signed word comparisons and avoiding the earlier narrow-local
codegen. One display base is shared across both reads. The existing
GX-format values are now expressed as an ordinary C `switch`; MWCC emits the
same sparse decision tree and case values as retail. The validation predicate
still permits only formats `0x40` through `0x45`, `0x90` and `0xA0`, with its
low/high branches written in the order retail uses. The unreachable `0xB0`
format arm remains in the mapping because retail contains it.

The remaining diff is dominated by saved-register allocation (retail keeps
the texture pointer in r26, clear flag in r27, image pointer in r31 and GX
format in r30; this candidate chooses another order). The validation's
`0x46` rejection also retains two extra early-return instructions. A
combined range predicate regressed the score and was reverted, as were
unhelpful declaration-order tests. Optional compiler register replay was
unavailable in this environment. There is no evidence for changing the
format values, GX call arguments, texture ownership or build flags to force
the remaining registers.

Guarded `ninja -j2 all_source build/GC6E01/report.json` and full `ninja -j2`
pass. The DOL and common REL SHA-1 values match `config/GC6E01/build.sha1`,
and the quality-scan tests pass. The recomp must continue treating this
callee as unaccepted and must not bind `fn_800D30F0`'s texture-conversion
branch from this source yet.
