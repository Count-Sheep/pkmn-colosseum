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
    $("#model").textContent = `${data.settings.ollama_model || "model"} via ${data.settings.ollama_host || "local"}`;
    renderMetrics(data); renderQueue(data); renderMap(data); renderReview(data); renderEvents(data); drawHistory(data);
  } catch (error) { $("#model").textContent = `Dashboard unavailable: ${error.message}`; }
}
window.addEventListener("resize", refresh); refresh(); setInterval(refresh, 10000);
