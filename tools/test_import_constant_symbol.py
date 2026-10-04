#!/usr/bin/env python3
"""Tests for tools/import_constant_symbol.py (TEMPORARY constant_import step).

Run: python3 tools/test_import_constant_symbol.py
The compiled round-trip tests need build/compilers/GC/1.3 and build/tools/wibo
(present after a configured build) and are skipped otherwise.
"""
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import import_constant_symbol as module  # noqa: E402

BIAS = bytes.fromhex("4330000080000000")
WIBO = ROOT / "build/tools/wibo"
MWCC = ROOT / "build/compilers/GC/1.3/mwcceppc.exe"
READELF = ROOT / "build/binutils/powerpc-eabi-readelf"
OBJDUMP = ROOT / "build/binutils/powerpc-eabi-objdump"
CFLAGS = ["-nodefaults", "-proc", "gekko", "-align", "powerpc", "-enum", "int",
          "-fp", "hardware", "-Cpp_exceptions", "off", "-O4,p", "-inline", "auto",
          "-nosyspath", "-RTTI", "off", "-fp_contract", "on", "-str", "reuse",
          "-multibyte", "-sdata", "8", "-sdata2", "8"]

CONVERT = b"float convert(int x) { return (float)x; }\n"
CONVERT_ZERO = (b"float convert(int x) { return (float)x; }\n"
                b"float zero(int x) { return x ? 1.5f : 0.0f; }\n")
OWN_DATA = b"float convert(int x) { return (float)x + 3.0f; }\n"


def compile_source(directory, source):
    path = Path(directory) / "unit.c"
    path.write_bytes(source)
    subprocess.run([str(WIBO), str(MWCC), *CFLAGS, "-c", str(path), "-o", str(directory)],
                   check=True, capture_output=True)
    return Path(directory) / "unit.o"


def fixture(literal=BIAS, extra=b""):
    """A hand-built MWCC-shaped object: .text, .sdata2, .rela.text, symtab, .comment."""
    names = ["", ".text", ".sdata2", ".rela.text", ".symtab", ".strtab", ".shstrtab", ".comment"]
    shstr = b"\0"
    offsets = []
    for name in names:
        offsets.append(len(shstr))
        shstr += name.encode() + b"\0"
    strtab = b"\0@16\0convert\0"
    symbols = (bytes(16)
               + struct.pack(">IIIBBH", 0, 0, 0, 3, 0, 2)             # section .sdata2
               + struct.pack(">IIIBBH", 1, 0, 8, 1, 0, 2)             # @16 local object
               + struct.pack(">IIIBBH", 5, 0, 8, 0x12, 0, 1))         # convert global func
    rela = struct.pack(">IIi", 2, (2 << 8) | 0x6D, 0) + struct.pack(">IIi", 6, (1 << 8) | 0x6D, 4)
    comment = b"CodeWarrior" + bytes(0x2C - 11) + b"".join(
        struct.pack(">IBBH", a, 0, 0, 0) for a in (0, 8, 8, 4))
    payloads = [b"", b"\x60\x00\x00\x00" * 2, literal + extra, rela, symbols, strtab, shstr, comment]
    types = [0, 1, 1, 4, 2, 3, 3, 1]
    flags = [0, 6, 3, 0, 0, 0, 0, 0]
    links = [0, 0, 0, 4, 5, 0, 0, 0]
    infos = [0, 0, 0, 1, 3, 0, 0, 0]
    aligns = [0, 4, 8, 4, 4, 1, 1, 1]
    entsizes = [0, 0, 0, 12, 16, 0, 0, 0]
    blob = bytearray(52)
    records = []
    for i, payload in enumerate(payloads):
        blob.extend(bytes(-len(blob) % max(aligns[i], 1)))
        records.append(struct.pack(">10I", offsets[i], types[i], flags[i], 0, len(blob),
                                   len(payload), links[i], infos[i], aligns[i], entsizes[i]))
        blob.extend(payload)
    blob.extend(bytes(-len(blob) % 4))
    shoff = len(blob)
    blob.extend(b"".join(records))
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH", b"\x7fELF\x01\x02\x01" + bytes(9),
                            1, 20, 1, 0, 0, shoff, 0, 52, 0, 0, 40, len(names), 6)
    return bytes(blob)


def relocations(path):
    _, sections = module.read_elf(path.read_bytes())
    _, symbols = module.parse_symbols(sections)
    result = []
    for section in sections:
        if section["fields"][1] == module.SHT_RELA:
            for offset in range(0, len(section["data"]), 12):
                r_offset, info, addend = struct.unpack_from(">IIi", section["data"], offset)
                result.append((section["name"], r_offset, info & 0xFF,
                               symbols[info >> 8]["name"], addend))
    return result


class FixtureTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name) / "unit.o"

    def run_import(self, *literals):
        return module.import_constants(self.path, module.parse_literals(list(literals)))

    def test_rewrites_section_and_symbol_references(self):
        self.path.write_bytes(fixture())
        self.assertTrue(self.run_import("lbl_8047B948:0:" + BIAS.hex()))
        _, sections = module.read_elf(self.path.read_bytes())
        self.assertNotIn(".sdata2", [s["name"] for s in sections])
        index, symbols = module.parse_symbols(sections)
        self.assertEqual([s["name"] for s in symbols], ["", "convert", "lbl_8047B948"])
        self.assertEqual(sections[index]["fields"][7], 1)  # one local (the null symbol)
        self.assertEqual(relocations(self.path), [
            (".rela.text", 2, 0x6D, "lbl_8047B948", 0),
            (".rela.text", 6, 0x6D, "lbl_8047B948", 4)])
        comment = [s for s in sections if s["name"] == ".comment"][0]["data"]
        self.assertEqual(len(comment), 0x2C + 8 * 3)
        self.assertEqual(comment[0x2C + 8:0x2C + 12], struct.pack(">I", 4))  # convert kept
        # Idempotent on an already rewritten object.
        self.assertFalse(self.run_import("lbl_8047B948:0:" + BIAS.hex()))
        if READELF.exists():
            subprocess.run([str(READELF), "-a", str(self.path)], check=True,
                           capture_output=True)

    def test_mismatched_bytes_fail_and_keep_object(self):
        self.path.write_bytes(fixture())
        before = self.path.read_bytes()
        with self.assertRaisesRegex(ValueError, "expected"):
            self.run_import("lbl_8047B948:0:4330000000000000")
        self.assertEqual(before, self.path.read_bytes())

    def test_unlisted_bytes_are_an_unsupported_layout(self):
        self.path.write_bytes(fixture(extra=b"\x3f\x80\x00\x00"))
        before = self.path.read_bytes()
        with self.assertRaisesRegex(ValueError, "unsupported layout"):
            self.run_import("lbl_8047B948:0:" + BIAS.hex())
        self.assertEqual(before, self.path.read_bytes())

    def test_rejects_bad_names_and_sections(self):
        with self.assertRaises(ValueError):
            module.parse_literals(["bad=name:0:00"])
        with self.assertRaises(ValueError):
            module.parse_literals([".data:lbl:0:00"])
        with self.assertRaises(ValueError):
            module.parse_literals([])


@unittest.skipUnless(WIBO.exists() and MWCC.exists() and OBJDUMP.exists(),
                     "needs a configured build with GC/1.3 and binutils")
class CompiledTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.mkdtemp(dir=ROOT / "build")
        self.addCleanup(shutil.rmtree, self.directory)

    def disassembly(self, path):
        out = subprocess.run([str(OBJDUMP), "-d", str(path)], check=True,
                             capture_output=True, text=True).stdout
        return out.split("\n", 3)[3]

    def test_round_trip_conversion_bias(self):
        path = compile_source(self.directory, CONVERT)
        before = self.disassembly(path)
        module.import_constants(path, module.parse_literals(["lbl_bias:0:" + BIAS.hex()]))
        self.assertEqual(before, self.disassembly(path))
        targets = [r[3] for r in relocations(path)]
        self.assertIn("lbl_bias", targets)
        self.assertFalse([t for t in targets if t.startswith("@")])
        subprocess.run([str(READELF), "-a", str(path)], check=True, capture_output=True)

    def test_round_trip_with_padding_between_literals(self):
        path = compile_source(self.directory, CONVERT_ZERO)
        _, sections = module.read_elf(path.read_bytes())
        index = [i for i, s in enumerate(sections) if s["name"] == ".sdata2"][0]
        data = sections[index]["data"]
        names = {BIAS: "lbl_bias", bytes.fromhex("3fc00000"): "lbl_one_half",
                 bytes(4): "lbl_zero"}
        literals = []
        for symbol in module.parse_symbols(sections)[1]:
            if symbol["shndx"] == index and symbol["name"].startswith("@"):
                value = data[symbol["value"]:symbol["value"] + symbol["size"]]
                literals.append(f"{names[value]}:{symbol['value']}:{value.hex()}")
        self.assertEqual(len(literals), 3)
        before = self.disassembly(path)
        module.import_constants(path, module.parse_literals(literals))
        self.assertEqual(before, self.disassembly(path))
        self.assertTrue({"lbl_bias", "lbl_one_half", "lbl_zero"} <=
                        {r[3] for r in relocations(path)})

    def test_wrong_bytes_fail_on_compiled_object(self):
        path = compile_source(self.directory, CONVERT)
        before = path.read_bytes()
        with self.assertRaises(ValueError):
            module.import_constants(path, module.parse_literals(["lbl_bias:0:3f80000000000000"]))
        self.assertEqual(before, path.read_bytes())

    def test_unit_owned_data_is_unsupported(self):
        path = compile_source(self.directory, OWN_DATA)
        before = path.read_bytes()
        with self.assertRaises(ValueError):
            module.import_constants(path, module.parse_literals(["lbl_bias:0:" + BIAS.hex()]))
        self.assertEqual(before, path.read_bytes())


if __name__ == "__main__":
    unittest.main()
