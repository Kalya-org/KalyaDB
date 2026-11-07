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

// Split a script on semicolons without splitting semicolons inside quoted text.
function splitStatements(script) {
  const statements = [];
  let current = "";
  let inString = false;

  for (let i = 0; i < script.length; i += 1) {
    const char = script[i];
    current += char;

    if (char === "'") {
      if (inString && script[i + 1] === "'") {
        current += script[i + 1];
        i += 1;
      } else {
        inString = !inString;
      }
    } else if (char === ";" && !inString) {
      const statement = current.slice(0, -1).trim();
      if (statement) statements.push(statement);
      current = "";
    }
  }

  if (inString) throw new Error("Unterminated quoted text in SQL script.");
  if (current.trim()) statements.push(current.trim());
  return statements;
}

function runSql() {
  if (!engine) {
    setStatus("The WebAssembly engine is not ready yet.", "error");
    return;
  }

  try {
    const statements = splitStatements(editor.value);
    if (statements.length === 0) {
      setStatus("Enter a SQL statement first.", "error");
      return;
    }

    let lastRows = [];
    let selectedRows = false;
    for (let i = 0; i < statements.length; i += 1) {
      const result = engine.executeSql(statements[i]);
      if (!result.ok) {
        setStatus(`Statement ${i + 1} failed: ${result.message}`, "error");
        output.textContent = "Execution stopped at the first error. Earlier statements in this script may already have completed.";
        return;
      }
      if (/^\\s*SELECT\\b/i.test(statements[i])) {
        lastRows = result.rows;
        selectedRows = true;
      }
    }

    if (selectedRows) renderRows(lastRows);
    else output.textContent = "Statements completed. Run a SELECT statement to display rows.";
    setStatus(`${statements.length} statement(s) executed successfully.`, "success");
  } catch (error) {
    setStatus(error.message || "Unexpected engine error.", "error");
    output.textContent = "The SQL script could not be executed.";
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
  setStatus("Example loaded. Press Run SQL to execute all four statements.");
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
  setStatus("KalyaDB engine ready. You can run a multi-statement SQL script.", "success");
} catch (error) {
  runButton.textContent = "Engine unavailable";
  setStatus("Could not load the WebAssembly engine. The build may still be running; refresh this page after GitHub Actions finishes.", "error");
  output.textContent = error.message || "WebAssembly module could not be loaded.";
}
