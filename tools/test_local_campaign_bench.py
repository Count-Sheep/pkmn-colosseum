"""Checks for the model benchmark: a fixed, stratified task set and history-free task copies."""

import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import local_campaign as campaign
import local_campaign_bench as bench
from local_campaign_coordination import Coordinator


class BenchTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def test_select_is_stratified_one_per_owner_and_skips_unusable(self):
        state = campaign.blank_state('unused', 'unused')
        for index, pct in enumerate([99.5, 99.1, 95.0, 90.0, 60.0, 50.0]):
            source = f'src/{index}.c'
            (self.root / 'src').mkdir(exist_ok=True)
            (self.root / source).write_text(f'int f{index}(void) {{ return {index}; }}\n')
            state['items'][f't{index}'] = {'id': f't{index}', 'symbol': f'f{index}', 'source': source, 'size': 100,
                                           'base_pct': pct, 'status': 'pending',
                                           'source_sha256': campaign.digest((self.root / source).read_text())}
        state['items']['t1']['owner_source'] = 'src/0.c'          # shares an owner with t0
        state['items']['t5']['status'] = 'blocked_source_context'
        campaign.write_json(self.root / 'state.json', state)
        with patch.object(campaign, 'ROOT', self.root), patch.object(campaign, 'STATE_FILE', self.root / 'state.json'), \
             patch.object(campaign, 'COORDINATOR', Coordinator(self.root)), \
             patch.object(bench, 'BENCH_DIR', self.root / 'bench'), patch.object(bench, 'SET_FILE', self.root / 'bench/set.json'):
            self.assertEqual(bench.select(1.0, 1, 2048, False), 0)
            chosen = json.loads((self.root / 'bench/set.json').read_text())['tasks']
            self.assertEqual(bench.select(1.0, 1, 2048, False), 1)   # an existing set is never replaced silently
        self.assertEqual(sorted(row['id'] for row in chosen), ['t0', 't2', 't3', 't4'])
        self.assertEqual({row['band'] for row in chosen}, {'98-100', '93-98', '85-93', '0-85'})

    def test_fresh_item_drops_campaign_history(self):
        item = {'id': 't', 'symbol': 'f', 'source': 's.c', 'base_pct': 90.0, 'status': 'non_exact', 'attempts': 7,
                'best': {'pct': 95.0}, 'tried': {'k': {}}, 'focus_round': 2, 'source_sha256': 'abc'}
        fresh = bench.fresh_item(item)
        self.assertEqual((fresh['status'], fresh['attempts'], fresh['source_sha256']), ('pending', 0, 'abc'))
        self.assertNotIn('best', fresh)
        self.assertNotIn('tried', fresh)
        self.assertEqual(item['attempts'], 7)   # the campaign's copy is untouched

    def test_summary_counts_improvements_and_rates(self):
        tasks = [
            {'outcome': 'done', 'base_pct': 90.0, 'best_pct': 95.0, 'seconds': 1800,
             'attempt_rows': [{'status': 'non_exact', 'variants': 3, 'generation_seconds': 5},
                              {'status': 'no_change', 'variants': 0, 'generation_seconds': 2}]},
            {'outcome': 'done', 'base_pct': 99.0, 'best_pct': 100.0, 'seconds': 1800,
             'attempt_rows': [{'status': 'review_exact', 'variants': 1, 'generation_seconds': 4}]},
            {'outcome': 'stale', 'base_pct': 80.0, 'best_pct': 80.0, 'seconds': 0},
        ]
        summary = bench.summarise({'name': 'x'}, tasks)
        self.assertEqual((summary['measured'], summary['skipped'], summary['improved'], summary['exact']), (2, 1, 2, 1))
        self.assertEqual(summary['attempts_per_hour'], 3.0)
        self.assertEqual(summary['improved_per_hour'], 2.0)
        self.assertAlmostEqual(summary['rates']['no_change'], 0.333, places=3)
        self.assertEqual(summary['mean_gain_without_top_pp'], 1.0)   # gains 5.0 and 1.0, the top one left out

    def test_prelim_is_a_size_spread_subset_of_the_main_set(self):
        tasks = [{'id': f't{i}', 'symbol': f'f{i}', 'size': size, 'base_pct': 90.0}
                 for i, size in enumerate([400, 72, 1500, 90, 800, 250, 1100, 600, 150])]
        with patch.object(bench, 'BENCH_DIR', self.root), patch.object(bench, 'SET_FILE', self.root / 'set.json'):
            campaign.write_json(self.root / 'set.json', {'created_at': 'x', 'tasks': tasks})
            self.assertEqual(bench.select_prelim(5, False), 0)
            chosen = json.loads(bench.set_file('prelim').read_text())['tasks']
        self.assertEqual([row['size'] for row in chosen], [72, 150, 400, 800, 1500])

    def test_gate_checks_speed_and_answer_health_only(self):
        def run(name, per_hour, unusable=0.0, no_compile=0.0, measured=5):
            return {'summary': {'name': name, 'measured': measured, 'tasks': 5, 'skipped': 5 - measured,
                                'attempts_per_hour': per_hour, 'unusable_rate': unusable,
                                'rates': {'verification_error': no_compile}, 'improved': 0}}
        verdicts = bench.gate([run('fast', 300), run('slow', 120), run('broken', 280, unusable=0.4),
                               run('cut', 300, measured=3)])
        self.assertTrue(verdicts['fast'][0])
        self.assertIn('slow', verdicts['slow'][1])
        self.assertIn('unusable', verdicts['broken'][1])
        self.assertIn('incomplete', verdicts['cut'][1])

    def test_sign_test(self):
        self.assertEqual(bench.sign_test(6, 6), 1.0)
        self.assertLess(bench.sign_test(10, 1), 0.02)
        self.assertEqual(bench.sign_test(0, 0), 1.0)


if __name__ == '__main__':
    unittest.main()
