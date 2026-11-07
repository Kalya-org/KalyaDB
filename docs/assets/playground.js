// This page is a text scratchpad only. It does not connect to or execute KalyaDB.
const editor = document.querySelector("#query");
const clearButton = document.querySelector("#clear");
const message = document.querySelector("#message");
clearButton.addEventListener("click", () => {
  editor.value = "";
  message.textContent = "Scratchpad cleared. No database operation was performed.";
  editor.focus();
});
editor.addEventListener("input", () => {
  message.textContent = editor.value.trim()
    ? "Draft only — KalyaDB does not execute or save this text."
    : "Nothing has been run. This page does not execute database commands.";
});
