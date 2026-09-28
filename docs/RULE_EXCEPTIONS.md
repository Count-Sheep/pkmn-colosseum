# Title-path rule exceptions: to revisit

User decision (2026-09-28): "if we can get items to 100% byte match for the
title screen do it. Record anything that breaks rules and we will return to
them."

For code on the title-screen path (the recomp checker's rows and their
closure), a byte-exact form may be used and linked even when it breaks the
strict acceptance policy in `docs/CAMPAIGN_OPERATIONS.md`. Every such use must
be:

1. **Tagged in the source**, right at the construct, with
   `RULE-EXCEPTION(title-path): <rule broken> — see docs/RULE_EXCEPTIONS.md`,
   so `grep -rn "RULE-EXCEPTION" src` finds them all.
2. **Listed below**: function, file, the rule it breaks, the form used, and
   what a clean fix would need.

These are not strict wins. They stay on this list until a policy-clean form
replaces them. Nothing here overrides AGENTS.md's hard rules: no `.inc`
files, no asm except the evidenced registry, and no extracted assets.

| Function(s) | File | Rule broken | Form used | Clean fix needs |
|---|---|---|---|---|
| fn_801B2038 (splGetSplinePoint; spline unit fn_801B18D8/1AD0/2038/2560 linked with it) | src/hsd/spline.c | Temporary whose only effect is scheduling (`f32 t = tension;` in splGetCardinalPoint) | Local copy of tension, read through on car1's reads (lane SP1, 0840b8d9) | A form without the copy that gives retail's schedule; SP1/B15 ruled out ~everything, GNT4 confirms the source otherwise |
| GSgfxDLBegin | src/game/gs_gfx_dl_exact_800DAF60.c | Single-use inline helper (GSgfxFindFreeDLCapture) with register-only evidence | Helper kept; carve linked | Repeated expansion or a listed fingerprint for the helper, or an in-place form (93.0–95.8% so far) |
| windowOpen, windowCreateCursorSprite, _winCalcWindowSize | src/game/window_r50_80104CA0_suffix.c | Extern named stand-in for the TU's own pool literal (lbl_8047CDEC, 1.0f) | `extern const f32 lbl_8047CDEC` | Link a unit that also owns windowDrawSprite2 and the literal, so the 1.0f is a real literal |
| fn_801845E4 | src/game/people/people.c | Invented unused local (dead) | `GSvec offset = {0.0f, 0.0f, 0.0f};`: places the orphan 12-byte zero image at 0x80273FCC and takes fn_801845E4 over the auto-inline limit (lane P30d) | Evidence of the image's real owner (XD demo map points at the stripped peopleRandomWalkPause, which has no body anywhere) |
| fn_80181850 | src/game/people/people.c | Block-scope extern to stop an inline | `extern void fn_8018F30C(void);` inside fn_80181850 under `-inline auto` (lane P30d) | A natural form that keeps fn_8018F30C out of the auto-inline candidates |
| fn_800F5A3C, fn_800F5CA0 (gs_vm.c linked; rows 23 and 24) | src/game/gs_vm.c | Dead store to an inline parameter | `desc &= ~0x80;` after the pop in GSvmGetOperand's 0x80 branch (lane V23b) | An evidenced construct that assigns to `desc` or takes its address. No evidenced debug macro (GS_ASSERT etc.) binds it; only a write or `&desc` does |
| gs_vm.c string/pool constants | src/game/gs_vm.c | Named stand-ins for the TU's own pool literals | Named `const char` arrays lbl_80271068/8027107C/80271294/802712B8/802712E4 and `const u8 lbl_8047CCB8[2] = "\n"`, which separately linked pieces reference by name | Link the whole TU 0x800F10E8–0x800F78A4 as one unit (about 10 units, including input.c) so these become literals again |
