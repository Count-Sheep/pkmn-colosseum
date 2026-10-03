# Dolphin OSAlarm.c decrementer handler in Pokémon Colosseum (GC6E01)

`dolphin/os/OSAlarm.c` covers .text 0x8009A27C–0x8009A92C and AlarmQueue
(.sbss 0x8047A6E0): the Dolphin SDK's `OSAlarm.c` (`OSInitAlarm`,
`OSCreateAlarm`, `InsertAlarm`, `OSSetAlarm`, `OSCancelAlarm`,
`DecrementerExceptionCallback`, `DecrementerExceptionHandler`). The bodies are
in `src/dolphin/sdk_range_8009A2D8.c`, which the unit includes with
`OS_ALARM_FILE`. Everything is C except the routine below, which uses the exact
retail mnemonics and matches the retail bytes.

## DecrementerExceptionHandler

Retail address 0x8009A8DC, size 0x50 (local symbol; dtk names it
`DecrementerExceptionHandler_8009A8DC`). `OSInitAlarm` installs it as the
handler for exception 8 (decrementer); `OSExceptionVector` reaches it through
SRR0 and `rfi` with r3 = exception number and r4 = context.

- Why it cannot be C: it is entered from the exception vector with only r3–r5 saved and no valid stack frame. It stores r0–r2 and r6–r31 into the context (`stmw r6, 0x18(r4)`), reads the graphics quantization registers GQR1–GQR7 with `mfspr`, carves an 8-byte frame with `stwu r1, -0x8(r1)` and tail-branches. MWCC never emits GQR `mfspr` from C, and a C body would save registers into its own frame before the context is filled.
- Other decompilations: [zeldaret/tww, commit f5234ec8, `src/dolphin/os/OSAlarm.c`](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSAlarm.c#L180-L208) keeps `DecrementerExceptionHandler` as an `asm` function with the same body, ending in `stwu r1, -8(r1)` and `b DecrementerExceptionCallback` ([L204-L205](https://github.com/zeldaret/tww/blob/f5234ec8f4b8f4119b8db4a018c1ca25572988b1/src/dolphin/os/OSAlarm.c#L204-L205)); [doldecomp/melee, commit e78dc834, `libs/dolphin/src/dolphin/os/OSAlarm.c`](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSAlarm.c#L252-L260) keeps it as `asm` with `OS_EXCEPTION_SAVE_GPRS(context)` and the same final branch ([L258](https://github.com/doldecomp/melee/blob/e78dc8349c587ca5d2f97322834b625d3bd1abc3/libs/dolphin/src/dolphin/os/OSAlarm.c#L258)).
- Origin: Nintendo Dolphin SDK, `os/OSAlarm.c`, hand-written assembly in the vendor source.
- External branch targets: the final `b DecrementerExceptionCallback` (0x8009A928) jumps to DecrementerExceptionCallback at 0x8009A6AC (same unit, a static C function) with r3/r4 holding the exception and context. It is a tail jump, not a call: the routine has no return path and only the 8-byte frame it builds itself, so MWCC's `bl` with a prologue would not reproduce it. The retail relocation at that instruction names DecrementerExceptionCallback.
