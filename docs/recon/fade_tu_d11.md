# Fade effect TU: flags, pool and whole-TU link plan (D11, 2026-09-30)

## Flags

The fade effect TU (fade_effect.c hooks 0x801C4814 - 0x801C4CB8 plus the
effect/fluid range 0x801C4CB8 - 0x801C766C) was built without the peephole
pass. The unit flag `-opt nopeephole` (GC/1.3 -O4,p) reproduces it; the old
per-function `#pragma peephole off` / `#pragma scheduling` blocks in
fade_range_801C4CB8.c are removed. With the flag:

- fadeEffectHookFunction_Doku is instruction-exact with natural literals and
  the FadeHook signature from fade.c
  (`u8 (u8 pending, f32 elapsed, f32 duration, f32 hookElapsed,
  f32 hookDuration, GStexture* texture)`). Without the flag the parameter copy
  is scheduled after the first divide (2 instructions off).
- The camera/trail init functions write through the global each time
  (`((FadeCameraWork*)lbl_80467030)->field`), not through a cached local:
  retail rematerialises the address after every call. That made
  fn_801C53BC, fn_801C5748 and fn_801C5D60 exact.

## Why Doku (and the rest) cannot link one function at a time

Doku owns the head of the TU literal pool, .sdata2 0x8047DFD8 - 0x8047E008:
the colour initialiser {0x1F,0x08,0x08,0xFF}, 0.5, 0.0, 1.0, 4.0, 2.0, 255,
640, 480, then the u8-to-float constant 2^52 at 0x8047E000 (0x8047DFFC is
alignment padding). The other fade functions' target objects reference those
literals by name (lbl_8047DFE0 about 50 times, down to lbl_8047E000 twice),
so a carve that compiles them as local @N literals leaves those references
undefined at link time. Extern stand-ins do not work for Doku either: the
compiler-generated conversion constant cannot be redirected to an extern
without changing its fsubs sequence.

So the TU links as a whole: one unit covering .text 0x801C4A44 - 0x801C766C
(merging fade_effect_candidate_801C4A44.c, fade_effect_exact_801C4C98.c,
fade_range_801C4CB8.c, fade_exact_801C6908.c and fade_range_801C6934.c) at
GC/1.3 -O4,p -opt nopeephole, with natural literals, owning .sdata2 from
0x8047DFD8 to the end of this TU's pool. The pool continues past 0x8047E090
(the range references lbl_8047E0A4 - lbl_8047E0E8, in
battle_sdata2_8047E090.c), so the end must be found where the next TU's
literals start. The 0x801C4814 Init prefix may stay separate (it uses no
literals).

## Still inexact (report after this commit)

| Function | Size | Score |
|---|---|---|
| fn_801C4CB8 | 0x704 | 99.54 |
| fn_801C55D8 | 0x170 | 97.77 |
| fn_801C6008 | 0x26C | 83.87 |
| _fadeEffectFunction_UDLR_FirstInit | 0x144 | 90.86 |
| fn_801C63C0 | 0x2C8 | 99.49 |
| fn_801C6934 | 0x1B4 | 92.86 |
| fadeFluidEvaluate | 0x1AC | 76.21 |
| fadeFluidCalcParms | 0x6C | 98.15 |
| fadeFluidInit | 0x43C | 72.63 |

Everything else in the range (21 of 30), Doku, carde and the 6908 carve is
exact.
