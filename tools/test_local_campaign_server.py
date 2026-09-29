"""Tailnet binding must never fall back to a LAN or loopback interface."""

import unittest
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).parent))

from local_campaign_server import tailnet_ipv4


class TailnetBindingTests(unittest.TestCase):
    def test_finds_tailscale_tunnel_only(self):
        interfaces = (
            "en0: flags=1\n\tinet 192.168.0.139\n"
            "utun4: flags=1\n\tinet 100.100.100.100\n\tinet6 fe80::1\n"
            "utun5: flags=1\n\tinet6 fd7a:115c:a1e0::1234\n\tinet 100.115.161.28\n"
        )
        self.assertEqual(tailnet_ipv4(interfaces), "100.115.161.28")

    def test_rejects_non_tailscale_addresses(self):
        self.assertIsNone(tailnet_ipv4("en0: flags=1\n\tinet 100.115.161.28\n"))
        self.assertIsNone(tailnet_ipv4("utun5: flags=1\n\tinet 192.168.0.2\n\tinet6 fd7a:115c:a1e0::1\n"))
        self.assertIsNone(tailnet_ipv4("utun5: flags=1\n\tinet 100.115.161.28\n"))


if __name__ == "__main__":
    unittest.main()
