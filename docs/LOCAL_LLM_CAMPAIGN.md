# Local LLM campaign

`tools/local_campaign.py` uses a local Ollama model to create review-only
proposals. It reads the canonical `build/GC6E01/report.json`, accounts for
every function below 100%, asks the model for one replacement function
definition where an editable source owner exists, and verifies an ignored
candidate copy through the existing objdiff-based verifier. Entries with no
declared source are retained as `blocked_no_editable_source`; the campaign must
not invent a split or source owner just to make those entries runnable.

It does not write to `src/`, `include/`, `configure.py`, or any tracked file.
An exact result is still only a review candidate. This retains the strict
campaign policy: no `.inc`, asm, compiler-control pragmas, generated products,
or code-generation shaping may be promoted because it happened to match.

## Start

First refresh the authoritative report and create the live worklist:

```bash
python3 configure.py --no-progress
ninja all_source build/GC6E01/report.json
python3 tools/local_campaign.py sync
```

The default model endpoint is `http://dreamworld:11434` using
`qwen2.5-coder:14b`. These can be overridden locally without committing
configuration:

```bash
export OLLAMA_HOST=http://dreamworld:11434
export OLLAMA_MODEL=qwen2.5-coder:14b
```

Run one task for a smoke test. Then start the single-worker full queue. It is
intentionally single-worker: candidate verification temporarily rebuilds the
shared object tree, so concurrent source probes would not be trustworthy.

```bash
python3 tools/local_campaign.py run --limit 1
nohup python3 tools/local_campaign.py run > build/local_llm_campaign/runner.log 2>&1 &
echo $! > build/local_llm_campaign/runner.pid
```

Stop it with:

```bash
kill "$(cat build/local_llm_campaign/runner.pid)"
```

State, prompts, model replies, candidate files, and logs live beneath ignored
`build/local_llm_campaign/`. They are never staged or included in a PR.

## Dashboard

The dashboard has no mutation API. It shows canonical objdiff measurements,
per-category code maps, campaign coverage, active work, and candidates that
are exact but awaiting strict human review.

```bash
python3 tools/local_campaign_server.py --open
```

It is available at `http://127.0.0.1:8765`.

## Review and promotion

For each `review_exact` record, audit the stored prompt, response, candidate,
source-policy flags, objdiff output, and link-gate output before making any
tracked edit. Apply only reviewed changes in a normal branch, then run the
standard validation sequence:

```bash
python3 configure.py --no-progress
ninja all_source build/GC6E01/report.json
ninja
python3 configure.py progress
```

After a source change is accepted elsewhere, rebuild the report and run
`python3 tools/local_campaign.py sync`. Tasks with changed source are marked
`stale_source` rather than silently being retried against a different baseline.
Use `sync --reset` only for an intentional campaign restart.
