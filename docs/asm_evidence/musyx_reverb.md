# MusyX StdReverb routines in Pokémon Colosseum (GC6E01)

The retail reverb translation unit places ReverbHICreate, ReverbHIModify,
DoCrossTalk (0x80164C40), HandleReverb (0x80164DD0), and ReverbHICallback
consecutively. Its shared .sdata2 literals span 0x8047D4F0–0x8047D540.
The exact C functions cannot be accepted as a linked object without the two
intervening library assembly functions. Only these two named routines are
admitted; the source copies their explicit instruction bodies from the
independently matching MusyX decompilation cited below.

## DoCrossTalk

- Why it cannot be C: the retail routine uses paired-single `ps_merge00`,
  `ps_muls0`, `ps_mul`, and `ps_sum0` arithmetic and `stfiwx` stores in a
  hand-scheduled two-channel mixing loop. This unit's MWCC C compiler does
  not generate paired-single instructions from ordinary C.
- Other decompilations: [doldecomp/ttyd, commit 62131fc3,
  `libs/musyx/src/musyx/runtime/StdReverb/reverb.c`](https://github.com/doldecomp/ttyd/blob/62131fc3/libs/musyx/src/musyx/runtime/StdReverb/reverb.c#L126)
  keeps this routine as an explicit `static asm void DoCrossTalk` body.
- Origin: Factor 5 MusyX GameCube StdReverb runtime, `reverb.c`, 2.x family.
  The precise Colosseum library patch version is not established; attribution
  is from the source tree and matched symbols, not an assumed version number.

## HandleReverb

- Why it cannot be C: the retail routine saves and restores r14–r31 with
  `stmw`/`lmw` even though this translation unit is built with
  `-use_lmw_stmw off`. It also directly forms `lis`/`@l` addresses of
  small-data constants and hand-schedules the 160-sample DSP loop. This
  combination is not MWCC output for a C function under the unit's flags.
- Other decompilations: [doldecomp/ttyd, commit 62131fc3,
  `libs/musyx/src/musyx/runtime/StdReverb/reverb.c`](https://github.com/doldecomp/ttyd/blob/62131fc3/libs/musyx/src/musyx/runtime/StdReverb/reverb.c#L239)
  keeps this routine as an explicit `static asm void HandleReverb` body.
- Origin: Factor 5 MusyX GameCube StdReverb runtime, `reverb.c`, 2.x family.
  The exact patch version in Colosseum remains unproven.

## Upstream Contribution Audit (2026-10-02)

Audited upstream `dougchansan/pkmn-colosseum` at
`41df4aa988add7d69dfc16c06ac4d31df96f4362` against the local source snapshot
`e2fee1b38e2d31b015ba36d11ea18d796cc4db7b`.

The maintainer's [response on PR #635](https://github.com/dougchansan/pkmn-colosseum/pull/635#issuecomment-5958296388)
welcomes small, focused contributions, asks that source cleanups, newly linked
units, and tooling remain separate, and requests a separate proposal before
accepting changes that depend on additional evidence rules. This is not an
approval of our local library-assembly exception.

Validation on the local checkout:

- `python3 configure.py --no-progress` and
  `ninja -j 1 all_source build/GC6E01/report.json` pass.
- The refreshed MusyX category has 350/350 exact functions, 127,072/127,072
  matched code bytes, and 36/36 complete linked units. This includes the two
  assembly functions above; it is not a claim that all 350 are C functions.
- `ninja -j 1` passes the DOL and both REL checksum checks. The rebuilt DOL
  and retail DOL both have SHA-1
  `870e8b9693ca780782d80f22a6a4572d8ba9458f`.
- `check_metric_integrity.py` passes: 8,608 distinct function addresses,
  no duplicate claims, and the full-game code denominator within tolerance.
- Running the **upstream** `quality_scan.py` implementation's `scan_source`
  over all lines of each of the 36 report-backed MusyX source units rejects
  `DoCrossTalk` and `HandleReverb` in
  `src/musyx/runtime/reverb_candidate_80164520.c`. Both are outside its
  permitted SDK assembly exceptions. The other 35 units pass this structural
  scan; that is not a substitute for source review or an upstream-based build.

Consequently, a whole-module MusyX PR is **not ready under upstream's current
rules**. Do not copy our assembly registry or quality-gate changes into a
source PR to make it pass. The two routines need a separately agreed policy
proposal, or the reverb unit must remain out of a C-only contribution.

Contribution scope notes:

- 23 active source units differ from upstream, covering 283 functions and
  95,432 code bytes. Excluding the five-function reverb unit leaves 22 changed
  units, 278 functions, and 91,692 code bytes as the C-only review scope, not
  necessarily all newly matching progress.
- `stream.c`'s local `inline_depth(4)` pragma, and the XD comparison comments
  in the seq carves, are already byte-for-byte present in upstream. They are
  not new changes introduced by this contribution.
- Only carry selected active units and their required headers, license notice,
  object entries, and justified data/code split changes. Inactive MusyX
  research files contain legacy assembly wrappers and must not be swept into
  the PR by copying the directory wholesale.
- Prepare and build an isolated branch from upstream before publishing any
  subset. Keep unrelated game improvements, harness code, policy changes,
  generated products, and extracted data out of it.

No upstream PR or policy proposal was published by this audit.
