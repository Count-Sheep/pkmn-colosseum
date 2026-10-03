# Dolphin GX paired-single FIFO copies in Pokémon Colosseum (GC6E01)

Three Dolphin SDK GX routines copy matrices or a light object into the
write-gather FIFO (0xCC008000) with paired-single loads and stores:

- `GXLoadLightObjImm` (0x800BA44C, `gx/GXLight.c`), unit
  `dolphin/sdk_range_800BA44C.c`;
- `GXLoadPosMtxImm` (0x800BD4B4) and `GXLoadNrmMtxImm` (0x800BD504)
  (`gx/GXTransform.c`), unit `dolphin/sdk_candidate_800BD454.c`.

In the vendor source the GX functions are C, and the copies are inline `asm`
blocks in small static helpers that MWCC inlines into each caller; the helpers
have no retail symbol. Only the asm blocks of the helpers named below are
admitted. Each uses the exact retail mnemonics, and the callers match the
retail bytes in objdiff and in the final link.

## PushLight

Inlined into `GXLoadLightObjImm` (0x800BA44C, size 0x7C).

- Why it cannot be C: the routine moves the light's attenuation, position and direction as six 8-byte pairs with `psq_l`/`psq_st` (GQR0, unscaled), together with the colour word and three zero words (`xor` of a register with itself) written to the FIFO. MWCC never emits paired-single loads or stores from C; a C copy produces twelve `lfs`/`stfs` pairs.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/gx/GXLight.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/gx/GXLight.c#L132) keeps `PushLight` as an inline `asm` block with the same `lwz`/`xor`/`psq_l`/`stw`/`psq_st` sequence, inlined into `GXLoadLightObjImm`.
- Origin: Nintendo Dolphin SDK, `gx/GXLight.c`, inline assembly in the vendor source.

## WriteMTXPS4x3

Inlined into `GXLoadPosMtxImm` (0x800BD4B4, size 0x50) and, for 3x4 texture matrices, into `GXLoadTexMtxImm` (0x800BD58C, `src/dolphin/sdk_range_800BB30C.c`).

- Why it cannot be C: it copies a 3x4 position matrix to the FIFO as six paired-single loads and six paired-single stores (`psq_l`/`psq_st`, GQR0). MWCC never emits paired-single instructions from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/gx/GXTransform.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/gx/GXTransform.c#L136) keeps `WriteMTXPS4x3` as an inline `asm` block, and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/gx/GXTransform.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/gx/GXTransform.c#L116) keeps it as an `asm` function.
- Origin: Nintendo Dolphin SDK, `gx/GXTransform.c`, inline assembly in the vendor source.

## WriteMTXPS4x2

Inlined into `GXLoadTexMtxImm` (0x800BD58C, size 0xB4) for 2x4 texture
matrices; the source is `src/dolphin/sdk_range_800BB30C.c`.

- Why it cannot be C: it copies a 2x4 texture matrix to the FIFO as four paired-single loads and four paired-single stores (`psq_l`/`psq_st`, GQR0). MWCC never emits paired-single instructions from C; plain C copies come out as `lfd`/`stfd` pairs.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/gx/GXTransform.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/gx/GXTransform.c#L199-L217) keeps `WriteMTXPS4x2` as an inline `asm` block, called from `GXLoadTexMtxImm` ([L238](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/gx/GXTransform.c#L238)), and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/gx/GXTransform.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/gx/GXTransform.c#L171-L183) keeps it as an `asm` function.
- Origin: Nintendo Dolphin SDK, `gx/GXTransform.c`, inline assembly in the vendor source.

## WriteMTXPS3x3

Inlined into `GXLoadNrmMtxImm` (0x800BD504, size 0x50). It reads the 3x3 normal
matrix out of a 3x4 matrix (melee calls it `WriteMTXPS3x3from3x4`).

- Why it cannot be C: each row goes out as one paired-single pair (`psq_l`/`psq_st`) and one single (`lfs`/`stfs`), skipping the fourth column. MWCC never emits paired-single instructions from C.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/gx/GXTransform.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/gx/GXTransform.c#L165) keeps `WriteMTXPS3x3` as an inline `asm` block, and [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/gx/GXTransform.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/gx/GXTransform.c#L135) keeps it as the `asm` function `WriteMTXPS3x3from3x4`.
- Origin: Nintendo Dolphin SDK, `gx/GXTransform.c`, inline assembly in the vendor source.
