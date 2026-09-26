const $ = (selector) => document.querySelector(selector);
const nf = new Intl.NumberFormat();

function percent(value) { return `${Number(value || 0).toFixed(2)}%`; }
function statusLabel(value) { return String(value || "pending").replaceAll("_", " "); }
function mapColor(entry) {
  if (entry.complete) return "exact";
  if (entry.fuzzy >= 95) return "near";
  if (entry.fuzzy >= 75) return "warm";
  if (entry.fuzzy >= 35) return "cool";
  return "open";
}

function elapsed(timestamp) {
  if (!timestamp) return "just started";
  const seconds = Math.max(0, Math.floor((Date.now() - new Date(timestamp).getTime()) / 1000));
  if (seconds < 60) return `${seconds}s elapsed`;
  return `${Math.floor(seconds / 60)}m ${seconds % 60}s elapsed`;
}

function renderLive(data) {
  const root = $("#live-work"); root.replaceChildren();
  const active = data.queue.active || [];
  $("#live-updated").textContent = `Dashboard refreshes every 2 seconds · ${new Date(data.generated_at).toLocaleTimeString()}`;
  if (!active.length) { root.innerHTML = '<p class="empty">No model request is active. A runner will claim the next pending task when available.</p>'; return; }
  for (const item of active) {
    const card = document.createElement("article"); card.className = "live-card";
    const activity = item.activity || {};
    const summary = document.createElement("div"); summary.className = "live-summary";
    const symbol = document.createElement("strong"); symbol.textContent = item.symbol;
    const badge = bootBadge(item); if (badge) symbol.append(badge);
    const phase = document.createElement("span"); phase.className = "phase"; phase.textContent = activity.phase || "Starting task";
    const detail = document.createElement("p"); detail.textContent = activity.detail || "Runner state has not reported a detailed phase yet.";
    summary.append(symbol, phase, detail); card.append(summary);
    const facts = document.createElement("dl"); facts.className = "live-facts";
    const values = [
      ["Worker", item.worker || "Local LLM"],
      ["Attempt", String(item.attempts || 1)], ["Elapsed", elapsed(activity.started_at || item.last_attempt_at)],
      ["Last heartbeat", activity.updated_at ? new Date(activity.updated_at).toLocaleTimeString() : "awaiting update"],
      ["Base match", percent(item.base_pct)], ["Function size", `${nf.format(item.size || 0)} bytes`],
      ["Owner source", item.owner_source || item.source], ["Scoring unit", item.unit],
    ];
    if (activity.prompt_chars !== undefined) values.push(["Prompt", `${nf.format(activity.prompt_chars)} chars`]);
    if (activity.num_predict !== undefined) values.push(["Output cap", `${nf.format(activity.num_predict)} tokens`]);
    if (activity.response_chars !== undefined) values.push(["Model response", `${nf.format(activity.response_chars)} chars across ${nf.format(activity.response_chunks || 0)} chunks`]);
    values.forEach(([key, value]) => { const dt = document.createElement("dt"); dt.textContent = key; const dd = document.createElement("dd"); dd.textContent = value; facts.append(dt, dd); });
    card.append(facts);
    if (activity.response_preview) { const preview = document.createElement("pre"); preview.className = "response-preview"; preview.textContent = activity.response_preview; card.append(preview); }
    root.append(card);
  }
}

const RECOMP_LABELS = {
  "decomp-blocked": "decomp blocked", "upstream-verification": "needs strict verification",
  "port-work": "ready to port", "port-ready": "port ready",
};
function el(tag, className, text) {
  const node = document.createElement(tag);
  if (className) node.className = className;
  if (text !== undefined) node.textContent = text;
  return node;
}
function bootBadge(item) {
  if (!item.boot_blocker) return null;
  const badge = el("span", `boot-badge${item.boot_blocker.current ? " current" : ""}`, `Boot blocker #${item.boot_blocker.index}`);
  badge.title = item.boot_blocker.title; return badge;
}
function nextAction(blocker) {
  const row = blocker.next;
  if (blocker.status === "port-work") return `Decomp side accepted — native port next. ${blocker.native_note || ""}`.trim();
  if (!row) return blocker.native_note || "No decomp functions listed; native work only.";
  const where = row.source ? ` in ${row.source}` : "";
  if (row.status === "needs-verification") return `${row.symbol} is 100%${where} but ${row.issues.join("; ")}.`;
  if (row.status === "missing") return `${row.symbol} is not in the report.`;
  return `${row.symbol} is ${percent(row.fuzzy)} (${nf.format(row.size)} bytes)${where}.`;
}

function renderRecomp(data) {
  const recomp = data.recomp || {}, current = $("#recomp-current");
  const openRows = new Set([...document.querySelectorAll("#recomp-blockers details[open]")].map((node) => node.dataset.id));
  if (!recomp.available) {
    current.className = "recomp-current"; current.replaceChildren(el("p", "", recomp.error || "Recomp status unavailable."));
    return;
  }
  const blocker = recomp.current, summary = recomp.summary;
  $("#recomp-updated").textContent = `recomp ${recomp.recomp_commit || "?"} · decomp ${recomp.decomp_commit || "?"} · report ${new Date(recomp.report_updated_at).toLocaleTimeString()}`;
  current.replaceChildren();
  if (!blocker) {
    current.className = "recomp-current port-ready";
    current.append(el("strong", "", "No boot blockers remain"), el("p", "", "Every inventory row is port-ready."));
  } else {
    current.className = `recomp-current ${blocker.status}`;
    const title = el("strong", "", `#${blocker.index} ${blocker.title}`);
    const line = el("p"); line.append(el("span", `chip ${blocker.status}`, RECOMP_LABELS[blocker.status]), ` ${blocker.area} · `, el("code", "", blocker.symbol));
    current.append(el("p", "eyebrow", "Current boot blocker"), title, line, el("p", "next", nextAction(blocker)), el("p", "", recomp.runtime_stop));
  }
  const counts = $("#recomp-counts"); counts.replaceChildren();
  counts.append(el("span", "", `${nf.format(summary.accepted_functions)} / ${nf.format(summary.functions)} boot-critical functions strictly accepted · ${nf.format(summary.exact_functions)} at 100%`));
  for (const [status, count] of Object.entries(summary.by_status)) { const item = el("span"); item.append(el("span", `chip ${status}`, `${count}`), ` ${RECOMP_LABELS[status]}`); counts.append(item); }
  const loop = data.boot_loop || {};
  if (loop.updated_at) {
    const age = (Date.now() - new Date(loop.updated_at).getTime()) / 1000;
    const live = loop.running && age < 3 * (loop.settings?.interval || 60);
    const text = live
      ? `Auto-loop live · focus blockers ${(loop.focus?.blockers || []).map((index) => `#${index}`).join(", ")} · ${loop.focus?.targets || 0} targets · ${Object.values(loop.attempts || {}).filter((row) => row.lane_status === "running").length} boot lane running · checked ${new Date(loop.updated_at).toLocaleTimeString()}`
      : `Auto-loop stopped · last check ${new Date(loop.updated_at).toLocaleString()}`;
    const node = el("span", "", text); if (loop.last_error) node.title = loop.last_error; counts.append(node);
    if (live && loop.agents_paused_until && new Date(loop.agents_paused_until) > new Date())
      counts.append(el("span", "chip", `Codex boot lanes paused until ${new Date(loop.agents_paused_until).toLocaleString()} (${loop.agents_paused_reason || "paused"}); Ollama workers still follow the focus`));
    const waiting = Object.entries(loop.attempts || {}).filter(([, row]) => row.lane_status === "review_ready" || row.lane_status === "finished");
    if (waiting.length) counts.append(el("span", "chip upstream-verification", `${waiting.length} boot lane${waiting.length === 1 ? "" : "s"} awaiting human review: ${waiting.map(([symbol, row]) => `${symbol} (${row.branch})`).join(", ")}`));
  }
  const drift = recomp.blockers.filter((row) => row.cpp_drift).length;
  if (drift) counts.append(el("span", "", `${drift} rows ahead of the recomp's boot_readiness.cpp table`));
  if (!summary.cpp_table_aligned) counts.append(el("span", "chip", "manifest order differs from boot_readiness.cpp"));

  const list = $("#recomp-blockers"); list.replaceChildren();
  for (const row of recomp.blockers) {
    const details = el("details", `recomp-row${blocker && row.index === blocker.index ? " current" : ""}`); details.dataset.id = row.id; details.open = openRows.has(row.id);
    const head = el("summary");
    const bar = el("div", "bar"); const fill = el("i", row.match_percent >= 100 ? "full" : ""); fill.style.width = `${row.match_percent ?? 0}%`; bar.append(fill);
    bar.title = row.match_percent === null ? "No decomp functions" : `${percent(row.match_percent)} size-weighted match`;
    head.append(el("span", "idx", row.index), el("span", "name", row.title), el("span", `chip ${row.status}`, RECOMP_LABELS[row.status]), bar,
      el("span", "count", row.functions.length ? `${row.accepted}/${row.functions.length}` : "—"));
    details.append(head);
    const fns = el("div", "recomp-fns");
    for (const fn of row.functions) {
      const line = el("div", `recomp-fn ${fn.status}`); line.append(el("code", "", fn.symbol), el("span", "", percent(fn.fuzzy)));
      line.title = [fn.unit, fn.source, `${nf.format(fn.size || 0)} bytes`, fn.linked ? "linked" : "not linked"].filter(Boolean).join("\n");
      if (fn.issues?.length) line.append(el("em", "", fn.issues.join("; ")));
      fns.append(line);
    }
    details.append(fns, el("p", "recomp-note", `Next: ${nextAction(row)}`));
    if (row.native_note) details.append(el("p", "recomp-note", `Native (${row.native}): ${row.native_note}`));
    if (row.cpp_drift) details.append(el("p", "recomp-note", `boot_readiness.cpp still says ${row.cpp_class}.`));
    list.append(details);
  }

  const queue = $("#recomp-queue"); queue.replaceChildren();
  const tasks = data.queue.boot_critical || [];
  if (!tasks.length) queue.append(el("p", "empty", "No queued campaign task covers an unaccepted boot-critical function."));
  for (const item of tasks) {
    const row = el("article", "recomp-task"); const title = el("p", "", `${item.symbol} · ${percent(item.base_pct)} · ${nf.format(item.size || 0)} bytes`);
    title.append(bootBadge(item));
    row.append(title, el("span", "", `${statusLabel(item.status)}${item.worker ? ` · ${item.worker}` : ""} · ${item.boot_blocker.title}`), el("code", "", item.owner_source || item.source));
    queue.append(row);
  }
}

function renderMetrics(data) {
  const measures = data.report_measures || {};
  const entries = [
    ["Fuzzy match", percent(measures.fuzzy_match_percent), "canonical objdiff report"],
    ["Exact functions", `${nf.format(measures.matched_functions || 0)} / ${nf.format(measures.total_functions || 0)}`, "functions at 100%"],
    ["Matched code", `${nf.format(measures.matched_code || 0)} / ${nf.format(measures.total_code || 0)} bytes`, percent(measures.matched_code_percent)],
    ["Linked units", `${nf.format(measures.complete_units || 0)} / ${nf.format(measures.total_units || 0)}`, percent(measures.complete_code_percent)],
  ];
  const root = $("#metrics"); root.replaceChildren(); const template = $("#metric-template");
  for (const [label, value, detail] of entries) {
    const node = template.content.cloneNode(true);
    node.querySelector("p").textContent = label;
    node.querySelector("strong").textContent = value;
    node.querySelector("span").textContent = detail;
    root.append(node);
  }
}

function renderWorkers(data) {
  const root = $("#workers"); root.replaceChildren();
  const coordination = data.coordination || {};
  for (const worker of Object.values(data.workers || {}).sort((a, b) => a.worker.localeCompare(b.worker))) {
    const row = document.createElement("article"); row.className = "worker-row fleet-row";
    const name = document.createElement("strong"); name.textContent = worker.worker;
    const model = document.createElement("span"); model.textContent = worker.ollama_model;
    const host = document.createElement("code"); host.textContent = worker.ollama_host;
    const detail = document.createElement("p"); detail.textContent = `${nf.format(worker.ollama_num_predict || 0)} token cap · pid ${worker.pid || "unknown"} · ${worker.updated_at ? new Date(worker.updated_at).toLocaleTimeString() : "no heartbeat"}`;
    const outcomes = data.worker_outcomes?.[worker.worker] || {};
    const progress = document.createElement("p"); progress.textContent = `${outcomes.attempted || 0} functions attempted · ${outcomes.improved || 0} latest candidates improved · ${Object.entries(outcomes.status || {}).map(([status, count]) => `${count} ${statusLabel(status)}`).join(" · ")}`;
    row.append(name, model, host, detail, progress); root.append(row);
  }
  for (const claim of Object.values(coordination.claims || {})) {
    const row = document.createElement("article"); row.className = "worker-row";
    const name = document.createElement("strong"); name.textContent = claim.worker;
    const symbol = document.createElement("span"); symbol.textContent = claim.symbol;
    const source = document.createElement("code"); source.textContent = claim.source;
    const detail = document.createElement("p"); detail.textContent = claim.detail;
    row.append(name, symbol, source, detail); root.append(row);
  }
  for (const lane of Object.values(coordination.agents || {})) {
    const row = document.createElement("article"); row.className = `worker-row agent-row ${lane.status || "queued"}`;
    const name = document.createElement("strong"); name.textContent = lane.worker || "Codex agent";
    const symbol = document.createElement("span"); symbol.textContent = lane.symbols?.join(", ") || lane.id;
    const source = document.createElement("code"); source.textContent = lane.source || lane.worktree;
    const detail = document.createElement("p"); detail.textContent = `${statusLabel(lane.status)} · ${lane.live_detail || lane.detail || lane.branch || "awaiting task"}`;
    row.append(name, symbol, source, detail); root.append(row);
  }
  for (const job of Object.values(coordination.builds || {})) {
    const row = document.createElement("p"); row.className = "build-row";
    row.textContent = `${job.worker} · ${job.status === "building" ? "Build lock held" : "Waiting for build lock"} · ${job.detail}`;
    root.append(row);
  }
}

function renderQueue(data) {
  const status = data.queue.status || {};
  const counts = Object.entries(status).sort().map(([key, value]) => `${nf.format(value)} ${statusLabel(key)}`);
  $("#queue-total").textContent = `${nf.format(data.queue.total || 0)} unresolved functions · ${counts.join(" · ")}`;
  $("#legend").innerHTML = [["exact", "linked / exact unit"], ["near", "95%+ fuzzy"], ["warm", "75-95%"], ["cool", "35-75%"], ["open", "below 35%"]]
    .map(([kind, label]) => `<span><i class="tile ${kind}"></i>${label}</span>`).join("");
}

function renderMap(data) {
  const names = Object.fromEntries((data.categories || []).map((item) => [item.id, item.name]));
  const root = $("#maps"); root.replaceChildren();
  for (const [category, entries] of Object.entries(data.maps || {})) {
    const section = document.createElement("section"); section.className = "map-section";
    const title = document.createElement("h3"); title.textContent = names[category] || category; section.append(title);
    const grid = document.createElement("div"); grid.className = "unit-map";
    entries.forEach((entry) => {
      const tile = document.createElement("div"); tile.className = `tile ${mapColor(entry)}`;
      tile.title = `${entry.name}\n${entry.source}\n${percent(entry.fuzzy)} fuzzy · ${entry.matched}/${entry.total} exact functions · ${nf.format(entry.code)} bytes`;
      grid.append(tile);
    });
    section.append(grid); root.append(section);
  }
}

function renderHighValue(data) {
  const root = $("#high-value");
  const expanded = new Set([...root.querySelectorAll("details[open]")].map((entry) => entry.dataset.task));
  root.replaceChildren();
  const items = data.queue.high_value || [];
  if (!items.length) { root.innerHTML = '<p class="empty">Priority scores will appear after the next queue sync.</p>'; return; }
  for (const item of items.slice(0, 12)) {
    const row = document.createElement("article"); row.className = "priority-item";
    const score = document.createElement("strong"); score.textContent = nf.format(item.value_score || 0);
    score.title = "Priority points: 6 x learning + 2 x ease + scoring-unit completion";
    const body = document.createElement("div");
    const title = document.createElement("p"); title.textContent = `${item.symbol} · ${percent(item.base_pct)} · ${nf.format(item.size || 0)} bytes`;
    const badge = bootBadge(item); if (badge) title.append(badge);
    const source = document.createElement("code"); source.textContent = item.owner_source || item.source;
    const reasons = document.createElement("span"); reasons.textContent = (item.value_reasons || []).join(" · ") || "pending scoring detail";
    const metrics = document.createElement("span");
    metrics.textContent = `Learning ${item.learning_score || 0}/100 · Ease ${item.ease_score || 0}/100 · Unit completion ${item.closure_score || 0}/100`;
    const beneficiaries = document.createElement("details");
    beneficiaries.dataset.task = item.id; beneficiaries.open = expanded.has(item.id);
    const label = document.createElement("summary"); label.textContent = `${item.potential_beneficiaries || 0} potential beneficiaries`;
    const list = document.createElement("p"); list.textContent = (item.beneficiaries || []).map((entry) => `${entry.symbol} (${entry.unit})`).join("; ") || "None identified";
    beneficiaries.append(label, list);
    body.append(title, source, metrics, reasons, beneficiaries); row.append(score, body); root.append(row);
  }
  const analysis = data.priority_analysis || {};
  $("#priority-coverage").textContent = `Retail reference coverage: ${nf.format(analysis.covered_functions || 0)} / ${nf.format(analysis.total_functions || 0)} functions`;
}

function renderReview(data) {
  const root = $("#review"); root.replaceChildren(); const items = data.queue.review || [];
  if (!items.length) { root.innerHTML = '<p class="empty">No exact candidates are waiting yet.</p>'; return; }
  items.forEach((item) => {
    const report = item.last_report || {}; const row = document.createElement("article"); row.className = "review-item";
    const link = report.link_gate?.linked ? "linked retail SHA passes" : "text exact; object/link review required";
    row.innerHTML = `<strong></strong><p></p><p></p>`;
    row.querySelector("strong").textContent = item.symbol;
    row.querySelectorAll("p")[0].textContent = `${item.source} · ${item.unit}`;
    row.querySelectorAll("p")[1].textContent = `${link} · ${item.candidate || "candidate retained"}`;
    if ((report.review_flags || []).length) { const flag = document.createElement("p"); flag.className = "flag"; flag.textContent = report.review_flags.join("; "); row.append(flag); }
    root.append(row);
  });
}

function renderEvents(data) {
  const root = $("#events"); root.replaceChildren();
  (data.events || []).slice(0, 18).forEach((event) => {
    const row = document.createElement("p");
    row.innerHTML = `<time>${new Date(event.at).toLocaleTimeString()}</time> <strong>${statusLabel(event.kind)}</strong>${event.symbol ? ` · ${event.symbol}` : ""}${event.pct !== undefined ? ` · ${percent(event.pct)}` : ""}`;
    root.append(row);
  });
  if (!root.children.length) root.innerHTML = '<p class="empty">The runner has not started.</p>';
}

function drawHistory(data) {
  const canvas = $("#history"), context = canvas.getContext("2d"), points = data.snapshots || [];
  const width = canvas.clientWidth, height = canvas.clientHeight, ratio = window.devicePixelRatio || 1;
  canvas.width = width * ratio; canvas.height = height * ratio; context.scale(ratio, ratio); context.clearRect(0, 0, width, height);
  context.strokeStyle = "#c9d1d9"; context.lineWidth = 1;
  [0.2, 0.5, 0.8].forEach((fraction) => { context.beginPath(); context.moveTo(0, height * fraction); context.lineTo(width, height * fraction); context.stroke(); });
  const maximum = Math.max(...points.map((point) => point.worklist || 1), 1);
  const line = (field, color) => { context.strokeStyle = color; context.lineWidth = 2; context.beginPath(); points.forEach((point, index) => { const x = points.length === 1 ? width / 2 : (index / (points.length - 1)) * width; const y = height - ((point[field] || 0) / maximum) * (height - 12) - 6; index ? context.lineTo(x, y) : context.moveTo(x, y); }); context.stroke(); };
  line("attempted", "#2f81f7"); line("review_exact", "#3fb950");
}

async function refresh() {
  try {
    const data = await fetch("/api/dashboard", {cache: "no-store"}).then((response) => response.json());
    const workerCount = Object.keys(data.workers || {}).length;
    $("#model").textContent = workerCount ? `${nf.format(workerCount)} model worker${workerCount === 1 ? "" : "s"} registered` : `${data.settings.ollama_model || "model"} via ${data.settings.ollama_host || "local"}`;
    renderRecomp(data); renderLive(data); renderWorkers(data); renderMetrics(data); renderQueue(data); renderHighValue(data); renderMap(data); renderReview(data); renderEvents(data); drawHistory(data);
  } catch (error) { $("#model").textContent = `Dashboard unavailable: ${error.message}`; }
}
window.addEventListener("resize", refresh); refresh(); setInterval(refresh, 2000);
