#!/usr/bin/env python3
"""Export an existing MWCC literal without changing allocated section bytes."""

import argparse
import re
import struct
import subprocess
import tempfile
from pathlib import Path


def read_object(blob):
    if blob[:6] != b"\x7fELF\x01\x02":
        raise ValueError("expected a big-endian ELF32 object")
    header = struct.unpack_from(">16sHHIIIIIHHHHHH", blob)
    if header[1:3] != (1, 20) or header[11] != 40:
        raise ValueError("expected a relocatable PowerPC object")
    records = [struct.unpack_from(">10I", blob, header[6] + i * 40)
               for i in range(header[12])]

    def payload(record):
        return blob[record[4]:record[4] + record[5]]

    def name(table, offset):
        end = table.index(b"\0", offset)
        return table[offset:end].decode("ascii")

    names = payload(records[header[13]])
    sections = {name(names, r[0]): (i, r, payload(r))
                for i, r in enumerate(records)}
    symbols = []
    for _, r, data in sections.values():
        if r[1] != 2:
            continue
        if r[9] != 16:
            raise ValueError("unexpected symbol-table entry size")
        strings = payload(records[r[6]])
        for offset in range(0, len(data), 16):
            n, value, size, info, _, section = struct.unpack_from(">IIIBBH", data, offset)
            symbols.append((name(strings, n), value, size, info, section))
    allocated = {n: (r[2], r[5], r[8], data) for n, (_, r, data) in sections.items()
                 if r[2] & 2 and r[1] != 8}
    return sections, symbols, allocated


def export_constant(path, objcopy, symbol, offset, expected):
    if not re.fullmatch(r"[A-Za-z_][A-Za-z_0-9]*", symbol):
        raise ValueError("invalid export name")
    if not expected or offset < 0:
        raise ValueError("expected constant bytes and a nonnegative offset")
    sections, symbols, allocated = read_object(path.read_bytes())
    index, _, data = sections[".sdata2"]
    if data[offset:offset + len(expected)] != expected:
        raise ValueError("compiler literal does not match the registered constant")
    exported = (symbol, offset, len(expected), 0x11, index)
    existing = [s for s in symbols if s[0] == symbol]
    if existing:
        if existing != [exported]:
            raise ValueError("export name already denotes a different object")
        return
    literals = [s for s in symbols if s[0].startswith("@") and
                s[1:] == (offset, len(expected), 1, index)]
    if len(literals) != 1:
        raise ValueError("expected exactly one local compiler literal at this offset")
    with tempfile.TemporaryDirectory(dir=path.parent) as directory:
        output = Path(directory) / path.name
        subprocess.run([str(objcopy), "--redefine-sym", literals[0][0] + "=" + symbol,
                        "--globalize-symbol", symbol, str(path), str(output)], check=True)
        after_sections, after_symbols, after_allocated = read_object(output.read_bytes())
        exported = (symbol, offset, len(expected), 0x11, after_sections[".sdata2"][0])
        if allocated != after_allocated or exported not in after_symbols:
            raise ValueError("constant export changed section bytes or failed to export")
        output.replace(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--objcopy", required=True, type=Path)
    parser.add_argument("--object", required=True, type=Path)
    parser.add_argument("--symbol", required=True)
    parser.add_argument("--offset", required=True, type=lambda s: int(s, 0))
    parser.add_argument("--expected", required=True, type=bytes.fromhex)
    args = parser.parse_args()
    export_constant(args.object, args.objcopy, args.symbol, args.offset, args.expected)


if __name__ == "__main__":
    main()
