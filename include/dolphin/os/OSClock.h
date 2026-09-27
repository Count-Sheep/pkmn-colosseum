#ifndef DOLPHIN_OS_OSCLOCK_H
#define DOLPHIN_OS_OSCLOCK_H

#include "dolphin/types.h"

/*
 * Dolphin SDK os.h bus clock and tick conversions. The SDK reads the bus
 * clock through the address-bound variable __OSBusClock (low memory
 * 0x800000F8), not a pointer cast, and MWCC optimises the two differently:
 * GBAInit only matches with the variable, whose load retail hoists out of
 * the channel loop.
 *
 * dolphin/si/SI.h still carries older pointer-cast copies of these macros
 * that its users depend on, so the two headers are not included together.
 */
extern u32 __OSBusClock : 0x800000F8;

#define OS_BUS_CLOCK __OSBusClock
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)

#endif /* DOLPHIN_OS_OSCLOCK_H */
