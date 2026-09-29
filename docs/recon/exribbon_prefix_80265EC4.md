# exribbon prefix: exact whole-object linkage

The original `game/gs_range_80265EC4.c` CodeCandidate covered six functions
across `0x80265EC4–0x8026635C`. Its first three functions, including
`exribbonInit`, were individually exact but could not count as linked because
the later two candidate functions kept their shared object incomplete.

The first three functions are an existing contiguous, text-only span
(`0x80265EC4–0x80265F94`, 208 bytes). Compiling that span with GC/1.3 and
unit-wide `-O4,s` produces the retail instructions and relocations for all
three functions without source-local optimizer or scheduler pragmas. The
remaining span (`0x80265F94–0x8026635C`) retains the original GC/1.3
CodeCandidate flags. The source gates select these two authentic spans; no
data ownership or symbol addresses were invented.

Canonical `build/GC6E01/report.json` after the split:

| Unit | Status | Functions | Code |
| --- | --- | --- | --- |
| `main/game/gs_range_80265EC4_exact` | `complete: true` | `exribbonSetNo`, `exribbonGetNo`, `exribbonInit`: all 100% | 208/208 matched |
| `main/game/gs_range_80265F94_suffix` | `complete: false` | `fn_80265F94` 95.39429%, `fn_80266250` 97.69231%, `d2presentOpen` 100% | 96.17355% fuzzy |

Guarded `configure.py --no-progress`, `ninja -j2 all_source
build/GC6E01/report.json`, and `ninja -j2` passed. The full build verified
`main.dol` and `common_rel.rel` against `config/GC6E01/build.sha1`.
`test_quality_scan.py` passed 27 tests, and the local quality scan accepted
all three source files. The suffix remains a decomp task; this change does
not claim it as linked progress.
