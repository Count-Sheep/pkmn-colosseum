/**
 * @file reverb_candidate_80164520.c
 * @brief MusyX StdReverb/reverb.c ReverbHICreate and ReverbHIModify,
 *        0x80164520 - 0x80164C40.
 *
 * The reference MusyX runtime's reverb.c (AxioDL/musyx) up to the first
 * handwritten-assembly function. DLsetdelay, DLcreate and DLdelete are the
 * reference's static helpers, inlined as in retail. Both functions are
 * exact with -fp_contract off (retail keeps the damping multiply and add
 * separate); with contraction on they are 98.5% / 98.2%.
 *
 * This stays a candidate. The rest of reverb.c is DoCrossTalk and
 * HandleReverb, which are handwritten `asm` in the reference and in retail
 * (stmw r14 prologues, lis/@l addressing of the sdata2 constants), and
 * ReverbHICallback. The TU's literal pool (.sdata2 0x8047D4F0 - 0x8047D548)
 * is shared: ReverbHICallback reads 0.0f/1.0f at 0x8047D4F0/0x8047D4F4
 * from this file's part of it. A linked object would give those entries
 * local pool labels, so the whole TU cannot be linked without the asm.
 *
 * Retail pool of reverb.c (.sdata2), in order:
 *   0x8047D4F0 0.0f   <- also read by ReverbHICallback
 *   0x8047D4F4 1.0f   <- also read by ReverbHICallback
 *   0x8047D4F8 0.01f, 0x8047D4FC 10.0f, 0x8047D500 0.1f, 0x8047D504 32000.0f,
 *   0x8047D508 10.0 (double), 0x8047D510 0.05f, 0x8047D514 0.8f,
 *   0x8047D518 int->float bias, 0x8047D520 100.0f          (this file)
 *   0x8047D528 i2fMagic, 0x8047D530 value0_6, 0x8047D534 value0_3
 *                        (the asm's `static const`s, reverse declaration order)
 *   0x8047D538 0.5f      (ReverbHICallback's own literal)
 * The shared entries are the first two literals this file emits, so no
 * split of the pool separates them from this file's code. Carving them into
 * a data unit and reading them through `extern const f32` gives
 * ReverbHICreate 99.64% / ReverbHIModify 99.36%: the literal 0.0f/1.0f
 * live in f7/f1 in retail but f1/f2 as extern loads, and `preDelay != 0.f`
 * swaps its fcmpu operands. Plain `extern f32` is further off (the 0.0f
 * load held in f30 across the memset/alloc calls can no longer be hoisted).
 * A defined `const f32` at file scope is folded into a second pool literal.
 *
 * The pool belongs to one object: C and asm are not two objects (checked
 * 2026-09-28, lane R22). Other MusyX games split reverb.c as a single
 * object with one .sdata2 range holding the same literals in the same order:
 *   - doldecomp/ttyd 62131fc3 (config/G8MJ01): reverb.c .text
 *     0x8028CDFC-0x8028DA78, .sdata2 0x80422D58-0x80422DA0 = 0.0f, 1.0f,
 *     0.01f, 10.0f, 0.1f, 32000.0f, 10.0, 0.05f, 0.8f, int bias, i2fMagic,
 *     0.3f, 0.6f, 0.5f. It is Colosseum's pool without ReverbHIModify's
 *     100.0f (Modify is dead-stripped there). Symbols: ReverbHICreate,
 *     DoCrossTalk, U_HandleReverb, ReverbHICallback, in address order.
 *   - mariopartyrd/marioparty4 147b165a (config/GMPE01_00): reverb.c .text
 *     0x80113054-0x80113D98, .sdata2 0x801D6B68-0x801D6BB0; i2fMagic,
 *     value0_3, value0_6 are `scope:local` symbols inside that range,
 *     followed by the callback's 0.5f.
 *   - PrimeDecomp/prime 55fd4dbd (config/GM8E01_00): reverb.c .text
 *     0x803B5BC0-0x803B6904, .sdata2 0x805AF3F0-0x805AF438.
 * All three link reverb.c as one Matching object with DoCrossTalk and
 * HandleReverb as `asm` functions in the file (AxioDL/musyx layout).
 * Colosseum's object is .text 0x80164520-0x801653BC (Create, Modify,
 * DoCrossTalk, HandleReverb, Callback; ReverbHIFree is dead-stripped),
 * .data 0x8036BF00-0x8036BF20 and .sdata2 0x8047D4F0-0x8047D540.
 *
 * Why no in-policy link exists: ReverbHICallback's `rev->rv.crosstalk != 0.f`
 * and `1.f - ...` load 0x8047D4F0/0x8047D4F4, the pool entries MWCC made for
 * ReverbHICreate. Only a single translation unit shares compiler literals,
 * so the callback is in this object. The two asm functions sit between
 * ReverbHIModify and the callback in .text, so no split of the object puts
 * the C functions in a unit without them. DoCrossTalk and HandleReverb are
 * hand-written: 23 paired-single instructions in DoCrossTalk, `stmw r14` in
 * HandleReverb although the unit is built -use_lmw_stmw off (Create and the
 * callback call _savegpr), and lis/@l addressing of .sdata2 constants in
 * both. C cannot emit them, and the asm bodies are outside the quality
 * allowlist. The current carve (callback linked from
 * game/musyx_range_801652DC.c through extern names) works only because this
 * unit is not linked and its dtk object exports lbl_8047D4F0/F4.
 */
#include "dolphin/types.h"
typedef struct _SND_REVHI_DELAYLINE {
    s32 inPoint;
    s32 outPoint;
    s32 length;
    f32* inputs;
    f32 lastOutput;
} _SND_REVHI_DELAYLINE;

typedef struct _SND_REVHI_WORK {
    _SND_REVHI_DELAYLINE AP[9];
    _SND_REVHI_DELAYLINE C[9];
    f32 allPassCoeff;
    f32 combCoef[9];
    f32 lpLastout[3];
    f32 level;
    f32 damping;
    s32 preDelayTime;
    f32 crosstalk;
    f32* preDelayLine[3];
    f32* preDelayPtr[3];
} _SND_REVHI_WORK;

typedef struct SND_AUX_REVERBHI {
    _SND_REVHI_WORK rv;
    u8 tempDisableFX;
    f32 coloration;
    f32 mix;
    f32 time;
    f32 damping;
    f32 preDelay;
    f32 crosstalk;
} SND_AUX_REVERBHI;

#include "crt/math_ppc.h"

extern void* memset(void* dst, int c, u32 n);
extern void* fn_801643D8(u32 len);
extern void fn_80164400(void* addr);

static void DLsetdelay(_SND_REVHI_DELAYLINE* delayline, s32 len) {
  delayline->outPoint = delayline->inPoint - (len * sizeof(f32));
  while (delayline->outPoint < 0) {
    delayline->outPoint += delayline->length;
  }
}

static void DLcreate(_SND_REVHI_DELAYLINE* delayline, s32 length) {
  delayline->length = (s32)length * sizeof(f32);
  delayline->inputs = (f32*)fn_801643D8(length * sizeof(f32));
  memset(delayline->inputs, 0, length * sizeof(length));
  delayline->lastOutput = 0.f;
  DLsetdelay(delayline, length >> 1);
  delayline->inPoint = 0;
  delayline->outPoint = 0;
}

static void DLdelete(_SND_REVHI_DELAYLINE* delayline) { fn_80164400(delayline->inputs); }
u32 ReverbHICreate(_SND_REVHI_WORK* rev, f32 coloration, f32 time, f32 mix, f32 damping,
                    f32 preDelay, f32 crosstalk) {
  static int lens[] = {1789, 1999, 2333, 433, 149, 47, 73, 67};
  unsigned char i; // r31
  unsigned char k; // r29
  if (coloration < 0.f || coloration > 1.f || time < 0.01f || time > 10.f || mix < 0.f ||
      mix > 1.f || crosstalk < 0.f || crosstalk > 1.f || damping < 0.f || damping > 1.f ||
      preDelay < 0.f || preDelay > 0.1f) {
    return FALSE;
  }

  memset(rev, 0, sizeof(_SND_REVHI_WORK));

  for (k = 0; k < 3; ++k) {
    for (i = 0; i < 3; ++i) {
      DLcreate(&rev->C[i + k * 3], lens[i] + 2);
      DLsetdelay(&rev->C[i + k * 3], lens[i]);
      rev->combCoef[i + k * 3] = powf(10.f, (lens[i] * -3) / (32000.f * time));
    }

    for (i = 0; i < 2; ++i) {
      DLcreate(&rev->AP[i + k * 3], lens[i + 3] + 2);
      DLsetdelay(&rev->AP[i + k * 3], lens[i + 3]);
    }
    DLcreate(&rev->AP[k * 3 + 2], lens[k + 5] + 2);
    DLsetdelay(&rev->AP[k * 3 + 2], lens[k + 5]);
    rev->lpLastout[k] = 0.f;
  }

  rev->allPassCoeff = coloration;
  rev->level = mix;
  rev->crosstalk = crosstalk;
  rev->damping = damping;
  if (rev->damping < 0.05f) {
    rev->damping = 0.05f;
  }

  rev->damping = 1.f - (rev->damping * 0.8f + 0.05f);
  if (preDelay != 0.f) {
    rev->preDelayTime = preDelay * 32000.f;
    for (i = 0; i < 3; ++i) {
      rev->preDelayLine[i] = (f32*)fn_801643D8(rev->preDelayTime * sizeof(f32));
      memset(rev->preDelayLine[i], 0, rev->preDelayTime * sizeof(f32));
      rev->preDelayPtr[i] = rev->preDelayLine[i];
    }
  } else {
    rev->preDelayTime = 0;
    for (i = 0; i < 3; ++i) {
      rev->preDelayPtr[i] = NULL;
      rev->preDelayLine[i] = NULL;
    }
  }

  return TRUE;
}

u32 ReverbHIModify(struct _SND_REVHI_WORK* rv, float coloration, float time, float mix,
                    float damping, float preDelay, float crosstalk) {
  u8 i; // r30

  if (coloration < 0.f || coloration > 1.f || time < 0.01f || time > 10.f || mix < 0.f ||
      mix > 1.f || crosstalk < 0.f || crosstalk > 1.f || damping < 0.f || damping > 1.f ||
      preDelay < 0.f || preDelay > 100.f) {
    return FALSE;
  }

  rv->allPassCoeff = coloration;
  rv->level = mix;
  rv->crosstalk = crosstalk;
  rv->damping = damping;
  if (rv->damping < 0.05f) {
    rv->damping = 0.05f;
  }

  rv->damping = 1.f - (rv->damping * 0.8f + 0.05f);

  for (i = 0; i < 9; ++i) {
    DLdelete(&rv->AP[i]);
  }

  for (i = 0; i < 9; ++i) {
    DLdelete(&rv->C[i]);
  }

  if (rv->preDelayTime != 0) {
    for (i = 0; i < 3; ++i) {
      fn_80164400(rv->preDelayLine[i]);
    }
  }

  return ReverbHICreate(rv, coloration, time, mix, damping, preDelay, crosstalk);
}
