#!/usr/bin/env python3

###
# Pokémon Colosseum (GC6E01) — dtk-template build configuration.
#
# Generates build.ninja and objdiff.json from the project configuration via the
# canonical decomp-toolkit pipeline (tools/project.py). The build splits the DOL
# into relocatable objects with dtk, links them back (substituting matching C
# objects where declared), and verifies the result against config/.../build.sha1.
#
# Usage:
#   python configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GC6E01",  # 0 — NTSC-U Rev 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="DIR",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (default: tools/dtk.exe)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (default: tools/objdiff-cli.exe)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.generate_map = args.map
config.non_matching = args.non_matching
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions (used as download fallbacks when the local path is absent).
config.binutils_tag = "2.42-2"
config.dtk_tag = "v1.8.3"
config.compilers_tag = "20251118"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# dtk, objdiff-cli and the Metrowerks compilers are all left unset so project.py
# downloads the pinned, PLATFORM-APPROPRIATE binaries (dtk_tag / objdiff_tag /
# compilers_tag) into build/ — the canonical dtk-template behavior, and what makes
# the Linux CI work from the same config as Windows. --dtk/--objdiff/--compilers
# override with a local copy.
config.binutils_path = args.binutils
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.compilers_path = args.compilers
config.sjiswrap_path = args.sjiswrap

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.force_active_symbols["main"] = [
    # game/gba/gba_conv_candidate_80088F58.c: debug-tool callbacks with no
    # reference in main.dol; compiled from source they would be dead-stripped.
    "fn_80088F58",
    "fn_80088F74",
    "fn_80088F88",
    "dbgMenuGBAAddCoupon",
    "dbgToolBattleDebugSetAGBConnectionMode",
    # game/menu/menu_r56b_800714C8_suffix.c: fn_8007169C has no reference
    # in main.dol but is present in retail; compiled from source it would be
    # dead-stripped.
    "fn_8007169C",
    # dolphin/sdk_candidate_8009ED70.c: OSLinkFixed has no reference in
    # main.dol but is present in retail.
    "OSLinkFixed",
    # game/dbgMenu_candidate_80133250.c / 801334A8.c: debug-menu callbacks
    # referenced only from unlinked data.
    "dbgMenuGSmemDispMap",
    "debugMenuShadowBorderDisp",
    # game/dbgMenu_r61_middle_8013327C.c: the same, for the GSmem check,
    # frame-rate and mail entries.
    "dbgMenuGSmemCheck",
    "dbgMenuFrameRate20",
    "dbgMenuFrameRate30",
    "dbgMenuSendAllMail",
    "dbgMenuSendMail",
    # game/dbgMenu_r61_prefix_80132C6C.cpp: menu callbacks referenced only from
    # unlinked data; compiled from source they would be dead-stripped.
    "fn_80132F7C",
    "dbgMenuMovieTest",
    "dbgMenuColisionDisp",
    "fn_80133050",
    "fn_8013308C",
    "fn_801330C8",
    "dbgMenuGSmemOptimize",
    "debugMenuColorBarDisp",
    "fn_801334DC",
    "fn_80133630",
    "fn_80006724",
    "fn_8000677C",
    "fn_800067D4",
    "fn_8000682C",
    "sprintf",
    "vsprintf",
    "vprintf",
    "printf",
    # game/movie.c: the movie scene callbacks have no reference in main.dol
    # but are present in retail; compiled from source they would be
    # dead-stripped.
    "fn_80035DD4",
    "fn_80035E04",
    "fn_80035EE4",
    "fn_80035F34",
    "fn_80035F64",
    "fn_800361C0",
    "fn_80036210",
    "fn_80036240",
    "fn_80036360",
    "fn_800363B0",
    "fn_800363B4",
    "fn_800363B8",
    "fn_800363BC",
    "fn_80036468",
    "fn_800364C8",
    "fn_800365B0",
    "fn_800365E0",
    "fn_80036640",
    "fn_8003669C",
    "fn_800366A0",
    "fn_800366A4",
]
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
# The retail DOL matches when linked with the project linker-script override and
# no extra mwldeppc flags.
config.ldflags = []
if args.map:
    config.ldflags.append("-mapunused")

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Base CodeWarrior flags, common to most GC games. Per-object overrides live in
# the libs/objects below. The byte-match linker is GC/1.2.5n (see ra/mwldeppc.exe).
config.linker_version = "GC/1.2.5n"

cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hard",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

if args.debug:
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# The people TU's one flag set (src/game/people/people.c header).
PEOPLE_TU_CFLAGS = [
    "-inline auto,deferred",
    "-use_lmw_stmw on",
    "-sdata 8",
    "-sdata2 8",
    "-str reuse,readonly",
]

Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Linked only when configured with --non-matching
DataCandidate = NonMatching       # Compared by objdiff, but not linked yet
CodeCandidate = NonMatching       # Compared by objdiff, but not linked yet


# REL modules were built with the SN Systems ProDG toolchain (GCC 2.95, SN's
# assembler and GNU-ld-based linker), not CodeWarrior: see
# docs/REL_MODULES.md. -G0 keeps small data out of the module.
config.gnu_ld_modules = ["common_rel"]
cflags_rel = [
    "-O0",
    "-G0",
    "-I include",
    f"-I build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]


def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "ProDG/3.5",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


def GameLib(lib_name: str, mw_version: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": mw_version,
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": objects,
    }


# Undeclared units link the dtk-extracted object as-is, reproducing the original
# DOL. Matching source objects are declared here and substituted in.
config.warn_missing_config = False
config.warn_missing_source = False
config.libs = [
    GameLib(
        "Runtime.PPCEABI.H",
        "GC/1.2.5n",
        [
            Object(Matching, "trk/ddh_cc_range_800C3E90.c", mw_version="GC/1.3", progress_category="runtime"),  # CALIB_TRK
            Object(
                Matching,
                "crt/stdio_range_800C7558.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # CALIB_CRT
            Object(
                Matching,
                "trk/ddh_cc_range_800C3C00.c",
                mw_version="GC/2.6",
                progress_category="runtime",
                extra_cflags=["-rostr"],
            ),  # BANK_TRK
            Object(
                Matching,
                "trk/gdev_cc_range_800C41AC.c",
                mw_version="GC/2.6",
                progress_category="runtime",
                extra_cflags=["-rostr"],
            ),  # BANK_TRK
            Object(Matching, "trk/gdev_cc_range_800C4444.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK
            # HAL sysdolphin mtx.c, the whole translation unit, built with the
            # library flags; it owns its .data (HSD_identityMtx), .bss (the
            # matrix/vector alloc data) and .sdata2 (strings and float pool).
            Object(
                Matching,
                "hsd/mtx.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/objalloc.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            Object(Matching, "hsd/hsd_obj_info_exact_801AA568.c", mw_version="GC/1.3", progress_category="hsd"),  # PR419 exact
            # HAL sysdolphin pobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/pobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
  # BANK_HSD_POBJ
            # HAL sysdolphin quatlib.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .sdata2.
            Object(
                Matching,
                "hsd/quatlib.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin robj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/robj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(Matching, "trk/TRKTarget_range_800C1310.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK2
            Object(
                Matching,
                "trk/TRKComm_range_800C3678.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKNub_exact_800BE47C.c",
                mw_version="GC/1.3",
                extra_cflags=["-rostr"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKNub_candidate_800BE6B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BE800.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKNub_candidate_800BE844.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BEBB0.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BEC18.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BED14.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_range_800BEE74.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKSerial_range_800BF088.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-rostr",
                    "-common off",
                    "-inline deferred",
                    "-char signed",
                    "-sdata 0",
                    "-sdata2 0",
                    "-sdatathreshold 0",
                ],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBuffer.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKDispatch_range_800BF53C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKDispatch_range_800BF53C_r41_800BFECC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-O4,s"],
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKDispatch_range_800BF53C_r41_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatch_r52_800C0504_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKDispatch_r52_800C08C0_inline_noauto.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-inline noauto"],
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKDispatch_range_800C0504_r41_800C0AA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-O4,s"],
                progress_category="runtime",
            ),
            *[
                Object(status, path, mw_version="GC/1.3", progress_category="runtime")
                for status, path in [
                    (Matching, "trk/TRKDispatch_exact_800C0CD8.c"),
                    (CodeCandidate, "trk/TRKDispatch_range_800C0CD8.c"),
                    (Matching, "trk/TRKDispatch_exact_800C0DA8.c"),
                    (CodeCandidate, "trk/TRKDispatch_candidate_800C0E60.c"),
                ]
            ],  # PR414_TRK_DISPATCH
            Object(
                CodeCandidate,
                "trk/TRKInterrupt.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTarget_range_800C1348.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKTarget_exact_800C1548.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(CodeCandidate, "trk/TRKTarget_residual_800C17CC.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKTarget_exact_800C195C.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKTarget_residual_800C1A08.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKTarget_residual_800C1A08_r41_800C1FB0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-O4,s"],
                progress_category="runtime",
            ),
            Object(Matching, "trk/TRKTarget_exact_800C24BC.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKTarget_residual_800C25FC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                CodeCandidate,
                "trk/TRKInit_r53_800C2D80_prefix.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "trk/TRKInit_r53_800C3218_lmw_on.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKInit_exact_800C3344.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),  # PR414_TRK_INIT
            Object(
                Matching,
                "trk/TRKBoard_exact_800C33BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-rostr", "-sdata 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(CodeCandidate, "trk/TRKBoard_candidate_800C3414.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKBoard_exact_800C349C.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata2 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKBoard_range_800C3630.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 0"],
                progress_category="runtime",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-char signed"]
                    + (
                        ["-use_lmw_stmw on"]
                        if path
                        in ["crt/printf.c", "crt/printf_residual_800C8864.c"]
                        else []
                    ),
                    progress_category="runtime",
                )
                for status, path in [
                    (Matching, "crt/printf.c"),
                    (Matching, "crt/printf_exact_800C87F8.c"),
                    (Matching, "crt/printf_residual_800C8864.c"),
                ]
            ],  # PR414_CRT_PRINTF
            Object(
                NonMatching,
                "__init_cpp_exceptions.cpp",
                source="crt_data/__init_cpp_exceptions.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mem_range_800C811C.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mem.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/string_range_800CA78C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/string.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/wchar.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/wchar_range_800C7FB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mwtrace_helpers.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/critical_regions.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/stdio_atexit.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A7CCC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDFs.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDQueue.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDError_ErrorCode2Num.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDError.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDLowInitWA.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDLowSetWAType.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/dvd/DVDLow.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/exi/EXI2Stubs.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/vi/VI_fn_800AA280.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/vi/VI_fn_800AA498.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/pad/PAD.c",
                extra_cflags=["-inline off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/pad/PAD_exact_800AB4FC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/pad/PAD_suffix_800ABD68.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/exi/EXI2_range_800CEA3C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B71F0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B770C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B7714.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B856C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA198.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA414.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA424.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA440.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BAE5C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BB2E4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BB2F8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/PPCArch.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSGetExceptionHandler.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSAlarmCreate.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSArena.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSContextCurrent.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSContextClear.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSInterruptHandlers.c",
                progress_category="sdk",
            ),
            Object(
                NonMatching,
                "dolphin/os/OSInterrupt.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSEXI_fn_8009E7A8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSEXI_fn_8009E7AC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSState_fn_8009FAEC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReboot_fn_800A064C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReboot_WriteSram.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSThreadQueue.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSThread_r51_800A1404_prefix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSThread_r51_800A1528_inline_noauto.c",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSThread_r51_800A16E8_suffix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSTime.c",
                source="dolphin/os/calendar/OSTime.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBInit.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBGetFirstCallback.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBIsExceptionMarked.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBPrintf.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800CF708.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800CF728.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SITypeDecode.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800D0F44.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_range_800D0F68.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800D104C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/menu/menu_bag.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/sound/sound.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/gs_effect.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/sex.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # msgctrl.c emits only its head (0x80131588-0x80131690) unless a
            # candidate chunk defines MSGCTRL_WHOLE_TU. The TU was built with
            # the peephole pass off (see msgctrl_exact_80132A38.c).
            Object(
                Matching,
                "game/msgctrl.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131690.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_candidate_80131714.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_801317FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/msgctrl_r49_8013182C_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_r49_80131A34_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131BA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/msgctrl_candidate_80131BF8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131F9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/msgctrl_candidate_80131FF4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # msgctrl.c tail on the TU's flags: -O4,p with the peephole pass
            # off (see the source header for the evidence).
            Object(
                Matching,
                "game/msgctrl_exact_80132A38.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ]
                    + (
                        ["-schedule on"]
                        if path
                        in {
                            "game/dbgMenu_candidate_80133218.c",
                            "game/dbgMenu_candidate_80133250.c",
                            "game/dbgMenu_candidate_80133050.c",
                            "game/dbgMenu_candidate_8013308C.c",
                            "game/dbgMenu_candidate_801333AC.c",
                            "game/dbgMenu_candidate_80133450.c",
                            "game/dbgMenu_candidate_801334A8.c",
                            "game/dbgMenu_candidate_801334DC.c",
                            "game/dbgMenu_candidate_80133630.c",
                        }
                        else []
                    )
                    # dbgMenu keeps its string literals in .rodata.
                    + (
                        ["-str reuse,readonly"]
                        if path == "game/dbgMenu_r61_middle_8013327C.c"
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/dbgMenu_r61_prefix_80132C6C.cpp"),
                    (Matching, "game/dbgMenu_candidate_80133050.c"),
                    (Matching, "game/dbgMenu_candidate_8013308C.c"),
                    (Matching, "game/dbgMenu_r61_middle_801330C8.c"),
                    (Matching, "game/dbgMenu_candidate_80133218.c"),
                    (Matching, "game/dbgMenu_candidate_80133250.c"),
                    (Matching, "game/dbgMenu_r61_middle_8013327C.c"),
                    (Matching, "game/dbgMenu_candidate_801333AC.c"),
                    (Matching, "game/dbgMenu_candidate_80133450.c"),
                    (Matching, "game/dbgMenu_candidate_801334A8.c"),
                    (Matching, "game/dbgMenu_candidate_801334DC.c"),
                    (CodeCandidate, "game/dbgMenu_r61_middle_80133510.c"),
                    (Matching, "game/dbgMenu_candidate_80133630.c"),
                    (Matching, "game/dbgMenu_r61_suffix_80133664.c"),
                    (Matching, "game/dbgMenu_exact_801337A0.c"),
                    (CodeCandidate, "game/dbgMenu_candidate_801337E4.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/pcbox_candidate_8013433C.c"),
                    (Matching, "game/pcbox_exact_801347D0.c"),
                    (Matching, "game/pcbox_exact_80135028.c"),
                ]
            ],
            Object(
                Matching,
                "game/pcbox_exact_801347E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pcbox_r54_80134E10_gc125_o1.c",
                mw_version="GC/1.2.5",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pcbox_r54_80134EF0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedatasave_status.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedata.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedataBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedatasaveBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/status.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/koukaBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/kouka.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/tenkouBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/tikeiBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    f"game/effect/{name}.c",
                    mw_version="GC/1.3",
                    cflags=[
                        "-O1"
                        if name == "effect_visual_candidate_80138838"
                        and flag == "-O4,p"
                        else flag
                        for flag in cflags_base
                    ],
                    extra_cflags=(
                        [
                            "-O1",
                            "-use_lmw_stmw on",
                            "-sdata 8",
                            "-sdata2 8",
                        ]
                        if name == "effect_visual_candidate_8013E54C"
                        else [
                            "-O1",
                            "-use_lmw_stmw on",
                            "-sdata 8",
                            "-sdata2 8",
                        ]
                        if name == "effect_visual_candidate_80138838"
                        else ["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    )
                    + (["-schedule on"] if name == "effect_visual_candidate_80138838" else [])
                    + (["-O3"] if name == "effect_visual_candidate_8013814C" else [])
                    + (["-O2"] if name == "effect_visual_r50_80139AC4_prefix" else [])
                    + (["-schedule off"] if name == "effect_visual_r49_80139074_suffix" else []),
                    progress_category="game",
                )
                for status, name in [
                    (Matching, "effect_visual_exact_801380D4"),
                    (CodeCandidate, "effect_visual_candidate_8013814C"),
                    (Matching, "effect_visual_exact_80138630"),
                    (CodeCandidate, "effect_visual_candidate_801386DC"),
                    (Matching, "effect_visual_exact_801387C0"),
                    (CodeCandidate, "effect_visual_candidate_80138838"),
                    (Matching, "effect_visual_exact_80138B00"),
                    (CodeCandidate, "effect_visual_r49_80138BBC_prefix"),
                    (CodeCandidate, "effect_visual_r49_80139074_suffix"),
                    (Matching, "effect_visual_exact_80139820"),
                    (CodeCandidate, "effect_visual_r50_80139AC4_prefix"),
                    (CodeCandidate, "effect_visual_r50_8013A1D4_suffix"),
                    (Matching, "effect_visual_exact_8013A42C"),
                    (Matching, "effect_visual_exact_8013AA8C"),
                    (CodeCandidate, "effect_visual_candidate_8013AB60"),
                    (Matching, "effect_visual_exact_8013AD68"),
                    (CodeCandidate, "effect_visual_candidate_8013AD9C"),
                    (Matching, "effect_visual_exact_8013B034"),
                    (CodeCandidate, "effect_visual_candidate_8013B268"),
                    (Matching, "effect_visual_exact_8013B490"),
                    (CodeCandidate, "effect_visual_r49_8013B5E4_prefix"),
                    (Matching, "effect_visual_exact_8013C5A0"),
                    (CodeCandidate, "effect_visual_r51_8013C670_prefix"),
                    (CodeCandidate, "effect_visual_r51_8013CA48_suffix"),
                    (Matching, "effect_visual_exact_8013CE58"),
                    (Matching, "effect_visual_exact_8013D604"),
                    (CodeCandidate, "effect_visual_candidate_8013D984"),
                    (Matching, "effect_visual_exact_8013DB64"),
                    (Matching, "effect_visual_exact_8013E470"),
                    (Matching, "effect_visual_candidate_8013E54C"),
                    (Matching, "effect_visual_exact_8013E5AC"),
                    (Matching, "effect_visual_exact_8013F000"),
                    (CodeCandidate, "effect_visual_candidate_8013F344"),
                    (Matching, "effect_visual_exact_8013F410"),
                    (CodeCandidate, "effect_visual_candidate_8013F80C"),
                    (Matching, "effect_visual_exact_8013FBE0"),
                    (CodeCandidate, "effect_visual_candidate_8013FF0C"),
                    (Matching, "effect_visual_exact_80140138"),
                    (CodeCandidate, "effect_visual_candidate_801402AC"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r57b_8013E8A4_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r57b_8013EA44_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r51_8013C718_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r51_8013D0A8_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O2", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r50_80139E80_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r49_80138DE4_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_r49_8013BE04_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_candidate_8013C074_o2.c",
                mw_version="GC/1.3",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_candidate_8013A520_gc125n.c",
                mw_version="GC/1.2.5n",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_candidate_8013DE6C.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/effect_visual_candidate_8013DE6C_r41_8013E258.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/item_range_80144574.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_get_private_id_exact_8014635C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_residual_801463C4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_exact_80146E88.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_residual_801485FC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_exact_80149090.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/synth.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_api.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/snd_synthapi.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/stream.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/synthdata.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/synthmacros.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-fp_contract off"]
                        if "_fp_contract_off" in path
                        or path == "musyx/runtime/snd_math.c"
                        or path == "musyx/runtime/snd3d.c"
                        else ["-inline noauto"]
                        if "_inline_noauto" in path
                        else []
                    ),
                    progress_category="musyx",
                )
                for status, path, version in [
                    (Matching, "musyx/runtime/synthvoice.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_ac.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_adsr.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_vsamples.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/s_data.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015A484.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015A838.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015A950.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015AAA0.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015AD1C.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015B250.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl_exact_8015D408.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd3d.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_init.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_math.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_midictrl.c", "GC/1.3.2"),
                ]
            ],
            # hw_volconv.c keeps every multiply and add separate (fmuls then
            # fadds, never fmadds) and salCalcVolume, its only function,
            # matches with -fp_contract off for the whole unit.
            Object(
                Matching,
                "musyx/runtime/hw_volconv.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/snd_service_exact_80162070.c"),
                ]
            ],
            Object(
                Matching,
                "musyx/runtime/hardware.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/hw_aramdma.c"),
                    (Matching, "musyx/runtime/hw_dolphin.c"),
                    (Matching, "musyx/musyx_candidate_801643D8.c"),
                    (Matching, "musyx/musyx_exact_80164488.c"),
                    (Matching, "musyx/musyx_r50_801644E0_prefix.c"),
                ]
            ],
            Object(
                Matching,
                "musyx/runtime/reverb_candidate_80164520.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/people/people_data_exact_80140ACC.c"),
                    (Matching, "game/people/people_data_exact_80141308.c"),
                    (Matching, "game/people/people_data_exact_80142368.c"),
                    (Matching, "game/people/people_data_candidate_801425E8.c"),
                    (Matching, "game/people/people_field_lookup_exact_80142984.c"),
                    (Matching, "game/people/people_data_exact_80142A88.c"),
                    (Matching, "game/people/people_data_exact_80142B24.c"),
                    (Matching, "game/people/people_data_exact_80142CF4.c"),
                    (Matching, "game/people/people_data_r51_801431AC_suffix.c"),
                    (Matching, "game/people/people_item_friend_exact_8014369C.c"),
                    (Matching, "game/people/people_data_candidate_801436F0.c"),
                    (Matching, "game/people/people_item_effort_exact_80143718.c"),
                    (Matching, "game/people/people_data_candidate_80143778.c"),
                    (Matching, "game/people/people_item_ppup_exact_801437A0.c"),
                    (Matching, "game/people/people_data_candidate_801437B8.c"),
                    (Matching, "game/people/people_item_hpup_exact_801437E0.c"),
                    (Matching, "game/people/people_data_candidate_801437F8.c"),
                    (Matching, "game/people/people_item_battle_boost_exact_801439B8.c"),
                    (Matching, "game/people/people_data_candidate_80143A44.c"),
                    (Matching, "game/people/people_item_tables_setters_exact_80143A94.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/people/people_data_r51_80142EF8_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/people/people_data_r49_80140588_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_data_r49_80140A9C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_item_getters_exact_80143C50.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_item_tail_exact_80144064.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/people/people_data_suffix_801441A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F7F80.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_range_801F7F80.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F8424.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_range_801F87CC.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_range_801F87CC_r41_801F8A18.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F8D80.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_r53_801F9130_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_r53_801F93F8_gc20p1_o1.c",
                mw_version="GC/2.0p1",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_r53_801F9600_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F9790.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_r51_801F99C8_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_r51_801F9F78_gc20_o2.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FA4B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_range_801FA524.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FA634.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801FAA58.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FB8F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801FBAD4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_db_range_801FBD10.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_pokemon_range_801FDB78.c",
                mw_version="GC/2.0p1",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-schedule on",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_pokemon_candidate_801FE168_gc20.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-O1",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_pokemon_candidate_801FE3F8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_pokemon_candidate_801FE3F8_r40_801FE91C_gc125n.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    Matching
                    if path
                    in {
                        "game/fight_pokemon_r58_801FEC10_prefix.c",
                        "game/fight_pokemon_r58_801FEF74_middle.c",
                        "game/fight_pokemon_r58_80200A5C_middle.c",
                    }
                    else CodeCandidate,
                    path,
                    mw_version=(
                        "GC/2.0"
                        if path
                        in {
                            "game/fight_pokemon_r58_801FED3C_o1.c",
                            "game/fight_pokemon_r58_801FF1BC_o1.c",
                        }
                        else "GC/1.3"
                    ),
                    extra_cflags=[
                        "-O1"
                        if path
                        in {
                            "game/fight_pokemon_r58_801FED3C_o1.c",
                            "game/fight_pokemon_r58_801FF1BC_o1.c",
                            "game/fight_pokemon_r58_80200B10_o1.c",
                        }
                        else "-O4,s",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                        *(
                            ["-schedule off"]
                            if path
                            in {
                                "game/fight_pokemon_r58_801FED3C_o1.c",
                                "game/fight_pokemon_r58_801FEF74_middle.c",
                            }
                            else []
                        ),
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/fight_pokemon_r58_801FEC10_prefix.c",
                    "game/fight_pokemon_r58_801FED3C_o1.c",
                    "game/fight_pokemon_r58_801FEF74_middle.c",
                    "game/fight_pokemon_r58_801FF1BC_o1.c",
                    "game/fight_pokemon_r58_80200A5C_middle.c",
                    "game/fight_pokemon_r58_80200B10_o1.c",
                ]
            ],
            Object(
                CodeCandidate,
                "game/fight_pokemon_candidate_80200E00_gc20.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-schedule on",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_pokemon_candidate_802010C8.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-O1",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_exact_80201248.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_target_exact_801F0058.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_target_801F0134.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_target_exact_801F0204.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fightActionFifoInit links as a data-free carve; the rest of the
            # range is scored from the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/fight_action_range_801F0898.c"),
                    (Matching, "game/fight_action_exact_801F108C.c"),
                    (CodeCandidate, "game/fight_action_range_candidate_801F1170.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_floor_exact_801F150C.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F1588.c"),
                    (Matching, "game/fight_floor_exact_801F1700.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F1990.c"),
                    (Matching, "game/fight_floor_exact_801F1A6C.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F1B14.c"),
                    (Matching, "game/fight_floor_exact_801F1F30.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F1F7C.c"),
                    (Matching, "game/fight_floor_exact_801F2020.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F2350.c"),
                    (Matching, "game/fight_floor_exact_801F2654.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F26A8.c"),
                    (Matching, "game/fight_floor_exact_801F27D4.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F2B5C.c"),
                    (Matching, "game/fight_floor_exact_801F32B0.c"),
                    (Matching, "game/fight_floor_candidate_801F32EC.c"),
                    (Matching, "game/fight_floor_exact_801F33E8.c"),
                    (Matching, "game/fight_floor_candidate_801F37B0.c"),
                    (Matching, "game/fight_floor_exact_801F3B24.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F3BB4.c"),
                    (Matching, "game/fight_floor_exact_801F4220.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F4354.c"),
                    (Matching, "game/fight_floor_exact_801F4460.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F54A4.c"),
                    (Matching, "game/fight_floor_exact_801F61BC.c"),
                    (CodeCandidate, "game/fight_floor_candidate_801F61EC.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_floor_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_side_exact_801F6B54.c"),
                    (CodeCandidate, "game/fight_side_candidate_801F6F38.c"),
                    (Matching, "game/fight_side_exact_801F7258.c"),
                    (CodeCandidate, "game/fight_side_candidate_801F72B0.c"),
                    (Matching, "game/fight_side_exact_801F7388.c"),
                    (Matching, "game/fight_side_exact_801F75F8.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_side_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801F7954.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_out_pokemon_candidate_80202810.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_8020355C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_candidate_802038A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_80203A6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_80206A04.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_out_pokemon_suffix_80207C6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_802096E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_out_pokemon_suffix_8020A8E0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020AE30.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_action_exact_8020AED0.c"),
                    (Matching, "game/fight_action_exact_8020AF30.c"),
                    (Matching, "game/fight_action_exact_8020D784.c"),
                    (Matching, "game/fight_action_exact_8020D844.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/fight_action_r51_8020B058_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_action_r51_8020CA98_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_action_r51_8020CFE0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020D868.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_kouka_exact_8020D968.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_candidate_8020DA14.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-proc 603"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_candidate_8020DAD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DD44.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DF10.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DF90.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020E020.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020E0B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_exact_8020E124.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_exact_8020E1A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_waza.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/fight_seq_candidate_80211170.c"),
                    (Matching, "game/fight_seq_exact_802117FC.c"),
                    (Matching, "game/fight_seq_exact_802119D4.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_seq_r54_80211810_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_seq_r54_80211830_gc13_o4p.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_seq_r54_802118FC_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/mail_exact_801D1338.c"),
                    (Matching, "game/mail_exact_801D13E4.c"),
                    (Matching, "game/mail_candidate_801D142C.c"),
                    (Matching, "game/mail_exact_801D1470.c"),
                    (Matching, "game/mail_exact_801D167C.c"),
                    (Matching, "game/mail_exact_801D16F0.c"),
                    (CodeCandidate, "game/mail_candidate_801D1734.c"),
                    (Matching, "game/mail_exact_801D1A44.c"),
                    (Matching, "game/mail_exact_801D1B10.c"),
                    (Matching, "game/mail_exact_801D1E50.c"),
                    (Matching, "game/mail_candidate_801D1F0C.c"),
                ]
            ],
            # mailMain TU (Colosseum map: mailMainReceiveTerminate,
            # mailMainInit): .text 0x801D2080-0x801D2B4C with its .sdata2
            # pool 0x8047E1B0-0x8047E1D8 and chkMailSend's jump table
            # 0x8036E130-0x8036E14C.
            Object(
                Matching,
                "game/mailMain.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Waza camera TU: the data-free camera start/stop functions link
            # as carves (wazaCameraStop is expanded at four retail sites), as
            # does wazaSequenceCameraGetPattern with its one pooled 0.0f;
            # the rest is scored from the whole-TU candidate
            # wazaSequenceCamera.c (included by the candidate wrappers).
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaSequenceCamera_exact_801D2B4C.c"),
                    (Matching, "game/wazaSequenceCamera_exact_801D2C6C.c"),
                    (CodeCandidate, "game/wazaSequenceCamera_candidate_801D2D28.c"),
                    (Matching, "game/wazaSequenceCamera_exact_801D2F94.c"),
                    (CodeCandidate, "game/wazaSequenceCamera_candidate_801D30BC.c"),
                    (Matching, "game/wazaSequenceCamera_exact_801D49D8.c"),
                    (CodeCandidate, "game/wazaSequenceCamera_candidate_801D4DA0.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/1.3.2"
                        if path == "game/wazaViewer_candidate_801D5464.c"
                        else "GC/1.3"
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaViewer_candidate_801D5328.c"),
                    (Matching, "game/wazaViewer_exact_801D53D4.c"),
                    (Matching, "game/wazaViewer_exact_801D53D8.c"),
                    (CodeCandidate, "game/wazaViewer_candidate_801D5464.c"),
                    (Matching, "game/wazaViewer_exact_801D744C.c"),
                    (CodeCandidate, "game/wazaViewer_candidate_801D7464.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_r56_801D7E58_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_r57_801D81CC_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_r57_801D84F4_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_candidate_801D87B0_gc125.c",
                mw_version="GC/1.2.5n",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_candidate_801D8B38_gc125.c",
                mw_version="GC/1.2.5",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_candidate_801D91EC_gc125.c",
                mw_version="GC/1.2.5",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_r56b_801D97F0_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_r56b_801D9950_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequenceEntry_candidate_801D9C1C_gc125.c",
                mw_version="GC/1.3",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceSys.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/wazaSequenceSys_r52_801DAC90_prefix.c"),
                    (CodeCandidate, "game/wazaSequenceSys_r52_801DAEF8_suffix.c"),
                    (Matching, "game/wazaSequenceSys_tail_exact_801DB060.c"),
                    (CodeCandidate, "game/wazaSequenceSys_tail_candidate_801DB288.c"),
                    (Matching, "game/wazaSequenceSys_tail_exact_801DB848.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/wazaSequenceSys_r52_801DADC0_gc11p1_o4s.c",
                mw_version="GC/1.1p1",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    cflags=[
                        "-O3"
                        if path == "game/wazaSequence_candidate_801DBDDC.c"
                        and flag == "-O4,p"
                        else flag
                        for flag in cflags_base
                    ],
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-O2"] if path == "game/wazaSequence_r52_801DC014_prefix.c" else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/wazaSequence_candidate_801DB988.c"),
                    (Matching, "game/wazaSequence_exact_801DBB10.c"),
                    (CodeCandidate, "game/wazaSequence_candidate_801DBDDC.c"),
                    (Matching, "game/wazaSequence_exact_801DBFB0.c"),
                    (CodeCandidate, "game/wazaSequence_r52_801DC014_prefix.c"),
                    (CodeCandidate, "game/wazaSequence_r52_801DC81C_suffix.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/wazaSequence_r52_801DC5F0_gc125_o2.c",
                mw_version="GC/1.2.5",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if path == "game/sequence_r51_801DE190_prefix.c"
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/sequence_exact_801DCDA8.c"),
                    (Matching, "game/sequence_exact_801DCF00.c"),
                    (Matching, "game/sequence_exact_801DCF84.c"),
                    (Matching, "game/sequence_candidate_801DD158.c"),
                    (Matching, "game/sequence_exact_801DD23C.c"),
                    (CodeCandidate, "game/sequence_candidate_801DD45C.c"),
                    (Matching, "game/sequence_exact_801DE164.c"),
                    (CodeCandidate, "game/sequence_r51_801DE190_prefix.c"),
                    (Matching, "game/sequence_r51_801DE598_suffix.c"),
                    (Matching, "game/sequence_exact_801DE654.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/sequence_r51_801DE418_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule off", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/kaisuu.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gapp_exact_80101B34.c"),
                    # fn_80101B90, fn_80101D5C and fn_80101D8C share the
                    # TU's literal pool (.sdata2 0x8047CD80-0x8047CDC0).
                    (Matching, "game/gapp_exact_80101B90.c"),
                    (Matching, "game/gapp_exact_80101FB8.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu_get_last_error_exact_80102004.c"),
                ]
            ],
            # The retail menu TU (0x80102004-0x80103E68) was built as a whole
            # with GC/1.3.2, the peephole optimizer off and read-only string
            # literals (-rostr: the GSlogWrite format is in .rodata, the
            # __FUNCTION__ string in .data). With that single setting and no
            # local pragmas every function of src/game/menu.c matches except
            # menuCursorNormal (register assignment only) and
            # _menuUpdateKeyInfo. GC/1.3 cannot reproduce retail's scheduling
            # of menuInit and _menuUpdateKeyInfo (loads hoisted above the
            # port-state stores). Open conflict: under -inline auto GC/1.3.2
            # inlines _menuGetAgbKeyInfo into _menuUpdateKeyInfo, which retail
            # calls; -inline noauto fixes that function but loses the
            # menuCloseSync/menuCloseCustom/menuGetCursor expansions the
            # 0x80102014 range needs. The exact, text-only ranges are linked
            # as function-boundary carves; see src/game/menu.c.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                        "-rostr",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu_r50_80102014_prefix.c"),
                    (Matching, "game/menu_r50_80102F38_o3.c"),
                    (Matching, "game/menu_exact_80103484.c"),
                    (Matching, "game/menu_exact_80103614.c"),
                    (Matching, "game/menu_exact_801038F8.c"),
                    (Matching, "game/menu_exact_80103BA8.c"),
                ]
            ],
            Object(
                Matching,
                "game/cursor_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Window TU (0x80103FE4 - 0x801058CC): -O4,p with the peephole
            # pass off, built with GC/2.5 like the winMsg TU after it; see
            # window_exact_80104318.c for the evidence.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/window_exact_80103FE4.c"),
                    (Matching, "game/window.c"),
                    (Matching, "game/window_exact_801040A0.c"),
                    (Matching, "game/window_exact_801040F0.c"),
                    (Matching, "game/window_exact_80104160.c"),
                    (Matching, "game/window_exact_80104318.c"),
                    (CodeCandidate, "game/window_candidate_80104530.c"),
                    (Matching, "game/window_exact_801045A8.c"),
                    (Matching, "game/window_exact_801046B8.c"),
                    (Matching, "game/window_candidate_801046C8.c"),
                    (Matching, "game/window_exact_80104704.c"),
                    (Matching, "game/window_exact_8010474C.c"),
                    (Matching, "game/window_candidate_80104828.c"),
                    (Matching, "game/window_r50_80104A94_o2.c"),
                    # windowOpen reads the window TU's own 1.0f pool literal (0x8047CDEC,
                    # shared with windowDrawSprite2) through an extern stand-in.
                    # RULE-EXCEPTION(title-path): linked anyway, see docs/RULE_EXCEPTIONS.md.
                    (Matching, "game/window_r50_80104CA0_suffix.c"),
                    (Matching, "game/window_exact_80105410.c"),
                    (Matching, "game/window_exact_801054B8.c"),
                    (Matching, "game/window_exact_80105624.c"),
                    (CodeCandidate, "game/window_candidate_80105634.c"),
                ]
            ],
            # winMsg TU (0x801058CC - 0x80106F98): -O4,p with the peephole
            # pass off, built with GC/2.5; see win_msg_exact_80105A3C.c for
            # the evidence. winMsgDraw (win_msg.c) is still a candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/win_msg.c"),
                    (Matching, "game/win_msg_exact_80105A3C.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/win_sequence.c"),
                    (Matching, "game/win_sequence_exact_801070F4.c"),
                ]
            ],
            # winSeq tail + winSprite TU (XD's winSprite.cpp) from
            # fn_80107170 to the menuOffScreen TU, with its literal pool,
            # jump table and .bss work area; GC/1.3.2, peephole off like the
            # winMsg TU, and -inline auto,deferred (functions are defined in
            # reverse address order). See the source header.
            Object(
                Matching,
                "game/win_sprite.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-inline auto,deferred",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu_offscreen.c",
                mw_version="GC/1.3",
                # All 12 functions match with the peephole pass off for the
                # whole TU (retail keeps the unfolded lbz/clrlwi and the
                # loop-entry branches); with it on, three of them do not.
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu_model.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/floor_data.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # floorEventGetTresure links as a data-free carve; the rest of the
            # TU is scored from the whole-TU candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/floor_event.c"),
                    (Matching, "game/floor_event_exact_80115E6C.c"),
                    (CodeCandidate, "game/floor_event_candidate_80116164.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/floor_character.c"),
                    (Matching, "game/floor_character_exact_80116E6C.c"),
                    (Matching, "game/floor_character_exact_80116F68.c"),
                    (Matching, "game/floor_character_exact_80117038.c"),
                    (Matching, "game/floor_character_exact_80117070.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/field_camera_exact_8011711C.c"),
                    (Matching, "game/field_camera.c"),
                    (Matching, "game/field_camera_exact_801174C4.c"),
                    (Matching, "game/field_camera_exact_80117AD4.c"),
                    (Matching, "game/field_camera_exact_80117AE4.c"),
                ]
            ],
            # floorUpdateFieldCamera: linked carve on the field camera TU's
            # flags (-opt nopeephole, as game/field_camera.c).
            Object(
                Matching,
                "game/field_camera_r50_80117514_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_camera_r50_801176C8_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_camera_r50_8011791C_suffix.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-inline noauto",
                    "-schedule on",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80117E58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80118100.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_801183EC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80118874.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_candidate_801195AC.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119824.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119BD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119D90.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_candidate_8011A0A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_8011A280.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_8011B2C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_8011B444.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/waza.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_data.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_8011F5FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_8011F77C.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80120B00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokemon_range_80121C18.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80122040.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokemon_range_801226D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801229F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_801237B8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80123C54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_exact_80123D58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80123E70.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801240C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80124410.c",
                # RULE-EXCEPTION(title-path): GC/1.3.2, unlike the rest of the
                # pokemon TU (GC/1.3); see docs/RULE_EXCEPTIONS.md.
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801248C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_set_status_exact_801254B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of pokemonGetStatus; owns its switch
            # table (jumptable_8035E4B0) like the pokemonSetStatus carve.
            Object(
                Matching,
                "game/pokemon_get_status_exact_8012640C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokemon_range_8012795C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of pokemonEvolutionAll and
            # pokemonEvolutionCreateAddPokemon; owns their .rodata list
            # initialisers and the evolution code's only .sdata2 float.
            Object(
                Matching,
                "game/pokemon_evolution_exact_8012805C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokemon_range_80128524.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80128CC0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_savedata_exact_80129280.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_bios_exact_8012A450.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_exact_8012AC9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # hero_move chunks of lane H1 are scored at the TU's own flags,
            # GC/1.3 -O4,p with deferred inlining (cbPoison expands
            # heroMoveInitEvent/heroMoveTermEvent, defined after it; see the
            # cbPoison comment in src/game/hero_move.c).
            Object(
                CodeCandidate,
                "game/hero_move_candidate_8012AD50.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Text-only carve of heroMoveChkHinderClear (pool stand-ins).
            Object(
                Matching,
                "game/hero_move_r49_8012B5E4_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Text-only carve: heroMoveAddAutoEvent and heroMoveSetEventList use
            # none of the TU's .sdata2 pool.
            Object(
                Matching,
                "game/hero_move_exact_8012BAD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r49_8012BBA8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_exact_8012BDE0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r40_8012BEB4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r46_8012C0B4_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Text-only carve of heroMoveCheckEvent (pool stand-ins).
            Object(
                Matching,
                "game/hero_move_r46_8012C540.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_exact_8012C660.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r46_8012CA84.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r46_8012D39C_o2.c",
                mw_version="GC/1.2.5n",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule on"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r46_8012D7F0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/hero_move_r46_8012E7B8_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_r46_8012EBD4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_exact_8012FAD8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_move_r46_8012FCD4_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_pokemon.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_8000D290.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_80011EA4.c",
                mw_version="GC/2.0p1",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule on"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_candidate_80012858.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_80033278.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/movie.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-inline deferred",
                    "-rostr",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_range_80037158.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_range_r47_8003A520_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_range_r47_8003A6C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_range_r46_8003AFDC_o4s.c",
                mw_version="GC/2.0",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_range_r46_8003B2D8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_8003B6D0_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_8003B814.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_8003B814_r46_8003CF38_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_8003B814_r46_8003D1FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_800492CC_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/pda_candidate_800495C8.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_pda_mail_r54b_8004B7EC_gc13_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_pda_mail_r54b_8004BDB8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_candidate_80080ED8_gc125.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_candidate_8008102C_gc125.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_candidate_80082650.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_candidate_80082960_gc125.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_r51_80082A88_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_r51_80082EA4_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_r51_80082FE4_middle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_r51_800836AC_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_80083CBC/fn_80083CFC link as a data-free carve on the TU's
            # flags; the rest of the suffix is scored from the candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/cardesavedata_r51_80083AF4_suffix.c"),
                    (Matching, "game/menu/cardesavedata_exact_80083CBC.c"),
                    (CodeCandidate, "game/menu/cardesavedata_r51_80083D30.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_80084038.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-O3", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_candidate_80084A8C_o3.c",
                mw_version="GC/2.5",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/cardesavedata_candidate_80087AE8.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-O3", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokeconv.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80090720.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gbaCommunication_candidate_80091DA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80092FC8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gbaCommunication_candidate_800937F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gbaCommunication_candidate_80094650_gc125n.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-O0",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gbaCommunication_candidate_8009567C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_800965C8 carve: GC/2.0 -O3, peephole off.
            Object(
                Matching,
                "game/gbaCommunication_candidate_800965C8_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-O3", "-use_lmw_stmw off", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gbaCommunication_candidate_80096C48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "dolphin/sdk_r48_80098108_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r48_80099400_o2.c",
                mw_version="GC/1.2.5",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r48_80099790_suffix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_8009A0F4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_8009A2D8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009A92C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_8009A9D8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009ABD0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009AC3C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_8009AC50.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009AFD0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r53_8009AFFC_o1.c",
                mw_version="GC/1.2.5n",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_r53_8009B1B8_suffix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.2.5n",
                    progress_category="sdk",
                    # OSFatal.c's RGB2YUV is emitted with separate fmuls/fadds,
                    # so this SDK family predates the project-wide
                    # -fp_contract on.
                    extra_cflags=["-fp_contract off"],
                )
                for status, path in [
                    (CodeCandidate, "dolphin/sdk_candidate_8009BD84.c"),
                    (Matching, "dolphin/sdk_exact_8009C2E0.c"),
                    (CodeCandidate, "dolphin/sdk_candidate_8009C578.c"),
                    (Matching, "dolphin/sdk_exact_8009C860.c"),
                    (Matching, "dolphin/sdk_candidate_8009CD38.c"),
                    (Matching, "dolphin/sdk_exact_8009D510.c"),
                    (CodeCandidate, "dolphin/sdk_candidate_8009DF3C.c"),
                ]
            ],
            *[
                Object(status, path, mw_version="GC/1.2.5n", progress_category="sdk")
                for status, path in [
                    (Matching, "dolphin/sdk_range_8009E7B0.c"),
                    (Matching, "dolphin/sdk_exact_8009ED4C.c"),
                    (Matching, "dolphin/sdk_candidate_8009ED70.c"),
                ]
            ],
            Object(
                Matching,
                "dolphin/sdk_range_8009F77C_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009F958.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009F9C8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_8009FADC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A03B4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800A07C4_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSSram_lock_exact_800A08F8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800A09B0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSSram_unlock_exact_800A0CB8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800A0D00.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A2B9C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version=version,
                    cflags=(
                        ["-O2" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o2
                        else None
                    ),
                    progress_category="sdk",
                )
                for path, version, use_o2 in [
                    ("dolphin/sdk_r58_800A2D38_prefix.c", "GC/1.2.5n", False),
                    ("dolphin/sdk_r58_800A30E4_o2.c", "GC/1.1p1", True),
                    ("dolphin/sdk_r59_800A3194_prefix.c", "GC/1.2.5n", False),
                    ("dolphin/sdk_r59_800A33B4_suffix.c", "GC/1.2.5n", False),
                ]
            ],
            Object(
                Matching,
                "dolphin/sdk_r59_800A335C_o2.c",
                mw_version="GC/1.1p1",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800A3458.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A35E4_suffix.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A3744.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A37CC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A3874.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3A78.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            # The vendor SDK separates vec.c from the quaternion source here.
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3C54.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3CB0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800A3D3C.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/dvdfs_range_800A4D28.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A7820.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A7880.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                DataCandidate,
                "game/data/data_80311C00.c",
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.2.5n",
                    extra_cflags=(
                        ["-inline deferred"]
                        if path == "dolphin/vi/VI_candidate_800A839C.c"
                        else []
                    ),
                    progress_category="sdk",
                )
                for status, path in [
                    (Matching, "dolphin/vi/VI_exact_800A8178.c"),
                    (CodeCandidate, "dolphin/vi/VI_candidate_800A839C.c"),
                    (Matching, "dolphin/vi/VI_exact_800A880C.c"),
                    (CodeCandidate, "dolphin/vi/VI_candidate_800A8894.c"),
                    (Matching, "dolphin/vi/VI_exact_800A8FE4.c"),
                    (CodeCandidate, "dolphin/vi/VI_candidate_800A9038.c"),
                    (Matching, "dolphin/vi/VI_candidate_800AA198.c"),
                ]
            ],
            Object(
                Matching,
                "dolphin/sdk_range_800AA288.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ai_exact_800AC02C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800AC440.c",
                mw_version="GC/1.1p1",
                extra_cflags=["-O4,p"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ai_exact_800AC5AC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800AC6D4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800AC910.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800AC954.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800ACB44.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800ACBFC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_dsp_exact_800AE3F0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800AE9FC.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800AF14C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800AF35C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800AF474.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800AF8A0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B016C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B0174.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B01AC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B0694.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B1788.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B2070.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B2968.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B29F4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B2F84.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800B3078.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B38DC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B3978.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B3B68.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800B4488.c",
                mw_version="GC/1.2.5n",

                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B45E8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B4644.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B4DC4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B4E50.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B4FC0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B5070.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5184.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B5228.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5AE0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5BA4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800B5C5C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800B5E8C_o4p.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B671C.c",
                mw_version="GC/1.2.5n",
                # GXInit.c tail (__GXInitGX): retail keeps the unfolded
                # `addi rX, rY, 0` moves and branch chains, so the TU was
                # built with the peephole pass off.
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B6FE0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B71FC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800B771C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800B771C_r40_800B7C18_gc11p1.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800B771C_r40_800B7D3C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B857C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B884C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B8AE8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B8DA8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B8E74.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B8E98.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B9578.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BA1B4.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800BA44C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800BA4C8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BA7C0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BAE64.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800BB30C_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BB780.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/sdk_range_800BB81C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800BC580.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800BC618.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BC8C8.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(CodeCandidate, "dolphin/sdk_candidate_800BC8F8.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/gx/GX_exact_800BCEBC.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(CodeCandidate, "dolphin/sdk_candidate_800BCFDC.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/gx/GX_exact_800BD07C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800BD16C.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD394.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800BD454.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-O2"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD554.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800BD58C.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD744.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_r52_800BD7A0_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800BE164_o3.c",
                mw_version="GC/1.2.5n",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BE30C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(CodeCandidate, "dolphin/sdk_candidate_800BE348.c", mw_version="GC/1.3", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_range_800BF33C.c",
                mw_version="GC/1.3.2",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "trk/trk_exact_800C3EBC.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_range_800C3EBC.c",
                mw_version="GC/1.3",
                extra_cflags=["-DTRK_RANGE_800C3EBC_ONLY"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_exact_800C4154.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_range_800C4470.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/gs_range_800C45A0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_range_800C470C.c",
                mw_version="GC/1.3",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800C4758.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_candidate_800C47A4_gc13.c",
                mw_version="GC/1.3",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C47F0_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C483C_o3.c",
                mw_version="GC/1.3.2",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4928_middle.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4A60_o3.c",
                mw_version="GC/1.3.2",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4B44_suffix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r53_800C4C50_o1.c",
                mw_version="GC/1.2.5n",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r53_800C4C74_o1.c",
                mw_version="GC/1.2.5n",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4C98_prefix.c",
                mw_version="GC/1.3",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4CC0_o3.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/sdk_r52_800C4D8C_suffix.c",
                mw_version="GC/1.3",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800C5458.c",
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-str pool,readonly"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "crt/math_range_800CAA58.c",
                source="crt_matching/math_range_800CAA58.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_ieee754_atan2.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "crt/math_range_800CB2B4.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "crt/math_range_800CB4D8.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_cos.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_rem_pio2.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_sin.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "crt/math_range_800CD648.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_copysign.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CDBE0.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_exact_800CE04C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CE378.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_exact_800CE59C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800CE7DC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CEB64.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_residual_800CED58.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CEF10.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CF254.c",
                mw_version="GC/1.2.5n",
                # This original EXIBios island alone was built without
                # instruction scheduling; adjacent Odemu/EXIUart code was not.
                extra_cflags=["-schedule off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CF47C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/gs_range_800D1070.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_mem.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_main_exact_800E3928.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw off"
                        if path == "game/gs_model_main_suffix_candidate_800E732C.c"
                        else "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_main_suffix_candidate_800E4AC0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4BF4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4C98.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4D18.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4D3C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4DB0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4E8C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E50A8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5188.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E51A4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5550.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E563C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5790.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E584C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5978.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E59C8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5A74.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5B68.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E5BE0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5D40.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E5E34.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5FAC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5FFC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E60F0.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E61BC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E638C.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E6478.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E65CC.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E66B8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6804.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E68D8.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E69C4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6B20.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6BC8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6DC0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6DCC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E7290.c"),
                    (CodeCandidate, "game/gs_model_main_suffix_candidate_800E732C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E85E8.c"),
                ]
            ],
            # modelShadowRender owns its .sdata2 literal pool (0x8047CBC0-0x8047CBE8)
            # and emits "shadow" there, so it needs -str reuse,readonly.
            Object(
                Matching,
                "game/gs_model_shadow_candidate_800E8684.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_shadow_exact_800E8EFC.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E8F80.c"),
                    (Matching, "game/gs_model_shadow_exact_800E8FE8.c"),
                    (Matching, "game/gs_model_shadow_flags_exact_800E90C8.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E9148.c"),
                    (Matching, "game/gs_model_shadow_exact_800E9288.c"),
                    (Matching, "game/gs_model_shadow_exact_800E92D8.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E9358.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/gs_model_state.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/1.3.2"
                        if path
                        in (
                            "game/gs_model_parse.c",
                            "game/gs_model_parse_exact_800EAFE4.c",
                        )
                        else "GC/1.3"
                    ),
                    # The parse TU is GC/1.3.2 -inline auto,deferred with
                    # strings in .rodata/.sdata2 (see gs_model_parse.c).
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-inline auto,deferred"]
                        if path == "game/gs_model_parse.c"
                        else []
                    )
                    + (
                        ["-str reuse,readonly"]
                        if path == "game/gs_model_parse_exact_800EAFE4.c"
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_parse.c"),
                    (Matching, "game/gs_model_parse_exact_800EAFE4.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_bound.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_bound_exact_800EB414.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_bound_exact_800EB464.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_bound_r55_800EB5A0_gc13_o2.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_bound_r55_800EB6E0_suffix.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_anim_candidate_800EBEEC.c"),
                    (Matching, "game/gs_model_anim_exact_800EC0E8.c"),
                    (Matching, "game/gs_model_anim_candidate_800EC35C.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_anim_exact_800EC4D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_exact_800ED4D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_candidate_800ED8C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_exact_800EE044.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part_exact_800EE150.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part_exact_800EE6B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_scratch_800EE928.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_scratch_alloc_800EEC38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_scratch_r57_800EEDF8_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_texture_range_800EEF48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80101910.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_8010C220 (an empty function) links as a carve; the rest of
            # the range is scored from the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_range_80109C88.c"),
                    (Matching, "game/gs_range_exact_8010C220.c"),
                ]
            ],
            # fn_8010C224: same compiler, peephole off (see the file header).
            Object(
                Matching,
                "game/gs_range_exact_8010C224.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-O3"] if o3 else []),
                    ],
                    progress_category="game",
                )
                for status, path, o3 in [
                    (Matching, "game/gs_range_8010CBD0.c", True),
                    (Matching, "game/gs_colsys_exact_8010D170.c", True),
                    (Matching, "game/gs_colsys_exact_8010D20C.c", True),
                    (Matching, "game/gs_colsys_candidate_8010E53C.c", True),
                ]
            ],
            Object(
                Matching,
                "game/GScolsys2Walk.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Walk_exact_8010E138.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_801140DC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_range_80114AE0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_exact_80115250.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_801654E0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80165D0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_80165DEC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80165FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_8016604C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80166D48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_80166E44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # HAL's particle link lists (pslist.c, named by its assert
            # strings): owns its .rodata string pool and the three per-link
            # .bss arrays. GC/1.3.2 pools the arrays in particleSort (1.3
            # does not); -inline deferred gives retail's .bss order (reverse
            # definition order; first-reference order would put
            # activeParticle first); -str readonly puts the strings in
            # .rodata. See the file header.
            Object(
                Matching,
                "game/pslist.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/ps_app_srt_exact_8016A644.c"),
                    (Matching, "game/ps_candidate_8016A79C.c"),
                    (Matching, "game/ps_app_srt_exact_8016A93C.c"),
                    (Matching, "game/ps_exact_8016A9B4.c"),
                    (Matching, "game/ps_app_srt_exact_8016AAAC.c"),
                ]
            ],
            # HAL's particle module (particle.c) as one TU, 0x80169034 -
            # 0x8016A644, with its .rodata strings, psSetGeneratorAngleRadius-
            # Scale's jump table (.data), the per-bank tables and point JObjs
            # (pooled .bss) and its .sdata2 pool, on the particle library
            # flags (GC/1.3.2 pools the .bss tables; -inline deferred gives
            # the reverse definition order). See the file header.
            Object(
                Matching,
                "game/particle.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle command interpreter (psinterpret.c) as one TU with
            # its .rodata __FILE__, psInterpretParticle0's jump table (.data),
            # getFloat's .sbss scratch and its .sdata2 pool, on the particle
            # library flags, no local pragmas. See the file header.
            Object(
                Matching,
                "game/psinterpret.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle display (psdisp.c) as one TU, 0x8016AB94 -
            # 0x8016EC1C, with its .rodata, the quad texture coordinates
            # (.data), the display matrices (.bss), its .sbss state and its
            # .sdata2 pool, on the particle library flags. See the file header.
            Object(
                Matching,
                "game/psdisp.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle TEV set-up (psdisptev.c) as one TU with its .sbss,
            # on the particle library flags. See the file header.
            Object(
                Matching,
                "game/psdisptev.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle generator module (generator.c) as one TU with its
            # data, on the pslist.c / psInitParticle flags. See the file header.
            Object(
                Matching,
                "game/generator.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-opt level=0"] if path in (
                            "game/gs_range_8017A5FC_prefix.c",
                            "game/gs_range_8017A624_middle.c",
                            "game/gs_range_8017A814_suffix.c",
                            "game/fsys/fsys_system_8017AAA4.c",
                            "game/fsys/fsys_request_8017AF6C.c",
                        ) else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_8017A5FC_prefix.c"),
                    (Matching, "game/gs_range_8017A624_middle.c"),
                    (Matching, "game/gs_range_8017A814_suffix.c"),
                    (Matching, "game/fsys/fsys_system_8017AAA4.c"),
                    (Matching, "game/fsys/fsys_request_8017AF6C.c"),
                ]
            ],
            # 0x8017F2C4 - 0x80180C78 is optimisation-level-0 code (peephole
            # and scheduling still on): dead induction counters kept in
            # r30/r31, single-use locals and parameters stack-homed. The
            # exact middle island matches entirely under `-opt level=0` with
            # no local pragmas, as does the LZSS prefix; the suffix residuals
            # stay candidates built with the same flag.
            Object(
                Matching,
                "game/gs_range_8017F2C4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_8017F3F8_middle.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # The 0x8017FA5C - 0x801812C4 tail is one retail unit (the job
            # pool tasks after 0x80180C78 share its .sbss job pool
            # lbl_8047B1E0/E4/E8 and are started by fn_8018094C; people.c
            # starts at 0x801812C4); its exact
            # functions are carved into their own objects so they can link,
            # the rest stay candidates. All share `-opt level=0` and
            # `-inline deferred`. Evidence for deferred inlining: fn_801800F8
            # addresses the unit's pooled .bss (queue +0, cache +0x20, arena
            # +0x1030). Normal inlining orders pooled .bss by first reference,
            # which puts the cache first (its pre-call store is the first
            # reference), while deferred inlining uses reverse definition
            # order, which gives retail's layout from plain definitions. Every
            # other exact function of the unit (fn_8017FDB0, fn_801808B4,
            # fn_801808E4, fn_80180B94) compiles byte-identically either way;
            # deferred inlining only reverses emission order, so
            # multi-function carves list their functions high address first.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0", "-inline deferred"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_8017FA5C_exact.c"),
                    (Matching, "game/gs_range_8017FA5C_suffix.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_8017FDB0.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_801800F8.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180320.c"),
                    (Matching, "game/gs_range_8017FA5C_residual_8018094C.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180B94.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180C78.c"),
                ]
            ],
            # HAL's bytecode.c (HSD_ByteCodeEval), built with the sysdolphin
            # library flags. Text-only candidate until the function is exact;
            # see the file header for the TU's data ranges.
            Object(
                CodeCandidate,
                "hsd/bytecode.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # hsdSearchClassInfo, hsdIsDescendantOf and hsdNew, carved from
            # HAL's class.c (the HSD base class): the whole TU with its
            # .rodata/.data/.sbss/.sdata2, exact with the HSD library flags and
            # no pragmas. See the file header.
            Object(
                Matching,
                "hsd/class.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL's mobj.c as one translation unit, text and data, with the
            # sysdolphin library flags and no local pragmas. See the file
            # header.
            Object(
                Matching,
                "hsd/mobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o1
                        else None
                    ),
                    extra_cflags=(
                        ["-use_lmw_stmw on"]
                        if use_o1
                        else ["-O2", "-use_lmw_stmw on"]
                    ),
                    progress_category="hsd",
                )
                for path, use_o1 in [
                ]
            ],
            # HAL's tobj.c as one translation unit, text and data, with the
            # sysdolphin library flags and no local pragmas. See the file
            # header.
            Object(
                Matching,
                "hsd/tobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "game/fight_range_80201764.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80265EC4_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_80265F94_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_80009178.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_800096B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_8000BE74.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80051710.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_range_8005344C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_exact_80054914.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_suffix_800552D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_prefix_80055E38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_exact_80056A80.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r56b_80056B74_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r56b_80057144_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r56b_80057270_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_suffix_80056B74_r41_80057538_gc125n.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_suffix_80056B74_r41_80057694.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80057B34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_candidate_80057C9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80057DE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_r56_80057E70_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_r56_80057F94_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_candidate_80058DCC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80058F08.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_pre_main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_800599AC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_8005CEE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_tail.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D26C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_tail_candidate_8005D2E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D3D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuColosseumBattle_tail_candidate_8005D5CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D6A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_bios_range_8005D7F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_debug_range_8005DA48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_candidate_8005E7F0_inline_off.c",
                mw_version="GC/1.3",
                cflags=[
                    "-inline off" if flag == "-inline auto" else flag
                    for flag in cflags_base
                ],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_candidate_8005FFE4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_r46_80060EF4_o3.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_r46_80061018.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_candidate_800615F4_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_candidate_80061A2C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_BattleStart_candidate_80061A2C_r41_80062834_gc125n.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_range_80062948.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80063D10.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_range_80063D14.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80064378.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r57_800643D4_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r57_80065628_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-i src/game/menu"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r57_80065730_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-i src/game/menu"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80065A48.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_range_800676EC.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80068738.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r55_80068794_gc13_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r55_800688C4_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80069048.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r50_8006905C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r50_80069220_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_r50_800693A4_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_800697C4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menuCB_range_800697F4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80069A08.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_Battle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_r56b_8007109C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_r56b_80071398_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # menuCB_Common.c tail carve: GC/1.3 -O4,p, peephole off.
            Object(
                Matching,
                "game/menu/menu_r56b_800714C8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # pkjb_uploader.c TU (0x800716C8 - 0x80075390): GC/2.0, -O4,p
            # with the peephole pass off; see pkjb_exact_80073990.c for the
            # evidence and include/game/menu/pkjb_uploader_shared.h for the
            # unit's extent. Carved at function boundaries; data stays extern.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/pkjb_exact_800716C8.c"),
                    (CodeCandidate, "game/menu/pkjb_candidate_80071AE4.c"),
                    (Matching, "game/menu/pkjb_exact_800722A0.c"),
                    (CodeCandidate, "game/menu/pkjb_candidate_80072A00.c"),
                    (Matching, "game/menu/pkjb_exact_80072C74.c"),
                    (Matching, "game/menu/pkjb_candidate_80072D58.c"),
                    (Matching, "game/menu/pkjb_exact_80073034.c"),
                    (Matching, "game/menu/pkjb_candidate_800733D0.c"),
                    (Matching, "game/menu/pkjb_exact_80073690.c"),
                    (CodeCandidate, "game/menu/pkjb_candidate_80073700.c"),
                    (Matching, "game/menu/pkjb_exact_80073990.c"),
                    (CodeCandidate, "game/menu/menu_candidate_80073E8C_gc20.c"),
                    (CodeCandidate, "game/menu/menu_candidate_80074324.c"),
                    (CodeCandidate, "game/menu/menu_candidate_8007480C_gc125n.c"),
                ]
            ],
            # GC/1.3 -O4,p chunk of the menu range bucket, built with
            # unit-wide -opt nopeephole instead of local pragmas; the
            # menuCBRule tail (0x80077A5C-0x80077ED4) links as a carve.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/menu/menu_candidate_80075390.c"),
                    (Matching, "game/menu/menu_exact_80077A5C.c"),
                    (CodeCandidate, "game/menu/menu_candidate_80077ED4.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/menu/menu_candidate_r47_80078390_o2.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_candidate_r47_800788BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_candidate_80075390_r46_8007B6D8_o3.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_candidate_80075390_r46_8007C23C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_poke_coupon.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSContext_range_8009B914.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/si/SI_range_800CF764.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_range_8018FE30.c"),
                    (Matching, "game/gs_flag_get_exact_801906A0.c"),
                    (CodeCandidate, "game/gs_range_8018FE30_suffix_8019075C.c"),
                    (Matching, "game/gs_flag_exact_801908D4.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/effect/fade_range_801C6934.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_801C766C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/field_range_801CA7EC_prefix.c"),
                    (Matching, "game/field_exact_801CA9F0.c"),
                    (Matching, "game/field_exact_801CA9F8.c"),
                    (CodeCandidate, "game/field_candidate_801CAA08.c"),
                    (Matching, "game/field_exact_801CADA0.c"),
                    (Matching, "game/field_candidate_801CADA8.c"),
                    (Matching, "game/field_exact_801CAE80.c"),
                ]
            ],
            Object(
                Matching,
                "game/field_exact_801CAEA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/field_range_801CAF0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (["-opt nopeephole"] if path in ("game/field_range_801CB180.c", "game/field_candidate_801CB834.c") else []),
                    progress_category="game",
                )
                for status, path, version in [
                    (Matching, "game/field_range_801CB180.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB59C.c", "GC/1.3"),
                    (CodeCandidate, "game/field_candidate_801CB61C.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB7C4.c", "GC/1.3"),
                    (Matching, "game/field_candidate_801CB834.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB9D8.c", "GC/1.3"),
                    (Matching, "game/field_candidate_801CBA0C.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CBA84.c", "GC/1.3"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/field_r55_801CBA90_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Whole memory-card TU (src/game/memcard.c, .text 0x801CBBAC-0x801D0AA0
            # with its .rodata, .data switch tables, .bss, .sbss and .sdata2):
            # GC/2.5 -O4,p with read-only strings and deferred inlining; see that
            # file for the evidence. Replaces the former per-function carves.
            Object(
                Matching,
                "game/memcard.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-str reuse,readonly",
                    "-inline deferred",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/battle/battle_exact_801D0AA0.c"),
                    (CodeCandidate, "game/battle/battle_candidate_801D0C30.c"),
                ]
            ],
            # fn_801DF160 links as a data-free carve; the rest of the range
            # is scored from the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_range_801DE698.c"),
                    (Matching, "game/gs_range_exact_801DF160.c"),
                    (CodeCandidate, "game/gs_range_candidate_801DF1D0.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/field_range_801DF790.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_candidate_801E09E0.c"),
                    # fn_801E0FB4 (XD _vtrUpdateFunc) opens the vtr unit and
                    # uses no pool constant, so it links as a data-free carve.
                    (Matching, "game/gs_exact_801E0FB4.c"),
                    (Matching, "game/gs_exact_801E1170.c"),
                    (Matching, "game/gs_candidate_801E11F0.c"),
                    (Matching, "game/gs_exact_801E1258.c"),
                    (Matching, "game/gs_candidate_801E1300.c"),
                    (Matching, "game/gs_exact_801E16D0.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_movie_801E189C.c",
                mw_version="GC/1.3.2",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-str reuse,readonly",
                ],
                progress_category="game",
            ),
            # Dolphin SDK THP player sample, as the game built it (the same
            # sample options doldecomp/sms uses: -inline auto + deferred).
            # Deferred inlining is evidenced by every TU's functions and .bss
            # objects being laid out in reverse source order, with the
            # sample's small queue helpers expanded into their callers; auto
            # inlining by THPVideoDecode.c (VideoDecoder only matches with it).
            # -O4,s is evidenced by two-register saves using stmw
            # (THPGXYuv2RgbSetup). THPPlayer.c builds with noauto: with auto,
            # its stream-start routine (0x801E34F0) is inlined into
            # THPPlayerPrepare, which the retail build did not do. On noauto
            # all 21 of its functions are exact except fn_801E2CA8 (98.7%).
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                    extra_cflags=[
                        inline,
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-str reuse,readonly",
                    ],
                    progress_category="sdk",
                )
                for status, path, inline in [
                    (Matching, "dolphin/thp/THPRead.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPDraw.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPPlayer.c", "-inline noauto,deferred"),
                    (Matching, "dolphin/thp/THPAudioDecode.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPVideoDecode.c", "-inline auto,deferred"),
                ]
            ],
            Object(
                Matching,
                "dolphin/thp/THPDec_range_801E5548.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/thp/THPDec_range_801E5A28.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/thp/THPDec_range_801E5DE4.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/thp/THPDec_range_801E6578.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/thp/THPDec_range_801ECAB0.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base]
                        if path == "game/field_range_801ECFE0.c"
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/field_range_801ECFE0.c"),
                    (Matching, "game/field_exact_801ED218.c"),
                    (Matching, "game/field_candidate_801ED310.c"),
                    (Matching, "game/field_exact_801ED388.c"),
                    (Matching, "game/field_exact_801ED3B8.c"),
                ]
            ],
            Object(
                Matching,
                "game/battle/battle_range_exact_801ED640.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801ED780.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EE034.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EE0BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/battle/battle_range_candidate_801EE958.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEAD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/battle/battle_range_candidate_801EEB34.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEC74.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801EEE6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEEB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "crt/__start.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80211A00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80212840.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802128D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80213158.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802128D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802134D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80213558.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802136A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80213A78.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80213A78.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00_exact_80214450.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80211A00_suffix_80216A58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80217018.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80217D34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80217E20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80218BD4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80218D24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80218FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80218FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219270.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_candidate_802192B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219804.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_candidate_80219838.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219D98.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219E10.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80219FE4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A054.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A2C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8021A338.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A6CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/fight_range_8021B910.c"),
                    (Matching, "game/fight_range_exact_8021C034.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_range_exact_8021C0F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8021CA00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O2"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021CB58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8021D40C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021D9C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8021FAD4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80220778.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80220868.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80220B8C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8022106C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80221104.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80222110.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80222BD8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80222C44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_802230BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80223A24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80224158.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802247D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_802249B8.c",
                mw_version="GC/2.0p1",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80226134.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_802274F0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80228DAC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/1.3.2"
                        if path == "game/fight_range_80229C28.c"
                        else "GC/1.3"
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/fight_range_80229704.c"),
                    (Matching, "game/fight_range_exact_80229B70.c"),
                    (CodeCandidate, "game/fight_range_80229C28.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_range_8022A504.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022B29C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022B2CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022BB84.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022BE2C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022D20C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022D6BC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022DCB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8022F2F8.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022FE20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80230568.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80232FE4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_802331F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80233DB0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80234A0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023565C_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802373B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023753C_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80238060.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802381C4_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_80238B0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80238E30_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8023A308.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023A740.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_range_8023B498.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00_exact_8023CA9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80240BD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_waza_value_candidate_80241660.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80241B70.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_waza_value_middle.c",
                mw_version="GC/1.2.5n",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_middle_exact_80244318.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_middle_suffix_8024498C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_802451C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_candidate_80245FC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80247048.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024A664.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_residual_8024B474.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024BA44.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_residual_8024BFC0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024C5BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024D818.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_candidate_8024DC7C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024DE8C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_irekae.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_irekae_candidate_8024E690_gc20.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    Matching if path == "game/fight_trainer_ai_irekae_r58b_80250070_suffix.c" else CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-O1"
                        if path
                        == "game/fight_trainer_ai_irekae_r58b_8024F8B4_o1.c"
                        else "-O4,s",
                        *(
                            ["-schedule off"]
                            if path == "game/fight_trainer_ai_irekae_r58b_8024F8B4_o1.c"
                            else []
                        ),
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/fight_trainer_ai_irekae_r58b_8024F8B4_o1.c",
                    "game/fight_trainer_ai_irekae_r58b_80250070_suffix.c",
                ]
            ],
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_irekae_r47_select_item_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_damage.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_80253950.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_waza_hit_candidate_802546E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O1", "-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_80254810.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_waza_hit_candidate_80256ED0.c",
                mw_version="GC/1.3",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_802570D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai_waza_hit.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025C5A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025C770.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_trainer_ai2_candidate_8025C808.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025CAA8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    cflags=[
                        "-O1"
                        if path == "game/toolentry.c" and flag == "-O4,p"
                        else flag
                        for flag in cflags_base
                    ],
                    extra_cflags=[
                        "-O1" if path == "game/toolentry.c" else "-O4,s",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-schedule on"] if path == "game/toolentry.c" else []),
                        *(["-schedule off"] if path == "game/toolentry_candidate_8025D644.c" else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/toolentry_exact_8025CD64.c"),
                    (CodeCandidate, "game/toolentry.c"),
                    (Matching, "game/toolentry_exact_8025D06C.c"),
                    (Matching, "game/toolentry_exact_8025D28C.c"),
                    (Matching, "game/toolentry_exact_8025D560.c"),
                    (CodeCandidate, "game/toolentry_candidate_8025D644.c"),
                    (Matching, "game/toolentry_exact_8025D744.c"),
                    (CodeCandidate, "game/toolentry_candidate_8025D788.c"),
                    (Matching, "game/toolentry_exact_8025D914.c"),
                    (Matching, "game/toolentry_exact_8025D938.c"),
                    (Matching, "game/toolentry_exact_8025D9A8.c"),
                    (Matching, "game/toolentry_exact_8025DBB0.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/toolentry_candidate_8025D164.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/toolentry_r55_8025D364_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/toolentry_r55_8025D3F4_gc13_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/table_res_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/pokemon_relive.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Dolphin SDK GBA library: GC/1.2.5n with the base SDK flags.
            *[
                Object(status, path, mw_version="GC/1.2.5n", progress_category="sdk")
                for status, path in [
                    (Matching, "game/gba/GBA.c"),
                    (Matching, "game/gba/GBARead.c"),
                    (Matching, "game/gba/GBAWrite.c"),
                    (Matching, "game/gba/GBAXfer.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/memo_exact_8025FEE4.c"),
                    (Matching, "game/memo_candidate_8025FF18.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/memo_r57b_8025FA20_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/memo_r57b_8025FBCC_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/memo_r57b_8025FD34_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/evolution_r57b_802600E4_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/evolution_r57b_80260EBC_o4p.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/evolution_r57b_802612D0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/char_name_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_gsfloor.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_menu.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_menu_candidate_8026316C_o2.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_menu_candidate_80263BC8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_exact_8026503C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_timer_exact_802658C8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_timer_candidate_80265A6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_timer_exact_80265B3C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/fight_timer_candidate_80265C84.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_timer_exact_80265D54.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_timer_exact_80265DB0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/d2present.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EF02C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_residual_801EF4B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EF5C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/battle/battle_main_candidate_801EFA08.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EFFC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/battle/battle_camera_exact_801C2AE8.c"),
                    (CodeCandidate, "game/battle/battle_camera.c"),
                    (Matching, "game/battle/battle_camera_exact_801C2D5C.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/battle/battle_camera_r50_801C2D80_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/battle/battle_camera_r50_801C2F00_suffix.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3108.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_r56_801C3114_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_r56_801C3A64_o1.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_range_800E0DDC_r40_800E1544_gc125n.c",
                mw_version="GC/1.3",
                cflags=[
                    "-O2" if flag == "-O4,p" else flag
                    for flag in cflags_base
                ],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule on"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuFight_r40_800117BC_gc125n.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-schedule off", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # menuFightCloseTop / fn_80011A1C: standalone data-free carve.
            Object(
                Matching,
                "game/menuFight_exact_800119A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pcbox_range_8001E3E0_r40_8001EF78.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pcbox_range_8001E3E0_r40_8001F304_gc125n.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pcbox_range_8001E3E0_r40_8001FD48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequence_candidate_801DC014_r40_801DCAA0_gc125n.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/wazaSequence_candidate_801DC014_r40_801DCBC8.c",
                mw_version="GC/1.2.5n",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuShop_r40_8002B880_gc125n.c",
                mw_version="GC/1.2.5n",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuShop_r40_8002BCE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_residual_801C3114_r40_801C3B80_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_801E075C belongs to the GC/1.3 unit that starts at fn_801DF474
            # (it reads that unit's .sdata2 pool 0x8047E3F0-0x8047E424);
            # GC/1.2.5n scored it far lower than GC/1.3 (file name kept).
            Object(
                CodeCandidate,
                "game/field_range_801DF790_r41_801E075C_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3C98.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_residual_801C3E3C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3F10.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/fade.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # fade_effect.o (XD fade_effect.cpp), linked whole: .text
            # 0x801C4814 - 0x801C6934 with its .sdata2 pool 0x8047DFD8 -
            # 0x8047E0A8. fade_effect.c includes fade_range_801C4CB8.c for
            # 0x801C4CB8 onwards. Built without the peephole pass
            # (docs/recon/fade_tu_d11.md).
            Object(
                Matching,
                "game/effect/fade_effect.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_party_access.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # gba_misc TU flag set: GC/2.0, -O4,p, "-opt nopeephole" (as the
            # fn_8008AC34 carve below); every exact function here matches under
            # it with no local pragmas.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gba/gba_misc_exact_800896B8.c"),
                    (Matching, "game/gba/gba_misc_r51_80089B8C_suffix.c"),
                    (Matching, "game/gba/gba_misc_exact_80089CA8.c"),
                    (Matching, "game/gba/gba_misc_exact_80089D30.c"),
                    (Matching, "game/gba/gba_misc_exact_80089D98.c"),
                    (Matching, "game/gba/gba_misc_candidate_80089E20.c"),
                    (Matching, "game/gba/gba_misc_exact_80089F58.c"),
                    (Matching, "game/gba/gba_misc_exact_8008A99C.c"),
                    (Matching, "game/gba/gba_misc_exact_8008A9AC.c"),
                    (CodeCandidate, "game/gba/gba_misc_candidate_8008A9E4.c"),
                    (Matching, "game/gba/gba_misc_exact_8008AB4C.c"),
                    (Matching, "game/gba/gba_misc_exact_8008ABE4.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/gba/gba_misc_r51_800896E8_o2.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gba/gba_misc_r51_80089978_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gba/gba_misc_candidate_8008ABE4_r42_8008AC34_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    Matching if path.endswith("_8008C6FC_suffix.c") else CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if path == "game/gba/gba_misc_r58_8008C5D4_o1.c"
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for path in [
                    "game/gba/gba_misc_r58_8008AE18_prefix.c",
                    "game/gba/gba_misc_r58_8008C5D4_o1.c",
                    "game/gba/gba_misc_r58_8008C6FC_suffix.c",
                ]
            ],
            Object(
                CodeCandidate,
                "game/gba/gba_misc_candidate_80089F78.c",
                mw_version="GC/2.0p1",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gba/gba_conv_exact_80088EA8.c"),
                    (Matching, "game/gba/gba_conv_candidate_80088F58.c"),
                    (Matching, "game/gba/gba_conv_exact_80089028.c"),
                    (Matching, "game/gba/gba_conv_candidate_80089030.c"),
                ]
            ],
            Object(
                Matching,
                "game/gba/gba_conv_r49_80088428_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gba/gba_conv_r49_800884BC_gc125_o2.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    cflags=(
                        [opt if flag == "-O4,p" else flag for flag in cflags_base]
                        if opt
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path, version, opt in [
                    (CodeCandidate, "game/gba/gba_conv_r59_800886D0_o1.c", "GC/2.0", "-O1"),
                    (Matching, "game/gba/gba_conv_r59_80088964_middle.c", "GC/1.3", None),
                    (CodeCandidate, "game/gba/gba_conv_r59_800889E4_o1.c", "GC/2.0", "-O1"),
                    (CodeCandidate, "game/gba/gba_conv_r59_80088C60_suffix.c", "GC/1.3", None),
                ]
            ],
            Object(
                Matching,
                "game/GScolsys2Util.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Human_range_8010FAF4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Human_exact_8010FFC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/GScolsys2Thru_candidate_801101B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Thru_exact_80111470.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Thru_candidate_8011163C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/GScolsys2Thru_r56_80111864_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Check.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # The GScolsys2Sun TU (.text 0x80111C24-0x80111DF8, .sdata2
            # 0x8047CF68-0x8047CF70), linked whole; the floor helpers after it
            # stay a candidate. Both build nopeephole, like floor.c.
            Object(
                Matching,
                "game/GScolsys2Sun.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/floor_range_80111DF8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/floor.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_colsys.c"),
                    (Matching, "game/GScolsys2Util_exact_8010C77C.c"),
                    (Matching, "game/gs_colsys_obj_enable_exact_8010C7BC.c"),
                    (Matching, "game/gs_colsys_exact_8010C8D0.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_field_resource.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D13C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Carve of fn_800D1B3C/fn_800D1D00 with the cameraBuildMatrix
            # inline (repeated expansion; XD name); text only.
            Object(
                Matching,
                "game/gs_render_util_exact_800D1B3C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D1EB8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D1F58.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ]
                    + (
                        ["-DGS_RENDER_UTIL_SUFFIX_800D1B3C"]
                        if status is CodeCandidate
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_render_util_exact_800D207C.c"),
                    (Matching, "game/gs_render_util_exact_800D20CC.c"),
                    (Matching, "game/gs_render_util_candidate_800D2150.c"),
                    (Matching, "game/gs_render_util_exact_800D21C8.c"),
                    (Matching, "game/gs_render_util_exact_800D2248.c"),
                    (Matching, "game/gs_render_util_exact_800D2584.c"),
                    (Matching, "game/gs_render_util_candidate_800D258C.c"),
                    (Matching, "game/gs_render_util_exact_800D2738.c"),
                    (Matching, "game/gs_render_util_candidate_800D27FC.c"),
                    (Matching, "game/gs_render_util_exact_800D2B44.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (["-opt nopeephole"] if path == "game/gs_event_exec_candidate_80014118.c" else []),
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_event_exec.c"),
                    (Matching, "game/gs_event_exec_exact_80013744.c"),
                    (CodeCandidate, "game/gs_event_exec_candidate_8001374C.c"),
                    (Matching, "game/gs_event_exec_exact_800140FC.c"),
                    (Matching, "game/gs_event_exec_exact_80014110.c"),
                    (Matching, "game/gs_event_exec_candidate_80014118.c"),
                    (Matching, "game/gs_event_exec_exact_80014198.c"),
                    (CodeCandidate, "game/gs_event_exec_r47_prefix.c"),
                    (CodeCandidate, "game/gs_event_exec_r47_suffix.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/gs_event_exec_r47_80014574_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # menuFight prefix: fn_8000DAA8/fn_8000DAB0 and the timer window
            # functions (0x8000DC88-0x8000DE24) link as data-free carves; the
            # rest is scored from the whole-TU candidate wrappers.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menuFight_exact_8000DAA8.c"),
                    (CodeCandidate, "game/menuFight_r51_8000DAA8_prefix.c"),
                    (Matching, "game/menuFight_exact_8000DC88.c"),
                    (CodeCandidate, "game/menuFight_r51_8000DE24.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/menuFight_r51_80011288_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuFight_r51_800114A4_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuFightStatus.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_range_80069C0C.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # menuCB_Bios.c part of menu_middle (0x8006A65C - 0x8006B6B4: the
            # functions using its rodata, lbl_80267DD8 / the "menuCB_Bios.c"
            # asserts at 0x80267DE8 - 0x80267EA3): GC/1.3, -O4,p with the
            # peephole pass off; see menu_middle_range_8006AF44.c for the
            # evidence.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-i src/game/menu",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/menu_middle_exact_8006A65C.c"),
                    (Matching, "game/menu/menu_middle_exact_8006A824.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006A990.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AC28.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006ACCC.c"),
                    (Matching, "game/menu/menu_middle_exact_8006ADB4.c"),
                    (Matching, "game/menu/menu_middle_range_8006AE18.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AEEC.c"),
                    (Matching, "game/menu/menu_middle_range_8006AF44.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AFC4.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006AFE4.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B09C.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006B154.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B1C0.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006B2A4.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B354.c"),
                    (CodeCandidate, "game/menu/menu_middle_range_8006B420.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B4AC.c"),
                    (CodeCandidate, "game/menu/menu_middle_r48_8006B5D0_prefix.c"),
                ]
            ],
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006B6B4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006B8E8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_range_8006B9B8.c",
                mw_version="GC/1.2.5n",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_range_8006B9B8_r41_8006BB34.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version=version,
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-i src/game/menu",
                    ],
                    progress_category="game",
                )
                for path, version in [
                    ("game/menu/menu_middle_candidate_8006DC28.c", "GC/1.3"),
                ]
            ],
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version=version,
                    cflags=(
                        [opt if flag == "-O4,p" else flag for flag in cflags_base]
                        if opt
                        else None
                    ),
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-i src/game/menu",
                    ],
                    progress_category="game",
                )
                for path, version, opt in [
                    ("game/menu/menu_middle_r59_8006C164_o1.c", "GC/2.0", "-O1"),
                    ("game/menu/menu_middle_r59_8006C5D8_suffix.c", "GC/2.0", None),
                    ("game/menu/menu_middle_r59_8006CCC0_o1.c", "GC/2.0", "-O1"),
                    ("game/menu/menu_middle_r59_8006D940_suffix.c", "GC/2.0", None),
                    ("game/menu/menu_middle_r59_8006E338_o1.c", "GC/2.0", "-O1"),
                    ("game/menu/menu_middle_r59_8006E798_middle.c", "GC/2.0", None),
                    ("game/menu/menu_middle_r59_8006E9A4_o1.c", "GC/1.3", "-O1"),
                    ("game/menu/menu_middle_r59_8006F284_prefix.c", "GC/2.0", "-O0"),
                    ("game/menu/menu_middle_r59_8006F720_o1.c", "GC/2.0", "-O1"),
                    ("game/menu/menu_middle_r59_8006FBFC_suffix.c", "GC/2.0", None),
                ]
            ],
            Object(
                Matching,
                "game/menu/menu_middle_r50_8006EE7C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_r50_8006EFF8_gc20_o4s.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_candidate_8006C7D4.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006FCF8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_range_8006FEE4.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_80070274.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_range_80070318.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_800704A4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_r47_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_middle_r47_80070D84_o2.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (CodeCandidate, "game/gs_thread.c"),
                    (Matching, "game/gs_thread_exact_800F036C.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_thread_exact_800F0424.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_thread_candidate_800F106C.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # GS VM native-call opcodes (0x800F10E8-0x800F1A0C), carved from
            # the fn_800F106C candidate: fn_800F106C is hand-written assembly
            # and stays in that unit. .text-only units. The GS VM TU is
            # GC/1.3.2 -O4,p: its pooled-string handlers (fn_800F1A0C,
            # fn_800F1E38, fn_800F6D18) address the pool's first message as
            # "addi rX,r31,0", which GC/1.3 emits as "mr rX,r31".
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_vm_exact_800F10E8.c"),
                    (Matching, "game/gs_vm_exact_800F13D0.c"),
                    (Matching, "game/gs_vm_exact_800F16C0.c"),
                ]
            ],
            # The GS VM translation unit (see include/game/gs_vm.h), linked
            # over 0x800F1A0C-0x800F75FC with the string pool and .sdata2
            # literals it owns. The .text-only units before and after it are
            # linked pieces of the same TU. -rostr puts the pool in .rodata.
            Object(
                Matching,
                "game/gs_vm.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_res.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # GSmsg (0x800F96E4-0x800FE35C) is one retail translation unit scored
            # through text-range units. It was built with the default GC/1.3
            # -O4,p flags and the peephole optimizer off: with -opt nopeephole
            # every one of its 36 functions scores equal or higher than with
            # the same source under plain -O4,p (none lower), and GSmsgInit,
            # GScharCpy, GScharLenCpy, GScharCmp, GSmsgDaemon, GSmsgFontOpen and
            # fn_800FBE7C only become exact with it. The former per-unit
            # GC/1.2.5n, -O3, -O4,s and -O1/-schedule settings were legacy
            # guesses. The 0x800F9D04 (GScharMakeFromSJIS..GScharCpy),
            # GScharCmp, GSmsgGetGSchar, GSmsgGetLength, GSmsgInit and
            # 0x800FC1D0 (GSmsgClose..GSmsgSetCtrlFunc) units compile only
            # their own text range from gs_msg.c and are linked.
            *[
                Object(
                    Matching
                    if path
                    in (
                        "game/gs_msg_exact_800F96E4.c",
                        "game/gs_msg_exact_800F9D04.c",
                        "game/gs_msg_r56b_800F9EE4_o2.c",
                        "game/gs_msg_candidate_800FA280_gc125.c",
                        "game/gs_msg_candidate_800FA314.c",
                        "game/gs_msg_exact_800FC1D0.c",
                        "game/gs_msg_r58b_800FC528_o1.c",
                        "game/gs_msg_exact_800FBF10.c",
                        "game/gs_msg_exact_800FDF1C.c",
                        "game/gs_msg_exact_800FDFE4.c",
                    )
                    else CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/gs_msg_exact_800F96E4.c",
                    "game/gs_msg_r56b_800F9AEC.c",
                    "game/gs_msg_exact_800F9D04.c",
                    "game/gs_msg_r56b_800F9EE4_o2.c",
                    "game/gs_msg_r56b_800FA064_suffix.c",
                    "game/gs_msg_candidate_800FA280_gc125.c",
                    "game/gs_msg_candidate_800FA314.c",
                    "game/gs_msg_candidate_r47_800FA3D0.c",
                    "game/gs_msg_candidate_800FA314_r46_800FB43C_o4s.c",
                    "game/gs_msg_r58b_800FB680_prefix.c",
                    "game/gs_msg_exact_800FBF10.c",
                    "game/gs_msg_r58b_800FBF74.c",
                    "game/gs_msg_exact_800FC1D0.c",
                    "game/gs_msg_r58b_800FC528_o1.c",
                    "game/gs_msg_r58b_800FC7E0_suffix.c",
                    "game/gs_msg_exact_800FDF1C.c",
                    "game/gs_msg_exact_800FDFE4.c",
                    "game/gs_msg_r58b_800FE010.c",
                ]
            ],
            # Sprite screen-environment TU (.text 0x800FE35C-0x800FE6DC with
            # its .sdata2 pool 0x8047CD50-0x8047CD80), linked whole; see the
            # file header for the unit-wide -opt nopeephole evidence.
            Object(
                Matching,
                "game/gs_thread_hi_range_800FE35C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gapp.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_thread_hi_range_800FEC34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006630.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80006724.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006884.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80006908.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006FAC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_task_residual_80007364.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_8000765C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_task_residual_80008868.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # people.c, the whole people TU: .text 0x801812C4-0x8018F470 with
            # its .rodata, .data, .sbss and .sdata2 (see the file header for
            # the evidence of its one flag set: GC/2.0 -O4,p, deferred
            # auto-inlining and read-only strings). Every function is exact;
            # the TU links as one object.
            Object(
                Matching,
                "game/people/people.c",
                mw_version="GC/2.0",
                extra_cflags=PEOPLE_TU_CFLAGS,
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-schedule off"]
                        if path == "game/gs_npc_event_candidate_8003037C_r40_800318D8.c"
                        else []
                    ),
                    progress_category="game",
                )
                for status, path, version in [
                    (Matching, "game/gs_npc_event_exact_80030170.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_800301B0.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_exact_80030370.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_8003042C_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80030574_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_800307A8_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_800308D4.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80030A44_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80030C14.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80030D34_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80030F0C_gc20.c", "GC/2.0"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80031188.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80031228_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80031404_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_80031648_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_npc_event_candidate_8003037C_r40_800318D8.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_800324A0.c", "GC/1.3"),
                    (CodeCandidate, "game/gs_npc_event_candidate_80032ED8.c", "GC/1.3"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/gs_npc_event_candidate_80031B4C_o3.c",
                mw_version="GC/1.3",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule on"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_npc_event_candidate_800327FC_o4s.c",
                mw_version="GC/2.0",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path, version in [
                    (CodeCandidate, "game/gs_pokemon_summary_r41_800167D0_gc20.c", "GC/2.0"),
                    (CodeCandidate, "game/gs_pokemon_summary_r41_80016ABC.c", "GC/1.3"),
                    (Matching, "game/gs_pokemon_summary_exact_800178EC.c", "GC/1.3"),
                ]
            ],
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_8001501C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r57b_800150E4_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r57b_800159BC_middle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r57b_80015E3C_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r57b_800161B0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r55_800166BC_gc13_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r55_8001793C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r55_80017CB8_gc13_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-schedule off", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_pokemon_summary_r55_80017E8C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/main_tail.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_texture.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_exact_800EF1E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_getters_exact_800EF3E0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_exact_800EF504.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_texture_exact_800EF548.c"),
                    (Matching, "game/gs_texture_exact_800EF578.c"),
                    (Matching, "game/gs_texture_exact_800EF5A4.c"),
                    (Matching, "game/gs_texture_candidate_800EF5FC.c"),
                    (Matching, "game/gs_texture_exact_800EFD14.c"),
                    (Matching, "game/gs_texture_exact_800EFFC0.c"),
                ]
            ],
            Object(
                Matching,
                "game/input/input_exact_800F75FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/input/input_candidate_800F760C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/input/input_exact_800F76E4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # One retail unit, 0x800F7758-0x800F915C. GC/1.3.2 because retail
            # keeps the pooled .bss `base + 0` add in fn_800F8138/fn_800F8428
            # (GC/1.3 folds it into a register copy); -inline deferred because
            # retail's pooled .bss layout (slots, timers, commands) is reverse
            # definition order, not first-reference order. Every function of
            # the unit is exact with this single setting and no pragmas;
            # deferred inlining emits functions in reverse, so the source
            # lists them high address first.
            Object(
                Matching,
                "game/input/input.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline deferred"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_gfx_exact_800D377C.c"),
                ]
            ],
            # GS render/graphics span 0x800D2B90-0x800D377C (fog setters,
            # camera projections, GSgfx frame functions) with its .sdata2
            # literal pool 0x8047C9F0-0x8047CA10. GC/2.5: fn_800D36B4
            # schedules its by-value colour loads ahead of the frame, which
            # GC/1.0-2.0 do not emit. See the file header.
            Object(
                Matching,
                "game/gs_gfx_range_800D2B90.c",
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_candidate_800D37D4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),  # GC/1.3.2: pooled static render-mode tables (see source header)
            Object(
                Matching,
                "hsd/dobj_exact_80198F7C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            # DObjLoad: user-approved rule exception (parameter copies); see
            # the source and docs/RULE_EXCEPTIONS.md.
            Object(
                Matching,
                "hsd/dobj_exact_801993A4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            Object(
                Matching,
                "hsd/dobj_exact_80199568.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            Object(
                Matching,
                "hsd/fobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            Object(
                Matching,
                "hsd/hsd_wobj.c",
                mw_version="GC/1.3",
                extra_cflags=["-O1", "-use_lmw_stmw on"],
                progress_category="hsd",
            ),
            # HAL sysdolphin fog.c, built with the library flags; owns its
            # .rodata/.data/.sdata2 (its zero colour stays in dtk's .sbss2).
            Object(
                Matching,
                "hsd/fog.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/id.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin initialize.c, built with the library flags; it
            # owns its .rodata/.data/.bss/.sdata/.sbss/.sdata2/.sbss2.
            Object(
                Matching,
                "hsd/initialize.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_object.c",
                mw_version="GC/1.3",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "dolphin/os/OSMemory_exact_8009F1B8.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSMemory_privileged.c",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/gs_gfx_core.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_800D3FA4 is its own TU (its .sdata2 literal pool
            # 0x8047CA20-0x8047CA30 sits between the 0x800D2DE8-0x800D3E4C
            # pool and fn_800D55D0's), built without copy propagation:
            # retail re-tests bit 0 of the second-pass flags after masking it.
            # The possible TU neighbours gs_gfx_core, 45F8 and 4F98 are
            # byte-identical under this flag.
            # RULE-EXCEPTION(title-path): per-unit compiler flag chosen because it matches - see docs/RULE_EXCEPTIONS.md
            Object(
                Matching,
                "game/gs_gfx_candidate_800D3FA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D45F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_800D461C owns its compiler-emitted switch table (.data
            # 0x80314188, formerly game/data/data_80314188.c).
            Object(
                Matching,
                "game/gs_gfx_candidate_800D461C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D4F98.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),  # GC/1.3.2 + -rostr: pooled .rodata request-buffer messages
            # fn_800D55D0/fn_800D5648 own their literal pool (.sdata2
            # 0x8047CA30-0x8047CA40: 0.0f, 42.5f, 6.0f).
            Object(
                Matching,
                "game/gs_gfx_candidate_800D55D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D56C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of fn_800D67BC (no jump tables or pooled
            # constants; data extern), on the neighbouring gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_candidate_800D67BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D6A00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_800D6B00: GC/1.3.2 with its pooled save-slot .bss defined
            # in the unit (retail passes the first slot as "addi r3,r31,0x0");
            # -inline deferred gives retail's .bss order (reverse definition
            # order, as in pslist.c). See the source header.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D6B00.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline deferred"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_gfx_layer_exact_800D7230.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D76A8.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7820.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D7894.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7940.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7A70.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D7D90.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7E5C.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D85D4.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D87AC.c"),
                ]
            ],
            # Byte-exact with tagged rule exceptions (docs/RULE_EXCEPTIONS.md).
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D892C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of fn_800D923C (compare-tree switch, no
            # jump tables or pooled constants; data extern), plain gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D923C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of fn_800D963C (compare-tree switch, no
            # jump tables or pooled constants; data extern), plain gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D963C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_range_800D9AF0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D9D68.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # The gs_gfx dl TU (0x800DA578 - 0x800DB890) linked whole with its
            # four switch tables (.data 0x803152B8 - 0x80315384): the last
            # table starts 4-aligned, so compiled tables cannot be carved.
            # _dlParseVertex uses opt_lifetimes off and GSgfxDLBegin a
            # single-use helper. RULE-EXCEPTION(title-path): see
            # docs/RULE_EXCEPTIONS.md.
            Object(
                Matching,
                "game/gs_gfx_dl.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_range_800DB890.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_backfb.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_light_exact_800DC560.c"),
                    (Matching, "game/gs_light_candidate_800DC6D8.c"),
                    (Matching, "game/gs_light_exact_800DC874.c"),
                    (Matching, "game/gs_light_candidate_800DC878.c"),
                    (Matching, "game/gs_light_exact_800DCA10.c"),
                    (Matching, "game/gs_light_exact_800DCC3C.c"),
                    (Matching, "game/gs_light_candidate_800DCC84.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_log.cpp",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_material_800DEFC8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_material.c"),
                    (Matching, "game/gs_material_exact_800DF11C.c"),
                    (CodeCandidate, "game/gs_material_candidate_800DF140.c"),
                    (Matching, "game/gs_material_exact_800DF1B8.c"),
                    (Matching, "game/gs_material_exact_800DF21C.c"),
                    (Matching, "game/gs_material_exact_800DF240.c"),
                    (CodeCandidate, "game/gs_material_candidate_800DF248.c"),
                    (Matching, "game/gs_material_exact_800DF470.c"),
                    (Matching, "game/gs_material_exact_800DF498.c"),
                    (Matching, "game/gs_material_candidate_800DFABC.c"),
                    (Matching, "game/gs_material_exact_800DFE98.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_math_vec.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_mtx.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_mtx44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_quat.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_init.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_bezier.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_lerp.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_math_range_800E09E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0BA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_candidate_800E0C04.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0C54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0CA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_util_exact_800E0D24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    Matching if path == "game/gs_range_800E0DDC_r58_prefix.c" else CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if path == "game/gs_range_800E0E14_r58_o1.c"
                        else None
                    ),
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-O2"] if path == "game/gs_range_800E0E14_r58_o1.c" else []),
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/gs_range_800E0DDC_r58_prefix.c",
                    "game/gs_range_800E0E14_r58_o1.c",
                ]
            ],
            Object(
                CodeCandidate,
                "game/menuNameEntry_r56_80026370_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_r56_800268F0_gc20p1_o4s.c",
                mw_version="GC/2.0p1",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-inline deferred",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_candidate_80026B44_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline deferred"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_candidate_80026FEC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_candidate_800275F4_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_r51_80027740_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_r51_80027AA4_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O2", "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuNameEntry_r51_80027D58_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuShop.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemonChange_r51_8002DD24_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemonChange_r51_8002DF10_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O2", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemonChange_r41_8002E460_gc125n.c",
                mw_version="GC/1.2.5n",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemonChange_r41_8002EA5C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_candidate_800205C8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-O1",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/2.0"
                        if path == "game/gs_title_candidate_80020EA4.c"
                        else "GC/1.3"
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_title_exact_80020328.c"),
                    (Matching, "game/gs_title_exact_800203B4.c"),
                    (Matching, "game/gs_title_exact_8002049C.c"),
                    (Matching, "game/gs_title_exact_8002058C.c"),
                    (Matching, "game/gs_title_exact_800205B8.c"),
                    (Matching, "game/gs_title_exact_8002060C.c"),
                    (CodeCandidate, "game/gs_title_candidate_80020618.c"),
                    (Matching, "game/gs_title_exact_8002091C.c"),
                    (Matching, "game/gs_title_exact_80020E9C.c"),
                    (Matching, "game/gs_title_candidate_80020EA4.c"),
                    (Matching, "game/gs_title_exact_80020F54.c"),
                    (CodeCandidate, "game/gs_title_candidate_800210F0.c"),
                    (Matching, "game/gs_title_exact_800215C4.c"),
                    (Matching, "game/gs_title_exact_80021624.c"),
                    (Matching, "game/gs_title_exact_80021644.c"),
                    (Matching, "game/gs_title_exact_800216E0.c"),
                    (Matching, "game/gs_title_r51_800218BC_suffix.c"),
                    (Matching, "game/gs_title_exact_80021A9C.c"),
                    (CodeCandidate, "game/gs_title_candidate_80021B14.c"),
                    (Matching, "game/gs_title_exact_80022050.c"),
                    (Matching, "game/gs_title_exact_80022E54.c"),
                    (CodeCandidate, "game/gs_title_candidate_80023068.c"),
                    (Matching, "game/gs_title_exact_80023274.c"),
                    (CodeCandidate, "game/gs_title_candidate_800232F0.c"),
                    (Matching, "game/gs_title_exact_80023DA8.c"),
                    (CodeCandidate, "game/gs_title_candidate_80023E60.c"),
                    (Matching, "game/gs_title_exact_80024438.c"),
                    (CodeCandidate, "game/gs_title_candidate_800246FC.c"),
                    (CodeCandidate, "game/gs_title_candidate_80024CDC.c"),
                    (Matching, "game/gs_title_exact_800259B0.c"),
                    (CodeCandidate, "game/gs_title_candidate_80025A80.c"),
                    (Matching, "game/gs_title_exact_80025F74.c"),
                    (CodeCandidate, "game/gs_title_candidate_80025F84.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/gs_title_r51_800216E8_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_title_r49_80024DBC_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_title_r49_80025730_o2.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_title_candidate_8002217C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_xfb_capture.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/gs_spline.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/camera_exact_801765F4.c"),
                    (Matching, "game/camera_exact_801766A8.c"),
                    (Matching, "game/camera_exact_801768F0.c"),
                    (Matching, "game/camera_exact_80176948.c"),
                    (Matching, "game/camera_exact_801769E4.c"),
                    (Matching, "game/camera_exact_80176B48.c"),
                    (Matching, "game/camera_exact_80176C04.c"),
                    (Matching, "game/camera_exact_80176F68.c"),
                    (Matching, "game/camera_exact_80176F98.c"),
                    (Matching, "game/camera_exact_80177004.c"),
                    (Matching, "game/camera_candidate_8017707C.c"),
                    (Matching, "game/camera_exact_801773F4.c"),
                    (Matching, "game/camera_exact_80177478.c"),
                    (Matching, "game/camera_exact_801778B4.c"),
                    (Matching, "game/camera_candidate_801779B0.c"),
                    (Matching, "game/camera_get_active_exact_801779EC.c"),
                    (Matching, "game/camera_candidate_80177A38.c"),
                    (Matching, "game/camera_scene_set_mode_exact_80177A44.c"),
                    (Matching, "game/camera_exact_8017865C.c"),
                    (Matching, "game/camera_exact_80179DFC.c"),
                ]
            ],
            # Chunks still below the policy bar compile the whole-TU candidate
            # game/camera.c with the camera unit's flags (see its header).
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline auto,deferred", "-str reuse,readonly"],
                    progress_category="game",
                )
                for path in [
                    "game/camera_candidate_80176C78.c",
                    "game/camera_candidate_80177A64.c",
                    "game/camera_candidate_801786F4.c",
                    "game/camera_candidate_80179E04.c",
                ]
            ],
            # fn_80179F4C: level-0 code (parameter homed on the stack), exact
            # with the unit-wide `-opt level=0` and no local pragmas; carved
            # from the fn_80179FA4 candidate below.
            Object(
                Matching,
                "game/gs_range_80179F4C_exact_80179F4C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fn_80179FA4: level-0 like the rest of fsys, and built with GC/2.0
            # like the fsys read units after it (gs_range_8017A5FC_prefix
            # onward): GC/1.3 colours its DMA-setup temporaries differently,
            # while GC/1.3.2 through 2.7 all emit the retail bytes.
            Object(
                Matching,
                "game/gs_range_80179F4C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-O1"] if path in (
                            "game/fsys/fsys_file_r48_8017B4BC_prefix.c",
                        ) else []),
                        *(["-opt level=0"] if path in (
                            "game/fsys/fsys_file.c",
                            "game/fsys/fsys_slot_8017B1CC.c",
                            "game/fsys/fsys_file_exact_8017B5C0.c",
                            "game/fsys/fsys_file_r48_8017B6B8_suffix.c",
                            "game/fsys/fsys_file_exact_8017BC90.c",
                            "game/fsys/fsys_file_r51_8017C5B8_prefix.c",
                            "game/fsys/fsys_file_candidate_8017C008.c",
                            "game/fsys/fsys_file_candidate_8017C074.c",
                            "game/fsys/fsys_file_candidate_8017C39C.c",
                            "game/fsys/fsys_file_exact_8017C1D8.c",
                            "game/fsys/fsys_file_candidate_8017C414.c",
                            "game/fsys/fsys_file_candidate_8017C894.c",
                            "game/fsys/fsys_file_candidate_8017C8C8.c",
                            "game/fsys/fsys_file_candidate_8017D3D4.c",
                        ) else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fsys/fsys_file.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017B1AC.c"),
                    (Matching, "game/fsys/fsys_slot_8017B1CC.c"),
                    (CodeCandidate, "game/fsys/fsys_file_r48_8017B4BC_prefix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017B5C0.c"),
                    (Matching, "game/fsys/fsys_file_r48_8017B6B8_suffix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017BC90.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017BFE8.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C008.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C074.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C1D8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C394.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C39C.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C414.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C568.c"),
                    (Matching, "game/fsys/fsys_file_r51_8017C5B8_prefix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C88C.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C894.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C8C0.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C8C8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C8F4.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017CEC8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017D3A0.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017D3D4.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017D400.c"),
                ]
            ],
            Object(
                Matching,
                "game/fsys/fsys_file_r51_8017C6E0_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fn_8017C8FC: level-0 code like the rest of fsys (it was scored
            # at -O4,s from a candidate include; exact at level 0).
            Object(
                Matching,
                "game/fsys/fsys_file_r49_8017C8FC_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Status-2000 handler, level-0 code like the rest of fsys: exact
            # only with the unit-wide `-opt level=0` (73% without it).
            Object(
                Matching,
                "game/fsys/fsys_file_r49_8017CE7C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_r48_8017BD34_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Like the rest of the fsys code, 0x8017D410 - 0x8017D960 is
            # optimisation-level-0 code: every function in the unit is exact
            # with the unit-wide `-opt level=0` and no local pragmas.
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017D410_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017D960_o4s_inline_noauto.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Level-0 code like the rest of fsys: exact only with the
            # unit-wide `-opt level=0` (66% without it).
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017DAB8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017DB74_gc20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # 0x8017DEA4 - 0x8017E30C: exact under the same unit-wide
            # `-opt level=0` (parameters homed on the stack, no pragmas).
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017DEA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fn_8017E30C: exact on the fsys level-0 flags. Its
            # `compressed = NULL` after the free is the unit's free-then-clear
            # idiom (see the file header for the evidence).
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017E30C_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # _fsysGetFilename on the fsys unit's level-0 flags; its "%s.fsys"
            # literal is in .sdata2, i.e. strings are read-only (the unit's
            # "gsfsys.toc" name is in .rodata too).
            Object(
                Matching,
                "game/fsys/fsys_file_exact_8017EB6C.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt level=0",
                    "-str reuse,readonly",
                ],
                progress_category="game",
            ),
            # DVD/ARQ completion callbacks, level-0 code like the rest of
            # fsys: both exact only with the unit-wide `-opt level=0`.
            Object(
                Matching,
                "game/fsys/fsys_file_r56_8017F108_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017CED8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/people/people_exact_8018F470.c"),
                    (Matching, "game/people/people_candidate_8018F4C8.c"),
                    (Matching, "game/people/people_exact_8018F5B4.c"),
                    (CodeCandidate, "game/people/people_candidate_8018F730.c"),
                    (Matching, "game/people/people_exact_8018FB2C.c"),
                    (Matching, "game/people/people_exact_8018FB94.c"),
                    (Matching, "game/people/people_exact_8018FBAC.c"),
                    (Matching, "game/people/people_exact_8018FC50.c"),
                    (Matching, "game/people/people_exact_8018FC74.c"),
                    (Matching, "game/people/people_candidate_8018FCE0.c"),
                    (Matching, "game/people/people_exact_8018FD88.c"),
                ]
            ],
            Object(
                CodeCandidate,
                "game/menuPokemon_r57_800181C4_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_r57_80019B48_gc20p1_o4s.c",
                mw_version="GC/2.0p1",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_r57_80019D5C_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_r57_80019F6C_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_candidate_8001C064.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_r50_8001C7B8_prefix.c",
                mw_version="GC/2.0",
                cflags=["-O0" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menuPokemon_r50_8001D378_o3.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O1", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r50_8001D624_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuSub TU (0x8001D718-0x8001EF78) with its .sdata2 pool, the
            # fn_8001E644 colour initializer and the fn_8001EC08 work area.
            Object(
                Matching,
                "game/menuSub.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # HAL sysdolphin cobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.sdata/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/cobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin displayfunc.c, built with the library flags; it
            # owns its .rodata/.data/.bss/.sdata/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/displayfunc.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin jobj.c, the whole translation unit, built with the
            # library flags; it owns its .rodata (IK vectors and string pool),
            # .data (hsdJObj and JObjUpdateFunc's switch table), .sbss and
            # .sdata2.
            Object(
                Matching,
                "hsd/jobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin list.c, built with the library flags; it owns its
            # .bss/.sdata2.
            Object(
                Matching,
                "hsd/list.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin lobj.c, built with the library flags; it owns
            # its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/lobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version=version,
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o1
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on"],
                    progress_category="hsd",
                )
                for path, version, use_o1 in [
                ]
            ],
            Object(
                Matching,
                "hsd/memory.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # HAL sysdolphin memory.c, library flags; owns its .rodata/.bss
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_dvd_candidate_80167040.c"),
                    (Matching, "game/gs_dvd_exact_80167E54.c"),
                    (Matching, "game/gs_dvd_candidate_80167E64.c"),
                    (Matching, "game/gs_dvd_exact_80167FA4.c"),
                    (Matching, "game/gs_dvd_r47_prefix.c"),
                ]
            ],
            # Function-boundary carve of fn_80168638 (no jump tables or pooled
            # constants; data extern), same GC/1.3 flags as the prefix owner.
            Object(
                Matching,
                "game/gs_dvd_r47_80168638_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF0A0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF178.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF4D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF58C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF660.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF730.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_r48_8007C300_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_r48_8007C7EC_o2.c",
                mw_version="GC/2.0",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_r48_8007CAB0_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_matrix_candidate_8007CBB4.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_matrix_candidate_8007D4FC_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/menu/menu_carde_matrix_candidate_8007D978.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/tracefx_r55_801364A8_gc13_o1.c",
                mw_version="GC/1.3",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                CodeCandidate,
                "game/effect/tracefx_r55_80137114_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # HAL sysdolphin shadow.c, built with the library flags; it owns
            # its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/shadow.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin util.c and video.c, built with the library
            # flags; each owns its data.
            Object(
                Matching,
                "hsd/util.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL video.c owns its complete text and literal/BSS pools. The
            # locally tagged Melee-style dont_inline keeps the draw-done flag
            # accessor out of line for HSD_VICopyXFBAsync.
            Object(
                Matching,
                "hsd/video.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin spline.c (.text 0x801B1890-0x801B25C4, .sdata2
            # 0x8047DE00-0x8047DE50) on the library-wide HSD flags.
            Object(
                Matching,
                "hsd/spline.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin state.c, the whole translation unit, on the
            # library-wide HSD flags; it owns the channel set-ups (.data),
            # dark_matter (.sdata) and its float pool (.sdata2).
            Object(
                Matching,
                "hsd/state.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin tev.c, one retail TU on the library-wide HSD flags.
            Object(
                Matching,
                "hsd/tev.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin texp.c / texpdag.c, each one retail TU built with
            # the library-wide HSD flags (GC/2.5, optimizer level 1 with
            # scheduling, deferred auto-inlining, lmw/stmw, read-only strings).
            Object(
                Matching,
                "hsd/texp.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/texpdag.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin aobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/aobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSCache_privileged_prefix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSCache_exact_8009B4D8.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSCache_privileged_suffix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSCache_l2_8009B628.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5624.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5784.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A5810.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A58BC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A58F0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5918.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A59CC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5C60.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5CC8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5D60.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/dvd/DVD_range_800A5D88.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5EE0.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/dvd/DVD_range_800A60D4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A62CC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A63C8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A640C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A6508.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A6578.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/dvd/DVD_range_800A6684.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A7484.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A76E4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A7728.c",
                progress_category="sdk",
            ),
            Object(
                CodeCandidate,
                "dolphin/os/OSReset.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "crt/exit.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/global_destructor_chain_exact_800C4668.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                CodeCandidate,
                "crt/global_destructor_chain_candidate_800C46B0.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKConstructEvent.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBufferReset.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKGetBuffer.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTerminateSerialHandler.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/usr_put_initialize.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchInit.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchConnected.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchMutex.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTargetState.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTargetStopped.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBoard.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKCommState.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKComm_exact_800C39BC.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/udp_cc.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/ddh_cc_close.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/ddh_cc_shutdown.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/circle_buffer_count.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/gdev_cc_close.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/gdev_cc_shutdown.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C580.c",
                source="crt_data/sdata2_math_8047C580.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/data/rodata_80267060.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_802663A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266BD8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266C30.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80266C7C.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80266D78.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267250.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267350.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80267398.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80268424.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026860C.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026F640.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026F818.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026FB94.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026FD4C.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_8026FE58.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80270008.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80270440.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80270528.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80270EE8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80271300.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80271E10.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80272200.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_802729C0.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80272B08.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80273548.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80273A00.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_802741F8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            # HAL sysdolphin debug.c from HSD_Panic on, built with the library
            # flags; HSD_SaveContext (0x80196CE0) is hand-written assembly in
            # HAL's source and stays in the generated assembly.
            Object(
                Matching,
                "hsd/debug.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80274708.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            # HAL sysdolphin hash.c and id.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); each owns its own data.
            Object(
                Matching,
                "hsd/hash.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80274EC8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_802757F0.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80279320.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                DataCandidate,
                "game/data/rodata_80279AE8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/bss_8039A700.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_8039E700.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803A1F88.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803A2040.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803A6498.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803A9E40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803B6E40.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803D6E40.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_803F6E40.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80402480.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80404BF0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80408400_prefix.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80435FF8.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80446F10.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80448440.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_8044FB90.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80452500.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80452EC8.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80455070.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80465378.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80465710.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80466DE8.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_80467378.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_8046D500.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/bss_804787E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8027A500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E28F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4B98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4D90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4DB0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E51C8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802ED9F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EDE54.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EE458.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EE608.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EF0A8.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_8030FFE4.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803107E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80310CD8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311868.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803119F0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80311AD4.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311D38.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80311E54.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312558.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803125E8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803127F0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312C40_suffix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80313590.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803137E0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80313824.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80313B18.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80313F48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80314350.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80315490.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803155D0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80315690.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B088.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B430.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B468.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B8A0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_8035B904.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_8035B96C.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B9F8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BA48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BB50.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BBA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035C430.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035E940.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_803634EC.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803635C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363774.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363B18.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363CA8_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80367E70.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803692C8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80369D20.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C248.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C2A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C568.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C7A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036CC00.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036CFA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036DCB8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036E030.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036E150.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803751F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80375230.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_803754AC.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80375938.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80375F98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803760D8.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80376FCC.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80377B0C.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803784C0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_803786D0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80378794.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803788C0.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80378A7C.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80378B00.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80378D14.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80379388.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_8037943C.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_803794CC.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80379714.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80379A3C.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_80379BF4.c",
                progress_category="game",
            ),
            Object(
                DataCandidate,
                "game/data/data_8038FFFC.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A048_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A088.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A538.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A648.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A6A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B6B8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B7A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B808.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B8A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B9A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BA58.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BAA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BBA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BCA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BDA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BEA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C0A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C1A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C280.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C2A0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C318.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C370_suffix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C3A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C408.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C418.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C8A0.c",
                source="crt_data/sdata2_math_8047C8A0.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C970.c",
                source="crt_data/sdata2_math_8047C970.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/gs_render_util_sdata2.c",
                source="game/gs_render_util_sdata2.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C9A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C9B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA10.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA50.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA70.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA88.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CAC8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB60.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CBE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CC90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CC98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CD00.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_sdata2_8047CDC0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CDE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CE70.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CE98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CF08.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CF48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CFD0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D028.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D098.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D110.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D198.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D298.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D3B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D3C0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D528.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D560.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D5C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D720.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D8A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DA18.c",
                progress_category="hsd",
            ),
            # HAL sysdolphin random.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .sdata and .sdata2.
            Object(
                Matching,
                "hsd/random.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DCA0.c",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DF58.c",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047DF90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047DFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E0A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E180.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E190.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E1E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_waza_sdata2_8047E290.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E390.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E4B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E508.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E530.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E538.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E628.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/colosseum_battle_sdata2.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/colosseum_battle_sdata2_8047E6E8.c",
                progress_category="game",
            ),
        ],
    ),
    # REL 125 (common_rel, common.fsys member 0)
    Rel(
        "common_rel",
        [
            Object(Matching, "rel/common_rel/common_rel.c"),
            Object(Matching, "rel/common_rel/snd_song_table.c"),
            Object(Matching, "rel/common_rel/snd_sample_table.c"),
        ],
    ),
]

config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "Dolphin SDK Code"),
    ProgressCategory("hsd", "HSD/sysdolphin (Third Party)"),
    ProgressCategory("musyx", "MusyX (Third Party)"),
    ProgressCategory("runtime", "Gekko Runtime Code"),
]
config.progress_each_module = args.verbose

if args.mode == "configure":
    generate_build(config)
elif args.mode == "progress":
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
