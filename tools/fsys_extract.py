#!/usr/bin/env python3
"""List and extract members of a Genius Sonority FSYS archive.

The layout below is the one the game's own loader walks (see
src/game/fsys/fsys_file.c fn_8017E30C and the header copy in
_fsysGetFilename, which DMA-copies the first 0x40 bytes of the archive
into the FSYSSlot):

Archive header (big-endian u32 words, 0x40 bytes):
    0x00  'FSYS'
    0x08  archive id            (used as the cache key, *(u32*)(archive + 8))
    0x0C  member count          (FSYSSlot.numEntries)
    0x18  offset of the table-of-tables (fn_8017E30C: *(archive + 0x18))
    0x20  archive size
Table-of-tables at header[0x18]:
    [0]   offset of the per-member entry-offset table
    [1]   offset of the member name strings
    [2]   offset of the member data region
Member entry (reached as archive + entryTable[i]):
    0x00  member id / name hash (low byte 2 is the file type)
    0x04  data offset in the archive (DVD read offset in the loader)
    0x08  unpacked size        (output allocation for the LZSS decode)
    0x0C  flags, bit 31 = LZSS packed
    0x14  packed size          (bytes read from disc)
    0x1C  offset of an optional loose-file override path (0 = none)
    0x20  resource group id    (decomp-pool / callback key)
    0x24  offset of the member name string

A packed member starts with a 16-byte header copied to lbl_80453FDC:
'LZSS', unpacked size, packed size (header included), 0. The stream after
it is decoded by fn_8017F2C4, Okumura's LZSS with N = 4096, F = 18,
THRESHOLD = 2, a zero-filled window and r starting at 0xFEE.

Usage:
    fsys_extract.py list ARCHIVE.fsys
    fsys_extract.py extract ARCHIVE.fsys [-o OUTDIR] [NAME ...]

Extracted members are named NAME.EXT, where EXT comes from the file type
byte when it is known (0x1C -> rel) and is 'bin' otherwise. Extracted RELs
are sanity-checked (module id, section table and import table must parse).
"""

import argparse
import struct
import sys
from pathlib import Path
from typing import Dict, List, NamedTuple, Optional

FSYS_MAGIC = b"FSYS"
LZSS_MAGIC = b"LZSS"
FLAG_PACKED = 0x80000000

# Member file types, from the id's third byte. Only types verified against
# the extracted contents are listed here.
TYPE_EXTENSIONS: Dict[int, str] = {
    0x1C: "rel",
}


class Member(NamedTuple):
    index: int
    member_id: int
    name: str
    data_offset: int
    unpacked_size: int
    packed_size: int
    flags: int
    group: int

    @property
    def file_type(self) -> int:
        return (self.member_id >> 8) & 0xFF

    @property
    def packed(self) -> bool:
        return bool(self.flags & FLAG_PACKED)

    @property
    def filename(self) -> str:
        return f"{self.name}.{TYPE_EXTENSIONS.get(self.file_type, 'bin')}"


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from(">I", data, offset)[0]


def c_string(data: bytes, offset: int) -> str:
    end = data.index(b"\0", offset)
    return data[offset:end].decode("ascii", errors="replace")


def parse_fsys(data: bytes) -> List[Member]:
    if data[:4] != FSYS_MAGIC:
        raise ValueError("not an FSYS archive (bad magic)")
    count = u32(data, 0x0C)
    tables = u32(data, 0x18)
    entry_table = u32(data, tables + 0)
    members = []
    for i in range(count):
        entry = u32(data, entry_table + 4 * i)
        members.append(
            Member(
                index=i,
                member_id=u32(data, entry + 0x00),
                name=c_string(data, u32(data, entry + 0x24)),
                data_offset=u32(data, entry + 0x04),
                unpacked_size=u32(data, entry + 0x08),
                flags=u32(data, entry + 0x0C),
                packed_size=u32(data, entry + 0x14),
                group=u32(data, entry + 0x20),
            )
        )
    return members


def lzss_decode(stream: bytes, out_size: int) -> bytes:
    """fn_8017F2C4: decode a packed member, header included."""
    if stream[:4] != LZSS_MAGIC:
        raise ValueError("packed member lacks an LZSS header")
    in_end = u32(stream, 8)  # packed size, header included
    window = bytearray(0x1000)
    r = 0xFEE
    out = bytearray()
    pos = 0x10
    flags = 0

    def take() -> Optional[int]:
        nonlocal pos
        if pos >= in_end:
            return None
        value = stream[pos]
        pos += 1
        return value

    while True:
        flags >>= 1
        if not flags & 0x100:
            c = take()
            if c is None:
                break
            flags = c | 0xFF00
        if flags & 1:
            c = take()
            if c is None:
                break
            out.append(c)
            window[r] = c
            r = (r + 1) & 0xFFF
        else:
            i = take()
            j = take() if i is not None else None
            if j is None:
                break
            i |= (j & 0xF0) << 4
            j = (j & 0x0F) + 2
            for k in range(j + 1):
                c = window[(i + k) & 0xFFF]
                out.append(c)
                window[r] = c
                r = (r + 1) & 0xFFF
    if len(out) != out_size:
        raise ValueError(f"LZSS size mismatch: got {len(out):#x}, expected {out_size:#x}")
    return bytes(out)


def member_data(data: bytes, member: Member) -> bytes:
    raw = data[member.data_offset : member.data_offset + member.packed_size]
    if not member.packed:
        return raw[: member.unpacked_size]
    header_size = u32(raw, 4)
    if header_size != member.unpacked_size:
        raise ValueError(f"{member.name}: LZSS header size disagrees with the entry")
    return lzss_decode(raw, member.unpacked_size)


def describe_rel(data: bytes) -> str:
    """Parse a REL header, section table and import table; raise if malformed."""
    module_id = u32(data, 0x00)
    num_sections = u32(data, 0x0C)
    section_info = u32(data, 0x10)
    version = u32(data, 0x1C)
    imp_offset = u32(data, 0x28)
    imp_size = u32(data, 0x2C)
    if version not in (1, 2, 3) or not 0 < num_sections < 64:
        raise ValueError("not a REL (bad version or section count)")
    if section_info + num_sections * 8 > len(data):
        raise ValueError("REL section table out of range")
    sections = []
    for i in range(num_sections):
        offset, size = struct.unpack_from(">II", data, section_info + 8 * i)
        if size == 0:
            continue
        exe = offset & 1
        offset &= ~1
        if offset and offset + size > len(data):
            raise ValueError(f"REL section {i} out of range")
        sections.append(f"{i}:{'x' if exe else 'd'}{size:#x}")
    if imp_size % 8 or imp_offset + imp_size > len(data):
        raise ValueError("REL import table out of range")
    imports = []
    for i in range(imp_size // 8):
        imp_module, rel_offset = struct.unpack_from(">II", data, imp_offset + 8 * i)
        if rel_offset >= len(data):
            raise ValueError("REL relocation offset out of range")
        imports.append(str(imp_module))
    return (
        f"REL module {module_id} v{version}, {num_sections} sections "
        f"[{' '.join(sections)}], imports from [{', '.join(imports)}]"
    )


def cmd_list(args: argparse.Namespace) -> int:
    data = Path(args.archive).read_bytes()
    print(f"{'#':>3} {'id':>10} {'type':>4} {'group':>5} {'offset':>8} {'packed':>8} {'size':>8}  name")
    for m in parse_fsys(data):
        print(
            f"{m.index:3} {m.member_id:#010x} {m.file_type:#04x} {m.group:5} "
            f"{m.data_offset:#8x} {m.packed_size:#8x} {m.unpacked_size:#8x}  "
            f"{m.name}{'' if m.packed else ' (stored)'}"
        )
    return 0


def cmd_extract(args: argparse.Namespace) -> int:
    data = Path(args.archive).read_bytes()
    out_dir = Path(args.output)
    out_dir.mkdir(parents=True, exist_ok=True)
    wanted = set(args.names)
    found = set()
    for m in parse_fsys(data):
        if wanted and m.name not in wanted and m.filename not in wanted:
            continue
        found.add(m.name)
        contents = member_data(data, m)
        path = out_dir / m.filename
        path.write_bytes(contents)
        note = ""
        if m.file_type == 0x1C:
            note = "  " + describe_rel(contents)
        print(f"{path} ({len(contents):#x} bytes){note}")
    missing = {n for n in wanted if n not in found and n.rsplit(".", 1)[0] not in found}
    if missing:
        print(f"error: no member named {', '.join(sorted(missing))}", file=sys.stderr)
        return 1
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    sub = parser.add_subparsers(dest="command", required=True)
    p_list = sub.add_parser("list", help="list archive members")
    p_list.add_argument("archive")
    p_list.set_defaults(func=cmd_list)
    p_extract = sub.add_parser("extract", help="extract (and unpack) members")
    p_extract.add_argument("archive")
    p_extract.add_argument("names", nargs="*", help="member names (default: all)")
    p_extract.add_argument("-o", "--output", default=".", help="output directory")
    p_extract.set_defaults(func=cmd_extract)
    args = parser.parse_args()
    try:
        return args.func(args)
    except (ValueError, struct.error) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
