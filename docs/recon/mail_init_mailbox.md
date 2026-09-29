# Mailbox initialization object (2026-09-29)

`main/game/mail_candidate_801D1F0C` covers three functions at
0x801D1F0C–0x801D2080. `mailGetMailIDInMailbox` and
`mailGetNbMailInMailbox` were already byte-exact but sat in a `CodeCandidate`
object. `mailInitMailbox` was 93.24074%: the candidate emitted an initial
signed-index bounds test before its fixed 0x200-entry clear loop that retail
does not have. The mailbox index is naturally unsigned; declaring it `u32`
removes that test and makes the 0xD8-byte function 100.0%.

The local `peephole off` and `scheduling off` directives surrounding the two
already-exact siblings were removable without changing their bytes. This
carve therefore needs no `RULE-EXCEPTION(title-path)` tag. Its forwarding
wrapper now selects only the 0x801D1F0C–0x801D2080 definitions; the older
wrapper retains its existing source selection. This was necessary because
linking the previously broad wrapper multiply-defined earlier mail symbols.

`configure.py` now marks this complete object `Matching`. The canonical report
marks all 372 code bytes, all three functions, and the unit complete. Guarded
`configure.py --no-progress`, `ninja -j2 all_source
build/GC6E01/report.json`, full `ninja -j2`, `configure.py progress`, and the
27 quality-scan tests pass. The full link checked `main.dol` and
`common_rel.rel` against both retail SHA-1 hashes successfully.
