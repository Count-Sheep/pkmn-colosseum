"""Short-lived, single-flight dashboard response cache."""

from __future__ import annotations

import json
import gzip
import http.client
import subprocess
import threading
import time
import unittest
from concurrent.futures import ThreadPoolExecutor
from http.server import ThreadingHTTPServer
from unittest.mock import patch

from local_campaign_server import CampaignHandler, DashboardCache, bounded_dashboard


class DashboardCacheTests(unittest.TestCase):
    def test_browser_receives_compressed_dashboard(self) -> None:
        class QuietHandler(CampaignHandler):
            def log_message(self, _format: str, *_args: object) -> None:
                pass

        server = ThreadingHTTPServer(("127.0.0.1", 0), QuietHandler)
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        try:
            with patch("local_campaign_server.DASHBOARD_CACHE", DashboardCache(lambda: {"version": 1})):
                connection = http.client.HTTPConnection("127.0.0.1", server.server_port, timeout=5)
                connection.request("GET", "/api/dashboard", headers={"Accept-Encoding": "gzip"})
                response = connection.getresponse()
                self.assertEqual(response.status, 200)
                self.assertEqual(response.getheader("Content-Encoding"), "gzip")
                self.assertEqual(json.loads(gzip.decompress(response.read())), {"version": 1})
                connection.close()
        finally:
            server.shutdown()
            server.server_close()
            thread.join(timeout=5)

    def test_expires_after_two_seconds_from_completed_load(self) -> None:
        now = [100.0]
        calls = [0]
        updated = threading.Event()

        def load() -> dict[str, int]:
            calls[0] += 1
            if calls[0] == 2:
                updated.set()
            return {"version": calls[0]}

        cache = DashboardCache(load, clock=lambda: now[0])
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        now[0] = 101.9
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        now[0] = 102.0
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        self.assertTrue(updated.wait(5))
        for _ in range(100):
            if json.loads(cache.get()) == {"version": 2}:
                break
            threading.Event().wait(0.01)
        self.assertEqual(json.loads(cache.get()), {"version": 2})
        self.assertEqual(calls[0], 2)

    def test_stale_response_does_not_wait_for_refresh(self) -> None:
        now = [100.0]
        entered = threading.Event()
        release = threading.Event()
        calls = [0]

        def load() -> dict[str, int]:
            calls[0] += 1
            if calls[0] == 2:
                entered.set()
                self.assertTrue(release.wait(5))
            return {"version": calls[0]}

        cache = DashboardCache(load, clock=lambda: now[0])
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        now[0] = 103.0
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        self.assertTrue(entered.wait(5))
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        self.assertEqual(calls[0], 2)
        release.set()

    def test_concurrent_misses_share_one_load(self) -> None:
        entered = threading.Event()
        release = threading.Event()
        calls = [0]

        def load() -> dict[str, int]:
            calls[0] += 1
            entered.set()
            self.assertTrue(release.wait(5))
            return {"version": calls[0]}

        cache = DashboardCache(load, clock=lambda: 100.0)
        with ThreadPoolExecutor(max_workers=4) as pool:
            results = [pool.submit(cache.get) for _ in range(4)]
            self.assertTrue(entered.wait(5))
            release.set()
            self.assertEqual([json.loads(result.result(timeout=5)) for result in results], [{"version": 1}] * 4)
        self.assertEqual(calls[0], 1)

    def test_failed_load_is_not_cached(self) -> None:
        calls = [0]

        def load() -> dict[str, int]:
            calls[0] += 1
            if calls[0] == 1:
                raise RuntimeError("temporary scan error")
            return {"version": calls[0]}

        cache = DashboardCache(load, clock=lambda: 100.0)
        with self.assertRaises(RuntimeError):
            cache.get()
        self.assertEqual(json.loads(cache.get()), {"version": 2})

    def test_periodic_refresh_without_browser_requests(self) -> None:
        calls = [0]
        updated = threading.Event()

        def load() -> dict[str, int]:
            calls[0] += 1
            if calls[0] >= 2:
                updated.set()
            return {"version": calls[0]}

        cache = DashboardCache(load, ttl_seconds=0.05)
        cache.start_periodic()
        try:
            self.assertTrue(updated.wait(5))
            for _ in range(100):
                if json.loads(cache.get())["version"] >= 2:
                    break
                threading.Event().wait(0.01)
            self.assertGreaterEqual(json.loads(cache.get())["version"], 2)
        finally:
            cache.stop_periodic()

    def test_periodic_refresh_retries_after_failed_scan(self) -> None:
        calls = [0]
        recovered = threading.Event()

        def load() -> dict[str, int]:
            calls[0] += 1
            if calls[0] == 1:
                raise RuntimeError("temporary scan error")
            recovered.set()
            return {"version": calls[0]}

        cache = DashboardCache(load, ttl_seconds=0.05)
        cache.start_periodic()
        try:
            self.assertTrue(recovered.wait(5))
            self.assertEqual(json.loads(cache.get()), {"version": 2})
        finally:
            cache.stop_periodic()

    def test_warm_requests_stay_fast_and_refresh_is_single_flight(self) -> None:
        entered = threading.Event()
        release = threading.Event()
        calls = [0]
        active = [0]
        peak = [0]
        guard = threading.Lock()

        def load() -> dict[str, int]:
            with guard:
                calls[0] += 1
                active[0] += 1
                peak[0] = max(peak[0], active[0])
                version = calls[0]
            try:
                if version == 2:
                    entered.set()
                    self.assertTrue(release.wait(5))
                return {"version": version}
            finally:
                with guard:
                    active[0] -= 1

        cache = DashboardCache(load, ttl_seconds=0.05)
        self.assertEqual(json.loads(cache.get()), {"version": 1})
        cache.start_periodic()
        try:
            self.assertTrue(entered.wait(5))
            for _ in range(20):
                started = time.monotonic()
                self.assertEqual(json.loads(cache.get()), {"version": 1})
                self.assertLess(time.monotonic() - started, 0.5)
                time.sleep(0.01)
            self.assertEqual(calls[0], 2)
            self.assertEqual(peak[0], 1)
        finally:
            release.set()
            cache.stop_periodic()

    def test_bounded_snapshot_worker_times_out(self) -> None:
        with patch("local_campaign_server.subprocess.run", side_effect=subprocess.TimeoutExpired("scan", 30)):
            with self.assertRaisesRegex(RuntimeError, "exceeded 30 seconds"):
                bounded_dashboard()


if __name__ == "__main__":
    unittest.main()
