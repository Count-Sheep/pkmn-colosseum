/**
 * @file field_candidate_801195AC.c
 * @brief Candidate field function, 0x801195AC - 0x80119824.
 *
 * fn_801195AC loads a GPT1 particle file into a free particle bank: it
 * relocates the file's description/data/texture offsets, checks the
 * description version (0x43), flushes the data, takes the first inactive
 * bank and the first texture type no bank uses, clears the bank's 64
 * slots and hands the file to psInitDataBank.
 *
 * Evidence:
 * - Unit flags: the default -O4,p, as its neighbours field_exact_80119824 /
 *   80119BD0 use. Retail's counter-register (bdnz) search loops and the
 *   32-way unrolled slot clear are -O4 loop transforms; the former -O2
 *   override could not produce them.
 * - Inline helpers (fieldParticleGetFreeBank, fieldParticleTextureTypeUsed,
 *   fieldParticleGetFreeTextureType): retail carries inline fingerprints --
 *   the "in use" result is materialised as li r0,1 / li r0,0 and then
 *   re-tested with clrlwi., and the 0xFF "none free" sentinel is set after
 *   the loop and compared straight away. The same logic written as nested
 *   loops without helpers measured 91.25%; with the helpers it is exact.
 *   No XD name is known for these helpers yet.
 * - Version check: retail loads the version into r5 and leaves it there
 *   for GSlogWrite, so the message takes it as its third argument, and
 *   compares it signed (cmpwi), i.e. as an int-typed value.
 */
#define FIELD_BANK_ACTIVE
#define FIELD_CANDIDATE_801195AC_80119824
#include "src/game/field_range_80117E58.c"
