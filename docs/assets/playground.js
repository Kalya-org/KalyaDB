const editor = document.querySelector("#query");
const runButton = document.querySelector("#run");
const clearButton = document.querySelector("#clear");
const exampleButton = document.querySelector("#example");
const resetButton = document.querySelector("#reset");
const status = document.querySelector("#status");
const output = document.querySelector("#output");

const DATABASE_PATH = "/kalya-playground.kdb";
const IDB_NAME = "KalyaDBPlayground";
const IDB_STORE = "databases";
const IDB_KEY = "relational-v2";
let engine = null;
let persistenceReady = false;

const exampleSql = "CREATE TABLE users (name TEXT, age INTEGER);\nINSERT INTO users VALUES ('Ada', 25);\nINSERT INTO users VALUES ('Grace Hopper', 85);\nSELECT * FROM users;";

function setStatus(message, kind = "") {
  status.textContent = message;
  status.className = ("sql-status " + kind).trim();
}

function openDatabase() {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open(IDB_NAME, 1);
    request.onupgradeneeded = () => {
      if (!request.result.objectStoreNames.contains(IDB_STORE)) {
        request.result.createObjectStore(IDB_STORE);
      }
    };
    request.onsuccess = () => resolve(request.result);
    request.onerror = () => reject(request.error || new Error("Could not open browser storage."));
  });
}

async function withStore(mode, operation) {
  const database = await openDatabase();
  try {
    return await new Promise((resolve, reject) => {
      const transaction = database.transaction(IDB_STORE, mode);
      const request = operation(transaction.objectStore(IDB_STORE));
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error || new Error("Browser storage operation failed."));
      transaction.onabort = () => reject(transaction.error || new Error("Browser storage transaction was aborted."));
    });
  } finally {
    database.close();
  }
}

async function restoreDatabase() {
  const savedBytes = await withStore("readonly", store => store.get(IDB_KEY));
  if (savedBytes) {
    engine.FS.writeFile(DATABASE_PATH, new Uint8Array(savedBytes));
    return true;
  }
  return false;
}

async function persistDatabase() {
  const bytes = engine.FS.readFile(DATABASE_PATH);
  await withStore("readwrite", store => store.put(bytes, IDB_KEY));
}

function renderRows(columns, rows) {
  output.replaceChildren();
  if (!columns || columns.length === 0) {
    output.textContent = "No query results. Run SELECT * FROM table_name to display rows.";
    return;
  }

  const table = document.createElement("table");
  const head = document.createElement("thead");
  const headRow = document.createElement("tr");
  for (const name of columns) {
    const heading = document.createElement("th");
    heading.textContent = name;
    headRow.append(heading);
  }
  head.append(headRow);

  const body = document.createElement("tbody");
  for (const values of rows) {
    const row = document.createElement("tr");
    for (const value of values) {
      const cell = document.createElement("td");
      cell.textContent = value;
      row.append(cell);
    }
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

async function runSql() {
  if (!engine) {
    setStatus("The WebAssembly engine is not ready yet.", "error");
    return;
  }

  runButton.disabled = true;
  try {
    const statements = splitStatements(editor.value);
    if (statements.length === 0) {
      setStatus("Enter a SQL statement first.", "error");
      return;
    }

    let lastColumns = [];
    let lastRows = [];
    let selectedRows = false;
    for (let i = 0; i < statements.length; i += 1) {
      const result = engine.executeSql(statements[i]);
      if (!result.ok) {
        setStatus("Statement " + (i + 1) + " failed: " + result.message, "error");
        output.textContent = "Execution stopped at the first error. Earlier statements in this script may already have completed.";
        return;
      }
      if (persistenceReady) {
        try {
          await persistDatabase();
        } catch (error) {
          persistenceReady = false;
          setStatus("SQL ran, but browser persistence failed: " + error.message + ". Data may be lost on reload.", "error");
        }
      }
      if (/^\s*SELECT\b/i.test(statements[i])) {
        lastColumns = result.columns;
        lastRows = result.rows;
        selectedRows = true;
      }
    }

    if (selectedRows) renderRows(lastColumns, lastRows);
    else output.textContent = "Statements completed. Run a SELECT statement to display rows.";
    if (persistenceReady) {
      setStatus(statements.length + " statement(s) executed and saved in this browser.", "success");
    } else {
      setStatus(statements.length + " statement(s) executed in temporary memory only.", "error");
    }
  } catch (error) {
    setStatus(error.message || "Unexpected engine error.", "error");
    output.textContent = "The SQL script could not be executed.";
  } finally {
    runButton.disabled = !engine;
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
resetButton.addEventListener("click", async () => {
  if (!engine) return;
  resetButton.disabled = true;
  try {
    engine.resetDatabase();
    if (persistenceReady) await persistDatabase();
    renderRows([], []);
    setStatus("Database reset. The empty database has been saved.", "success");
  } catch (error) {
    setStatus("Database reset, but saving failed: " + error.message, "error");
  } finally {
    resetButton.disabled = false;
  }
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
  try {
    const restored = await restoreDatabase();
    persistenceReady = true;
    setStatus(restored
      ? "KalyaDB 0.2.0 ready. Your saved relational database has been restored."
      : "KalyaDB 0.2.0 ready. Browser persistence is enabled; run SQL to create saved data.", "success");
  } catch (storageError) {
    persistenceReady = false;
    setStatus("KalyaDB ready, but browser storage is unavailable: " + storageError.message + ". Data will be temporary.", "error");
  }
  runButton.disabled = false;
  runButton.textContent = "Run SQL";
} catch (error) {
  runButton.textContent = "Engine unavailable";
  setStatus("Could not load the WebAssembly engine. The build may still be running; refresh this page after GitHub Actions finishes.", "error");
  output.textContent = error.message || "WebAssembly module could not be loaded.";
}
