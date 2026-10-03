/**
 * @file OSAlarm.c
 * @brief The Dolphin SDK's OSAlarm.c, 0x8009A27C - 0x8009A92C, with its
 *        AlarmQueue (.sbss 0x8047A6E0): OSInitAlarm, OSCreateAlarm,
 *        InsertAlarm, OSSetAlarm, OSCancelAlarm, DecrementerExceptionCallback
 *        and the asm DecrementerExceptionHandler. The bodies live in
 *        sdk_range_8009A2D8.c, shared with the heap units that follow.
 */
#define OS_ALARM_FILE
#include "src/dolphin/sdk_range_8009A2D8.c"
