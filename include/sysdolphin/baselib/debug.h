/**
 * @file debug.h
 * @brief HAL sysdolphin debug.h: __assert/HSD_Panic and the assert macros.
 *
 * HSD_ASSERT (from hsd/hsd_debug.h) is an expression, as in HAL's header:
 * the file string comes from __FILE__ (MWCC expands it to the including
 * file's basename), the line is HAL's literal line number.
 */
#ifndef SYSDOLPHIN_BASELIB_DEBUG_H
#define SYSDOLPHIN_BASELIB_DEBUG_H

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"

void HSD_Panic(const char* file, u32 line, const char* msg);
void OSReport(const char* fmt, ...);

/* Reports the message, then fails the assertion (Melee's debug.h). */
#define HSD_ASSERTREPORT(line, cond, ...)                                     \
    ((cond) ? (void) 0                                                        \
            : (OSReport(__VA_ARGS__), __assert(__FILE__, line, #cond)))

#endif /* SYSDOLPHIN_BASELIB_DEBUG_H */
