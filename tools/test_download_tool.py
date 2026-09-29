"""Host preparation for downloaded build tools."""

import unittest
from pathlib import Path
from unittest.mock import patch

from download_tool import prepare_tool


class PrepareToolTests(unittest.TestCase):
    @patch("download_tool.subprocess.run")
    @patch("download_tool.platform.system", return_value="Darwin")
    def test_macos_wibo_is_adhoc_signed(self, _system, run) -> None:
        prepare_tool("wibo", Path("build/tools/wibo"))
        run.assert_called_once_with(
            ["codesign", "--force", "--sign", "-", "build/tools/wibo"], check=True
        )

    @patch("download_tool.subprocess.run")
    @patch("download_tool.platform.system", return_value="Linux")
    def test_other_hosts_are_unchanged(self, _system, run) -> None:
        prepare_tool("wibo", Path("build/tools/wibo"))
        run.assert_not_called()

    @patch("download_tool.subprocess.run")
    @patch("download_tool.platform.system", return_value="Darwin")
    def test_other_tools_are_unchanged(self, _system, run) -> None:
        prepare_tool("dtk", Path("build/tools/dtk"))
        run.assert_not_called()


if __name__ == "__main__":
    unittest.main()
