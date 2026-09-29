# `fn_8006B6B4` menu-rule save-section initialization

This function is a dependency of `savedataInit` (via `savedataGetPart(save,
fn_80128CF8)`), rather than `savedataCreate` itself. Retail receives that
section pointer in `r3`, clears `0xCC2C` bytes, and writes the menu-rule
defaults. The previous candidate was register-shaped pseudocode: it assigned
the incoming pointer from a zero-initialized local and had nonsensical
copy-loop counters. Its canonical function score was 55.51773%, with no
linked object.

The target's six calls to `menuCBRule_ConstantRule` use indices
`0, 1, 2, 0, 0, 0`. That already-matched callee returns one of three
`0x54`-byte rule records. Retail copies each record to consecutive save-section
offsets `0xC9D8`, `0xCA2C`, `0xCA80`, `0xCAD4`, `0xCB28`, `0xCB7C` using ten
eight-byte word transfers and a final word. Typed `0x54`-byte record
assignment reproduces this source-backed operation and code shape; ordinary
`memcpy` emitted six calls and scored only 34.212765%. The remaining stores
initialize three halfwords to 6 at `0xCB86`, `0xCB32`, `0xCADE`, six individual
flags at `0xCBD4..0xCBD9`, and seven two-byte flag pairs at
`0xCBDB..0xCBE8`.

Guarded objdiff of the reviewed C now scores **85.17731%**, up from
55.51773%; the six record-copy blocks and stores through function offset
`+0x1C8` align with retail. The candidate is 516 bytes against retail's
564 bytes. Retail advances a pointer by 2 and recomputes its high-half
base for each later flag pair; MWCC folds the same explicit C writes into
constant offsets from one base, omitting those pointer instructions. A
source-level helper returning an advanced pointer produced the same
564-byte length but only 74.14893%, so it was reverted. A source-level
helper around each pair left 81.92908% unchanged and was also removed.
No volatile qualifier, dead copy, or compiler-control pragma was added to
force the remaining pointer sequence.

`configure.py` still declares `menu_middle_r48_8006B6B4_o2.c` a
`CodeCandidate`, and its included residual source also emits
`fn_8006B5D0` outside this `0x8006B6B4..0x8006B8E8` split. Even an exact
function would need an isolated, whole-object linked split and the normal
quality/hash gates before Recomp can bind it. The current improvement is
source reconstruction only, not accepted port code.

## Resolved (lane D11)

Linked as `src/game/menu/menu_middle_exact_8006B6B4.c`, 100% under the menu
units' flags (GC/1.3, -O4,p, -opt nopeephole). Two fixes did it:
- The rule records are at `0xC9DC`, `0xCA30`, `0xCA84`, `0xCAD8`, `0xCB2C`,
  `0xCB80`, four bytes past the offsets above; the copy loop's `r5` is
  destination minus four.
- The flag pairs are cleared by `for (i = 0; i < 7; i++, p += 2)`, with the
  pointer advanced in the loop header. MWCC keeps that pointer's IV through
  unrolling, which gives retail's per-pair `addi`/`addis`.
No pragma or stand-in.
