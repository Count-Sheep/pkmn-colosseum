# Local LLM campaign

`tools/local_campaign.py` uses a local Ollama model to create review-only
proposals. It reads the canonical `build/GC6E01/report.json`, accounts for
every function below 100%, asks the model for one replacement function
definition where an editable source owner exists, and verifies an ignored
candidate copy through the existing objdiff-based verifier. Entries with no
declared source are retained as `blocked_no_editable_source`; the campaign must
not invent a split or source owner just to make those entries runnable.

Verification temporarily replaces source under the shared build lock and
restores the original bytes afterward. It reads raw objdiff JSON scores,
rebuilds the baseline object and checks restoration before releasing the lock.
It does not change `configure.py` or attempt automatic object promotion.
No proposal is automatically promoted. An exact result is
still only a review candidate. This retains the strict
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
export LOCAL_CAMPAIGN_WORKER=dreamworld-3080ti
export OLLAMA_HOST=http://dreamworld:11434
export OLLAMA_MODEL=qwen2.5-coder:14b
export OLLAMA_NUM_PREDICT=4096
```

The worker caps each response at 4,096 generated tokens by default so one
weak proposal cannot monopolize the queue. Set `OLLAMA_NUM_PREDICT` or pass
`--num-predict` before the command to tune it for unusually large functions.
It also stops a response when a substantial trailing sequence repeats four
times, records `model_loop`, and continues to the next queue item.

Each task gets at most two proposals per visit. An unchanged, non-exact,
uncompilable or truncated proposal gets one correction with the previous
response and measured/compiler feedback. Truncation doubles the output cap for
that correction, up to 4096 tokens. An identical function is recorded as
`no_change` without compiling. Partial responses (including loops and stream
errors) and an `attempt-NNN.report.json` manifest are retained for each attempt.
These are proposal outcomes, not accepted-source progress.

Source lookup follows chained scoring wrappers and skips disabled `#if 0`
stubs, comments and assembly definitions. Multiple possible C definitions
fail closed. `blocked_source_context` entries remain in the worklist until
their active source can be established; the runner never invents a signature
from an assembly placeholder.

Run one task for a smoke test. Then start a named model worker. Codex can work
alongside it using the source claims and build commands below. Model generation
runs concurrently with manual work and other named model workers; verification
uses an exclusive build lock because it temporarily replaces source and
rebuilds the shared object tree.

```bash
python3 tools/local_campaign.py --worker dreamworld-3080ti run --limit 1
nohup python3 tools/local_campaign.py --worker dreamworld-3080ti run > build/local_llm_campaign/dreamworld-3080ti.log 2>&1 &
echo $! > build/local_llm_campaign/dreamworld-3080ti.pid
```

Stop it with:

```bash
kill "$(cat build/local_llm_campaign/dreamworld-3080ti.pid)"
```

SIGTERM/SIGINT request a graceful stop: generation stops on the next packet,
and an in-progress verification completes restoration before exit. A failed
baseline restoration creates `build/local_llm_campaign/halt.json`; all workers
stop using the build tree. Inspect that report and repair/rebuild the baseline
under the shared build lock before removing the halt file and restarting.

State, prompts, model replies, candidate files, and logs live beneath ignored
`build/local_llm_campaign/`. They are never staged or included in a PR.

## Working alongside the model

Claim a task before editing. A claim reserves its entire owner file, including
other functions in that file and its scoring wrappers. Use a queue ID instead
of a symbol if the symbol is ambiguous. A conflicting claim fails immediately;
the model skips claimed owners and continues through the remaining worklist.

```bash
python3 tools/local_campaign.py claim fn_801093C8 --worker Codex
python3 tools/local_campaign.py claims
```

Keep the returned source path and claim token. Manual claims remain until
explicit release, including across CLI exits. Automatic model claims are
released after each attempt and recovered if the owning process dies.
Each model worker needs a stable `--worker` name. The same worker name is
single-instance locked, while different worker names may run in parallel and
claim different owner sources. A concurrent `sync` is still rejected; stop the
fleet before refreshing the authoritative worklist.

All manual builds, objdiff reads, report refreshes, and verification scripts
must use the shared build lock:

```bash
python3 tools/local_campaign.py build --worker Codex -- ninja -j2 all_source build/GC6E01/report.json
python3 tools/local_campaign.py build --worker Codex -- ninja -j2
```

The command's exit code is preserved. Waiting builds appear in the dashboard.
Source claims and the build lock are advisory: unrelated tools invoking raw
`ninja` or editing unclaimed sources bypass this coordination. This mechanism
coordinates one shared checkout and state directory on macOS/Linux. To move the
campaign to a dedicated box, move or mount the whole repository including
ignored `build/local_llm_campaign/`, keep `tools/local_campaign.py` as the
single queue entrypoint, and start each machine with its own worker name,
Ollama host, model, and output cap:

```bash
OLLAMA_NUM_CTX=8192 python3 tools/local_campaign.py \
  --worker mac-m3-fast \
  --ollama-host http://127.0.0.1:11434 \
  --model qwen2.5-coder:7b \
  --num-predict 1024 \
  run --max-function-bytes 512 --delay 3
```

This Mac fast-pass profile caps target size at 512 retail bytes and pauses
three seconds between attempts. A worker without that ceiling, such as
dreamworld, continues through the entire remaining worklist. `OLLAMA_NUM_CTX`
sets the per-process context allocation (default 32768); the current dreamworld
worker uses 16384. Model inference uses the Mac GPU when the local worker is
running, so the device may warm up even while no compilation is active.

Network workers should reach the same checkout through SSH, a shared volume, or
a job launcher on the host that owns the checkout. Do not let two independent
copies maintain separate queue state unless they are intentionally separate
experiments, because their claims and retained candidates will diverge. Shared
header changes and build-configuration edits require stopping the fleet, since
their effects extend beyond a single source-owner claim.

The model checks the queued source hash again under the build lock immediately
before candidate verification. An intervening source edit marks the attempt
`stale_source` and the candidate is not applied.

## Fleet priority

Queue entries carry a `value_score` and `value_reasons` after `sync`. This is
not an acceptance shortcut; it is only a scheduling hint for the worker fleet.
Priority points = 6 x learning + 2 x ease + scoring-unit completion:

- Learning (0-100): 8 points per distinct unresolved function referencing the
  target (capped at 80), plus 2 per unresolved owner peer (capped at 20).
- Ease (0-100): 0.7 x current fuzzy percentage, plus 30 for targets up to 192
  bytes or 15 for targets up to 512 bytes. This is a heuristic, not a success
  probability or measured time estimate.
- Scoring-unit completion: 100 for the last residual, 50 for 2-4 residuals,
  otherwise zero. This does not imply the object is eligible for linking.

References come from retail-object relocations using the existing ELF reader
and `objdiff.json` target paths. They include address-taking, not just calls.
Indirect calls, ambiguous names, section-symbol targets, and branches without
relocations are omitted. Coverage and missing-object details are saved with
the report hash; zero references does not establish independence. Beneficiary
lists deduplicate reference users and source-owner peers. Shared ownership is
a weaker hint, not proof that functions use the same types or algorithms.

The dashboard exposes these components and expandable beneficiary lists.
Matching a helper may clarify types, ABI and semantics for its users; it does
not automatically improve their emitted bytes. After reviewed source is
accepted and the queue is synced, workers receive up to two related exact
source examples (3,000 source characters total). Examples are hash-checked,
screened against source-policy patterns and drawn from the canonical report,
never from unaccepted review candidates. These are measured examples, not an
automated semantic audit. Compiler-shaping advice from legacy briefs is
removed from campaign prompts.

Workers sort pending tasks by `value_score` before fuzzy percent. The dashboard
shows the top pending high-value targets and the reasons behind each score.
Low-value entries remain in the queue; the fleet will still move through them
when higher-value work is exhausted or claimed by another worker.

## Codex Review Lanes

Completed local-model attempts can be handed to Codex review agents without
stopping the model. Each lane gets its own Git worktree and branch, so source
and build outputs remain isolated from the local-model runner. Use one owner
source per lane; multiple symbols in that owner may share a lane.

Create each worktree from the current committed base, then link the original
system files with the existing helper:

```bash
python3 tools/setup_worker_worktree.py /tmp/pkmn-agent-win-sprite \
  --branch codex/agent-win-sprite --base HEAD
```

Launch the agent with a retained local-model queue item:

```bash
python3 tools/local_campaign_agents.py launch \
  --lane win-sprite \
  --worktree /tmp/pkmn-agent-win-sprite \
  --branch codex/agent-win-sprite \
  --task fn_801093C8
```

The lane appears in the dashboard alongside the local model. Agent work is
review-only until its branch is inspected; the launcher never pushes,
cherry-picks, or merges it. A finished lane remains visible with its worktree,
branch, log, and final-message paths for review.

Record the human or Codex review result so the dashboard distinguishes a strict
survivor from a rejected probe:

```bash
python3 tools/local_campaign_agents.py review --lane LANE --outcome rejected \
  --detail "No strict survivor; retain the measured blocker for later work."
```

After validation, release the manual claim with its exact token:

```bash
python3 tools/local_campaign.py release src/game/win_sprite.c --token TOKEN_FROM_CLAIM
```

Stop the model worker before `sync` to refresh the queue after accepted edits.
An exact function is removed from the refreshed worklist. Other entries with a
changed owner remain `stale_source` for explicit review; their old attempts are
retained. The dashboard shows both workers' source claims and the build queue.

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
