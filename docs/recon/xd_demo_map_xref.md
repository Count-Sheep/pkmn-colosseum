# XD demo linker map cross-reference (2026-09-28, lane BX)

This recon follows up [cross_build_evidence.md](cross_build_evidence.md) (lane
B43). That lane found that the Pokémon XD JP demo's own linker map is public
(`NXXJ01.map`, the disc's `Master.MAP`). This lane built a tool that lines the
map up with Colosseum and asked one question of it: do the stripped functions
that left orphan data in Colosseum have a live body in any XD build?

Base: `de08b227`. Commits on this branch:

| Commit | What |
|---|---|
| `0f0290d2` | `tools/xd_map_xref.py`, the cross-reference tool |
| `d3f4976e` | 11 SDK renames in `config/GC6E01/symbols.txt` (and the non-wall sources that used the old names) |
| this file | the findings |

No wall source file was edited. No split was changed.

## Sources

| Source | Revision | Used for |
|---|---|---|
| [StarsMmd/Colo-XD-PBR-symbol-maps](https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps) `NXXJ01.map` | `6b51d3af` | **Primary.** Real `mwldeppc` output: objects, live and `UNUSED` (dead-stripped) symbols with sizes, in object order |
| [TeamOrre/xd-decomp](https://github.com/TeamOrre/xd-decomp) `config/GXXE01/symbols.txt` | `4989794e` | XD retail (US) names and addresses, including dtk's `fn_` functions |
| [trevor403/xd-asm](https://github.com/trevor403/xd-asm) `code/func_*.s` | `b1087f18` | XD retail bodies (Ghidra) |
| Colosseum `build/GC6E01/asm`, `orig/GC6E01/sys/main.dol` | this tree | Colosseum bodies and bytes |
| `build/reference/melee/src` (doldecomp/melee) | local copy | HSD cross-check |

The other maps in the StarsMmd repository are derived from `NXXJ01.map` and
were not used as evidence.

## The tool: `tools/xd_map_xref.py`

The three external repositories are cloned into a lane-private `.lane/`
(`maps`, `xd-decomp`, `xd-asm`). Their paths can be overridden with `--map`,
`--retail-symbols` and `--xd-asm`. Colosseum bodies come from
`build/GC6E01/asm`, so run `ninja` once first.

| Command | Output |
|---|---|
| `objects` | one line per XD object: live and stripped counts, the Colosseum range, the overlapping split units |
| `object NAME` | one object in detail: aligned `.text` rows, then every data section's stripped symbols (`--all-data` lists the live ones too) |
| `tu UNIT` / `addr 0x...` | the objects that overlap a split unit or an address |
| `retail-live` | for each object, XD retail functions (or bytes not covered by any function) between two live demo functions that the demo does not name. These are candidates for a demo-`UNUSED` function that retail keeps |
| `renames` | pairs where the XD function is live, the Colosseum one is `fn_`, they sit in the same object in the same order, the sizes are equal and the retail body matches (`--min-sim`, default 0.9) |
| `json FILE` | everything, machine-readable |

How the alignment works:

1. **Name anchors.** An XD function anchors when its C++ base name
   (`_peopleWillMove__FP13tagPeopleWork` → `_peopleWillMove`) matches a
   unique, non-automatic Colosseum name. Methods (`__Q…`, `__<n>class`) are
   skipped.
2. **Body anchors** (unless `--no-bodies`). Within a window around the name
   anchors, a live XD function whose retail body clearly best matches a
   Colosseum `fn_` becomes an anchor. The threshold is similarity ≥ 0.8
   (`--anchor-sim`), with a lead of 0.05 over the runner-up.
3. **Chain and cluster.** The tool keeps the longest chain of anchors that
   runs in one direction, in either direction, because MWCC's deferred
   inlining emits a TU in reverse. It then keeps the densest cluster, because
   static names such as `OnReset` collide across objects and some Colosseum
   names come from the noisy automatic port.
4. **Filling the gaps.** Between anchors, a small dynamic-programming pass
   pairs the rest by size and body. The head and tail of each object are
   aligned against the neighbouring Colosseum functions, up to the next
   object's anchor.

Retail addresses of the demo's live functions are resolved per object. The
median demo→retail offset comes from the object's unique names, so repeated
statics such as `_swap32` resolve to the right copy.

Limits:

- A row marked `size` pairs an XD `UNUSED` entry, or an unanchored function,
  with a Colosseum function of similar size. That is a lead only.
- XD rewrote some modules in C++ (people, heroMove, the VM, the movie player),
  so bodies there match poorly and anchors are sparse.
- `renames` output is a candidate list. Every pair still needs a check by hand.

## Key question: stripped functions with live XD bodies

**Answer: no, for all four walls.** For each object, `retail-live` compares the
demo's list with XD retail's full function list: xd-asm's Ghidra functions,
plus dtk's `fn_` functions from GXXE01 `symbols.txt`, plus any bytes that no
function covers. In the four wall objects:

- `camera.o`, `jobj.o` and `GSspline.o` have **no** retail-only function and
  no uncovered bytes;
- `people.o` has one, which is not a candidate owner (see People below).

Every candidate owner is `UNUSED` in the demo and absent from retail. So no
XD body exists to test, and the scratch build that would add a real XD
function to a Colosseum TU could not be run. The mechanism itself (uncalled
functions stripped object by object, `.rodata` kept whole when a live function
addresses it from the section base) was already shown by B43 in
`.lane/striptest`.

Over the whole map, retail keeps functions that the demo strips in only a
handful of objects, none of them near a wall:

- `pokecolo.o` (three small functions)
- `menuBingo.o`, `menuTitle.o`
- `GSmsg.o` (`fn_8010B208`)
- `memcard.o` (`fn_801CC508`)
- the runtime's `_savefpr`/`_restgpr` entry points
- `people.o` (below)

### People: the orphan 12-byte zero image at 0x80273FCC

| | |
|---|---|
| Candidate owners (demo `UNUSED`) | `peopleSetFootParticle` 0x184, `_getNeckIndex` 0x38, `peopleSetHeroMove` 0x8, `peopleRandomWalkPause` 0xA4, `_peopleWillMove` 0x194, `peopleMoveVector` 0x154, `peopleGetPosXYZ` 0x78, `peopleGetFollowerObjID` 0x34, `peopleSetFollower` 0x20, `peopleOpenWithoutModel` 0x70, `_peopleHasMotionType` 0x64, `_peopleUpdateRotation` 0x48, `peopleGetShadowLight` 0x20, `_peopleCheckNearOtherChara` 0xFC, the `peopleViewer*` set |
| Live in XD retail? | None of them. Retail's only extra people function is `fn_8029CD20` (0xF0), between `peopleMoveNeckTargetHero` and `peopleMoveNeckTarget`. It is a new neck-target setter (`peopleGet` ×2, `_getNeck`, stores at +0xC8..+0x104), with no initialized vector |
| Could any XD body produce the image? | **No.** XD's `people.o` `.rodata` holds no 12-byte object at all, live or `UNUSED`. It has only `...rodata.0`, three `UNUSED` 0x12-byte strings (`@4323..@4325`), and `@4740` 0xE, `@5987` 0x14, `@5988` 0x1F. No XD people function, live or stripped, has an initialized 12-byte local. XD rewrote the module (C++, `tagPeopleWork`), so its bodies cannot supply Colosseum's image owner |
| Also in the map | A stripped function-local static, `buffer$4259` / `init$4260` (`.sbss`, `UNUSED`), and three stripped `.sdata` tunables (`NUM_CYCLE`, `fix_end_range`, `fix_end_range2`) |

Size coincidence, a lead only: the Colosseum-live `fn_8018790C` is 0x154,
exactly the size of XD's stripped `peopleMoveVector`. If it really is
`peopleMoveVector`, it is live in Colosseum and cannot own the orphan image.

### Camera: `cameraDispInfo`'s strings

The demo has `cameraDispInfo` 0x3EC `UNUSED`. Retail `camera.o` has no extra
function and no uncovered bytes, so the function is stripped in every XD build.
No body exists. B43's finding stands: the 15 strings survive in Colosseum only
because `cameraInit` addresses `.rodata` from its base.

Two leads for the camera lane, from functions XD strips but Colosseum keeps:

- XD `cameraGetAnimeFrame` (0x74, `UNUSED`) is the same size as Colosseum's
  live `fn_80176C04` (0x74), and sits at the same place in the order;
- XD `cameraInitFloor` (0x2CC, `UNUSED`) matches `fn_80179748` (0x2D0) in the
  same way.

These names are supported by position and size only; there is no body to
compare against.

### jobj.c: the `.rodata` string pool behind `JObjLoad`'s offsets

XD's `jobj.o` `.rodata` matches Colosseum's 0x80274AA0-0x80274D58 **string
for string, in the same order and sizes**:

| Pool part | XD symbols | Content |
|---|---|---|
| Vectors | four 0xC vectors | |
| Library strings | `@1857` 0x18, `@1858`, `@2012`, `@2013`, `@2015`, `@2555` | `sysdolphin_base_library`, `hsd_jobj`, `object.h`, the ref-count asserts |
| Dump strings | `@2752`..`@2766` | "jobj[%d,%d]", `SKELETON_ROOT `, …, `  tra(G): ` |
| Asserts and roots | `@3371`..`@3376`, `@3697`, `@3887`, `@3889`, `@3993`, `@4546`..`@4548`, `@5107` | the `HSD_JObjAddChild` asserts, `jobj->child`, `jobj_root` / `jobj_root == NULL`, `hsdIsDescendantOf(...)`, … |

In XD all 31 strings are live, because `...rodata.0` is live.

The map settles which stripped functions own the strings. `@` numbers follow
code generation, which is address order in both builds, and each `UNUSED`
function sits in its emitted position.

- **The 0x8C bytes that shift `JObjLoad`'s pool offsets.** These are the dump
  strings `@2752`..`@2766` (0x80274B8C-0x80274C17 in Colosseum). They come
  between `HSD_JObjAlloc` and `HSD_JObjSetMtxDirty`, where the map lists
  `UNUSED` `HSD_JObjFree` 0x34, `HSD_JObjDmpNodeAll` 0x34,
  **`HSD_JObjDmpNode` 0x2EC**, `dmpSpace` 0x44, `getNbBros` 0x38 and
  `getNbParent` 0x20. The owner is `HSD_JObjDmpNode`, with its helpers. Its
  `.sdata2` short strings are `UNUSED` in XD too (`@2751` 2, `@2756` 8,
  `@2757` 8, `@2759` 2, `@2764` 4), so Colosseum would strip them as well.
- **`jobj_root` / `jobj_root == NULL` (`@3887` / `@3889`).** These come between
  `HSD_JObjResolveRefs` (`jobj->child`, `@3697`) and `HSD_JObjSetDefaultClass`
  (`@3993`). In that stretch the map has `HSD_JObjLoadJoint`, then `UNUSED`
  **`HSD_JObjLookup` 0x36C**, then `JObjLoad`, then `UNUSED`
  `HSD_JObjLoadJointSub` 0xD4, `GetDefaultClass` and `SetDefaultClass`. The
  3-byte `.sdata2` literal `@3888`, numbered between the two, is **live** in XD.
  That fits string reuse: `"jp"` is created first in the stripped function,
  then reused by `JObjUpdateFunc`'s assert. This is the "jp" assert pooled
  ahead of `JObjUpdateFunc`'s 1.0 that the jobj.c header mentions.

**Bodies:**

- demo: `UNUSED`;
- retail `jobj.o`: no extra function;
- Melee: only the orphan string arrays (`unused1..4`, `sysdolphin/baselib/jobj.c`
  lines 677-678 and 1544-1545).

No body exists for `HSD_JObjDmpNode` or `HSD_JObjLookup`, so the wall stays.
The map does give the jobj lane the names and sizes of the two owners.

### gs_spline.c: the stripped functions' error strings

- **Demo.** XD `GSspline.o` has the same three live functions as Colosseum.
  Its stripped functions are `GSsplineGetVectorValue` 0x3DC,
  `GSsplineGetFloatValue` 0x34C, `GSsplineAddControlFloatValue` 0x140,
  `GSsplineInitialise` 0x148, `_splineCalculateLagrangeParameters` and the
  `_create*` / `_splineCalculate*` helpers.
- **Retail.** No extra function and no uncovered bytes. No body exists.
- **Pool comparison.** The XD pool (`@745`..`@938`) against Colosseum's, which
  starts at **0x80273A10**:

  | Colosseum | XD | Owner | Match |
  |---|---|---|---|
  | 0x80273A10 0x3B, 0x80273A4C 0x29 | `@745`, `@746` | `GSsplineGetVectorValue` | same |
  | 0x80273A78 `"GSspline"` (9 bytes) | none | read by no live Colosseum function | **Colosseum only** |
  | `"GetFloatValue: spline parameters not initialised!"` 0x32 | `@807` 0x3A (`"GSsplineGetFloatValue: …"`) | `GSsplineGetFloatValue` | text differs |
  | `@808`, `@824`..`@826`, `@840`..`@842`, `@867`, `@935`..`@938` | same | GetFloatValue, AddControlVectorValue, AddControlFloatValue, Initialise, Create | same, one for one |

- **Correction to B43.** Colosseum's first spline string is at 0x80273A10, not
  0x80273A80, and the two `GSsplineGetVectorValue` strings are present.
- **The "GSspline" string.** Its position places its first use in
  `GSsplineGetVectorValue`, after that function's two log strings, or in
  `GSsplineGetFloatValue`, before its strings. It is plausibly a memory tag,
  and XD dropped it.

## Names the map settles

**Committed (`d3f4976e`).** Eleven SDK functions. Each is in the same object,
between the same named neighbours, with the same size, and its body is
identical to the same-named XD retail function. Only the SDK sources that
referenced them changed. `main.dol` and `common_rel.rel` are OK and the report
is unchanged apart from names (7319/8605 matched before and after).

| Colosseum | Name | XD object | XD retail |
|---|---|---|---|
| `fn_8009A23C` | `__OSGetDIConfig` | OS.o | 800AA138 |
| `fn_800A4C80` | `DVDLowBreak` | dvdlow.o | 800B4B64 |
| `fn_800A836C` | `__DVDPrintFatalMessage` | dvdFatal.o | 800B8318 |
| `fn_800AE7A4` | `DSPCheckMailFromDSP` | dsp.o | 800BE8C4 |
| `fn_800AF660` | `__CARDReadStatus` | CARDBios.o | 800BF728 |
| `fn_800B71F0` | `GXInitFifoLimits` | GXFifo.o | 800C76DC |
| `fn_800B7484` | `GXSetBreakPtCallback` | GXFifo.o | 800C7C40 |
| `fn_800B9578` | `__GXSetGenMode` | GXGeometry.o | 800CA0F4 |
| `fn_800BAE5C` | `GXGetTexObjFmt` | GXTexture.o | 800CB888 |
| `fn_800C4C74` | `__shr2u` | runtime.o | 800DA620 |
| `fn_800CEF10` | `DBGReadMailbox` | DebuggerDriver.o | 800E777C |

**Candidates not committed.** The table below is `renames --min-sim 0.95`, less
the committed ones: equal size, same order, retail body ≥ 0.95 (1.00 unless
shown). They were left out for one of these reasons:

- the old name is referenced in a wall file (for example `THPPlayer.c` uses
  `VISetPostRetraceCallback`, and `jobj.c` uses the HSD ones);
- the function is tiny (0x8 getters), where order carries most of the weight;
- the object is head- or tail-aligned, so only one side is anchored;
- the Colosseum names in that object came from the noisy port (`dbgMenuFight*`).

The table is for the owning lanes to confirm and apply with their sources.

| XD object | Colosseum → XD name |
|---|---|
| dbgMenuFight.o | `fn_800077F8` dbgMenuFightFightTrainerAiAddsubWaza; `fn_80007840` dbgMenuFightFightTrainerAiAddsubIrekae; `fn_80008144` dbgMenuFightMain; `fn_8000814C` dbgMenuFightGetYubiwohuruWazaDataId; `fn_80008154` dbgMenuFightGetSoubikoukaFlag; `fn_8000815C` dbgMenuFightGetFightPokeStMenuOpenFlag; `fn_80008164` dbgMenuFightGetFightTrainerAiLastValueReviseFlag; `fn_8000816C` dbgMenuFightGetFightTrainerAiFlowDispFlag; `fn_80008174` dbgMenuFightGetFightDebugMenuOpenFlag; `fn_8000817C` dbgMenuFightGetAllFightTrainerOperateFlag |
| dbgMenuHero.o | `fn_8000BA94` dbgMenuHeroCreateItem; `fn_8000BAB8` dbgMenuHeroIncCoupon; `fn_8000BB00` dbgMenuHeroIncMoney |
| menuPokeCoupon.o | `fn_8007C260` menuPokeCouponExit |
| dbgMenuToolBattle.o | `fn_80088F58` dbgMenuGBAClearCouponFlag; `fn_80088F74` dbgMenuGBASetClearFlag; `fn_80088F88` dbgMenuGBAResetCoupon |
| gbaCommand.o | `fn_80089F58` gbaCommandGetGBAChangePokemon; `fn_80089F60` gbaCommandGetWazaTarget; `fn_80089F68` gbaCommandGetWazaDataId; `fn_80089F70` gbaCommandGetAction |
| agbCommunication.o | `fn_80072684` _alarm_handler; `fn_80073690` PKJB_SendWazaText (0.964) |
| menuCB_Bios.o | `fn_8006A7D8` menuCBTrainerBios_GetPlayerID; `fn_8006A7E0` menuCBTrainerBios_SetHomePlace; `fn_8006A7E8` menuCBTrainerBios_GetHomePlace; `fn_8006A814` menuCBTrainerBios_GetControlerID; `fn_8006A81C` menuCBTrainerBios_SetControlerID; `fn_8006B8E8` menuCBBios_ContextIsLocked; `fn_8006B8F0` menuCBBios_UnlockContext; `fn_8006B8FC` menuCBBios_LockContext |
| menuCB_Rule.o | `fn_80077A5C` menuCBRule_IsBlankPokemon (0.950) |
| dbgMenuMenu.o | `fn_8000CB54` testD2Present; `fn_8000CCA8` dbgMenuMenuTestTeacherMemo; `fn_8000CCD0` dbgMenuMenuTestCompleteMemo |
| menuLogoDemo.o | `fn_80035DD4` menuStaffRollExit; `fn_800365B0` menuTpcExit; `fn_800366A4` _NintendoWarningMain |
| OSLink.o | `fn_8009E7A8` OSNotifyLink; `fn_8009E7AC` OSNotifyUnlink |
| OSMemory.o | `fn_8009F3D4` OSGetPhysicalMemSize |
| OSThread.o | `fn_800A128C` DefaultSwitchThreadCallback; `fn_800A13F8` OSGetCurrentThread |
| dvd.o | `fn_800A7820` DVDSetAutoInvalidation; `fn_800A7BCC` DVDGetCurrentDiskID |
| vi.o | `fn_800A880C` VISetPreRetraceCallback; `fn_800A8850` VISetPostRetraceCallback; `fn_800AA280` VIGetRetraceCount |
| dsp.o | `fn_800AE794` DSPCheckMailToDSP |
| GXFifo.o | `fn_800B770C` GXGetCPUFifo; `fn_800B7714` GXGetGPFifo |
| GXAttr.o | `fn_800B856C` GXInvalidateVtxCache |
| GXMisc.o | `fn_800B8E74` GXPixModeSync; `fn_800B8FD8` GXSetDrawSyncCallback; `fn_800B90A4` GXSetDrawDoneCallback |
| GXLight.o | `fn_800BA198` GXInitLightAttn; `fn_800BA414` GXInitLightPos; `fn_800BA424` GXInitLightDir; `fn_800BA440` GXInitLightColor |
| AmcExi2Stubs.o | `fn_800CE79C` EXI2_Init; `fn_800CE7A0` EXI2_EnableInterrupts; `fn_800CE7A4` EXI2_Poll; `fn_800CE7AC` EXI2_ReadN; `fn_800CE7B4` EXI2_WriteN; `fn_800CE7BC` EXI2_Reserve; `fn_800CE7C0` EXI2_Unreserve |
| DebuggerDriver.o | `fn_800CE7D4` DBClose; `fn_800CE7D8` DBOpen; `fn_800CECAC` DBGReadStatus |
| SIBios.o | `fn_800D0338` SISetCommand; `fn_800D034C` SITransferCommands; `fn_800D0F44` SIProbe |
| SISamplingRate.o | `fn_800D104C` SIRefreshSamplingRate |
| floor.o | `fn_80113F48` floorGetCurrentGroupID |
| seq_api.o | `fn_8014D5C8` sndSeqPause; `fn_8014D648` sndSeqContinue |
| stream.o | `fn_8014E7CC` streamCorrectLoops |
| synthvoice.o | `fn_801576B0` vidMakeRoot |
| snd3d.o | `fn_8015FE84` s3dExit |
| snd_init.o | `fn_8015FFD4` sndIsInstalled |
| snd_service.o | `fn_80162070` sndRand; `fn_801621BC` sndConvertMs; `fn_80162214` sndConvert2Ms |
| hardware.o | `fn_8016245C` hwSetTimeOffset; `fn_80162464` hwGetTimeOffset; `fn_8016248C` hwSetMesgCallback; `fn_80162F68` hwExitStream; `fn_80162F88` hwGetStreamPlayBuffer; `fn_80162FAC` hwTransAddr; `fn_801630E4` hwSetSaveSampleCallback; `fn_801631A8` hwFrameDone; `fn_801631C0` hwDisableHRTF |
| hw_aramdma.o | `fn_80163794` aramExit; `fn_80163798` aramGetZeroBuffer |
| hw_dolphin.o | `fn_801640C4` salStartAi; `fn_80164324` hwExitIrq; `fn_80164398` hwIRQEnterCritical; `fn_801643B8` hwIRQLeaveCritical |
| sound.o | `fn_801653BC` soundGetEnvID; `fn_801653C4` soundGetBGMID |
| GSsnd.o | `fn_801677BC` _sndFreeEmitter; `fn_801677F4` _sndFreeListener; `fn_8016782C` _sndFreeSndWork; `fn_80167864` _sndGetNewEmitter (0.969); `fn_801678E4` _sndGetNewListener (0.969); `fn_80167964` _sndGetNewSndWork (0.969); `fn_801679E4` _sndInitEmitter; `fn_80167A14` _sndInitListener; `fn_80167A6C` _sndInitSndWork |
| GSdvd.o | `fn_80167DC0` GSdvdSetErrorMsg; `fn_80167E54` GSdvdWrite; `fn_80168164` _freeDvdWork |
| fade_effect.o | `fn_801C6928` fadeEffectInit |
| script.o | `fn_801CA858` scriptSetPoint; `fn_801CA884` scriptGetPoint |
| memcard.o | `fn_801D04E8` memcardIsMemCardValid |
| GSvtr.o | `fn_801E1170` GSvtrSlowPlay; `fn_801E118C` GSvtrSingleStep; `fn_801E119C` GSvtrPlay; `fn_801E11B0` GSvtrPause; `fn_801E11CC` GSvtrGetInfoDisp; `fn_801E11D4` GSvtrSetInfoDisp; `fn_801E11E0` GSvtrGetState; `fn_801E11E8` GSvtrIsActive; `fn_801E1258` GSvtrEnable |
| fightMain.o | `fn_801EF7B4` fightSoundOpenPlayInit |
| fight.o | `fn_801EF61C` fightSetFightEncountDataId; `fn_801EF624` fightGetFightEncountDataId; `fn_801EF62C` fightSetFightResultId; `fn_801EF634` fightGetFightResultId; `fn_801EF63C` fightIsFight |
| fightSeqWSWaza.o | `fn_802136A4` _fightSeqClearOneSelfTurnCountSub |
| fightSeqWazaseq.o | `fn_80217BD0` fightSeqGetYatuatariIryoku; `fn_80217BEC` fightSeqGetOngaesiIryoku |
| displayfunc.o | `fn_8019733C` HSD_JObjSetSPtclCallback |
| fobj.o | `fn_80199A84` dumpObjAlloc |
| jobj.o | `fn_8019D610` HSD_JObjSetPtclTargetCallback; `fn_8019D618` HSD_JObjSetDPtclCallback; `fn_801A0D3C` iref_CNT; `fn_801A1980` HSD_CObjGetViewingMtxPtrDirect |
| mobj.o | `fn_801A6DA0` HSD_MObjAddTObj |
| robj.o | `fn_801AEBE0` dumpObjAlloc |
| shadow.o | `fn_801B06D4` HSD_ShadowSetDebug |
| tev.o | `fn_801B3168` _HSD_StateInvalidateTexCoordGen; `fn_801B387C` HSD_StateGetNumTevStages; `fn_801B3884` HSD_StateInitTev |
| tobj.o | `fn_801BBE3C` HSD_TObjAddNext; `fn_801BE800` HSD_TObjAnimAll |
| peopleInfoBios.o | `fn_8018F4AC` peopleInfoBiosGetTalkSe; `fn_8018F5CC` peopleInfoBiosGetColBallSize; `fn_8018F5FC` peopleInfoBiosGetColType; `fn_8018F618` peopleInfoBiosGetNeckDownLimit; `fn_8018F638` peopleInfoBiosGetNeckUpLimit |
| peopleBios.o | `fn_8018FC08` peopleBiosSetRot; `fn_8018FC2C` peopleBiosGetRot; `fn_8018FC74` peopleBiosSetPos; `fn_8018FC98` peopleBiosGetPos; `fn_8018FCBC` peopleBiosGetPosPtr |

**Confirmed body match, name already present.** Colosseum `0x80134274`
(`dbgMenuGetRootMenu`, 0x44) matches XD's `dbgMenuGetRootMenu` (retail
80066C48) at 0.86. The difference is only the ROM-version switch values.
XD keeps that function in `dbgMenuSub.o`; Colosseum places it inside the
`dbgMenu.o` range (see below).

## TU boundaries (findings only; `splits.txt` is unchanged)

### floor.c starts at 0x80111DF8, not 0x80112380, and runs past 0x801140DC

**Start.** The four functions between `GScolsys2Sun` and `floorCheckFightKind`
(`fn_80111DF8` 0x134, `fn_80111F2C` 0x150, `fn_8011207C` 0x1E4 and
`fn_80112260` 0x120) belong to floor.c. They are currently in
`GScolsys2Sun_range_80111C24.c`. The evidence:

- floor.c's own `.rodata` at 0x80272094 is a pointer table,
  `{fn_8011207C, fn_80111F2C, fn_80111DF8}`;
- `fn_80112260` and `fn_80111DF8` read the `"scene_data"` string at
  0x802720B0, which is floor.c `.rodata`;
- in the XD map, `GScolsys2Sun` is the last function of `GScolsys2Sun.o`, and
  `floor.o` follows immediately. `floor.o` opens with the map-resource
  functions: `_floorMapPushCB`, `_floorGetStateSize` / `_floorRestoreStateData` /
  `_floorMakeStateData`, `_floorMapPopCB`, `_floorMapGetCountCB`,
  `floorMapResourceDataIterate` and `floorMapSetDisp`;
- all four Colosseum bodies resemble XD's `floorMapResourceDataIterate`
  (0.60-0.79). Each one walks the map's models and lights with the callback
  expanded into it;
- `fn_80112260(u8 disp)` sets model visibility and light activity. It is
  called with 0/1 from `menuPokemonChange` (4 sites) and from
  `gs_npc_event`. XD's `menuPokemonChangeInit` /
  `stateFunctionExChangeMain` call `floorMapSetDisp(0/1)` at the same points.
  So **`fn_80112260` is `floorMapSetDisp`**. It is not renamed here, because
  its size differs from XD's (the callback is inlined) and it is referenced
  in several sources;
- the three table functions are most likely the state handlers
  (`_floorGetStateSize` / `_floorRestoreStateData` / `_floorMakeStateData`,
  each with the iterator inlined). That is a lead only.

**End.** floor.c probably ends at 0x801142F8, not 0x801140DC.
`field_range_801140DC.c` (0x801140DC-0x8011432C) continues XD `floor.o`'s
tail in order and size:

| Colosseum | Size | XD `floor.o` | XD size |
|---|---|---|---|
| `fn_801140C8` | 0x14 | `floorClearReturnPoint` | 0x14 |
| `fn_801140DC` | 0x90 | `floorReSetReturnPoint` (`UNUSED` in XD) | 0x90 |
| `fn_8011416C` | 0x20 | `floorSetReturnPoint` (`UNUSED` in XD) | 0x20 |
| `fn_8011418C` | 0x4C | `floorGetSavePoint` (`UNUSED` in XD) | 0x4C |
| `fn_801141D8` | 0x20 | (`floorSetSavePoint`?) | 0x34 |
| `EvlogSet__FScUl` | | `EvlogSet`, a name already in `symbols.txt` | |

After that come `fn_80114254` (0x60, best body match `_floorMapGetResourceCounts`
0x60, 0.54) and `fn_801142B4`. `floorReadGFLPostFunc` (0x801142F8) opens
`floorRead.o`. The range also uses floor.c's work block `lbl_80408378`.

### people: 0x8018F470-0x8018FE30 is two XD objects, not one

The people.c header calls this range one separate unit. The XD map has two
objects here:

- **`peopleInfoBios.o`** (0x8018F490-0x8018F730). It runs from
  `peopleInfoBiosGetBoundCheck` to `peopleInfoBiosGetPtr`: 17 getters,
  6 anchored by body. Its data:
  - `.data` `@2101` 0x24: a 9-entry switch table, "its own switch table" in
    the header;
  - `.sdata2` with three 4-byte constants, which fit Colosseum's
    0x8047D8A8-0x8047D8B8: 0.0f, 0.0174533f and 1.0f.
- **`peopleBios.o`** (0x8018F730-0x8018FE30). It runs from
  `peopleBiosGetPushDataSize` to `peopleBiosGetNewWork` and has **no** data
  sections in XD.

Colosseum's `peopleGetEntry`, `peopleGetMaxCount`, `peopleFree` and
`peopleInit` (0x8018FD88-0x8018FE30), and the `.sbss` at
0x8047B1F8-0x8047B208, have no counterpart in these two XD objects. Their
owner is open. `fn_8018F470` (0x20) sits just before
`peopleInfoBiosGetBoundCheck`.

### dbgMenu: 0x80133664 is where XD splits `dbgMenuSub.o` | `dbgMenu.o`, but Colosseum may be one object

In XD, `dbgMenuSub.o` ends with `debugMenuChangeMemInfo` / `debugMenuChangePerfBar`
and `dbgMenu.o` starts with `dbgMenuGetLayerID` / `dbgMenuSetLayerID` / `dbgMenuCursor`.
Colosseum `dbgMenuCursor` is at 0x80133664. Two pieces of evidence say
Colosseum still had **one** object:

1. **`.sbss` interleaves.** Colosseum's `.sbss` uses, in address order:

   | Address | Used by | XD owner |
   |---|---|---|
   | 0x8047AEB0-AECC | `fn_80132C6C` | dbgMenuSub side |
   | 0x8047AED0 / AED1 | `dbgMenuGetEnable` and the `dbgMenuMain` range | XD `dbgMenu.o`'s `_dbgMenuEnable` / `_dbgMenuMode` |
   | 0x8047AED4 | `fn_80133630` | dbgMenuSub side |
   | 0x8047AED8 / AED9 | `debugMenuShadowBorderDisp`, `fn_801334DC` | XD `dbgMenuSub.o`'s `borderDisp$2063` / `init$2064` |
   | 0x8047AEDC | the `dbgMenuMain` range | |

   Objects are contiguous within a section. So either this is one object, or
   `dbgMenuSub` owns the enable and mode flags.
2. **`dbgMenuGetRootMenu`.** It is in `dbgMenuSub.o` in XD but at 0x80134274
   in Colosseum, inside the `dbgMenu.o` range.

So the XD split is a hint, not proof, for Colosseum.

## Leads per lane

| Lane | Lead |
|---|---|
| **B7 hero_move** | `tools/xd_map_xref.py object heroMove.o` aligns 25 anchors. XD strips three functions that Colosseum keeps live: `heroMoveChkHinderClearPos` (0x344) at `fn_8012B19C` (0x448, 65%), `getCpLineCircle` (0x384) at `fn_8012D39C` (0x454) and `dispLeaderLog` (0xAC) at `fn_8012F1FC` (0x210). These are names only, with no XD body. XD retail bodies exist for the counterparts of the low-scoring Colosseum functions: `heroMoveChkHinderClear` 8014F070, `heroMoveInitEvent` 8014F5C0, `moveParty` 801502D0 (at `fn_8012DE94`), `updateLeaderMovement` 8015083C (at `fn_8012E388`, 0.62), `heroMoveTermEvent` 8014F518. XD's heroMove is C++ (`HEROMOVE_MEMBER`, `AnalogStickEmu`), so use these as structure references only. XD retail has one heroMove function the demo does not name, `fn_80151F6C` (0x20, not inspected). |
| **B15 dbgMenu** | The TU question above (one object vs `dbgMenuSub` + `dbgMenu`). `fn_80132C6C` is a table allocator (two `_toolentryAlloc` / `fn_800E27B0` pairs, then 8 unrolled item resets). No XD retail window scores 0.5 against its first 61 instructions. The XD names that could fit are stripped in XD (`dbgMenuInit` 0xD4 in `dbgMenu.o`; `dbgMenuDispAMemInfo` / `dbgMenuDispMemInfo` in `dbgMenuSub.o`). `fn_80132F7C` toggles menu 0xAB (`menuIsCheck` / `menuClose` / `menuOpenCustom`). It is the Colosseum form of XD `debugMenuChangeMemInfo` / `debugMenuNodrawArea`: 0.72, with the same call list. |
| **B15 spline** | Pool facts above: the Colosseum pool starts at 0x80273A10 with `GSsplineGetVectorValue`'s two strings; there is a Colosseum-only `"GSspline"` at 0x80273A78; and GetFloatValue's first string text differs. The owners, their order and their sizes are known from the map. No body exists in any build. |
| **B23 gs_vm** | Nothing new. XD's VM is the C++ `GSscript::Tiga`, so there is no C-era operand helper in the map (as B43 found). |
| **B30d/B30r people** | The two-object split of 0x8018F470-0x8018FE30 and its data (above). Ten candidate names for `peopleInfoBios` / `peopleBios` getters are in the table. For the 12-byte image, XD cannot supply any owner, live or stripped (above). Size lead: `fn_8018790C` (0x154) matches XD's `peopleMoveVector` (0x154). |
| **B32 THP** | XD replaced the player with `GSmovie.a` (`movieStream.o`, `GSmovie.o`, `THPDraw.o`). By body: `fn_801E260C` ≈ `StreamUpdateCallback` (0.63), `fn_801E2B74` ≈ `GetAudioSample` (0.60), `fn_801E34F0` ≈ `_StreamInitFillStreamBuffer` (0.52), `fn_801E2CA8` ≈ `StreamUpdateCallback` (0.40, weak). `movieStream.o` has these inlined-only `UNUSED` helpers: `FillStreamBuffer` 0x130 (the helper already admitted), `CheckBoundary` 0x78, `EntryBoundary` 0x40, `_movieStreamAudioDecode` 0x224 and `_movieStreamVideoDecode` 0x264. They are names with no body, so the sister-title clause is not met by them alone. |
| **U5 carves** | The rename candidate table. floor.c's true extent, 0x80111DF8-~0x801142F8, matters when floor-adjacent carves are linked: `GScolsys2Sun_range_80111C24.c` holds four floor.c functions, and `field_range_801140DC.c` holds floor.c's tail plus the start of `floorRead.o` at 0x801142F8. `tools/xd_map_xref.py objects` shows, for every XD object, the Colosseum split units its range overlaps. A carve that spans two XD objects is a boundary to check. |

## Reproduction

```sh
# clones (lane-private)
git clone https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps .lane/maps     # 6b51d3af
git clone https://github.com/TeamOrre/xd-decomp .lane/xd-decomp               # 4989794e
git clone https://github.com/trevor403/xd-asm .lane/xd-asm                     # b1087f18
python3 configure.py --no-progress && ninja all_source build/GC6E01/report.json
python3 tools/xd_map_xref.py retail-live
python3 tools/xd_map_xref.py --all-data object jobj.o
python3 tools/xd_map_xref.py --all-data object GSspline.o
python3 tools/xd_map_xref.py --all-data object people.o
python3 tools/xd_map_xref.py object floor.o
python3 tools/xd_map_xref.py renames --min-sim 0.95
```
