# fn_800194E4 matching evidence

`fn_800194E4` is the 252-byte party-menu slot-position callback at
`0x800194E4`, built in `main/game/menuPokemon_candidate_800194E4` with
GC/2.0, level 4, and no peephole optimization.

The owner uses `MENU_POKEMON_800194E4_ONLY`; the old candidate wrapper lacked
an `_ONLY` define and therefore compiled the complete shared source. Adding the
carve is required for the exact callback to link without duplicating its
already linked menuPokemon siblings.

The previous source was 99.68254%. Its opcodes, control flow, and relocations
already matched; four operands differed because MWCC placed the table byte
offset in r5 and the vertical offset in r6, opposite retail.

The accepted source consumes the dead post-loop index as its byte offset:

```c
i *= sizeof(MenuPokemonSlotPos);
offset = (4 - pos) * 0x1f;
slot = (MenuPokemonSlotPos*)(lbl_802E4E10 + i);
```

With scoped `#pragma opt_propagation off`, GC/2.0 retains the byte offset in
r6 and the vertical offset in r5. This emits all 252 retail text bytes and the
four `lbl_802E4E10` relocations exactly. The pragma is reset immediately after
the function. The source changes no calls, branches, memory accesses, or
observable values; `i` is not read after the callback forms `slot`.

An isolated compile-and-objdiff matrix also tested the original array index,
pointer-first ordering, assignment expressions, direct field accesses, a
separate byte-offset local, split offset arithmetic, and scheduling modes.
Only the explicit byte-offset forms with propagation disabled reached 100%.
The original source remains 99.68254%; scheduling changes regress materially.

Final validation after marking the unit `Matching` confirmed one exact source
symbol, 252 `.text` bytes, no owned data, and four matching relocations. The
full project link succeeds, `dtk shasum -q -c config/GC6E01/build.sha1`
reports all 26 files OK, the report marks the unit complete, and the live-asm
wrapper and changed-source quality scans pass.
