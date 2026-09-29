# GSmodelSetShadowTextureSize branch-layout wall

`GSmodelSetShadowTextureSize` at `0x800E8FA0` is a 72-byte retail function in
`main/game/gs_model_shadow_candidate_800E8F80` (`0x800E8F80..0x800E8FE8`).
The current C candidate is 94.166664% by objdiff. The same 104-byte object
also contains `GSmodelSetShadowDebug`, which individually scores 100%, but the
object remains `CodeCandidate` and is not linked/accepted.

Objdiff's only instruction-shape mismatch is the final signed height check:

| Retail | Candidate |
| --- | --- |
| `cmpwi r4, 0x1e0` | `cmpwi r4, 0x1e0` |
| `ble` to the two global stores | no instruction |
| `blr` for the rejected case | `bgtlr` |
| `stw r3, lbl_8047AB90@sda21` | same |
| `stw r4, lbl_8047AB8C@sda21` | same |
| `blr` | same |

Source-faithful trials of an explicit `height > 0x1E0` early return, one
compound bounds condition, and an `else if` for the final height condition
all compiled to the same 68-byte candidate. They were reverted. A branch-form
forcing trick would not be evidence of the original source. An authentic
standalone carve at the known `0x800E8FA0` symbol boundary could be considered
after the function is exact, but a carve now would merely leave a separate
unlinked 94.166664% candidate, so no split/config change was made.

This function remains a Decomp blocker for exact accepted source in the title
renderer closure. The Recomp should continue treating it as unaccepted and
must not infer runtime behavior from the candidate alone.
