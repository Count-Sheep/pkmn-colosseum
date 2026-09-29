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
