"""Dashboard timestamps distinguish source edits from built measurements."""

from __future__ import annotations

import os
import hashlib
import tempfile
import unittest
from pathlib import Path

from local_campaign_freshness import freshness


class FreshnessTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        root = Path(self.temporary.name)
        self.decomp = root / "Pokemon-Decomp"
        self.recomp = root / "Pokemon-Recomp"
        self.decomp.mkdir()
        self.recomp.mkdir()
        self.logs = root / "logs"
        self.logs.mkdir()

    def freshness(self) -> dict:
        return freshness(self.decomp, self.recomp, self.logs)

    def touch(self, root: Path, name: str, second: int) -> None:
        path = root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("test", encoding="utf-8")
        os.utime(path, ns=(second * 1_000_000_000, second * 1_000_000_000))

    def test_report_and_app_staleness_use_source_mtimes(self) -> None:
        self.touch(self.decomp, "build/GC6E01/report.json", 100)
        self.touch(self.decomp, "src/game/boot.c", 110)
        self.touch(self.recomp, "build/macos-debug/Pokemon Colosseum Recomp.app/Contents/MacOS/Pokemon Colosseum Recomp", 120)
        self.touch(self.recomp, "src/game/boot.cpp", 130)
        self.touch(self.recomp, "build/macos-debug/colosseum-hsd-tests", 140)
        result = self.freshness()
        self.assertTrue(result["decomp"]["report_behind_source"])
        self.assertIsNone(result["decomp"]["build"])
        self.assertTrue(result["recomp"]["app_behind_source"])
        self.assertEqual(result["recomp"]["build"]["path"], "build/macos-debug/colosseum-hsd-tests")
        self.assertEqual(result["latest_work"]["path"], "build/macos-debug/colosseum-hsd-tests")
        self.assertNotIn("mtime_ns", result["latest_work"])

        self.touch(self.decomp, "build/GC6E01/report.json", 150)
        self.touch(self.decomp, "build/GC6E01/main.dol", 155)
        self.touch(self.recomp, "build/macos-debug/Pokemon Colosseum Recomp.app/Contents/MacOS/Pokemon Colosseum Recomp", 160)
        result = self.freshness()
        self.assertFalse(result["decomp"]["report_behind_source"])
        self.assertEqual(result["decomp"]["build"]["path"], "build/GC6E01/main.dol")
        self.assertFalse(result["recomp"]["app_behind_source"])

    def test_docs_count_as_work_not_source_needing_rebuild(self) -> None:
        self.touch(self.decomp, "build/GC6E01/report.json", 200)
        self.touch(self.decomp, "src/game/boot.c", 180)
        self.touch(self.decomp, "docs/notes.md", 220)
        result = self.freshness()
        self.assertEqual(result["decomp"]["work"]["path"], "docs/notes.md")
        self.assertFalse(result["decomp"]["report_behind_source"])
        self.assertIsNone(result["recomp"]["source"])
        self.assertIsNone(result["recomp"]["build"])

    def test_retail_hash_status_tracks_both_outputs(self) -> None:
        dol = self.decomp / "build/GC6E01/main.dol"
        rel = self.decomp / "build/GC6E01/common_rel/common_rel.rel"
        manifest = self.decomp / "config/GC6E01/build.sha1"
        dol.parent.mkdir(parents=True)
        rel.parent.mkdir(parents=True)
        manifest.parent.mkdir(parents=True)
        dol.write_bytes(b"dol")
        rel.write_bytes(b"rel")
        manifest.write_text(
            f"{hashlib.sha1(b'dol').hexdigest()}  build/GC6E01/main.dol\n"
            f"{hashlib.sha1(b'rel').hexdigest()}  build/GC6E01/common_rel/common_rel.rel\n",
            encoding="utf-8",
        )
        self.assertEqual(self.freshness()["decomp"]["retail_hashes"]["status"], "match")
        rel.write_bytes(b"wrong")
        self.assertEqual(self.freshness()["decomp"]["retail_hashes"]["status"], "mismatch")

    def test_live_boot_probe_reports_only_validated_counts_and_target(self) -> None:
        probe = self.logs / "colosseum-recomp-20260928-232038.log"
        probe.write_text(
            "Diagnostic fn_800057B0 prefix executed 31 of 55 source-order calls\n"
            "fn_800057B0 native port stops at call 32 of 55: fn_800FF828(0x4)\n",
            encoding="utf-8",
        )
        os.utime(probe, ns=(300 * 1_000_000_000, 300 * 1_000_000_000))
        result = self.freshness()
        self.assertEqual(result["recomp"]["boot_probe"]["status"], "safe-stop")
        self.assertEqual(result["recomp"]["boot_probe"]["executed"], 31)
        self.assertEqual(result["recomp"]["boot_probe"]["next_target"], "fn_800FF828")
        self.assertFalse(result["recomp"]["boot_probe"]["diagnostic_only"])
        self.assertEqual(result["latest_work"]["path"], "31/55 calls; next #32 fn_800FF828 (safe stop)")
        self.assertNotIn("mtime_ns", result["recomp"]["boot_probe"])

        newer = self.logs / "colosseum-recomp-20260928-232100.log"
        newer.write_text("fn_800057B0 native port stops at call 12 of 55: unrelated()\n", encoding="utf-8")
        os.utime(newer, ns=(400 * 1_000_000_000, 400 * 1_000_000_000))
        self.assertEqual(self.freshness()["recomp"]["boot_probe"]["executed"], 31)

    def test_diagnostic_stop_wins_over_production_preflight_stop(self) -> None:
        probe = self.logs / "colosseum-recomp-20260928-232201.log"
        probe.write_text(
            "Diagnostic fn_800057B0 prefix executed 35 of 55 source-order calls\n"
            "Diagnostic fn_800057B0 prefix next unbound call 36 of 55: GSmsgInit(0x2, 0x5)\n"
            "fn_800057B0 native port stops at call 32 of 55: fn_800FF828(0x4)\n",
            encoding="utf-8",
        )
        result = self.freshness()["recomp"]["boot_probe"]
        self.assertEqual(result["executed"], 35)
        self.assertEqual(result["next_call"], 36)
        self.assertEqual(result["next_target"], "GSmsgInit")
        self.assertTrue(result["diagnostic_only"])
        self.assertIn("production readiness unchanged", result["path"])

    def test_failed_or_inconsistent_probe_does_not_claim_safe_stop(self) -> None:
        bad = self.logs / "colosseum-recomp-20260928-232200.log"
        bad.write_text(
            "Diagnostic fn_800057B0 prefix executed 31 of 55 source-order calls\n"
            "fn_800057B0 native port stops at call 34 of 55: fn_800FF828()\n",
            encoding="utf-8",
        )
        self.assertIsNone(self.freshness()["recomp"]["boot_probe"])
        bad.write_text("Diagnostic fn_800057B0 prefix failed: test transport failure\n", encoding="utf-8")
        self.assertEqual(self.freshness()["recomp"]["boot_probe"]["status"], "failed")


if __name__ == "__main__":
    unittest.main()
