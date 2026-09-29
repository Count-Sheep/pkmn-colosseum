const $ = (selector) => document.querySelector(selector);
const nf = new Intl.NumberFormat();

function percent(value) { return `${Number(value || 0).toFixed(2)}%`; }
function statusLabel(value) { return String(value || "pending").replaceAll("_", " "); }
function timestampLabel(value) { return value ? new Date(value).toLocaleString() : "not available"; }
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
    if (activity.thinking_chars) values.push(["Model reasoning", `${nf.format(activity.thinking_chars)} chars`]);
    if (activity.response_chars !== undefined) values.push(["Model response", `${nf.format(activity.response_chars)} chars across ${nf.format(activity.response_chunks || 0)} chunks`]);
    values.forEach(([key, value]) => { const dt = document.createElement("dt"); dt.textContent = key; const dd = document.createElement("dd"); dd.textContent = value; facts.append(dt, dd); });
    card.append(facts);
    if (activity.response_preview) { const preview = document.createElement("pre"); preview.className = "response-preview"; preview.textContent = activity.response_preview; card.append(preview); }
    root.append(card);
  }
}

const RECOMP_LABELS = {
  "decomp-blocked": "decomp blocked", "upstream-verification": "needs strict verification",
  "port-work": "closure/native work", "port-ready": "port ready",
};
function el(tag, className, text) {
  const node = document.createElement(tag);
  if (className) node.className = className;
  if (text !== undefined) node.textContent = text;
  return node;
}
function renderFreshness(data) {
  const fresh = data.freshness || {}, decomp = fresh.decomp || {}, recomp = fresh.recomp || {};
  const retail = decomp.retail_hashes;
  $("#freshness-polled").textContent = `Dashboard checked ${timestampLabel(data.generated_at)} · auto-refresh every 2 seconds`;
  const warning = $("#freshness-warning");
  warning.hidden = !decomp.report_behind_source && !recomp.app_behind_source && retail?.status !== "mismatch";
  warning.textContent = [
    decomp.report_behind_source ? "Decomp source is newer than the canonical match report; percentages and acceptance remain at the last report build." : "",
    recomp.app_behind_source ? "Recomp source is newer than the application binary; native changes have not reached that executable." : "",
    retail?.status === "mismatch" ? "The built DOL or REL does not match the retail SHA-1; linked match claims are not build-validated yet." : "",
  ].filter(Boolean).join(" ");
  const grid = $("#freshness-grid"); grid.replaceChildren();
  const rows = [
    [`Latest observed work${fresh.latest_work?.project ? ` (${fresh.latest_work.project})` : ""}`, fresh.latest_work],
    ["Decomp source edit", decomp.source], ["Decomp match report", decomp.report], ["Decomp build artifact", decomp.build],
    ["Retail DOL + REL hashes", retail ? {at: decomp.build?.at, path: retail.files.map((row) => `${row.path.split("/").at(-1)}: ${row.status}`).join(" · ")} : null],
    ["Recomp source edit", recomp.source], ["Recomp build artifact", recomp.build], ["Recomp test run", recomp.test],
    ["Latest live boot probe", recomp.boot_probe],
  ];
  for (const [label, record] of rows) {
    const card = el("div", "freshness-item");
    card.append(el("span", "", label), el("time", "", timestampLabel(record?.at)), el("code", "", record?.path || "No output yet"));
    if (record?.at) card.querySelector("time").dateTime = record.at;
    grid.append(card);
  }
}
function bootBadge(item) {
  if (!item.boot_blocker) return null;
  const badge = el("span", `boot-badge${item.boot_blocker.current ? " current" : ""}`, `Boot blocker #${item.boot_blocker.index}`);
  badge.title = item.boot_blocker.title; return badge;
}
function nextAction(blocker) {
  const row = blocker.next;
  if (row) {
    const where = row.source ? ` in ${row.source}` : "";
    if (row.status === "needs-verification") return `Entry ${row.symbol} is 100%${where} but ${row.issues.join("; ")}.`;
    if (row.status === "missing") return `Entry ${row.symbol} is not in the report.`;
    return `Entry ${row.symbol} is ${percent(row.fuzzy)} (${nf.format(row.size)} bytes)${where}.`;
  }
  const deps = blocker.dependencies || {};
  if (deps.blocking_count) {
    const first = deps.blocking[0];
    const reason = first.issues?.length ? ` — ${first.issues.join("; ")}` : "";
    return `${nf.format(deps.blocking_count)} unaccepted callees in the decomp closure; first: ${first.symbol} (${percent(first.fuzzy)})${reason}.`;
  }
  if (blocker.bindable) return "Accepted closure and dependent rows are ready; native binding is next.";
  if (blocker.depends_on_rows?.length) return `Accepted closure; waiting on dependent rows before binding. ${blocker.native_note || ""}`.trim();
  return blocker.native_note || "Accepted closure; native implementation is next.";
}

function renderRecomp(data) {
  const recomp = data.recomp || {}, current = $("#recomp-current");
  const openRows = new Set([...document.querySelectorAll("#recomp-blockers details[open]")].map((node) => node.dataset.id));
  if (!recomp.available) {
    current.className = "recomp-current"; current.replaceChildren(el("p", "", recomp.error || "Recomp status unavailable."));
    $("#recomp-updated").textContent = "";
    $("#recomp-counts").replaceChildren();
    $("#recomp-blockers").replaceChildren();
    $("#recomp-queue").replaceChildren();
    return;
  }
  const blocker = recomp.current, summary = recomp.summary;
  $("#recomp-updated").textContent = `recomp ${recomp.recomp_commit || "?"} · decomp ${recomp.decomp_commit || "?"} · report ${new Date(recomp.report_updated_at).toLocaleTimeString()}`;
  current.replaceChildren();
  if (!blocker) {
    current.className = "recomp-current port-ready";
    current.append(el("strong", "", "No title-path inventory blockers remain"), el("p", "", "Every inventory row is port-ready; runtime validation is still required."));
  } else {
    current.className = `recomp-current ${blocker.status}`;
    const title = el("strong", "", `#${blocker.index} ${blocker.title}`);
    const line = el("p"); line.append(el("span", `chip ${blocker.status}`, RECOMP_LABELS[blocker.status]), ` ${blocker.area} · `, el("code", "", blocker.symbol));
    current.append(el("p", "eyebrow", "Earliest incomplete inventory row"), title, line,
      el("p", "next", nextAction(blocker)), el("p", "", recomp.runtime_stop));
  }
  const counts = $("#recomp-counts"); counts.replaceChildren();
  const readyRows = Number(summary.by_status?.["port-ready"] || 0);
  const totalRows = Object.values(summary.by_status || {}).reduce((sum, count) => sum + Number(count || 0), 0);
  if (totalRows) {
    const readiness = el("span", "", `${(100 * readyRows / totalRows).toFixed(1)}% title-milestone inventory ready · ${readyRows}/${totalRows} rows`);
    readiness.title = "An unweighted checklist measure, not a runtime-completion percentage or time estimate; remaining rows vary greatly in effort.";
    counts.append(readiness);
  }
  counts.append(el("span", "", `${nf.format(summary.accepted_functions)} / ${nf.format(summary.functions)} title-path entry functions strictly accepted · ${nf.format(summary.exact_functions)} at 100%`));
  if (recomp.native_ports) {
    const ports = recomp.native_ports;
    counts.append(el("span", "", `${nf.format(ports.done)} native recomp ports · ${nf.format(ports.skipped)} deferred · ledger ${new Date(ports.updated_at).toLocaleString()}`));
  }
  for (const [status, count] of Object.entries(summary.by_status)) { const item = el("span"); item.append(el("span", `chip ${status}`, `${count}`), ` ${RECOMP_LABELS[status]}`); counts.append(item); }
  const loop = data.boot_loop || {};
  if (loop.updated_at) {
    const age = (Date.now() - new Date(loop.updated_at).getTime()) / 1000;
    const live = loop.running && age < 3 * (loop.settings?.interval || 60);
    const text = live
      ? `Auto-loop live · focus blockers ${(loop.focus?.blockers || []).map((index) => `#${index}`).join(", ")} · ${loop.focus?.targets || 0} targets · ${Object.values(loop.attempts || {}).filter((row) => row.lane_status === "running").length} boot lane running · checked ${new Date(loop.updated_at).toLocaleTimeString()}`
      : `Auto-loop stopped · last check ${new Date(loop.updated_at).toLocaleString()}`;
    if (live) { const node = el("span", "", text); if (loop.last_error) node.title = loop.last_error; counts.append(node); }
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
      const line = el("div", `recomp-fn ${fn.status}`);
      const match = fn.fuzzy >= 100 ? "100% exact" : percent(fn.fuzzy);
      line.append(el("code", "", fn.symbol), el("span", "", `${match} · ${fn.linked ? "linked" : "not linked"} · ${statusLabel(fn.status)}`));
      line.title = [fn.unit, fn.source, `${nf.format(fn.size || 0)} bytes`, fn.linked ? "linked" : "not linked"].filter(Boolean).join("\n");
      if (fn.issues?.length) line.append(el("em", "", fn.issues.join("; ")));
      fns.append(line);
    }
    details.append(fns, el("p", "recomp-note", `Next: ${nextAction(row)}`));
    const deps = row.dependencies || {};
    if (deps.blocking_count) {
      const closure = el("div", "recomp-closure");
      closure.append(el("strong", "", `${nf.format(deps.blocking_count)} unaccepted callees in closure`));
      for (const fn of deps.blocking.slice(0, 8)) {
        const line = el("div", "recomp-closure-fn");
        line.append(el("code", "", fn.symbol), el("span", "", `${percent(fn.fuzzy)} · ${statusLabel(fn.status)}`));
        if (fn.issues?.length) line.append(el("small", "", fn.issues.join("; ")));
        closure.append(line);
      }
      if (deps.blocking_count > 8) closure.append(el("p", "", `+ ${nf.format(deps.blocking_count - 8)} more in the live evaluator`));
      details.append(closure);
    } else if (row.closure_accepted) {
      details.append(el("p", "recomp-note", row.bindable ? "Accepted closure; bindable now." : "Accepted closure; waiting on dependent rows or native work."));
    }
    if (row.native_note) details.append(el("p", "recomp-note", `Native (${row.native}): ${row.native_note}`));
    if (row.cpp_drift) details.append(el("p", "recomp-note", `boot_readiness.cpp still says ${row.cpp_class}.`));
    list.append(details);
  }

  const queue = $("#recomp-queue"); queue.replaceChildren();
  const tasks = data.queue.boot_critical || [];
  if (!tasks.length) queue.append(el("p", "empty", "No queued campaign task matches an unaccepted entry function or callee."));
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
    const tuning = [
      `${nf.format(worker.ollama_num_predict || 0)} token cap`,
      worker.ollama_num_ctx ? `${nf.format(worker.ollama_num_ctx)} ctx` : "",
      worker.think && worker.think !== "auto" ? `reasoning ${worker.think}` : "",
      worker.retries !== undefined ? `${worker.retries} correction round${worker.retries === 1 ? "" : "s"}` : "",
      worker.timeout ? `${Math.round(worker.timeout / 60)} min timeout` : "",
      worker.max_function_bytes ? `≤${nf.format(worker.max_function_bytes)} B functions` : "",
      worker.min_pct ? `≥${worker.min_pct}% matched` : "",
      worker.recycle ? "recycles the corpus" : "",
    ].filter(Boolean).join(" · ");
    const detail = document.createElement("p"); detail.textContent = `${tuning} · pid ${worker.pid || "unknown"} · last task start ${worker.updated_at ? new Date(worker.updated_at).toLocaleTimeString() : "none"}`;
    const outcomes = data.worker_outcomes?.[worker.worker] || {};
    const progress = document.createElement("p"); progress.textContent = `${outcomes.attempted || 0} functions attempted · ${outcomes.improved || 0} latest candidates improved · ${Object.entries(outcomes.status || {}).map(([status, count]) => `${count} ${statusLabel(status)}`).join(" · ")}`;
    row.append(name, model, host, detail, progress); root.append(row);
  }
  // One row per claim holder: long lane claims would otherwise push the rest of the page far down.
  const holders = new Map();
  for (const claim of Object.values(coordination.claims || {})) {
    if (!holders.has(claim.worker)) holders.set(claim.worker, []);
    holders.get(claim.worker).push(claim);
  }
  for (const [holder, claims] of holders) {
    const row = document.createElement("article"); row.className = "worker-row";
    const name = document.createElement("strong"); name.textContent = holder;
    const symbol = document.createElement("span"); symbol.textContent = `${claims.length} file${claims.length === 1 ? "" : "s"} claimed`;
    const source = document.createElement("code"); source.textContent = claims.map((claim) => claim.source.replace(/^src\//, "")).join(" · ");
    const detail = document.createElement("p"); detail.textContent = claims[0].detail;
    row.append(name, symbol, source, detail); root.append(row);
  }
  const laneRow = (lane) => {
    const row = document.createElement("article"); row.className = `worker-row agent-row ${lane.status || "queued"}`;
    const name = document.createElement("strong"); name.textContent = lane.worker || "Codex agent";
    const symbol = document.createElement("span"); symbol.textContent = lane.symbols?.join(", ") || lane.id;
    const source = document.createElement("code"); source.textContent = lane.source || lane.worktree;
    const detail = document.createElement("p"); detail.textContent = `${statusLabel(lane.status)} · ${lane.live_detail || lane.detail || lane.branch || "awaiting task"}`;
    row.append(name, symbol, source, detail); return row;
  };
  const lanes = Object.values(coordination.agents || {});
  const done = new Set(["accepted", "merged", "paused", "stopped", "finished", "rejected", "failed"]);
  lanes.filter((lane) => !done.has(lane.status)).forEach((lane) => root.append(laneRow(lane)));
  const finished = lanes.filter((lane) => done.has(lane.status)).sort((a, b) => String(b.updated_at || "").localeCompare(String(a.updated_at || "")));
  if (finished.length) {
    const wasOpen = root.dataset.finishedOpen === "1";
    const box = document.createElement("details"); box.className = "finished-lanes"; box.open = wasOpen;
    box.addEventListener("toggle", () => { root.dataset.finishedOpen = box.open ? "1" : "0"; });
    const summary = document.createElement("summary");
    summary.textContent = `${finished.length} finished lane${finished.length === 1 ? "" : "s"} (${finished.filter((l) => l.status === "accepted" || l.status === "merged").length} merged)`;
    box.append(summary); finished.forEach((lane) => box.append(laneRow(lane))); root.append(box);
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

/* ---- Code map: a canvas treemap in the style of decomp.dev's project pages ---- */
const treemap = { category: "all", units: [], layoutKey: "", hover: null, dirty: true, filter: "", ready: false, cache: null };
const SIZE_UNITS = ["B", "kB", "MB", "GB"];
function formatBytes(value) { let unit = 0; while (value >= 1000 && unit < SIZE_UNITS.length - 1) { value /= 1000; unit += 1; } return `${value.toFixed(2)} ${SIZE_UNITS[unit]}`; }
function treemapPercent(value) { const v = value !== 0 && value !== 100 ? Math.min(Math.max(value, 0.01), 99.99) : value; return `${v.toFixed(2)}%`; }

function squarify(items, x, y, w, h) {
  // Squarified treemap (Bruls, Huizing, van Wijk). `items` are sorted descending and carry `area`.
  const out = []; let rest = items.slice();
  while (rest.length) {
    const short = Math.min(w, h), row = [rest[0]]; let i = 1;
    const worst = (r) => { const s = r.reduce((a, b) => a + b.area, 0), mx = Math.max(...r.map((q) => q.area)), mn = Math.min(...r.map((q) => q.area)); return Math.max((short * short * mx) / (s * s), (s * s) / (short * short * mn)); };
    while (i < rest.length && worst([...row, rest[i]]) <= worst(row)) { row.push(rest[i]); i += 1; }
    const sum = row.reduce((a, b) => a + b.area, 0);
    if (w >= h) {
      const cw = sum / h; let cy = y;
      for (const r of row) { const ch = r.area / cw; out.push({ ...r, x, y: cy, w: cw, h: ch }); cy += ch; }
      x += cw; w -= cw;
    } else {
      const rh = sum / w; let cx = x;
      for (const r of row) { const rw = r.area / rh; out.push({ ...r, x: cx, y, w: rw, h: rh }); cx += rw; }
      y += rh; h -= rh;
    }
    rest = rest.slice(i);
  }
  return out;
}

function treemapFilterMatch(term, unit) {
  if (term === "is:linked") return unit.complete;
  if (term === "is:unlinked") return !unit.complete;
  const m = term.match(new RegExp(`^(>=|<=|!=|==|=|>|<)(\\d+(?:\\.\\d+)?)(%|${SIZE_UNITS.join("|")})$`, "i"));
  if (m) {
    let value, limit = Number.parseFloat(m[2]);
    if (m[3] === "%") value = unit.fuzzy;
    else { value = unit.code; let k = 0; while (k < SIZE_UNITS.length - 1 && m[3].toLowerCase() !== SIZE_UNITS[k].toLowerCase()) { limit *= 1000; k += 1; } }
    return { ">": value > limit, "<": value < limit, ">=": value >= limit, "<=": value <= limit, "=": value === limit, "==": value === limit, "!=": value !== limit }[m[1]];
  }
  return `${unit.name} ${unit.source}`.toLowerCase().includes(term);
}

function treemapLayout(canvas) {
  const { width, height } = canvas.getBoundingClientRect();
  const key = `${treemap.category}|${treemap.units.length}|${Math.round(width)}x${Math.round(height)}|${treemap.units.reduce((a, u) => a + u.code, 0)}`;
  if (key === treemap.layoutKey) return;
  const total = treemap.units.reduce((a, u) => a + u.code, 0) || 1, area = width * height;
  // Name order (as decomp.dev does) keeps related units together, so subsystems read as regions.
  const items = treemap.units.filter((u) => u.code > 0).sort((a, b) => a.name.localeCompare(b.name)).map((u) => ({ ...u, area: (u.code / total) * area }));
  const laid = squarify(items, 0, 0, width, height);
  const byName = new Map(laid.map((r) => [r.name, r]));
  treemap.units.forEach((u) => { const r = byName.get(u.name); if (r) Object.assign(u, { x: r.x / width, y: r.y / height, w: r.w / width, h: r.h / height }); });
  treemap.layoutKey = key; treemap.dirty = true;
}

function treemapDraw() {
  const canvas = $("#treemap"); if (!canvas || !canvas.getContext) return;
  treemapLayout(canvas);
  const { width, height } = canvas.getBoundingClientRect(), ratio = window.devicePixelRatio || 1;
  const pw = Math.round(width * ratio), ph = Math.round(height * ratio);
  if (canvas.width !== pw || canvas.height !== ph) { canvas.width = pw; canvas.height = ph; treemap.dirty = true; }
  if (!treemap.cache) treemap.cache = document.createElement("canvas");
  const cache = treemap.cache;
  if (treemap.dirty || cache.width !== pw || cache.height !== ph) {
    cache.width = pw; cache.height = ph;
    const c = cache.getContext("2d"); c.setTransform(ratio, 0, 0, ratio, 0, 0);
    c.fillStyle = "#181c25"; c.fillRect(0, 0, width, height); c.lineWidth = 1; c.strokeStyle = "#000";
    const terms = treemap.filter.toLowerCase().split(/\s+/).filter(Boolean);
    for (const u of treemap.units) {
      if (u.w === undefined) continue;
      u.filtered = terms.length > 0 && !terms.every((t) => treemapFilterMatch(t, u));
      const x = u.x * width, y = u.y * height, w = u.w * width, h = u.h * height;
      const [inner, outer] = u.fuzzy >= 100
        ? ["hsl(120 100% 39%)", "hsl(120 100% 17%)"]
        : [`color-mix(in srgb, hsl(200 0% 21%), hsl(200 100% 35%) ${u.fuzzy}%)`, `color-mix(in srgb, hsl(200 0% 15%), hsl(200 100% 15%) ${u.fuzzy}%)`];
      const g = c.createRadialGradient(x + 0.4 * w, y + 0.4 * h, (w + h) * 0.1, x + 0.4 * w, y + 0.4 * h, (w + h) * 0.5);
      g.addColorStop(0, inner); g.addColorStop(1, outer);
      c.fillStyle = g; c.beginPath(); c.rect(x, y, w, h);
      c.save(); if (u.filtered) c.clip(); c.stroke(); c.restore();
      if (u.filtered) c.globalAlpha = 0.1; c.fill(); c.globalAlpha = 1;
    }
    treemap.dirty = false;
  }
  const ctx = canvas.getContext("2d"); ctx.setTransform(1, 0, 0, 1, 0, 0); ctx.clearRect(0, 0, pw, ph); ctx.drawImage(cache, 0, 0);
  ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
  const u = treemap.hover; if (!u || u.w === undefined) return;
  const x = u.x * width, y = u.y * height, w = u.w * width, h = u.h * height;
  ctx.lineWidth = 2; ctx.strokeStyle = "#fff"; ctx.strokeRect(x, y, w, h);
  ctx.font = "600 13px Inter, ui-sans-serif, system-ui, sans-serif"; ctx.textBaseline = "middle";
  let text = `${u.name.replace(/^main\//, "")} • ${formatBytes(u.code)} • ${treemapPercent(u.fuzzy)}`;
  while (ctx.measureText(text).width + 20 > width && text.length > 8) text = `${text.slice(0, text.length / 2 - 2)}…${text.slice(text.length / 2 + 1)}`;
  const tw = ctx.measureText(text).width + 20, th = 26;
  let tx = Math.min(Math.max(x + (w - tw) / 2, 0), width - tw), ty = y - th - 6, anchor = y;
  if (ty < 0) { ty = y + h + 6; anchor = y + h; } if (ty + th > height) { ty = y + 5; anchor = y; }
  ctx.fillStyle = "#fff"; ctx.beginPath(); ctx.roundRect(tx, ty, tw, th, 5);
  const ax = x + w / 2;
  if (anchor <= ty) { ctx.moveTo(ax, anchor); ctx.lineTo(ax + 5, ty); ctx.lineTo(ax - 5, ty); } else { ctx.moveTo(ax, anchor); ctx.lineTo(ax + 5, ty + th); ctx.lineTo(ax - 5, ty + th); }
  ctx.fill(); ctx.fillStyle = "#101418"; ctx.fillText(text, tx + 10, ty + th / 2);
}

function treemapHit(canvas, clientX, clientY) {
  const { width, height, left, top } = canvas.getBoundingClientRect(), px = clientX - left, py = clientY - top;
  for (const u of treemap.units) {
    if (u.filtered || u.w === undefined) continue;
    const x = u.x * width, y = u.y * height;
    if (px >= x && px <= x + u.w * width && py >= y && py <= y + u.h * height) return u;
  }
  return null;
}

function treemapSetup() {
  if (treemap.ready) return; treemap.ready = true;
  const canvas = $("#treemap"), filter = $("#treemap-filter");
  const redraw = () => requestAnimationFrame(treemapDraw);
  new ResizeObserver(redraw).observe(canvas);
  canvas.addEventListener("mousemove", (e) => { const u = treemapHit(canvas, e.clientX, e.clientY); if (u !== treemap.hover) { treemap.hover = u; canvas.style.cursor = u ? "pointer" : "default"; redraw(); } });
  canvas.addEventListener("mouseleave", () => { treemap.hover = null; redraw(); });
  canvas.addEventListener("click", (e) => {
    const u = treemapHit(canvas, e.clientX, e.clientY); if (!u) return;
    $("#treemap-detail").textContent = `${u.name} · ${u.source} · ${treemapPercent(u.fuzzy)} · ${u.matched}/${u.total} functions exact · ${formatBytes(u.code)} · ${u.complete ? "linked" : "not linked"}`;
  });
  try { filter.value = localStorage.getItem("treemap-filter") || ""; treemap.category = localStorage.getItem("treemap-category") || "all"; } catch (_) { /* storage unavailable */ }
  treemap.filter = filter.value;
  filter.addEventListener("input", () => { treemap.filter = filter.value; treemap.dirty = true; try { localStorage.setItem("treemap-filter", filter.value); } catch (_) { /* ignore */ } redraw(); });
}

function renderMap(data) {
  treemapSetup();
  const names = Object.fromEntries((data.categories || []).map((item) => [item.id, item.name]));
  const maps = data.maps || {};
  const tabs = $("#treemap-tabs");
  const ids = ["all", ...Object.keys(maps)];
  if (!ids.includes(treemap.category)) treemap.category = "all";
  if (tabs.dataset.ids !== ids.join(",") || tabs.dataset.active !== treemap.category) {
    tabs.replaceChildren(); tabs.dataset.ids = ids.join(","); tabs.dataset.active = treemap.category;
    for (const id of ids) {
      const units = id === "all" ? Object.values(maps).flat() : maps[id];
      const code = units.reduce((a, u) => a + u.code, 0), done = units.reduce((a, u) => a + u.code * (u.fuzzy >= 100 ? 1 : 0), 0);
      const button = el("button", `treemap-tab${id === treemap.category ? " active" : ""}`, `${id === "all" ? "All code" : names[id] || id} · ${code ? ((100 * done) / code).toFixed(1) : "0.0"}%`);
      button.setAttribute("role", "tab"); button.type = "button";
      button.addEventListener("click", () => { treemap.category = id; tabs.dataset.active = ""; treemap.layoutKey = ""; try { localStorage.setItem("treemap-category", id); } catch (_) { /* ignore */ } renderMap(data); });
      tabs.append(button);
    }
  }
  const units = treemap.category === "all" ? Object.values(maps).flat() : maps[treemap.category] || [];
  const previous = new Map(treemap.units.map((u) => [u.name, u]));
  const changed = units.length !== treemap.units.length || units.some((u) => { const p = previous.get(u.name); return !p || p.fuzzy !== u.fuzzy || p.complete !== u.complete || p.code !== u.code; });
  if (changed) {
    treemap.units = units.map((u) => ({ ...u, ...(previous.get(u.name) ? { x: previous.get(u.name).x, y: previous.get(u.name).y, w: previous.get(u.name).w, h: previous.get(u.name).h } : {}) }));
    treemap.hover = null; treemap.dirty = true;
  }
  requestAnimationFrame(treemapDraw);
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

function renderLocalPriority(data) {
  const root = $("#local-priority"); if (!root) return; root.replaceChildren();
  const spec = data.queue.local_priority || {rows: [], promoted: []};
  $("#local-priority-why").textContent = spec.why ? `${spec.why}${spec.at ? ` · set ${new Date(spec.at).toLocaleTimeString()}` : ""}` : "No explicit priority list; workers follow the boot frontier, then value order.";
  const tally = {};
  spec.rows.forEach((row) => { const key = row.promoted_pct ? "promoted" : (row.status || "pending"); tally[key] = (tally[key] || 0) + 1; });
  if (spec.rows.length) root.append(el("p", "muted", `${spec.rows.length} targets · ${Object.entries(tally).sort((a, b) => b[1] - a[1]).map(([k, v]) => `${v} ${statusLabel(k)}`).join(" · ")}`));
  const shown = 15;
  let more = null;
  if (spec.rows.length > shown) {
    more = document.createElement("details"); more.className = "priority-more";
    more.open = root.dataset.moreOpen === "1";
    more.addEventListener("toggle", () => { root.dataset.moreOpen = more.open ? "1" : "0"; });
    more.append(el("summary", "", `Show the other ${spec.rows.length - shown} targets`));
  }
  spec.rows.forEach((row, index) => {
    const item = el("article", `priority-row ${row.status || ""}`);
    const gain = row.best_pct !== null && row.best_pct !== undefined && row.base_pct !== null && row.best_pct > row.base_pct
      ? ` → best ${percent(row.best_pct)}` : "";
    item.append(
      el("strong", "", `${index + 1}. ${row.symbol}`),
      el("span", "", `${statusLabel(row.status)}${row.worker ? ` · ${row.worker}` : ""} · ${row.attempts || 0} attempts`),
      el("code", "", `${row.base_pct === null || row.base_pct === undefined ? "n/a" : percent(row.base_pct)}${gain}`),
    );
    (index < shown || !more ? root : more).append(item);
  });
  if (more) root.append(more);
  const promoted = spec.promoted || [];
  root.append(el("p", "muted", promoted.length
    ? `Promoted into source since the last sync: ${promoted.map((row) => `${row.symbol} (${percent(row.pct)})`).join(", ")}`
    : "Promotions from the local models are committed automatically every 30 minutes after policy, build and hash checks."));
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

// Every series is a percentage on one shared axis, so the 100% goal line means the same thing for
// all of them. Counts (functions accepted / matched) are converted with their totals; the legend
// keeps the raw counts.
const HISTORY_SERIES = [
  {key: "path", label: "Title-screen path: functions accepted", color: "#1a7f37",
   value: (p, last) => p.path_accepted != null && (p.path_functions || last.path_functions) ? 100 * p.path_accepted / (p.path_functions || last.path_functions) : null,
   detail: (p) => `${p.path_accepted} of ${p.path_functions} functions`},
  {key: "fuzzy", label: "Whole game: fuzzy match", color: "#8250df",
   value: (p) => typeof p.fuzzy_match_percent === "number" && p.fuzzy_match_percent > 0 ? p.fuzzy_match_percent : null,
   detail: () => "byte-weighted similarity to retail"},
  {key: "matched", label: "Whole game: functions matched exactly", color: "#0969da",
   value: (p, last) => p.matched_functions && (p.total_functions || last.total_functions) ? 100 * p.matched_functions / (p.total_functions || last.total_functions) : null,
   detail: (p, last) => `${nf.format(p.matched_functions)} of ${nf.format(p.total_functions || last.total_functions)} functions`},
  {key: "complete", label: "Whole game: code in fully linked units", color: "#bf8700",
   value: (p) => typeof p.complete_code_percent === "number" && p.complete_code_percent > 0 ? p.complete_code_percent : null,
   detail: () => "share of code bytes"},
];

function drawHistory(data) {
  const canvas = $("#history"), context = canvas.getContext("2d");
  const points = (data.snapshots || []).filter((point) => point.at).map((point) => ({...point, t: Date.parse(point.at)}))
    .sort((a, b) => a.t - b.t);
  const width = canvas.clientWidth, height = canvas.clientHeight, ratio = window.devicePixelRatio || 1;
  canvas.width = width * ratio; canvas.height = height * ratio; context.scale(ratio, ratio); context.clearRect(0, 0, width, height);
  if (!points.length) { $("#history-legend").textContent = "No snapshots yet."; return; }
  const last = points[points.length - 1];
  const series = HISTORY_SERIES.map((s) => ({...s, values: points.map((p) => ({t: p.t, v: s.value(p, last), p})).filter((row) => row.v != null)}))
    .filter((s) => s.values.length);
  // Axis: from a round floor below the lowest value up to exactly 100%.
  const lowest = Math.min(...series.flatMap((s) => s.values.map((row) => row.v)));
  const floor = Math.max(0, Math.floor((lowest - 2) / 10) * 10), ceiling = 100;
  const left = 44, right = 150, top = 14, bottom = 22, plotW = Math.max(width - left - right, 10), plotH = height - top - bottom;
  const x = (t) => left + (points.length === 1 ? plotW / 2 : ((t - points[0].t) / Math.max(last.t - points[0].t, 1)) * plotW);
  const y = (v) => top + (1 - (v - floor) / (ceiling - floor)) * plotH;
  context.font = "11px -apple-system, BlinkMacSystemFont, sans-serif"; context.textBaseline = "middle";
  // Gridlines with percentage labels.
  const step = ceiling - floor > 50 ? 20 : 10;
  for (let v = floor; v < ceiling; v += step) {
    context.strokeStyle = "#d8dee4"; context.lineWidth = 1; context.setLineDash([]);
    context.beginPath(); context.moveTo(left, y(v)); context.lineTo(left + plotW, y(v)); context.stroke();
    context.fillStyle = "#57606a"; context.textAlign = "right"; context.fillText(`${v}%`, left - 6, y(v));
  }
  // The 100% goal line: bold, dashed, labelled.
  context.strokeStyle = "#1a7f37"; context.lineWidth = 2; context.setLineDash([6, 4]);
  context.beginPath(); context.moveTo(left, y(100)); context.lineTo(left + plotW, y(100)); context.stroke(); context.setLineDash([]);
  context.fillStyle = "#1a7f37"; context.font = "bold 11px -apple-system, BlinkMacSystemFont, sans-serif";
  context.textAlign = "right"; context.fillText("100%", left - 6, y(100));
  context.textAlign = "left"; context.fillText("100% = fully matched", left + 6, y(100) - 9);
  context.font = "11px -apple-system, BlinkMacSystemFont, sans-serif";
  // Time axis: first and last point, and the midpoint.
  context.fillStyle = "#57606a"; context.textBaseline = "top";
  const stamp = (t) => new Date(t).toLocaleString([], {month: "short", day: "numeric", hour: "2-digit", minute: "2-digit"});
  [[points[0].t, "left"], [(points[0].t + last.t) / 2, "center"], [last.t, "right"]].forEach(([t, align]) => {
    if (points.length > 1 || align === "center") { context.textAlign = align; context.fillText(stamp(t), x(t), top + plotH + 6); }
  });
  context.textBaseline = "middle";
  // Lines, each ending in a dot and its current value.
  const endLabels = [];
  series.forEach((s) => {
    context.strokeStyle = s.color; context.lineWidth = 2.5; context.beginPath();
    s.values.forEach((row, index) => { index ? context.lineTo(x(row.t), y(row.v)) : context.moveTo(x(row.t), y(row.v)); });
    context.stroke();
    const end = s.values[s.values.length - 1];
    context.fillStyle = s.color; context.beginPath(); context.arc(x(end.t), y(end.v), 3.5, 0, Math.PI * 2); context.fill();
    endLabels.push({y: y(end.v), text: `${end.v.toFixed(end.v >= 99.95 ? 0 : 1)}%`, color: s.color});
  });
  // Keep end labels from overlapping.
  endLabels.sort((a, b) => a.y - b.y);
  for (let i = 1; i < endLabels.length; i++) endLabels[i].y = Math.max(endLabels[i].y, endLabels[i - 1].y + 13);
  context.textAlign = "left"; context.font = "bold 11px -apple-system, BlinkMacSystemFont, sans-serif";
  endLabels.forEach((label) => { context.fillStyle = label.color; context.fillText(label.text, left + plotW + 8, label.y); });
  // Legend: what each line is, its current value, and its change over the window.
  $("#history-legend").innerHTML = series.map((s) => {
    const first = s.values[0], end = s.values[s.values.length - 1], delta = end.v - first.v;
    const since = new Date(first.t).toLocaleString([], {month: "short", day: "numeric", hour: "2-digit", minute: "2-digit"});
    return `<span class="history-key"><i class="tile" style="background:${s.color}"></i><span><b>${s.label}</b>: ` +
      `${end.v.toFixed(2)}% <span class="muted">(${s.detail(end.p, last)}; ${delta >= 0 ? "+" : ""}${delta.toFixed(2)} pts since ${since})</span></span></span>`;
  }).join("") + `<span class="muted">Dashed green line: 100% = fully matched. Last point ${new Date(last.t).toLocaleString()}.</span>`;
}

async function refresh() {
  if (refresh.inFlight) return;
  refresh.inFlight = true;
  const controller = new AbortController();
  const timeout = setTimeout(() => controller.abort(), 30000);
  try {
    const response = await fetch("/api/dashboard", {cache: "no-store", signal: controller.signal});
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    const data = await response.json();
    const workerCount = Object.keys(data.workers || {}).length;
    $("#model").textContent = workerCount ? `${nf.format(workerCount)} model worker${workerCount === 1 ? "" : "s"} registered` : `${data.settings.ollama_model || "model"} via ${data.settings.ollama_host || "local"}`;
    renderFreshness(data); renderRecomp(data); renderLive(data); renderWorkers(data); renderMetrics(data); renderQueue(data); renderLocalPriority(data); renderHighValue(data); renderMap(data); renderReview(data); renderEvents(data); drawHistory(data);
  } catch (error) {
    $("#model").textContent = `Dashboard unavailable: ${error.message}`;
    $("#freshness-polled").textContent = `Refresh failed ${new Date().toLocaleString()}; retrying automatically`;
  } finally { clearTimeout(timeout); refresh.inFlight = false; }
}
window.addEventListener("resize", refresh); refresh(); setInterval(refresh, 2000);
