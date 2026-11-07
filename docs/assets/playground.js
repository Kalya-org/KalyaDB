const editor = document.querySelector("#query");
const runButton = document.querySelector("#run");
const clearButton = document.querySelector("#clear");
const exampleButton = document.querySelector("#example");
const resetButton = document.querySelector("#reset");
const status = document.querySelector("#status");
const output = document.querySelector("#output");

let engine = null;

const exampleSql = `CREATE TABLE users;
INSERT INTO users VALUES ('Ada');
INSERT INTO users VALUES ('Grace Hopper');
SELECT * FROM users;`;

function setStatus(message, kind = "") {
  status.textContent = message;
  status.className = `sql-status ${kind}`.trim();
}

function renderRows(rows) {
  output.replaceChildren();
  if (!rows || rows.length === 0) {
    output.textContent = "No rows returned.";
    return;
  }

  const table = document.createElement("table");
  const head = document.createElement("thead");
  const headRow = document.createElement("tr");
  const heading = document.createElement("th");
  heading.textContent = "value";
  headRow.append(heading);
  head.append(headRow);

  const body = document.createElement("tbody");
  for (const value of rows) {
    const row = document.createElement("tr");
    const cell = document.createElement("td");
    cell.textContent = value;
    row.append(cell);
    body.append(row);
  }

  table.append(head, body);
  output.append(table);
}

function runSql() {
  if (!engine) {
    setStatus("The WebAssembly engine is not ready yet.", "error");
    return;
  }

  const sql = editor.value.trim();
  if (!sql) {
    setStatus("Enter a SQL statement first.", "error");
    return;
  }

  try {
    const result = engine.executeSql(sql);
    setStatus(result.message, result.ok ? "success" : "error");
    if (result.ok) renderRows(result.rows);
    else output.textContent = "The statement could not be executed.";
  } catch (error) {
    setStatus(error.message || "Unexpected engine error.", "error");
    output.textContent = "The statement could not be executed.";
  }
}

runButton.addEventListener("click", runSql);
clearButton.addEventListener("click", () => {
  editor.value = "";
  editor.focus();
  setStatus("Editor cleared.");
});
exampleButton.addEventListener("click", () => {
  editor.value = exampleSql;
  editor.focus();
  setStatus("Example loaded. Run each statement separately, from top to bottom.");
});
resetButton.addEventListener("click", () => {
  if (!engine) return;
  engine.resetDatabase();
  renderRows([]);
  setStatus("Temporary database reset.", "success");
});

editor.addEventListener("keydown", (event) => {
  if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
    event.preventDefault();
    runSql();
  }
});

try {
  const { default: createKalyaDB } = await import("./kalyadb.js");
  engine = await createKalyaDB();
  runButton.disabled = false;
  runButton.textContent = "Run SQL";
  setStatus("KalyaDB engine ready. Run one statement at a time.", "success");
} catch (error) {
  runButton.textContent = "Engine unavailable";
  setStatus("Could not load the WebAssembly engine. The build may still be running; refresh this page after GitHub Actions finishes.", "error");
  output.textContent = error.message || "WebAssembly module could not be loaded.";
}
