/**
 * @file fight_range_802249B8_to_8022BB84.c
 * @brief Original contiguous fight-sequence unit, including its switch data.
 */
#define FIGHT_RANGE_802249B8_COMBINED

#include "dolphin/types.h"
extern u32 fightOutPokemonGetUseWazaDataId();
extern s32 pokemonGetStatus();
extern void pokemonSetStatus();
extern void wazaSetStatus();
extern u32 fightFloorGetFightOutPokemonPtrRandom();

#define FIGHT_RANGE_802249B8_ONLY
#include "src/game/fight_range_80211A00.c"
#undef FIGHT_RANGE_802249B8_ONLY

#pragma push
#pragma optimize_for_size on
#include "src/game/fight_range_exact_80226134.c"
#pragma pop

#pragma push
#pragma optimize_for_size on
#define FIGHT_RANGE_802274F0_ONLY
#include "src/game/fight_range_80211A00.c"
#undef FIGHT_RANGE_802274F0_ONLY
#pragma pop

#pragma push
#pragma optimize_for_size on
#include "src/game/fight_range_exact_802279F8.c"
#pragma pop

#pragma push
#pragma optimize_for_size on
#define FIGHT_RANGE_80227DB8_ONLY
#include "src/game/fight_range_80211A00.c"
#undef FIGHT_RANGE_80227DB8_ONLY
#pragma pop

#include "src/game/fight_range_exact_8022808C.c"

#pragma push
#pragma optimize_for_size on
#pragma opt_common_subs off
#pragma opt_propagation off
#define FIGHT_RANGE_802282D8_ONLY
#include "src/game/fight_range_80211A00.c"
#undef FIGHT_RANGE_802282D8_ONLY
#pragma pop

#pragma push
#pragma optimize_for_size on
#include "src/game/fight_range_exact_80228DAC.c"
#pragma pop
#include "src/game/fight_range_80229704.c"
#include "src/game/fight_range_exact_80229B70.c"
#pragma push
#pragma optimize_for_size on
#include "src/game/fight_range_80229C28.c"
#pragma pop
#pragma push
#pragma optimize_for_size on
#include "src/game/fight_range_8022A504.c"
#pragma pop
#include "src/game/fight_range_exact_8022B29C.c"
#include "src/game/fight_range_8022B2CC.c"
