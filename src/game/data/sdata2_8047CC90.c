#include "dolphin/types.h"

#pragma section ".sdata2"
#define SDATA2 __declspec(section ".sdata2")

/*
 * aobj.h assert string between the GSmodel animation literal pool
 * (0x8047CC80-0x8047CC90) and the 0x8047CC98 slice.
 */
SDATA2 const u8 lbl_8047CC90[7] = "aobj.h";
