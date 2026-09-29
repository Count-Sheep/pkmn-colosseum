# `fn_801A02B0` / `HSD_JObjRemove` candidate wall

Measured on 2026-09-28 with `objdiff-cli diff -p . -u
main/hsd/hsd_jobj_suffix_8019FF74 fn_801A02B0`, under the campaign build
lock. The active `src/hsd/hsd_jobj.c` candidate is 99.0184% for the 0x28C-byte
function, but its object is `CodeCandidate`, not linked progress.

The only live text mismatch in the best candidate is at the pooled-string base
setup. Retail emits `lis r3, lbl_80274AA0@ha; addi r31, r3,
lbl_80274AA0@l`. The candidate inserts `addi r0, r3,
lbl_80274AA0@l` before the saved-r28 store and then emits `mr r31, r0`
where retail has the direct `addi r31`. This is an inlining/register-allocation
effect, not missing game logic.

Controlled source tests on the same object:

| Source variant | Score | Outcome |
|---|---:|---|
| Existing local `base` passed to `JObjGetPrev` and `jobj_Unref` | 99.0184% | Two-instruction prologue wall |
| `u8* const base` | 99.0184% | Identical output |
| Direct global for assert only or `JObjGetPrev` argument only | 96.993866% | Prologue wall remains; later code differs |
| Direct global for `jobj_Unref` argument only | 93.269936% | Prologue becomes direct, but inlined unref code differs |
| Direct global at all three uses | 88.27608% | Direct prologue, large later mismatch |
| `jobj_Unref` without a base argument, with a local base inside | 93.613495% | Direct prologue, later mismatch |
| `JObjGetPrev` without panic-message argument | 96.993866% | Prologue wall remains |

The Melee `jobj.c` source has the natural `HSD_JObjRemove` flow: assert one
child, find previous sibling, replace links, then `HSD_JObjUnref`. It does not
establish a distinct source statement that would safely force this allocation.
The local `#pragma optimization_level 1` remains a title-path policy exception
if this function ever reaches 100%; it should not be counted as accepted at
99.0184%. Even an exact function needs an accepted, linked object. The current
suffix object also contains `HSD_JObjAddNext` at 85.64754%, so closing the
whole object or finding a valid carve with its authentic data/relocations is
the next linkage gate. Do not promote this candidate based on a text-only score.
