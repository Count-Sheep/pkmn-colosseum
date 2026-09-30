# GSmsg pool object: lane D23 (2026-09-30)

This continues docs/recon/gsmsg_d21.md, gsmsg_d14.md and gsmsg_d13.md.

## Result

The GSmsg object links. `src/game/GSmsg_800F9D04.c` defines
`GS_MSG_POOL_OBJECT` and includes `gs_msg.c`. It is built with GC/1.3.2
and -O4,p -opt nopeephole, and owns three ranges:

- .text 0x800F9D04-0x800FE35C (GScharMakeFromSJIS through _msgGetSize);
- .bss 0x80401DE0-0x80402518, the TU's .bss;
- .sdata2 0x8047CD00-0x8047CD50, the literal pool in retail's order.

Checks:

- The report shows the unit at 100% code and 100% data, complete.
- main.dol and common_rel.rel are OK.
- check_regression reports no regressions, and fn_800FAEF8 is new.
- fn_800F96E4 stays its own linked unit. fn_800F9AEC and fn_800F9C04 are
  still candidates (the switch-leaf `beq` wall).

## Why fn_800FAEF8's `mr r7,r30` was a wall

Retail clears the print task through r30 (`addi r30,r31,0x5D0`) and fills it
through `mr r7,r30`. D21 showed that the copy comes from post-allocation CSE
rewriting a second `addi base,0x5D0`. Add propagation normally folds such an
addi into the store displacements. The per-use predicate is at 0x56bcd0 in
GC/2.6's mwcceppc.exe, called from the driver at 0x56aeb0. It only rejects a
use in these cases:

- the store's value is the pointer;
- the use is not a d-form, addi or mr;
- the base register is redefined in between;
- the displacement is out of range;
- one memory flag is set.

None of these can be reached from ordinary source for an extern array plus a
constant.

The GSmsg buffers are not extern arrays. They are pooled file statics: XD's
GSmsg.o .bss is 0x744 bytes of anonymous statics. MWCC addresses a file
static as `addi rX, poolbase, <object>`, and the compiler version decides
what happens to that addi:

- GC/1.3 folds the object offsets into the store displacements.
- GC/1.3.2 keeps them out (D15 saw the same in win_sprite).

With the buffers declared as statics and built with 1.3.2, the fill keeps its
own addi until register allocation, and post-allocation CSE produces
retail's `mr r7,r30`. The flag store still folds (`stb r8,1488(r31)`),
because it indexes the static directly. The rest of the TU compiles
identically under 1.3 and 1.3.2.

One register pair was also wrong. glyphWidth in msgSetChar (r29) was
coloured after the sprite branch's s16 locals. Declaring it `s16` and last
makes it the local's own web, numbered before those locals, and gives
retail's r29.

## The .bss

Retail's layout is:

| Offset | Object | Notes |
|---|---|---|
| 0x000 | lbl_80401DE0 task | global: the fn_800F96E4 unit names it |
| 0x068 | lbl_80401E48 task | GSmsgGetRect |
| 0x0D0 | u16 code[0x200] | lbl_80401EB0 |
| 0x4D0 | char str[0x100] | lbl_804022B0 |
| 0x5D0 | print task | lbl_804023B0 |
| 0x638 | lbl_80402418 task | msgPrintRect |
| 0x6A0 | lbl_80402480 task | _msgGetSize |
| 0x708 | lbl_804024E8 | the 0x2C MessageSystem; global, because .sdata lbl_80478B08 points at it |

MWCC lays pooled .bss out in the order that code generation first references
each object:

- Unused inlines do not count.
- Declaration order and frontend-only copies do not count.
- `-inline deferred` reverses the whole function order.

The TU's own reference order gives str before code, and lbl_804024E8 before
lbl_80402480. An unreferenced static helper at the top of the file,
`msgBssLayout` (the D15 precedent), touches the objects in retail's order.
The linker dead-strips it, so the object's .text is 0x30 larger than the
split range. This is tagged RULE-EXCEPTION.

Two config changes were needed:

- **symbols.txt.** The old dtk boundaries were lbl_80401E48 0x1C8 and
  lbl_80402010 0x408. They are replaced by the real objects: E48 0x68, EB0
  0x400, 22B0 0x100, 23B0 0x68, and 24E8 0x2C. The statics use those names,
  so objdiff pairs them and data reaches 100%.
- **config.yml.** cardesavedata's bit-mask table lbl_80478948
  (`80 40 20 10 ...`) read as a pointer to 0x80402010, which is now inside a
  static. A block_relocations target drops that false pointer.

## The .sdata2 pool

Retail's pool is: two GXColor whites, 1.0f, the signed bias, 1.0, 0.5, the
unsigned bias, 2.0f, 0.5f, 0.4f, 8.0f, pi, 25.0f, 4.0f, 1/512.

Retail creates the first 1.0f in fn_800F96E4, and that function is linked
separately and names lbl_8047CD08. So the first three entries are named here,
tagged RULE-EXCEPTION:

- `static const GXColor lbl_8047CD00` and `lbl_8047CD04`, used by fn_800FC7E0
  and GSmsgSetColor;
- `const f32 lbl_8047CD08[1]`, read through `GS_MSG_ONE`. The alternatives
  both fail:
  - a scalar const gets folded into a second, anonymous 1.0f literal;
  - a non-const variable loses GSmsgInit's hoisted `lfs f31`.

The other entries are literals. In msgSetFontInfo, `0.5 * x + 1.0` creates
1.0 before 0.5, which is retail's order.

## Clean-up left

- Linking fn_800F96E4, fn_800F9AEC and fn_800F9C04 into the object needs the
  two switch-leaf walls fixed. That would make the three named pool entries
  ordinary literals and could remove the layout helper, if those functions
  reference the buffers in retail's order.
- config/GC6E01/object_map.freeze.json still names the removed units. That
  freeze was already out of date and is on the user's list.

## Scripts (scratchpad/d23)

- `md.py`: whole-TU function diff (D14's msgdiff for this worktree).
- `gc.sh` / `gc2.sh`: compile one FAEF8 variant with a chosen compiler
  version.
- `gv*.py`: variant runners.
- `secchk.py`: section check.
- `addprop.s` / `pred.s`: the add-propagation disassembly.
