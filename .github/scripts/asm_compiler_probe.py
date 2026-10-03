#!/usr/bin/env python3
"""Compiler probe: show that no MWCC build reproduces a retail routine from C.

Evidence for first-party (game-code) assembly, where no other decompilation
exists to cite. The probe compiles a C candidate for the routine with every
GameCube MWCC version in build/compilers/GC, using the unit's own flags from
build.ninja, and compares the routine's instruction words with the retail
object. Relocated fields are masked on both sides, so only real instruction
differences count.

    python3 .github/scripts/asm_compiler_probe.py \
        --unit-src src/game/gs_scratch_r57_800EEDF8_o2.c \
        --func GSscratchInit \
        --candidate docs/asm_evidence/probes/GSscratchInit.c \
        --out docs/asm_evidence/probes/GSscratchInit.txt

The report lists every compiler with its result and ends with
"verdict: no-match" (no compiler reproduced the retail words) or
"verdict: MATCH <version>" (the routine can be C, so asm is not admissible).
It needs a local build with the retail disc extracted.
"""

import argparse
import re
import shlex
import subprocess
import sys
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
OBJDUMP = REPO_ROOT / "build" / "binutils" / "powerpc-eabi-objdump"
WIBO = REPO_ROOT / "build" / "tools" / "wibo"
SJISWRAP = REPO_ROOT / "build" / "tools" / "sjiswrap.exe"
COMPILERS = REPO_ROOT / "build" / "compilers" / "GC"


def unit_build(unit_src: str) -> tuple[str, str, Path]:
    """cflags, default compiler version and retail object for a unit source."""
    ninja = (REPO_ROOT / "build.ninja").read_text(encoding="utf-8")
    stem = unit_src.removeprefix("src/").removesuffix(".c")
    block = re.search(
        rf"(?ms)^build build/GC6E01/src/{re.escape(stem)}\.o: .*?(?=^build |\Z)", ninja)
    if not block:
        sys.exit(f"{unit_src}: no compile rule in build.ninja (run configure.py)")
    text = block.group(0).replace("$\n", " ")
    version = re.search(r"mw_version = (\S+)", text).group(1)
    cflags = re.search(r"cflags = (.*)", text).group(1).strip()
    retail = REPO_ROOT / "build" / "GC6E01" / "obj" / f"{stem}.o"
    return cflags, version, retail


def function_words(obj: Path, func: str) -> list[int] | None:
    """Instruction words of `func`, with relocated fields masked out."""
    dump = subprocess.run([str(OBJDUMP), "-dr", str(obj)],
                          capture_output=True, text=True).stdout
    lines = dump.splitlines()
    start = next((i for i, l in enumerate(lines) if re.match(rf"^[0-9a-f]+ <{re.escape(func)}>:$", l)), None)
    if start is None:
        return None
    words: list[int] = []
    for line in lines[start + 1:]:
        if re.match(r"^[0-9a-f]+ <", line) or not line.strip():
            if words:
                break
            continue
        reloc = re.match(r"^\s+[0-9a-f]+:\s+R_PPC_(\S+)", line)
        if reloc and words:
            kind = reloc.group(1)
            mask = 0xFC000003 if kind in ("REL24", "ADDR24") else (
                0xFFFF0000 if kind.startswith(("ADDR16", "EMB_SDA21", "SDAREL16", "REL14")) else 0)
            words[-1] &= mask
            continue
        insn = re.match(r"^\s+[0-9a-f]+:\s+((?:[0-9a-f]{2} ){4})", line)
        if insn:
            words.append(int(insn.group(1).replace(" ", ""), 16))
    return words


def compile_candidate(version: str, cflags: str, candidate: Path, out: Path) -> str | None:
    compiler = COMPILERS / version / "mwcceppc.exe"
    if not compiler.is_file():
        return "compiler missing"
    cmd = [str(WIBO), str(SJISWRAP), str(compiler), *shlex.split(cflags), "-c", str(candidate), "-o", str(out)]
    result = subprocess.run(cmd, capture_output=True, text=True, cwd=REPO_ROOT)
    if result.returncode != 0 or not out.is_file():
        return "compile error"
    return None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--unit-src", required=True)
    parser.add_argument("--func", required=True)
    parser.add_argument("--candidate", required=True, type=Path)
    parser.add_argument("--out", required=True, type=Path)
    args = parser.parse_args()

    cflags, unit_version, retail_obj = unit_build(args.unit_src)
    retail = function_words(retail_obj, args.func)
    if retail is None:
        sys.exit(f"{args.func} not found in retail object {retail_obj}")
    versions = sorted(p.name for p in COMPILERS.iterdir() if (p / "mwcceppc.exe").is_file())

    report = [
        f"# Compiler probe: {args.func}",
        f"unit: {args.unit_src} (unit compiler GC/{unit_version.split('/')[-1]})",
        f"candidate: {args.candidate}",
        f"flags: {' '.join(cflags.split())}",
        f"retail: {len(retail)} instructions",
        "",
    ]
    matched = None
    compared = 0
    with tempfile.TemporaryDirectory() as tmp:
        for version in versions:
            obj = Path(tmp) / f"{version}.o"
            error = compile_candidate(version, cflags, REPO_ROOT / args.candidate, obj)
            if error:
                report.append(f"GC/{version}: {error}")
                continue
            words = function_words(obj, args.func)
            if words is None:
                report.append(f"GC/{version}: {args.func} not emitted")
                continue
            compared += 1
            same = sum(1 for a, b in zip(words, retail) if a == b)
            if words == retail:
                report.append(f"GC/{version}: MATCH")
                matched = matched or version
            else:
                report.append(f"GC/{version}: differs ({len(words)} vs {len(retail)} instructions, "
                              f"{same} positions equal)")
    report.append("")
    if matched:
        report.append(f"verdict: MATCH GC/{matched}")
    elif compared < 5:
        # A probe that compiled almost nothing proves nothing.
        report.append(f"verdict: inconclusive (only {compared} compilers produced the function)")
    else:
        report.append(f"verdict: no-match ({compared} compilers compared)")
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text("\n".join(report) + "\n", encoding="utf-8")
    print("\n".join(report))
    return 0


if __name__ == "__main__":
    sys.exit(main())
