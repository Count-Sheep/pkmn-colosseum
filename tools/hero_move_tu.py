#!/usr/bin/env python3
"""Generate the whole-TU hero_move source from src/game/hero_move.c.

The hero_move TU (.text 0x8012AC9C-0x80130660) can only link as one unit
(see the link plan in src/game/hero_move.c's header). While lanes still work
on its functions in hero_move.c (built through the score-only chunk units),
this tool writes the linkable layout: everything that is not one of the TU's
functions (declarations, types, file-scope tables, static inline helpers)
first, then the TU's functions in retail address order, with the two
Matching carves (cbTsureFriend, heroMoveAddStepCallback) taken from their
carve files. ORDER also places proc1Step, the static function whose 7.0f
pools at 0x8047D064 (see its comment).

Usage:
  python3 tools/hero_move_tu.py OUT.c         # write the TU source
  python3 tools/hero_move_tu.py --refs        # list lbl_ reads left per function

A function still reading lbl_8047D0xx / lbl_80272xxx by symbol is not ready:
the TU must pool those constants itself (literals, the getResID table).
"""
import re, sys

SRC = "src/game/hero_move.c"
ORDER = ["cbTsureFriend__Fl15FootStepCounterl", "cbPoison__Fl15FootStepCounterl", "heroMoveSetLockFrame", "fn_8012B19C",
         "heroMoveChkHinderClear", "heroMoveAddAutoEvent", "heroMoveSetEventList", "heroMoveTermEvent", "heroMoveInitEvent",
         "heroMoveAddStepCallback", "proc1Step", "getStep__FP8FOOTSTEPP8_GSmodelPiP8FOOTWORK", "updateChat__F15HEROMOVE_MEMBER",
         "heroMoveCheckEvent", "updateAnimation__Ff15HEROMOVE_MEMBER", "fn_8012CA84", "heroMoveGetHeroRot", "heroMoveGetHeroPos",
         "fn_8012D39C", "fn_8012D7F0", "fn_8012DE94", "fn_8012E388", "moveLeader__F15HEROMOVE_MEMBER", "heroMoveMain",
         "heroMoveGetResID", "heroMoveSetNeckMode", "heroMoveIsMember", "heroMoveDismissMember", "fn_8012F1FC", "fn_8012F40C",
         "initFloor__Fv", "heroMoveGetKenObjID", "heroMoveInit", "heroMoveSyncWithHero", "fn_8013024C"]


def resolve_if(text):
    out = []
    stack = []
    for line in text.split('\n'):
        s = line.strip()
        if s.startswith('#if '):
            v = s[4:].strip()
            stack.append([v != '0', True])
            continue
        if s.startswith('#else') and stack:
            stack[-1][0] = not stack[-1][0]
            continue
        if s.startswith('#endif') and stack:
            stack.pop()
            continue
        if all(x[0] for x in stack):
            out.append(line)
    return '\n'.join(out)


def mask(t):
    r = list(t)
    i = 0
    n = len(t)
    while i < n:
        if t.startswith('/*', i):
            j = t.index('*/', i) + 2
            for k in range(i, j):
                if r[k] != '\n':
                    r[k] = ' '
            i = j
        elif t.startswith('//', i):
            j = t.find('\n', i)
            for k in range(i, j):
                r[k] = ' '
            i = j
        elif t[i] in '"\'':
            q = t[i]
            j = i + 1
            while t[j] != q:
                j += 2 if t[j] == '\\' else 1
            for k in range(i + 1, j):
                r[k] = ' '
            i = j + 1
        else:
            i += 1
    return ''.join(r)


def find_def(text, m, name, keep_comment=True):
    for mm in re.finditer(r'^[A-Za-z_][^;\n{]*\b' + re.escape(name) + r'\s*\(', m, re.M):
        j = mm.end()
        depth = 1
        while depth:
            if m[j] == '(':
                depth += 1
            elif m[j] == ')':
                depth -= 1
            j += 1
        k = j
        while m[k] in ' \t\n':
            k += 1
        if m[k] == ';':
            continue
        b = m.index('{', k)
        if not re.fullmatch(r'(\s*[A-Za-z_][\w \*]*;\s*)*', m[k:b]):
            continue
        d = 0
        e = b
        while True:
            if m[e] == '{':
                d += 1
            elif m[e] == '}':
                d -= 1
                if d == 0:
                    break
            e += 1
        st = mm.start()
        pre = text[:st].rstrip()
        if keep_comment and pre.endswith('*/'):
            cs = pre.rfind('/*')
            ls = text.rfind('\n', 0, cs) + 1
            if text[ls:cs].strip() == '':
                st = ls
        return st, e + 1
    return None


def refs():
    text = resolve_if(open(SRC).read())
    m = mask(text)
    pat = r'lbl_(?:8047D0[0-9A-F]{2}|80272[0-9A-F]{3})'
    for name in ORDER:
        a, b = find_def(text, m, name, keep_comment=False)
        print(f"{name:45s} {' '.join(sorted(set(re.findall(pat, m[a:b]))))}")
    for mm in re.finditer(r'^static inline [^\n]*?(\w+)\(', m, re.M):
        sp = find_def(text, m, mm.group(1), keep_comment=False)
        if sp:
            r = sorted(set(re.findall(pat, m[sp[0]:sp[1]])))
            if r:
                print(f"helper {mm.group(1):38s} {' '.join(r)}")


def main():
    if sys.argv[1:] == ["--refs"]:
        refs()
        return
    text = resolve_if(open(SRC).read())
    m = mask(text)
    spans = {}
    for name in ORDER:
        sp = find_def(text, m, name)
        if sp is None:
            print("missing", name, file=sys.stderr)
            sys.exit(1)
        spans[name] = sp
    bodies = {n: text[a:b] for n, (a, b) in spans.items()}
    rest = []
    pos = 0
    for a, b in sorted(spans.values()):
        rest.append(text[pos:a])
        pos = b
    rest.append(text[pos:])
    prologue = ''.join(rest)
    for path, name in (("src/game/hero_move_exact_8012AC9C.c", "cbTsureFriend__Fl15FootStepCounterl"),
                       ("src/game/hero_move_exact_8012BDE0.c", "heroMoveAddStepCallback")):
        t = open(path).read()
        a, b = find_def(t, mask(t), name)
        body = t[a:b]
        # the carve's file-scope prototypes become block-scope ones here, since
        # hero_move.c's file scope declares some of these names differently
        protos = [l for l in t[:a].split('\n') if l.startswith('extern ') and '(' in l]
        if protos:
            i = body.index('{') + 1
            body = body[:i] + '\n' + '\n'.join('    ' + p for p in protos) + body[i:]
        bodies[name] = body
    res = (prologue.rstrip() + "\n\n/* ===== .text 0x8012AC9C-0x80130660 in address order ===== */\n\n"
           + "\n\n".join(bodies[n].strip('\n') for n in ORDER) + "\n")
    open(sys.argv[1], "w").write(res)
    print("ok", len(res.splitlines()))


if __name__ == '__main__':
    main()
