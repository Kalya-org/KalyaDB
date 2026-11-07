# KalyaDB Architecture

**Status: proposal — not implemented.**

This document records a starting design, not a description of existing functionality. The architecture may change as requirements and prototypes are tested.

## Intended shape

KalyaDB is intended to be a modular, local-first database engine that can eventually be embedded in the Kalya desktop application.

Conceptual components:

1. **Public API** — the interface applications use to open a database and perform operations.
2. **Query layer** — parses requests, validates them, and plans execution.
3. **Execution layer** — performs reads and writes against storage.
4. **Storage engine** — manages pages or records and durable on-disk representation.
5. **Schema/catalog manager** — tracks databases, collections/tables, fields, and metadata.
6. **Index manager** — maintains indexes used to accelerate lookups.
7. **Transaction and recovery layer** — defines atomicity, consistency, isolation, durability, and crash recovery.
8. **Backup and integrity tools** — supports verification, export, backup, and restore.

## Initial design priorities

- Define the on-disk format before promising compatibility.
- Make writes recoverable after interruption.
- Specify transaction boundaries and concurrency behaviour.
- Use versioned metadata and plan for format migrations.
- Keep diagnostics useful without exposing stored secrets.
- Test corruption handling and recovery using disposable test databases.

## Decisions still open

The implementation language, query language, storage layout, page size, indexing strategy, concurrency model, and exact API have not been selected. These decisions should follow small benchmarks and prototypes rather than assumptions.

## Integration boundary

The first intended integration is local use by Kalya desktop. GitHub Pages hosts documentation only; it does not run or host the database engine. The engine itself will run within an application or another supported local runtime once implemented.
