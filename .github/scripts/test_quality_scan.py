#!/usr/bin/env python3
"""Focused tests for the quality gate's asm exceptions."""

import contextlib
import io
import unittest

import quality_scan


def body_ok(body: str, path: str, func: str) -> bool:
    with contextlib.redirect_stdout(io.StringIO()):
        return quality_scan.asm_body_ok(body, path, func)


def source_ok(source: str, path: str, added_lines: list[tuple[int, str]]) -> bool:
    with contextlib.redirect_stdout(io.StringIO()):
        return quality_scan.scan_source(path, source, added_lines)


class QualityScanAllowlistTests(unittest.TestCase):
    def test_hardware_primitive_still_allowed_globally(self) -> None:
        self.assertTrue(body_ok("mfmsr r3\nblr", "src/dolphin/os/OS.c", "PPCMfmsr"))

    def test_paired_single_allowed_for_named_dolphin_sdk_function(self) -> None:
        body = """
            nofralloc
            stwu r1, -64(r1)
            psq_l f0, 0(r3), 0, qr0
            ps_madds1 f1, f2, f3, f4
            stfd f14, 8(r1)
            blr
        """
        self.assertTrue(body_ok(body, quality_scan.DOLPHIN_PAIRED_SINGLE_PATH, "PSMTXConcat"))

    def test_paired_single_rejected_outside_dolphin(self) -> None:
        self.assertFalse(body_ok("ps_add f1, f2, f3", "src/game/fight.c", "PSMTXConcat"))

    def test_paired_single_rejected_for_unknown_symbol(self) -> None:
        self.assertFalse(body_ok("ps_add f1, f2, f3", quality_scan.DOLPHIN_PAIRED_SINGLE_PATH, "PSMTXNew"))

    def test_calls_remain_forbidden_in_paired_single_function(self) -> None:
        self.assertFalse(body_ok("ps_add f1, f2, f3\nbl helper", quality_scan.DOLPHIN_PAIRED_SINGLE_PATH, "PSVECNormalize"))

    def test_plain_branch_to_external_symbol_remains_forbidden(self) -> None:
        self.assertFalse(body_ok(
            "ps_add f1, f2, f3\nb ExternalFunction",
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            "PSVECNormalize",
        ))

    def test_plain_branch_to_local_label_is_allowed(self) -> None:
        self.assertTrue(body_ok(
            "ps_add f1, f2, f3\nb done\ndone:\nblr",
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            "PSVECNormalize",
        ))

    def test_condition_register_branch_must_still_target_local_label(self) -> None:
        path = "src/musyx/runtime/reverb_candidate_80164520.c"
        self.assertTrue(body_ok("beq cr7, done\ndone:\nblr", path, "HandleReverb"))
        self.assertFalse(body_ok("beq cr7, ExternalFunction\nblr", path, "HandleReverb"))

    def test_authentic_asm_needs_registry_entry_and_full_evidence(self) -> None:
        import tempfile
        from pathlib import Path
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            doc = root / "docs" / "asm_evidence" / "reverb.md"
            doc.parent.mkdir(parents=True)
            entry = {"path": "src/musyx/reverb.c", "function": "HandleReverb",
                     "mnemonics": ["stmw", "lis", "lfs", "blr"], "evidence": "docs/asm_evidence/reverb.md"}
            doc.write_text("## HandleReverb\n- **Why it cannot be C:** stmw r14 in a -use_lmw_stmw off unit.\n")
            self.assertTrue(quality_scan.evidence_problems(entry, root))  # two fields missing
            doc.write_text(
                "## HandleReverb\n"
                "- **Why it cannot be C:** stmw r14 in a -use_lmw_stmw off unit; lis/@l small-data loads.\n"
                "- **Other decompilations:** kept as asm in https://github.com/doldecomp/ttyd/blob/62131fc3/src/musyx/reverb.c\n"
                "- **Origin:** MusyX 2.0 StdReverb, Factor 5.\n")
            self.assertEqual(quality_scan.evidence_problems(entry, root), [])
            doc.write_text(doc.read_text().replace("https://github.com/doldecomp/ttyd/blob/62131fc3/src/musyx/reverb.c", "the ttyd decompilation, file reverb.c"))
            self.assertTrue(any("commit hash" in p for p in quality_scan.evidence_problems(entry, root)))

    def _branch_entry(self, path: str = "src/trk/TRKInit.c") -> dict:
        return {"path": path, "function": "InitMetroTRK", "mnemonics": ["addi", "b"],
                "evidence": "docs/asm_evidence/trk_init.md", "branch_targets": ["TRK_main"]}

    def _with_entry(self, entry: dict, symbols: set[str], evidence_ok: bool = True):
        from unittest import mock
        stack = contextlib.ExitStack()
        stack.enter_context(mock.patch.dict(quality_scan.AUTHENTIC_ASM,
                                            {(entry["path"], entry["function"]): entry}))
        stack.enter_context(mock.patch.object(quality_scan, "function_symbols", lambda *a: symbols))
        stack.enter_context(mock.patch.object(
            quality_scan, "evidence_problems", lambda *a: [] if evidence_ok else ["incomplete"]))
        return stack

    def test_declared_branch_target_allowed_for_registered_library_asm(self) -> None:
        entry = self._branch_entry()
        with self._with_entry(entry, {"TRK_main"}):
            self.assertTrue(body_ok("addi r1, r1, 8\nb TRK_main", entry["path"], "InitMetroTRK"))

    def test_undeclared_branch_target_still_rejected(self) -> None:
        entry = self._branch_entry()
        with self._with_entry(entry, {"TRK_main", "OtherFunction"}):
            self.assertFalse(body_ok("b OtherFunction", entry["path"], "InitMetroTRK"))

    def test_declared_target_must_be_a_known_function(self) -> None:
        entry = self._branch_entry()
        with self._with_entry(entry, set()):
            self.assertFalse(body_ok("b TRK_main", entry["path"], "InitMetroTRK"))

    def test_branch_targets_rejected_outside_library_paths(self) -> None:
        entry = self._branch_entry("src/game/fight.c")
        with self._with_entry(entry, {"TRK_main"}):
            self.assertFalse(body_ok("b TRK_main", entry["path"], "InitMetroTRK"))

    def test_branch_targets_need_complete_evidence(self) -> None:
        entry = self._branch_entry()
        with self._with_entry(entry, {"TRK_main"}, evidence_ok=False):
            self.assertFalse(body_ok("b TRK_main", entry["path"], "InitMetroTRK"))

    def test_self_branch_allowed_for_registered_library_asm(self) -> None:
        entry = self._branch_entry()
        with self._with_entry(entry, set()):
            self.assertTrue(body_ok("addi r1, r1, 8\nb InitMetroTRK", entry["path"], "InitMetroTRK"))

    def test_branch_target_evidence_field_required(self) -> None:
        import tempfile
        from pathlib import Path
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "config" / "GC6E01").mkdir(parents=True)
            (root / "config" / "GC6E01" / "symbols.txt").write_text(
                "TRK_main = .text:0x800C2B10; // type:function size:0x40\n")
            doc = root / "docs" / "asm_evidence" / "trk_init.md"
            doc.parent.mkdir(parents=True)
            base = ("## InitMetroTRK\n"
                    "- **Why it cannot be C:** ends in a tail branch MWCC never emits for C.\n"
                    "- **Other decompilations:** kept as asm in https://github.com/zeldaret/tp/blob/abc1234/src/TRK/init.c\n"
                    "- **Origin:** MetroTRK 0.4, Metrowerks debugger stub.\n")
            doc.write_text(base)
            entry = self._branch_entry()
            self.assertTrue(any("External branch targets" in p
                                for p in quality_scan.evidence_problems(entry, root)))
            doc.write_text(base + "- **External branch targets:** `b TRK_main` at the end hands control to TRK_main.\n")
            self.assertEqual(quality_scan.evidence_problems(entry, root), [])

    def test_static_asm_function_is_scanned(self) -> None:
        source = "static asm void Foo(void) {\n    nofralloc\n    lwz r3, 0(r4)\n    blr\n}\n"
        self.assertFalse(source_ok(source, "src/dolphin/os/OS.c", [(3, "    lwz r3, 0(r4)")]))

    def test_unregistered_asm_still_rejected(self) -> None:
        self.assertFalse(body_ok("stmw r14, 8(r1)\nblr", "src/musyx/reverb.c", "HandleReverb"))

    def test_general_gpr_load_remains_forbidden(self) -> None:
        self.assertFalse(body_ok("ps_mul f1, f2, f3\nlwz r3, 0(r4)", quality_scan.DOLPHIN_PAIRED_SINGLE_PATH, "PSMTXInverse"))

    def test_paired_exception_requires_paired_instruction(self) -> None:
        self.assertFalse(body_ok("fres f1, f2", quality_scan.DOLPHIN_PAIRED_SINGLE_PATH, "PSMTXInverse"))

    def test_semicolon_cannot_hide_a_second_instruction(self) -> None:
        self.assertFalse(body_ok(
            "ps_add f1, f2, f3; lwz r3, 0(r4); blr",
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            "PSMTXCopy",
        ))

    def test_record_form_is_not_the_allowlisted_opcode(self) -> None:
        self.assertFalse(body_ok(
            "ps_add. f1, f2, f3",
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            "PSMTXCopy",
        ))

    def test_block_comments_do_not_become_fake_instructions(self) -> None:
        self.assertTrue(body_ok(
            "/* lwz r3, 0(r4) */\nps_add f1, f2, f3",
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            "PSMTXCopy",
        ))

    def test_vendor_c_wrappers_are_not_allowlisted(self) -> None:
        self.assertNotIn("PSMTXRotRad", quality_scan.DOLPHIN_PAIRED_SINGLE_FUNCTIONS)
        self.assertNotIn("PSMTXRotAxisRad", quality_scan.DOLPHIN_PAIRED_SINGLE_FUNCTIONS)
        self.assertEqual(len(quality_scan.DOLPHIN_PAIRED_SINGLE_FUNCTIONS), 26)

    def test_changed_instruction_inside_existing_inline_asm_is_scanned(self) -> None:
        source = """void PSVECNormalize(void)
{
    asm {
        psq_l f1, 0(r3), 0, qr0
        frsqrte f2, f1
    }
}
"""
        self.assertTrue(source_ok(
            source,
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            [(5, "        frsqrte f2, f1")],
        ))

    def test_whole_asm_function_is_mapped_to_its_symbol(self) -> None:
        source = """asm void PSMTXCopy(void)
{
    psq_l f0, 0(r3), 0, qr0
    psq_st f0, 0(r4), 0, qr0
    blr
}
"""
        self.assertTrue(source_ok(
            source,
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            [(1, "asm void PSMTXCopy(void)")],
        ))

    def test_multiline_whole_asm_declaration_is_mapped(self) -> None:
        source = """asm
void PSMTXCopy(void)
{
    psq_l f0, 0(r3), 0, qr0
    psq_st f0, 0(r4), 0, qr0
    blr
}
"""
        self.assertFalse(source_ok(
            source,
            "src/game/not_dolphin.c",
            [(1, "asm"), (4, "    psq_l f0, 0(r3), 0, qr0")],
        ))

    def test_preprocessor_cannot_rewrite_symbol_or_mnemonic(self) -> None:
        source = """#define PSMTXCopy EvilGameFunction
#define ps_add lwz
asm void PSMTXCopy(void)
{
    ps_add r3, 0(r4)
    blr
}
"""
        self.assertFalse(source_ok(
            source,
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            [
                (1, "#define PSMTXCopy EvilGameFunction"),
                (2, "#define ps_add lwz"),
                (3, "asm void PSMTXCopy(void)"),
                (5, "    ps_add r3, 0(r4)"),
            ],
        ))

    def test_changed_header_cannot_define_case_variant_asm_token(self) -> None:
        source = "#define PS_ADD lwz\n"
        self.assertFalse(source_ok(
            source,
            "src/dolphin/mtx_aliases.h",
            [(1, "#define PS_ADD lwz")],
        ))

    def test_changed_inline_asm_rejects_forbidden_instruction(self) -> None:
        source = """void PSVECNormalize(void)
{
    asm {
        psq_l f1, 0(r3), 0, qr0
        lwz r3, 0(r4)
    }
}
"""
        self.assertFalse(source_ok(
            source,
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            [(5, "        lwz r3, 0(r4)")],
        ))

    def test_unbalanced_inline_asm_fails_closed(self) -> None:
        source = """void PSVECNormalize(void)
{
    asm {
        psq_l f1, 0(r3), 0, qr0
}
"""
        self.assertFalse(source_ok(
            source,
            quality_scan.DOLPHIN_PAIRED_SINGLE_PATH,
            [(3, "    asm {")],
        ))

    def test_asm_words_in_comments_and_strings_are_ignored(self) -> None:
        source = """/* asm void Fake(void) { */
const char* text = "asm {";
// asm volatile {
"""
        self.assertTrue(source_ok(
            source,
            "src/game/comments.c",
            [
                (1, "/* asm void Fake(void) { */"),
                (2, 'const char* text = "asm {";'),
                (3, "// asm volatile {"),
            ],
        ))

    def test_inc_include_in_comments_and_strings_is_ignored(self) -> None:
        source = """/* #include "fake.inc" */
const char* text = "#include \\"fake.inc\\"";
"""
        self.assertTrue(source_ok(
            source,
            "src/game/comments.c",
            [
                (1, '/* #include "fake.inc" */'),
                (2, 'const char* text = "#include \\"fake.inc\\"";'),
            ],
        ))

    def test_real_inc_include_is_rejected(self) -> None:
        source = '#include "target.inc"\n'
        self.assertFalse(source_ok(
            source,
            "src/game/target.c",
            [(1, '#include "target.inc"')],
        ))

    def test_zero_context_diff_tracks_added_head_line_numbers(self) -> None:
        diff = """diff --git a/src/example.c b/src/example.c
--- a/src/example.c
+++ b/src/example.c
@@ -4,0 +5,2 @@
+    asm {
+        ps_add f1, f2, f3
"""
        self.assertEqual(
            quality_scan.added_lines_from_diff(diff),
            {
                "src/example.c": [
                    (5, "    asm {"),
                    (6, "        ps_add f1, f2, f3"),
                ],
            },
        )


if __name__ == "__main__":
    unittest.main()
