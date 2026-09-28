# Cross-build evidence recon (2026-09-28, lane B43)

Which other Genius Sonority builds (Colosseum regions and demos, XD regions and
demos, Pokémon Box, Battle Revolution) have public text that helps the
title-path walls? This recon used only public pages, repositories and symbol
maps. No disc images or game binaries were downloaded. Base: `baae25fa`.

## Headline: the XD JP demo's own linker map is public

The Pokémon XD JP demo disc (`NXXJ01`) ships its CodeWarrior linker map
(`files/Master.MAP`). A copy is published as `NXXJ01.map` in
[StarsMmd/Colo-XD-PBR-symbol-maps](https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps)
(repo HEAD `6b51d3afc20ed4c878e2a696ddb22ffaf179981b`; the file was added in
`6193d7c28547`, "Merged symbols from official map").
[TeamOrre/xd-decomp](https://github.com/TeamOrre/xd-decomp) (HEAD
`4989794e6c6430684e033bc56f4bb97c9a921e73`) supports the same build and refers
to it in `config/NXXJ01/config.yml` (`# map: orig/NXXJ01/files/Master.MAP`).

This is real `mwldeppc` output. It gives:

- object paths (`C:\pxdvs\program\game\src\app\camera\camera.o`);
- every linked symbol, with its size;
- every **dead-stripped** function and data object, as an `UNUSED` line with
  its size, in its original position within its object.

The `UNUSED` lines matter most here. They name functions that no shipped build
contains, including the functions a Colosseum TU inlined everywhere or stripped.
Whatever the XD map shows as inlined-only has no standalone body anywhere, so
it can support a name but never the code.

The other maps in that repository (`GC6E01*.map`, `GC6J01.map`, `GC6P01.map`,
`GXXE01/GXXJ01/GXXP01.map`, `RPB*.map`) are community maps derived from
`NXXJ01.map` (same `@NNNN` numbers, same object paths). **`GC6E01 official
only.map` is a byte-signature port, and it is noisy**: for example, it gives
`fn_80007778` five different `dbgMenuFight*` names. Its names already in our
`symbols.txt` include some that don't fit their code; `gs_task.c` notes
`dbgMenuCameraSetType` as "a reused debug-menu name". Only
`NXXJ01.map` is primary evidence.

## Findings by wall

### B43: camera.c (`cameraPlayAnime` row 43; `.rodata` and `cameraUpdate`)

1. **The stripped debug function is `cameraDispInfo`.** From `NXXJ01.map`, camera.o:
   ```
   0018eee0 000024 80194580 ... cameraSetMirrorFlag
   UNUSED   000018 ........     cameraGetAddSize
   UNUSED   0003ec ........     cameraDispInfo
   0018ef04 000020 801945a4 ... cameraSetOffsetScale
   ```
   Its `.rodata` has 15 `UNUSED` strings, `@2606 @2614 @2617..@2629`. Their
   sizes (`0xD 0xD 0xC 0x19 0x1B 0xF 0xF 0x15 0x1B 0x1D 0x1B 0x15 0xE 0xE
   0xD`) match Colosseum's 15 strings one for one, in the same order:
   "TargetFollow", "Offset Anime", "Type:  [%s]", "Pos:  (%.3f, %.3f, %.3f)", …
   "Far Z:(%.2f)" (0x80273DF8-0x80273F34). The function also has:
   - seven short strings, `@2607..@2613`, in `.sdata2` (the remaining mode names);
   - a 9-entry pointer table, `@2630` (0x24), in `.data`. That is one name per
     camera mode, with "TargetFollow" as mode 0 and "Offset Anime" as mode 8.

   Where it sits: in the map, right after `cameraSetMirrorFlag` (Colosseum
   `fn_801765F4`). That position is consistent with Colosseum's `.rodata`
   order:
   - initializer images come first, in source order, so its three zero
     vectors follow `cameraMoveTarget`'s;
   - strings come in code-generation order, so its strings precede
     `cameraWaitSyncAnime`'s.

   The code is stripped in the XD demo, XD retail and Colosseum, so **no source
   gives its body**.
2. **The strings survive naturally. Controlled test**, run in `.lane/striptest`
   and not committed, with GC/1.3.2 at the camera unit's flags and `mwldeppc`
   GC/1.2.5n:
   - MWCC emits the strings and initializer images of an uncalled global
     function.
   - It does the same for an unreferenced plain `static` function (code
     included).
   - A `static inline` that is never called emits nothing.
   - The linker then strips `.rodata` objects one by one, unless a live
     function in the same object addresses `.rodata` through the section
     symbol `...rodata.0`. In that case the whole `.rodata` section of the
     object is kept.

   Colosseum's `cameraInit` does exactly that: `lis/addi lbl_80273D98`, the
   unit's first `.rodata` byte, and then offsets. So `cameraDispInfo`'s
   strings and zero images stay, while its code, the `.data` name table and
   the `.sdata2` names are stripped. The XD demo map shows the flip side: XD's
   camera.o has `...rodata.0` itself `UNUSED`, so XD lost the strings too.
3. **A research shape reproduces `.rodata` byte for byte, but it is not
   committed.** The shape is:
   - three zero-initialized `GSSceneVec3` locals;
   - a `static` 9-name table;
   - the 13 formats in order;
   - placed just above `fn_801765F4`.

   With it, camera.c's `.rodata` is identical to retail, apart from the
   relocated handler words. It is not committed because its body is guessed.
   That falls under "invented stand-in".
4. **`cameraUpdate`'s view modes were separate functions in XD.** Camera.o
   lists, as `UNUSED` (inlined everywhere):

   | Function | Size | Mode |
   |---|---|---|
   | `_cameraFollowUpdate__FP9_GScamera` | 0x194 | 0 |
   | `_cameraLookAtUpdate__FP9_GScamera` | 0x218 | 1/2 |
   | `_cameraFreeUpdate__FP9_GScamera` | 0x9C | 3 |
   | `_cameraAnimeUpdate__FP9_GScamera` | 0x4 | 4, empty |
   | `_cameraDynamicUpdate__FP9_GScamera` | 0x174 | 7 |
   | `_cameraUpdateFov__FP9_GScamera` | 0x58 | the perspective re-apply that camera.c already inlines |

   Written as static inlines with those names, `cameraUpdate` goes from
   99.96% to 100% in the candidate:
   - retail's frame, with the sqrtf NaN slots at 76/80 above the perspective
     blocks at 12-72;
   - no helper symbol survives.

   This is committed in camera.c (unit still unlinked) and flagged there as a
   **judgement call**. The helpers are single-use, and XD has only their map
   names, with no address and no standalone body to compare calls against.
   The written sister-title clause is therefore not met.
5. Other stripped camera.o functions in XD: `cameraGetAddSize`,
   `cameraSetPositionXYZ`, `cameraGetAnimeFrame`, `cameraMoveTargetExt`,
   `cameraSetLookAt`, `cameraInitFloor`, `_cameraSearchFloor__FUi`,
   `_cameraGetPushDataSize`, `_cameraPushData`, `_cameraPopData`,
   `_cameraGetAnimeResouce`, `_cameraAnimLerp` (2).
6. The live "Camera Info" display is a menu window, not `cameraDispInfo`.
   Colosseum's `dbgMenuCameraChangeDisp` (0x80006654, name from the noisy port
   but the body fits) toggles menu 6 at (20, 260). So does the pad camera in
   `_cameraPadMoveUpdate` (pad button 0x400). TCRF lists the matching
   debug-menu text "カメラ情報 ON/OFF" ("Camera Info ON/OFF") among
   `common_rel.fdat`'s debug strings, "present in all versions except JP".

**Net for row 43:** `cameraPlayAnime` still needs the whole camera TU linked,
because its pool constants are shared (see `camera_candidate_80176C78.c`).
Every camera function is now exact in the candidate, one of them by judgement
call. The one remaining gap is `cameraDispInfo`'s `.rodata`. The name and the
data are evidenced, but no build shows the body.

### People lane: people.c sixth 12-byte zero image (0x80273FCC)

This is the same mechanism as camera.
- people.c addresses its strings from a block base; its header notes that "the
  string offsets from the block base are 12 short".
- So the whole `.rodata` section is kept, and a **stripped** people function
  with an initialized `GSvec` local would leave exactly such an image, with no
  reader in the live code.
- By `.rodata` order, the owner sits below `fn_80184D80` in address order.

`NXXJ01.map` lists these stripped people.o functions (XD sizes), in address
order among the live ones:
- `peopleSetFootParticle` 0x184, `_getNeckIndex` 0x38, `peopleSetHeroMove` 0x8,
  `peopleRandomWalkPause` 0xA4;
- `_peopleWillMove__FP13tagPeopleWork` 0x194, `peopleMoveVector` 0x154,
  `peopleGetPosXYZ` 0x78;
- `peopleGetFollowerObjID` 0x34, `peopleSetFollower` 0x20,
  `peopleOpenWithoutModel` 0x70;
- `_peopleHasMotionType` 0x64, `_peopleUpdateRotation__FPfff` 0x48,
  `peopleGetShadowLight` 0x20, `_peopleCheckNearOtherChara` 0xFC;
- the `peopleViewer*` debug set.

XD people.o `.rodata` also has three `UNUSED` 0x12-byte objects,
`@4323..@4325`. For the people TU (`__FUNCTION__` "peopleOpenSub"): XD's
`peopleOpenSub` is live at 0x8029C060. As with camera, the image owner's body
isn't evidenced, so it can't be linked. This only says where to look and
why an image with no reader is natural.

### dbgMenu lane: dbgMenu.c `fn_80132C6C` / pool helpers

Colosseum 0x80132C6C-0x80133664 lines up with XD's `dbgMenuSub.o` (no path
prefix in the map), and 0x80133664+ lines up with `dbgMenu.a …\dbgMenu\dbgMenu.o`.

XD `dbgMenuSub.o` has these `UNUSED` entries:
- `dbgMenuFsysInfo` 0x8, `dbgMenuDispNodrawArea` 0x2F8, `dbgMenuDispColorBar`
  0x410, `dbgMenuPartyChange` 0x6C, `dbgMenuDispAMemInfo` 0x2B4 and
  `dbgMenuDispMemInfo` 0x31C;
- static locals `maxResCount$2011`, `init$2012`, `minTotalFree$2014` and
  `init$2015` (`.sbss`);
- nine 4-byte `.sdata2` constants, `@2072..@2079` and `@2108`, and further
  constants `@2574..@2578`, `@2655`, `@2663`, `@2699..@2701` and `@2706`.

XD `dbgMenu.o` has these `UNUSED` entries: `dbgMenuGetMsgID` 0x8C,
`dbgMenuDraw` 0x214, `dbgMenuChangeFunc` 0xD8, `dbgMenuChangeLink` 0x140,
`dbgMenuInit` 0xD4, `_dbgMenuCallFunction` 0x90, `dbgMenuSetFunc__FlPFll_l`,
`dbgMenuGetFunc__Fl`, `dbgMenuGetTitle__Fl`, `dbgMenuSetLink__Fls` and
`dbgMenuGetTerminate__Fl`.

The `DbgMenuItem` reset in `fn_80132C6C` (0x310) may belong to a
`dbgMenuInit`-like or `dbgMenuDisp*Info`-like function. No XD body exists
(all stripped), so this names candidates only. The recon above found "no XD
counterpart"; the map now supplies names.

### Spline lane: gs_spline.c

XD `GSspline.o` has the same three live functions as Colosseum:
`GSsplineFree`, `GSsplineAddControlVectorValue` and `GSsplineCreate`. The map
lists the stripped ones with sizes:
- `GSsplineGetVectorValue` 0x3DC, `GSsplineGetFloatValue` 0x34C,
  `GSsplineAddControlFloatValue` 0x140 and `GSsplineInitialise` 0x148;
- `_splineCalculateLagrangeParameters` 0x80;
- `_create{Vector,Float}{Lagrange,BezierHermite,Linear}Data`;
- `_splineCalculate{Lagrange,Hermite,Bezier,Linear}{Vector,Float}Value`.

XD keeps `...rodata.0 (entry of .rodata)` for GSspline.o, so the stripped
functions' error strings survive there too, as `@745..@938`. Colosseum's
strings at 0x80273A80-0x80273D97 name those functions themselves: "GSsplineGetFloatValue:
wrong data type!", "GSsplineAddControlFloatValue: …" and "GSsplineInitialise: …".
The two versions differ in one place: XD's first string is
"GSsplineGetVectorValue: …" (0x3B), while Colosseum's first is
"GetFloatValue: spline parameters not initialised!" (0x32). The keep-mechanism
is natural, but the stripped bodies still aren't evidenced.

### Script-VM lane: gs_vm.c operand helper

There's no usable name. XD's VM is the C++ rewrite `GSScript.a …\GSscript\tiga.o`
/ `tvariant.o` / `chank.o` / `defobj.o` (`GSscript::Tiga::calcExpr1/2`,
`execute`, `execMethod`, …). Its `UNUSED` entries are C++ methods
(`getThread`, `run`, `freeTigaReg`, `createGSThread`, …), with no C-era
operand reader. This matches the earlier recon.

## Other builds checked

| Build | Public material | Useful? |
|---|---|---|
| Colosseum JP `GC6J01`, PAL `GC6P01` | Community address maps `GC6J01.map` and `GC6P01.map` (StarsMmd, 904 named lines each, mostly auto `zz_`). [GameTDB GC6J01](https://www.gametdb.com/Wii/GC6J01). TCRF: the debug-menu strings in `common_rel.fdat` are absent from JP. | No names beyond the XD-derived port. No public JP/PAL decomp or official map. |
| Colosseum Korean | None found; no evidence of a Korean GameCube release | - |
| Colosseum kiosk/demo | Colosseum appears on Interactive Multi-Game Demo Disc v14 as a movie only (no playable build, so no DOL/map). TCRF: every Colosseum disc carries the JP Bonus Disc's files. | No |
| Colosseum Bonus Disc (JP) | TCRF mention only; no map or decomp | No |
| XD JP demo `NXXJ01` | **`Master.MAP`** (above); TeamOrre/xd-decomp NXXJ01 config (23,354 symbols) | **Yes**: primary source of real names, including stripped functions |
| XD US `GXXE01` | TeamOrre/xd-decomp symbols; trevor403/xd-asm (`b1087f18efdb2d502b0f615ca55e5c1ba84f0344`) disassembly, code only | Code only. `cameraUpdate` there is the C++ rewrite (frame 0x150, `CameraAnimation<>` templates). |
| XD playable US demo (IMGDD 31-33) | Mentioned on Wikipedia/TCRF; no map published | Unknown; would be worth checking for a `Master.MAP` if a text copy is ever published |
| XD JP/PAL | Community maps `GXXJ01.map`/`GXXP01.map` | Derived, no new names |
| Pokémon Box R&S (`GPXE01`) | TCRF: at start-up it looks for `symbol.map`, which is not shipped; crash-debugger input sequence | No map. Its GS-engine link isn't verified. |
| Battle Revolution (Wii) | [bgsamm/pbr-dtk](https://github.com/bgsamm/pbr-dtk) (`09af9f9a5b1ffad9b83483485a6c49f35a1c4976`, RPBP01, 21,632 symbols); [pret/pokerevo](https://github.com/pret/pokerevo) (`ae02670ef33650e44313443dd95180f485150d84`) | No. The engine is a C++ rewrite (`GScamera::__ct(GSobject*, GSnodeData*)`), no `camera.o`/`cameraDispInfo`/`dbgMenu`/`GSspline` names, and the `RPB*.map` in StarsMmd are ports of the XD demo map. |

## Sources

- StarsMmd/Colo-XD-PBR-symbol-maps, `NXXJ01.map`, `GC6E01 official only.map`:
  https://github.com/StarsMmd/Colo-XD-PBR-symbol-maps (HEAD `6b51d3af`, NXXJ01.map at `6193d7c2`)
- TeamOrre/xd-decomp `config/NXXJ01/{config.yml,symbols.txt,splits.txt}`:
  https://github.com/TeamOrre/xd-decomp (`4989794e`)
- trevor403/xd-asm `code/func_FUN_80198100.s` (XD `cameraUpdate`):
  https://github.com/trevor403/xd-asm (`b1087f18`)
- bgsamm/pbr-dtk: https://github.com/bgsamm/pbr-dtk (`09af9f9a`); pret/pokerevo:
  https://github.com/pret/pokerevo (`ae02670e`)
- TCRF, Pokémon Colosseum/Unused Text, "Debug Strings" section (read through the
  Wayback Machine because tcrf.net serves a bot challenge):
  https://web.archive.org/web/2025/https://tcrf.net/Pok%C3%A9mon_Colosseum/Unused_Text
- TCRF, Pokémon Box Ruby & Sapphire (`symbol.map` lookup, crash debugger):
  https://tcrf.net/Pok%C3%A9mon_Box_Ruby_%26_Sapphire
- TCRF, Talk:Pokémon Colosseum (`debug_menu` file on the disc):
  https://tcrf.net/Talk:Pok%C3%A9mon_Colosseum
- Interactive Multi-Game Demo Disc v14 contents:
  https://sonic.fandom.com/wiki/Interactive_Multi-Game_Demo_Disc_Version_14
