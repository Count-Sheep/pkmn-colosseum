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

    def test_source_locator_accepts_knr_definition_but_not_prototype(self):
        source = '''int target(arg0, arg1);
int unrelated(void) { return 0; }
int target(arg0, arg1)
    unsigned int arg0;
    const char* arg1;
{
    return arg0 + (arg1 != 0);
}
'''
        start, end = campaign.function_span(source, 'target')
        self.assertEqual(source[start:end], source[source.index('int target(arg0, arg1)\n'):].rstrip())
        with self.assertRaisesRegex(ValueError, 'not found'):
            campaign.function_span('int target(arg0);\nint unrelated(void) { return 0; }', 'target')
        with self.assertRaisesRegex(ValueError, 'not found'):
            campaign.function_span('int target(arg0)\nint arg0;\nint unrelated;\n{ return arg0; }', 'target')

    def test_chained_owner_and_cycle(self):
        (self.root / 'src').mkdir()
        (self.root / 'src/a.c').write_text('#include "src/b.c"')
        (self.root / 'src/b.c').write_text('#include "src/c.c"')
        (self.root / 'src/c.c').write_text('int f(void) { return 1; }')
        with patch.object(campaign, 'ROOT', self.root):
            self.assertEqual(campaign.owner_source('src/a.c'), 'src/c.c')
            (self.root / 'src/real.c').write_text(
                '/* A real owner, not a forwarding shim. */\n'
                '#include "src/c.c"\n'
                'int own(void) { return 2; }\n')
            self.assertEqual(campaign.owner_source('src/real.c'), 'src/real.c')
            (self.root / 'src/flags.c').write_text(
                '#pragma push\n#define SCORE_VARIANT 1\n'
                '#include "src/c.c"\n#pragma pop\n')
            self.assertEqual(campaign.owner_source('src/flags.c'), 'src/c.c')
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

    def test_focused_snippet_duplicate_externs_and_whole_definitions(self):
        source = ("extern u32 lbl_8047A420;\n"
                  "int f(int a) {\n"
                  "    int b;\n"
                  "    b = a + 1;\n"
                  "    return b + lbl_8047A420;\n"
                  "}\n")
        response = "```c\n    extern s32 lbl_8047A420;\n    extern void g(int x);\n    b = 1 + a;\n```"
        proposal = campaign.apply_focused(source, 'f', (4, 4), response)
        self.assertNotIn('extern s32 lbl_8047A420', proposal)   # already declared at file scope
        self.assertIn('extern void g(int x);', proposal)         # new, so kept
        self.assertIn('b = 1 + a;', proposal)
        whole = "```c\nint f(int a) {\n    int b;\n    b = a - -1;\n    return b + lbl_8047A420;\n}\n```"
        proposal = campaign.apply_focused(source, 'f', (4, 4), whole)
        self.assertEqual(proposal.count('int f(int a)'), 1)
        self.assertIn('b = a - -1;', proposal)

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

    def test_rewrite_steps_replay_whole_and_never_half_a_loop(self):
        base = "void f(int n) {\n    int i;\n    s = g();\n    for (i = 0; i < n; i++) {\n        h(i);\n    }\n}\n"
        loop = "write the for (i < n) loop as a while"
        replayed = campaign.replay_rewrites(base, [loop])
        self.assertIn("i = 0;\n    while (i < n) {\n        h(i);\n        i++;\n    }", replayed)
        self.assertIsNone(campaign.replay_rewrites(base, ["swap statements that do not exist"]))
        self.assertEqual(campaign.replay_rewrites(base, []), base)

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

    def test_focused_snippet_redeclaring_a_parameter_is_repaired(self):
        lines = ['void f(void* x, s32 value,', '       s32 (*cb)(int), u8 buf[4]) {', '    u8 sprite[8];',
                 '    s32 local = value;', '}']
        self.assertEqual(campaign.parameter_names(lines), {'x', 'value', 'cb', 'buf'})
        self.assertEqual(campaign.parameter_names(['int f(void) {']), set())
        # A bare redeclaration is dropped; one with an initializer becomes an assignment.
        self.assertEqual(campaign.repair_snippet(lines, 2, 3, ['    u8 sprite[8];', '    s32 value;', '    void* x = 0;']),
                         (['    u8 sprite[8];', '    x = 0;'], set()))

    def test_repeated_or_whitespace_only_answers_are_not_rebuilt(self):
        source = 'int f(int a) {\n    return a + 1;\n}\n'
        (self.root / 'source.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90,
                'source_sha256': campaign.digest(source), 'status': 'pending'}
        state['items']['task'] = item
        changed = 'int f(int a) {\n    return 1 + a;\n}'
        statuses = []
        def verify(item, candidate, worker):
            return {'pct': 80.0, 'deltas': 2, 'diff_feedback': []}
        def answers(*args, **kwargs):
            return next(replies)
        replies = iter([changed, changed.replace('    ', '  '), 'int f(int a)\n{\n    return a + 1;\n}'])
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch.object(campaign, 'ollama', side_effect=answers), \
             patch.object(campaign, 'verify', side_effect=verify) as measured:
            for _ in range(3):
                campaign.process(state, item, 'unused', 'unused', 1, 'worker')
                statuses.append(item['status'])
        self.assertEqual(statuses, ['non_exact', 'duplicate', 'no_change'])
        self.assertEqual(measured.call_count, 1)
        self.assertEqual(json.loads((self.root / 'candidates/task/attempt-002.report.json').read_text())['error'],
                         'identical to attempt 1, which scored 80.0%')

    def test_discarded_answer_feedback_anchors_on_the_best_and_lists_attempts(self):
        source = 'int f(int a) {\n    return a + 1;\n}\n'
        (self.root / 'source.c').write_text(source)
        (self.root / 'worse.c').write_text('int f(int a) {\n    return 1 + a;\n}\n')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90.0, 'attempts': 1,
                'status': 'non_exact', 'last_report': {'pct': 80.0, 'diff_feedback': ['[structural] of the worse answer']},
                'candidate': 'worse.c', 'tried': {'k': {'attempt': 1, 'pct': 80.0, 'candidate': 'worse.c'}}}
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root):
            feedback = campaign.retry_feedback(item)
            self.assertIn('scored 80.0%, lower than the best so far (90.0%, from the assigned source)', feedback)
            self.assertNotIn('of the worse answer', feedback)   # the discarded answer's diff is not the target
            self.assertIn('Attempt 1: 80.0%\n-    return a + 1;\n+    return 1 + a;', feedback)
            # A new best is still corrected directly, quoting its own remaining differences.
            item['best'] = {'pct': 95.0, 'attempt': 1, 'candidate': 'worse.c'}
            item['last_report']['pct'] = 95.0
            self.assertIn('of the worse answer', campaign.retry_feedback(item))

    def test_focused_variants_skip_echoes_and_repeats_and_keep_the_best(self):
        source = "int f(int a) {\n    int b;\n    b = a + 1;\n    return b * 2;\n}\n"
        (self.root / 'source.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90.0, 'attempts': 1,
                'source_sha256': campaign.digest(source), 'status': 'non_exact', 'retry_mode': 'plateau',
                'focus_hunks': [[3]], 'focus_hunk_texts': ['[structural]\n-> addi r3, r3, 1   add r3, r3, r0']}
        state['items']['task'] = item
        prompts = []
        repeated = "int f(int a) {\n    int b;\n    b = 1 + a;\n    return b * 2;\n}"
        item['tried'] = {campaign.digest(' '.join(repeated.split())): {'attempt': 1, 'pct': 91.0, 'candidate': 'x.c'}}
        def ollama(host, model, prompt, timeout, num_predict, *args, **kwargs):
            prompts.append((prompt, num_predict))
            # The region is lines 2-4: an echo, a repeat of attempt 1, a new answer, and a fourth block.
            return ("```c\n    int b;\n    b = a + 1;\n    return b * 2;\n```\n"
                    "```c\n    int b;\n    b = 1 + a;\n    return b * 2;\n```\n"
                    "```c\n    int b;\n    b = a;\n    b += 1;\n    return b * 2;\n```\n"
                    "```c\n    int b;\n    b = (a + 1);\n    return b << 1;\n```")
        scores = iter([93.0, 95.0])
        def verify(item, candidate, worker):
            return {'pct': next(scores), 'deltas': 1, 'diff_feedback': []}
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch.object(campaign, 'ollama', side_effect=ollama), \
             patch.object(campaign, 'verify', side_effect=verify) as measured:
            campaign.process(state, item, 'unused', 'unused', 1, 'worker', 'feedback')
        prompt, cap = prompts[0]
        self.assertIn('-> addi r3, r3, 1', prompt)                   # the hunk is quoted beside the lines
        self.assertIn(f'exactly\n{campaign.FOCUS_VARIANTS} alternative', prompt)
        self.assertEqual(cap, campaign.FOCUS_TOKENS)
        # Echo and repeat are not built; only the first FOCUS_VARIANTS blocks are read.
        self.assertEqual(measured.call_count, 1)
        self.assertEqual(item['last_report']['pct'], 93.0)
        self.assertEqual(item['best']['pct'], 93.0)
        self.assertTrue((self.root / 'candidates/task/attempt-002.c').is_file())

    def test_snippet_adding_an_early_return_is_refused(self):
        region = ['    if (obj == NULL) {', '        return 0;', '    }']
        self.assertTrue(campaign.adds_early_exit(region, '    if (!obj) {\n        return 0;\n    }\n    return 1;\n'))
        self.assertFalse(campaign.adds_early_exit(region, '    if (!obj) {\n        return 0;\n    }\n'))
        self.assertFalse(campaign.adds_early_exit(['    x = 1;', '    return x;'], '    return 1;\n'))
        source = "int f(int* obj) {\n    int v;\n    if (obj == NULL) {\n        return 0;\n    }\n    v = *obj;\n    return v;\n}\n"
        (self.root / 'source.c').write_text(source)
        state = campaign.blank_state('unused', 'unused')
        item = {'id': 'task', 'symbol': 'f', 'source': 'source.c', 'base_pct': 90.0, 'attempts': 1,
                'source_sha256': campaign.digest(source), 'status': 'non_exact', 'retry_mode': 'plateau',
                'focus_hunks': [[4]]}
        state['items']['task'] = item
        early = "```c\n    if (!obj) {\n        return 0;\n    }\n    return 1;\n```"
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root), \
             patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(campaign, 'build_prompt', return_value=('brief', 'prompt')), \
             patch.object(campaign, 'focus_region', return_value=(3, 5)), \
             patch.object(campaign, 'ollama', return_value=early * 3), patch.object(campaign, 'verify') as measured:
            campaign.process(state, item, 'unused', 'unused', 1, 'worker', 'feedback')
            feedback = campaign.retry_feedback(item)
        measured.assert_not_called()
        self.assertEqual(item['status'], 'invalid_edit')
        self.assertIn('statements after them must still run', feedback)

    def test_openai_compatible_server_stream(self):
        def sse(content=None, finish=None):
            return ('data: ' + json.dumps({'choices': [{'delta': {'content': content} if content else {},
                                                        'finish_reason': finish}]}) + '\n').encode()
        payload = campaign.openai_payload('m', 'task', 64, 0.45)
        self.assertEqual(payload['messages'], [{'role': 'user', 'content': 'task'}])
        self.assertEqual((payload['max_tokens'], payload['temperature'], payload['stream']), (64, 0.45, True))
        self.assertIsNone(campaign.openai_packet(b': keep-alive\n'))
        self.assertTrue(campaign.openai_packet(b'data: [DONE]\n')['done'])
        requests = []
        class Stream(list):
            def __enter__(self): return self
            def __exit__(self, *exc): return False
        def urlopen(request, timeout):
            requests.append(request)
            return Stream([sse('```c\nint f'), b'\n', sse('(void);\n```'), sse(finish='stop'), b'data: [DONE]\n'])
        with patch.object(campaign.urllib.request, 'urlopen', side_effect=urlopen):
            answer = campaign.ollama('http://box:8080/v1', 'm', 'task', 60, 64, lambda **kw: None)
            self.assertEqual(answer, '```c\nint f(void);\n```')
            self.assertEqual(requests[0].full_url, 'http://box:8080/v1/chat/completions')
            urlopen_cut = lambda request, timeout: Stream([sse('```c\nint'), sse(finish='length')])
            with patch.object(campaign.urllib.request, 'urlopen', side_effect=urlopen_cut):
                with self.assertRaises(campaign.IncompleteResponse):
                    campaign.ollama('http://box:8080/v1', 'm', 'task', 60, 64, lambda **kw: None)

    def test_sampling_profile_reaches_both_request_formats(self):
        with patch.object(campaign, 'SAMPLING', {'top_k': 20, 'min_p': 0}), \
             patch.object(campaign, 'TEMPLATE_KWARGS', {'reasoning_effort': 'medium'}), patch.object(campaign, 'THINK', None):
            chat = campaign.openai_payload('m', 'p', 64, 0.6, seed=7)
            native = campaign.ollama_payload('m', 'p', 64, 0.6, seed=7)
        self.assertEqual((chat['top_k'], chat['min_p'], chat['seed']), (20, 0, 7))
        self.assertEqual(chat['chat_template_kwargs'], {'reasoning_effort': 'medium'})
        self.assertEqual((native['options']['top_k'], native['options']['seed']), (20, 7))
        self.assertNotIn('chat_template_kwargs', campaign.openai_payload('m', 'p', 64))

    def test_streamed_reasoning_marks_the_model_as_reasoning(self):
        class Stream(list):
            def __enter__(self): return self
            def __exit__(self, *exc): return False
        def sse(delta, finish=None):
            return ('data: ' + json.dumps({'choices': [{'delta': delta, 'finish_reason': finish}]}) + '\n').encode()
        stream = Stream([sse({'reasoning_content': 'think'}), sse({'content': '```c\nint f;\n```'}), sse({}, 'stop')])
        with patch.object(campaign, 'REASONING_SEEN', False), \
             patch.object(campaign.urllib.request, 'urlopen', return_value=stream):
            campaign.ollama('http://box/v1', 'm', 'p', 60, 64, lambda **kw: None)
            self.assertTrue(campaign.REASONING_SEEN)

    def test_output_cap_follows_the_function_size(self):
        self.assertEqual(campaign.output_cap(4096, 'x' * 663, False, None), 768)
        self.assertEqual(campaign.output_cap(4096, 'x' * 3500, False, None), 2000 + 384)
        self.assertEqual(campaign.output_cap(4096, 'x' * 70000, False, None), 4096)
        self.assertEqual(campaign.output_cap(8192, 'x' * 663, False, 'incomplete_response'), 8192)
        self.assertEqual(campaign.output_cap(4096, 'x' * 663, True, 'plateau'), campaign.FOCUS_TOKENS)

    def test_compile_error_and_cut_off_feedback(self):
        output = ("[1/1] MWCC x.o\n### mwcceppc.exe Compiler:\n#      In: src\\game\\a.c\n#    From: src\\game\\b.c\n"
                  "# --------------------------------------------------\n#     823: owner->field_68 = sequence;\n"
                  "#   Error:      ^^\n#   not a struct/union/class\n\nErrors caused tool to abort.\n")
        error = campaign.compile_error(output)
        self.assertIn('not a struct/union/class', error)
        self.assertNotIn('From:', error)
        item = {'id': 't', 'status': 'incomplete_response', 'attempts': 1, 'base_pct': 97.0,
                'last_error': 'model reached output cap', 'last_report': {}}
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_DIR', self.root):
            (self.root / 'candidates/t').mkdir(parents=True)
            (self.root / 'candidates/t/attempt-001.response.md').write_text('Looking at r3 and r5 ' * 200)
            feedback = campaign.retry_feedback(item)
        self.assertIn('only the ```c', feedback)
        self.assertNotIn('Looking at r3', feedback)

    def test_hunks_label_target_and_ours(self):
        rows = [('mflr r0', 'mflr r0', 1), ('li r3, 0', 'li r4, 0', 2), ('blr', 'blr', 3)]
        item = {'symbol': 'f'}
        with patch.object(campaign, '_objdiff_rows', return_value=rows):
            hunks = campaign.line_annotated_hunks(item, 'int f(void) {\n    return 0;\n}\n')
        self.assertIn('TARGET (retail, want)', hunks[0])
        self.assertIn('OURS (this source, have)', hunks[0])

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
