#!/usr/bin/env python3
"""Delete the dead `#if 0` asm stubs that include `.inc` files (user decision, 2026-10-21).

Many sources still carry the asm wrapper a function had before it was
decompiled, disabled by `#if 0` and followed by the C body under `#else`:

    #if 0
    asm void fn_80193AF0(void) {
    #include "src/hsd/hsd_class_fn_80193AF0.inc"
    }
    #else
    void fn_80193AF0(void* cls) { ... }
    #endif

The `#if 0` branch never reaches the compiler. The `.inc` files no longer
exist, yet the stubs block renames: the quality scan rejects any edited line
inside an asm block that includes a `.inc`. This tool removes every `#if 0`
group whose dead branch includes a `.inc`:
  * `#if 0 ... #else BODY #endif`  -> BODY (the `#if 0`..`#else` lines and the
    matching `#endif` go; BODY is kept byte for byte);
  * `#if 0 ... #elif COND ... #endif` -> `#if COND ... #endif`;
  * `#if 0 ... #endif` -> nothing.
Nothing else changes: pragmas, comments and blank lines around the group stay.
A `.inc` include that is not inside a dead `#if 0` branch is live; the tool
reports it and changes nothing in that file (there should be none).

It also lists the `.inc` files referenced only by the deleted blocks that
exist in the tree, and removes them with --delete-inc.

Usage: tools/naming/delete_dead_stubs.py [--dry-run] [--delete-inc] [--root R] [PATH ...]
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
DIRECTIVE = re.compile(r"^[ \t]*#[ \t]*(\w+)(.*)$")
INC = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"\r\n]*\.inc)"')
IF0 = re.compile(r"^\s*0\s*(?:/\*.*\*/\s*|//.*)?$")


class Group:
    def __init__(self, start: int, dead: bool):
        self.start = start          # line index of #if
        self.dead = dead            # first branch is `#if 0`
        self.first_end = None       # index of the #else/#elif/#endif closing branch 1
        self.first_kind = None
        self.end = None             # index of #endif
        self.incs: list[str] = []   # .inc includes in the dead first branch


def scan(lines: list[str]):
    """Return (groups to delete, live .inc includes)."""
    stack: list[Group] = []
    done: list[Group] = []
    live: list[tuple[int, str]] = []
    for i, line in enumerate(lines):
        m = DIRECTIVE.match(line)
        if m:
            d, rest = m.group(1), m.group(2)
            if d in ("if", "ifdef", "ifndef"):
                stack.append(Group(i, d == "if" and bool(IF0.match(rest))))
                continue
            if d in ("else", "elif") and stack:
                g = stack[-1]
                if g.first_end is None:
                    g.first_end, g.first_kind = i, d
                continue
            if d == "endif" and stack:
                g = stack.pop()
                if g.first_end is None:
                    g.first_end, g.first_kind = i, "endif"
                g.end = i
                if g.dead and g.incs:
                    done.append(g)
                continue
        inc = INC.match(line)
        if inc:
            owner = None
            # innermost enclosing dead branch (anywhere up the stack) makes it dead
            for g in reversed(stack):
                if g.dead and g.first_end is None:
                    owner = g
                    break
            if owner is None:
                live.append((i, inc.group(1)))
            else:
                owner.incs.append(inc.group(1))
    if stack:
        raise ValueError(f"unbalanced #if at line {stack[-1].start + 1}")
    # drop groups nested inside another deleted dead branch (the outer one removes them)
    outer = []
    for g in sorted(done, key=lambda g: g.start):
        if any(o.start < g.start and g.end < o.first_end for o in outer):
            continue
        outer.append(g)
    return outer, live


def rewrite(lines: list[str], groups: list[Group]) -> list[str]:
    drop: set[int] = set()
    replace: dict[int, str] = {}
    for g in groups:
        drop.update(range(g.start, g.first_end + (0 if g.first_kind == "elif" else 1)))
        if g.first_kind == "elif":
            line = lines[g.first_end]
            replace[g.first_end] = re.sub(r"#([ \t]*)elif", r"#\1if", line, count=1)
        else:
            drop.add(g.end)
    return [replace.get(i, l) for i, l in enumerate(lines) if i not in drop]


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*", type=Path)
    ap.add_argument("--root", type=Path, default=REPO)
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--delete-inc", action="store_true", help="also git rm .inc files only the deleted blocks used")
    args = ap.parse_args()
    root = args.root.resolve()
    files = args.paths or sorted(p for d in ("src", "include") for p in (root / d).rglob("*")
                                 if p.suffix in (".c", ".cpp", ".h", ".hpp", ".cp"))
    n_blocks = n_files = 0
    dead_incs: set[str] = set()
    live_incs: set[str] = set()
    refused = []
    for path in files:
        text = path.read_text(encoding="utf-8", errors="surrogateescape")
        if ".inc" not in text:
            continue
        lines = text.splitlines(keepends=True)
        try:
            groups, live = scan(lines)
        except ValueError as e:
            refused.append(f"{path.relative_to(root)}: {e}")
            continue
        live_incs.update(inc for _, inc in live)
        if live:
            refused.append(f"{path.relative_to(root)}: live .inc include(s) at line(s) "
                           + ", ".join(str(i + 1) for i, _ in live))
            continue
        if not groups:
            continue
        for g in groups:
            dead_incs.update(g.incs)
        n_blocks += len(groups)
        n_files += 1
        if not args.dry_run:
            path.write_text("".join(rewrite(lines, groups)), encoding="utf-8", errors="surrogateescape")
    only_dead = sorted(dead_incs - live_incs)
    existing = [p for p in only_dead if (root / p).exists()]
    print(f"dead stubs: {n_blocks} block(s) in {n_files} file(s); "
          f"{len(only_dead)} distinct .inc path(s), {len(existing)} present in the tree")
    for p in existing:
        print(f"  inc: {p}")
    if args.delete_inc and existing and not args.dry_run:
        subprocess.run(["git", "rm", "-q", "--", *existing], cwd=root, check=True)
    for r in refused:
        print(f"refused: {r}", file=sys.stderr)
    return 1 if refused else 0


if __name__ == "__main__":
    sys.exit(main())
