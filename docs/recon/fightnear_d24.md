# Fight near-links, lane D24 (2026-09-30)

Scope: the fight_* units in build/GC6E01/report.json whose only inexact
functions are at 98%+ (the list in the D24 brief, biggest first). Skipped as
instructed: fight_floor_candidate_801F54A4, fn_80238B0C, and the fight_timer
80265A6C/80265C84 pair. Every linked unit passed configure, report, full
ninja (main.dol and common_rel.rel SHA-1 OK), quality_scan and
check_regression.

## Linked

| Commit | Unit | What it took |
|---|---|---|
| f392cc8e | fight_range_80229704 (fn_80229704 99.61, fn_80229934 98.29) | One shared inline, `fightCountJoutai(target, statusId)`: it counts the valid, fought-in Pokemon on the target's side that can take the status. fn_80229704 expands it twice and fn_80229934 once, and fn_802249B8 expands the same sequence in cases 7/8. Locals declared pokemon, side, trainerCount, found, trainer, trainerIndex, pokemonIndex. GC/1.3 |
| 8f2b6abf | fight_range_80229C28 (WS_HITCHECK 99.70, with fn_80229C28 and fn_80229C90) | GC/1.3.2 -O4,s. Two inline blocks, fightSeqCheckTokuseiBlock (Soundproof: defender ability 0x2b against a sound move, replacing a one-member struct) and fightSeqCopyWaza. The PP read passes the move slot as its fourth argument. On the miss path the flag is set before the PC is loaded. Both inlines are single-use, so each has a RULE-EXCEPTION row. fn_80229C90 keeps its exact one-member struct form (tagged); a natural form reaches 99.78 |
| 0db37aa3 | fight_range_80211A00_suffix_80216A58 (fn_80216A58 99.11 + three exact) | The candidate dropped heroGetStatus's index argument: it is `heroGetStatus(party, 3, index)`, which is why retail loads the loop test into r5. The damage division is two statements. The locals follow the declaration order found by the simulator. GC/1.3 -O4,s, no pragmas; fn_80216D9C's `register` locals became a plain `* 15 / 10` |

## Lessons

- **An odd volatile register usually means a missing call argument.** The
  Ghidra-derived candidates drop arguments of unprototyped calls. A temp in
  r5/r6 where r0 was expected is the value being passed. This was the case
  for the r5 loop test in fn_80216A58 and the extsb r6 slot in WS_HITCHECK.
- **Declaration order can be solved in the simulator, not by compiling.**
  Take an mwcc-debugger dump, then run `vmap.py` to get retail's register for
  each vreg. `simclimb.py` then searches vreg positions (declaration order)
  with D16's wi5 `run()` and prints the order. It found fn_80216A58's exact
  order in seconds, where a compile-based climb stalled at 99.70.
- **When the simulator cannot reach retail, the cause is structural.** A
  miss that no declaration order fixes points at an inline, a missing
  argument or a merged variable, not at order.
- **The struct hack marks an inline temp.** The candidates' one-member struct
  locals imitate an inline's return temp; look for the inline (WS_HITCHECK's
  Soundproof block).
- **Statement order changes the colour order of scheduler temps.** For
  flag-clear blocks, `lbl_80478D78[5] = 1;` before `lbl_8047B618 &= ~0x2000;`
  gives retail's r0/r4. For flag-and-PC pairs, the flag goes first.
- **Unprototyped u16 arguments create CSE temps.** fn_802249B8's `count`
  argument became a CSE'd `(u16)` temp; `fn_801F0134(u32, u16)` removes it.

Tools (scratchpad/d24, copy them):
- cmp.py: one-object compile plus objdiff.
- ext.py: extracts a function from fight_range_80211A00.c with its needed
  declarations.
- var.py: tries source variants.
- climb.py, mvdecl.py, perm.py: compile-based declaration climbs.
- vmap.py: D16's vreg-to-retail mapping, pointed at this worktree.
- simclimb.py: simulated declaration-order search.
- applyorder.py: rewrites a declaration block into a given order.

## Walls

### fn_802249B8 (fight_range_802249B8, 98.41% -> 98.52%)

Best source is scratchpad/d24/best_802249B8.c (GC/1.3.2 -O4,s, standalone).
What it already has:
- plain source with no pragmas or volatiles;
- fightCheckJoutaiRule (fn_80229704's body as an inline) for cases 7/8;
- `extern u16 fn_80203B5C(u32, u16)`;
- `lvl += (u16)rand() % 3 - 1`;
- flag-clear blocks with the [5] store first;
- `fn_801F0134(u32, u16)`;
- the simulator's declaration order.

What is left:
- trainer (retail r28) and slotB (retail r18) are swapped. The simulator
  says no declaration order and no single extra interference fixes it, so
  the cause is structural.
- At five sites, `fn_802025B8(ctx, lbl_80279EF4[lbl_80478D78[3]])` places
  `mr r3,r29` before the `slwi` (retail places it after). Tried: a tbl
  variable, pointer and u32-index forms, prototypes, inline wrappers, a
  const table. None changed it.
- The four fn_8021B910 calls in cases 15-52 place `clrlwi r4` (the u8 slot)
  before `li r3`/`li r6` (retail places it last). Tried: prototypes
  (u32,u8,u8,u32), (u32,u8,u32,u32), (u8 x4), (u32 x4).
- One `mr r7, r15` placement (case 31).

Linking also needs the switch table at .data 0x8039A220, which D20's
fight_range_8022B2CC now owns as a data initializer.

### fn_8022F2F8 (fight_range_8022F2F8, 99.56% -> 99.43% size-equal variant)

- Retail keeps `target` as a copy of ctx (`mr r24, r31`, right after the
  0x14 floor-status call and before its clrlwi). Case 12 writes only
  selectedTarget (r20), never target, but the copy is not propagated.
- In ours, either the case-12 `target = selectedTarget` survives (extra
  `mr r24, r20`), or, without it, target is copy-propagated away.
- The mr/clrlwi order never matched. Tried:
  - moving `target = ctx` to every position;
  - `floorStatus` as u16 with and without the mask;
  - a u32 floorStatus with casts;
  - fightFloorGetStatus prototyped u16/u32 (it is implicitly declared int
    in the candidate);
  - dropping the flagBase pointer, which strength-reduces the address to
    +5, unlike retail.
- Linking also needs the jump table .data 0x8039A538-0x8039A5D8, currently
  data_8039A538.c.

### fn_802230BC / fn_802232F4 (fight_range_802230BC, 98.27 / 98.47)

- fn_802230BC: `entry` becomes a compiler temp (@34), coloured before
  selected/replacement, so it takes r26 where retail uses r24.
  - `entry = (s16)fn_8012640C(...)` in one statement makes entry a named
    local but adds an `mr` (572 bytes).
  - s16/u16/u32/int entry types, an inline for the repeated party-pokemon
    lookup, and a climb over declarations did not fix it.
  - In the simulator, even with the temp's position free, selected and
    entry still swap.
- fn_802232F4 was not attempted.

### fn_8021D40C / fn_8021D688 (fight_range_8021D40C, 98.65 / 98.83)

- fn_8021D40C case 1 is the Follow Me redirect. D20's fightGetFollowMeTarget
  inline makes the size exact (98.18). Retail keeps a join
  (`mr r28,r31; mr r7,r28`) that the inline form folds away.
- The in-place form with its own floor local gets 98.33. Permuting the
  inline's locals gets 98.43.
- Case 6 has a pokemon/outPokemon swap.
- fn_8021D688 was not attempted.
- The jump table 0x8039A088 sits in data_8039A088.c together with
  fn_8021FAD4's (0x8039A0A8) and fn_80220868's (0x8039A0F0, 0x8039A110)
  tables.

### fn_8021FAD4 (fight_range_8021FAD4, 98.82)

- All 18 callee-saved registers are in use, and retail spills two
  variables:
  - floorStatus as a u16 (`sth`/`lhz` at sp+8);
  - stopState as a u32 (`stw`/`lwz` + clrlwi at sp+0xc).
- The candidate fakes the spills with `u16 floorStatus[1]` and a volatile,
  which puts the slots in the wrong order.
- Plain locals make MWCC spill different webs (98.21). This needs retail's
  exact interference; not pursued.

### fn_8021B910 (fight_range_8021B910, 98.24 standalone)

- The simulator reaches only 6 of 23 retail registers with any declaration
  order.
- Structural: an `andi. r0, r28, 0xbf` path and swapped parameter homes.
  Not pursued.

## Not attempted

- fight_range_8023B498: it needs .rodata/.sdata2 ownership.
- fn_80213E94, fn_8023A308, fn_80220868 (fn_80220868's two tables are in
  data_8039A088.c).
- fightFloor/fightSide/fightPokemon candidates.
