# msgctrl callbacks for the recomp (lane D29, 2026-09-30)

Worktree `/private/tmp/pkmn-decomp-d12`, branch `claude/decomp-d29-msgctrl`.
This lane follows `carves2_d22.md`. GSmsgDispatchControl's native registry
refuses any msgctrlcode callback the decomp has not linked. This lane links
the ones fn_80005E00's message daemons need.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| db1dc43b | game/msgctrl_candidate_80131FF4.c, now 0x80131FF4-0x80132834 (31 functions, msgctrlTsuikaMons to msgctrlCR) | Standalone carve with no pragmas, built with the unit-wide `-opt nopeephole` used by the other msgctrl carves. The four *Mons getters share one static inline (`msgctrlOutPokemonName`) that each of them expands. Split change: msgctrlCR's int-to-float bias (0x8047D0E0) is the TU's only .sdata2 literal, so the unit owns .sdata2 0x8047D0E0-0x8047D0E8. sdata2_8047D098.c now ends at 0x8047D0E0, and the "movie" string and the movie constants move to the new sdata2_8047D0E8.c. The message object struct moves to `include/game/msgctrl_obj.h`. Clean |

Every callback on the brief is linked in that unit, and no rule exception was
needed:

- msgctrlCR, which was 81%;
- msgctrlWait, which was 99.8%;
- msgctrlPalette, which was 72%;
- msgctrlColor and msgctrlFont;
- msgctrlRubyEnd, msgctrlRubyTop and msgctrlRubyStart;
- msgctrlKeyWait and msgctrlKeyEnd;
- msgctrlMenuMsg and msgctrlMenuMsg2;
- msgctrlMenuDigit and msgctrlMenuDigit2.

The msgctrlcode table (.data in msgctrl_exact_80132A38.c, which is linked)
references each callback, so none of them needed `force_active_symbols`.

## What fixed the three non-exact callbacks

- **msgctrlCR**:
  `obj->field_10 += obj->field_64 * (obj->field_23 + obj->field_42);`
  with field_23 as u8 and field_42 as s8 (D22's type fix). The old form had
  explicit casts and a float temporary. The remaining 0.24% came only from
  the literal relocation, and owning the .sdata2 literal fixed it.
- **msgctrlWait**: `if (--obj->waitCounter <= 0)` directly, with no counter
  temporary and no gotos.
- **msgctrlPalette**: index a `{u8 r, g, b, a}` struct array
  (`lbl_80478E8C[idx].r << 24 | ...`) and return s32 0. The old form used a
  byte pointer with `idx * 4` and returned void.

## Digit helper: exact and linked (2026-09-30)

`_msgctrlMakeDigit__FPUslUll` at `0x80132834` now matches all `0x204`
bytes (516 bytes) in both direct objdiff and the canonical report. Its owner,
`src/game/msgctrl_candidate_80132834.c`, is now a standalone source unit
declared `Matching` in configure.py; the historical filename is retained.
The full linked DOL and both RELs pass their retail SHA checks. Remaining
whole-TU candidates include this source to preserve their compilation context.

The decisive changes were restoring peephole-off/scheduling-on settings,
retaining the signed view of the stack-backed value
parameter, ordering locals to match retail allocation, selecting the glyph
table with a switch, and expressing digit extraction with ordinary unsigned
`%` and `/`. The padding loop is compiler-unrolled; no hand-unrolled source or
assembly is needed.

The source-level optimizer pragmas from the initial return have been removed.
The standalone owner uses `-opt nopeephole -schedule on` in configure.py;
only the C++ linkage pragma and its push/pop scope remain. Its sole function
still matches all 516 bytes, and all 2,077 units retain their prior report
measures. This follows the neighboring msgctrl units' translation-unit
nopeephole setting without carrying compiler-control pragmas into the helper.

The recomp return checker audits four objects: `msgctrl_candidate_80131714`,
`msgctrl_r49_80131A34_suffix`, `msgctrl_candidate_80131FF4`, and
`msgctrl_candidate_80132834`. After generating configuration, explicitly clean
and recompile those object targets before regenerating the report: Ninja does
not otherwise rebuild unchanged siblings just because configure.py,
objdiff.json, or compile_commands.json is newer. The checker requires those
inputs to precede every audited object, and the report to follow all of them.

The effective interface is `u16* (u16* buffer, s32 capacity, u32 value,
s32 mode)`. The helper writes backward from `capacity - 1`, leaves the preceding
buffer untouched, and returns a pointer to the first emitted `u16`. Its digit
loop is pre-tested: zero skips extraction and is supplied by the padding loop.
Signed modes compute the magnitude with unsigned subtraction, including for
`0x80000000`. Modes 0/1 are signed/unsigned decimal, 2 pads decimal to ten
digits, 3 pads uppercase hexadecimal to eight, 4 inserts thousands separators,
and 5 uses signed full-width digits with an ordinary minus sign. There is no
capacity clipping.

Every digit callback calls it: codes 47/48 (Digit), 52/53 (MenuDigit), 69-74
(MenuUDigit, MenuHex, MenuZDigit), 75 (Money), 79 (MenuFullDigit) and 80
(MenuMoney). The headed-boot caller `msgctrlMenuDigit` (`0x801322E0`) uses
mode 0 and a capacity of 16 characters; its live input value remains unknown.
This resolves the helper's decomp source/link eligibility blocker. Native
recomp integration and a new headed-boot run have not been performed here.

Validation note: `check_object_map_freeze.py` still fails against its older
2,287-unit/8,603-function baseline; the pre-existing report already had
2,077 units/8,608 functions. This change preserves that topology and all split
addresses, adding one matched/linked function and 516 matched/linked bytes.
The freeze was not regenerated.

## Remaining unlinked helpers

- **msgctrlTime** (38%, msgctrl_r49_8013182C_o2) is code 76.
- The **msgctrlSide\*Name** callbacks are codes 31-33 and 66-68. They sit in
  msgctrl_candidate_80131BF8, which is held back by _msgctrlSideName at 69%.

### Reachability check

The bank-only no-input analysis below is not a runtime reachability proof. A
later headed-boot trace reaches `_msgctrlMakeDigit__FPUslUll` through
`msgctrlMenuDigit` (0x801322E0) during early message handling. Treat the
earlier "not on the no-input title path" conclusion as superseded for this
helper; it is required for the headed boot even though the static message-bank
scan did not identify the triggering digit code.

A callback runs only when a message containing its code is rendered or
measured. On the path from the logos to the title and the attract demo with
no input, three message banks are open:

1. the DOL bank lbl_802CF810 (684 messages), opened by fn_800057B0;
2. the common_rel tableRes 4 bank lbl_125_data_58044 (2018 messages),
   also opened by fn_800057B0;
3. title.fsys's `system_tool` bank (file type 5, 423 messages, keys
   0x30D4-0x44F0), registered by floorReadMsgPostFunc when floor 900 loads.

The fsys archives of the other title-path floors hold no message files:

- nintendo_logo and genius_logo hold one texture each;
- pokemon_logo and auto_demo hold a `dummy`;
- common.fsys holds only common_rel, mail, sound banks and textures.

All three banks were decoded from the user's ISO: FST, then FSYS, then
LZSS. The script is in the D29 scratch dir, `title_msgs.py` and
`codeuse.py`. What they show:

- **Time (76)** appears in one message, DOL 0x1794, which is just `{76}`.
  That is the save file's play-time field.
- **SideName (31-33, 66-68)** appears in no message in any of the three
  banks. These are battle-side names.
- **The MakeDigit codes** appear only in messages behind player input:
  - item counts and bag text (0x1B6C "toss out {47}...");
  - memory-card space (0x1778), which the save flow shows after Start
    (floor 929);
  - weights and heights (0x189C/D);
  - money and coupon text (0x3CB5, 0x3C11);
  - GBA-link prompts (0x3C42);
  - `Lv{52}` and BOX{52} labels;
  - the numeric menu fields in the common_rel bank (0xC9-0xE8).
- **No bank has a "PRESS START" or copyright string.** The idle title
  draws those as textures (title.fsys menu_016/018/042), so it renders no
  GSmsg text at all. The title's own message callbacks only set values
  for the post-Start menus:
  - fn_8002469C/fn_800246CC set code 55 (MenuMsg) to keys 0x3CDF and
    0x3CE4;
  - fn_800246FC does the same through the menu item tables.
  - 0x3CDF and 0x3CE4 are in none of the three banks, so their menus
    load a bank that is not open on this path.

The static scan therefore remains useful for locating encoded control codes,
but not for deciding whether this helper blocks boot. The headed trace is the
authoritative evidence for `_msgctrlMakeDigit__FPUslUll`.
