#!/usr/bin/env python3
"""Serve the read-only local LLM campaign dashboard."""

from __future__ import annotations

import argparse
import gzip
import ipaddress
import json
import mimetypes
import re
import subprocess
import sys
import webbrowser
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from threading import Event, Lock, Thread
from time import monotonic
from typing import Any, Callable
from urllib.parse import urlparse

from local_campaign import ROOT, dashboard

WEB_ROOT = ROOT / "tools" / "local_campaign_ui"
SNAPSHOT_TIMEOUT_SECONDS = 30


def bounded_dashboard() -> dict[str, Any]:
    """Isolate a slow or wedged scan so a later refresh can retry it."""
    try:
        result = subprocess.run(
            [sys.executable, str(Path(__file__).resolve()), "--snapshot"],
            capture_output=True, text=True, check=True, timeout=SNAPSHOT_TIMEOUT_SECONDS,
        )
    except subprocess.TimeoutExpired as error:
        raise RuntimeError(f"dashboard scan exceeded {SNAPSHOT_TIMEOUT_SECONDS} seconds") from error
    return json.loads(result.stdout)


class DashboardCache:
    """Share one dashboard calculation across concurrent short-poll clients."""

    def __init__(self, loader: Callable[[], dict[str, Any]], ttl_seconds: float = 2.0,
                 clock: Callable[[], float] = monotonic) -> None:
        self.loader = loader
        self.ttl_seconds = ttl_seconds
        self.clock = clock
        self.lock = Lock()
        self.body: bytes | None = None
        self.updated_at = 0.0
        self.refreshing = False
        self.stop_event = Event()
        self.periodic_thread: Thread | None = None

    def _refresh(self) -> None:
        try:
            body = json.dumps(self.loader()).encode("utf-8")
        except Exception as error:
            print(f"dashboard refresh failed: {type(error).__name__}: {error}")
        else:
            with self.lock:
                self.body = body
                self.updated_at = self.clock()
        finally:
            with self.lock:
                self.refreshing = False

    def get(self) -> bytes:
        with self.lock:
            if self.body is None:
                # Only the first request waits for a cold scan. Once warm, slow
                # rebuilds never hold up a remote browser polling the dashboard.
                body = json.dumps(self.loader()).encode("utf-8")
                self.body = body
                self.updated_at = self.clock()
            elif self.clock() - self.updated_at >= self.ttl_seconds and not self.refreshing:
                self.refreshing = True
                Thread(target=self._refresh, name="dashboard-refresh", daemon=True).start()
            return self.body

    def start_periodic(self) -> None:
        """Refresh even when no browser is connected; GETs still serve stale data."""
        if self.periodic_thread is not None:
            return
        self.periodic_thread = Thread(target=self._periodic, name="dashboard-periodic", daemon=True)
        self.periodic_thread.start()

    def _periodic(self) -> None:
        while not self.stop_event.is_set():
            try:
                self.get()
            except Exception as error:
                print(f"dashboard refresh failed: {type(error).__name__}: {error}")
            self.stop_event.wait(max(self.ttl_seconds, 0.05))

    def stop_periodic(self) -> None:
        self.stop_event.set()
        if self.periodic_thread is not None:
            self.periodic_thread.join(timeout=1)


DASHBOARD_CACHE = DashboardCache(bounded_dashboard)


def tailnet_ipv4(ifconfig_output: str) -> str | None:
    """Find Tailscale's IPv4 on a tunnel with its distinctive IPv6 prefix."""
    for interface in re.split(r"(?=^[a-zA-Z][a-zA-Z0-9]*: flags=)", ifconfig_output, flags=re.MULTILINE):
        if not interface.startswith("utun") or "fd7a:115c:a1e0:" not in interface:
            continue
        for address in re.findall(r"^\s*inet (\d+\.\d+\.\d+\.\d+)\b", interface, flags=re.MULTILINE):
            ipv4 = ipaddress.IPv4Address(address)
            if ipv4 in ipaddress.IPv4Network("100.64.0.0/10"):
                return address
    return None


def current_tailnet_ipv4() -> str:
    try:
        output = subprocess.check_output(["ifconfig", "-a"], text=True)
    except (OSError, subprocess.CalledProcessError) as error:
        raise RuntimeError("Could not inspect the Tailscale network interface") from error
    address = tailnet_ipv4(output)
    if address is None:
        raise RuntimeError("No active Tailscale IPv4 interface; connect to the tailnet and retry")
    return address


class CampaignHandler(SimpleHTTPRequestHandler):
    def translate_path(self, path: str) -> str:
        requested = Path(urlparse(path).path.lstrip("/"))
        candidate = (WEB_ROOT / requested).resolve()
        if WEB_ROOT not in candidate.parents and candidate != WEB_ROOT:
            return str(WEB_ROOT / "index.html")
        if candidate.is_dir():
            candidate /= "index.html"
        return str(candidate)

    def do_GET(self) -> None:  # noqa: N802
        if urlparse(self.path).path == "/api/dashboard":
            body = DASHBOARD_CACHE.get()
            accepts_gzip = any(
                item.strip().split(";", 1)[0].lower() == "gzip"
                for item in self.headers.get("Accept-Encoding", "").split(",")
            )
            if accepts_gzip:
                body = gzip.compress(body, compresslevel=5)
            self.send_response(HTTPStatus.OK)
            self.send_header("Content-Type", "application/json; charset=utf-8")
            self.send_header("Cache-Control", "no-store")
            self.send_header("Vary", "Accept-Encoding")
            if accepts_gzip:
                self.send_header("Content-Encoding", "gzip")
            self.send_header("Content-Length", str(len(body)))
            self.end_headers()
            self.wfile.write(body)
            return
        super().do_GET()

    def end_headers(self) -> None:
        # "/" is the page itself: without no-store a browser keeps an old index.html that lacks
        # sections the current app.js renders into.
        path = urlparse(self.path).path
        if path == "/" or path.endswith((".html", ".css", ".js")):
            self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def log_message(self, format: str, *args: object) -> None:
        print("dashboard", format % args)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    binding = parser.add_mutually_exclusive_group()
    binding.add_argument("--host", default=None, help="bind to this explicit address (default: 127.0.0.1)")
    binding.add_argument("--tailnet", action="store_true", help="bind only to the active Tailscale IPv4 address")
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--open", action="store_true")
    parser.add_argument("--snapshot", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args()
    if args.snapshot:
        print(json.dumps(dashboard()))
        return
    mimetypes.add_type("text/javascript", ".js")
    host = current_tailnet_ipv4() if args.tailnet else args.host or "127.0.0.1"
    server = ThreadingHTTPServer((host, args.port), CampaignHandler)
    DASHBOARD_CACHE.start_periodic()
    url = f"http://{host}:{args.port}"
    print(f"Campaign dashboard: {url}")
    if args.open:
        webbrowser.open(url)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
        DASHBOARD_CACHE.stop_periodic()


if __name__ == "__main__":
    main()
