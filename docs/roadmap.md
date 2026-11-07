# KalyaDB Roadmap

**Status: planning.** These are proposed milestones, not delivery promises.

## 0 — Project foundation

- [x] Create the repository and baseline documentation.
- [x] Add a project-specific ignore file and license.
- [x] Publish the initial documentation source in `docs/`.
- [ ] Agree on project requirements and non-goals.
- [ ] Choose the implementation language and supported platforms.

## 1 — Architecture and prototype

- [ ] Specify the public API and data model.
- [ ] Choose and document the on-disk format.
- [ ] Build a disposable storage prototype.
- [ ] Add automated tests for basic persistence and restart behaviour.
- [ ] Record benchmark methodology and baseline results.

## 2 — Core engine

- [ ] Implement database creation and opening.
- [ ] Implement basic read and write operations.
- [ ] Add schema/catalog management.
- [ ] Define and implement transaction behaviour.
- [ ] Add integrity checks and recovery tests.

## 3 — Querying and indexes

- [ ] Select a query syntax.
- [ ] Implement parsing and validation.
- [ ] Add query execution.
- [ ] Add indexes and measure their performance.

## 4 — Kalya integration

- [ ] Define the Electron integration boundary.
- [ ] Add a safe application-facing API.
- [ ] Test concurrent access, shutdown, and crash recovery.
- [ ] Document backup, restore, and migration workflows.

## 5 — Release readiness

- [ ] Complete security and data-integrity reviews.
- [ ] Publish format and API documentation.
- [ ] Establish compatibility and migration policies.
- [ ] Create repeatable release builds and test suites.

A milestone should only be marked complete when its implementation, tests, and documentation are present.
