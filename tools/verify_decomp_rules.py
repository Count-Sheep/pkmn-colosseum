#!/usr/bin/env python3
"""Pre-push verification: every function matches, and every source construct
follows the project rules (AGENTS.md) or a tagged, listed exception
(docs/RULE_EXCEPTIONS.md, docs/asm_evidence/registry.json).

Checks, each reported as a finding class with a severity:

  fail  breaks a rule; exit status 1
  warn  needs a human look; exit status 1 only with --strict
  info  counted for the record (accepted exceptions, exemptions)

 1. match/link   every report function is 100% and its unit's code fully
                 matches; unlinked units are listed and classified (code vs
                 data-only).
 2. asm/.inc     no active `#include "*.inc"` and no tracked .inc file; every
                 live asm block is either registry-evidenced library/first-party
                 asm (evidence document, citation, mnemonics and branch targets
                 checked through .github/scripts/quality_scan.py and
                 verify_asm_branch_targets.py) or the allowlisted Dolphin
                 paired-single math.
 3. pragmas      every compiler-control #pragma in effect for a compiled
                 function, after the unit's -D flags, wrapper #defines and
                 carve guards are applied, carries a RULE-EXCEPTION tag directly
                 above it and a docs/RULE_EXCEPTIONS.md row. The rules mirror the
                 native port's acceptance check (Decomp.assess in
                 pkmn-colosseum-recomp/tools/boot_status.py).
 4. tags/rows    every RULE-EXCEPTION tag has a row (function or file); every
                 row names functions and files that exist.
 5. prototypes   calls with no prototype in scope, found by recompiling each
                 MWCC unit with -requireprotos (warning class).
 6. build        object-map freeze, retail SHA-1 (dtk shasum), report
                 freshness (ninja -n), tracked forbidden files, symbol renames
                 since origin/master.

Results are cached per unit under build/GC6E01/rules_cache/ (keyed by the
files each unit reads) and computed in parallel. The JSON report goes to
build/GC6E01/rules_report.json.

usage: python3 tools/verify_decomp_rules.py [--strict] [--jobs N] [--no-protos]
                                            [--no-sha1] [--no-cache] [-v]
"""

from __future__ import annotations

import argparse
import bisect
import contextlib
import hashlib
import io
import json
import os
import re
import shlex
import subprocess
import sys
import tempfile
import time
from collections import Counter, defaultdict
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / ".github" / "scripts"))
import quality_scan as qs  # noqa: E402  (asm allowlists, registry evidence rules)

VERSION = 2  # bump when the per-unit analysis changes, to drop cached results
BUILD = ROOT / "build" / "GC6E01"
REPORT = BUILD / "report.json"
OUT = BUILD / "rules_report.json"
CACHE_DIR = BUILD / "rules_cache"
RULES_DOC = ROOT / "docs" / "RULE_EXCEPTIONS.md"
REGISTRY = ROOT / "docs" / "asm_evidence" / "registry.json"
SYMBOLS = ROOT / "config" / "GC6E01" / "symbols.txt"
NINJA_FILE = ROOT / "build.ninja"

# ---------------------------------------------------------------------------
# Rules mirrored from pkmn-colosseum-recomp tools/boot_status.py (Decomp.assess)
# ---------------------------------------------------------------------------

# Pragmas that place data, scope state or change language mode rather than a
# function's code generation. Exemption class "pragma.structural".
NON_CONTROL_PRAGMAS = frozenset((
    "push", "pop", "section", "force_active", "weak", "pack", "cplusplus",
    "explicit_zero_data", "once", "mark", "message", "warning", "error",
    "options", "align",
))
COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)
DIRECTIVE = re.compile(r"^[ \t]*#[ \t]*(\w+)[ \t]*(.*)$")
INCLUDE_PATH = re.compile(r'^["<]([^">]+)[">]')
PRAGMA = re.compile(r"^[ \t]*#[ \t]*pragma[ \t]+(\w+)(.*)$", re.MULTILINE)
ASM = re.compile(r"\b(?:asm|__asm|__asm__)\b")
RULE_TAG = re.compile(r"RULE-EXCEPTION\(")
TAG_MARK = "@RULE_EXCEPTION@"
DEFAULT_DEFINES = {"BUILD_VERSION": "0", "VERSION_GC6E01": "1", "NDEBUG": "1"}
DEFAULT_INCLUDE_DIRS = ("include", "build/GC6E01/include")
C_KEYWORDS = {"if", "while", "for", "switch", "return", "sizeof", "do", "else", "case"}


def rel(path: str | Path) -> str:
    try:
        return Path(path).resolve().relative_to(ROOT).as_posix()
    except ValueError:
        return str(path)


def source_names(symbol: str) -> list[str]:
    """C sources may spell a mangled name literally or use its demangled base."""
    base = re.sub(r"__F.*$", "", symbol)
    return [symbol] if base == symbol else [symbol, base]


def c_condition(expression: str, macros: dict[str, str]) -> bool:
    """Evaluate a preprocessor condition. Unparseable conditions count as true,
    so an unsupported construct can never hide source from the checks."""
    expression = re.sub(r"\bdefined\s*(?:\(\s*(\w+)\s*\)|(\w+))",
                        lambda m: " 1 " if (m.group(1) or m.group(2)) in macros else " 0 ", expression)
    for _ in range(16):
        expanded = re.sub(r"\b[A-Za-z_]\w*\b",
                          lambda m: f" ({macros[m.group(0)] or '0'}) " if m.group(0) in macros else m.group(0),
                          expression)
        if expanded == expression:
            break
        expression = expanded
    expression = re.sub(r"\b(?!0[xX])[A-Za-z_]\w*\b", " 0 ", expression)
    expression = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", expression)
    expression = expression.replace("&&", " and ").replace("||", " or ")
    expression = re.sub(r"!(?!=)", " not ", expression).replace("/", "//")
    if re.search(r"[^\s\w()<>=!+\-*/%&|^~]", expression):
        return True
    try:
        return bool(eval(expression, {"__builtins__": {}}, {}))  # noqa: S307 - sanitized integer expression
    except Exception:
        return True


class Preprocessor:
    """The unit's source as MWCC sees it: comments blanked (RULE-EXCEPTION
    comments kept as TAG_MARK), inactive #if regions blanked, project .c
    includes expanded in place, and each header's pragmas kept where it is
    included. Every output line remembers the (file, line) it came from."""

    def __init__(self, macros: dict[str, str], dirs: list[Path]) -> None:
        self.macros = macros
        self.dirs = dirs
        self.read_files: set[str] = set()
        self.blanked: dict[str, list[str]] = {}
        self.header_texts: list[tuple[str, list[tuple[str, int]]]] = []
        self.inc_includes: list[tuple[str, int, str]] = []

    def _resolve(self, name: str, path: Path) -> Path | None:
        for base in (path.parent, ROOT, *self.dirs):
            candidate = base / name
            if candidate.is_file():
                return candidate
        return None

    def run(self, path: Path, stack: frozenset = frozenset()) -> tuple[str, list[tuple[str, int]]]:
        self.read_files.add(str(path))
        key = rel(path)

        def blank(match: re.Match[str]) -> str:
            spaced = re.sub(r"[^\n]", " ", match.group(0))
            if not RULE_TAG.search(match.group(0)):
                return spaced
            if len(spaced) >= len(TAG_MARK) and "\n" not in spaced[:len(TAG_MARK)]:
                return TAG_MARK + spaced[len(TAG_MARK):]
            return TAG_MARK + " " + spaced
        lines = COMMENT.sub(blank, path.read_text(encoding="utf-8", errors="replace")).split("\n")
        self.blanked[key] = lines
        stack = stack | {path}
        output: list[str] = []
        origins: list[tuple[str, int]] = []
        frames: list[list[bool]] = []  # [parent active, a branch was taken, this branch active]
        active = True
        index = 0
        macros = self.macros
        while index < len(lines):
            line, extra, number = lines[index], 0, index + 1
            mapped: list[tuple[str, int]] | None = None
            while line.endswith("\\") and index + extra + 1 < len(lines):
                extra += 1
                line = line[:-1] + " " + lines[index + extra]
            index += 1 + extra
            directive = DIRECTIVE.match(line)
            text = ""
            if directive:
                name, rest = directive.group(1), directive.group(2).strip()
                if name != "pragma":
                    rest = rest.replace(TAG_MARK, "").strip()
                if name in ("if", "ifdef", "ifndef"):
                    if name == "if":
                        taken = active and c_condition(rest, macros)
                    else:
                        taken = active and ((rest.split() or [""])[0] in macros) == (name == "ifdef")
                    frames.append([active, taken, taken])
                elif name == "elif" and frames:
                    frame = frames[-1]
                    frame[2] = frame[0] and not frame[1] and c_condition(rest, macros)
                    frame[1] |= frame[2]
                elif name == "else" and frames:
                    frame = frames[-1]
                    frame[2] = frame[0] and not frame[1]
                    frame[1] = True
                elif name == "endif" and frames:
                    frames.pop()
                elif active and name == "define":
                    match = re.match(r"(\w+)(\()?\s*(.*)", rest)
                    if match:
                        macros[match.group(1)] = "" if match.group(2) else match.group(3).strip()
                elif active and name == "undef":
                    macros.pop((rest.split() or [""])[0], None)
                elif active and name == "include":
                    target = INCLUDE_PATH.match(rest)
                    if target and target.group(1).endswith(".inc"):
                        self.inc_includes.append((key, number, target.group(1)))
                    resolved = self._resolve(target.group(1), path) if target else None
                    if resolved and resolved not in stack:
                        included, included_origins = self.run(resolved, stack)
                        if resolved.suffix in (".c", ".cpp"):
                            text, mapped = included, included_origins
                        else:
                            self.header_texts.append((included, included_origins))
                            kept = [(hl, origin) for hl, origin in zip(included.split("\n"), included_origins)
                                    if PRAGMA.match(hl)]
                            if kept:
                                text = "\n".join(hl for hl, _ in kept)
                                mapped = [origin for _, origin in kept]
                elif active and name == "pragma":
                    text = line
                active = frames[-1][2] if frames else True
            elif active:
                text = line
            origins.extend(mapped if mapped is not None else [(key, number)])
            output.append(text)
            output.extend([""] * extra)
            origins.extend((key, number + offset) for offset in range(1, extra + 1))
        return "\n".join(output), origins


def pragma_events(text: str) -> list[tuple[int, str, str]]:
    return [(m.start(), m.group(1), m.group(2).replace(TAG_MARK, "").strip()) for m in PRAGMA.finditer(text)]


def active_pragmas(events: list[tuple[int, str, str]], start: int, end: int) -> list[tuple[str, str, int]]:
    """Compiler controls in effect anywhere over [start, end) (push/pop and
    per-control reset honoured), each as (control, setting, offset)."""
    state: dict[str, list[tuple[str, int]]] = {}
    scopes: list[dict[str, list[tuple[str, int]]]] = []
    found: dict[tuple[str, str, int], None] = {}
    entered = False

    def snapshot() -> None:
        for control, settings in sorted(state.items()):
            if settings:
                found[(control, *settings[-1])] = None
    for offset, name, rest in events:
        if offset >= end:
            break
        if offset >= start and not entered:
            snapshot()
            entered = True
        if name == "push":
            scopes.append({control: list(settings) for control, settings in state.items()})
        elif name == "pop":
            if scopes:
                state = scopes.pop()
                if entered:
                    snapshot()
        elif name not in NON_CONTROL_PRAGMAS:
            settings = state.setdefault(name, [])
            if rest.startswith("reset"):
                if settings:
                    settings.pop()
                if entered and settings:
                    found[(name, *settings[-1])] = None
            else:
                settings.append((rest, offset))
                if entered:
                    found[(name, rest, offset)] = None
    if not entered:
        snapshot()
    return list(found)


def tagged_line(lines: list[str], number: int) -> bool:
    """A RULE-EXCEPTION tag on the pragma's line, or heading the run of blank,
    tag and #pragma lines directly above it. A directive (#if), code or an
    untagged non-pragma line in between ends the run."""
    if not lines or not 1 <= number <= len(lines):
        return False
    if TAG_MARK in lines[number - 1]:
        return True
    for line in reversed(lines[:number - 1]):
        if TAG_MARK in line:
            return True
        if line.strip() and not PRAGMA.match(line):
            return False
    return False


FUNC_PARAMS = r"(?:[^;{}()]|\([^;{}()]*\))*"
KR_DECLS = r"(?:\s*[^;{}()]+;)*"


def function_body(text: str, name: str) -> tuple[int, int] | None:
    """(start, end) offsets of name's definition in preprocessed text."""
    pattern = re.compile(rf"[^\n;#]*\b{re.escape(name)}\s*\({FUNC_PARAMS}\){KR_DECLS}\s*\{{")
    for occurrence in re.finditer(rf"\b{re.escape(name)}\s*\(", text):
        line_start = text.rfind("\n", 0, occurrence.start()) + 1
        match = pattern.match(text, line_start)
        if not match or match.start() != line_start:
            continue
        depth = 0
        for index in range(match.end() - 1, len(text)):
            if text[index] == "{":
                depth += 1
            elif text[index] == "}":
                depth -= 1
                if depth == 0:
                    return match.start(), index + 1
        return match.start(), len(text)
    return None


def definition_names(macros: dict[str, str], symbol: str) -> list[str]:
    """Spellings whose definition compiles to symbol (object-like #define aliases)."""
    def expand(name: str) -> str:
        seen = set()
        while name in macros and name not in seen and re.fullmatch(r"[A-Za-z_]\w*", macros[name]):
            seen.add(name)
            name = macros[name]
        return name
    targets = source_names(symbol)
    spellings = [name for name in targets if expand(name) == name]
    spellings += sorted(name for name in macros if name not in targets and expand(name) in targets)
    return spellings


def analyse_unit(job: dict[str, Any]) -> dict[str, Any]:
    """Worker: preprocess one unit and report its pragma sites, asm tokens,
    .inc includes and each function's location and controls in effect."""
    source = ROOT / job["source"]
    pre = Preprocessor(dict(job["macros"]), [ROOT / d for d in job["dirs"]])
    result: dict[str, Any] = {"read_files": [], "inc": [], "asm": [], "events": [], "functions": {}}
    if not source.is_file():
        result["error"] = f"source {job['source']} is missing"
        return result
    text, origins = pre.run(source)
    line_starts = [0, *(m.end() for m in re.finditer("\n", text))]

    def origin(offset: int, origin_list=origins, starts=line_starts) -> tuple[str, int]:
        line = bisect.bisect_right(starts, offset) - 1
        return tuple(origin_list[line]) if 0 <= line < len(origin_list) else (job["source"], 0)

    events = pragma_events(text)
    tag_cache: dict[tuple[str, int], bool] = {}

    def tagged(where: tuple[str, int]) -> bool:
        if where not in tag_cache:
            tag_cache[where] = tagged_line(pre.blanked.get(where[0], []), where[1])
        return tag_cache[where]

    for offset, name, rest in events:
        if name in ("push", "pop") or rest.startswith("reset"):
            continue
        where = origin(offset)
        result["events"].append([name, rest, where[0], where[1], tagged(where)])

    # Live asm tokens, in the unit text and in included headers (inline bodies).
    seen_asm = set()
    for body, body_origins in [(text, origins), *pre.header_texts]:
        starts = [0, *(m.end() for m in re.finditer("\n", body))]
        for match in ASM.finditer(body):
            where = origin(match.start(), body_origins, starts)
            if where not in seen_asm:
                seen_asm.add(where)
                result["asm"].append(list(where))

    for name in job["functions"]:
        spellings = definition_names(pre.macros, name)
        address = job["addresses"].get(name, 0)
        if address:
            # Local functions the report names by their source name may be
            # spelled name_XXXXXXXX in a carve, and fn_ aliases may be the
            # registry's real name for an asm routine.
            spellings += [f"{base}_{address:08X}" for base in source_names(name)]
        spellings += job["aliases"].get(name, [])
        found = None
        for spelling in spellings:
            span = function_body(text, spelling)
            if span:
                found = (text, origins, line_starts, span, events)
                break
        if found is None:
            for body, body_origins in pre.header_texts:
                for spelling in spellings:
                    span = function_body(body, spelling)
                    if span and re.search(r"\binline\b", body[span[0]:body.index("{", span[0])]):
                        starts = [0, *(m.end() for m in re.finditer("\n", body))]
                        found = (body, body_origins, starts, span, pragma_events(body))
                        break
                if found:
                    break
        if found is None:
            result["functions"][name] = {"located": False, "spellings": spellings, "pragmas": []}
            continue
        body, body_origins, starts, (start, end), body_events = found
        where = origin(start, body_origins, starts)
        pragmas = []
        for control, value, offset in active_pragmas(body_events, start, end):
            site = origin(offset, body_origins, starts)
            pragmas.append([control, value, site[0], site[1], tagged(site)])
        result["functions"][name] = {"located": True, "file": where[0], "line": where[1],
                                     "spellings": spellings, "pragmas": pragmas}
    result["inc"] = [list(item) for item in pre.inc_includes]
    result["read_files"] = sorted(rel(p) for p in pre.read_files)
    return result


# ---------------------------------------------------------------------------
# Inputs: report, build.ninja, ninja deps, symbols, exceptions table
# ---------------------------------------------------------------------------

def parse_ninja() -> dict[str, dict[str, Any]]:
    """Compile edges of build.ninja: object -> {rule, source, vars}."""
    text = re.sub(r"\$\n[ \t]*", "", NINJA_FILE.read_text(encoding="utf-8", errors="replace"))
    edges: dict[str, dict[str, Any]] = {}
    current = None
    for line in text.split("\n"):
        if line.startswith("build "):
            match = re.match(r"build (.+?): (\S+)\s*(.*)$", line)
            current = None
            if not match:
                continue
            outputs, rule, rest = match.groups()
            if not (rule.startswith("mwcc") or rule == "prodg"):
                continue
            inputs = rest.split("|")[0].split()
            current = {"rule": rule, "source": inputs[0] if inputs else "", "vars": {}}
            for output in outputs.split():
                if output.endswith(".o"):
                    edges[output] = current
        elif current is not None and line.startswith("  "):
            key, _, value = line.strip().partition(" = ")
            current["vars"][key] = value
        else:
            current = None
    return edges


def ninja_deps() -> dict[str, list[str]]:
    try:
        out = subprocess.run(["ninja", "-t", "deps"], cwd=ROOT, capture_output=True, text=True, timeout=120).stdout
    except (OSError, subprocess.SubprocessError):
        return {}
    deps: dict[str, list[str]] = {}
    current = None
    for line in out.splitlines():
        if line and not line.startswith(" "):
            current = line.split(":", 1)[0]
            deps[current] = []
        elif current and line.strip():
            deps[current].append(rel(ROOT / line.strip()))
    return deps


def compile_flags(cflags: str) -> tuple[dict[str, str], list[str], list[str]]:
    args = shlex.split(cflags)
    macros: dict[str, str] = {}
    dirs: list[str] = []
    for index, arg in enumerate(args):
        if arg.startswith("-D"):
            spec = arg[2:] or (args[index + 1] if index + 1 < len(args) else "")
            name, _, value = spec.partition("=")
            macros[name] = value or "1"
        elif arg in ("-i", "-I", "-ir") and index + 1 < len(args):
            dirs.append(args[index + 1])
        elif arg.startswith("-I") and len(arg) > 2:
            dirs.append(arg[2:])
    return macros or dict(DEFAULT_DEFINES), dirs or list(DEFAULT_INCLUDE_DIRS), args


def load_symbols(text: str) -> dict[str, tuple[str, str]]:
    """symbols.txt name -> (section:address, type)."""
    symbols = {}
    for match in re.finditer(r"(?m)^([^\s=]+)\s*=\s*([.\w]+:0x[0-9A-Fa-f]+);[^\n]*?(?:type:(\w+))?[^\n]*$", text):
        symbols[match.group(1)] = (match.group(2).upper(), match.group(3) or "")
    return symbols


def split_row(line: str) -> list[str]:
    return [cell.strip() for cell in line.strip().strip("|").split("|")]


def load_rows() -> tuple[str, list[dict[str, Any]]]:
    text = RULES_DOC.read_text(encoding="utf-8")
    rows = []
    header_seen = False
    for number, line in enumerate(text.splitlines(), 1):
        if not line.startswith("|"):
            continue
        cells = split_row(line)
        if cells and cells[0] == "Function(s)":
            header_seen = True
            continue
        if not header_seen or set(line) <= set("|-: "):
            continue
        rows.append({"line": number, "cells": cells, "functions": cells[0] if cells else "",
                     "files": cells[1] if len(cells) > 1 else ""})
    return text, rows


PATH_TOKEN = re.compile(r"(?<![\w/.])((?:src|include|config|tools|docs|\.github)/[\w./$-]+\.\w+|configure\.py)")
IDENT = re.compile(r"(?<![\w$./])([A-Za-z_][\w$]*)")
ENGLISH = set("""and or with the in of as a an to for via on at by from its it is unit linked unlinked carve
candidate candidates combined range exact suffix prefix row lane only text all other plus helpers helper
inline inlines callers caller callback functions function file files see etc same both each also""".split())


def row_identifiers(cell: str) -> list[str]:
    """Function-like identifiers in a row's first column, prose in parentheses removed."""
    depth, kept = 0, []
    for char in cell:
        if char in "([":
            depth += 1
        elif char in ")]":
            depth = max(0, depth - 1)
        elif depth == 0:
            kept.append(char)
    text = re.sub(r"`", "", "".join(kept))
    names = []
    for name in IDENT.findall(text):
        if name.lower() in ENGLISH or len(name) < 4:
            continue
        if name.startswith(("fn_", "lbl_", "_")) or "_" in name or re.search(r"[a-z][A-Z]|[A-Z][a-z]+[A-Z]|\d", name):
            names.append(name)
    return names


def word_in(name: str, text: str) -> bool:
    return re.search(rf"(?<![\w$]){re.escape(name)}(?![\w$])", text) is not None


# ---------------------------------------------------------------------------
# Findings
# ---------------------------------------------------------------------------

CLASS_INFO = {
    # class: (severity, description)
    "match.function_below_100": ("fail", "report function below 100% fuzzy match"),
    "match.unit_code_unmatched": ("fail", "unit's matched_code/matched_functions short of its totals"),
    "link.unlinked_code_unit": ("warn", "unit with code that is not linked (complete=false)"),
    "link.unlinked_data_unit": ("info", "data-only unit not linked (no functions)"),
    "build.report_stale": ("fail", "ninja -n has work to do for the report or SHA-1 check"),
    "build.sha1": ("fail", "dtk shasum -c config/GC6E01/build.sha1 failed"),
    "build.sha1_not_run": ("warn", "retail SHA-1 check could not be run"),
    "build.object_map_freeze": ("fail", "tools/check_object_map_freeze.py failed"),
    "build.uncommitted_sources": ("warn", "uncommitted src/include/config/configure.py changes: results describe the working tree, not HEAD"),
    "files.forbidden_new": ("fail", "tracked .inc/asset/build-product/compiler file not on origin/master"),
    "files.forbidden_preexisting": ("warn", "tracked .inc/asset/build-product/compiler file already on origin/master"),
    "inc.active_include": ("fail", "active #include of a .inc file in a compiled unit"),
    "asm.unregistered": ("fail", "live asm outside the evidence registry and the paired-single allowlist"),
    "asm.hardware_primitive": ("warn", "live asm accepted only by quality_scan's hardware-primitive allowlist (not a registry entry; AGENTS.md admits only registry + paired-single)"),
    "asm.instructions": ("fail", "registered/allowlisted asm uses an instruction or branch outside its allowlist"),
    "asm.evidence": ("fail", "registry entry's evidence is incomplete (quality_scan.evidence_problems)"),
    "asm.citation": ("fail", "registered asm does not cite its evidence document within 40 lines above"),
    "asm.macro_shadow": ("fail", "a C macro can rewrite tokens inside an asm body"),
    "asm.unparseable": ("fail", "live asm token not inside a parseable asm function/block"),
    "asm.branch_targets": ("fail", "verify_asm_branch_targets.py failed"),
    "asm.registry_not_live": ("warn", "registry entry whose asm function is not compiled in any unit"),
    "asm.registered": ("info", "live asm admitted by the evidence registry"),
    "asm.paired_single": ("info", "live asm admitted by the Dolphin paired-single allowlist"),
    "pragma.untagged": ("fail", "game-code compiler-control pragma in effect for a compiled function without a RULE-EXCEPTION tag directly above (per site)"),
    "pragma.untagged_library": ("fail", "same, in SDK/library source (src/dolphin, crt, trk, musyx, hsd, include); no existing scan exempts these (per site)"),
    "pragma.tagged_unlisted": ("fail", "tagged pragma whose function/file has no docs/RULE_EXCEPTIONS.md row (per site)"),
    "pragma.no_function": ("warn", "untagged compiler-control pragma active in a unit but covering no report function"),
    "pragma.accepted": ("info", "compiler-control pragma tagged and listed: accepted exception (per site)"),
    "pragma.structural": ("info", "exempt: non-control pragma (push/pop/section/force_active/pack/...) (per site)"),
    "function.unlocated": ("warn", "function definition not found in the unit's preprocessed source; pragma check could not run"),
    "tag.no_row": ("fail", "RULE-EXCEPTION tag whose function and file appear in no docs/RULE_EXCEPTIONS.md row"),
    "tag.prose_only": ("warn", "tag's function is named in the doc, but not in a row's Function/File columns"),
    "tag.no_doc_reference": ("warn", "RULE-EXCEPTION comment does not mention docs/RULE_EXCEPTIONS.md"),
    "row.stale_function": ("warn", "row names a function that exists neither in symbols.txt/report nor in the sources"),
    "row.stale_file": ("warn", "row names a file that does not exist"),
    "row.no_source_tag": ("warn", "row matched by no RULE-EXCEPTION tag in src/include, and not a configure.py/unit-level exception"),
    "row.config_only": ("info", "row with no source tag that describes a configure.py/unit-level exception (no source construct to tag)"),
    "protos.implicit_call": ("warn", "call to a function with no declaration in scope: implicit int-returning declaration (MWCC -requireprotos)"),
    "protos.unprototyped_decl": ("warn", "call to a function declared only as `name()` (old-style, no prototype) (MWCC -requireprotos)"),
    "protos.compile_error": ("warn", "unit failed to compile in the -requireprotos pass for another reason"),
    "protos.skipped": ("info", "unit not checked for prototypes (ProDG compiler)"),
    "symbols.check_naming": ("fail", "tools/check_naming.py (function-rename ledger, NAME-T tags) failed"),
    "symbols.renamed_since_origin": ("info", "symbols.txt address renamed since origin/master (needs a reason; not machine-checkable)"),
    "unit.cflag_controls": ("info", "unit-level compiler-control flags beyond the base set (-opt/-inline/-pragma), set in configure.py"),
}


class Findings:
    def __init__(self) -> None:
        self.items: dict[str, list[dict[str, Any]]] = defaultdict(list)

    def add(self, cls: str, message: str, **where: Any) -> None:
        self.items[cls].append({"message": message, **{k: v for k, v in where.items() if v is not None}})

    def count(self, severity: str) -> int:
        return sum(len(v) for k, v in self.items.items() if CLASS_INFO[k][0] == severity)


# ---------------------------------------------------------------------------
# Checks
# ---------------------------------------------------------------------------

def check_match(report: dict, findings: Findings) -> dict[str, Any]:
    summary = {"functions": 0, "functions_100": 0, "units": len(report["units"]), "unlinked_code": [], "unlinked_data": []}
    for unit in report["units"]:
        meta = unit.get("metadata") or {}
        measures = unit.get("measures") or {}
        functions = unit.get("functions") or []
        for function in functions:
            summary["functions"] += 1
            fuzzy = float(function.get("fuzzy_match_percent") or 0)
            if fuzzy >= 100:
                summary["functions_100"] += 1
            else:
                findings.add("match.function_below_100", f"{function['name']} {fuzzy:.2f}%",
                             unit=unit["name"], function=function["name"])
        if functions and (int(measures.get("matched_code", 0)) != int(measures.get("total_code", 0))
                          or int(measures.get("matched_functions", 0)) != int(measures.get("total_functions", 0))):
            findings.add("match.unit_code_unmatched",
                         f"{unit['name']}: code {measures.get('matched_code')}/{measures.get('total_code')}, "
                         f"functions {measures.get('matched_functions')}/{measures.get('total_functions')}",
                         unit=unit["name"])
        if not meta.get("complete"):
            if functions:
                entry = {"unit": unit["name"], "source": meta.get("source_path"), "functions": len(functions),
                         "code_bytes": int(measures.get("total_code", 0)),
                         "categories": meta.get("progress_categories")}
                summary["unlinked_code"].append(entry)
                findings.add("link.unlinked_code_unit",
                             f"{unit['name']} ({entry['source']}, {len(functions)} functions, {entry['code_bytes']} bytes, "
                             f"categories {entry['categories']})", unit=unit["name"])
            else:
                kind = "auto-generated" if meta.get("auto_generated") else ("source" if meta.get("source_path") else "no-source")
                summary["unlinked_data"].append({"unit": unit["name"], "kind": kind,
                                                 "data_bytes": int(measures.get("total_data", 0))})
                findings.add("link.unlinked_data_unit", f"{unit['name']} ({kind})", unit=unit["name"])
    summary["unlinked_data_by_kind"] = dict(Counter(u["kind"] for u in summary["unlinked_data"]))
    return summary


def run_tool(cmd: list[str], timeout: int = 600) -> tuple[int, str]:
    try:
        proc = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, timeout=timeout)
        return proc.returncode, (proc.stdout + proc.stderr).strip()
    except (OSError, subprocess.SubprocessError) as exc:
        return -1, str(exc)


def check_build(findings: Findings, skip_sha1: bool) -> dict[str, Any]:
    out: dict[str, Any] = {}
    code, text = run_tool(["ninja", "-n", "build/GC6E01/report.json", "build/GC6E01/ok"])
    stale = [line for line in text.splitlines() if re.match(r"^\[\d+/\d+\]", line)]
    out["ninja_dry_run"] = "no work to do" if code == 0 and not stale else f"{len(stale)} steps pending"
    if code != 0 or stale:
        findings.add("build.report_stale", f"ninja -n: {len(stale)} build steps pending (rc {code}); "
                     f"first: {stale[0] if stale else text[:200]}")
    code, text = run_tool([sys.executable, "tools/check_object_map_freeze.py"])
    out["object_map_freeze"] = text.splitlines()[-1] if text else ""
    if code != 0:
        findings.add("build.object_map_freeze", text[-500:])
    if skip_sha1:
        findings.add("build.sha1_not_run", "--no-sha1 given")
    elif not (ROOT / "build" / "tools" / "dtk").exists():
        findings.add("build.sha1_not_run", "build/tools/dtk missing")
    else:
        code, text = run_tool(["build/tools/dtk", "shasum", "-c", "config/GC6E01/build.sha1"])
        out["sha1"] = text[-2000:]
        if code != 0:
            findings.add("build.sha1", text[-500:])
    code, text = run_tool(["git", "status", "--porcelain", "--", "src", "include", "config", "configure.py",
                           "docs/RULE_EXCEPTIONS.md", "docs/asm_evidence"])
    if text:
        for line in text.splitlines():
            findings.add("build.uncommitted_sources", line.strip())
    return out


FORBIDDEN = re.compile(r"(?i)(\.inc|\.o|\.dol|\.rel|\.elf|\.iso|\.gcm|\.ciso|\.rvz|\.fsys|\.thp|\.gsw|\.pkx|\.rgba|\.exe|\.dll)$"
                       r"|^orig/(?!.*\.gitkeep$)|^build/")


def check_tracked_files(findings: Findings) -> None:
    code, text = run_tool(["git", "ls-files", "-s"])
    origin_code, origin_text = run_tool(["git", "ls-tree", "-r", "--name-only", "origin/master"])
    on_origin = set(origin_text.splitlines()) if origin_code == 0 else set()
    for line in text.splitlines():
        meta, _, path = line.partition("\t")
        if not FORBIDDEN.search(path):
            continue
        mode = meta.split()[0]
        note = " (symlink)" if mode == "120000" else ""
        cls = "files.forbidden_preexisting" if path in on_origin else "files.forbidden_new"
        findings.add(cls, f"{path}{note}", file=path)


def check_symbol_renames(findings: Findings, current: dict[str, tuple[str, str]]) -> dict[str, int]:
    if (ROOT / "tools" / "check_naming.py").is_file():
        code, text = run_tool([sys.executable, "tools/check_naming.py"])
        if code != 0:
            for line in text.splitlines():
                if line.startswith("error"):
                    findings.add("symbols.check_naming", line)
            if not findings.items.get("symbols.check_naming"):
                findings.add("symbols.check_naming", text[-300:])
    code, text = run_tool(["git", "show", "origin/master:config/GC6E01/symbols.txt"])
    if code != 0:
        return {}
    before = load_symbols(text)
    by_address = {addr: name for name, (addr, _) in before.items()}
    now_by_address = {addr: name for name, (addr, _) in current.items()}
    renamed = 0
    for addr, name in sorted(now_by_address.items()):
        old = by_address.get(addr)
        if old and old != name:
            renamed += 1
            findings.add("symbols.renamed_since_origin", f"{addr}: {old} -> {name}")
    return {"renamed": renamed, "added": len(set(now_by_address) - set(by_address)),
            "removed": len(set(by_address) - set(now_by_address))}


def load_cache(no_cache: bool) -> dict[str, Any]:
    path = CACHE_DIR / "units.json"
    if no_cache:
        return {}
    try:
        cache = json.loads(path.read_text(encoding="utf-8"))
        return cache if cache.get("version") == VERSION else {}
    except (OSError, ValueError):
        return {}


def file_stamp(files: list[str]) -> str:
    digest = hashlib.sha1()
    for name in sorted(set(files)):
        try:
            stat = (ROOT / name).stat()
            digest.update(f"{name}:{stat.st_size}:{stat.st_mtime_ns}\n".encode())
        except OSError:
            digest.update(f"{name}:missing\n".encode())
    return digest.hexdigest()


def unit_jobs(report: dict, edges: dict[str, dict[str, Any]]) -> list[dict[str, Any]]:
    registry = json.loads(REGISTRY.read_text(encoding="utf-8")).get("entries", [])
    by_source = {edge["source"]: (obj, edge) for obj, edge in edges.items()}
    jobs = []
    for unit in report["units"]:
        meta = unit.get("metadata") or {}
        source = meta.get("source_path")
        if not source:
            continue
        obj, edge = by_source.get(source, (None, None))
        cflags = edge["vars"].get("cflags", "") if edge else ""
        macros, dirs, _ = compile_flags(cflags) if edge else (dict(DEFAULT_DEFINES), list(DEFAULT_INCLUDE_DIRS), [])
        jobs.append({
            "unit": unit["name"], "source": source, "object": obj, "edge": edge, "cflags": cflags,
            "macros": macros, "dirs": dirs, "linked": bool(meta.get("complete")),
            "functions": [f["name"] for f in unit.get("functions") or []],
            "addresses": {f["name"]: int((f.get("metadata") or {}).get("virtual_address") or 0)
                          for f in unit.get("functions") or []},
            "aliases": {f["name"]: [e["function"] for e in registry
                                    if e.get("path") == source and e.get("address") == f["name"]]
                        for f in unit.get("functions") or []
                        if any(e.get("path") == source and e.get("address") == f["name"] for e in registry)},
        })
    return jobs


def run_unit_analysis(jobs: list[dict[str, Any]], cache: dict[str, Any], deps: dict[str, list[str]],
                      workers: int) -> dict[str, dict[str, Any]]:
    cached_units = cache.get("units", {})
    results: dict[str, dict[str, Any]] = {}
    pending = []
    for job in jobs:
        identity = json.dumps([job["source"], job["cflags"], job["functions"], job["aliases"]])
        entry = cached_units.get(job["unit"])
        if entry and entry.get("identity") == identity:
            files = entry["result"].get("read_files", []) + deps.get(job["object"] or "", [])
            if entry.get("stamp") == file_stamp(files):
                results[job["unit"]] = entry["result"]
                continue
        pending.append((job, identity))
    if pending:
        payload = [{k: job[k] for k in ("source", "macros", "dirs", "functions", "addresses", "aliases")}
                   for job, _ in pending]
        with ProcessPoolExecutor(max_workers=workers) as pool:
            for (job, identity), result in zip(pending, pool.map(analyse_unit, payload, chunksize=8)):
                results[job["unit"]] = result
                files = result.get("read_files", []) + deps.get(job["object"] or "", [])
                cached_units[job["unit"]] = {"identity": identity, "stamp": file_stamp(files), "result": result}
    cache["units"] = cached_units
    return results


def rule_listed(names: list[str], address: int, sources: list[str], doc_text: str, rows: list[dict]) -> bool:
    """Mirror of boot_status Decomp.rule_listed: the doc names the function (symbol,
    spelling or fn_ address name), or a row's File column names a .c file the unit reads."""
    spellings = {n for n in names if n}
    if address:
        spellings.add(f"fn_{address:08X}")
    if any(word_in(name, doc_text) for name in spellings):
        return True
    return any(source.endswith((".c", ".cpp")) and any(re.search(rf"(?<![\w/.]){re.escape(source)}\b", row["files"])
                                                        for row in rows)
               for source in sources)


LIBRARY_PREFIXES = ("src/dolphin/", "src/crt/", "src/trk/", "src/musyx/", "src/hsd/", "include/")


def check_pragmas_and_functions(jobs, results, doc_text, rows, findings: Findings) -> dict[str, Any]:
    """Per function: every compiler control in effect over its definition must be
    tagged and listed. Findings are grouped per pragma site (file:line), with the
    functions it covers."""
    stats = Counter()
    sites: dict[tuple[str, int, str], dict[str, Any]] = {}
    for job in jobs:
        result = results.get(job["unit"]) or {}
        if result.get("error"):
            findings.add("function.unlocated", result["error"], unit=job["unit"])
            continue
        read = result.get("read_files", [])
        covered = set()
        for name in job["functions"]:
            info = result.get("functions", {}).get(name)
            if not info or not info.get("located"):
                findings.add("function.unlocated", f"{name} in {job['source']}", unit=job["unit"], function=name)
                stats["functions_unlocated"] += 1
                continue
            stats["functions_located"] += 1
            for control, value, path, line, tagged in info["pragmas"]:
                covered.add((path, line))
                pragma = f"#pragma {control} {value}".strip()
                if not tagged:
                    cls = "pragma.untagged_library" if path.startswith(LIBRARY_PREFIXES) else "pragma.untagged"
                elif rule_listed(info["spellings"] + [name], job["addresses"].get(name, 0), read, doc_text, rows):
                    cls = "pragma.accepted"
                else:
                    cls = "pragma.tagged_unlisted"
                site = sites.setdefault((path, line, cls), {"pragma": pragma, "functions": [], "units": set()})
                if name not in site["functions"]:
                    site["functions"].append(name)
                site["units"].add(job["unit"])
        for control, value, path, line, tagged in result.get("events", []):
            if control in NON_CONTROL_PRAGMAS:
                sites.setdefault((path, line, "pragma.structural"),
                                 {"pragma": f"#pragma {control} {value}".strip(), "functions": [], "units": set()}
                                 )["units"].add(job["unit"])
            elif (path, line) not in covered and not tagged:
                sites.setdefault((path, line, "pragma.no_function"),
                                 {"pragma": f"#pragma {control} {value}".strip(), "functions": [], "units": set()}
                                 )["units"].add(job["unit"])
    covered_anywhere = {(path, line) for (path, line, cls) in sites if cls != "pragma.no_function"}
    for (path, line, cls), site in sorted(sites.items()):
        if cls == "pragma.no_function" and (path, line) in covered_anywhere:
            continue  # covers a function in another unit that compiles the same file
        functions = site["functions"]
        shown = ", ".join(functions[:4]) + (f" (+{len(functions) - 4})" if len(functions) > 4 else "")
        message = f"{site['pragma']} at {path}:{line}" + (f" covers {shown}" if functions else
                                                         f" (units: {', '.join(sorted(site['units']))[:120]})")
        findings.add(cls, message, file=path, line=line, functions=functions, units=sorted(site["units"]))
    return dict(stats)


def check_asm(jobs, results, findings: Findings) -> dict[str, Any]:
    live: dict[str, set[int]] = defaultdict(set)
    for job in jobs:
        for path, line in (results.get(job["unit"]) or {}).get("asm", []):
            live[path].add(line)
        for path, line, target in (results.get(job["unit"]) or {}).get("inc", []):
            findings.add("inc.active_include", f'#include "{target}" at {path}:{line}', unit=job["unit"], file=path, line=line)
    registry = qs.load_authentic_asm(REGISTRY)
    for entry in registry.values():
        for problem in qs.evidence_problems(entry):
            findings.add("asm.evidence", problem, file=entry.get("path"), function=entry.get("function"))
    live_functions: set[tuple[str, str]] = set()
    for path, lines in sorted(live.items()):
        source_path = ROOT / path
        if not source_path.is_file():
            continue
        source = source_path.read_text(encoding="utf-8", errors="replace")
        masked_lines = qs._mask_non_code(source).splitlines()
        source_lines = source.splitlines()
        lines = {n for n in lines if 1 <= n <= len(masked_lines) and qs.ASM_TOKEN.search(masked_lines[n - 1])}
        if not lines:
            continue
        regions = qs.find_asm_regions(source)
        macro_defs = {m.group("name") for m in qs.MACRO_DEFINE.finditer(qs._mask_non_code(source))}
        touched: dict[str, list[dict]] = defaultdict(list)
        for number in sorted(lines):
            hits = [r for r in regions if r["start_line"] <= number <= r["end_line"]]
            if not hits:
                findings.add("asm.unparseable", f"{path}:{number}: {source_lines[number - 1].strip()[:80]}",
                             file=path, line=number)
            for region in hits:
                if region not in touched[region["func"]]:
                    touched[region["func"]].append(region)
        for func, func_regions in sorted(touched.items()):
            first = min(r["start_line"] for r in func_regions)
            where = {"file": path, "line": first, "function": func}
            if not func or any(not r["closed"] for r in func_regions):
                findings.add("asm.unparseable", f"{path}:{first}: asm block not mapped to a closed function", **where)
                continue
            live_functions.add((path, func))
            entry = registry.get((path, func))
            paired = path == qs.DOLPHIN_PAIRED_SINGLE_PATH and func in qs.DOLPHIN_PAIRED_SINGLE_FUNCTIONS
            body = "\n".join(r["body"] for r in func_regions)
            shadowed = sorted(macro_defs & (set(qs.IDENTIFIER.findall(body)) | {func}))
            if shadowed:
                findings.add("asm.macro_shadow", f"{path}:{func}: {', '.join(shadowed)}", **where)
            if entry is not None:
                above = "\n".join(source_lines[max(0, first - 41):first - 1])
                if entry.get("evidence", "\0") not in above:
                    findings.add("asm.citation", f"{path}:{func} does not cite {entry.get('evidence')}", **where)
            buffer = io.StringIO()
            with contextlib.redirect_stdout(buffer):
                ok = qs.asm_body_ok(body, path, func)
            if entry is None and not paired:
                cls = "asm.hardware_primitive" if ok else "asm.unregistered"
                findings.add(cls, f"{path}:{func} (lines {first}-{max(r['end_line'] for r in func_regions)})", **where)
                continue
            if not ok:
                findings.add("asm.instructions", f"{path}:{func}: {buffer.getvalue().strip()[:300]}", **where)
                continue
            findings.add("asm.registered" if entry is not None else "asm.paired_single",
                         f"{path}:{func}" + (f" ({entry.get('tier', 'library')})" if entry else ""), **where)
    for (path, func), entry in sorted(registry.items()):
        if (path, func) not in live_functions:
            findings.add("asm.registry_not_live", f"{path}:{func}", file=path, function=func)
    code, text = run_tool([sys.executable, ".github/scripts/verify_asm_branch_targets.py"])
    if code != 0:
        for line in text.splitlines():
            if "::error::" in line:
                findings.add("asm.branch_targets", line.split("::error::", 1)[1])
    _, summary = run_tool([sys.executable, ".github/scripts/asm_evidence_summary.py"])
    return {"live_asm_files": len(live), "branch_targets": text.splitlines()[-1] if text else "",
            "evidence_summary": summary.splitlines()}


TAG_COMMENT = re.compile(r"/\*.*?\*/|//[^\n]*", re.DOTALL)


def check_tags_and_rows(doc_text: str, rows: list[dict], symbols: dict, report_names: set[str],
                        name_address: dict[str, int], findings: Findings) -> dict[str, Any]:
    files = sorted(p for d in ("src", "include") for p in (ROOT / d).rglob("*") if p.suffix in (".c", ".h", ".cpp"))
    corpus_identifiers: set[str] = set()
    row_hits = Counter()
    row_function_cells = [(row, set(IDENT.findall(row["functions"] + " " + row["files"]))) for row in rows]
    tags = 0
    for path in files:
        source = path.read_text(encoding="utf-8", errors="replace")
        corpus_identifiers.update(re.findall(r"[A-Za-z_][\w$]*", source))
        if "RULE-EXCEPTION(" not in source:
            continue
        relpath = rel(path)
        masked = qs._mask_non_code(source)
        regions = [r for r in qs._function_regions(source, masked) if r["func"] not in C_KEYWORDS]
        starts = sorted(regions, key=lambda r: r["start"])
        for comment in TAG_COMMENT.finditer(source):
            body = comment.group(0)
            if "RULE-EXCEPTION(" not in body:
                continue
            tags += 1
            offset = comment.start()
            line = source.count("\n", 0, offset) + 1
            if "RULE_EXCEPTIONS.md" not in body:
                findings.add("tag.no_doc_reference", f"{relpath}:{line}", file=relpath, line=line)
            enclosing = [r for r in regions if r["start"] <= offset <= r["end"]]
            following = next((r for r in starts if r["start"] > offset), None)
            candidates = []
            if enclosing:
                candidates.append(max(enclosing, key=lambda r: r["start"])["func"])
            if following:
                candidates.append(following["func"])
            names = set()
            # A wrapper or carve file names its address: fn_XXXXXXXX.
            names.update(f"fn_{a.upper()}" for a in re.findall(r"(?<![0-9A-Fa-f])(8[0-9A-Fa-f]{7})(?![0-9A-Fa-f])", path.name))
            for func in candidates:
                names.update(source_names(func))
                address = name_address.get(func)
                if address:
                    names.add(f"fn_{address:08X}")
            hit_rows = [i for i, (row, idents) in enumerate(row_function_cells) if names & idents]
            hit_rows += [i for i, row in enumerate(rows)
                         if re.search(rf"(?<![\w/.]){re.escape(relpath)}\b", row["files"])
                         or re.search(rf"(?<![\w.]){re.escape(path.name)}\b", row["files"])]
            where = {"file": relpath, "line": line, "function": candidates[0] if candidates else None}
            if hit_rows:
                for i in set(hit_rows):
                    row_hits[i] += 1
            elif any(word_in(name, doc_text) for name in names):
                findings.add("tag.prose_only", f"{relpath}:{line} ({', '.join(sorted(names))})", **where)
            else:
                findings.add("tag.no_row", f"{relpath}:{line} (function {', '.join(candidates) or '?'})", **where)
    known = report_names | set(symbols)
    known |= {re.sub(r"__F.*$", "", name) for name in known}
    known |= {f"fn_{address:08X}" for address in name_address.values() if address}
    for index, row in enumerate(rows):
        for name in row_identifiers(row["functions"]):
            base = re.sub(r"__F.*$", "", name)
            if name in known or base in known or name in corpus_identifiers or base in corpus_identifiers:
                continue
            if re.search(rf"(?<![\w$]){re.escape(name)}\*", row["functions"]) and \
                    any(k.startswith(name) for k in known):
                continue  # a wildcard family such as dbgMenuFight*
            findings.add("row.stale_function", f"docs/RULE_EXCEPTIONS.md:{row['line']}: {name}", line=row["line"])
        for match in PATH_TOKEN.finditer(row["files"]):
            token = match.group(1).rstrip(".,;")
            if not (ROOT / token).exists():
                findings.add("row.stale_file", f"docs/RULE_EXCEPTIONS.md:{row['line']}: {token}", line=row["line"])
        if not row_hits[index]:
            config_level = re.search(r"configure\.py|constant_import|unit-level|per-unit|-opt |-inline |cflags",
                                     " ".join(row["cells"]))
            findings.add("row.config_only" if config_level else "row.no_source_tag", f"docs/RULE_EXCEPTIONS.md:{row['line']}: {row['functions'][:90]}",
                         line=row["line"])
    return {"tags": tags, "rows": len(rows)}


# ---------------------------------------------------------------------------
# Prototype pass (MWCC -requireprotos)
# ---------------------------------------------------------------------------

def proto_command(edge: dict[str, Any], out_obj: str) -> list[str] | None:
    if not edge or not edge["rule"].startswith("mwcc"):
        return None
    version = edge["vars"].get("mw_version", "GC/1.3")
    cmd = ["build/tools/wibo"]
    if "sjis" in edge["rule"]:
        cmd.append("build/tools/sjiswrap.exe")
    cmd += [f"build/compilers/{version}/mwcceppc.exe", *shlex.split(edge["vars"].get("cflags", "")),
            "-requireprotos", "-c", edge["source"], "-o", out_obj]
    return cmd


def parse_proto_output(text: str) -> tuple[list[dict[str, Any]], list[str]]:
    implicit, other = [], []
    current_file = ""
    lines = text.splitlines()
    for index, line in enumerate(lines):
        header = re.match(r"^#\s+(File|In):\s+(.+)$", line)
        if header:
            current_file = rel(ROOT / header.group(2).strip().replace("\\", "/"))
        elif re.match(r"^#\s+Error:", line):
            message = lines[index + 1][1:].strip() if index + 1 < len(lines) else ""
            code_line = lines[index - 1] if index else ""
            number_match = re.match(r"^#\s+(\d+):", code_line)
            if message == "function has no prototype":
                callee = "?"
                caret = line.find("^")
                if caret >= 0 and number_match:
                    ident = re.match(r"[A-Za-z_][\w$]*", code_line[caret:])
                    if ident and re.match(r"\s*\(", code_line[ident.end() + caret:]):
                        callee = ident.group(0)
                    else:
                        # Tabs shift MWCC's caret; take the call nearest to it.
                        calls = [m for m in re.finditer(r"\b([A-Za-z_][\w$]*)\s*\(", code_line[number_match.end():])
                                 if m.group(1) not in C_KEYWORDS]
                        if calls:
                            offset = caret - number_match.end()
                            callee = min(calls, key=lambda m: abs(m.start() - offset)).group(1) + "?"
                implicit.append({"file": current_file, "line": int(number_match.group(1)) if number_match else 0,
                                 "callee": callee, "code": code_line.split(":", 1)[-1].strip()[:120]})
            else:
                other.append(f"{current_file}:{number_match.group(1) if number_match else '?'}: {message}")
    return implicit, other


def run_protos(jobs, deps, cache, workers, findings: Findings, unit_results: dict) -> dict[str, Any]:
    proto_cache = cache.setdefault("protos", {})
    tmp = Path(tempfile.mkdtemp(prefix="protos_", dir=CACHE_DIR))
    todo, results = [], {}
    for job in jobs:
        edge = job["edge"]
        if not edge:
            continue
        if edge["rule"] == "prodg":
            findings.add("protos.skipped", f"{job['unit']} (ProDG)", unit=job["unit"])
            continue
        identity = json.dumps([edge["source"], edge["vars"].get("cflags"), edge["vars"].get("mw_version"), edge["rule"]])
        files = deps.get(job["object"] or "", []) + [job["source"]]
        stamp = file_stamp(files)
        entry = proto_cache.get(job["object"])
        if entry and entry["identity"] == identity and entry["stamp"] == stamp and not entry["other"]:
            results[job["object"]] = entry
            continue
        todo.append((job, identity, stamp))

    def compile_one(item):
        job, identity, stamp = item
        out_obj = str(tmp / (hashlib.sha1(job["object"].encode()).hexdigest() + ".o"))
        cmd = proto_command(job["edge"], out_obj)
        code, text = run_tool(cmd, timeout=300)
        implicit, other = parse_proto_output(text)
        if code != 0 and not implicit and not other:
            other = [text[-300:]]
        return job, {"identity": identity, "stamp": stamp, "implicit": implicit, "other": other}

    if todo:
        with ThreadPoolExecutor(max_workers=workers) as pool:
            for job, entry in pool.map(compile_one, todo):
                results[job["object"]] = entry
                if not entry["other"]:  # never cache an environment failure
                    proto_cache[job["object"]] = entry
    seen = set()
    texts: dict[str, str] = {}

    def declared_without_prototype(callee: str, files: list[str]) -> bool:
        pattern = re.compile(rf"\b{re.escape(callee.rstrip('?'))}\s*\(\s*\)\s*;")
        for name in files:
            if name not in texts:
                try:
                    texts[name] = (ROOT / name).read_text(encoding="utf-8", errors="replace")
                except OSError:
                    texts[name] = ""
            if pattern.search(texts[name]):
                return True
        return False
    for job in jobs:
        entry = results.get(job["object"])
        if not entry:
            continue
        files = (unit_results.get(job["unit"]) or {}).get("read_files", [job["source"]])
        for item in entry["implicit"]:
            key = (item["file"], item["line"], item["callee"])
            if key in seen:
                continue
            seen.add(key)
            cls = "protos.unprototyped_decl" if declared_without_prototype(item["callee"], files) \
                else "protos.implicit_call"
            findings.add(cls, f"{item['file']}:{item['line']}: {item['callee']}  [{item['code']}]",
                         file=item["file"], line=item["line"], function=item["callee"], unit=job["unit"])
        for message in entry["other"]:
            findings.add("protos.compile_error", f"{job['unit']}: {message}", unit=job["unit"])
    try:
        for leftover in tmp.iterdir():
            leftover.unlink()
        tmp.rmdir()
    except OSError:
        pass
    return {"compiled": len(todo), "cached": len(results) - len(todo)}


BASE_CONTROL = re.compile(r"^-(opt|inline|pragma|O\d|schedule|peephole)")


def check_unit_flags(jobs, findings: Findings) -> None:
    """Record unit-level compiler controls (info): configure.py owns these, so
    they are not source pragmas, but a reviewer may want the list."""
    for job in jobs:
        if not job["edge"]:
            continue
        args = shlex.split(job["cflags"])
        controls = []
        for index, arg in enumerate(args):
            if arg in ("-opt", "-inline", "-pragma") and index + 1 < len(args):
                if (arg, args[index + 1]) not in (("-inline", "auto"), ("-pragma", "cats off")):
                    controls.append(f"{arg} {args[index + 1]}")
        if controls:
            findings.add("unit.cflag_controls", f"{job['unit']}: {' '.join(controls)}", unit=job["unit"])


# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--strict", action="store_true", help="fail on warnings too")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    parser.add_argument("--no-protos", action="store_true", help="skip the MWCC -requireprotos pass")
    parser.add_argument("--no-sha1", action="store_true", help="skip dtk shasum")
    parser.add_argument("--no-cache", action="store_true", help="ignore cached per-unit results")
    parser.add_argument("--report", type=Path, default=REPORT)
    parser.add_argument("--json", type=Path, default=OUT)
    parser.add_argument("-v", "--verbose", action="store_true", help="list every finding, not just examples")
    args = parser.parse_args()
    started = time.time()

    if not args.report.is_file():
        print(f"missing {args.report}: run `ninja` first", file=sys.stderr)
        return 2
    report = json.loads(args.report.read_text(encoding="utf-8"))
    findings = Findings()
    CACHE_DIR.mkdir(parents=True, exist_ok=True)
    cache = load_cache(args.no_cache)
    cache["version"] = VERSION

    sections: dict[str, Any] = {}
    sections["match"] = check_match(report, findings)
    sections["build"] = check_build(findings, args.no_sha1)
    check_tracked_files(findings)
    symbols = load_symbols(SYMBOLS.read_text(encoding="utf-8", errors="replace"))
    sections["symbols"] = check_symbol_renames(findings, symbols)

    edges = parse_ninja()
    deps = ninja_deps()
    jobs = unit_jobs(report, edges)
    sections["units"] = {"with_source": len(jobs), "compiled_in_ninja": sum(1 for j in jobs if j["edge"]),
                         "ninja_compile_edges": len(edges)}
    for job in jobs:
        if not job["edge"]:
            findings.add("function.unlocated", f"{job['unit']}: no compile edge for {job['source']} in build.ninja",
                         unit=job["unit"])
    results = run_unit_analysis(jobs, cache, deps, args.jobs)

    doc_text, rows = load_rows()
    sections["pragmas"] = check_pragmas_and_functions(jobs, results, doc_text, rows, findings)
    sections["asm"] = check_asm(jobs, results, findings)
    name_address = {}
    for unit in report["units"]:
        for function in unit.get("functions") or []:
            name_address[function["name"]] = int((function.get("metadata") or {}).get("virtual_address") or 0)
    for name, (addr, _) in symbols.items():
        if addr.startswith(".TEXT:0X") and name not in name_address:
            name_address[name] = int(addr.split(":0X")[1], 16)
    sections["tags"] = check_tags_and_rows(doc_text, rows, symbols, set(name_address), name_address, findings)
    check_unit_flags(jobs, findings)
    if args.no_protos:
        sections["protos"] = "skipped (--no-protos)"
    else:
        sections["protos"] = run_protos(jobs, deps, cache, args.jobs, findings, results)
    (CACHE_DIR / "units.json").write_text(json.dumps(cache), encoding="utf-8")

    fails, warns = findings.count("fail"), findings.count("warn")
    failed = fails > 0 or (args.strict and warns > 0)
    classes = {}
    for cls, (severity, description) in CLASS_INFO.items():
        items = findings.items.get(cls, [])
        classes[cls] = {"severity": severity, "description": description, "count": len(items), "findings": items}
    out = {
        "tool": "tools/verify_decomp_rules.py", "version": VERSION, "strict": args.strict,
        "result": "FAIL" if failed else "PASS", "failures": fails, "warnings": warns,
        "pragma_exemption_classes": {
            "pragma.structural": sorted(NON_CONTROL_PRAGMAS),
            "reset/push/pop": "restore earlier state; the setting they restore is what is checked",
        },
        "sections": sections, "classes": classes,
        "elapsed_seconds": round(time.time() - started, 1),
    }
    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(out, indent=1) + "\n", encoding="utf-8")

    match = sections["match"]
    print(f"functions at 100%: {match['functions_100']}/{match['functions']}; units {match['units']}; "
          f"unlinked code units {len(match['unlinked_code'])}, unlinked data-only units {len(match['unlinked_data'])} "
          f"{match['unlinked_data_by_kind']}")
    print(f"build: {sections['build']}")
    print(f"{'severity':8} {'class':32} {'count':>6}  description / examples")
    order = {"fail": 0, "warn": 1, "info": 2}
    for cls, info in sorted(classes.items(), key=lambda kv: (order[kv[1]["severity"]], kv[0])):
        if not info["count"] and info["severity"] == "info":
            continue
        print(f"{info['severity']:8} {cls:32} {info['count']:6}  {info['description']}")
        shown = info["findings"] if args.verbose else info["findings"][:3]
        if info["severity"] != "info" or args.verbose:
            for item in shown:
                print(f"{'':49}- {item['message'][:160]}")
    print(f"\n{'FAIL' if failed else 'PASS'}: {fails} failure(s), {warns} warning(s)"
          f"{' (strict)' if args.strict else ''}; JSON: {rel(args.json)}; {out['elapsed_seconds']}s")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
