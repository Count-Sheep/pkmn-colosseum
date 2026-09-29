# `cbStep` suffix-object wall (2026-09-29)

`cbStep` at `0x801D29D8` is registered by the newly linked `mailMainInit`,
but belongs to `main/game/mailMain_r54b_801D23C0_suffix`, a four-function
`CodeCandidate` object. The canonical report before this pass scored the
304-byte retail `cbStep` at 95.32895%; the candidate function was 316 bytes.

The source-backed correction uses the same direct nested allocation call and
`count, i, sent` loop declaration order already present in the related
`mailMainSendAllMail` source. It removes `cbStep`'s unnecessary `buffer`
temporary without changing call order or mailbox behavior. A guarded object
build and canonical report now score `cbStep` at **97.302635%**; its candidate
body is 312 bytes. The other three suffix functions retain their previous
canonical scores: `fn_801D2404` 98.02469%, `mailMainReceiveStart` 98.14815%,
and `chkMailSend` 81.815285%. The whole object remains unlinked.

Two extra candidate instructions remain against retail. After
`fn_801D16C4()` the compiler emits `clrlwi r3, r3, 24` because the shared
header correctly declares its byte-sized return. Retail passes the result
directly to `fn_801D1650`, whose implementation masks the index to eight
bits. Retail also emits `mr r30, r3` after `fn_8016557C()`, whereas the
candidate emits `mr r0, r3; mr r30, r0`. The first difference points to an
original caller declaration or ABI boundary not established by the current
header; a mismatched function-pointer cast or a broad header change solely to
remove the instruction is not justified. The second is a register/scheduling
wall, not evidence of a missing call or mailbox branch. Both require fresh
source provenance before an edit.

The guarded `ninja -j2 all_source build/GC6E01/report.json` passed. Current
`mailMain.c` SHA-256 is
`5e4a43dc062fa2ef9b23ae7955e70bbe763afe0d3ac87992d24f3e4a5ba5a92e`.
This is a measured source improvement, **not** an exact-function or
whole-object promotion.
