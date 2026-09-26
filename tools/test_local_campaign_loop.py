"""Regression checks for the live campaign's source and verification loop."""

import json
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

import local_campaign as campaign
from local_campaign_coordination import Coordinator


class LoopTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def test_source_locator_skips_dead_stubs_comments_and_strings(self):
        source = '''/* int target(void) { return 9; } */
const char* note = "int target(void) { return 9; }";
#if 0
#ifdef SOMETHING
asm void target(void) { }
#else
int target(void) { return 8; }
#endif
#else
static unsigned int
target(unsigned int value) { return value; }
#endif
'''
        start, end = campaign.function_span(source, 'target')
        self.assertEqual(source[start:end], 'static unsigned int\ntarget(unsigned int value) { return value; }')
        changed = campaign.splice(source, 'target', 'static unsigned int target(unsigned int value) { return value + 1; }')
        self.assertIn('asm void target(void) { }', changed)
        self.assertIn('return value + 1;', changed)

    def test_ambiguous_conditional_definitions_fail_closed(self):
        with self.assertRaisesRegex(ValueError, 'ambiguous'):
            campaign.function_span('#if FEATURE\nint f(void) {}\n#else\nint f(void) {}\n#endif', 'f')

    def test_chained_owner_and_cycle(self):
        (self.root / 'src').mkdir()
        (self.root / 'src/a.c').write_text('#include "src/b.c"')
        (self.root / 'src/b.c').write_text('#include "src/c.c"')
        (self.root / 'src/c.c').write_text('int f(void) { return 1; }')
        with patch.object(campaign, 'ROOT', self.root):
            self.assertEqual(campaign.owner_source('src/a.c'), 'src/c.c')
            (self.root / 'src/c.c').write_text('#include "src/a.c"')
            with self.assertRaisesRegex(ValueError, 'cyclic'):
                campaign.owner_source('src/a.c')

    def test_truncated_and_disconnected_streams_retain_output(self):
        class Response:
            def __init__(self, packets):
                self.packets = packets
            def __enter__(self):
                return self
            def __exit__(self, *args):
                pass
            def __iter__(self):
                return iter(json.dumps(packet).encode() for packet in self.packets)
        for final in ([], [{'done': True, 'done_reason': 'length'}]):
            path = self.root / 'response.md'
            with patch('urllib.request.urlopen', return_value=Response([{'response': 'partial'}] + final)):
                with self.assertRaises(campaign.IncompleteResponse):
                    campaign.ollama('http://test', 'test', 'prompt', 1, 1, lambda **kw: None, path)
            self.assertEqual(path.read_text(), 'partial')

    def test_raw_near_match_and_byte_preserving_restoration(self):
        original = b'int target(void) { return 1; }\r\n'
        (self.root / 'source.c').write_bytes(original)
        candidate = self.root / 'candidate.c'
        candidate.write_text('int target(void) { return 2; }')
        compiled = self.root / 'build/GC6E01/source.o'
        compiled.parent.mkdir(parents=True)
        def command(args, timeout):
            if args[0] == 'ninja':
                compiled.write_bytes(b'object')
            else:
                symbol = {'name': 'target', 'match_percent': 99.99999, 'instructions': []}
                Path(args[args.index('-o') + 1]).write_text(json.dumps({'left': {'symbols': [symbol]}, 'right': {'symbols': [symbol]}}))
            return subprocess.CompletedProcess(args, 0, '', '')
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'objdiff_cli', return_value=Path('objdiff')), patch.object(campaign, 'checked_command', side_effect=command):
            result = campaign.verify_unlocked({'source': 'source.c', 'symbol': 'target', 'unit': 'u'}, candidate)
        self.assertEqual(result['pct'], 99.99999)
        self.assertEqual(result['returncode'], 1)
        self.assertEqual((self.root / 'source.c').read_bytes(), original)

    def test_failed_restore_is_fatal(self):
        original = 'int f(void) { return 1; }'
        (self.root / 'source.c').write_text(original)
        (self.root / 'candidate.c').write_text('int f(void) { return 2; }')
        failed = subprocess.CompletedProcess([], 1, 'build failed', '')
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'checked_command', return_value=failed):
            with self.assertRaises(campaign.RestorationError):
                campaign.verify_unlocked({'source': 'source.c', 'symbol': 'f', 'unit': 'u'}, self.root / 'candidate.c')
        self.assertEqual((self.root / 'source.c').read_text(), original)

    def test_unchanged_response_skips_compile_and_saves_report(self):
        source = 'int f(void) { return 1; }'
        (self.root / 'source.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90,
                'source_sha256': campaign.digest(source), 'status': 'pending'}
        state['items']['task'] = item
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch.object(campaign, 'ollama', return_value=source), patch.object(campaign, 'verify') as verify:
            campaign.process(state, item, 'unused', 'unused', 1, 'worker')
        verify.assert_not_called()
        self.assertEqual(item['status'], 'no_change')
        saved = json.loads((self.root / 'candidates/task/attempt-001.report.json').read_text())
        self.assertEqual(saved['status'], 'no_change')

    def test_retry_is_bounded_and_moves_to_next_task(self):
        state = campaign.blank_state('unused', 'unused')
        for name in ('a', 'b'):
            state['items'][name] = {'id': name, 'symbol': name, 'source': name + '.c',
                                    'base_pct': 90, 'size': 100, 'residual_functions': 1,
                                    'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        visited = []
        def process(state, item, *args):
            visited.append((item['id'], len(args)))
            item.update(status='no_change', last_error='unchanged', updated_at=campaign.timestamp())
            item['attempts'] += 1
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'process', side_effect=process), patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 2, 1, 0, 'worker', 1024)
        self.assertEqual([name for name, _ in visited], ['a', 'a', 'b', 'b'])
        self.assertEqual([count for _, count in visited], [4, 5, 4, 5])

    def test_unreachable_host_requeues_without_consuming_attempt(self):
        import urllib.error
        source = 'int f(void) { return 1; }'
        (self.root / 'source.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90, 'attempts': 3,
                'source_sha256': campaign.digest(source), 'status': 'pending'}
        state['items']['task'] = item
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch('urllib.request.urlopen', side_effect=urllib.error.URLError('timed out')):
            with self.assertRaises(campaign.HostUnavailable):
                campaign.process(state, item, 'http://test', 'unused', 1, 'worker')
        self.assertEqual(item['status'], 'pending')
        self.assertEqual(item['attempts'], 3)

    def test_unreachable_host_backs_off_then_retries_same_task(self):
        state = campaign.blank_state('unused', 'unused')
        state['items']['a'] = {'id': 'a', 'symbol': 'a', 'source': 'a.c', 'base_pct': 90, 'size': 100,
                               'residual_functions': 1, 'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        calls, pauses = [], []
        def process(state, item, *args):
            calls.append(item['id'])
            if len(calls) < 3:
                raise campaign.HostUnavailable('down')
            item.update(status='non_exact', attempts=1)
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'process', side_effect=process), \
             patch.object(campaign, 'pause', side_effect=pauses.append), patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 1, 1, 0, 'worker', 1024)
        self.assertEqual(calls, ['a', 'a', 'a'])
        self.assertEqual(pauses[:2], [30, 60])

    def test_context_window_never_truncates_large_prompts(self):
        with patch.object(campaign, 'NUM_CTX', 8192):
            self.assertEqual(campaign.context_window('x' * 9000, 1024), 8192)
            self.assertEqual(campaign.context_window('x' * 60000, 1024), campaign.DEFAULT_NUM_CTX)


if __name__ == '__main__':
    unittest.main()
