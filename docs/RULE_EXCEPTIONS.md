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
