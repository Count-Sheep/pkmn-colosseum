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

    def test_focused_retry_edits_the_best_candidate_and_keeps_its_gain(self):
        source = "int f(int a) {\n    int b;\n    b = a + 1;\n    a = 0;\n    b += a;\n    return b * 2;\n}\n"
        best = "int f(int a) {\n    int b;\n    b = 1 + a;\n    a = 0;\n    b += a;\n    return b * 2;\n}\n"
        (self.root / 'source.c').write_text(source)
        (self.root / 'best.c').write_text(best)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90, 'attempts': 1,
                'source_sha256': campaign.digest(source), 'status': 'non_exact', 'retry_mode': 'plateau',
                'best': {'pct': 95.0, 'attempt': 1, 'candidate': 'best.c', 'focus_hunks': [[6]]}}
        state['items']['task'] = item
        prompts = []
        def ollama(host, model, prompt, *args, **kwargs):
            prompts.append(prompt)
            return "```c\n    b += a;\n    return b << 1;\n```"
        def verify(item, candidate, worker):
            item['focus_hunks'] = [[3]]  # verify re-annotates the candidate in place
            return {'pct': 97.0, 'deltas': 1, 'diff_feedback': []}
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch.object(campaign, 'ollama', side_effect=ollama), patch.object(campaign, 'verify', side_effect=verify):
            campaign.process(state, item, 'unused', 'unused', 1, 'worker', 'feedback')
        self.assertIn('best candidate so far', prompts[0])
        written = (self.root / 'candidates/task/attempt-002.c').read_text()
        self.assertIn('b = 1 + a;', written)       # the earlier gain survives
        self.assertIn('return b << 1;', written)   # the focused fix is spliced in
        self.assertEqual(item['best']['pct'], 97.0)
        self.assertEqual(item['best']['focus_hunks'], [[3]])
        self.assertNotIn('focus_hunks', item)      # the assigned source's hunks were not overwritten

    def test_missing_owner_source_marks_stale_instead_of_crashing(self):
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'gone', 'symbol': 'f', 'source': 'deleted.c', 'base_pct': 90, 'status': 'pending',
                'source_sha256': 'x'}
        state['items']['gone'] = item
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)):
            campaign.process(state, item, 'unused', 'unused', 1, 'worker')
        self.assertEqual(item['status'], 'stale_source')

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

    def test_sharpen_diff_ignores_branch_offset_only_rows(self):
        brief = "\n".join([
            "```",
            "   #  TARGET (want)                          OURS (have)",
            "    0  cmpwi r5, 0x7                          cmpwi r5, 0x7",
            ">>  1  beq 0x4a4                              beq 0x434",
            ">>  2  bne 0x438                              beq 0x42c",
            ">>  3  addi r5, r5, 0x10                      addi r0, r5, 0x10",
            "```",
        ])
        sharpened, hunks = campaign.sharpen_diff(brief)
        self.assertIn("    1  beq 0x4a4", sharpened)
        self.assertIn(">>  2  bne 0x438", sharpened)
        self.assertEqual(len(hunks), 1)
        self.assertIn("-> bne 0x438", hunks[0])
        self.assertIn("-> addi r5, r5, 0x10", hunks[0])

    def test_diff_class_separates_noise_registers_and_shape(self):
        self.assertEqual(campaign.diff_class('lfs f3, lbl_8047CD08@sda21', 'lfs f3, @182@sda21'), 'data')
        self.assertEqual(campaign.diff_class('beq 0x4a4', 'beq 0x434'), 'branch')
        self.assertEqual(campaign.diff_class('srwi r4, r3, 16', 'srwi r0, r3, 16'), 'register')
        self.assertEqual(campaign.diff_class('', 'mr r31, r0'), 'structural')
        self.assertEqual(campaign.diff_class('beq 0x43c', 'bne 0x23a0'), 'structural')
        self.assertEqual(campaign.diff_class('lwz r3, lbl_80478B08@sda21', 'lwz r3, lbl_80478B0C@sda21'), 'structural')
        self.assertEqual(campaign.diff_class('lis r6, jumptable_8036CB84@ha', 'lis r6, @128@ha'), 'data')
        self.assertEqual(campaign.diff_class('', 'addi r6, r6, @128@l'), 'data')
        self.assertEqual(campaign.diff_class('', 'lfs f1, @123@sda21'), 'structural')

    def test_structural_hunks_are_listed_before_register_only(self):
        brief = "\n".join([
            "```",
            ">>  0  srwi r4, r3, 16                        srwi r0, r3, 16",
            "    1  li r5, 0x0                             li r5, 0x0",
            "    2  li r6, 0x0                             li r6, 0x0",
            "    3  li r7, 0x0                             li r7, 0x0",
            "    4  li r8, 0x0                             li r8, 0x0",
            ">>  5  lfs f3, lbl_8047CD08@sda21             lfs f3, @182@sda21",
            "    6  li r9, 0x0                             li r9, 0x0",
            "    7  li r10, 0x0                            li r10, 0x0",
            "    8  li r11, 0x0                            li r11, 0x0",
            ">>  9  clrlwi r4, r3, 16",
            "```",
        ])
        sharpened, hunks = campaign.sharpen_diff(brief)
        self.assertIn("    5  lfs f3", sharpened)
        self.assertEqual(len(hunks), 2)
        self.assertTrue(hunks[0].startswith("[structural]"))
        self.assertIn("clrlwi r4, r3, 16", hunks[0])
        self.assertTrue(hunks[1].startswith("[register-only]"))

    def test_line_annotated_hunks_quote_the_producing_statements(self):
        source = "int f(int a) {\n    int b = a + 1;\n    return b * 2;\n}\n"
        rows = [('stwu r1, -0x10(r1)', 'stwu r1, -0x10(r1)', 1),
                ('addi r3, r3, 0x1', 'addi r4, r3, 0x1', 2),
                ('', 'mr r3, r4', 2),
                ('slwi r3, r3, 1', 'slwi r3, r3, 1', 3),
                ('blr', 'blr', 4)]
        with patch.object(campaign, '_objdiff_rows', return_value=rows):
            hunks = campaign.line_annotated_hunks({'symbol': 'f'}, source)
        self.assertEqual(len(hunks), 1)
        self.assertTrue(hunks[0].startswith('[structural]'))
        self.assertIn('int b = a + 1;', hunks[0])
        self.assertIn('return b * 2;', hunks[0])
        with patch.object(campaign, '_objdiff_rows', return_value=None):
            self.assertIsNone(campaign.line_annotated_hunks({'symbol': 'f'}, source))

    def test_melee_reference_found_by_name_and_policy_filtered(self):
        base = self.root / 'melee' / 'src' / 'sysdolphin'
        base.mkdir(parents=True)
        (base / 'mtx.c').write_text('void HSD_MtxGetScale(Mtx m, Vec3* v)\n{\n    v->x = m[0][0];\n}\n\n'
                                    'void HSD_Shaped(void)\n{\n    asm { nop }\n}\n')
        with patch.object(campaign, 'MELEE_ROOT', self.root / 'melee' / 'src'), patch.dict(campaign._MELEE_INDEX, {}, clear=True):
            reference = campaign.melee_reference('HSD_MtxGetScale')
            self.assertIn('v->x = m[0][0];', reference)
            self.assertEqual(campaign.melee_reference('HSD_Shaped'), '')
            self.assertEqual(campaign.melee_reference('fn_80000000'), '')

    def test_focused_edit_replaces_only_the_hunk_lines(self):
        source = ("int other(void) { return 0; }\n"
                  "int f(int a) {\n"
                  "    int b;\n"
                  "    b = a + 1;\n"
                  "    return b * 2;\n"
                  "}\n")
        region = campaign.focus_region(source, 'f', [4], context=0)
        self.assertEqual(region, (4, 4))
        self.assertIsNone(campaign.focus_region(source, 'f', [1]))
        proposal = campaign.apply_focused(source, 'f', region, "```c\n    b = 1 + a;\n```")
        self.assertIn('b = 1 + a;', proposal)
        self.assertIn('return b * 2;', proposal)
        self.assertTrue(proposal.startswith('int f(int a) {'))
        with self.assertRaises(ValueError):
            campaign.apply_focused(source, 'f', region, "no code here")
        prompt = campaign.focused_prompt('base', source, region)
        self.assertIn('    b = a + 1;', prompt.split('## Focused edit')[1])
        self.assertIn('ran out of output tokens', campaign.focused_prompt('base', source, region, 'incomplete_response'))
        self.assertIn('too long', campaign.focused_prompt('base', source, region, 'too_large'))

    def test_focused_snippet_redeclarations_and_dropped_declarations_are_repaired(self):
        source = ("int f(int a) {\n"
                  "    int handle;\n"
                  "    int count;\n"
                  "    int size = 0;\n"
                  "    handle = a;\n"
                  "    count = handle + size;\n"
                  "    return count;\n"
                  "}\n")
        # Region = lines 4-6. The model redeclares `handle` (declared outside the region) and
        # drops `size`, which it still uses.
        response = "```c\n    int handle = a * 2;\n    count = size + handle;\n```"
        proposal = campaign.apply_focused(source, 'f', (4, 6), response)
        self.assertIn('    int size = 0;\n    handle = a * 2;\n    count = size + handle;', proposal)
        self.assertEqual(proposal.count('int handle'), 1)
        # A declaration the region itself made stays, even when another block reuses the name.
        lines = ['{', '    int i;', '    {', '        int i;', '        i = 1;', '    }', '}']
        self.assertEqual(campaign.repair_snippet(lines, 3, 5, ['        int i;', '        i = 2;']),
                         (['        int i;', '        i = 2;'], set()))
        # Declaration lists count on both sides; a call's argument commas are not split.
        lines = ['{', '    s32 i, j;', '    u32 h = f(a, b);', '    i = 0;', '    j = h;', '}']
        self.assertEqual(campaign.repair_snippet(lines, 3, 5, ['    s32 i;', '    s32 k = 3, j = 1;', '    i = k;']),
                         (['    s32 k = 3;', '    j = 1;', '    i = k;'], set()))
        self.assertEqual(set(campaign.local_declarations(lines)), {'i', 'j', 'h'})
        # A call initializer is never turned into an assignment (it could move the call).
        self.assertEqual(campaign.repair_snippet(lines, 3, 5, ['    u32 h = g();', '    i = 0;']),
                         (['    u32 h = g();', '    i = 0;'], set()))

    def test_focused_snippet_that_moves_a_declaration_deletes_the_old_copy(self):
        source = ("int f(void) {\n"
                  "    int a = 1;\n"
                  "    int b = 2;\n"
                  "    u32 h = get();\n"
                  "    return a + b + h;\n"
                  "}\n")
        # Region = lines 2-3 (the declarations of a and b); the model hoists h's declaration.
        response = "```c\n    u32 h = get();\n    int b = 2;\n    int a = 1;\n```"
        proposal = campaign.apply_focused(source, 'f', (2, 3), response)
        self.assertEqual(proposal.count('get()'), 1)
        self.assertIn('    u32 h = get();\n    int b = 2;\n    int a = 1;\n    return a + b + h;', proposal)

    def test_focus_region_takes_whole_statements_and_skips_the_opening_brace(self):
        source = ("void f(void)\n"          # 1
                  "{\n"                     # 2
                  "    int i;\n"            # 3
                  "    call(a, b,\n"        # 4
                  "         c);\n"          # 5
                  "    for (i = 0; i < 3; i++) {\n"  # 6
                  "        g(i);\n"         # 7
                  "    }\n"                 # 8
                  "    h();\n"              # 9
                  "}\n")                    # 10
        # Line 2 is the opening brace: a hunk on line 3 with context starts at line 3.
        self.assertEqual(campaign.focus_region(source, 'f', [3]), (3, 5))
        # A hunk on the call's first line takes the whole call.
        self.assertEqual(campaign.focus_region(source, 'f', [4], context=0), (4, 5))
        # A hunk on the loop header is widened to close the loop's block.
        self.assertEqual(campaign.focus_region(source, 'f', [6], context=0), (6, 8))
        self.assertEqual(campaign.focus_region(source, 'f', [7], context=0), (7, 7))

    def test_large_function_exceeds_output_budget(self):
        self.assertFalse(campaign.exceeds_budget('x' * 4000, 4096))
        self.assertTrue(campaign.exceeds_budget('x' * 28445, 8192))
        self.assertTrue(campaign.exceeds_budget('x' * 28445, 16384 // 2))

    def test_retry_temperature_rises(self):
        self.assertEqual(campaign.ollama_payload('m', 'p', 10)['options']['temperature'], 0.15)
        self.assertEqual(campaign.ollama_payload('m', 'p', 10, 0.7)['options']['temperature'], 0.7)
        self.assertEqual(campaign.RETRY_TEMPERATURES[0], 0.15)
        self.assertGreater(campaign.RETRY_TEMPERATURES[-1], campaign.RETRY_TEMPERATURES[1])

    def test_no_change_feedback_does_not_echo_the_unchanged_answer(self):
        item = {'id': 't', 'status': 'no_change', 'attempts': 1, 'base_pct': 97.0,
                'last_error': 'model repeated the existing function unchanged', 'last_report': {'status': 'no_change'}}
        feedback = campaign.retry_feedback(item)
        self.assertIn('[structural]', feedback)
        self.assertNotIn('Previous response', feedback)

    def test_recycle_requeues_only_attempts_older_than_the_harness(self):
        state = campaign.blank_state('unused', 'unused')
        state['events'].append({'at': '2026-01-02T00:00:00', 'kind': 'harness_improved'})
        rows = {'old': ('no_change', '2026-01-01T00:00:00', None), 'new': ('no_change', '2026-01-03T00:00:00', None),
                'rejected': ('non_exact', '2026-01-01T00:00:00', 'shaping'), 'exact': ('review_exact', '2026-01-01T00:00:00', None)}
        for name, (status, at, rejected) in rows.items():
            state['items'][name] = {'id': name, 'symbol': name, 'source': name + '.c', 'size': 100, 'status': status,
                                    'last_attempt_at': at, 'promotion_rejected': rejected}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)):
            self.assertEqual(campaign.recycle_attempted(state), 1)
        self.assertEqual([k for k, v in state['items'].items() if v['status'] == 'pending'], ['old'])

    def test_min_pct_limits_run_and_recycle_to_near_matches(self):
        state = campaign.blank_state('unused', 'unused')
        for name, pct in (('near', 97.5), ('far', 60.0)):
            state['items'][name] = {'id': name, 'symbol': name, 'source': name + '.c', 'base_pct': pct, 'size': 100,
                                    'residual_functions': 1, 'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        visited = []
        def process(state, item, *args):
            visited.append(item['id'])
            item.update(status='non_exact', attempts=1, updated_at=campaign.timestamp())
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'process', side_effect=process), patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 0, 1, 0, 'worker', 1024, 0, 95)
            state = campaign.load_state('unused', 'unused', update_settings=False)
            state['events'].append({'at': '9999-01-01T00:00:00', 'kind': 'model_tuned'})
            state['items']['far']['status'] = 'no_change'
            self.assertEqual(campaign.recycle_attempted(state, min_pct=95), 1)
        self.assertEqual(visited, ['near'])
        self.assertEqual(state['items']['far']['status'], 'no_change')

    def test_register_only_no_change_skips_retries_and_sorts_last(self):
        state = campaign.blank_state('unused', 'unused')
        state['items']['reg'] = {'id': 'reg', 'symbol': 'reg', 'source': 'reg.c', 'base_pct': 99.9, 'size': 100,
                                 'residual_functions': 1, 'status': 'pending', 'attempts': 1, 'structural_hunks': 0}
        state['items']['shape'] = {'id': 'shape', 'symbol': 'shape', 'source': 'shape.c', 'base_pct': 96.0, 'size': 100,
                                   'residual_functions': 1, 'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        visited = []
        def process(state, item, *args):
            visited.append(item['id'])
            item.update(status='no_change', last_error='unchanged', attempts=item['attempts'] + 1,
                        last_report={'status': 'no_change'}, updated_at=campaign.timestamp())
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), patch.object(campaign, 'RETRIES', 3), \
             patch.object(campaign, 'process', side_effect=process), patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 0, 1, 0, 'worker', 1024)
        self.assertEqual(visited, ['shape', 'shape', 'shape', 'shape', 'reg'])

    def test_declaration_block_and_orderings(self):
        import local_campaign_permute as permute
        function = ("int f(int a) {\n    int b;\n    u8* p = NULL;\n    f32 x = 1.0f;\n\n"
                    "    b = a;\n    return b;\n}\n")
        start, end, lines = permute.declaration_block(function)
        self.assertEqual([l.strip() for l in lines], ['int b;', 'u8* p = NULL;', 'f32 x = 1.0f;'])
        self.assertEqual(function[start:end], "".join(lines))
        orders = list(permute.orderings(lines))
        self.assertEqual(len(orders), 5)
        self.assertNotIn(lines, orders)
        self.assertIsNone(permute.declaration_block("int f(void) {\n    int b = g();\n    return b;\n}\n"))
        self.assertEqual(len(list(permute.orderings([str(i) for i in range(8)], cap=50))), 50)

    def test_permute_search_keeps_best_order_and_restores_source(self):
        source = "int f(int a) {\n    int b;\n    int c;\n    b = a;\n    c = b;\n    return c;\n}\n"
        (self.root / 'f.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 't', 'symbol': 'f', 'source': 'f.c', 'base_pct': 99.0, 'status': 'non_exact',
                'source_sha256': campaign.digest(source)}
        state['items']['t'] = item
        def score(item, directory):
            text = (self.root / 'f.c').read_text()
            return 100.0 if text.index('int c;') < text.index('int b;') else 99.0
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'scratch_score', side_effect=score):
            top = campaign.permute_search(state, item, 'worker')
        self.assertEqual(top, 100.0)
        self.assertEqual((self.root / 'f.c').read_text(), source)
        self.assertEqual(item['status'], 'review_exact')
        best = (self.root / item['best']['candidate']).read_text()
        self.assertLess(best.index('int c;'), best.index('int b;'))
        self.assertFalse(campaign.permute_eligible(item))

    def test_retries_stop_after_stall_rounds(self):
        state = campaign.blank_state('unused', 'unused')
        state['items']['a'] = {'id': 'a', 'symbol': 'a', 'source': 'a.c', 'base_pct': 96.0, 'size': 100,
                               'residual_functions': 1, 'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        calls = []
        def process(state, item, *args):
            calls.append(item['id'])
            item.update(status='non_exact', attempts=len(calls), last_report={'pct': 90.0},
                        last_error='x', updated_at=campaign.timestamp())
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), patch.object(campaign, 'RETRIES', 12), \
             patch.object(campaign, 'STALL_ROUNDS', 3), patch.object(campaign, 'process', side_effect=process), \
             patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 1, 1, 0, 'worker', 1024)
        self.assertEqual(len(calls), 4)

    def test_flat_or_echoed_retries_become_focused_and_rotate_hunks(self):
        state = campaign.blank_state('unused', 'unused')
        state['items']['a'] = {'id': 'a', 'symbol': 'a', 'source': 'a.c', 'base_pct': 96.0, 'size': 100,
                               'residual_functions': 1, 'status': 'pending', 'attempts': 0}
        state_file = self.root / 'state.json'
        campaign.write_json(state_file, state)
        seen = []
        outcomes = ['non_exact', 'non_exact', 'no_change', 'non_exact', 'non_exact']
        def process(state, item, *args):
            seen.append((item.pop('retry_mode', None), item.get('focus_round', 0)))
            item['last_focused'] = seen[-1][0] is not None
            item.update(status=outcomes[len(seen) - 1], attempts=len(seen), last_report={'pct': 90.0},
                        last_error='x', updated_at=campaign.timestamp())
            campaign.save_state(state)
        with patch.object(campaign, 'STATE_DIR', self.root), patch.object(campaign, 'STATE_FILE', state_file), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), patch.object(campaign, 'RETRIES', 4), \
             patch.object(campaign, 'STALL_ROUNDS', 9), patch.object(campaign, 'process', side_effect=process), \
             patch.object(campaign, 'register_only', return_value=False), patch('builtins.print'):
            campaign.run(state, 'unused', 'unused', 1, 1, 0, 'worker', 1024)
        # First retry after an ordinary first attempt is whole-function; a flat score then switches to
        # focused edits, which stay on and move to the next hunk each time the score does not rise.
        self.assertEqual(seen, [(None, 0), (None, 0), ('plateau', 0), ('no_change', 1), ('plateau', 2)])

    def test_focused_prompt_shows_the_best_candidate_it_builds_on(self):
        source = "int f(int a) {\n    int b;\n    b = a + 1;\n    return b;\n}\n"
        prompt = campaign.focused_prompt('base', source, (3, 3), 'plateau', current=source)
        self.assertIn('best candidate so far', prompt)
        self.assertIn('stopped raising the score', prompt)
        self.assertIn('Lines to rewrite:\n\n```c\n    b = a + 1;\n```', prompt)

    def test_same_file_context_includes_callee_bodies_and_data(self):
        source = ("static s32 lbl_1;\nstatic s32 helper(s32 x) {\n    return x + lbl_1;\n}\n"
                  "s32 f(s32 a) {\n    return helper(a);\n}\n")
        index = {'functions': {'f': {'callees': ['helper'], 'data': ['lbl_1']}}}
        with patch.object(campaign.corpus, 'load_index', return_value=index):
            context = campaign.same_file_context({'symbol': 'f'}, source)
        self.assertIn('return x + lbl_1;', context)
        self.assertIn('static s32 lbl_1;', context)

    def test_large_diff_tables_are_capped_but_small_ones_kept(self):
        big = "## Instruction diff\n```\n" + "row\n" * 5000 + "```\n## Next\n"
        capped = campaign.cap_diff_table(big)
        self.assertIn('5001-row instruction table is omitted', capped)
        self.assertIn('## Next', capped)
        small = "## Instruction diff\n```\nrow\n```\n"
        self.assertEqual(campaign.cap_diff_table(small), small)
        self.assertIn('declare it earlier', campaign.MWCC_GUIDE)

    def test_pragmas_active_at_honours_push_pop_and_reset(self):
        text = ("#pragma optimization_level 1\nint a(void) { return 0; }\n"
                "#pragma push\n#pragma scheduling off\nint b(void) { return 1; }\n#pragma pop\n"
                "#pragma optimization_level reset\nint c(void) { return 2; }\n")
        self.assertEqual(campaign.pragmas_active_at(text, text.index('int a')), {'optimization_level': '1'})
        self.assertEqual(campaign.pragmas_active_at(text, text.index('int b')),
                         {'optimization_level': '1', 'scheduling': 'off'})
        self.assertEqual(campaign.pragmas_active_at(text, text.index('int c')), {})

    def test_neutralize_wraps_only_the_target_with_unit_defaults(self):
        text = "#pragma scheduling off\nint f(void) { return 0; }\nint g(void) { return 1; }\n"
        out = campaign.neutralize(text, 'f', 'mwcceppc -O4,p -opt nopeephole -c x.c')
        self.assertIn("#pragma push\n#pragma optimization_level 4\n#pragma optimize_for_size off\n", out)
        self.assertIn("#pragma peephole off\n", out)
        self.assertIn("#pragma scheduling on\n", out)
        self.assertLess(out.index('#pragma pop'), out.index('int g'))
        self.assertGreater(out.index('#pragma pop'), out.index('int f'))

    def test_think_flag_only_sent_when_chosen(self):
        with patch.object(campaign, 'THINK', None):
            self.assertNotIn('think', campaign.ollama_payload('m', 'p', 10))
        with patch.object(campaign, 'THINK', False):
            self.assertIs(campaign.ollama_payload('m', 'p', 10)['think'], False)

    def test_context_window_never_truncates_large_prompts(self):
        with patch.object(campaign, 'NUM_CTX', 8192):
            self.assertEqual(campaign.context_window('x' * 9000, 1024), 8192)
            self.assertEqual(campaign.context_window('x' * 60000, 1024), campaign.DEFAULT_NUM_CTX)


if __name__ == '__main__':
    unittest.main()
