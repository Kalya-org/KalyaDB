# KalyaDB Roadmap

This roadmap tracks the transition from an experimental native prototype into a relational database platform. A milestone is complete only when implementation, automated tests, and documentation are present.

## 0.1.x — Experimental foundation

- [x] Repository and documentation foundation.
- [x] C++ native storage prototype.
- [x] Basic named-table and SQL prototypes.
- [x] Native CMake/CTest workflow.
- [x] WebAssembly playground.
- [x] Browser-local persistence using IndexedDB.
- [ ] Replace prototype record encoding with a documented, versioned relational catalog and row format.

## 0.2.0 — Relational database core

- [ ] Named columns and table schemas.
- [ ] Multiple values per row.
- [ ] Initial data types: TEXT and INTEGER.
- [ ] Type and column-count validation.
- [ ] Structured query results with column names.
- [ ] Schema-aware persistence and a safe migration/reset strategy for older prototype files.
- [ ] Tests for reopen, validation failures, and persisted multi-column rows.
- [ ] Update the WebAssembly playground to demonstrate relational SQL.

## 0.3.x — Relational integrity

- [ ] Primary keys and unique constraints.
- [ ] Foreign keys and referential integrity.
- [ ] NOT NULL and NULL semantics.
- [ ] UPDATE and DELETE.
- [ ] Basic WHERE filtering and ordering.
- [ ] Joins across related tables.

## 0.4.x — Storage reliability

- [ ] Versioned page/record storage design.
- [ ] Transactions and rollback.
- [ ] Crash recovery and integrity checking.
- [ ] Indexes and query-planning foundations.
- [ ] Backup, restore, and migration tools.
- [ ] Concurrency and locking model.

## 0.5.x — Local database service

- [ ] Define a stable engine interface.
- [ ] Build a local service process around the engine.
- [ ] Add a documented HTTP API.
- [ ] Add connection management, request validation, and safe parameter handling.
- [ ] Add JavaScript/TypeScript client SDK.
- [ ] Integrate the service with the Kalya desktop application.

## 0.6.x — Platform services

- [ ] Authentication and service credentials.
- [ ] Roles, grants, and row/table-level authorization policies.
- [ ] API key management and secret rotation.
- [ ] Audit logs and service health/metrics.
- [ ] Realtime events for committed, authorized changes.

## 0.7.x — Dashboard and operations

- [ ] Project and database management.
- [ ] Table editor and relationship visualizer.
- [ ] SQL editor and query results.
- [ ] API explorer and SDK guides.
- [ ] User, role, and policy management.
- [ ] Backup, restore, logs, and monitoring views.

## 1.0.0 — Production-readiness review

- [ ] Independent security and data-integrity review.
- [ ] Documented durability, isolation, and recovery guarantees.
- [ ] Load, stress, and failure testing.
- [ ] Stable storage and API compatibility policies.
- [ ] Supported deployment and upgrade procedures.
- [ ] Complete documentation and release process.

The roadmap is a build sequence, not a promise that every feature exists today. Avoid using valuable or sensitive data until the 1.0.0 readiness requirements have been met.
