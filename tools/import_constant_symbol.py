#!/usr/bin/env python3
"""Redirect MWCC literals to a shared constant that another unit owns.

TEMPORARY (user-approved stopgap, see docs/RULE_EXCEPTIONS.md): MWCC emits a
private copy of a literal (for example the int-to-float conversion bias) in
.sdata2, while retail shares one named copy owned by another unit.  This tool
rewrites the compiled object so every reference to the listed literals goes
through an undefined global symbol of the registered name, and drops the
literal section, so the unit no longer allocates its own copy.  No code bytes
change.  Remove it when the original source files are merged back into one
translation unit.

Only the layout "the listed literals are the whole section" is supported.
Anything else fails loudly and leaves the object untouched.
"""

import argparse
import re
import struct
from pathlib import Path

SHT_SYMTAB, SHT_STRTAB, SHT_RELA, SHT_NOBITS, SHT_REL = 2, 3, 4, 8, 9
SHN_LORESERVE = 0xFF00
STT_OBJECT, STT_SECTION = 1, 3
COMMENT_HEADER = 0x2C


def read_elf(blob):
    if blob[:6] != b"\x7fELF\x01\x02":
        raise ValueError("expected a big-endian ELF32 object")
    header = list(struct.unpack_from(">16sHHIIIIIHHHHHH", blob))
    if header[1:3] != [1, 20] or header[11] != 40:
        raise ValueError("expected a relocatable PowerPC object")
    sections = []
    for i in range(header[12]):
        fields = list(struct.unpack_from(">10I", blob, header[6] + i * 40))
        data = b"" if fields[1] == SHT_NOBITS else blob[fields[4]:fields[4] + fields[5]]
        sections.append({"fields": fields, "data": data})
    names = sections[header[13]]["data"]
    for section in sections:
        offset = section["fields"][0]
        section["name"] = names[offset:names.index(b"\0", offset)].decode("ascii")
    return header, sections


def write_elf(header, sections):
    blob = bytearray(52)
    for index in sorted(range(1, len(sections)), key=lambda i: sections[i]["fields"][4]):
        fields, data = sections[index]["fields"], sections[index]["data"]
        if fields[1] != SHT_NOBITS:
            align = max(fields[8], 1)
            blob.extend(bytes(-len(blob) % align))
            fields[4] = len(blob)
            fields[5] = len(data)
            blob.extend(data)
        else:
            fields[4] = len(blob)
    blob.extend(bytes(-len(blob) % 4))
    header[6] = len(blob)
    header[12] = len(sections)
    for section in sections:
        blob.extend(struct.pack(">10I", *section["fields"]))
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH", *header)
    return bytes(blob)


def string_at(table, offset):
    return table[offset:table.index(b"\0", offset)].decode("ascii")


def parse_symbols(sections):
    tables = [i for i, s in enumerate(sections) if s["fields"][1] == SHT_SYMTAB]
    if len(tables) != 1 or sections[tables[0]]["fields"][9] != 16:
        raise ValueError("expected exactly one symbol table with 16-byte entries")
    index = tables[0]
    strings = sections[sections[index]["fields"][6]]["data"]
    data = sections[index]["data"]
    symbols = []
    for offset in range(0, len(data), 16):
        name, value, size, info, other, shndx = struct.unpack_from(">IIIBBH", data, offset)
        symbols.append({"name": string_at(strings, name), "value": value, "size": size,
                        "info": info, "other": other, "shndx": shndx})
    return index, symbols


def parse_literals(values):
    literals = []
    for value in values:
        parts = value.split(":")
        if len(parts) == 3:
            parts.insert(0, ".sdata2")
        if len(parts) != 4:
            raise ValueError(f"invalid literal {value!r}; expected [SECTION:]SYMBOL:OFFSET:HEX")
        section, symbol, offset, expected = parts
        if section not in (".sdata2", ".sdata"):
            raise ValueError(f"unsupported literal section {section}")
        if not re.fullmatch(r"[A-Za-z_][A-Za-z_0-9]*", symbol):
            raise ValueError(f"invalid import name {symbol!r}")
        offset = int(offset, 0)
        expected = bytes.fromhex(expected)
        if offset < 0 or not expected:
            raise ValueError("expected constant bytes and a nonnegative offset")
        literals.append({"section": section, "symbol": symbol, "offset": offset,
                         "expected": expected})
    if not literals:
        raise ValueError("no literals to import")
    return literals


def check_layout(section, entries):
    """The listed literals must tile the section; only zero alignment padding between them."""
    data = section["data"]
    cursor = 0
    for entry in sorted(entries, key=lambda e: e["offset"]):
        start, size = entry["offset"], len(entry["expected"])
        if start < cursor:
            raise ValueError(f"{entry['symbol']}: overlapping literal ranges in {section['name']}")
        gap = data[cursor:start]
        if any(gap) or len(gap) >= max(size, 1) or start % size:
            raise ValueError(f"{section['name']}: bytes {cursor:#x}..{start:#x} are not "
                             "alignment padding; unsupported layout (literals must be the "
                             "whole section)")
        if data[start:start + size] != entry["expected"]:
            raise ValueError(f"{entry['symbol']}: compiler literal at {section['name']}+{start:#x} "
                             f"is {data[start:start + size].hex()}, expected "
                             f"{entry['expected'].hex()}")
        cursor = start + size
    if cursor != len(data):
        raise ValueError(f"{section['name']}: bytes {cursor:#x}..{len(data):#x} are not covered "
                         "by the listed literals; unsupported layout (literals must be the "
                         "whole section)")


def import_constants(path, literals):
    blob = path.read_bytes()
    header, sections = read_elf(blob)
    names = {s["name"]: i for i, s in enumerate(sections) if i}
    symtab_index, symbols = parse_symbols(sections)
    wanted = sorted({e["section"] for e in literals})
    present = [n for n in wanted if n in names]
    if not present:
        # Already rewritten: every import must already be an undefined global.
        undefined = {s["name"] for s in symbols if s["shndx"] == 0 and s["info"] >> 4 == 1}
        if all(e["symbol"] in undefined for e in literals):
            return False
        raise ValueError("literal section is missing and the imports are not present")
    if present != wanted:
        raise ValueError("some listed literal sections are missing")
    if len({e["symbol"] for e in literals}) != len(literals):
        raise ValueError("duplicate import name")

    removed_sections = sorted(names[n] for n in wanted)
    for index in removed_sections:
        section = sections[index]
        if section["fields"][1] != 1 or not section["fields"][2] & 2:
            raise ValueError(f"{section['name']} is not an allocated PROGBITS section")
        check_layout(section, [e for e in literals if e["section"] == section["name"]])
    for section in sections:
        if section["fields"][1] == SHT_REL:
            raise ValueError("SHT_REL relocation sections are not supported")
        if section["fields"][1] == SHT_RELA and section["fields"][7] in removed_sections:
            if section["data"]:
                raise ValueError(f"{section['name']}: the literal section has relocations")

    first_global = sections[symtab_index]["fields"][7]
    if any(s["info"] >> 4 == 0 for s in symbols[first_global:]) or \
            any(s["info"] >> 4 != 0 for s in symbols[:first_global]):
        raise ValueError("symbol table is not ordered locals before globals")

    # Symbols defined in the literal sections: only local section symbols and
    # exactly one local compiler object per literal.  A global would mean the
    # unit exports the literal, so redirecting it would be wrong.
    owner = {}
    removed_symbols = set()
    for index, symbol in enumerate(symbols):
        if symbol["shndx"] not in removed_sections:
            continue
        section_name = sections[symbol["shndx"]]["name"]
        if symbol["info"] >> 4 != 0:
            raise ValueError(f"{symbol['name']} is a non-local symbol in {section_name}")
        kind = symbol["info"] & 0xF
        if kind == STT_SECTION and symbol["value"] == 0:
            removed_symbols.add(index)
            continue
        matches = [e for e in literals if e["section"] == section_name and
                   e["offset"] == symbol["value"] and len(e["expected"]) == symbol["size"]]
        if kind != STT_OBJECT or len(matches) != 1 or id(matches[0]) in owner:
            raise ValueError(f"{symbol['name']} in {section_name} does not match one "
                             "registered literal")
        owner[id(matches[0])] = index
        removed_symbols.add(index)
    for entry in literals:
        if id(entry) not in owner:
            raise ValueError(f"{entry['symbol']}: no local compiler literal at "
                             f"{entry['section']}+{entry['offset']:#x}")

    # New undefined globals (or reuse of an existing undefined reference).
    strtab_index = sections[symtab_index]["fields"][6]
    strtab = bytearray(sections[strtab_index]["data"])
    kept = [i for i in range(len(symbols)) if i not in removed_symbols]
    new_symbols = [dict(symbols[i]) for i in kept]
    old_to_new = {old: new for new, old in enumerate(kept)}
    target_index = {}
    for entry in literals:
        existing = [i for i, s in enumerate(new_symbols) if s["name"] == entry["symbol"]]
        if existing:
            symbol = new_symbols[existing[0]]
            if len(existing) != 1 or symbol["shndx"] != 0 or symbol["info"] >> 4 != 1:
                raise ValueError(f"{entry['symbol']} is already defined in this object")
            target_index[id(entry)] = existing[0]
            continue
        offset = len(strtab)
        strtab.extend(entry["symbol"].encode("ascii") + b"\0")
        new_symbols.append({"name": entry["symbol"], "name_offset": offset, "value": 0,
                            "size": 0, "info": 0x10, "other": 0, "shndx": 0})
        target_index[id(entry)] = len(new_symbols) - 1

    # Retarget relocations.
    retargeted = 0
    for section in sections:
        fields = section["fields"]
        if fields[1] != SHT_RELA or fields[7] in removed_sections:
            continue
        if fields[6] != symtab_index or fields[9] != 12:
            raise ValueError(f"{section['name']}: unexpected relocation layout")
        data = bytearray(section["data"])
        for offset in range(0, len(data), 12):
            r_offset, r_info, addend = struct.unpack_from(">IIi", data, offset)
            old = r_info >> 8
            if old in removed_symbols:
                symbol = symbols[old]
                section_name = sections[symbol["shndx"]]["name"]
                target = symbol["value"] + addend
                hits = [e for e in literals if e["section"] == section_name and
                        e["offset"] <= target < e["offset"] + len(e["expected"])]
                if len(hits) != 1:
                    raise ValueError(f"{section['name']}+{r_offset:#x} points into "
                                     f"{section_name} padding")
                new_index = target_index[id(hits[0])]
                addend = target - hits[0]["offset"]
                retargeted += 1
            else:
                new_index = old_to_new[old]
            struct.pack_into(">IIi", data, offset, r_offset, (new_index << 8) | (r_info & 0xFF),
                             addend)
        section["data"] = bytes(data)
    if not retargeted:
        raise ValueError("no relocation references the listed literals")

    def section_index(old):
        return old - sum(1 for r in removed_sections if r < old)

    # MWCC's .comment carries one 8-byte entry per symbol; keep it in step.
    if ".comment" in names:
        comment = sections[names[".comment"]]
        data = comment["data"]
        if not data.startswith(b"CodeWarrior") or \
                len(data) != COMMENT_HEADER + 8 * len(symbols):
            raise ValueError(".comment is not an MWCC per-symbol table")
        entries = [data[COMMENT_HEADER + 8 * i:COMMENT_HEADER + 8 * i + 8] for i in kept]
        entries += [bytes(8)] * (len(new_symbols) - len(kept))
        comment["data"] = data[:COMMENT_HEADER] + b"".join(entries)

    # Re-pack symbols, reusing their original string offsets.
    raw = sections[symtab_index]["data"]
    packed = bytearray()
    for new, symbol in enumerate(new_symbols):
        if new < len(kept):
            name = struct.unpack_from(">I", raw, kept[new] * 16)[0]
        else:
            name = symbol["name_offset"]
        shndx = symbol["shndx"]
        if 0 < shndx < SHN_LORESERVE:
            shndx = section_index(shndx)
        packed.extend(struct.pack(">IIIBBH", name, symbol["value"], symbol["size"],
                                  symbol["info"], symbol["other"], shndx))
    sections[symtab_index]["data"] = bytes(packed)
    sections[symtab_index]["fields"][7] = first_global - sum(
        1 for i in removed_symbols if i < first_global)
    sections[strtab_index]["data"] = bytes(strtab)

    for section in sections:
        fields = section["fields"]
        if fields[1] in (SHT_SYMTAB, SHT_RELA) or fields[6]:
            fields[6] = section_index(fields[6]) if fields[6] else 0
        if fields[1] == SHT_RELA and fields[7]:
            fields[7] = section_index(fields[7])
    header[13] = section_index(header[13])
    rewritten = [s for i, s in enumerate(sections) if i not in removed_sections]
    output = write_elf(header, rewritten)
    verify(blob, output, literals, retargeted)
    path.write_bytes(output)
    return True


def verify(before, after, literals, retargeted):
    _, old_sections = read_elf(before)
    _, new_sections = read_elf(after)
    removed = {e["section"] for e in literals}
    old_alloc = {s["name"]: s["data"] for s in old_sections
                 if s["fields"][2] & 2 and s["name"] not in removed}
    new_alloc = {s["name"]: s["data"] for s in new_sections if s["fields"][2] & 2}
    if old_alloc != new_alloc:
        raise ValueError("import changed allocated section contents")
    _, symbols = parse_symbols(new_sections)
    imports = {e["symbol"] for e in literals}
    for name in imports:
        found = [s for s in symbols if s["name"] == name]
        if len(found) != 1 or found[0]["shndx"] != 0 or found[0]["info"] != 0x10:
            raise ValueError(f"{name} is not a single undefined global after import")
    count = 0
    for section in new_sections:
        if section["fields"][1] != SHT_RELA:
            continue
        for offset in range(0, len(section["data"]), 12):
            info = struct.unpack_from(">I", section["data"], offset + 4)[0]
            if info >> 8 >= len(symbols):
                raise ValueError("relocation symbol index out of range")
            if symbols[info >> 8]["name"] in imports:
                count += 1
    if count < retargeted:
        raise ValueError("retargeted relocations were lost")


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--object", required=True, type=Path)
    parser.add_argument("--literal", action="append", default=[],
                        help="[SECTION:]SYMBOL:OFFSET:HEX (section defaults to .sdata2)")
    args = parser.parse_args()
    import_constants(args.object, parse_literals(args.literal))


if __name__ == "__main__":
    main()
