#!/usr/bin/env python3
import importlib.util
import struct
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location("constant_export", ROOT / "tools/export_constant_symbol.py")
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
EXPECTED = bytes.fromhex("4330000080000000")


def fixture(symbol="@14", global_symbol=False, text=b"code", reordered=False):
    names = ["", ".text", ".sdata2", ".symtab", ".strtab", ".shstrtab"]
    order = [0, 1, 5, 2, 3, 4] if reordered else list(range(6))
    indices = {original: current for current, original in enumerate(order)}
    section_strings = b"\0"
    name_offsets = []
    for name in names:
        name_offsets.append(len(section_strings))
        section_strings += name.encode() + b"\0"
    symbols = bytes(16) + struct.pack(">IIIBBH", 1, 0, 8, 0x11 if global_symbol else 1, 0, indices[2])
    payloads = [b"", text, EXPECTED, symbols, b"\0" + symbol.encode() + b"\0", section_strings]
    types = [0, 1, 1, 2, 3, 3]
    flags = [0, 6, 2, 0, 0, 0]
    alignments = [0, 4, 8, 4, 1, 1]
    blob = bytearray(52)
    records = []
    for i in order:
        payload = payloads[i]
        records.append(struct.pack(">10I", name_offsets[i], types[i], flags[i], 0,
                                   len(blob), len(payload), indices[4] if i == 3 else 0,
                                   (1 if global_symbol else 2) if i == 3 else 0,
                                   alignments[i], 16 if i == 3 else 0))
        blob.extend(payload)
    shoff = len(blob)
    blob.extend(b"".join(records))
    blob[:52] = struct.pack(">16sHHIIIIIHHHHHH", b"\x7fELF\x01\x02\x01" + bytes(9),
                            1, 20, 1, 0, 0, shoff, 0, 52, 0, 0, 40, 6, indices[5])
    return bytes(blob)


class ConstantExportTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.path = Path(self.directory.name) / "source.o"
        self.path.write_bytes(fixture())

    def export(self, expected=EXPECTED):
        module.export_constant(self.path, Path("objcopy"), "lbl_8047B948", 0, expected)

    def test_exports_only_metadata(self):
        before = module.read_object(self.path.read_bytes())[2]
        def objcopy(command, **kwargs):
            self.assertIn("@14=lbl_8047B948", command)
            Path(command[-1]).write_bytes(fixture("lbl_8047B948", True))
        with patch.object(module.subprocess, "run", side_effect=objcopy):
            self.export()
        _, symbols, after = module.read_object(self.path.read_bytes())
        self.assertEqual(before, after)
        self.assertIn(("lbl_8047B948", 0, 8, 0x11, 2), symbols)

    def test_idempotent(self):
        self.path.write_bytes(fixture("lbl_8047B948", True))
        with patch.object(module.subprocess, "run") as run:
            self.export()
            run.assert_not_called()

    def test_handles_objcopy_section_reordering(self):
        before = module.read_object(self.path.read_bytes())[2]
        def objcopy(command, **kwargs):
            Path(command[-1]).write_bytes(fixture("lbl_8047B948", True, reordered=True))
        with patch.object(module.subprocess, "run", side_effect=objcopy):
            self.export()
        _, symbols, after = module.read_object(self.path.read_bytes())
        self.assertEqual(before, after)
        self.assertIn(("lbl_8047B948", 0, 8, 0x11, 3), symbols)

    def test_rejects_wrong_constant(self):
        original = self.path.read_bytes()
        with patch.object(module.subprocess, "run") as run:
            with self.assertRaises(ValueError):
                self.export(b"incorrect")
            run.assert_not_called()
        self.assertEqual(original, self.path.read_bytes())

    def test_rejects_changed_code_and_keeps_original(self):
        original = self.path.read_bytes()
        def objcopy(command, **kwargs):
            Path(command[-1]).write_bytes(fixture("lbl_8047B948", True, b"evil"))
        with patch.object(module.subprocess, "run", side_effect=objcopy):
            with self.assertRaises(ValueError):
                self.export()
        self.assertEqual(original, self.path.read_bytes())

    def test_requires_compiler_literal(self):
        self.path.write_bytes(fixture("ordinary_data"))
        with self.assertRaises(ValueError):
            self.export()

    def test_rejects_invalid_format_and_name(self):
        with self.assertRaises(ValueError):
            module.read_object(b"not an ELF object")
        with self.assertRaises(ValueError):
            module.export_constant(self.path, Path("objcopy"), "bad=name", 0, EXPECTED)


if __name__ == "__main__":
    unittest.main()
