# `_threadSwitch` floor-resident wait prerequisite

`fn_801123D4` is a fully linked floor-load function, but its inline
`floorSyncResident` and `floorWaitResident` paths call `_threadSwitch` while
waiting for archive status changes. The current `main/game/gs_thread` object
is incomplete and unlinked. Its canonical report scores `_threadSwitch` at
55.36% (100 retail bytes); the seven-function unit has only one exact
function and includes the register-save/load primitives.

The retail routine is a cooperative context switch, not an ordinary C
function. Guarded objdiff confirms that it saves incoming registers `r3` and
`r5`, captures LR with `mflr r5`, calls `threadSaveGPRRegisters`, saves the
live stack pointer `r1` at context offset `+0x04`, optionally saves FPRs,
then calls `threadLoadGPRRegisters`. Before returning it restores LR via
`mtlr r5`, **loads `r1` from the next context**, then restores `r5` and `r3`
and executes `blr`. The current C candidate instead writes zero placeholders
for the incoming registers, LR, and SP; it is not an executable context
switch. In particular, ordinary MWCC C cannot express replacing its own
stack pointer before return while preserving this ABI.

The authentic hand-written-assembly exception requires a one-function
registry entry, documented non-C instructions, and a cited other
decompilation retaining the routine as assembly. No such evidence entry is
present for `_threadSwitch` in `docs/asm_evidence/registry.json`; this pass
did not add an asm wrapper, touch an `.inc` file, or claim an exception from
the observed instructions alone. Until the required provenance and full
object closure exist, `_threadSwitch` remains an explicit Decomp and native
floor-load blocker. This is separate from `fn_800D36B4`, the fog-color setter
that also remains unlinked at 66.16%.
