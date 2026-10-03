# Waza Reset Match

`fn_801DADC0` at `0x801DADC0` is byte-exact and linked through
`src/game/wazaSequenceSys_reset.c`, built with GC/1.3.2 and `-O4,p`.

The original extern-only candidate represented the receiver array, pool header
and scratch as fields reached through one pointer. That folded away retail's
two explicit pool-address calculations. Owning the actual BSS objects and using
direct receiver-array indexing reproduces the instruction and register sequence.

The unit owns 312 text bytes and BSS `0x80467C80-0x80467CF8` (120 bytes):

| Symbol | Size |
| --- | --- |
| `lbl_80467C80` | 64 bytes |
| `lbl_80467CC0` | 20 bytes |
| `lbl_80467CD4` | 36 bytes |

MWCC pools BSS in first-reference order. The uncalled `wazaResetBssLayout`
helper establishes retail's receiver/header/scratch order and is dead-stripped.
This follows the existing GSmsg exception and is separately recorded in
`docs/RULE_EXCEPTIONS.md`. It is not counted as a decompiled function.
The surrounding BSS candidate is split into a prefix and suffix so every
original symbol retains its address and has exactly one owner.

Validation in an isolated checkout of committed source:

- `python3 configure.py --no-progress`: passed.
- `ninja all_source build/GC6E01/report.json`: passed; reset is 100% in a
  complete unit, with 312 matched/linked code bytes and 120 matched data bytes.
- `ninja`: passed all three canonical SHA-1 checks below.
- `python3 configure.py progress`: passed.

| Output | SHA-1 |
| --- | --- |
| `main.dol` | `870e8b9693ca780782d80f22a6a4572d8ba9458f` |
| `common_rel.rel` | `e816761813900aa0fd4f87d012b3b2a19d2f003e` |
| `mail.rel` | `28bc997c8bc065db08fefc3c361fd5982e39dcbc` |

No symbol was renamed, no assembly was added, and the existing shadow getters
remain in their original linked unit. `menuNameEntry` is still a separate
incomplete candidate; this reset acceptance does not clear that blocker.
