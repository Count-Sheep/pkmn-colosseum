#!/usr/bin/env python3
"""Quality gate: block match-cheating patterns in newly added source.

Rules:
- Any added `#include "*.inc"` in src/ fails (raw-asm shim).
- Any new or changed asm block fails unless its enclosing function fits one of
  three authentic classes:
  - hardware-register/privileged primitives (PPCMfmsr etc.);
  - the explicitly named Dolphin SDK paired-single math routines; or
  - authentic hand-written library assembly, registered per function in
    docs/asm_evidence/registry.json with documented evidence (see below).

MWCC cannot emit the required privileged or paired-single instructions from C.
The paired-single exception is restricted to one source unit, known SDK
symbols, and the target's verified mnemonic set. Calls and general GPR memory
access remain forbidden, so game logic cannot hide behind the exception.
"""

import json
import re
import subprocess
import sys
from pathlib import Path


HARDWARE_ALLOWED = {
    # SPR / MSR / FPSCR access
    "mfmsr", "mtmsr", "mfspr", "mtspr", "mfdec", "mtdec",
    "mffs", "mtfsf", "mtfsb0", "mtfsb1",
    # synchronization / control
    "sync", "isync", "eieio", "rfi", "sc", "nop",
    # register/immediate shuffling needed around SPR ops (no memory access)
    "li", "lis", "ori", "oris", "or", "mr", "rlwinm", "addi",
    # control flow: plain branch and return only (no bl -- no calls)
    "b", "blr", "bne", "beq", "bne-", "beq-", "bne+", "beq+", "cmpwi",
}

DOLPHIN_PAIRED_SINGLE_PATH = "src/dolphin/sdk_range_800A2D38.c"

# These 26 routines are the paired-single implementations in this SDK unit.
# PSMTXRotRad and PSMTXRotAxisRad are deliberately absent: the vendor source
# implements them as C wrappers, and both already match from C.
DOLPHIN_PAIRED_SINGLE_FUNCTIONS = {
    "PSMTXIdentity",
    "PSMTXCopy",
    "PSMTXConcat",
    "PSMTXTranspose",
    "PSMTXInverse",
    "PSMTXInvXpose",
    "PSMTXRotTrig",
    "__PSMTXRotAxisRadInternal",
    "PSMTXTrans",
    "PSMTXTransApply",
    "PSMTXScale",
    "PSMTXScaleApply",
    "PSMTXQuat",
    "PSMTXMultVec",
    "PSMTXMultVecSR",
    "PSVECAdd",
    "PSVECSubtract",
    "PSVECScale",
    "PSVECNormalize",
    "PSVECSquareMag",
    "PSVECMag",
    "PSVECDotProduct",
    "PSVECCrossProduct",
    "PSVECSquareDistance",
    "PSVECDistance",
    "PSQUATMultiply",
}

# Exact mnemonic union of the 26 target functions. This includes ten support
# instructions omitted from the contributor's initial sketch, but needed by
# the target: ps_muls1, frsqrte, fadds, fsubs, fmuls, fmadds, fnmsubs, fcmpu,
# cmplwi, and stwu. It intentionally excludes bl, lwz/stw, and update-form
# paired loads/stores because none belongs to this target unit.
DOLPHIN_PAIRED_SINGLE_ALLOWED = {
    "addi", "b", "beq", "blr", "bne", "cmplwi",
    "fadds", "fcmpu", "fmadds", "fmuls", "fnmsubs", "fres", "frsp",
    "frsqrte", "fsubs", "lfd", "lfs", "li", "lis", "ori",
    "ps_add", "ps_cmpo0", "ps_madd", "ps_madds0", "ps_madds1",
    "ps_merge00", "ps_merge01", "ps_merge10", "ps_merge11", "ps_msub",
    "ps_mul", "ps_muls0", "ps_muls1", "ps_neg", "ps_nmadd", "ps_nmsub",
    "ps_sub", "ps_sum0", "ps_sum1", "psq_l", "psq_st",
    "stfd", "stfs", "stwu",
}

# Authentic hand-written library assembly (user decision, 2026-09-28): functions
# the original developers wrote in assembly (e.g. MusyX's reverb DSP routines)
# cannot be expressed in C, so a unit that contains one can only link with its
# asm body in the source. Each such function must be registered individually
# in docs/asm_evidence/registry.json with its exact mnemonic set and an evidence
# document; nothing is assumed. The evidence document must, under a
# "## <function>" heading, give all three of:
#   - "Why it cannot be C:" instructions or conventions MWCC never emits;
#   - "Other decompilations:" at least one other project that also keeps it as
#     assembly, as a GitHub URL with a commit hash;
#   - "Origin:" where the routine comes from (library, vendor, version).
# The source must name the evidence document within 40 lines above the asm
# function. Missing or incomplete evidence fails the scan.
REPO_ROOT = Path(__file__).resolve().parents[2]
AUTHENTIC_ASM_REGISTRY = REPO_ROOT / "docs" / "asm_evidence" / "registry.json"
EVIDENCE_FIELDS = ("Why it cannot be C:", "Other decompilations:", "Origin:")
BRANCH_TARGET_FIELD = "External branch targets:"
# Only vendor library code may declare branches to other functions.
LIBRARY_ASM_PREFIXES = ("src/dolphin/", "src/trk/", "src/crt/")
SYMBOLS_FILE = REPO_ROOT / "config" / "GC6E01" / "symbols.txt"
GITHUB_COMMIT = re.compile(r"github\.com/[\w.-]+/[\w.-]+\S*\b[0-9a-f]{7,40}\b|\b[0-9a-f]{7,40}\b\S*github\.com/[\w.-]+/[\w.-]+")


def load_authentic_asm(registry: Path = AUTHENTIC_ASM_REGISTRY) -> dict[tuple[str, str], dict]:
    try:
        data = json.loads(registry.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return {}
    return {(entry["path"], entry["function"]): entry for entry in data.get("entries", [])
            if entry.get("path") and entry.get("function")}


def evidence_problems(entry: dict, root: Path = REPO_ROOT) -> list[str]:
    """Why an authentic-asm registry entry's evidence is insufficient (empty if it is sufficient)."""
    problems = []
    func = entry.get("function", "")
    if not entry.get("mnemonics"):
        problems.append(f"{func}: registry entry lists no mnemonics")
    document = root / entry.get("evidence", "")
    if not entry.get("evidence") or not document.is_file():
        return problems + [f"{func}: evidence document {entry.get('evidence')!r} is missing"]
    text = document.read_text(encoding="utf-8", errors="replace")
    section = re.search(rf"(?ms)^##\s+{re.escape(func)}\b(.*?)(?=^##\s|\Z)", text)
    if not section:
        return problems + [f"{func}: {entry['evidence']} has no '## {func}' section"]
    body = section.group(1)
    for field in EVIDENCE_FIELDS:
        match = re.search(rf"(?ms)^\s*[-*]?\s*\**{re.escape(field)}\**(.*?)(?=^\s*[-*]?\s*\**(?:{'|'.join(map(re.escape, EVIDENCE_FIELDS))})|\Z)", body)
        if not match or len(match.group(1).strip()) < 20:
            problems.append(f"{func}: evidence field '{field}' is missing or empty in {entry['evidence']}")
        elif field == "Other decompilations:" and not GITHUB_COMMIT.search(match.group(1)):
            problems.append(f"{func}: 'Other decompilations:' needs a GitHub URL with a commit hash")
    targets = entry.get("branch_targets") or []
    if targets:
        if not str(entry.get("path", "")).startswith(LIBRARY_ASM_PREFIXES):
            problems.append(f"{func}: branch_targets are only allowed for library code "
                            f"({', '.join(LIBRARY_ASM_PREFIXES)})")
        match = re.search(rf"(?ms)^\s*[-*]?\s*\**{re.escape(BRANCH_TARGET_FIELD)}\**(.*?)(?=^\s*[-*]?\s*\**[A-Z][^\n:]*:\**|\Z)", body)
        if not match or len(match.group(1).strip()) < 20:
            problems.append(f"{func}: branch_targets need an '{BRANCH_TARGET_FIELD}' field in {entry['evidence']}")
        else:
            for target in targets:
                if target not in match.group(1):
                    problems.append(f"{func}: '{BRANCH_TARGET_FIELD}' does not name {target}")
        known = function_symbols(root / SYMBOLS_FILE.relative_to(REPO_ROOT))
        for target in targets:
            if target not in known:
                problems.append(f"{func}: branch target {target} is not a function in symbols.txt")
    return problems


def function_symbols(path: Path = SYMBOLS_FILE) -> set[str]:
    """Names of functions declared in symbols.txt (empty if it cannot be read)."""
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return set()
    return {m.group(1) for m in re.finditer(r"(?m)^([A-Za-z_][\w$@.]*)\s*=\s*\.text:0x[0-9A-Fa-f]+;[^\n]*type:function", text)}


AUTHENTIC_ASM = load_authentic_asm()

HUNK = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,\d+)? @@")
ASM_FUNCTION = re.compile(
    r"(?m)^[ \t]*(?:static[ \t]+)?asm[ \t\r\n]+(?:[A-Za-z_]\w*[ \t\r\n*]+)+"
    r"(?P<name>[A-Za-z_]\w*)[ \t\r\n]*\([^;{}]*\)[ \t\r\n]*\{"
)
FUNCTION = re.compile(
    r"(?m)^[ \t]*(?!(?:static[ \t]+)?asm\b)(?:[A-Za-z_]\w*[ \t\r\n*]+)+"
    r"(?P<name>[A-Za-z_]\w*)[ \t\r\n]*\([^;{}]*\)[ \t\r\n]*\{"
)
INLINE_ASM = re.compile(r"\basm\s*(?:volatile\s*)?\{")
ASM_TOKEN = re.compile(r"\basm\b")
INC_INCLUDE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"[^"\r\n]*\.inc"')
MACRO_DEFINE = re.compile(r"(?m)^[ \t]*#[ \t]*define[ \t]+(?P<name>[A-Za-z_]\w*)")
IDENTIFIER = re.compile(r"\b[A-Za-z_]\w*\b")
LABEL_PREFIX = re.compile(r"^(?P<label>[.A-Za-z_]\w*):(?P<rest>.*)$")
LOCAL_BRANCHES = {"b", "beq", "bne", "beq-", "bne-", "beq+", "bne+"}

# The C preprocessor runs before MWCC parses an asm block. Redefining one of
# these tokens can make the source text pass this scanner while emitting a
# different symbol, opcode, or register. No active source/header currently
# defines one, so additions are rejected fail-closed.
ASM_PROTECTED_MACROS = (
    HARDWARE_ALLOWED
    | DOLPHIN_PAIRED_SINGLE_ALLOWED
    | DOLPHIN_PAIRED_SINGLE_FUNCTIONS
    | {"asm", "nofralloc", "fralloc", "entry"}
    | {mnemonic for entry in AUTHENTIC_ASM.values() for mnemonic in entry.get("mnemonics", [])}
    | {func for _, func in AUTHENTIC_ASM}
    | {f"r{i}" for i in range(32)}
    | {f"f{i}" for i in range(32)}
    | {f"qr{i}" for i in range(8)}
    | {f"cr{i}" for i in range(8)}
)
ASM_PROTECTED_MACROS_LOWER = {name.lower() for name in ASM_PROTECTED_MACROS}


def _mask_non_code(source: str) -> str:
    """Blank comments and literals while preserving offsets and newlines."""
    out = []
    state = "code"
    quote = ""
    i = 0
    while i < len(source):
        ch = source[i]
        nxt = source[i + 1] if i + 1 < len(source) else ""
        if state == "code":
            if ch == "/" and nxt == "/":
                out.extend((" ", " "))
                state = "line_comment"
                i += 2
                continue
            if ch == "/" and nxt == "*":
                out.extend((" ", " "))
                state = "block_comment"
                i += 2
                continue
            if ch in ('"', "'"):
                quote = ch
                out.append(" ")
                state = "literal"
                i += 1
                continue
            out.append(ch)
            i += 1
            continue

        if state == "line_comment":
            if ch == "\n":
                out.append("\n")
                state = "code"
            else:
                out.append(" ")
            i += 1
            continue

        if state == "block_comment":
            if ch == "*" and nxt == "/":
                out.extend((" ", " "))
                state = "code"
                i += 2
            else:
                out.append("\n" if ch == "\n" else " ")
                i += 1
            continue

        # String or character literal.
        if ch == "\\" and nxt:
            out.append(" ")
            out.append("\n" if nxt == "\n" else " ")
            i += 2
        elif ch == quote:
            out.append(" ")
            state = "code"
            i += 1
        else:
            out.append("\n" if ch == "\n" else " ")
            i += 1

    return "".join(out)


def _matching_brace(masked: str, opening: int) -> int | None:
    depth = 0
    for i in range(opening, len(masked)):
        if masked[i] == "{":
            depth += 1
        elif masked[i] == "}":
            depth -= 1
            if depth == 0:
                return i
    return None


def _line_number(source: str, offset: int) -> int:
    return source.count("\n", 0, offset) + 1


def _function_regions(source: str, masked: str) -> list[dict]:
    regions = []
    for match in FUNCTION.finditer(masked):
        opening = masked.find("{", match.start(), match.end())
        closing = _matching_brace(masked, opening)
        if closing is None:
            continue
        regions.append({
            "func": match.group("name"),
            "start": match.start(),
            "end": closing,
        })
    return regions


def find_asm_regions(source: str) -> list[dict]:
    """Return whole-function and inline asm regions with enclosing symbols."""
    masked = _mask_non_code(source)
    functions = _function_regions(source, masked)
    regions = []

    for match in ASM_FUNCTION.finditer(masked):
        opening = masked.find("{", match.start(), match.end())
        closing = _matching_brace(masked, opening)
        end = closing if closing is not None else len(source) - 1
        regions.append({
            "func": match.group("name"),
            "body": source[opening + 1:end],
            "start_line": _line_number(source, match.start()),
            "end_line": _line_number(source, end),
            "closed": closing is not None,
        })

    for match in INLINE_ASM.finditer(masked):
        opening = masked.find("{", match.start(), match.end())
        closing = _matching_brace(masked, opening)
        end = closing if closing is not None else len(source) - 1
        enclosing = [
            region for region in functions
            if region["start"] <= match.start() <= region["end"]
        ]
        func = max(enclosing, key=lambda region: region["start"])["func"] if enclosing else ""
        regions.append({
            "func": func,
            "body": source[opening + 1:end],
            "start_line": _line_number(source, match.start()),
            "end_line": _line_number(source, end),
            "closed": closing is not None,
        })

    return regions


def allowlist_for(path: str, func: str) -> tuple[set[str], str, bool]:
    if path == DOLPHIN_PAIRED_SINGLE_PATH and func in DOLPHIN_PAIRED_SINGLE_FUNCTIONS:
        return DOLPHIN_PAIRED_SINGLE_ALLOWED, "Dolphin paired-single SDK", True
    entry = AUTHENTIC_ASM.get((path, func))
    if entry is not None:
        problems = evidence_problems(entry)
        if problems:
            for problem in problems:
                print(f"::error::authentic-asm evidence: {problem}")
            return set(), "authentic library asm (evidence incomplete)", False
        return {m.lower() for m in entry["mnemonics"]}, "authentic library asm", False
    return HARDWARE_ALLOWED, "hardware-primitive", False


def _asm_statements(body: str) -> tuple[list[str], set[str]]:
    """Split MWCC asm statements and collect labels without losing semicolon ops."""
    statements = []
    labels = set()
    for raw in _mask_non_code(body).splitlines():
        line = raw.strip()
        if not line:
            continue
        # A leading # is a C preprocessor directive, not an assembler comment.
        # Preserve it so the allowlist rejects it. Elsewhere # starts a comment.
        if not line.startswith("#"):
            line = line.split("#", 1)[0]
        for raw_statement in line.split(";"):
            statement = raw_statement.strip()
            while statement:
                match = LABEL_PREFIX.match(statement)
                if not match:
                    break
                labels.add(match.group("label"))
                statement = match.group("rest").strip()
            if statement:
                statements.append(statement)
    return statements, labels


def declared_branch_target(path: str, func: str, target: str) -> bool:
    """A registered library routine may branch to a function it declares (or to itself)."""
    entry = AUTHENTIC_ASM.get((path, func))
    if entry is None or not path.startswith(LIBRARY_ASM_PREFIXES) or evidence_problems(entry):
        return False
    if target == func:
        return True
    return target in (entry.get("branch_targets") or []) and target in function_symbols()


def asm_body_ok(body: str, path: str = "", func: str = "") -> bool:
    allowed, allowlist_name, require_paired_single = allowlist_for(path, func)
    saw_paired_single = False
    statements, labels = _asm_statements(body)
    for line in statements:
        if line in ("{", "}", "nofralloc", "fralloc"):
            continue
        mnemonic = line.split()[0].lower()
        if mnemonic.startswith(("ps_", "psq_")):
            saw_paired_single = True
        if mnemonic not in allowed:
            print(
                f"::error::asm instruction '{mnemonic}' outside "
                f"{allowlist_name} allowlist for {path}:{func}: {line}"
            )
            return False
        if mnemonic in LOCAL_BRANCHES:
            parts = line.split(None, 1)
            operands = parts[1].strip() if len(parts) == 2 else ""
            # Conditional PPC branches may select a condition-register field
            # (for example, `bne+ cr1, local_label`). The final operand is
            # still required to be a label in this exact asm body.
            target = operands.rsplit(",", 1)[-1].strip()
            if target not in labels and not declared_branch_target(path, func, target):
                print(
                    f"::error::asm branch target '{target}' is not a label "
                    f"defined inside {path}:{func}"
                )
                return False
    if require_paired_single and not saw_paired_single:
        print(f"::error::{path}:{func} uses paired-single exception without a ps_/psq_ instruction")
        return False
    return True


def added_lines_from_diff(diff: str) -> dict[str, list[tuple[int, str]]]:
    added: dict[str, list[tuple[int, str]]] = {}
    current_file = None
    new_line = None
    for line in diff.splitlines():
        if line.startswith("+++ b/"):
            current_file = line[6:]
            added.setdefault(current_file, [])
            continue
        match = HUNK.match(line)
        if match:
            new_line = int(match.group(1))
            continue
        if current_file is None or new_line is None:
            continue
        if line.startswith("+") and not line.startswith("+++"):
            added[current_file].append((new_line, line[1:]))
            new_line += 1
        elif line.startswith("-") and not line.startswith("---"):
            continue
        elif not line.startswith("\\"):
            new_line += 1
    return added


def scan_source(path: str, source: str, added_lines: list[tuple[int, str]]) -> bool:
    """Validate every asm function/block touched by an added diff line."""
    added_numbers = {line_number for line_number, _ in added_lines}
    masked = _mask_non_code(source)
    source_lines = source.splitlines()
    masked_lines = masked.splitlines()
    regions = find_asm_regions(source)
    touched = [
        region for region in regions
        if any(region["start_line"] <= line <= region["end_line"] for line in added_numbers)
    ]

    fail = False
    macro_definitions = [
        (match.group("name"), _line_number(masked, match.start()))
        for match in MACRO_DEFINE.finditer(masked)
    ]
    for name, line_number in macro_definitions:
        if line_number in added_numbers and name.lower() in ASM_PROTECTED_MACROS_LOWER:
            print(f"::error::asm-sensitive macro '{name}' added in {path}:{line_number}")
            fail = True

    for line_number, _ in added_lines:
        if not 1 <= line_number <= len(source_lines):
            print(f"::error::cannot map added line {path}:{line_number} to HEAD source")
            fail = True
            continue
        text = source_lines[line_number - 1]
        code = masked_lines[line_number - 1]
        include = INC_INCLUDE.match(text)
        hash_index = text.find("#", 0, include.end()) if include else -1
        if include and code[hash_index] == "#":
            print(
                f"::error::.inc include added in {path}:{line_number} "
                "— raw-asm shim, not a real match."
            )
            fail = True
        if ASM_TOKEN.search(code) and not any(
            region["start_line"] <= line_number <= region["end_line"] for region in regions
        ):
            print(f"::error::unparseable or unterminated asm block in {path}:{line_number}")
            fail = True

    touched_funcs = {region["func"] for region in touched}
    for func in sorted(touched_funcs):
        func_regions = [region for region in regions if region["func"] == func]
        if not func or any(not region["closed"] for region in func_regions):
            print(f"::error::cannot safely map asm block to a closed function in {path}")
            fail = True
            continue
        entry = AUTHENTIC_ASM.get((path, func))
        if entry is not None:
            first = min(region["start_line"] for region in func_regions)
            above = "\n".join(source_lines[max(0, first - 41):first - 1])
            if entry.get("evidence", "\0") not in above:
                print(
                    f"::error::authentic asm {path}:{func} must cite its evidence document "
                    f"({entry.get('evidence')}) in a comment within 40 lines above it"
                )
                fail = True
                continue
        combined_body = "\n".join(region["body"] for region in func_regions)
        asm_identifiers = set(IDENTIFIER.findall(combined_body)) | {func}
        shadowed = sorted({name for name, _ in macro_definitions} & asm_identifiers)
        if shadowed:
            print(
                f"::error::C preprocessor macro(s) can rewrite asm in {path}:{func}: "
                + ", ".join(shadowed)
            )
            fail = True
            continue
        if asm_body_ok(combined_body, path, func):
            _, allowlist_name, _ = allowlist_for(path, func)
            print(f"asm in {func} ({path}): {allowlist_name} allowlist OK")
        else:
            print(
                f"::error::asm-wrapper/inline-asm in {path}:{func} is outside "
                "the authentic SDK exceptions"
            )
            fail = True
    return not fail


def main() -> int:
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} BASE HEAD", file=sys.stderr)
        return 2
    base, head = sys.argv[1], sys.argv[2]
    diff = subprocess.run(
        [
            "git", "diff", "--unified=0", f"{base}...{head}", "--",
            "src/**/*.c", "src/**/*.h",
        ],
        capture_output=True,
        text=True,
        check=True,
    ).stdout
    added_by_path = added_lines_from_diff(diff)
    fail = False

    for path, added_lines in added_by_path.items():
        try:
            source = subprocess.run(
                ["git", "show", f"{head}:{path}"],
                capture_output=True,
                text=True,
                check=True,
            ).stdout
        except subprocess.CalledProcessError:
            print(f"::error::cannot read changed source {path} at {head}")
            fail = True
            continue
        if not scan_source(path, source, added_lines):
            fail = True

    if not fail:
        print("No asm-wrapper / inline-asm / .inc cheats in added source.")
    return int(fail)


if __name__ == "__main__":
    sys.exit(main())
